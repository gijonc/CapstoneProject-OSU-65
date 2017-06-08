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


PORT = '/dev/tty.SLAB_USBtoUART'	# mac OS
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

def reset(plane, yaw, pitch, roll, color):
	plane.visible = False
	del plane
	#time.sleep(0.05)
	plane = vp.frame()
	vp.ellipsoid(frame=plane, pos=(0,0,0), length=8, height=2.5, width=2.5, color=color, opacity=0.5)
	vp.pyramid(frame=plane, pos=(-1,0,0), size=(4,6,1), color=color, opacity=0.5)
	vp.pyramid(frame=plane, pos=(-3.5,0,1), size=(2,1,2), color=color, opacity=0.5)
	plane.rotate(angle=math.pi/2, axis=(-1,0,0), origin=(0,0,0))
	
	plane.rotate(angle=-math.pi/2, axis=(0,1,0))	
	plane.rotate(angle=roll, axis=(0,0,1))		
	plane.rotate(angle=pitch, axis=(1,0,0))	
	return plane;

def text_reset(text, data, x, y, z):
	text.visible = False
	del text
	data = data*RtoD
	text = vp.label(pos=(x,y,z), text=str(data), height=10, border=0, font='sans', color=vp.color.green, box=0)
	return text; 		
	
def display():
	# scene and object initilaizing
	scene2 = vp.display(title='Airplane Simulation',
     x=0, y=0, width=800, height=600,
     center=(0,0,8), autoscale = 1, background=vp.color.black)
	 
	#Text  
	text1 = vp.label(pos=(-4,-8,0), text='Yaw', height=18, border=0, font='sans', color=vp.color.green, box=0)
	text1 = vp.label(pos=(0,-8,0), text='Pitch', height=18, border=0, font='sans', color=vp.color.green, box=0)
	text1 = vp.label(pos=(4,-8,0), text='Roll', height=18, border=0, font='sans', color=vp.color.green, box=0)
	text1 = vp.label(pos=(-8,-9,0), text='HUD IMU Data:', height=10, border=0, font='sans', color=vp.color.green, box=0)
	text1 = vp.label(pos=(-8,-10,0), text='Airplane IMU Data:', height=10, border=0, font='sans', color=vp.color.green, box=0)
	
	#Plane 1
	plane = vp.frame()
	vp.ellipsoid(frame=plane, pos=(0,0,0), length=8, height=2.5, width=2.5, color=vp.color.red, opacity=0.5)
	vp.pyramid(frame=plane, pos=(-1,0,0), size=(4,6,1), color=vp.color.red, opacity=0.5)
	vp.pyramid(frame=plane, pos=(-3.5,0,1), size=(2,1,2), color=vp.color.red, opacity=0.5)
	plane.rotate(angle=math.pi/2, axis=(-1,0,0), origin=(0,0,0))
	#Axis Rotation for debugging purpose
	plane.rotate(angle=math.pi/2, axis=(0,-1,0), origin=(0,0,0))
	# plane.rotate(angle=math.pi/2, axis=(0,0,-1), origin=(0,0,0))
	
	#Plane 2
	plane2 = vp.frame()
	vp.ellipsoid(frame=plane2, pos=(0,0,0), length=8, height=2.5, width=2.5, color=vp.color.green, opacity=0.5)
	vp.pyramid(frame=plane2, pos=(-1,0,0), size=(4,6,1), color=vp.color.green, opacity=0.5)
	vp.pyramid(frame=plane2, pos=(-3.5,0,1), size=(2,1,2), color=vp.color.green, opacity=0.5)
	plane2.rotate(angle=math.pi/2, axis=(-1,0,0), origin=(0,0,0))
	#Axis Rotation for debugging purpose
	plane2.rotate(angle=math.pi/2, axis=(0,-1,0), origin=(0,0,0))
	# plane2.rotate(angle=math.pi/2, axis=(0,0,-1), origin=(0,0,0))
	
	#Axis and Horizon Line
	Axis = vp.label(pos=(0,0,7.5), text='Y', height=10, border=0, font='sans', color=vp.color.green, box=0)
	Axis = vp.arrow(pos=(0,0,0), axis=(7,0,0), shaftwidth=0.1, color=vp.color.green, opacity=0.5)
	Axis = vp.label(pos=(7.5,0,0), text='X', height=10, border=0, font='sans', color=vp.color.green, box=0)
	Axis = vp.arrow(pos=(0,0,0), axis=(0,7,0), shaftwidth=0.1, color=vp.color.green, opacity=0.5)
	Axis = vp.label(pos=(0,7.5,0), text='Z', height=10, border=0, font='sans', color=vp.color.green, box=0)
	Axis = vp.arrow(pos=(0,0,0), axis=(0,0,7), shaftwidth=0.1, color=vp.color.green, opacity=0.5)
	
	#Print Original Data	
	text2 = vp.label(pos=(-4,-9,0), text='Original Data', height=10, border=0, font='sans', color=vp.color.green, box=0)
	text3 = vp.label(pos=(0,-9,0), text='Original Data', height=10, border=0, font='sans', color=vp.color.green, box=0)
	text4 = vp.label(pos=(4,-9,0), text='Original Data', height=10, border=0, font='sans', color=vp.color.green, box=0)
			
	#Print Aligned Data	
	text5 = vp.label(pos=(-4,-10,0), text='Alignment Error', height=10, border=0, font='sans', color=vp.color.green, box=0)
	text6 = vp.label(pos=(0,-10,0), text='Alignment Error', height=10, border=0, font='sans', color=vp.color.green, box=0)
	text7 = vp.label(pos=(4,-10,0), text='Alignment Error', height=10, border=0, font='sans', color=vp.color.green, box=0)
	
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
		vp.rate(50)	
		while SERIRAL_INPUT.inWaiting() == 0:
			pass

		serial_line = SERIRAL_INPUT.readline()
		c = readData(serial_line)
		if len(c) == 6:	# filter out none numeric data input
			print c
			imu1.cur_yaw = float(c[3]) * DtoR
			imu1.cur_pitch = float(c[1]) * DtoR
			imu1.cur_roll = float(c[2]) * DtoR

			imu2.cur_yaw = float(c[3]) * DtoR
			imu2.cur_pitch = float(c[4]) * DtoR
			imu2.cur_roll = float(c[5]) * DtoR

		plane = reset(plane, imu1.cur_yaw, imu1.cur_pitch, imu1.cur_roll, vp.color.red)
		plane2 = reset(plane2, imu2.cur_yaw, imu2.cur_pitch, imu2.cur_roll, vp.color.green)
		text2 = text_reset(text2, imu1.cur_yaw, -4, -9, 0)
		text3 = text_reset(text3, imu1.cur_pitch, 0, -9, 0)
		text4 = text_reset(text4, imu1.cur_roll, 4, -9, 0 )
		text5 = text_reset(text5, imu2.cur_yaw, -4, -10, 0)
		text6 = text_reset(text6, imu2.cur_pitch, 0, -10, 0 )
		text7 = text_reset(text7, imu2.cur_roll, 4, -10, 0 )

	SERIRAL_INPUT.close() # Only executes once the loop exits


if __name__ == "__main__":
	yawA = []
	# for loop allows serial reading for couple of seconds
	# to avoid error
	print "initializing..."
		

	serial_line = SERIRAL_INPUT.readline().rstrip()

	while (serial_line != 'DONE'):
		serial_line = SERIRAL_INPUT.readline().rstrip()
		print serial_line


	display()

