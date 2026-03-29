#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoMqttClient.h>

#include "arduino_secrets.h"
#include "LightStrip.h"
#include "Command.h"
#include "Topics.h"

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

IPAddress myDns(192, 168, 86, 1);
IPAddress ip(192, 168, 86, 177);

EthernetClient client;
MqttClient mqttClient(client);

bool connectedToBroker = false;

// Variables to measure the speed
unsigned long beginMicros, endMicros;
unsigned long byteCount = 0;
bool printWebData = true;  // set to false for better speed measurement

LightStrip strip(70);  // 10 brightness instead of 50

const char broker[] = "192.168.86.78";
int        port     = 1883;
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

  connectToMqttBroker();
  strip.begin();
  // Initialize with a simple color test instead of pulseWhite
  //strip.setColor(255, 150, 0, 0);  // Orange color for testing
  publishInitialStatus();
}

void publishInitialStatus() {
 /* mqttClient.beginMessage(Topics::LightstripSwitchStatus);
  mqttClient.print("ON");
  mqttClient.endMessage();

  mqttClient.beginMessage(Topics::LightstripBrightnessStatus);
  mqttClient.print("50");
  mqttClient.endMessage();
  Serial.println("Published initial status");*/
}

void connectToMqttBroker() {
  if(!connectedToBroker)
  {
    // You can provide a unique client ID, if not set the library uses Arduino-millis()
    // Each client must have a unique client ID
    mqttClient.setId("lightstrip-livingroom");

    // You can provide a username and password for authentication
    mqttClient.setUsernamePassword(MQTT_USERNAME, MQTT_PASSWORD);

    String willPayload = "disconnected";
    bool willRetain = true;
    int willQos = 1;

    mqttClient.beginWill(Topics::LightstripHeartbeat, willPayload.length(), willRetain, willQos);
    mqttClient.print(willPayload);
    mqttClient.endWill();

    Serial.print("Attempting to connect to the MQTT broker: ");
    Serial.println(broker);

    if (!mqttClient.connect(broker, port)) {
      Serial.print("MQTT connection failed! Error code = ");
      Serial.println(mqttClient.connectError());
    }
    else
    {
      connectedToBroker = true;
    
      Serial.println("You're connected to the MQTT broker!");
      Serial.println();

      mqttClient.onMessage(onMqttMessage);

      for(Topic topic : TopicsAvailable::list) {
        Serial.println("Subscribing to topic: ");
        Serial.print(topic.name);
        Serial.println();
        mqttClient.subscribe(topic.name);
      }

    }
  }
}

bool publishHeartbeat() {
  mqttClient.beginMessage(Topics::LightstripHeartbeat);
  mqttClient.print("alive");
  mqttClient.endMessage();
  Serial.println("Published heartbeat");
  return true; // return true to repeat this timer
}

void loop() {
  mqttClient.poll();

  if(!mqttClient.connected())
  {
    Serial.println("Disconnected from MQTT broker");
    connectedToBroker = false;
    connectToMqttBroker();
  }
  //strip.setColor(0, 0, 0, 255);  // Orange color for testing
  strip.setKelvin(2700, 220);
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= heartbeatInterval && connectedToBroker) {
    // save the last time a message was sent
    previousMillis = currentMillis;
    publishHeartbeat();
  }
}
void onMqttMessage(int messageSize) {
  String topic = mqttClient.messageTopic();
  Serial.print("Message arrived on topic: "); 
  Serial.print(topic);

  // we received a message, print out the topic and contents
  char message[messageSize + 1];

  int i = 0;

  while(mqttClient.available() && i < messageSize) {
    message[i] = (char)mqttClient.read();
    i++;
  }
  message[i] = '\0'; // Null-terminate the string

  Serial.print("Received message: ");
  Serial.println(message);
  Serial.println();
  
  if(topic.indexOf("brightness")>=0)
  {
    int brightness = atoi(message);
    strip.setBrightness(brightness);
    mqttClient.beginMessage(Topics::LightstripBrightnessStatus);
    mqttClient.print(message);
    mqttClient.endMessage();

  }
  else
  {
    if(strcmp(message, "ON") == 0)
    {
      strip.setColor(255, 150, 0, 30);
      mqttClient.beginMessage(Topics::LightstripSwitchStatus);
      mqttClient.print("ON");
      mqttClient.endMessage();
    }
    else if(strcmp(message, "OFF") == 0)
    {
      strip.turnOff();
      mqttClient.beginMessage(Topics::LightstripSwitchStatus);
      mqttClient.print("OFF");
      mqttClient.endMessage();
    }
    else
    {
      Serial.println("Unknown command received");
    }
  }
  Serial.println();
}
