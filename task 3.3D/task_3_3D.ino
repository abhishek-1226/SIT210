#include <WiFiNINA.h>          
#include <ArduinoMqttClient.h>  

char ssid[] = "Redmi Note 11T 5G";       
char pass[] = "yeeyeeassed";   

const char broker[] = "c4c7d2b1d21e40e6bd20667642a0b59c.s1.eu.hivemq.cloud";
int port = 8883;  

const char topic[] = "SIT210/wave";

const char mqttUser[] = "hivemq.webclient.1756369363108";
const char mqttPass[] = "79Ozk%Z8H,>MFy.3rlsE";

WiFiSSLClient wifiClient;
MqttClient mqttClient(wifiClient);

const int trigPin = 3;
const int echoPin = 2;

const int ledPin  = 4;

long duration;
int distance;

void setup() {
  Serial.begin(9600);  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);
  blinkLED(2, 500);
  connectWiFi();
  mqttClient.setUsernamePassword(mqttUser, mqttPass);
  Serial.print("Connecting to HiveMQ Cloud...");
  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed. Error code = ");
    Serial.println(mqttClient.connectError());
    while (1);
  }
  Serial.println("connected!");
  mqttClient.onMessage(onMqttMessage);
  mqttClient.subscribe(topic);
  Serial.println("Subscribed to topic: SIT210/wave");
}

void loop() {
  mqttClient.poll();
  distance = getDistance();
  Serial.print("Distance: ");
  Serial.println(distance);
  if (distance > 20 && distance < 40) {
    Serial.println("Wave detected. Publishing message...");
    mqttClient.beginMessage(topic);
    mqttClient.print("wave:Abhishek"); 
    mqttClient.endMessage();
    delay(2000); 
  }
  else if (distance <= 5 && distance > 0) {
    Serial.println("Pat detected. Publishing message...");
    mqttClient.beginMessage(topic);
    mqttClient.print("pat:Abhishek");
    mqttClient.endMessage();
    delay(2000);
  }
}

int getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  return duration * 0.034 / 2;
}

void onMqttMessage(int messageSize) {
  String msg = "";
  while (mqttClient.available()) {
    msg += (char)mqttClient.read();
  }
  Serial.println("Message received: " + msg);
  if (msg.startsWith("wave")) {
    blinkLED(3, 300);
  }
  else if (msg.startsWith("pat")) {
    blinkLED(4, 800);
  }
}

void blinkLED(int times, int delayTime) {
  for (int i = 0; i < times; i++) {
    digitalWrite(ledPin, HIGH);
    delay(delayTime);
    digitalWrite(ledPin, LOW);
    delay(delayTime);
  }
}

void connectWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Connected to WiFi.");
}