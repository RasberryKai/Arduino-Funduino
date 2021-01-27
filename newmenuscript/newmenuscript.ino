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
const int button = 10;
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
bool bew_poti = false;

bool NormalB = true;
bool ExtendedB = false;
bool regler = false;
bool modus = false;

int NormalC = 1;
int ExtendedC;


class Menu { // Contains extended and normal menu display
  public:
    void extended() { // Second menu / only DISPLAYS extended menu
      Serial.println("Extended Menu update"); // Prints it in Console for overview
      switch (ExtendedC) {
        case 1: // Potentiometer
          Serial.println("MENU EXTENDED 1"); // Prints it in Console for overview
          lcd.clear();
          lcd.print(">Potentiometer");
          lcd.setCursor(0, 1);
          lcd.print(" Bewegungsmelder");
          lcd.setCursor(0, 2);
          lcd.print(" Kalibrierung");
          lcd.setCursor(0, 3);
          if (bew_poti) {
            lcd.print(" Bew.-Meld-Poti aus");
          } else {
            lcd.print(" Bew.-Meld-Poti an");
          }
          break;

        case 2: // Bewegungsmelder
          Serial.println("MENU EXTENDED Case 2"); // Prints it in Console for overview
          lcd.clear();
          lcd.print(" Potentiometer");
          lcd.setCursor(0, 1);
          lcd.print(">Bewegungsmelder");
          lcd.setCursor(0, 2);
          lcd.print(" Kalibrierung");
          lcd.setCursor(0, 3);
          if (bew_poti) {
            lcd.print(" Bew.-Meld-Poti aus");
          } else {
            lcd.print(" Bew.-Meld-Poti an");
          }
          break;

        case 3: // Kalibrierung
          Serial.println("MENU EXTENDED Case 3"); // Prints it in Console for overview
          lcd.clear();
          lcd.print(" Potentiometer");
          lcd.setCursor(0, 1);
          lcd.print(" Bewegungsmelder");
          lcd.setCursor(0, 2);
          lcd.print(">Kalibrierung");
          lcd.setCursor(0, 3);
          if (bew_poti) {
            lcd.print(" Bew.-Meld-Poti aus");
          } else {
            lcd.print(" Bew.-Meld-Poti an");
          }
          break;

        case 4:
          Serial.println("MENU EXTENDED Case 4");
          lcd.clear();
          lcd.print(" Potentiometer");
          lcd.setCursor(0, 1);
          lcd.print(" Bewegungsmelder");
          lcd.setCursor(0, 2);
          lcd.print(" Kalibrierung");
          lcd.setCursor(0, 3);
          if (bew_poti) {
            lcd.print(">Bew.-Meld-Poti aus");
          } else {
            lcd.print(">Bew.-Meld-Poti an");
          }
          break;
      }
    }

