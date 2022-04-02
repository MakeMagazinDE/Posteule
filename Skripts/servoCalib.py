#!/usr/bin/python3

import time
from adafruit_servokit import ServoKit
from time import sleep

kit = ServoKit(channels = 16)

kit.servo[0].angle = 90
#kit.servo[1].angle = 90
#kit.servo[2].angle = 90
#kit.servo[3].angle = 90


time.sleep(1)
kit.servo[0].angle = None
#kit.servo[1].angle = None
#kit.servo[2].angle = None
#kit.servo[3].angle = None


#Hier könnt ihr die Werte festhalten, damit ihr sie 
#nicht vergesst.

#eyesMin = 
#eyesMax = 
#beakMin = 
#beakMax = 
#wingsMax = 
#wingsMin = 
