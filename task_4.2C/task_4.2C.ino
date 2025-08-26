const int buttonPin = 2;
const int motionPin = 3;
const int led1Pin = 4;
const int led2Pin = 5;

volatile bool buttonFlag = false;
volatile bool motionFlag = false;

bool led1State = LOW;
bool led2State = LOW;

void buttonISR() {
  buttonFlag = true;
}

void motionISR() {
  motionFlag = true;
}

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(motionPin, INPUT);
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  
  digitalWrite(led1Pin, led1State);
  digitalWrite(led2Pin, led2State);
  
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(motionPin), motionISR, RISING);
  
  Serial.begin(9600);
  Serial.println("System started. Waiting for interrupts");
}

void loop() {
  if (buttonFlag) {
    led1State = !led1State;
    digitalWrite(led1Pin, led1State);
    Serial.println("Button pressed: LED1 toggled");
    buttonFlag = false;
  }
  
  if (motionFlag) {
    led2State = !led2State;
    digitalWrite(led2Pin, led2State);
    Serial.println("Motion detected: LED2 toggled");
    motionFlag = false;
  }
}
