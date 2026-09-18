#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ================= OLED =================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_SDA 8
#define OLED_SCL 9

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ================= POTENTIOMETER =================
#define POT 4

// ================= LEDs =================
#define BLUE_LED 3
#define GREEN_LED 11
#define RED_LED 12

// ================= TB6612FNG =================
#define PWMA 10
#define AIN1 5
#define AIN2 6
#define STBY 7

void setup() {

  // ---------- OLED ----------
  Wire.begin(OLED_SDA, OLED_SCL);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (1);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  // ---------- Motor Driver ----------
  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(STBY, OUTPUT);

  digitalWrite(STBY, HIGH);
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);

  // ---------- LEDs ----------
  pinMode(BLUE_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  digitalWrite(BLUE_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);

  // ---------- Startup Screen ----------
  display.clearDisplay();

  display.setTextSize(2);
  display.setCursor(15, 8);
  display.println("MOTOR");

  display.setCursor(25, 32);
  display.println("CTRL");

  display.display();

  delay(1500);
}

void loop() {

  // ================= READ POTENTIOMETER =================

  int potValue = analogRead(POT);

  // ================= PWM CALCULATION =================

  int pwmValue;

  // Motor OFF at minimum
  if (potValue < 100) {
    pwmValue = 0;
  }
  else {
    // Minimum useful motor PWM = 100
    // Maximum PWM = 255
    pwmValue = map(potValue, 100, 4095, 100, 255);
  }

  // Limit PWM
  pwmValue = constrain(pwmValue, 0, 255);

  // Send PWM to motor driver
  analogWrite(PWMA, pwmValue);

  // ================= SPEED PERCENTAGE =================

  int speedPercent = map(pwmValue, 0, 255, 0, 100);

  // ================= LED CONTROL =================

  // Turn all LEDs OFF first
  digitalWrite(BLUE_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);

  if (speedPercent == 0) {

    // Motor OFF
    // All LEDs remain OFF
  }

  else if (speedPercent < 50) {

    // 1% - 49%
    digitalWrite(BLUE_LED, HIGH);
  }

  else if (speedPercent < 90) {

    // 50% - 89%
    digitalWrite(GREEN_LED, HIGH);
  }

  else {

    // 90% - 100%
    digitalWrite(RED_LED, HIGH);
  }

  // ================= OLED DISPLAY =================

  display.clearDisplay();

  // ----- Header -----
  display.setTextSize(1);
  display.setCursor(25, 2);
  display.println("MOTOR CONTROLLER");

  // Header line
  display.drawLine(0, 13, 127, 13, SSD1306_WHITE);

  // ----- SPEED -----

  display.setTextSize(1);
  display.setCursor(5, 20);
  display.println("SPEED");

  display.setTextSize(2);
  display.setCursor(75, 17);
  display.print(speedPercent);
  display.print("%");

  // ----- SPEED BAR -----

  display.drawRect(5, 40, 118, 10, SSD1306_WHITE);

  int barWidth = map(speedPercent, 0, 100, 0, 114);

  if (barWidth > 0) {
    display.fillRect(7, 42, barWidth, 6, SSD1306_WHITE);
  }

  // ----- STATUS -----

  display.setTextSize(1);
  display.setCursor(43, 55);

  if (speedPercent == 0) {
    display.print("OFF");
  }
  else if (speedPercent < 50) {
    display.print("LOW SPEED");
  }
  else if (speedPercent < 90) {
    display.print("MEDIUM");
  }
  else {
    display.print("HIGH SPEED");
  }

  display.display();

  delay(100);
}
