#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);  //I2C 

//AUDIOCONFIG
static int8_t commandArray[8] = {0x7E,0xFF,0x06,0,0x00,0,0,0xEF};

#define sndPlayFolderFile 0X0F
#define sndSelectDevice 0X09
#define sndDevTF 0X02
#define sndSetVolume 0X06

#define minPulseWidth 600
#define maxPulseWidth 2400
#define freq 50
#define servoEyes 0   //Servo für die Augen steckt im ersten Slot
#define servoBeak 1   //Servo für den Schnabel steckt im zweiten Slot
#define servoHead 2   //Servo für den Nacken steckt im dritten Slot
#define servoWings 3  //Servo für die Flügel steckt im vierten Slot


/* Die Min- und Max-Werte durch die mit servoCalib.ino ermittelten ersetzen.
 *  Die Min-Werte sollten die kleineren sein, die Max-Werte die größeren, 
 *  weil ansonsten die programmierten Bewegungen nicht funktionieren.
 */
#define eyesMin 90    
#define eyesMax 90    

#define beakMin 90    
#define beakMax 90    

#define headMin 0
#define headMid 90 
#define headMax 180    

#define wingsMax 90  
#define wingsMin 90  

//aktuelle Servo-Positionen festlegen
int eyesPos = eyesMin;
int beakPos = beakMin;
int headPos = headMid;
int wingsPos = wingsMax;


//Umrechnung zum Ansteuern der Winkel
int servoAngle(int angle){

  int pulseWidth, analogValue;
  pulseWidth = map(angle, 0, 180, minPulseWidth, maxPulseWidth); //Wir übertragen die Pulsweite auf die Winkel 0 - 180 Grad
  analogValue = int(float(pulseWidth)/1000000 * freq * 4096);    //Daraus generieren wir ein 12-Bit PWM-Signal
  return analogValue;

}

void esp2mp3(byte func, byte param1, byte param2){

  commandArray[3] = func;
  commandArray[5] = param1;
  commandArray[6] = param2;

  for(uint8_t i=0; i<8; i++){
    Serial2.write(commandArray[i]);
  }
}

void servoStartUpReset() {

  pwm.setPWM(servoEyes, 0, servoAngle(eyesPos));
  pwm.setPWM(servoBeak, 0, servoAngle(beakPos));
  pwm.setPWM(servoHead, 0, servoAngle(headPos));
  pwm.setPWM(servoWings, 0, servoAngle(wingsPos));
  Serial.println("Alle Servos in Position.");

}

/********************/
/* GRUNDANIMATIONEN */
/********************/

/* FLÜGEL HOCH */
void wingsUp(int aniSpeed) {

  if (wingsPos>=wingsMin){
    for (int i=wingsPos; i>=wingsMin; i--){
      pwm.setPWM(servoWings, 0, servoAngle(i));
      delay(aniSpeed);
      Serial.println(i);
    }
    wingsPos = wingsMin;
    Serial.println("Wings Position");
    Serial.println(wingsPos);
    pwm.setPWM(servoWings, 0, 4096);
  }
  
}

/* FLÜGEL RUNTER */
void wingsDown(int aniSpeed) {

  if (wingsPos<=wingsMax){
    for (int i=wingsPos; i<=wingsMax; i++){
      pwm.setPWM(servoWings, 0, servoAngle(i));
      delay(aniSpeed);
    }
    wingsPos = wingsMax;
    Serial.println("Wings Position");
    Serial.println(wingsPos);
    pwm.setPWM(servoWings, 0, 4096);
  }

}

/* AUGEN AUF */
void eyesOpen(int aniSpeed) {
  if (eyesPos>=eyesMin){
    for (int i=eyesPos; i>=eyesMin; i--){
      pwm.setPWM(servoEyes, 0, servoAngle(i));
      delay(aniSpeed);
    }
    eyesPos = eyesMin;
    Serial.println("Eyes Position");
    Serial.println(eyesPos);
    pwm.setPWM(servoEyes, 0, 4096);
  }
}


/* AUGEN ZU */
void eyesShut(int aniSpeed) {
  if (eyesPos<=eyesMax){
    for (int i=eyesPos; i<=eyesMax; i++){
      pwm.setPWM(servoEyes, 0, servoAngle(i));
      delay(aniSpeed);
    }
    eyesPos = eyesMax;
    Serial.println("Eyes Position");
    Serial.println(eyesPos);
    pwm.setPWM(servoEyes, 0, 4096);
  }
}


