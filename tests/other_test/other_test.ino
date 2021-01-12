#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

const int led1 = 5;
const int led2 = 6;
const int iregler = 4;
const int ibewegungsmelder = 7;

void setup() {
  lcd.begin();
  Serial.begin(9600);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(iregler, OUTPUT);
  pinMode(ibewegungsmelder, OUTPUT);
  digitalWrite(led1, HIGH);
  digitalWrite(led2, HIGH);
  delay(5000);
}

void loop() {
  digitalWrite(iregler, HIGH);
  Serial.println("Potentiometer Indicator on");
  delay(2000);
  digitalWrite(iregler, LOW);
  Serial.println("Potentiometer Indicator off");
  delay(2000);
  digitalWrite(ibewegungsmelder, HIGH);
  Serial.println("Bewegungsmelder Indicator on");
  delay(2000);
  digitalWrite(ibewegungsmelder, LOW);
  Serial.println("Bewegungsmelder Indicator off");
  delay(2000);
  lcd.noBacklight();
  Serial.println("Display off");
  delay(2000);
  lcd.backlight();
  Serial.println("Display on");
  delay(1000);
  for (int i = 0; i <= 3; i++) {
    lcd.print("abcdefghijklmnop");
    lcd.setCursor(0, 1);
    lcd.print("abcdefghijklmnop");
    lcd.setCursor(0, 2);
    lcd.print("abcdefghijklmnop");
    lcd.setCursor(0, 3);
    lcd.print("abcdefghijklmnop");
    Serial.println("Printed");
    delay(1000);
    lcd.clear();
    Serial.println("cleared");
    delay(1000);
  }
  digitalWrite(led1, LOW);
  Serial.println("Led1 ON");
  delay(1000);
  digitalWrite(led1, HIGH);
  Serial.println("Led1 OFF");
  delay(1000);
  digitalWrite(led2, LOW);
  Serial.println("Led2 ON");
  delay(1000);
  digitalWrite(led2, HIGH);
  Serial.println("Led2 OFF");
  delay(1000);
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  Serial.println("Both Leds on");
  delay(1000);
  digitalWrite(led1, HIGH);
  digitalWrite(led2, HIGH);
  lcd.clear();
  lcd.noBacklight();
  Serial.println("Finished");
  delay(5000);
}
