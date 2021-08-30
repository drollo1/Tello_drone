from tello_drone import *
import time
import cv2
from easytello import tello

drone = Drone(silent = False, threading = False)
drone.streamon()
flying = False
while True:
	# print(x)
	valid, img = drone.get_frame()
	if valid:
		cv2.imshow("Tello Drone Video Feed", img)
		k = cv2.waitKey(1) & 0xFF
		if k == 27:
			break
		elif not 255 == k:
			print(k)
drone.streamoff()
cv2.destroyAllWindows()
drone.shutdown()
print("END")