#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <max6675.h>

// --- LCD ---
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- Thermocouple Pins ---
const int thermoDO  = 12;
const int thermoCS  = 10;
const int thermoCLK = 13;

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

// --- Potentiometer ---
const int potPin = A0;

// --- PWM Control ---
const int pwmPin = 9;
const int pwmMaxDuty = 200;  // 60% of 255

// --- Temp Range ---
const int tempMin = 0;
const int tempMax = 450;

// --- Control Timing ---
unsigned long lastMeasureTime = 0;
unsigned long pwmResumeTime = 0;
bool pwmPaused = false;

// --- PI Controller ---
float integralError = 0;
const float Kp = 1.0;    // Proportional gain
const float Ki = 0.005;  // Integral gain
const float integralMax = 1;  // clamp integral to prevent wind-up

// --- Measurement Buffer ---
const int measureInterval = 1000;  // every 1s
const int settleDelay = 70;        // ms to wait after PWM off
const int holdDelay   = 70;        // ms to wait before PWM resume

double actualTemp = 0;
double lastValidTemp = 25;  // assume room temp as starting point

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  pinMode(pwmPin, OUTPUT);
  analogWrite(pwmPin, 0);

  lcd.setCursor(0, 0);
  lcd.print("DC Soldering Iron");
  delay(1000);
  lcd.clear();
}

void loop() {
  unsigned long now = millis();

  // --- Periodic measurement cycle ---
  if (!pwmPaused && now - lastMeasureTime >= measureInterval) {
    pwmPaused = true;
    analogWrite(pwmPin, 0);       // Turn off PWM
    delay(settleDelay);           // Let noise settle

    double t = thermocouple.readCelsius();
    if (!isnan(t)) {
      actualTemp = t;
      lastValidTemp = t;
    } else {
      actualTemp = lastValidTemp; // use last valid reading
    }

    delay(holdDelay);             // small buffer before resuming PWM
    pwmResumeTime = now;          // mark resume time
    lastMeasureTime = now;
    pwmPaused = false;
  }

  // --- Read Setpoint ---
  int potValue = analogRead(potPin);
  int setTemp = constrain(map(potValue, 0, 1023, tempMin, tempMax), tempMin, tempMax);

  // --- PI Control ---
  double error = setTemp - actualTemp;

  // Proportional
  double controlP = Kp * error;

  // Integral (with clamping)
  integralError += error * 0.1;  // integrate (0.1 is approximate loop time scale factor)
  if (integralError > integralMax) integralError = integralMax;
  if (integralError < -integralMax) integralError = -integralMax;
  double controlI = Ki * integralError;

  double control = controlP + controlI;

  int targetPWM = 0;

  if (setTemp <= actualTemp) {
    targetPWM = 0;
    integralError = 0;  // reset integral when reached
  } else {
    targetPWM = constrain((int)control, 0, pwmMaxDuty);
  }

  // --- Direct PWM Output (no ramp) ---
  if (!pwmPaused) {
    analogWrite(pwmPin, targetPWM);
  }

  // --- Display ---
  lcd.setCursor(0, 0);
  lcd.print("Set: ");
  lcd.print(setTemp);
  lcd.print((char)223);
  lcd.print("C   ");

  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  lcd.print(actualTemp, 1);
  lcd.print((char)223);
  lcd.print("C   ");

  delay(50);
}
