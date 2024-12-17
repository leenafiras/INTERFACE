#include <Wire.h>
#include <LiquidCrystal.h>
#include <Encoder.h>
#include <Adafruit_ADXL345_U.h> // Include the ADXL345 library (Install via Library Manager)

// Francis - Miar - Leena

// Motor and Encoder Variables
const int Enable = 9;
const int IN_1 = 4;
const int IN_2 = 5;
const int potPin = A0;
float degrees = 0;
Encoder myEnc(3, 2);

// Initialize I2C LCD (Address: 0x27, 16 columns, 2 rows)
LiquidCrystal lcd(8, 7, 10, 11, 12, 13);

int motorSpeed = 80; // Fixed slow speed
long lastPosition = 0;

// ADXL345 Variables
Adafruit_ADXL345_Unified adxl = Adafruit_ADXL345_Unified(12345);
float adxlRoll = 0, adxlPitch = 0;

void setup() {
  Serial.begin(9600);

  // Initialize LCD
  lcd.begin(16, 2); // 16x2 LCD
  lcd.print("Motor Control!");
  delay(2000);
  lcd.clear();

  // Initialize motor pins
  pinMode(Enable, OUTPUT);
  pinMode(IN_1, OUTPUT);
  pinMode(IN_2, OUTPUT);
  pinMode(potPin, INPUT);

  // Initialize ADXL345
  if (!adxl.begin()) {
    lcd.print("ADXL FAIL");
    while (1); // Stop if ADXL345 is not connected
  }
  adxl.setRange(ADXL345_RANGE_16_G);
  lcd.print("ADXL OK");
  delay(2000);
  lcd.clear();
}

void loop() {
  //setMotorSpeed(motorSpeed);
  // Read encoder for position feedback
  float currentPosition = myEnc.read();
  Serial.println(currentPosition);

  currentPosition = currentPosition / 800;
  degrees = fmod(currentPosition * 360, 360); // Ensure degrees is between -360 and 360
  // Read ADXL345 angles
  sensors_event_t event;
  adxl.getEvent(&event);

  // Calculate pitch and roll (simplified)
  adxlPitch = atan2(event.acceleration.y, event.acceleration.z) * 180 / PI;
  adxlRoll = atan2(event.acceleration.x, event.acceleration.z) * 180 / PI;

  // Normalize roll to stay between -360 and 360
  adxlRoll = fmod(adxlRoll, 360); // Keep roll within the range
  if (adxlRoll < -360) adxlRoll += 360;
  if (adxlRoll > 360) adxlRoll -= 360;

  // Check if roll matches degree
  if (abs(abs(adxlRoll) - degrees) < 5.0) { // Tolerance of 1 degree
    setMotorSpeed(0); // Stop the motor
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("STOPPED!");
    lcd.setCursor(0, 1);
    lcd.print("Roll = Degree");
    Serial.println("Motor stopped: Roll matches Degree");
  } else {
    setMotorSpeed(motorSpeed); // Run motor at fixed speed
  }

  // Update displays
  if (currentPosition != lastPosition) {
    updateLCD(currentPosition, motorSpeed, degrees);
    updateSerial(currentPosition, motorSpeed, degrees, adxlRoll);
    lastPosition = currentPosition / 85;
  }

  delay(10);
}

void setMotorSpeed(int speed) {
  speed = constrain(speed, 0, 255); // Motor should not run in reverse

  if (speed == 0) {
    digitalWrite(IN_1, LOW);
    digitalWrite(IN_2, LOW);
  } else {
    digitalWrite(IN_1, HIGH);
    digitalWrite(IN_2, LOW);
  }

  analogWrite(Enable, speed);
}

void updateLCD(long revs, int speed, float degree) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Revs:");
  lcd.print(revs);

  lcd.setCursor(0, 1);
  lcd.print("mpu-deg:");
  lcd.print(adxlRoll);
  lcd.print("          ");
  lcd.print("motor Deg:");
  lcd.print(degree);
}

void updateSerial(float revs, int speed, float degree, float roll) {
 Serial.print(" | Roll: ");
 Serial.println(roll);
}