#ifndef MQTT_BROKER_H
#define MQTT_BROKER_H

#include <MqttClient.h>
#include <stdint.h>
#include "Command.h"
#include "LightstripStatus.h"

struct CommandEvent {
    Command command;
    int16_t value;      // brightness, kelvin, etc.
    bool hasValue;
};

struct EventHandlerConfig {
    const char* brokerHost;
    uint16_t brokerPort;
    const char* username;
    const char* password;
};

class EventHandler {
public:
    typedef void (*CommandCallback)(const CommandEvent& event);
    
    EventHandler(MqttClient& client, const EventHandlerConfig& config);

    void init();
    void loop();
    bool isConnected() const;

    void setCommandCallback(CommandCallback callback);

    bool publishStatus(const LightStripStatus& status);
    bool publishHeartbeat(const char* value = "alive");

private:
    bool connectToBrokerIfNeeded();
    void suscribeToTopics();
    void onMqttMessage(int messageSize);
    CommandEvent parseMessage(const char* topic, const char* payload) const;

    static EventHandler* instance_; // for static callback access
    static void onMqttMessageThunk(int messageSize) {
        if (instance_) {
            instance_->onMqttMessage(messageSize);
        }
    }

    MqttClient& mqttClient_;
    EventHandlerConfig config_;
    CommandCallback commandCallback_;
};

#endif