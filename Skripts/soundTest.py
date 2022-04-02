#!/usr/bin/python3

import time
from pysinewave import SineWave
from time import sleep

sinewave = SineWave(pitch = 12)
sinewave.play()
time.sleep(2)
sinewave.stop()
