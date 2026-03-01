#include <ArduinoMqttClient.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Adafruit_NeoPixel.h>
#include <arduino-timer.h>

#include "arduino_secrets.h"

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

IPAddress myDns(192, 168, 86, 1);
IPAddress ip(192, 168, 86, 177);

EthernetClient client;
MqttClient mqttClient(client);

#define LED_PIN     13
#define LED_COUNT  60
#define BRIGHTNESS 20 // Set BRIGHTNESS to about 1/5 (max = 255)

bool colorSet = false;

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRBW + NEO_KHZ800);

// Variables to measure the speed
unsigned long beginMicros, endMicros;
unsigned long byteCount = 0;
bool printWebData = true;  // set to false for better speed measurement


const char broker[] = "192.168.86.78";
int        port     = 1883;
const char primaryTopic[]  = "arduino/lightstrip";
const char preconfig[] = "arduino/lightstrip/*";
const char heartbeatTopic[] = "arduino/lightstrip/heartbeat";

auto timer = timer_create_default();

void setup() {
  //Initialize serial and wait for port to open:
  Ethernet.init(10);  // Most Arduino shields

  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

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

  // You can provide a unique client ID, if not set the library uses Arduino-millis()
  // Each client must have a unique client ID
  mqttClient.setId("light-strip");

  // You can provide a username and password for authentication
  mqttClient.setUsernamePassword(MQTT_USERNAME, MQTT_PASSWORD);

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();

  Serial.print("Subscribing to topic: ");
  Serial.println(primaryTopic);
  Serial.println();

  // subscribe to a topic
  mqttClient.subscribe(primaryTopic);

  // topics can be unsubscribed using:
  // mqttClient.unsubscribe(topic);

  Serial.print("Waiting for messages on topic: ");
  Serial.println(primaryTopic);
  Serial.println();

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(BRIGHTNESS);

  String willPayload = "disconnected";
  bool willRetain = true;
  int willQos = 1;

  mqttClient.beginWill(heartbeatTopic, willPayload.length(), willRetain, willQos);
  mqttClient.print(willPayload);
  mqttClient.endWill();

  timer.every(10000, publishHeartbeat);
}

bool publishHeartbeat(void *) {
  mqttClient.beginMessage(heartbeatTopic);
  mqttClient.print("alive");
  mqttClient.endMessage();
  Serial.println("Published heartbeat");
  return true; // return true to repeat this timer
}

void loop() {
  timer.tick();
  mqttClient.poll();

  if(!mqttClient.connected())
  {
    Serial.println("Disconnected from MQTT broker");
    exit(0);
  }

  int messageSize = mqttClient.parseMessage();
  if (messageSize) {
    // we received a message, print out the topic and contents
    Serial.print("Received a message with topic '");
    Serial.print(mqttClient.messageTopic());
    Serial.print("', length ");
    Serial.print(messageSize);
    Serial.println(" bytes:");

    // use the Stream interface to print the contents
    while (mqttClient.available()) {
      Serial.print((char)mqttClient.read());
    }
    Serial.println();

    Serial.println();
  }

  if(!colorSet)
  {
    //colorWipe(strip.Color(255,   100,   0, 255)     , 25); // Red
    colorSet = true;
  }
  pulseWhite(5);
}
void pulseWhite(uint8_t wait) {
  for(int j=0; j<256; j++) { // Ramp up from 0 to 255
    // Fill entire strip with white at gamma-corrected brightness level 'j':
    strip.fill(strip.Color(255, 100, 0, strip.gamma8(j)));
    strip.show();
    delay(wait);
  }

  for(int j=255; j>=0; j--) { // Ramp down from 255 to 0
    strip.fill(strip.Color(255, 100, 0, strip.gamma8(j)));
    strip.show();
    delay(wait);
  }
}