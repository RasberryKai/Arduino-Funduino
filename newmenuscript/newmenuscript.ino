#include <IRremote.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);
IRrecv irrecv(2);
decode_results results;

const int trigger = 9;
const int echo = 8;
long dauer = 0;
long entfernung = 0;

const int x_pin = A1;
const int y_pin = A0;
const int sw_pin = 10;
const int led1 = 6;
const int led2 = 5;

const int iregler = 4;
const int ibewegungsmelder = 7;
const int licht = A3;
const int poti = A2;

const int t = 400;
float kalibrierwert;
int ledpwm = 240;

unsigned long timestamp;
unsigned long autosleep;

bool led = false;

bool NormalB = true;
bool ExtendedB = false;
bool regler = false;
bool modus = false;

int NormalC = 1;
int ExtendedC;


class Menu {
  public:
    void extended() {
      Serial.println("Extended Menu update");
      switch (ExtendedC) {
        case 1:
          Serial.println("MENU EXTENDED 1");
          lcd.clear();
          lcd.print(">Potentiometer");
          lcd.setCursor(0, 1);
          lcd.print(" Bewegungsmelder");
          lcd.setCursor(0, 2);
          lcd.print(" Kalibrierung");
          break;

        case 2:
          Serial.println("MENU EXTENDED Case 2");
          lcd.clear();
          lcd.print(" Potentiometer");
          lcd.setCursor(0, 1);
          lcd.print(">Bewegungsmelder");
          lcd.setCursor(0, 2);
          lcd.print(" Kalibrierung");
          break;

        case 3:
          Serial.println("MENU EXTENDED Case 3");
          lcd.clear();
          lcd.print(" Potentiometer");
          lcd.setCursor(0, 1);
          lcd.print(" Bewegungsmelder");
          lcd.setCursor(0, 2);
          lcd.print(">Kalibrierung");
          break;
      }
    }

    void normal() {
      Serial.println("Normal Menu updte");
      switch (NormalC) {
        case 1:
          Serial.println("MENU NORMAL Case 1");
          lcd.clear();
          lcd.print(">An");
          lcd.setCursor(0, 1);
          lcd.print(" Aus");
          lcd.setCursor(0, 2);
          lcd.print(" Schlafmodus");
          break;

        case 2:
          Serial.println("MENU NORMAL Case 2");
          lcd.clear();
          lcd.print(" An");
          lcd.setCursor(0, 1);
          lcd.print(">Aus");
          lcd.setCursor(0, 2);
          lcd.print(" Schlafmodus");
          break;

        case 3:
          Serial.println("MENU NORMAL Case 3");
          lcd.clear();
          lcd.print(" An");
          lcd.setCursor(0, 1);
          lcd.print(" Aus");
          lcd.setCursor(0, 2);
          lcd.print(">Schlafmodus");
          break;
      }
    }
};


Menu menu;


class Bewegungsmelder {
  private:
    const int lichtvergleichswert = 10;
    int sensorwert;
    int entfernung1;
    int entfernung2;
    int entfernung3;
    bool returnvalue;

    void swap(int arr[], int i1, int i2) {
      int t = arr[i1];
      arr[i1] = arr[i2];
      arr[i2] = t;
    }

    bool sort(int arr[]) {
      for (int i  = 1; i < 3; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && key < arr[j]) {
          arr[j + 1] = arr[j];
          j -= 1;
        }
        arr[j + 1] = key;
      }

      if (arr[2] - arr[0] < 3) {
        return true;
      } else {
        return false;
      }
    }

  public:
    float entfernung_abfrage() {
      Serial.println("BEWEGUNGSMELDER entfernungs_abfage");
      digitalWrite(trigger, LOW);
      delay(5);
      digitalWrite(trigger, HIGH);
      delay(10);
      digitalWrite(trigger, LOW);
      dauer = pulseIn(echo, HIGH);
      entfernung = (dauer / 2) * 0.03432;
      return entfernung;
    }

    bool licht_abfrage() {
      sensorwert = analogRead(licht);
      if (sensorwert < lichtvergleichswert) {
        return true;
      } else {
        return false;
      }
    }


    bool kalibrierung() {
      lcd.clear();
      lcd.print("kalibriert...");

      delay(200);
      entfernung1 = entfernung_abfrage();
      delay(1000);
      entfernung2 = entfernung_abfrage();
      delay(1000);
      entfernung3 = entfernung_abfrage();

      int list[] = {entfernung1, entfernung2, entfernung3};

      returnvalue = sort(list);

      if (returnvalue) {
        kalibrierwert = (entfernung1 + entfernung2 + entfernung3) / 3;
        return true;
      } else {
        return false;
      }
    }
};


