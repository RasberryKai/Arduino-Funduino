#include <IRremote.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

//Pins
const int IRremotePin = 2;
const int trigger = 9;
const int echo = 8;
const int x_pin = A1;
const int y_pin = A0;
const int button = 10;
const int led1 = 6;
const int led2 = 5;
const int lightsensor = A3;
const int poti = A2;
//Pins

class Menu{
  private:
  int NormalC = 1;
  int ExtendedC;

  public:
  void updateMenu(){
    switch(NormalC){
      case 1:
        lcd.clear();
        lcd.print(">An");
        lcd.setCursor(0, 1);
        lcd.print(" Aus");
        lcd.setCursor(0, 2);
        lcd.print(" Potentiometer");
        lcd.setCursor(0, 3);
        lcd.print(" Bewegungsmelder");
        break;
      case 2:
        lcd.clear();
        lcd.print(" An");
        lcd.setCursor(0, 1);
        lcd.print(">Aus");
        lcd.setCursor(0, 2);
        lcd.print(" Potentiometer");
        lcd.setCursor(0, 3);
        lcd.print(" Bewegungsmelder");
        break;
      case 3:
        lcd.clear();
        lcd.print(" An");
        lcd.setCursor(0, 1);
        lcd.print(" Aus");
        lcd.setCursor(0, 2);
        lcd.print(" Potentiometer");
        lcd.setCursor(0, 3);
        lcd.print(">Bewegungsmelder");
        break;
      case 4:
        lcd.clear();
        lcd.print(" An");
        lcd.setCursor(0, 1);
        lcd.print(" Aus");
        lcd.setCursor(0, 2);
        lcd.print(">Potentiometer");
        lcd.setCursor(0, 3);
        lcd.print(" Bewegungsmelder");
        break;
    }
  }

  void increaseCount(){
    NormalC ++;
    updateMenu();
  }

  void decreaseCount(){
    NormalC --;
    updateMenu();
  }

  int getCount(){
    return NormalC;
  }
};

Menu menu;

class SoundSensor{
  private:
  int distances[3];
  int calibratedValue;
  long dauer = 0;
  long entfernung = 0;

  void sort(){
    //Here we sort the distances array
    for (int i  = 1; i < 3; i++) {
        int key = distances[i];
        int j = i - 1;
        while (j >= 0 && key < distances[j]) {
          distances[j + 1] = distances[j];
          j -= 1;
        }
        distances[j + 1] = key;
      }
  }

  bool getLight(){
    if (analogRead(lightsensor) < 5){
      return true;
    } else{
      return false;
    }
  }

  float getDistance(){
    //This returns the distance measured from the sound sensor
    digitalWrite(trigger, LOW);
    delay(5);
    digitalWrite(trigger, HIGH);
    delay(10);
    digitalWrite(trigger, LOW);
    dauer = pulseIn(echo, HIGH);
    entfernung = (dauer / 2) * 0.03432;
    return entfernung;
  }

  public:
  bool calibration(){
    //If this function is called it calibrates the sound sensor
    //And returns if it was successfull
    lcd.clear();
    delay(200);
    distances[0] = getDistance();
    delay(500);
    distances[1] = getDistance();
    delay(500);
    distances[2] = getDistance();
    sort();

    if (distances[2] - distances[0] < 3){
      calibratedValue = (distances[0] + distances[1] + distances[2]) / 3;
      return true;
    } else {
      return false;
    }
  }//calibration-end

  bool isCalibrated(){
    if (calibratedValue == 0){
      return false;
    } else{
      return true;
    }
  }

   bool sensorCheck(){
    //Here we check if we got any movement
    //(Can only be called after calibration)
    if (getDistance() < calibratedValue && getLight()){
      return true;
    } else{
      return false;
    }
  }
};

SoundSensor soundsensor;

class Administrator{
  private:


  public:
  void controllerInput(){

  }

  void remoteInput(){
    
  }
};

Administrator admin;

class LEDControl{
  private:
  bool last = false;

  public:
  void on(){
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    last = true;
  }

  void off(){
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
    last = false;
  }

  void turn(){
    if (last){
      off();
    } else{
      on();
    }
  }

  void analog(int value){
    analogWrite(led1, value);
    analogWrite(led2, value);
  }
};

LEDControl ledcontrol;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  // pin decleration
  IrReceiver.begin(IRremotePin, ENABLE_LED_FEEDBACK);
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(x_pin, INPUT);
  pinMode(y_pin, INPUT):
  pinMode(button, INPUT);
  pinMode(licht, INPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  digitalWrite(led1, HIGH);
  digitalWrite(led2, HIGH);
  menu.updateMenu();
}

void loop() {
  // put your main code here, to run repeatedly:

}
