#include <ArduinoMqttClient.h>
#include <WiFi101.h>
#include <ThingSpeak.h>

#define lightPin A1 // Pin for light sensor
#define soilPin A2 // Pin for soil moisture sensor

// WiFi details
char ssid[] = "Bambi"; // My network SSID (name)
char pass[] = "x34211234"; // My network password (use for WPA, or use as key for WEP)

WiFiClient wifiClient;
WiFiClient wifiClient4ThingSpeak;
MqttClient mqttClient(wifiClient);

// Broker details
const char broker[] = "192.168.0.149"; // Broker here is mosquitto (IP of client)
int port= 1883; // Channel of the broker

// ThingSpeak API Key and channel details
const char THINGSPEAK_API_KEY[] = "245EKRUDDY92L2KW";
unsigned long THINGSPEAK_CHANNEL_ID = 2741565;

// Topics
const char topic1[] = "light_level_topic";
const char topic2[] = "soil_moist_topic";

// Set interval for sending messages (milliseconds) for MQTT
const long interval = 8000;
unsigned long previousMillis = 0;

// Set interval for sending messages (milliseconds) for ThingSpeak
const long thingSpeakInterval = 30000; // 30 seconds
unsigned long thingSpeakPreviousMillis = 0;

// Set up
void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to open/connect. Needed for native USB port only
  }

  // Attempt to connect to Wifi network:
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // Failed, retry
    Serial.print(".");
    delay(5000);
  }

  // Print success message for connection to wifi
  Serial.println("You're connected to the network");
  Serial.println();
    
  // Print success message for connecting attempt to broker
  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  // If connection to the broker failed, print failed message
  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    while (1);
  }

  // Print MQTT connected message
  Serial.println("You're connected to the MQTT broker!");
  Serial.println();

  // Initialize ThingSpeak
  // ThingSpeak.begin(wifiClient4ThingSpeak);
}

// Main loop
void loop() {
/* Call poll() regularly to allow the library to send MQTT keep alive which
   avoids being disconnected by the broker */
mqttClient.poll();

unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // Save the last time a message was sent
    previousMillis = currentMillis;
    
    // Record light sensor value
    float light_level = analogRead(lightPin); // Read light level from pin

    // Record soil moisture value
    float soil_moisture = analogRead(soilPin); // Read soil moisture from pin

  /* // ThingSpeak portion commented out
    if (currentMillis - thingSpeakPreviousMillis >= thingSpeakInterval) {
      thingSpeakPreviousMillis = currentMillis;

      // Update ThingSpeak
      ThingSpeak.setField(1, light_level); 
      ThingSpeak.setField(2, soil_moisture);

      int status = ThingSpeak.writeFields(THINGSPEAK_CHANNEL_ID, THINGSPEAK_API_KEY);
      if (status == 200) {
        Serial.println("ThingSpeak update successful.");
      } else {
        Serial.print("ThingSpeak update failed, HTTP error code: ");
        Serial.println(status);
      }
    }
    
    // Send raw sensor readings to ThingSpeak
    ThingSpeak.setField(1, light_level); // Field 1 for light level
    ThingSpeak.setField(2, soil_moisture); // Field 2 for soil moisture

    int status = ThingSpeak.writeFields(THINGSPEAK_CHANNEL_ID, THINGSPEAK_API_KEY);
    if (status == 200) {
      Serial.println("ThingSpeak update successful.");
    } else {
      Serial.print("ThingSpeak update failed, HTTP error code: ");
      Serial.println(status);
    }
    */

    // Register the light level by the topic 1
    Serial.print("Sending message to topic: ");
    Serial.println(topic1);
    Serial.println(light_level);

    // Register the soil moisture by the topic 2
    Serial.print("Sending message to topic: ");
    Serial.println(topic2);
    Serial.println(soil_moisture);
    
    // Send the message for topic 1
    mqttClient.beginMessage(topic1);
    mqttClient.print(light_level);
    mqttClient.endMessage();

    // Send the message for topic 2
    mqttClient.beginMessage(topic2);
    mqttClient.print(soil_moisture);
    mqttClient.endMessage();
  
    Serial.println();
  }
}
