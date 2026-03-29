#pragma once

#include <cstdint>

#include "Command.h"

enum class TopicType : uint8_t {
    Heartbeat,
    Command,
    Status,
    Unknown
};

struct Topics {
    static constexpr char LightstripSwitchStatus[] = "ic/livingroom/lightstrip/status";
    static constexpr char LightstripSwitchCommand[] = "ic/livingroom/lightstrip/switch";
    static constexpr char LightstripBrightnessCommand[] = "ic/livingroom/lightstrip/brightness/set";
    static constexpr char LightstripBrightnessStatus[] = "ic/livingroom/lightstrip/brightness/status";
    static constexpr char LightstripHeartbeat[] = "ic/livingroom/lightstrip/heartbeat";
};

struct Topic
{
    const char* name;
    TopicType type;
    bool shouldSubscribe() const {
        return type == TopicType::Command;
    }
};

struct TopicsAvailable {
    static constexpr Topic list[] = {
        {Topics::LightstripSwitchStatus, TopicType::Status},
        {Topics::LightstripSwitchCommand, TopicType::Command},
        {Topics::LightstripBrightnessCommand, TopicType::Command},
        {Topics::LightstripBrightnessStatus, TopicType::Status},
        {Topics::LightstripHeartbeat, TopicType::Heartbeat}
    };
};
