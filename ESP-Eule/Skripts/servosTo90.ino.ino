#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);  //I2C 

#define minPulseWidth 600
#define maxPulseWidth 2400
#define freq 50
#define servoEyes 0   //Servo für die Augen steckt im ersten Slot
#define servoBeak 1   //Servo für den Schnabel steckt im zweiten Slot
#define servoHead 2   //Servo für den Nacken steckt im dritten Slot
#define servoWings 3  //Servo für die Flügel steckt im vierten Slot

//Umrechnung von Pulsweite auf Winkel
int servoAngle(int angle){

  //Wir übertragen die Pulsweite auf die Winkel 0 - 180 Grad,
  //damit wir die Servos bequemer ansteuern können.
  int pulseWidth, analogValue;
  pulseWidth = map(angle, 0, 180, minPulseWidth, maxPulseWidth); 
  analogValue = int(float(pulseWidth)/1000000 * freq * 4096);    
  return analogValue;
   
}


void setup() {
  
  Serial.begin(9600);

  pwm.begin();
  pwm.setPWMFreq(freq);

  //Diese Befehle stellen alle Servos auf 90 Grad ein, also die Mittelstellung
  pwm.setPWM(servoEyes, 0, servoAngle(90));
  delay(1000);
  pwm.setPWM(servoBeak, 0, servoAngle(90));
  delay(1000);
  pwm.setPWM(servoHead, 0, servoAngle(90));
  delay(1000);
  pwm.setPWM(servoWings, 0, servoAngle(90));
  delay(1000);
  
}

void loop() {

}
