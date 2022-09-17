#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#include <WiFi.h>
#include <ESP_Mail_Client.h>


/**********************/
/* WLAN-EINSTELLUNGEN */
/**********************/

#define WIFI_SSID "hier SSID eintragen"
#define WIFI_PASSWORD "hier Passwort eintragen"


/************************/
/* E-MAIL-EINSTELLUNGEN */
/************************/

#define IMAP_HOST "imap.gmail.com"
#define IMAP_PORT 993
#define AUTHOR_EMAIL "emailAdresse@gmail.com"
#define AUTHOR_PASSWORD "App-Passwort hier eintragen"

IMAPSession imap;
ESP_Mail_Session session;
IMAP_Config config;
#define checkFreq 60000  //Einmal pro Minute E-Mails abrufen


/***********************/
/* AUDIO-EINSTELLUNGEN */
/***********************/

//Das Array für die serielle Kommunikation zwischen ESP32 und YX5300
static int8_t commandArray[8] = {0x7E,0xFF,0x06,0,0x00,0,0,0xEF};

//Konstanten für die Befehle, die wir benötigen. Weitere gibt es im Handbuch des YX5300.
#define sndPlayFolderFile 0X0F
#define sndSelectDevice 0X09
#define sndDevTF 0X02
#define sndSetVolume 0X06

//Sounds definieren, damit man sich nicht die Namen merken muss.
#define startUp 1
#define owlHoot 2


/***********************/
/* SERVO-EINSTELLUNGEN */
/***********************/

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);  //I2C 

#define minPulseWidth 600
#define maxPulseWidth 2400
#define freq 50
#define servoEyes 0   //Servo für die Augen steckt im ersten Slot
#define servoBeak 1   //Servo für den Schnabel steckt im zweiten Slot
#define servoHead 2   //Servo für den Nacken steckt im dritten Slot
#define servoWings 3  //Servo für die Flügel steckt im vierten Slot

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

//PWM in Winkel umrechnen
int servoAngle(int angle){

  int pulseWidth, analogValue;

  //Die Pulsweite (600-2400) wird auf die Winkel (0-180) gemappt.
  pulseWidth = map(angle, 0, 180, minPulseWidth, maxPulseWidth);
  //Danach wird daraus ein 12-Bit Wert für den Winkel errechnet.
  analogValue = int(float(pulseWidth)/1000000 * freq * 4096);
  return analogValue;
  
}


/************************/
/* GENERELLE FUNKTIONEN */
/************************/

//Diese Funktion fügt die Funktion und ihre Parameter in den commandArray ein
//und sendet den Befehl seriell an den YX5300-MP3-Player.
void esp2mp3(byte func, byte param1, byte param2){

  commandArray[3] = func;
  commandArray[5] = param1;
  commandArray[6] = param2;

  for(uint8_t i=0; i<8; i++){
    Serial2.write(commandArray[i]);
  }
}


//Servos auf die Ausgangspositionen einstellen
void servoStartUpReset() {

  pwm.setPWM(servoEyes, 0, servoAngle(eyesPos));
  pwm.setPWM(servoBeak, 0, servoAngle(beakPos));
  pwm.setPWM(servoHead, 0, servoAngle(headPos));
  pwm.setPWM(servoWings, 0, servoAngle(wingsPos));
  Serial.println("Alle Servos in Position.");

}


//Diese Funktion verbindet den ESP mit dem WLAN.
void connectWiFi(){
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(200);
  }
  //Serial.println();
  //Serial.print("Zum WLAN-Netzwerk ");
  //Serial.print(WIFI_SSID);
  //Serial.println(" verbunden.");
  //Serial.print("IP-Adresse: ");
  //Serial.println(WiFi.localIP());
  //Serial.println();
}


//Die Hauptfunktion, die E-Mails abruft, löscht und Bewegungen ausführt.
void checkIncoming(){

  // Session config
  session.server.host_name = IMAP_HOST;
  session.server.port = IMAP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;

  // In die Klasse MessageList kommen alle E-Mails, die gelöscht werden sollen.
  MessageList toDelete;
  
  if (!imap.connect(&session, &config))
    return;
  
  imap.selectFolder("INBOX");

  SelectedFolderInfo sFolder = imap.selectedFolder();
  ESP_MAIL_PRINTF("Total Message Index: %d\n", sFolder.msgCount());
  ESP_MAIL_PRINTF("Unseen Message Index: %d\n", sFolder.unseenIndex());

  //Wenn eine Nachricht im Posteingang ist, wird etwas ausgeführt.
  if (imap.selectedFolder().msgCount() > 0){
    owlMF();

    /* Hier können auch andere Abläufe erstellt werden. Ich finde es aber übersichtlicher,
     *  sie in eigene Funktionen wie owlMF() zu verpacken.
     */
    //eyesBlink();
    //wingsUp(50);
  }

  //Alle Nachrichten im Posteingang werden gelöscht.
  for(int i = imap.selectedFolder().msgCount(); i > 0; i--){
    toDelete.add(imap.getUID(i));
  }
  
  imap.deleteMessages(&toDelete, true);

  //Nach der Pause checkFreq wird der Posteingang erneut abgefragt.
  delay(checkFreq);
  
}



/***************************/
/* GRUNDLEGENDE BEWEGUNGEN */
/***************************/

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



/*********************************/
/* ANIMATIONEN UND CHOREOGRAFIEN */
/*********************************/

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




//Eine kleiner Animationsablauf
void animation1(){

  wingsUp(50);
  delay(2000);
  wingsDown(50);
  delay(2000);
  
}

//Eine kleiner Animationsablauf
void animation2(){

  headLeft(30);
  delay(1000);
  headRight(30);
  delay(1000);
  headCenter(100);
  delay(2000);
  
}

//Eine kleiner Animationsablauf
void animation3(){

  eyesOpen(10);
  beakOpen(10);
  esp2mp3(sndPlayFolderFile, 1, 3);
  delay(3000);   //Zeit für den Sound, um zu Ende zu spielen
  beakClose(30);
  eyesShut(100);
  
}

//Diese Funktion spielt zufällig einen der kleinen Animationsabläufe ab
void randAnimation(){

  int whichAnimation = random(0,3);
  if (whichAnimation==0){
    animation1();
  } else if (whichAnimation==1){
    animation2();
  } else if (whichAnimation==2){
    animation3();
  } 
}



/*****************************************/
/* HIER STARTET DAS EIGENTLICHE PROGRAMM */
/*****************************************/

void setup() {

  //Serielle Verbindung zum PC
  Serial.begin(115200);

  //Mit dem WLAN verbinden
  connectWiFi();

  //Servo PWM starten
  pwm.begin();
  pwm.setPWMFreq(freq);

  //Verbindung zum YX5300 herstellen
  Serial2.begin(9600, SERIAL_8N1, 16, 17);
  esp2mp3(sndSelectDevice, 0, sndDevTF);
  delay(200);

  //Lautstärke einstellen 0xD2 = 210 (ca. X Prozent)
  esp2mp3(sndSetVolume, 0, 0xD2);
  delay(200);

  //StartUp-Sound abspielen
  esp2mp3(sndPlayFolderFile, 1, 1);
  delay(3000);

  //Alle Servos auf Mittelstellung setzen
  servoStartUpReset();
  
}

void loop() {

  checkIncoming();

}
