#!/usr/bin/python3

import time
import random
from adafruit_servokit import ServoKit
from time import sleep
from imapclient import IMAPClient
from pydub import AudioSegment
from pydub.playback import play


###########################
### Variablen festlegen ###
###########################

### Variablen fuer den E-Mail-Abruf

HOSTNAME = 'imap.gmail.com'  #oder Hostname eines anderen Anbieters
USERNAME = 'mail@gmail.com' #gebt hier die Gmail-Adresse ein
PASSWORD = 'katze123' #und hier das App-Passwort

#Die Mail-Abfrage in Sekunden. Wenn der Wert zu niedrig ist, kann euch der Mail-
#Server blockieren, also übertreibt es nicht. 5 Sekunden haben bei mir für Tests
#noch gut funktioniert. Im Betrieb muss die Eule nicht alle paar Sekunden nach-
#sehen.

checkFreq = 20


### Variablen für die Servosteuerung

kit = ServoKit(channels=16)
sleepDuration = 1
headMid = 90
headMin = 0
headMax = 180
eyesMin = XX #Minimalwert einsetzen für Augen zu
eyesMax = XX #Maximalwert einsetzen für Augen auf
beakMin = XX #Minimalwert einsetzen für Schnabel zu
beakMax = XX #Maximalwert einsetzen für Schnabel auf
wingsMin = XX #Minimalwert einsetzen für Flügel unten
wingsMax = XX #Maximalwert einsetzen für Flügel oben


### Variablen für die Animationen

headPos = headMid
headAim = headMid
eyesPos = eyesMin
eyesAim = eyesMin
beakPos = beakMin
beakAim = beakMin
wingsPos = wingsMin
wingsAim = wingsMin
aniSpeed = 0.01

### Eulen-Sounds

#Fügt hier eure eigenen Sounds anstatt der angegeben WAVs ein.

sndHoot = AudioSegment.from_wav('owlHoot.wav')
sndHootLong = AudioSegment.from_wav('owlHootLong.wav')
sndCall = AudioSegment.from_wav('owlCall.wav')
sndWakeup = AudioSegment.from_wav('owlWakeup.wav')
#sndSleep = AudioSegment.from_wav('owlSleep.wav')


###########################
### Einzelne Bewegungen ###
###########################



### AUGEN: servo[0]


def aniBlink():

#Die Augen schließen sich, bis sie sich 0.2 Sekunden später wieder öffnen. 
#Dadurch entsteht ein einzelnes Blinzeln.

  kit.servo[0].angle = eyesMin
  time.sleep(0.2)
  kit.servo[0].angle = eyesMax
  time.sleep(1)
  kit.servo[0].angle = None

def aniCloseEyes():

#Die Augen langsam geschlossen.

  global eyesPos

  eyesAim = eyesMin
  while eyesPos != eyesAim:
    if eyesPos <= eyesAim:
      eyesPos += 1
    else:
      eyesPos -= 1

    kit.servo[0].angle = eyesPos
    time.sleep(aniSpeed)

  kit.servo[0].angle = None


def aniOpenEyes():

#Die Augen werden lansgam geöffnet.

  global eyesPos

  eyesAim = eyesMax
  while eyesPos != eyesAim:
    if eyesPos <= eyesAim:
      eyesPos += 1
    else:
      eyesPos -= 1

    kit.servo[0].angle = eyesPos
    time.sleep(aniSpeed)

  kit.servo[0].angle = None



### SCHNABEL: servo[1]


def aniOpenBeak():

#Der Schnabel wird langsam geöffnet.

  global beakPos

  beakAim = beakMax
  while beakPos != beakAim:
    if beakPos <= beakAim:
      beakPos += 1
    else:
      beakPos -= 1

    kit.servo[1].angle = beakPos
    time.sleep(aniSpeed)

  kit.servo[1].angle = None


def aniCloseBeak():

#Der Schnabel wird langsam geschlossen.

  global beakPos

  beakAim = beakMin
  while beakPos != beakAim:
    if beakPos <= beakAim:
      beakPos += 1
    else:
      beakPos -= 1

    kit.servo[1].angle = beakPos
    time.sleep(aniSpeed)

  kit.servo[1].angle = None



### KOPF: servo[2]

def aniHeadRandom():

#Der Kopf bewegt sich zu einer zufälligen Position.

  global headPos

  headAim = random.randint(headMin,headMax)
  while headPos != headAim:
    if headPos <= headAim:
      headPos += 1
    else:
      headPos -= 1

    kit.servo[2].angle = headPos
    time.sleep(aniSpeed)

  kit.servo[2].angle = None


def aniHeadLeft():

#Der Kopf dreht sich nach links.

  global headPos

  headAim = headMin
  while headPos != headAim:
    if headPos <= headAim:
      headPos += 1
    else:
      headPos -= 1

    kit.servo[2].angle = headPos
    time.sleep(aniSpeed)

  kit.servo[2].angle = None


def aniHeadRight():

#Der Kopf dreht sich nach rechts.

  global headPos

  headAim = headMax
  while headPos != headAim:
    if headPos <= headAim:
      headPos += 1
    else:
      headPos -= 1

    kit.servo[2].angle = headPos
    time.sleep(aniSpeed)

  kit.servo[2].angle = None


