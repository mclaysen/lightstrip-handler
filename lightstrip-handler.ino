#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoMqttClient.h>

#include "arduino_secrets.h"
#include "LightStrip.h"
#include "Command.h"
#include "Topics.h"

#include "EventHandler.h"
#include "LightstripStatus.h"

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

IPAddress myDns(192, 168, 86, 1);
IPAddress ip(192, 168, 86, 177);

EthernetClient client;
MqttClient mqttClient(client);

LightStrip strip(70);

const char broker[] = "192.168.86.78";
uint16_t        port     = 1883;
EventHandlerConfig config = {broker, port, MQTT_USERNAME, MQTT_PASSWORD};
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
        delay(1); // do nothing, no point running without Ethernet hardware
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

  strip.begin();
  eventHandler.init();
  eventHandler.setCommandCallback(commandCallback);
  eventHandler.publishStatus({true, 50});
}

void commandCallback(const CommandEvent& event) {
  switch (event.command) {
    case Command::On:
      strip.setColor(255, 150, 0, 30);
      break;
    case Command::Off:
      strip.turnOff();
      break;
    case Command::ChangeBrightness:
      strip.setBrightness(event.value);
      break;
    case Command::ChangeKelvin:
      strip.setKelvin(event.value, 220);
      break;
    default:
      Serial.println("Unknown command received");
  }
}

void loop() {
  eventHandler.loop();

  if(!eventHandler.isConnected())
  {
    Serial.println("Disconnected from event handler");
    eventHandler.init();
  }
  strip.setKelvin(2700, 220);
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= heartbeatInterval && eventHandler.isConnected()) {
    previousMillis = currentMillis;
    eventHandler.publishHeartbeat();
  }
}
