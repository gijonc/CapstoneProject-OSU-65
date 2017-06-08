#ifndef _DATAUTILS_H_
#define _DATAUTILS_H_

float getMean(float *samples, int sampleSize);
float getStandardDeviation(float *samples, float mean, int sampleSize);
float getConfidenceInterval(float standardDeviation, int sampleSize, float z_value);

double quaternionToPhi(Quaternion *q);
double quaternionToTheta(Quaternion *q);
double quaternionToPsi(Quaternion *q);

float quaternionToYaw(Quaternion *q);
float quaternionToPitch(Quaternion *q);
float quaternionToRoll(Quaternion *q);

Quaternion* q_difference(Quaternion *q1, Quaternion *q2);

Quaternion* q_inverse(Quaternion *q);
Quaternion* q_conjugate(Quaternion *q);
float q_dot(Quaternion *q1, Quaternion *q2);
Quaternion* q_mult(Quaternion *q1, Quaternion *q2);
Quaternion* q_div(Quaternion *q, float c);

#endif