def aniHeadForward():

#Der Kopf schaut nach vorn

  global headPos

  headAim = headMid
  while headPos != headAim:
    if headPos <= headAim:
      headPos += 1
    else:
      headPos -= 1

    kit.servo[2].angle = headPos
    time.sleep(aniSpeed)

  kit.servo[2].angle = None


### FLÜGEL: servo[3]

def aniFlapUp():

#Die Flügel bewegen sich hoch.
  global wingsPos

  wingsAim = wingsMax
  while wingsPos != wingsAim:
    if wingsPos <= wingsAim:
      wingsPos += 1
    else:
      wingsPos -= 1

    kit.servo[3].angle = wingsPos
    time.sleep(aniSpeed)

  kit.servo[3].angle = None


def aniFlapDown():

#Die Flügel bewegen sich runter.

  global wingsPos

  wingsAim = wingsMin
  while wingsPos != wingsAim:
    if wingsPos <= wingsAim:
      wingsPos += 1
    else:
      wingsPos -= 1

    kit.servo[3].angle = wingsPos
    time.sleep(aniSpeed)

  kit.servo[3].angle = None



#########################
### Bewegungsabläufe ###
#########################

#Mithilfe der einzelnen Bewegungen und Sounds lassen sich hier ganze Bewegungsabläufe
#choreografieren, die sich anschließend mit einem Befehl abrufen lassen.


def aniSpecialMail():

  aniOpenEyes()
  play(sndWakeup)
  aniFlapUp()
  aniOpenBeak()
  play(sndHoot)
  aniBlink()
  aniCloseBeak()
  aniOpenBeak()
  play(sndHoot)
  aniBlink()
  aniCloseBeak()
  aniFlapDown()
  aniHeadRight()
  aniBlink()
  aniHeadLeft()
  aniHeadForward()
  aniBlink()
  aniCloseEyes()
  #play(sndSleep)


def aniSpecialMail2():

  aniOpenEyes()
  aniBlink()
  time.sleep(1)
  aniBlink()
  aniHeadLeft()
  time.sleep(1)
  aniBlink()
  time.sleep(2)
  aniHeadForward()
  time.sleep(1)
  aniBlink()
  time.sleep(2)
  aniHeadLeft()
  time.sleep(1)
  aniFlapUp()
  aniFlapDown()
  aniBlink()
  aniHeadForward()
  time.sleep(1)
  aniOpenBeak()
  play(sndHoot)
  aniCloseBeak()
  aniBlink()
  aniHeadLeft()
  time.sleep(1)
  aniBlink()
  time.sleep(2)
  aniHeadForward()
  aniBlink()
  time.sleep(2)
  aniCloseEyes()

def aniStartUp():

  aniOpenEyes()
  aniBlink()
  time.sleep(1)
  aniHeadLeft()
  aniBlink()
  aniOpenBeak()
  play(sndHoot)
  aniCloseBeak()
  aniHeadForward()
  time.sleep(1)
  aniBlink()
  aniFlapUp()
  aniFlapDown()
  aniBlink()
  aniHeadRight()
  aniOpenBeak()
  play(sndHoot)
  aniCloseBeak()
  aniHeadForward()
  aniBlink()
  time.sleep(1)
  aniCloseEyes()



#######################
### Startfunktionen ###
#######################

### Alles auf 0 setzen

### Servos in Ausgangsposition

def resetServos():

#Nach vorne schauen

  kit.servo[2].angle = headMid
  time.sleep(1)
  kit.servo[2].angle = None

#Schnabel schließen

  kit.servo[1].angle = beakMin
  time.sleep(1)
  kit.servo[1].angle = None

#Flügel senken

  kit.servo[3].angle = wingsMin
  time.sleep(1)
  kit.servo[3].angle = None

#Augen zu

  kit.servo[0].angle = eyesMin
  time.sleep(1)
  kit.servo[0].angle = None


### Mail-Abruf und Löschbefehl

def checkIncoming():

#Login am E-Mail-Server

  server = IMAPClient(HOSTNAME, use_uid=True, ssl=True)
  server.login(USERNAME,PASSWORD)

#Das Postfach Eingang wird ausgewählt

  server.select_folder('Inbox')

#Im Eingang wird nach spezifischen E-Mail-Adressen gesucht

  person1 = server.search(['FROM','absender1@gmx.de'])
  person2 = server.search(['FROM','absender2@gmail.com'])

#Falls eine E-Mail von den spezifischen E-Mail-Adressen existiert,
#wird eine vorher festgelegete Animation ausgeführt.

  if(len(person1) > 0):
    aniSpecialMail()
  elif(len(person2) > 0):
    aniSpecialMail2()

#Danach werden alle E-Mails herausgesucht
#und nacheinander gelöscht.

  all = server.search('ALL')
  for mail in all:
    server.delete_messages([mail])

#Nach dieser Pause beginnt die Abfrage erneut.

  time.sleep(checkFreq)


########################
### Hier geht es los ###
########################

#Zu Beginn werden die Servos auf Ausgangsstellung gestellt.
#Danach läuft die Mail-Abfrage in Schleife.

resetServos()
time.sleep(1)
aniStartUp()

while True:
  checkIncoming()
