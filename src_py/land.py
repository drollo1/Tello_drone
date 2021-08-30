from tello_drone import *
import time
import cv2

drone = Drone(silent = False, threading =False)
drone.land()
drone.shutdown()
print("END")