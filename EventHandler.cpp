#include "EventHandler.h"
#include "Topics.h"
#include <ArduinoMqttClient.h>
#include "Command.h"

#define MQTT_ID "lightstrip-livingroom"

EventHandler* EventHandler::instance_ = nullptr;

EventHandler::EventHandler(MqttClient& client, const EventHandlerConfig& config)
    : mqttClient_(client), config_(config), commandCallback_(nullptr) {
    instance_ = this;
}

void EventHandler::init() {
    connectToBrokerIfNeeded();
}

void EventHandler::loop() {
    mqttClient_.poll();
}

bool EventHandler::isConnected() const {
    return mqttClient_.connected();
}

void EventHandler::setCommandCallback(CommandCallback callback) {
    commandCallback_ = callback;
}

bool EventHandler::publishStatus(const LightStripStatus& status) {
    mqttClient_.beginMessage(Topics::LightstripSwitchStatus);
    mqttClient_.print(status.isOn ? "ON" : "OFF");
    mqttClient_.endMessage();

    mqttClient_.beginMessage(Topics::LightstripBrightnessStatus);
    mqttClient_.print(String(status.brightness));
    mqttClient_.endMessage();

    mqttClient_.beginMessage(Topics::LightstripRgbwStatus);
    mqttClient_.print(status.rgbw.toString());
    mqttClient_.endMessage();

    /*mqttClient_.beginMessage(Topics::LightstripTempStatus);
    mqttClient_.print(String(status.temperature));
    mqttClient_.endMessage();*/

    mqttClient_.beginMessage(Topics::LightstripEffectStatus);
    mqttClient_.print(status.currentEffectName);
    mqttClient_.endMessage();

    return true;
}

bool EventHandler::publishHeartbeat(HeartbeatStatus status) {
    mqttClient_.beginMessage(Topics::LightstripHeartbeat);
    mqttClient_.print(heartbeatPayload(status));
    mqttClient_.endMessage();
    Serial.println("Published heartbeat");
    return true; 
}

bool EventHandler::connectToBrokerIfNeeded() {
    if (!mqttClient_.connected()) {
        Serial.println("Connecting to MQTT broker...");
        mqttClient_.setId(MQTT_ID);

        mqttClient_.setUsernamePassword(config_.username, config_.password);

        String willPayload = heartbeatPayload(HeartbeatStatus::Dead);
        bool willRetain = true;
        int willQos = 1;

        mqttClient_.beginWill(Topics::LightstripHeartbeat, willPayload.length(), willRetain, willQos);
        mqttClient_.print(willPayload);
        mqttClient_.endWill();

        if (mqttClient_.connect(config_.brokerHost, config_.brokerPort)) {
            Serial.println("Connected to MQTT broker");
            mqttClient_.onMessage(onMqttMessageThunk);
            suscribeToTopics();
            return true;
        } else {
            Serial.print("Failed to connect to MQTT broker, state: ");
            Serial.println(mqttClient_.connectError());
            return false;
        }
    }
    return true;
}

void EventHandler::suscribeToTopics() {
    for (const Topic& topic : TopicsAvailable::list) {
        if (topic.shouldSubscribe()) {
            Serial.print("Subscribing to topic: ");
            Serial.println(topic.name);
            mqttClient_.subscribe(topic.name);
        }
    }
}

void EventHandler::onMqttMessage(int messageSize) {
    String topic = mqttClient_.messageTopic();
    Serial.print("Message arrived on topic: ");
    Serial.println(topic);

    char message[messageSize + 1];
    int i = 0;
    while (mqttClient_.available() && i < messageSize) {
        message[i] = (char)mqttClient_.read();
        i++;
    }
    message[i] = '\0'; // Null-terminate the string

    Serial.print("Received message: ");
    Serial.println(message);
    Serial.println();

    CommandEvent event = parseMessage(topic.c_str(), message);
    if (commandCallback_) {
        commandCallback_(event);
    }
}

CommandEvent EventHandler::parseMessage(const char* topic, const char* payload) const {
    CommandEvent event;
    event.valueType = CommandValueType::None;

    if (strcmp(topic, Topics::LightstripSwitchCommand) == 0) {
        if (strcmp(payload, "ON") == 0) {
            event.command = Command::On;
            event.valueType = CommandValueType::None;
        } else if (strcmp(payload, "OFF") == 0) {
            event.command = Command::Off;
            event.valueType = CommandValueType::None;
        } else {
            event.command = Command::Unknown;
            event.valueType = CommandValueType::None;
        }
    } 
    else if (strcmp(topic, Topics::LightstripBrightnessCommand) == 0) {
        event.command = Command::ChangeBrightness;
        event.valueType = CommandValueType::Integer;
        event.intValue = atoi(payload);
    } 
    else if(strcasecmp(topic, Topics::LightstripRgbwSet) == 0) 
    {
        event.command = Command::ChangeColor;
        event.valueType = CommandValueType::Rgbw;
        sscanf(payload, "%hhu,%hhu,%hhu,%hhu", &event.rgbwValue.r, &event.rgbwValue.g, &event.rgbwValue.b, &event.rgbwValue.w);
    } 
    else if(strcasecmp(topic, Topics::LightstripTempSet) == 0) 
    {
        event.command = Command::ChangeTemperature;
        event.valueType = CommandValueType::Integer;
        event.intValue = atoi(payload);
    }
    else if(strcasecmp(topic, Topics::LightstripEffectSet) == 0)
    {
        event.command = Command::SetEffect;
        event.valueType = CommandValueType::String;
        strncpy(event.stringValue, payload, sizeof(event.stringValue) - 1);
        event.stringValue[sizeof(event.stringValue) - 1] = '\0';
    }
    else {
        event.command = Command::Unknown;
        event.valueType = CommandValueType::None;
    }
    return event;
}

