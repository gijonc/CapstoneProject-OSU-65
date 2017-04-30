#!/usr/bin/python


'''

Graphical User-Interace 

Author:		Jiongcheng Luo, Drew Hamm, Krisna Irawan

version: 1.0

'''

import serial
import visual as vp
import math
import sys
import time 


PORT = '/dev/tty.SLAB_USBtoUART'	# mac
#PORT = 'COM4'						# windows


BAUD_RATE = 38400
DtoR = (math.pi/180)
RtoD = (180/math.pi)

SERIRAL_INPUT = serial.Serial(PORT, BAUD_RATE)


def readData(string):
	newSerialInput = []
	for x in string.split(','):
		newSerialInput.append(x.strip())
	return newSerialInput

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
	# scene and object initilaizing

	scene = vp.display(title='Airplane Simulation',
     x=0, y=0, width=800, height=600,
     center=(0,0,8), background=vp.color.black, autoscale=True)
	 
	#Text  
	text1 = vp.label(pos=(-4,-8,0), text='Yaw', height=18, border=0, font='sans', color=vp.color.green, box=0)
	text1 = vp.label(pos=(0,-8,0), text='Pitch', height=18, border=0, font='sans', color=vp.color.green, box=0)
	text1 = vp.label(pos=(4,-8,0), text='Roll', height=18, border=0, font='sans', color=vp.color.green, box=0)
	text1 = vp.label(pos=(-8,-9,0), text='Alignment Error', height=10, border=0, font='sans', color=vp.color.green, box=0)
	text1 = vp.label(pos=(-8,-10,0), text='Aligned Data', height=10, border=0, font='sans', color=vp.color.green, box=0)
	text1 = vp.label(pos=(-8,-11,0), text='Original Data', height=10, border=0, font='sans', color=vp.color.green, box=0)
	
	#Plane 1
	plane = vp.frame()
	vp.ellipsoid(frame=plane, pos=(0,0,0), length=8, height=2.5, width=2.5, color=vp.color.red, opacity=0.5)
	vp.pyramid(frame=plane, pos=(-1,0,0), size=(4,6,1), color=vp.color.red, opacity=0.5)
	vp.pyramid(frame=plane, pos=(-3.5,0,1), size=(2,1,2), color=vp.color.red, opacity=0.5)
	plane.rotate(angle=math.pi/2, axis=(-1,0,0), origin=(0,0,0))
	#Axis Rotation for debugging purpose
	#plane.rotate(angle=math.pi/2, axis=(0,-1,0), origin=(0,0,0))
	#plane.rotate(angle=math.pi, axis=(0,0,-1), origin=(0,0,0))
	
	#Plane 2
	plane2 = vp.frame()
	vp.ellipsoid(frame=plane2, pos=(0,0,0), length=8, height=2.5, width=2.5, color=vp.color.green, opacity=0.5)
	vp.pyramid(frame=plane2, pos=(-1,0,0), size=(4,6,1), color=vp.color.green, opacity=0.5)
	vp.pyramid(frame=plane2, pos=(-3.5,0,1), size=(2,1,2), color=vp.color.green, opacity=0.5)
	plane2.rotate(angle=math.pi/2, axis=(-1,0,0), origin=(0,0,0))
	#Axis Rotation for debugging purpose
	#plane2.rotate(angle=math.pi/2, axis=(0,-1,0), origin=(0,0,0))
	#plane2.rotate(angle=math.pi/2, axis=(0,0,-1), origin=(0,0,0))
	
	#Axis and Horizon Line 
	Axis = vp.arrow(pos=(0,0,0), axis=(7,0,0), shaftwidth=0.1, color=vp.color.green, opacity=0.5)
	Axis = vp.label(pos=(7.5,0,0), text='X', height=10, border=0, font='sans', color=vp.color.green, box=0)
	Axis = vp.arrow(pos=(0,0,0), axis=(0,7,0), shaftwidth=0.1, color=vp.color.green, opacity=0.5)
	Axis = vp.label(pos=(0,7.5,0), text='Y', height=10, border=0, font='sans', color=vp.color.green, box=0)
	Axis = vp.arrow(pos=(0,0,0), axis=(0,0,7), shaftwidth=0.1, color=vp.color.green, opacity=0.5)
	Horizon = vp.arrow(pos=(-20,0,0), axis=(40,0,0), shaftwidth=0.1, color=vp.color.red, opacity=0.5)
	Horizon = vp.label(pos=(-7.5,-0.5,0), text='Horizon Line', height=10, border=0, font='sans', color=vp.color.red, box=0)
	