Bewegungsmelder bm;


class Controller {
  private:
    bool ledcheck = false;
    int previouswert;
    int wert;

    void Potimeter() {
      Serial.println("CONTOLLER Potentiometer");
      wert = analogRead(poti) / 4 + 0.75;
      if (wert - previouswert < -2 || wert - previouswert > 2) {
        autosleep = millis();
      }

      previouswert = wert;
      if (wert >= 247) {
        digitalWrite(led1, HIGH);
        digitalWrite(led2, HIGH);
      } else {
        analogWrite(led1, wert);
        analogWrite(led2, wert);
      }
    }

    void Bewegungsmelder() {
      Serial.println("CONTROLLER Bewegungsmelder");
      if (ledcheck) {
        if (millis() - timestamp >= 10000) {
          digitalWrite(led1, HIGH);
          digitalWrite(led2, HIGH);
          ledcheck = false;
        }
      }
      bool licht = bm.licht_abfrage();
      float distance = bm.entfernung_abfrage();
      if  (licht && distance < kalibrierwert) {
        digitalWrite(led1, LOW);
        digitalWrite(led2, LOW);
        timestamp = millis();
        ledcheck = true;
      }
    }

  public:
    void Init() {
      if (regler) {
        Potimeter();
      }
      if (modus) {
        Bewegungsmelder();
      }
    }
};


Controller controller;


class Actions {
  private:
    bool kalibriert = false;
    bool loopB = false;

    void indicatorcheck() {
      Serial.println("Indicatorcheck");
      if (regler) {
        digitalWrite(iregler, HIGH);
      } else if (!regler) {
        digitalWrite(iregler, LOW);
      }
      if (modus) {
        digitalWrite(ibewegungsmelder, HIGH);
      } else if (!modus) {
        digitalWrite(ibewegungsmelder, LOW);
      }
    }


    void Regler() {
      modus  = false;
      regler = not(regler);
      if (!regler) {
        digitalWrite(led1, HIGH);
        digitalWrite(led2, HIGH);
        ledpwm = 240;
      }
      indicatorcheck();
    }

    void Melder() {
      if (kalibriert) {
        digitalWrite(led1, HIGH);
        digitalWrite(led2, HIGH);
        ledpwm = 240;

        regler = false;
        modus = not(modus);
        indicatorcheck();
      } else {
        lcd.clear();
        lcd.print("Der Bewegungsmelder");
        lcd.setCursor(0, 1);
        lcd.print("wurde noch nicht");
        lcd.setCursor(0, 2);
        lcd.print("kalibriert.");
        delay(2000);
        menu.extended();
      }
    }

  public:
    void sleepmode() {
      lcd.clear();
      lcd.noBacklight(); // Schaltet das Licht des lcd-Displays aus
      analogWrite(iregler, 0);
      analogWrite(ibewegungsmelder, 0);
      delay (t);
      irrecv.resume();

      while (true) {
        Serial.println("SLEEPMODE");
        controller.Init();
        if (digitalRead(sw_pin)) {
          Serial.println("SLEEPMODE Button Exit");
          autosleep = millis();
          break;
        }
        if (irrecv.decode(&results)) {
          if (results.value == 0xFF02FD) {
            Serial.println("SLEEPMODE Remote Exit");
            autosleep = millis();
            irrecv.resume();
            break;
          }
        }
      }
      lcd.backlight();
      indicatorcheck();
      menu.normal();
      delay(t);
    }

    void autosleepCheck() {
      if (millis() - autosleep >= 120000) {
        sleepmode();
      }
    }