    void normal() { // First Menu / only DISPLAYS normal menu
      Serial.println("Normal Menu updte");
      switch (NormalC) {
        case 1: // An
          Serial.println("MENU NORMAL Case 1"); // Prints it in Console for overview
          lcd.clear();
          lcd.print(">An");
          lcd.setCursor(0, 1);
          lcd.print(" Aus");
          lcd.setCursor(0, 2);
          lcd.print(" Schlafmodus");
          break;

        case 2: // Aus
          Serial.println("MENU NORMAL Case 2"); // Prints it in Console for overview
          lcd.clear();
          lcd.print(" An");
          lcd.setCursor(0, 1);
          lcd.print(">Aus");
          lcd.setCursor(0, 2);
          lcd.print(" Schlafmodus");
          break;

        case 3: // Schlafmodus
          Serial.println("MENU NORMAL Case 3"); // Prints it in Console for overview
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


Menu menu; // Declares Menu class


class Bewegungsmelder { // Contains everything connected with the motion detector
  private:
    const int lichtvergleichswert = 5;
    int sensorwert;
    int entfernung1;
    int entfernung2;
    int entfernung3;
    bool returnvalue;

    bool sort(int arr[]) { // Usses Insertion Sort to sort the array
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
        /* Takes the highest and the lowest value, calculates difference and compares them to some value
          If the difference is smaller than 3 it returns true */
        return true;
      } else {
        // Else it returns false
        return false;
      }
    }

  public:
    float entfernung_abfrage() { // Takes output from sound sensor
      Serial.println("BEWEGUNGSMELDER entfernungs_abfage");
      digitalWrite(trigger, LOW);
      delay(5);
      digitalWrite(trigger, HIGH);
      delay(10);
      digitalWrite(trigger, LOW);
      dauer = pulseIn(echo, HIGH);
      entfernung = (dauer / 2) * 0.03432; //Convert distance into cm
      return entfernung; // Returns distance
    }

    bool licht_abfrage() { // If it's dark it returns true
      sensorwert = analogRead(licht);
      if (sensorwert < lichtvergleichswert) {
        return true;
      } else {
        return false;
      }
    }


    bool kalibrierung() {
      lcd.clear();
      lcd.print("kalibriert..."); // Prints "kalibriert" onto display

      delay(200);
      entfernung1 = entfernung_abfrage();
      delay(1000);
      entfernung2 = entfernung_abfrage();
      delay(1000);
      entfernung3 = entfernung_abfrage();
      // Takes a distance 3 times


      int list[] = {entfernung1, entfernung2, entfernung3}; // Compresses distances into list


      returnvalue = sort(list);


      if (returnvalue) {
        kalibrierwert = (entfernung1 + entfernung2 + entfernung3) / 3;
        return true;
      } else {
        return false;
      }
    }
};


Bewegungsmelder bm; // Declares "Bewegungsmelder" class


class Controller { // Controls the motion sensor and Potentiometer (if activated)
  private:
    bool ledcheck = false;
    int previouswert;
    int wert;
    bool licht;
    float distance;

    void Potimeter() { // Sets Led Brightness equal to Potentiometer value
      Serial.println("CONTOLLER Potentiometer");
      wert = analogRead(poti) / 4 + 0.75;
      if (wert - previouswert < -2 || wert - previouswert > 2) { // If the value is different it resets timer for autosleep mode
        autosleep = millis();
      }

      previouswert = wert;
      if (wert >= 247) {
        /* if value too high turn led off
          because otherwise it would flicker a bit */
        digitalWrite(led1, HIGH);
        digitalWrite(led2, HIGH);
      } else {
        analogWrite(led1, wert);
        analogWrite(led2, wert);
      }
    }

    void Bewegungsmelder() {
      /* Controls sound sensor
        if it's dark and the measured distance is smaller than the calibratet distance it turns lights on for 10 seconds */
      Serial.println("CONTROLLER Bewegungsmelder");
      if (ledcheck) { // If motion-sensor was triggered
        if (bew_poti) {
          Potimeter();
        }
        if (millis() - timestamp >= 10000) {
          digitalWrite(led1, HIGH);
          digitalWrite(led2, HIGH);
          ledcheck = false;
        }
      }

      licht = bm.licht_abfrage();
      distance = bm.entfernung_abfrage();

      if  (licht && distance < kalibrierwert) {
        if (!bew_poti) {
          digitalWrite(led1, LOW);
          digitalWrite(led2, LOW);
        }
        timestamp = millis();
        ledcheck = true; // Sets var to true so the func checks 10 sec counter
      }
    }

  public:
    void Init() { // Calls funcions above
      if (regler) {
        Potimeter();
      }
      if (modus) {
        Bewegungsmelder();
      }
    }
};


Controller controller; // Declares Controller class


class Actions {
  private:
    bool kalibriert = false; // Used to check if the sensor was already "calibrated"
    bool loopB = false; // We use a bool to controll the wile loop so we can break out of 2 loops "at the same time"
    bool second = false; // used to track the funcmode menu state

    void indicatorcheck() { // Turns lights for Potentiometer and sound sensor on and off. Based on if it's on or off
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


    void Regler() { // Turns Potentiometer on and off
      modus  = false; // Sets variable for sound sensor to false
      regler = not(regler);
      if (!regler) {
        digitalWrite(led1, HIGH);
        digitalWrite(led2, HIGH);
        ledpwm = 240; // used to track status of the leds
      }
      indicatorcheck(); // checks indicator leds
    }

    void Melder() { // Turns sound sensor on or off
      if (kalibriert) { // if sound sensor was already calibrated
        digitalWrite(led1, HIGH); // turns leds off
        digitalWrite(led2, HIGH);
        ledpwm = 240; // used to track status of leds

        regler = false;
        modus = not(modus);
        indicatorcheck();
      } else { // gets activated if sound sensor was not calibrated
        lcd.clear();
        lcd.print("Der Bewegungsmelder");
        lcd.setCursor(0, 1);
        lcd.print("wurde noch nicht");
        lcd.setCursor(0, 2);
        lcd.print("kalibriert.");
        delay(2000);
        menu.extended(); // updates second menu
      }
    }

  public:
    void sleepmode() {
      lcd.clear(); // clears display
      lcd.noBacklight(); // Turns display off

      digitalWrite(iregler, LOW); // Turns indicators off
      digitalWrite(ibewegungsmelder, LOW);
      delay (t);
      irrecv.resume(); // Continues to receive infrared light

      while (true) { // goes into a while loop
        Serial.println("SLEEPMODE");
        controller.Init(); // Checks Potentiometer and sound sensor
        if (digitalRead(button)) {
          Serial.println("SLEEPMODE Button Exit");
          autosleep = millis(); // resets timer for autosleepmode
          break;
        }
        if (irrecv.decode(&results)) { // if infrared light is received
          if (results.value == 0xFF02FD) { // if >|| button is pressed
            Serial.println("SLEEPMODE Remote Exit");
            autosleep = millis(); // resets timer for autosleepmode
            irrecv.resume(); // Continues to receive infrared light
            break;
          }
        }
      }
      //Gets triggered when you exit sleepmode
      lcd.backlight(); // Turns display on
      indicatorcheck(); // checks indicators
      menu.normal(); // updates normal menu
      delay(t);
      //
    }

    void autosleepCheck() { // after 2m without any actions the arduino goes into sleepmode automatically
      if (millis() - autosleep >= 120000) {
        sleepmode();
      }
    }

    void funcmode() { // activates Potentiometer, sound sensor and calibration via remote control (or disables it)
      lcd.clear(); // clears display
      // Prints options that you have in funcmode
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
      if (bew_poti) {
        lcd.print("4- Bew.-Meld-Poti aus");
      } else {
        lcd.print("4- Bew.-Meld-Poti an");
      }
      delay(400);
      irrecv.resume(); // Contines to receive infrared light
      loopB = true;
      Serial.println("Im FUNKTIONSMODUS");
      //

      while (loopB) {
        if (digitalRead(button)) { // Exits out without doing anything
          Serial.println("FUNKTIONSMODUS button exit");
          if (ExtendedB) {
            menu.extended();
          } else {
            menu.normal();
          }
          loopB = false;
        }
        if (irrecv.decode(&results)) {
          switch (results.value) {
            case 0xFF30CF: // remote button 1
              // turns Potentiometer on or off
              Serial.println("FUNKTIONSMODUS 1-Taste");
              Regler();
              delay(t);
              if (ExtendedB) { // updates display
                menu.extended();
              } else {
                menu.normal();
              }
              loopB = false;
              Serial.println("loopB = false");
              Serial.println("break;");
              break;

            case 0xFF18E7: // remote button 2
              // turns sound sensor on or off
              Serial.println("FUNKTIONSMODUS 2-Taste");
              Melder();
              delay(t);
              if (ExtendedB) { // updates display
                menu.extended();
              } else {
                menu.normal();
              }
              Serial.println("menu.extended()");
              loopB = false;
              Serial.println("loopB = false");
              Serial.println("break;");
              break;

            case 0xFF7A85: // remote button 3
              // goes into calibration if sound sensor is off
              Serial.println("FUNKTIONSMODUS 3-Taste");
              if (modus) {
                lcd.clear();
                lcd.print("Schalten sie");
                lcd.setCursor(0, 1);
                lcd.print("Bewegungsmelder aus");
              } else {
                kalibriert = bm.kalibrierung();
                if (kalibriert) { // if calibration was sucesfull
                  lcd.clear();
                  lcd.print("Kalibrierung");
                  lcd.setCursor(0, 1);
                  lcd.print("erfolgreich");
                } else { // if calibration failed
                  lcd.clear();
                  lcd.print("Kalibrierung");
                  lcd.setCursor(0, 1);
                  lcd.print("fehlgeschlagen");
                }
              }
              delay(2000);
              if (ExtendedB) { // updates display
                menu.extended();
              } else {
                menu.normal();
              }
              loopB = false;
              Serial.println("loopB = false");
              Serial.println("break;");
              break;

            case 0xFF10EF: // remote button 4
              bew_poti = not(bew_poti);
              menu.extended();
              digitalWrite(iregler, HIGH);
              delay(400);
              digitalWrite(iregler, LOW);
              delay(200);
              indicatorcheck();
              loopB = false;
              Serial.println("loopB = false");
              Serial.println("break;");
              break;

            case 0xFFE21D: //remote Func-button
              // Exits out without doing anything
              Serial.println("FUNKTIONSMODUS Func-Taste");
              delay(t);
              if (ExtendedB) { // updates display
                menu.extended();
              } else {
                menu.normal();
              }
              loopB = false;
              Serial.println("loopB = false");
              Serial.println("break;");
              break;

            case 0xFF906F: // Up-Taste
              if (second) {
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
                if (bew_poti) {
                  lcd.print("4- Bew.-Meld-Poti aus");
                } else {
                  lcd.print("4- Bew.-Meld-Poti an");
                }
                second = false;
              } // second end
              break; // UP-Taste Ende

            case 0xFFE01F: // Down-Taste
              if (!second) {
                lcd.clear();
                lcd.print("Func- Exit");
                second = true;
              }
          }
          irrecv.resume(); // continues to receive infrared light
        }
      }
    }


    void extended(int extendedC) { // Actions that are triggered if you press "ok-button" in second/extended Menu
      switch (extendedC) { // Takes extendedC to determine which action to triger
        case 1: // Potentiometer
          Serial.println("Extended Action 1");
          Regler();
          break;

        case 2: // Sound sensor
          Serial.println("Extended Action 2");
          Melder();
          break;

        case 3: // triggers calibration
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
          menu.extended(); // refresh display
          break;

        case 4: // Bew.-Meld-Poti
          bew_poti = not(bew_poti);
          menu.extended();
          digitalWrite(iregler, HIGH);
          delay(400);
          digitalWrite(iregler, LOW);
          delay(200);
          indicatorcheck();
          break;

      }
    }

    void normal(int normalC) { // Actions that are triggered if you press "ok-button" in first/normal Menu
      switch (normalC) { // Takes normalC to determine which action to trigger
        case 1: // turns leds on
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

        case 2: // turns leds off
          Serial.println("Normal Action 2");
          led = false;
          regler = false;
          modus = false;
          indicatorcheck();
          ledpwm = 60;
          digitalWrite(led1, HIGH);
          digitalWrite(led2, HIGH);
          break;

        case 3: // turns sleepmode on
          Serial.println("Normal Action 3");
          sleepmode();
          break;
      }
    }
};


Actions actions; // Declares Action class


class InputManager { // Controlls Inputs and triggers actions
  private:
    void remoteControl(int code) { // Controls remote control inputs
      switch (code) {
        case 0xFFA25D: // Power-button
          // Turns leds on or off (depends on led status)
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

        case 0xFFE21D: // Funktions-Taste
          // Goes into function mode
          Serial.println("Funktions Taste");
          irrecv.resume();
          actions.funcmode();
          delay(t);
          break;

        case 0xFFE01F: //Down-Taste
          // Dimms led
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
          // brightens led
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
          // Goes into sleepmode
          Serial.println("Pause Weiter");
          actions.sleepmode();
          delay(t);
          break;

        // Hier
        // könnte
        // vielleicht
        // Code
        // stehen

        case 0xFF30CF: // 1
          // Led almost off
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
          // Led mid brightness
          if (modus == false && regler == false) {
            Serial.println("2 Taste");
            analogWrite(led1, 160);
            analogWrite(led2, 160);
            ledpwm = 180;
          }
          delay(t);
          break;

        case 0xFF7A85: // 3
          // Led almost max brightness
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
          // Turns sound sensor and Potentiometer off
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
    void Init() { // initializes "InputManager"
      if (irrecv.decode(&results)) {
        Serial.print("Infrarot Messung: ");
        Serial.println(results.value);
        delay(t);
        irrecv.resume();
        autosleep = millis(); // resets autosleepmode timer
        remoteControl(results.value);
      }


      int y = analogRead(y_pin);
      int x = analogRead(x_pin);


      if (ExtendedB) {
        if (y > 900) {
          autosleep = millis();
          Serial.println("Extended -> Normal");
          ExtendedB = false;
          NormalB = true;
          if (ExtendedC <= 3) {
            NormalC = ExtendedC;
          } else {
            NormalC = 3;
          }
          menu.normal();
          while (analogRead(y_pin) > 900);
        }
        if (x > 900 && ExtendedC != 4) {
          autosleep = millis();
          Serial.println("Extended Down");
          ExtendedC ++;
          menu.extended();
          while (analogRead(x_pin) > 900);
        } else if (x < 250 && ExtendedC != 1) {
          autosleep = millis();
          Serial.println("Extended Up");
          ExtendedC --;
          menu.extended();
          while (analogRead(x_pin) < 250);
        }
        if (digitalRead(button)) {
          autosleep = millis();
          Serial.println("Extended Button");
          actions.extended(ExtendedC);
          while (digitalRead(button));
        }
      } else {
        if (y < 100) {
          autosleep = millis();
          Serial.println("Normal -> Extended");
          NormalB = false;
          ExtendedB = true;
          ExtendedC = NormalC;
          menu.extended();
          while (analogRead(y_pin) < 100);
        }
        if (x > 900 && NormalC != 3) {
          autosleep = millis();
          Serial.println("Normal Down");
          NormalC ++;
          menu.normal();
          while (analogRead(x_pin) > 900);
        } else if (x < 250 && NormalC != 1) {
          autosleep = millis();
          Serial.println("Normal Up");
          NormalC --;
          menu.normal();
          while (analogRead(x_pin) < 250);
        }
        if (digitalRead(button)) {
          autosleep = millis();
          Serial.println("Normal Button");
          actions.normal(NormalC);
          while (digitalRead(button));
        }
      }
    }
};


InputManager inputmanager; // Declares InputManager


void setup() { // Declares pinMode and initializes display
  Serial.begin(9600);
  lcd.begin();
  lcd.backlight();

  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(x_pin, INPUT);
  pinMode(y_pin, INPUT);

  pinMode(button, INPUT);
  pinMode(licht, INPUT);
  timestamp = millis();

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  digitalWrite(led1, HIGH);
  digitalWrite(led2, HIGH);

  menu.normal();
  irrecv.enableIRIn();
}


void loop() { // Calls InputManager, Controller and
  inputmanager.Init();
  controller.Init();
  actions.autosleepCheck();
}
