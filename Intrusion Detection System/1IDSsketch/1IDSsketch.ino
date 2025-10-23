#define TRIG_PIN 3
#define ECHO_PIN 2
#define VIBRATION_PIN A0
#define BUZZER_PIN 4

long duration;
int distance;
int vibrationValue;

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(VIBRATION_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
}

void loop() {
  // Ultrasonic reading
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration * 0.034 / 2;

  // Vibration reading
  vibrationValue = analogRead(VIBRATION_PIN);

  // Send both readings as JSON-like line
  Serial.print("{\"distance\":");
  Serial.print(distance);
  Serial.print(",\"vibration\":");
  Serial.print(vibrationValue);
  Serial.println("}");

  delay(500);
}