#---------------------------------------------
#					GUI PART
#---------------------------------------------
	def sense_filter(cur,prev):	# adjust animation sensitivity
		__SENSE = 0.005		# higher _SENSE = higher sensitivity
		delta = cur - prev
		if math.fabs(delta) > __SENSE:
			return delta
		return 0

	# enable which axis to run 
	yawAxis = True
	pitchAxis = True
	rollAxis = True

	imu1 = IMU('1')
	imu2 = IMU('2')

	# init = 0
	# loop over for animation
	while True:
		# performed rate for animation
		vp.rate(30)	
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
			
		#may cause overload
		#Print Aligned Error	
		text2 = vp.label(pos=(-4,-9,0), text='Alignment Error', height=10, border=0, font='sans', color=vp.color.green, box=0)
		text2 = vp.label(pos=(0,-9,0), text='Alignment Error', height=10, border=0, font='sans', color=vp.color.green, box=0)
		text2 = vp.label(pos=(4,-9,0), text='Alignment Error', height=10, border=0, font='sans', color=vp.color.green, box=0)
		
		#Print Aligned Data	
		text2 = vp.label(pos=(-4,-10,0), text=str(round(float(c[0])*RtoD,3)), height=10, border=0, font='sans', color=vp.color.green, box=0)
		text2 = vp.label(pos=(0,-10,0), text=str(round(float(c[1])*RtoD,3)), height=10, border=0, font='sans', color=vp.color.green, box=0)
		text2 = vp.label(pos=(4,-10,0), text=str(round(float(c[2])*RtoD,3)), height=10, border=0, font='sans', color=vp.color.green, box=0)
			
		#Print Original Data	
		text2 = vp.label(pos=(-4,-11,0), text=str(round(float(c[3])*RtoD,3)), height=10, border=0, font='sans', color=vp.color.green, box=0)
		text2 = vp.label(pos=(0,-11,0), text=str(round(float(c[4])*RtoD,3)), height=10, border=0, font='sans', color=vp.color.green, box=0)
		text2 = vp.label(pos=(4,-11,0), text=str(round(float(c[5])*RtoD,3)), height=10, border=0, font='sans', color=vp.color.green, box=0)
		
		# update rotate angle
		imu1_d_yaw	 	= sense_filter(imu1.cur_yaw, imu1.prev_yaw)
		imu1_d_pitch 	= sense_filter(imu1.cur_pitch,imu1.prev_pitch)
		imu1_d_roll 	= sense_filter(imu1.cur_roll,imu1.prev_roll)
		imu2_d_yaw	 	= sense_filter(imu2.cur_yaw, imu2.prev_yaw)
		imu2_d_pitch 	= sense_filter(imu2.cur_pitch,imu2.prev_pitch)
		imu2_d_roll 	= sense_filter(imu2.cur_roll,imu2.prev_roll)

		# call to rotate
		plane.rotate(angle=-imu1_d_pitch, axis=(0,0,pitchAxis))	#pitch
		plane.rotate(angle=imu1_d_roll, axis=(rollAxis,0,0))	#roll
		plane.rotate(angle=imu1_d_yaw, axis=(0,yawAxis,0))		#yaw
		plane2.rotate(angle=-imu2_d_pitch, axis=(0,0,pitchAxis))#pitch
		plane2.rotate(angle=imu2_d_roll, axis=(rollAxis,0,0))	#roll
		plane2.rotate(angle=imu2_d_yaw, axis=(0,yawAxis,0))	#yaw

		# reset to new angle
		imu1.prev_yaw = imu1.cur_yaw		
		imu1.prev_pitch = imu1.cur_pitch
		imu1.prev_roll = imu1.cur_roll
		imu2.prev_yaw = imu2.cur_yaw		
		imu2.prev_pitch = imu2.cur_pitch
		imu2.prev_roll = imu2.cur_roll

	SERIRAL_INPUT.close() # Only executes once the loop exits



if __name__ == "__main__":
	yawA = []
	# for loop allows serial reading for couple of seconds
	# to avoid error
	print "initializing..."
		
	for i in range(100):
		serial_line = SERIRAL_INPUT.readline()

	display()

