#ifndef _QUATERNIONFILTERS_H_
#define _QUATERNIONFILTERS_H_

void MadgwickQuaternionUpdate(Quaternion *q, float ax, float ay, float az, float gx, float gy,
                              float gz, float mx, float my, float mz,
                              float deltat);
void MahonyQuaternionUpdate(Quaternion *q, float eInt[], float ax, float ay, float az, float gx, float gy,
                            float gz, float mx, float my, float mz,
                            float deltat);

#endif