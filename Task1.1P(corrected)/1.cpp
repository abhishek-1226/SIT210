const int buttonPin = 6;
const int ledPin = LED_BUILTIN;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT); 
}

void loop() {
  if (digitalRead(buttonPin) == HIGH) 
{  
    blinkNameInMorse();
    delay(1500);
  }
}

void dot() 
{
  digitalWrite(ledPin, HIGH);
  delay(200);
  digitalWrite(ledPin, LOW);
  delay(200);
}

void dash() 
{
  digitalWrite(ledPin, HIGH);
  delay(600);
  digitalWrite(ledPin, LOW);
  delay(200);
}

void blinkLetter(char letter) 
{
  switch (letter) 
{
    case 'A': dot(); dash(); break;
    case 'B': dash(); dot(); dot(); dot(); break;
    case 'H': dot(); dot(); dot(); dot(); break;
    case 'I': dot(); dot(); break;
    case 'S': dot(); dot(); dot(); break;
    case 'E': dot(); break;
    case 'K': dash(); dot(); dash(); break;
  }
  delay(600);  // Space between letters
}

void blinkNameInMorse() 
{
  blinkLetter('A');
  blinkLetter('B');
  blinkLetter('H');
  blinkLetter('I');
  blinkLetter('S');
  blinkLetter('H');
  blinkLetter('E');
  blinkLetter('K');
}
