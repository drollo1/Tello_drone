from tello_drone import *
import time
import cv2
from easytello import tello

def _process_thread(k, cons):

	change = 5

	if 32 == k:
		drone.takeoff()
		return
	if 233 == k:
		drone.land()
		return	

	if 81 == k:
		# drone.left(500)
		if cons[0] > (-100):
			cons[0] -= change
			# cons[0] = -50
	elif 83 == k:
		# drone.right(500)
		if cons[0] < 100:
			cons[0] += change
			# cons[0] = 50
	else:
		cons[0] = 0

	if 82 == k:
		# drone.forward(500)
		if cons[1] < 100:
			cons[1] += change
			# cons[1] = 50
	elif 84 == k:
		# drone.back(500)
		if cons[1] > (-100):
			cons[1] -= change
			# cons[1] = -50
	else:
		# print(cons[0])
		cons[1] = 0

	if 119 == k:
		# drone.up(500)
		if cons[2] < 100:
			cons[2] += change
			# cons[2] = 50
	elif 115 == k:
		# drone.down(500)
		if cons[2] > (-100):
			cons[2] -= change
			# cons[2] = -50
	else:
		cons[2] = 0
	
	if 100 == k:
		# drone.cw(360)
		if cons[3] < 100:
			cons[3] += change
			# cons[3] = 50
	elif 97 == k:
		# drone.ccw(360)
		if cons[3] > (-100):
			cons[3] -= change
	else:
		# print(cons[3])
		cons[3] = 0

	# print(cons)
	# if not 255 == k:
	# 	print(k)
	drone.rc_controls(cons)

drone = Drone(threading = False)
drone.streamon()
flying = False
rc_cons = [0, 0, 0, 0]
while True:
	valid, img = drone.get_frame()
	if valid:
		cv2.imshow("Tello Drone Video Feed", img)
		k = cv2.waitKey(33) & 0xFF
		if k == 27:
			break
		control_thread = Thread(target=_process_thread, args=(k, rc_cons))
		control_thread.daemon = True
		control_thread.start()
drone.streamoff()
cv2.destroyAllWindows()
drone.shutdown()
print("END")