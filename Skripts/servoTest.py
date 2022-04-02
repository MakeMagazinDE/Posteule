#!/usr/bin/python3

import time
import random
from adafruit_servokit import ServoKit
from time import sleep

kit = ServoKit(channels=16)

while True:
  kit.servo[0].angle = eyesMin #hier den Minimalwert eingeben
  time.sleep(0.1)
  kit.servo[0].angle = eyesMax #hier den Maximalwert eingeben
  time.sleep(2)
  kit.servo[0].angle = None
  time.sleep(1)
