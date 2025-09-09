#include <Adafruit_ZeroTimer.h>
#include <Wire.h>
#include <BH1750.h>

// ---------------- Pin definitions ----------------
#define BUTTON_PIN   2
#define TRIG_PIN     4
#define ECHO_PIN     5
#define LED1         6
#define LED2         7
#define LED3         8

// ---------------- BH1750 Setup ----------------
BH1750 lightMeter;
uint8_t bh1750Address = 0x23;  // default, may switch to 0x5C

// ---------------- Timer Setup ----------------
Adafruit_ZeroTimer zt3 = Adafruit_ZeroTimer(3);  // use TC3
volatile bool tickFlag = false;
volatile int tickCount = 0;   // to make 2-second intervals

// ---------------- Volatile flags ----------------
volatile bool led1State = false;

// ---------------- Interrupt Service Routines ----------------
void buttonISR() {
  led1State = !led1State;   // toggle LED1
  Serial.print("Button pressed → LED1 ");
  Serial.println(led1State ? "ON" : "OFF");
}

// Timer ISR handler (Adafruit_ZeroTimer requires this form)
void TC3_Handler() {
  Adafruit_ZeroTimer::timerHandler(3);
}

// Our timer callback (runs every 1s)
void myCallback() {
  tickFlag = true;
}

// ---------------- Ultrasonic function ----------------
long readUltrasonicCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // timeout 30ms (~5m max)
  long distanceCM = duration * 0.034 / 2;
  return distanceCM;
}

// ---------------- Setup ----------------
void setup() {
  Serial.begin(115200);
  while (!Serial);
  delay(500);

  Serial.println("System initialized...");

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

  // External interrupt for button
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);

  // Configure TC3 for 1 Hz
  zt3.configure(TC_CLOCK_PRESCALER_DIV1024,   
                TC_COUNTER_SIZE_16BIT,
                TC_WAVE_GENERATION_MATCH_FREQ);

  zt3.setCompare(0, 46875);  // (48MHz / 1024 / 46875 = 1 Hz)
  zt3.setCallback(true, TC_CALLBACK_CC_CHANNEL0, myCallback);
  zt3.enable(true);

  // Initialize BH1750
  Wire.begin();
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0x23, &Wire)) {
    bh1750Address = 0x23;
    Serial.println("BH1750 detected at 0x23");
  } else if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0x5C, &Wire)) {
    bh1750Address = 0x5C;
    Serial.println("BH1750 detected at 0x5C");
  } else {
    Serial.println("BH1750 sensor NOT found!");
  }
}

// ---------------- Loop ----------------
void loop() {
  // --- Handle button LED ---
  digitalWrite(LED1, led1State);

  // --- Handle Ultrasonic ---
  long distance = readUltrasonicCM();
  if (distance > 10 && distance < 60) {  // object detected within 50 cm
    digitalWrite(LED2, HIGH);
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm | LED2: ON");
  } else {
    digitalWrite(LED2, LOW);
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm | LED2: OFF");
  }

  // --- Handle timer events ---
  if (tickFlag) {
    tickFlag = false;
    tickCount++;

    // Every 2 seconds
    if (tickCount >= 2) {
      tickCount = 0;

      // Toggle LED3
      digitalWrite(LED3, !digitalRead(LED3));

      // Read BH1750
      if (bh1750Address == 0x23 || bh1750Address == 0x5C) {
        float lux = lightMeter.readLightLevel();
        if (lux >= 0) {
          Serial.print("Light: ");
          Serial.print(lux);
          Serial.print(" lux | LED3: ");
          Serial.println(digitalRead(LED3) ? "ON" : "OFF");
        } else {
          Serial.println("BH1750 read failed!");
        }
      }
    }
  }

  delay(200); // stabilize ultrasonic readings
}