    void func() {
      lcd.clear();
      if (regler) {
        lcd.print("1- Poti aus");
      } else {
        lcd.print("1- Poti an");
      }
      lcd.setCursor(0, 1);
      if (modus) {
        lcd.print("2- Bew.-meld. aus");
      } else {
        lcd.print("2- Bew.-meld. an");
      }
      lcd.setCursor(0, 2);
      lcd.print("3- Kalibrierung");
      lcd.setCursor(0, 3);
      lcd.print("Func- Exit");
      delay(400);
      irrecv.resume();
      loopB = true;
      Serial.println("Im FUNKTIONSMODUS");
      while (loopB) {
        if (irrecv.decode(&results)) {
          switch (results.value) {
            case 0xFF30CF: // 1
              Serial.println("FUNKTIONSMODUS 1-Taste");
              Regler();
              delay(t);
              if (ExtendedB) {
                menu.extended();
              } else {
                menu.normal();
              }
              loopB = false;
              Serial.println("loopB = false");
              Serial.println("break;");
              break;

            case 0xFF18E7: // 2
              Serial.println("FUNKTIONSMODUS 2-Taste");
              Melder();
              delay(t);
              if (ExtendedB) {
                menu.extended();
              } else {
                menu.normal();
              }
              Serial.println("menu.extended()");
              loopB = false;
              Serial.println("loopB = false");
              Serial.println("break;");
              break;

            case 0xFF7A85: // 3
              Serial.println("FUNKTIONSMODUS 3-Taste");
              if (modus) {
                lcd.clear();
                lcd.print("Schalten sie");
                lcd.setCursor(0, 1);
                lcd.print("Bewegungsmelder aus");
              } else {
                kalibriert = bm.kalibrierung();
                if (kalibriert) {
                  lcd.clear();
                  lcd.print("Kalibrierung");
                  lcd.setCursor(0, 1);
                  lcd.print("erfolgreich");
                } else {
                  lcd.clear();
                  lcd.print("Kalibrierung");
                  lcd.setCursor(0, 1);
                  lcd.print("fehlgeschlagen");
                }
              }
              delay(2000);
              if (ExtendedB) {
                menu.extended();
              } else {
                menu.normal();
              }
              loopB = false;
              Serial.println("loopB = false");
              Serial.println("break;");
              break;

            case 0xFFE21D: //Func
              Serial.println("FUNKTIONSMODUS Func-Taste");
              delay(t);
              if (ExtendedB) {
                menu.extended();
              } else {
                menu.normal();
              }
              loopB = false;
              Serial.println("loopB = false");
              Serial.println("break;");
              break;
          }
          irrecv.resume();
        }
      }
    }


    void extended(int extendedC) {
      switch (extendedC) {
        case 1:
          Serial.println("Extended Action 1");
          Regler();
          break;

        case 2:
          Serial.println("Extended Action 2");
          Melder();
          break;

        case 3:
          Serial.println("Extended Action 3");
          if (modus) {
            lcd.clear();
            lcd.print("Schalten sie");
            lcd.setCursor(0, 1);
            lcd.print("Bewegungsmelder aus");
            delay(2000);
            menu.extended();
          } else {
            kalibriert = bm.kalibrierung();
            if (kalibriert) {
              lcd.clear();
              lcd.print("Kalibrierung");
              lcd.setCursor(0, 1);
              lcd.print("erfolgreich");
            } else {
              lcd.clear();
              lcd.print("Kalibrierung");
              lcd.setCursor(0, 1);
              lcd.print("fehlgeschlagen");
            }
          }
          delay(2000);
          menu.extended();
          break;
      }
    }

    void normal(int normalC) {
      switch (normalC) {
        case 1:
          Serial.println("Normal Action 1");
          led = true;
          regler = false;
          modus = false;
          ledpwm = 240;
          analogWrite(iregler, 0);
          analogWrite(ibewegungsmelder, 0);
          digitalWrite(led1, LOW);
          digitalWrite(led2, LOW);
          break;

        case 2:
          Serial.println("Normal Action 2");
          led = false;
          regler = false;
          modus = false;
          indicatorcheck();
          ledpwm = 60;
          digitalWrite(led1, HIGH);
          digitalWrite(led2, HIGH);
          break;

        case 3:
          Serial.println("Normal Action 3");
          sleepmode();
          break;
      }
    }
};


Actions actions;


