#include <WiFiNINA.h>
#include <PubSubClient.h>  // MQTT library

#define WIFI_SSID "AirFiber-R938A6"
#define WIFI_PASSWORD "yeeyeeassed259"

#define MQTT_SERVER "99abf86df0934a9ca7609c3e75e785a4.s1.eu.hivemq.cloud"
#define MQTT_PORT 8883
#define MQTT_TOPIC "sensor/sunlight"
#define MQTT_USER "hivemq.webclient.1755889253691"
#define MQTT_PASSWORD "7w.5Ku3QIPHDs#o6k!c?"

// Light Sensor Pin and Threshold
const int lightSensorPin = A0;
const int lightThreshold = 500;  // Adjust: Higher = brighter light needed to trigger
bool sunlightDetected = false;
bool prevSunlightDetected = false;

// Use SSL Client for secure connection
WiFiSSLClient wifiSSLClient;
PubSubClient mqttClient(wifiSSLClient);

// Function to Connect to MQTT Broker
void connectMQTT() {
  while (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT...");
    if (mqttClient.connect("ArduinoNanoIoT", MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("Connected to MQTT Broker!");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(lightSensorPin, INPUT);

  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Set MQTT Server
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);

  // Connect to MQTT
  connectMQTT();
}

void loop() {
  if (!mqttClient.connected()) {
    connectMQTT();
  }
  mqttClient.loop(); // Keep MQTT connection alive

  int lightValue = analogRead(lightSensorPin);
  sunlightDetected = (lightValue > lightThreshold);

  // Only publish if state has changed
  if (sunlightDetected != prevSunlightDetected) {
    if (sunlightDetected) {
      Serial.println("Sunlight Detected!");
      mqttClient.publish(MQTT_TOPIC, "Sunlight detected");
    } else {
      Serial.println("No sunlight");
      mqttClient.publish(MQTT_TOPIC, "No sunlight detected");
    }
    prevSunlightDetected = sunlightDetected;
  }

  delay(1000); // Check every second
}