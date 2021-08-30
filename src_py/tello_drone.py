import socket
import time
from threading import Thread, Lock
import cv2
import datetime

class Drone:
	def __init__(self, speed = 10, silent = True, threading = True):
		# Open local UDP pots
		self.local_ip = ''
		self.local_port = 8889
		self.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
		self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
		self.socket.bind((self.local_ip, self.local_port))


		self.tello_ip = "192.168.10.1"
		self.tello_port = 8889
		self.tello_address = (self.tello_ip, self.tello_port)
		
		self.speed = speed
		self.silent = silent
		self.streaming = False
		self.cap = None
		self.flying = False
		self.frame_lock = Lock()
		self.last_frame = None
		self.frame_num = 0

		self.initlize()
		if(threading):
			self.thread = Thread(target=self._checking_thread)
			self.thread.daemon = True
			self.thread.start()

	def initlize(self):
		cmd = ""
		while not 'ok' == cmd:
			cmd = self.send_command("command", True).decode('utf-8')
		spd = self.send_command("speed %d" % self.speed, True).decode('utf-8')
		if not self.silent:
			print("Command: %s\nSpeed(%d): %s" %(cmd, self.speed, spd))
		print("battery: %s" % self.send_command("battery?", True))

	def _checking_thread(self):
		while True:
			bat = self.send_command("battery?", True)
			h = self.send_command("height?", True)
			speed = self.send_command("speed", True)
			print("Battery: %s" % bat[:-2])
			print("height: %s" % h[:-2])
			print("speed: %s" % speed)
			time.sleep(1)


	def _video_stream_thread(self):
		self.frame_lock.acquire()
		cap = cv2.VideoCapture('udp://'+self.tello_ip+':11111')
		self.streaming = True
		ret, self.last_frame = cap.read()
		self.frame_lock.release()
		start = datetime.datetime.now()
		while self.streaming:
			ret, self.last_frame = cap.read()
			print((start - datetime.datetime.now()).microseconds)
			start = datetime.datetime.now()
		cap.release()

	def send_command(self, command, responsed = False):
		# self.com_lock.acquire()
		self.socket.sendto(command.encode('utf-8'), self.tello_address)
		start = time.time()
		if not responsed:
			return None
		try:
			self.response, ip = self.socket.recvfrom(1024)
			# self.com_lock.release()
			return self.response
		except socket.error as exc:
			print('Socket error: {}'.format(exc))

	def takeoff(self):
		err = self.send_command("takeoff", True).decode('utf-8')
		if 'ok' == err:
			self.flying = True
		if not self.silent:
			print("takeoff %s" % err)
		# print("Height: %s" % self.send_command("height?"))

	def land(self):
		err = self.send_command("land", True).decode('utf-8')
		if 'ok' == err:
			self.flying = False
		if not self.silent:
			print("land %s" % err)

	def cw(self, degrees = 0):
		err = self.send_command("cw %d" % degrees)
		if not self.silent:
			print("cw %s" % err)

	def ccw(self, degrees = 0):
		err = self.send_command("ccw %d" % degrees)
		if not self.silent:
			print("ccw %s" % err)

	def up(self, dis = 0):
		if dis != 0:
			err = self.send_command("up %d" % dis)

	def down(self, dis = 0):
		if dis != 0:
			err = self.send_command("down %d" % dis)

	def forward(self, dis = 0):
		if dis != 0:
			err = self.send_command("forward %d" % dis)

	def back(self, dis = 0):
		if dis != 0:
			err = self.send_command("back %d" % dis)	

	def left(self, dis = 0):
		if dis != 0:
			err = self.send_command("left %d" % dis)

	def right(self, dis = 0):
		if dis != 0:
			err = self.send_command("right %d" % dis)
	def stop(self):
		self.send_command("stop")
		print("stopping")


	def streamon(self):
		if not self.streaming:
			err = self.send_command("streamon", True).decode('utf-8')
			self.videostream = Thread(target=self._video_stream_thread)
			self.videostream.daemon = True
			self.videostream.start()
			self.frame_lock.acquire()
			self.frame_lock.release()
		else:
			print("Already streaming")

	def streamoff(self):
		if not self.streaming:
			print("streaming is not running")
		else:
			self.streaming = False
			# self.cap.release()
			

	def get_frame(self):
		if self.streaming:
			return True, self.last_frame
		else:
			print("Streaming is not running")
			return False, None

	def shutdown(self):
		self.connected = False

	def rc_controls(self, rc_cons):
		err = self.send_command("rc %d %d %d %d" % (rc_cons[0], rc_cons[1], rc_cons[2], rc_cons[3]))