/* BLINZELN
 * Die Funktion eyesBlink() hat keine Variable aniSpeed, weil das Blinzeln 
 * mit maximaler Geschwindigkeit ablaufen soll. Man könnte aber, wie bei den 
 * anderen Funktionen noch aniSpeed noch ergänzen.
*/
void eyesBlink(){
  pwm.setPWM(servoEyes, 0, servoAngle(eyesMin));
  delay(100);
  pwm.setPWM(servoEyes, 0, servoAngle(eyesMax));
  delay(100);
  pwm.setPWM(servoEyes, 0, servoAngle(eyesMin));
  delay(100);
  pwm.setPWM(servoEyes, 0, 4096);
}


/* SCHNABEL AUF */
void beakOpen(int aniSpeed) {
  if (beakPos<=beakMax){
    for (int i=beakPos; i<=beakMax; i++){
      pwm.setPWM(servoBeak, 0, servoAngle(i));
      delay(aniSpeed);
    }
    beakPos = beakMax;
    Serial.println("Beak Position");
    Serial.println(beakPos);
    pwm.setPWM(servoBeak, 0, 4096);
  }
}


/* SCHNABEL ZU */
void beakClose(int aniSpeed) {
  if (beakPos>beakMin){
    for (int i=beakPos; i>=beakMin; i--){
      pwm.setPWM(servoBeak, 0, servoAngle(i));
      delay(aniSpeed);
    }
    beakPos = beakMin;
    Serial.println("Beak Position");
    Serial.println(beakPos);
    pwm.setPWM(servoBeak, 0, 4096);
  }
}

/* KOPF NACH RECHTS DREHEN */
void headRight(int aniSpeed) {
  //pwm.setPWM(servoHead, 0, headPos);
  
  if (headPos>=headMin){
    for (int i=headPos; i>=headMin; i--){
      pwm.setPWM(servoHead, 0, servoAngle(i));
      delay(aniSpeed);
    }
    headPos = headMin;
    Serial.println("Head Position");
    Serial.println(headPos);
    pwm.setPWM(servoHead, 0, 4096);
  }
}

/* DEN KOPF NACH LINKS DREHEN */
void headLeft(int aniSpeed) {
  //pwm.setPWM(servoHead, 0, headPos);
    
  if (headPos<=headMax){
    for (int i=headPos; i<=headMax; i++){
      pwm.setPWM(servoHead, 0, servoAngle(i));
      delay(aniSpeed);
    }
    headPos = headMax;
    Serial.println("Head Position");
    Serial.println(headPos);
    pwm.setPWM(servoHead, 0, 4096);
  }
}

/*DEN KOPF ZUR MITTE DREHEN */
void headCenter(int aniSpeed) {
  //pwm.setPWM(servoHead, 0, headPos);
    
  if (headPos<=headMid){
    for (int i=headPos; i<=headMid; i++){
      pwm.setPWM(servoHead, 0, servoAngle(i));
      delay(aniSpeed);
    } 
    headPos = headMid;
    Serial.println("Head Position");
    Serial.println(headPos);
    pwm.setPWM(servoHead, 0, 4096);
    
  } else if (headPos>=headMid){
    for (int i=headPos; i>=headMid; i--){
      pwm.setPWM(servoHead, 0, servoAngle(i));
      delay(aniSpeed);
    }
    headPos = headMid;
    Serial.println("Head Position");
    Serial.println(headPos);
    pwm.setPWM(servoHead, 0, 4096);
  }
}


/* DEN KOPF ZU EINEM ZUFÄLLIGEN WINKEL DREHEN */
void headRand(int aniSpeed) {
  //pwm.setPWM(servoHead, 0, headPos);
  int randAngle = random(headMin+5, headMax-5);

  if (headPos<=randAngle){
    for (int i=headPos; i<=randAngle; i++){
      pwm.setPWM(servoHead, 0, servoAngle(i));
      delay(aniSpeed);
    }
    headPos = randAngle;
    Serial.println("Head Position");
    Serial.println(headPos);
    pwm.setPWM(servoHead, 0, 4096);

  } else if (headPos>=randAngle){
    for (int i=headPos; i>=randAngle; i--){
      pwm.setPWM(servoHead, 0, servoAngle(i));
      delay(aniSpeed);
    }
    headPos = randAngle;
    Serial.println("Head Position");
    Serial.println(headPos);
    pwm.setPWM(servoHead, 0, 4096);
  }
}


/* BEWEGUNGSABLAUF DER MAKER FAIRE 2022
 *  Ihr könnt so auch eigene Bewegungsabläufe schaffen.
 *  Im void loop() wird lediglich owlMF() aufgerufen
 *  und spielt die darin enthaltenen Bewegungen ab.
 */
