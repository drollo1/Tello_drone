from tello_drone import *
import time

drone = Drone(silent = False, threading =False)
drone.takeoff()
drone.land()
drone.shutdown()
print("END")