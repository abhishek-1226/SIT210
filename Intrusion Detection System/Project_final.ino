const int trigPin = 9;
const int echoPin = 10;
const int vibPin = 2;
const int buzzerPin = 6;

const unsigned long readInterval = 500; 
unsigned long lastRead = 0;

long readUltrasonic() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 20000); 
  if (duration == 0) return 400;                
  long distance = duration * 0.034 / 2;
  return distance > 400 ? 400 : distance;
}

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(vibPin, INPUT);
  pinMode(buzzerPin, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'B') {
      tone(buzzerPin, 1000, 4500); 
    }
  }

  if (millis() - lastRead >= readInterval) {
    lastRead = millis();

    long distance = readUltrasonic();
    int vib = digitalRead(vibPin);

    Serial.print("D:");
    Serial.print(distance);
    Serial.print(",V:");
    Serial.println(vib);
  }
}
