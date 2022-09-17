//Konstanten für die Befehle, die wir benötigen. Weitere gibt es im Handbuch des YX5300.
#define sndPlayFolderFile 0x0F
#define sndSelectDevice 0x09
#define sndDevTF 0x02
#define sndSetVolume 0x06

//Sounds definieren, damit man sich nicht deren Nummern merken muss
#define startUp 1   //entspricht der Datei 001_startUpSound.mp3
#define owlHoot 2   //entspricht der Datei 002_hoot.wav usw.

//Das Array für die serielle Kommunikation zwischen ESP32 und YX5300
static int8_t commandArray[8] = {0x7E,0xFF,0x06,0,0x00,0,0,0xEF};

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


void setup() {

  //Serielle Verbindung zum PC
  Serial.begin(115200);
  
  //Verbindung zum YX5300 herstellen
  Serial2.begin(9600, SERIAL_8N1, 16, 17);
  esp2mp3(sndSelectDevice, 0, sndDevTF);
  
  //Lautstärke einstellen 0xD2 = 210 (ca. X Prozent)
  esp2mp3(sndSetVolume, 0, 0xD2);

  //Startsound abspielen
  esp2mp3(sndPlayFolderFile, 1, startUp);
  
  //Der Audiodatei Zeit zum Abspielen geben
  delay(3000);

  Serial.println("Sound abgespielt");
  
}


void loop() {

  //Zum Einstellen des Poti am PAM-Verstärker könnt ihr auch 
  //einen Sound in Schleife abspielen.
  
  //esp2mp3(sndPlayFolderFile, 1, 2);
  //delay(2000);

}
