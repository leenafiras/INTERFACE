// Francis - Miar - Leena
#include <Wire.h>
#include <LiquidCrystal.h>
#include <Encoder.h>

const int Enable = 9;
const int IN_1 = 4;
const int IN_2 = 5;
const int potPin = A0;

// Create Encoder object
Encoder myEnc(3, 2);

// Initialize I2C LCD (Address: 0x27, 16 columns, 2 rows)
LiquidCrystal lcd(8, 7, 10, 11, 12, 13); 

int motorSpeed = 0;
long lastPosition = 0;

void setup() {
  Serial.begin(9600);
  
  // Initialize LCD
  lcd.begin(16, 2); // 16x2 LCD
  lcd.print("Motor Control!"); 
  delay(2000); 
  lcd.clear();
  pinMode(Enable, OUTPUT);
  pinMode(IN_1, OUTPUT);
  pinMode(IN_2, OUTPUT);
  pinMode(potPin, INPUT);
}

void loop() {
  int potValue = analogRead(potPin);
  
  // Convert pot value to motor speed and direction
  // 0-512 = reverse, 512-1023 = forward
  if (potValue < 512) {
    motorSpeed = map(potValue, 511, 0, 0, 255);  // Reverse
  } else {
    motorSpeed = map(potValue, 512, 1023, 0, 255);  // Forward
  }
  
  // Read encoder for position feedback
  long currentPosition = myEnc.read();
  currentPosition=currentPosition/210;
  // Set motor speed
  setMotorSpeed(motorSpeed * (potValue < 512 ? -1 : 1));
  
  // Update displays
  if (currentPosition != lastPosition) {
    updateLCD(currentPosition, motorSpeed, potValue);
    updateSerial(currentPosition, motorSpeed, potValue);
    lastPosition = currentPosition/85;
  }
  
  delay(10);
}

void setMotorSpeed(int speed) {
  speed = constrain(speed, -255, 255);
  
  // Dead zone handling
  if (abs(speed) < 20) {
    speed = 0;
  }
  
  if (speed >= 0) {
    digitalWrite(IN_1, HIGH);
    digitalWrite(IN_2, LOW);
  } else {
    digitalWrite(IN_1, LOW);
    digitalWrite(IN_2, HIGH);
  }
  
  analogWrite(Enable, abs(speed));
}

void updateLCD(long revs, int speed, int pot) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Revs:");
  lcd.print(revs);
  
  lcd.setCursor(0, 1);
  lcd.print("Spd:");
  lcd.print(speed);
  lcd.print(" P:");
  lcd.print(pot);
}

void updateSerial(long revs, int speed, int pot) {
  Serial.print("revs: ");
  Serial.print(revs);
  Serial.print(" | Speed: ");
  Serial.print(speed);
  Serial.print(" | Pot: ");
  Serial.print(pot);
  Serial.print(" | Dir: ");
  Serial.println(speed >= 0 ? "Forward" : "Reverse");
}