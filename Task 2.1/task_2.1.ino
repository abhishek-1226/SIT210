#include <WiFiNINA.h>
#include <WiFiClient.h>
#include <ArduinoHttpClient.h>

char ssid[] = "AirFiber-R938A6";     
char pass[] = "yeeyeeassed259"; 

const char* server = "api.thingspeak.com";
String apiKey = "R304RDVGVVGCDBJH";

const int trigPin = 2;
const int echoPin = 3;

WiFiClient wifi;
HttpClient client = HttpClient(wifi, server, 80);

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("WiFi connected!");
}

void loop() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  float distance = duration * 0.034 / 2; // cm

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  String postData = "api_key=" + apiKey + "&field1=" + String(distance);

  client.post("/update", "application/x-www-form-urlencoded", postData);
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  Serial.print("Status Code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);

  delay(10); // Wait 10
}
