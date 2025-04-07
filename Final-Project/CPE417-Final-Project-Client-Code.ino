#define BLYNK_TEMPLATE_ID "TMPL2ewERJvbx"
#define BLYNK_TEMPLATE_NAME "Yanai"
#define BLYNK_PRINT SerialUSB

#include <ArduinoMqttClient.h>
#include <WiFi101.h>
#include <SPI.h>
#include <BlynkSimpleWiFiShield101.h>

// WiFi details
char ssid[] = "Bambi"; // My network SSID (name)
char pass[] = "x34211234"; // My network password

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

// Blynk Auth Token
char auth[] = "tKIm_pAUXFNg4ewB7zW2ICcPgAzJvHdq";

// Broker details
const char broker[] = "192.168.0.149"; // Broker here is mosquitto (IP of client)
int port = 1883; // Channel of the broker

// Topics
const char topic1[] = "light_level_topic";
const char topic2[] = "soil_moist_topic";

// LED and buzzer pins
const int redPin = 6; // Red color pin
const int yellowPin = 7; // Green color pin
const int greenPin = 8; // Blue color pin
const int buzzerPin = 10; // Speaker pin

// Variables to store topic values
float lightValue = 0.0;
float moistureValue = 0.0;

// Bool for button on Blynk app
bool isBuzzerControlledByBlynk = false;

// Write to Bynk app to control buzzer
BLYNK_WRITE(V1) {
  // Set flag based on V1
  isBuzzerControlledByBlynk = (param.asInt() == 1);
  
  if (isBuzzerControlledByBlynk) {
    digitalWrite(buzzerPin, HIGH);
  } else {
    digitalWrite(buzzerPin, LOW);
  }
}

// Set up
void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  while (!Serial) {
    ; // Wait for serial port to open/connect. Needed for native USB port only
  }

  // Initialize RGB LED pins
  pinMode(redPin, OUTPUT);
  pinMode(yellowPin, OUTPUT);
  pinMode(greenPin, OUTPUT);

  // Set all colors to OFF
  analogWrite(redPin, LOW);
  analogWrite(yellowPin, LOW);
  analogWrite(greenPin, LOW);

  // Attempt to connect to Wifi network:
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);

  // Failed, retry
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
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
  Serial.println("You're connected to the MQTT broker!"); // Print connected message
  Serial.println();

  // Set the message receive callback, setup that you are the subscriber
  mqttClient.onMessage(onMqttMessage);
  
  // Prints topics subscribing message
  Serial.println("Subscribing to topic: ");
  Serial.println(topic1);
  Serial.println(topic2);
  Serial.println();

  // Subscribe to a topic
  mqttClient.subscribe(topic1);
  mqttClient.subscribe(topic2);

  // Print topics
  Serial.print("Topic: ");
  Serial.println(topic1);
  Serial.print("Topic: ");
  Serial.println(topic2);
  Serial.println();
  
  // Start Blynk
  Blynk.begin(auth, ssid, pass);
}

void loop() {
  // Call poll() regularly to allow the library to receive MQTT messages and
  // Send MQTT keep alive which avoids being disconnected by the broker
  mqttClient.poll();
  // Run Blynk
  Blynk.run();
}

// Set individual LED colors based on the topic and value
void setLEDs(int redValue, int yellowValue, int greenValue) {
  analogWrite(redPin, redValue);
  analogWrite(yellowPin, yellowValue);
  analogWrite(greenPin, greenValue);
}

// Blink the RGB LED with a specific color passed
void blinkLED(int red, int yellow, int green, int delayMs) {
  setLEDs(red, yellow, green);
  delay(delayMs);
  setLEDs(0, 0, 0);
  delay(delayMs);
}

// Print the MQTT message
void onMqttMessage(int messageSize) {
  
/* Commented out - for debugging
  // We received a message, print out the topic and contents
  Serial.println("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");

  // Use the Stream interface to print the contents
  while (mqttClient.available()) {
    Serial.print((char)mqttClient.read());
  }

  Serial.println();
  Serial.println();
*/

  // Get the topic
  String topic = mqttClient.messageTopic(); 
  // Store the message content
  String message = "";

  // Read the message content
  while (mqttClient.available()) {
    message += (char)mqttClient.read();
  }
  
  // Print the message content
  Serial.print("Received message on topic '");
  Serial.print(topic);
  Serial.print("': ");
  Serial.println(message);

  // Convert message to a float
  float value = message.toFloat();
  
  // Store values based on the topic
  // Light level
  if (topic == topic1){
    lightValue = value;
    // Write to Blynk to V5 var
    Blynk.virtualWrite(V5, value);
    
  }
  // Soil moisture
  else if (topic == topic2) { 
    moistureValue = value;
    // Write to Blynk to V6 var
    Blynk.virtualWrite(V6, value);
  }
  
  // Allow MQTT control only if Blynk isn't
  if (!isBuzzerControlledByBlynk) {
    // Combined condition: check both light and moisture values
    if (lightValue < 50.0 || moistureValue < 50.0) {
      // Critical (red)
      analogWrite(redPin, 255);
      analogWrite(greenPin, 0);
      // Play warning tone on speaker
      sirenEffect();
		}
    else {
      // Optimal (green)
      analogWrite(redPin, 0);
      analogWrite(greenPin, 255);
      tone(buzzerPin, 0);
    }
	}
}

// Siren effect function
void sirenEffect() {
  // Turn the tone on and off repeatedly to create the siren effect
  tone(buzzerPin, 1000); // Play tone at 1000Hz
  delay(500); // Wait for 500ms
  tone(buzzerPin, 0); // Turn off tone
  delay(500); // Wait for 500ms
}