void owlMF(){

  eyesOpen(20);
  delay(1000);
  eyesBlink();  //Hier keinen Wert in die Klammer eintragen, da eyesBlink() keine Variable aniSpeed hat.
  delay(random(1000,2000));
  headRand(random(5,20));
  delay(random(1000,3000));

  //Zufällige Flügelbewegung
  int flap = random(0,2);
  if (flap==1){
    wingsUp(random(5,20));
    delay(random(0,2000));
    wingsDown(random(5,20));
    delay(random(1000,2000));
    eyesBlink();
  }

  //Zufällige Schnabelbewegung
  int chatter = random(0,2);
  if (chatter==1){
    beakOpen(random(5,20));
    delay(random(0,2000));
    beakClose(random(5,20));
    delay(random(1000,2000));
    eyesBlink();
  }

  //Zufällig schlafen
  int sleeping = random(0,2);
  if (sleeping==1){
    delay(2000);
    beakOpen(20);
    eyesShut(20);
    beakClose(30);
    delay(random(3000,20000));
    eyesOpen(20);
    delay(random(1000,2000));
    eyesBlink();
  }

  //Zufällige Animation
  int widerstand = random(0,3);
  if (widerstand==1){
    beakOpen(random(5,20));
    delay(1000);
    esp2mp3(sndPlayFolderFile, 1, 3);
    delay(2000);
    beakClose(random(5,20));
    delay(random(1000,2000));
    eyesBlink();
  }
  
}


void setup() {
  
  Serial.begin(115200);
  Serial.println("Hello");
  pwm.begin();
  pwm.setPWMFreq(freq);

  //Verbindung zum YX5300 herstellen
  Serial2.begin(9600, SERIAL_8N1, 16, 17);
  esp2mp3(sndSelectDevice, 0, sndDevTF);
  delay(200);

  //Lautstärke einstellen 0xD2 = 210 (ca. 82 Prozent bei max. 255)
  esp2mp3(sndSetVolume, 0, 0xD2);
  delay(200);

  //StartUp-Sound abspielen
  esp2mp3(sndPlayFolderFile, 1, 1);
  delay(3000);

  servoStartUpReset();  //Setzt alle Servos auf 90 und den Kopf in Mittelstellung

 
  
  
  /******************************************************************/
  /* MIN-MAX-TESTS                                                  */
  /* Die einzelnen Servos sollten nacheinander in unterschiedlichen */
  /* Geschwindigkeiten die Min- und Max-Werte ansteuern.            */
  /******************************************************************/


  /* FLÜGEL-TESTS */
  wingsUp(20);    //Der Wert in der Klammer wird an die Funktion als aniSpeed übergeben und steuert die Zeit des Delay in Millisekunden.
  wingsDown(20);
  wingsUp(20);
  wingsDown(20);
  wingsUp(10);
  wingsDown(10);
  wingsUp(10);
  wingsDown(10);
  wingsUp(5);
  wingsDown(5);


  /* AUGEN-TESTS */
  eyesOpen(20);
  eyesShut(20);
  eyesOpen(20);
  eyesShut(20);
  eyesOpen(10);
  eyesShut(10);
  eyesOpen(5);
  eyesShut(5);
  eyesOpen(1);
  eyesShut(1);

  delay(2000);
  eyesBlink();
  

  /* KOPF-DREH-TESTS */
  delay(2000);
  headLeft(20);
  delay(1000);
  headCenter(20);
  delay(1000);
  headRight(20);
  delay(1000);
  headCenter(20);
  delay(1000);
  headLeft(20);
  delay(1000);
  headRight(20);
  delay(1000);
  headCenter(20);

  delay(2000);
  headLeft(10);
  delay(1000);
  headCenter(10);
  delay(1000);
  headRight(10);
  delay(1000);
  headCenter(10);

  delay(2000);
  headLeft(5);
  delay(1000);
  headCenter(5);
  delay(1000);
  headRight(5);
  delay(1000);
  headCenter(5);


  /* SCHNABEL-TESTS */
  beakOpen(20);
  delay(1000);
  beakClose(20);
  delay(1000);
  beakOpen(10);
  delay(1000);
  beakClose(10);
  delay(1000);
  beakOpen(5);
  delay(1000);
  beakClose(5);
  delay(1000);
  beakOpen(2);
  delay(1000);
  beakClose(2);
  delay(1000);
  
}


void loop() {
  //owlMF();

}
