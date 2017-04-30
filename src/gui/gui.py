#!/usr/bin/python


'''
---	Oregon State University 2017 Capstone Project ---

Author:		Jiongcheng Luo, Drew Hamm, Krisna Irawan
Description:
	graphical display program using vpython (http://vpython.org/)
	and pyserial (https://pythonhosted.org/pyserial/) libraries
'''

import serial
import visual as vp
import math
import sys
import time 


PORT = '/dev/tty.SLAB_USBtoUART'	# Roger
#PORT = 'COM3'						# Krisna
#PORT = 'COM6'						# Drew


BAUD_RATE = 57600
DtoR = (math.pi/180)
RtoD = (180/math.pi)

SERIRAL_INPUT = serial.Serial(PORT, BAUD_RATE)


def readData(string):
	newSerialInput = []
	for x in string.split(','):
		newSerialInput.append(x.strip())
	return newSerialInput

def debug():
	# debug for vpython display
	scene2 = vp.display(title='Examples of Tetrahedrons',
     x=0, y=0, width=800, height=600,
     center=(0,0,2), background=vp.color.white)

	plane = vp.box (pos=vp.vector(-5,0,0), length=5, height=1, width=3, color=vp.color.red, opacity=0.5)	
	plane2 = vp.box (pos=vp.vector(5,0,0), length=5, height=1, width=3, color=vp.color.yellow, opacity=0.5)

	# debug for serial port input
	while 1:
		serial_line = SERIRAL_INPUT.readline()
		if serial_line:
			c = readData(serial_line)
			if len(c) == 6:	# filter out none numeric data input
				print c
	SERIRAL_INPUT.close() # Only executes once the loop exits


#---------------------------------------------
#				MAIN PROGRAM
#---------------------------------------------
class IMU():
	def __init__(self,name):
		self.name = name
		self.prev_pitch = 0
		self.prev_roll = 0
		self.prev_yaw = 0

		self.cur_pitch = 0		
		self.cur_roll = 0
		self.cur_yaw = 0

		self.pitch = 0		
		self.roll = 0
		self.yaw = 0

	def reset(self,b):
		if b == True:
			self.pitch = 0		
			self.roll = 0
			self.yaw = 0


def display():
	# Initializing UI config
	scene2 = vp.display(title='Examples of Tetrahedrons',
     x=0, y=0, width=800, height=600,
     center=(0,12,3), background=vp.color.white)

	plane = vp.box (pos=(5,0,0), length=5, height=1, width=3, color=vp.color.red, opacity=0.5)	
	plane2 = vp.box (pos=vp.vector(5,0,0), length=5, height=1, width=3, color=vp.color.yellow, opacity=0.5)	

	
	#---------------------------------------------
	#	Update Animation from serial-port data
	#---------------------------------------------
	def sense_filter(cur,prev):	# adjust sensitivity
		__SENSE = 0.005		# higher _SENSE = higher sensitivity
		delta = cur - prev
		if math.fabs(delta) > __SENSE:
			return delta
		return 0

	# enable which axis to run 
	yawAxis = True
	pitchAxis = True
	rollAxis = True

	imu1 = IMU('hud')
	imu2 = IMU('airplane')

	# init = 0
	# loop over for animation
	while True:
		# animation rate: Halts computations until 1.0/frequency seconds
		vp.rate(50)	
		while SERIRAL_INPUT.inWaiting() == 0:
			pass

		serial_line = SERIRAL_INPUT.readline()
		c = readData(serial_line)
		if len(c) == 6:	# filter out none numeric data input
			print c
			imu1.cur_yaw = float(c[0])
			imu1.cur_pitch = float(c[1])
			imu1.cur_roll = float(c[2])

			imu2.cur_yaw = float(c[3])
			imu2.cur_pitch = float(c[4])
			imu2.cur_roll = float(c[5])
		# update rotate angle
		imu1_d_yaw	 	= sense_filter(imu1.cur_yaw, imu1.prev_yaw)
		imu1_d_pitch 	= sense_filter(imu1.cur_pitch,imu1.prev_pitch)
		imu1_d_roll 	= sense_filter(imu1.cur_roll,imu1.prev_roll)
		imu2_d_yaw	 	= sense_filter(imu2.cur_yaw, imu2.prev_yaw)
		imu2_d_pitch 	= sense_filter(imu2.cur_pitch,imu2.prev_pitch)
		imu2_d_roll 	= sense_filter(imu2.cur_roll,imu2.prev_roll)

		# coll to rotate
		#plane.rotate(angle=-imu1_d_pitch, axis=(0,0,pitchAxis))	#pitch
		#plane.rotate(angle=imu1_d_roll, axis=(rollAxis,0,0))	#roll
		plane.rotate(angle=imu1_d_yaw, axis=(0,yawAxis,0), origin=(1,0,0))		#yaw
		#plane2.rotate(angle=-imu2_d_pitch, axis=(0,0,pitchAxis))	#pitch
		#plane2.rotate(angle=imu2_d_roll, axis=(rollAxis,0,0))	#roll
		plane2.rotate(angle=imu2_d_yaw, axis=(0,yawAxis,0))		#yaw

		# reset to new angle
		imu1.prev_yaw = imu1.cur_yaw		
		imu1.prev_pitch = imu1.cur_pitch
		imu1.prev_roll = imu1.cur_roll
		imu2.prev_yaw = imu2.cur_yaw		
		imu2.prev_pitch = imu2.cur_pitch
		imu2.prev_roll = imu2.cur_roll

		#print "up:",plane.up
		#print "axis:",plane.axis
	SERIRAL_INPUT.close() # Only executes once the loop exits



if __name__ == "__main__":
	#debug()
	yawA = []
	# for loop allows serial reading for couple of seconds
	# to avoid error
	print "initializing..."
		
	for i in range(100):
		serial_line = SERIRAL_INPUT.readline()
	if (!serial_line):
		print "Failed to read serial input"
		return False

	display()

