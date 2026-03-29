#ifndef MQTT_BROKER_H
#define MQTT_BROKER_H

#include <MqttClient.h>
#include <stdint.h>
#include "Command.h"
#include "LightstripStatus.h"

enum class HeartbeatStatus : uint8_t {
    Alive,
    Dead
};

enum class CommandValueType : uint8_t {
    None,
    Integer,
    Rgbw
};

struct CommandEvent {
    Command command;
    CommandValueType valueType;
    union {
        int16_t intValue;
        RgbwValue rgbwValue;
    };
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
    bool publishHeartbeat(HeartbeatStatus status = HeartbeatStatus::Alive);

private:
    String heartbeatPayload(HeartbeatStatus status) const {
        return status == HeartbeatStatus::Alive ? "alive" : "dead";
    }
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