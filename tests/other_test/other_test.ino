#include <LiquidCrystal_I2C.h>
#include <Wire.h>

const int led1 = 5;
const int led2 = 6;

void setup(){
    lcd.begin();
    Serial.begin(9600);
    pinMode(led1, OUTPUT);
    pinMode(led2, OUTPUT);
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
}

void loop(){
    delay(1000);
    lcd.noBacklight();
    Serial.println("Display off");
    delay(1000);
    lcd.backlight();
    Serial.println("Display on");
    delay(1000);
    for (int i = 0; i <= 3; i++){
        lcd.print("abcdefghijklmnop");
        lcd.setCursor(0, 1);
        lcd.print("abcdefghijklmnop");
        lcd.setCursor(0, 2);
        lcd.print("abcdefghijklmnop");
        lcd.setCurosr(0, 3);
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