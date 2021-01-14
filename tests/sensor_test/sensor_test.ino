// This is a test I wrote to check if my sensors that I built into my project are working fine
// Checks for: sound sensor, light sensor, Joystick, button, Potentiometer

const int trigger = 9; // set vars for pins
const int echo = 8;
long dauer = 0;
long entfernung = 0;
const int x_pin = A1;
const int y_pin = A0;
const int sw_pin = 10;
const int licht = A3;
const int poti = A2;

void soundsensor() { // measures distance between sound sensor and next object
  digitalWrite(trigger, LOW);
  delay(5);
  digitalWrite(trigger, HIGH);
  delay(10);
  digitalWrite(trigger, LOW);
  dauer = pulseIn(echo, HIGH);
  entfernung = (dauer / 2) * 0.03432; //Convert distance into cm
  Serial.print("Sound Sensor: ");
  Serial.println(entfernung); // Prints distance to console
}

void setup() { // Declare pinModes and initialize Serial Console
  Serial.begin(9600);
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(x_pin, INPUT);
  pinMode(y_pin, INPUT);
  pinMode(sw_pin, INPUT);
  pinMode(licht, INPUT);
  pinMode(poti, INPUT);
}

void loop() { // Prints all important data to the Console
  soundsensor();
  Serial.print("Light sensor: ");
  Serial.println(analogRead(licht));
  Serial.print("Potentiometer: ");
  Serial.println(analogRead(poti));
  Serial.println("Joystick: ");
  Serial.print("x: ");
  Serial.println(analogRead(x_pin));
  Serial.print("y: ");
  Serial.println(analogRead(y_pin));
  Serial.print("Button: ");
  Serial.println(digitalRead(sw_pin));
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println();
  delay(5000);
}
