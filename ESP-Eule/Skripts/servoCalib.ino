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

#define eyesMin 90    //ausgehend von 90 muss dieser Wert schrittweise verkleinert werden
#define eyesMax 90    //ausgehend von 90 muss dieser Wert schrittweise vergrößert werden

#define beakMin 90    //ausgehend von 90 muss dieser Wert schrittweise verkleinert werden
#define beakMax 90    //ausgehend von 90 muss dieser Wert schrittweise vergrößert werden

#define headMid 90    //der Kopf wird nach dem Zusammenschrauben nicht genau mittig sitzen. Diesen Wert anpassen, um die Mittelposition zu finden.

#define wingsMin 90   //ausgehend von 90 muss dieser Wert schrittweise verkleinert werden
#define wingsMax 90   //ausgehend von 90 muss dieser Wert schrittweise vergrößert werden

//Umrechnung zum Ansteuern der Winkel
int servoAngle(int angle){

  int pulseWidth, analogValue;
  pulseWidth = map(angle, 0, 180, minPulseWidth, maxPulseWidth); //Wir übertragen die Pulsweite auf die Winkel 0 - 180 Grad
  analogValue = int(float(pulseWidth)/1000000 * freq * 4096);    
  return analogValue;
  
}

void setup() {
    
  Serial.begin(115200);

  pwm.begin();
  pwm.setPWMFreq(freq);

  //Den Servo nach jedem Übertragen des Sketches schrittweise an das eine äußere Ende bewegen.
  //Nachdem eyesMin eingestellt ist, eyesMin durch eyesMax ersetzen und in die andere Richtung bewegen.
  //Danach servoEyes durch servoBeak ersetzen und eyesMax durch beakMin.
  //Diesen Vorgang solange wiederholen, bis alle Min- und Max-Werte ermittelt sind. 
  //Die Servos nach Möglichkeit nicht ganz bis zum Anschlag bewegen und etwas Spiel lassen.
  pwm.setPWM(servoEyes, 0, servoAngle(eyesMin));     //sobald eyesMin ermittelt ist, eyesMin durch eyesMax ersetzen
  
  delay(1000);
  
  //PWM-Signal für alle Servos deaktivieren, damit die Servos nach dem Erreichen des Winkels lockerlassen
  pwm.setPWM(servoEyes, 0, 4096);
  pwm.setPWM(servoBeak, 0, 4096);
  pwm.setPWM(servoHead, 0, 4096);
  pwm.setPWM(servoWings, 0, 4096);

}

void loop() {
  

}