class InputManager {
  private:
    void remoteControl(int code) {
      switch (code) {
        case 0xFFA25D: //Power-Taste
          irrecv.resume();
          regler = false;
          modus = false;
          digitalWrite(iregler, LOW);
          digitalWrite(ibewegungsmelder, LOW);
          Serial.println("Power");
          led = not(led);
          if (led) {
            digitalWrite(led1, LOW);
            digitalWrite(led2, LOW);
            ledpwm = 60;
          } else {
            digitalWrite(led1, HIGH);
            digitalWrite(led2, HIGH);
            ledpwm = 240;
          }
          delay(t);
          break;

        case 0xFFE21D: //Funktions-Taste
          Serial.println("Funktions Taste");
          irrecv.resume();
          actions.func();
          delay(t);
          break;

        case 0xFFE01F: //Down-Taste
          Serial.println("Down-Taste");
          if (modus == false && regler == false && ledpwm < 240) {
            irrecv.resume();
            Serial.print("Davor");
            Serial.println(ledpwm);
            ledpwm += 60;
            Serial.print("Danach: ");
            Serial.println(ledpwm);
            if (ledpwm == 240) {
              led = false;
            }
            analogWrite(led1, ledpwm);
            analogWrite(led2, ledpwm);
          }
          delay(t);
          break;

        case 0xFF906F: //Up-Taste
          Serial.println("Up-Taste");
          if (modus == false && regler == false && ledpwm > 60) {
            Serial.print("Davor: ");
            Serial.println(ledpwm);
            ledpwm -= 60;
            Serial.print("Danach: ");
            Serial.println(ledpwm);
            if (ledpwm == 60) {
              led = true;
            }
            analogWrite(led1, ledpwm);
            analogWrite(led2, ledpwm);
          }
          delay(t);
          break;

        case 0xFF02FD: // >||
          Serial.println("Pause Weiter");
          actions.sleepmode();
          delay(t);
          break;

        case 0xFF30CF: // 1
          if (modus == false && regler == false) {
            Serial.println("1 Taste");
            analogWrite(led1, 240);
            analogWrite(led2, 240);
            ledpwm = 240;
            led = false;
          }
          delay(t);
          break;

        case 0xFF18E7: // 2
          if (modus == false && regler == false) {
            Serial.println("2 Taste");
            analogWrite(led1, 160);
            analogWrite(led2, 160);
            ledpwm = 180;
          }
          delay(t);
          break;

        case 0xFF7A85: // 3
          if (modus == false && regler == false) {
            Serial.println("3 Taste");
            analogWrite(led1, 80);
            analogWrite(led2, 80);
            ledpwm = 60;
            led = true;
          }
          delay(t);
          break;

        case 0xFF9867: // EQ
          Serial.println("EQ Taste");
          regler = false;
          modus = false;
          digitalWrite(iregler, LOW);
          digitalWrite(ibewegungsmelder, LOW);
          digitalWrite(led1, HIGH);
          digitalWrite(led2, HIGH);
          ledpwm = 240;
          delay(t);
          break;
      }
    }

  public:
    void Init() {
      if (irrecv.decode(&results)) {
        Serial.print("Infrarot Messung: ");
        Serial.println(results.value);
        delay(t);
        irrecv.resume();
        autosleep = millis();
        remoteControl(results.value);
      }

      int y = analogRead(y_pin);
      int x = analogRead(x_pin);
      int sw = digitalRead(sw_pin);

      if (ExtendedB) {
        if (y > 1000) {
          autosleep = millis();
          Serial.println("Extended -> Normal");
          ExtendedB = false;
          NormalB = true;
          if (ExtendedC == 4) {
            NormalC = 3;
          } else {
            NormalC = ExtendedC;
          }
          menu.normal();
          while (analogRead(y_pin) > 1000);
        }
        if (x > 1000 && ExtendedC != 3) {
          autosleep = millis();
          Serial.println("Extended Down");
          ExtendedC ++;
          menu.extended();
          while (analogRead(x_pin) > 1000);
        } else if (x < 10 && ExtendedC != 1) {
          autosleep = millis();
          Serial.println("Extended Up");
          ExtendedC --;
          menu.extended();
          while (analogRead(x_pin) < 10);
        }
        if (sw) {
          autosleep = millis();
          Serial.println("Extended Button");
          actions.extended(ExtendedC);
          while (digitalRead(sw_pin));
        }
      } else {
        if (y < 10) {
          autosleep = millis();
          Serial.println("Normal -> Extended");
          NormalB = false;
          ExtendedB = true;
          ExtendedC = NormalC;
          menu.extended();
          while (analogRead(y_pin) < 10);
        }
        if (x > 1000 && NormalC != 3) {
          autosleep = millis();
          Serial.println("Normal Down");
          NormalC ++;
          menu.normal();
          while (analogRead(x_pin) > 1000);
        } else if (x < 10 && NormalC != 1) {
          autosleep = millis();
          Serial.println("Normal Up");
          NormalC --;
          menu.normal();
          while (analogRead(x_pin) < 10);
        }
        if (sw) {
          autosleep = millis();
          Serial.println("Normal Button");
          actions.normal(NormalC);
          while (digitalRead(sw_pin));
        }
      }
    }
};


InputManager inputmanager;


void setup() {
  Serial.begin(9600);
  lcd.begin();
  lcd.backlight();

  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(x_pin, INPUT);
  pinMode(y_pin, INPUT);

  pinMode(sw_pin, INPUT);
  pinMode(licht, INPUT);
  timestamp = millis();

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  digitalWrite(led1, HIGH);
  digitalWrite(led2, HIGH);

  menu.normal();
  irrecv.enableIRIn();
}


void loop() {
  inputmanager.Init();
  controller.Init();
  actions.autosleepCheck();
}
