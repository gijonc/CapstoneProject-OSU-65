#ifndef _QUATERNION_MODULE_H_
#define _QUATERNION_MODULE_H_

class Quaternion_Module
{
	public:
		// Vector to hold integral error for Mahony method
	    float eInt[3] = {0.0f, 0.0f, 0.0f};
	    Quaternion *q;

	    // used to calculate integration interval
	    float deltat = 0.0f;
    	uint32_t lastUpdate = 0;
    	uint32_t now = 0;
		void update(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz);
		Quaternion_Module();
};

#endif
