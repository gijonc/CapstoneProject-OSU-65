#include "stdafx.h"

// Sensors x (y)-axis of the accelerometer is aligned with the y (x)-axis of
// the magnetometer; the magnetometer z-axis (+ down) is opposite to z-axis
// (+ up) of accelerometer and gyro! We have to make some allowance for this
// orientationmismatch in feeding the output to the quaternion filter. For the
// MPU-9250, we have chosen a magnetic rotation that keeps the sensor forward
// along the x-axis just like in the LSM9DS0 sensor. This rotation can be
// modified to allow any convenient orientation convention. This is ok by
// aircraft orientation standards! Pass gyro rate as rad/s
void Quaternion_Module::update(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz){
	// Get current time
	now = micros();

	// Set integration time by time elapsed since last filter update
	deltat = ((now - lastUpdate) / 1000000.0f);
	lastUpdate = now;



	//MahonyQuaternionUpdate(q, eInt, ax, ay, az, gx*DEG_TO_RAD, gy*DEG_TO_RAD, gz*DEG_TO_RAD, my, mx, mz, deltat);

	//MadgwickQuaternionUpdate(q, ax, ay, az, gx*DEG_TO_RAD, gy*DEG_TO_RAD, gz*DEG_TO_RAD, my, mx, mz, deltat);
	MadgwickQuaternionUpdate_v2(q, ax, ay, az, gx*DEG_TO_RAD, gy*DEG_TO_RAD, gz*DEG_TO_RAD, deltat);
}

Quaternion_Module::Quaternion_Module(){
	q = new Quaternion();
}
