#ifndef MQTT_BROKER_H
#define MQTT_BROKER_H

#include <MqttClient.h>
#include <Ethernet.h>
#include "Command.h"
#include "LightstripStatus.h"



class EventHandler {
    public:
        typedef void (*MessageHandler)(const Command& topic, const char& payload);
    
    EventHandler(EthernetClient& client);

    bool publishStatus(const LightStripStatus& status);
    void publishHeartbeat();
    void setMessageHandler(MessageHandler handler);





