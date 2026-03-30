#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoMqttClient.h>

#include "arduino_secrets.h"
#include "LightStrip.h"
#include "Command.h"
#include "Topics.h"

#include "EventHandler.h"
#include "LightstripStatus.h"

#include "src/effects/heartbeat/Heartbeat.h"
#include "src/effects/easter/easter.h"
#include "Effect.h"

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

IPAddress myDns(192, 168, 86, 1);
IPAddress ip(192, 168, 86, 177);

EthernetClient client;
MqttClient mqttClient(client);

LightStrip strip;

RgbwValue currentRgbw = { 100, 100, 100, 50 };

IEffect* currentEffect;

LightStripStatus currentStatus = { true, 50, currentRgbw, 0, CurrentColorMode::Rgbw, false, "none" };

const char broker[] = "192.168.86.78";
uint16_t port = 1883;
EventHandlerConfig config = { broker, port, MQTT_USERNAME, MQTT_PASSWORD };
EventHandler eventHandler(mqttClient, config);
const long heartbeatInterval = 10000;
unsigned long previousMillis = 0;


void setup() {
  //Initialize serial and wait for port to open:
  Ethernet.init(10);  // Most Arduino shields

  Serial.begin(9600);

  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true) {
        delay(1);  // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    // try to configure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, myDns);
  } else {
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }

  eventHandler.init();
  initializeStrip();
  eventHandler.setCommandCallback(commandCallback);
}

void initializeStrip() {
  strip.begin();
  strip.setBrightnessAndShow(currentStatus.brightness);
  if (currentStatus.isOn) {
    uint8_t r, g, b, w;
    r = currentStatus.rgbw.r;
    g = currentStatus.rgbw.g;
    b = currentStatus.rgbw.b;
    w = currentStatus.rgbw.w;
    strip.setColor(r, g, b, w);
  } else {
    strip.turnOff();
  }
  eventHandler.publishStatus(currentStatus);
  eventHandler.publishHeartbeat();
}

void setStripToCurrentStatus() {
  if (currentStatus.isOn) {
    if (currentStatus.colorMode == CurrentColorMode::Rgbw) {
      strip.setColor(currentStatus.rgbw.r, currentStatus.rgbw.g, currentStatus.rgbw.b, currentStatus.rgbw.w);
    } else if (currentStatus.colorMode == CurrentColorMode::Temperature) {
      strip.setKelvin(currentStatus.temperature, 220);
    }
  } else {
    strip.turnOff();
  }
}

void commandCallback(const CommandEvent& event) {
  switch (event.command) {
    case Command::On:
      if (currentStatus.isOn) break;

      currentStatus.isOn = true;
      setStripToCurrentStatus();
      break;
    case Command::Off:
      strip.turnOff();
      currentStatus.isOn = false;
      currentStatus.hasEffect = false;
      currentStatus.currentEffectName = "none";
      currentEffect = nullptr;
      break;
    case Command::ChangeBrightness:
      strip.setBrightnessAndShowevent.intValue);
      currentStatus.brightness = event.intValue;
      break;
    case Command::ChangeTemperature:
      strip.setKelvin(event.intValue, 220);
      currentStatus.temperature = event.intValue;
      currentStatus.colorMode = CurrentColorMode::Temperature;
      break;
    case Command::ChangeColor:
      strip.setColor(event.rgbwValue.r, event.rgbwValue.g, event.rgbwValue.b, event.rgbwValue.w);
      currentStatus.rgbw = event.rgbwValue;
      currentStatus.colorMode = CurrentColorMode::Rgbw;
      break;
    case Command::SetEffect:
      Serial.println("Setting effect:");
      Serial.println(event.stringValue);
      if (strcmp(event.stringValue, "heartbeat") == 0) {
        currentEffect = new HeartbeatEffect();
        currentEffect->begin(strip);
        currentStatus.hasEffect = true;
        currentStatus.currentEffectName = "heartbeat";
      } 
      else if (strcmp(event.stringValue, "easter") == 0) {
        currentEffect = new EasterEffect();
        currentEffect->begin(strip);
        currentStatus.hasEffect = true;
        currentStatus.currentEffectName = "easter";
      }
      else if(strcmp(event.stringValue, "none") == 0) {
        if(currentEffect) {
          currentEffect->stop(strip);
          delete currentEffect;
          currentEffect = nullptr;
        }
        currentStatus.hasEffect = false;
        currentStatus.currentEffectName = "none";
        setStripToCurrentStatus();
      }
      else {
        Serial.print("Unknown effect: ");
        Serial.println(event.stringValue);
        currentStatus.currentEffectName = "none";
        currentStatus.hasEffect = false;
      }
      break;
    default:
      Serial.println("Unknown command received");
  }
  eventHandler.publishStatus(currentStatus);
}

void loop() {
  eventHandler.loop();

  if (!eventHandler.isConnected()) {
    Serial.println("Disconnected from event handler");
    eventHandler.init();
  }
  unsigned long currentMillis = millis();

  if(currentStatus.hasEffect && currentEffect) {
    currentEffect->update(strip, currentMillis);
  }

  if (currentMillis - previousMillis >= heartbeatInterval && eventHandler.isConnected()) {
    previousMillis = currentMillis;
    eventHandler.publishHeartbeat();
  }
}
