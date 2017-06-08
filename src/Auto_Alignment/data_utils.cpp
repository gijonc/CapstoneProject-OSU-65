#include "stdafx.h"




float getMean(float * samples, int sampleSize){
  float mean = 0.0;
  for(int i = 0; i < sampleSize; i++){
    mean += samples[i];
  }
  mean = mean / sampleSize;
  return mean;
}

float getStandardDeviation(float * samples, float mean, int sampleSize){
  float standardDeviation = 0.0;
  for(int i = 0; i < sampleSize; i++){
    standardDeviation += (samples[i] - mean) * (samples[i] - mean);
  }
  standardDeviation = sqrt(standardDeviation / (float)sampleSize);
  return standardDeviation;
}

float getConfidenceInterval(float standardDeviation, int sampleSize, float z_value){
  float confidenceInterval = 0.0;
  confidenceInterval = z_value * (standardDeviation / sqrt((float)sampleSize));
  return confidenceInterval;
}


// Define output variables from updated quaternion---these are Tait-Bryan
// angles, commonly used in aircraft orientation. In this coordinate system,
// the positive z-axis is down toward Earth. Yaw is the angle between Sensor
// x-axis and Earth magnetic North (or true North if corrected for local
// declination, looking down on the sensor positive yaw is counterclockwise.
// Pitch is angle between sensor x-axis and Earth ground plane, toward the
// Earth is positive, up toward the sky is negative. Roll is angle between
// sensor y-axis and Earth ground plane, y-axis up is positive roll. These
// arise from the definition of the homogeneous rotation matrix constructed
// from quaternions. Tait-Bryan angles as well as Euler angles are
// non-commutative; that is, the get the correct orientation the rotations
// must be applied in the correct order which for this configuration is yaw,
// pitch, and then roll.
// For more see
// http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
// which has additional links.

//Returns yaw in radians
float quaternionToYaw(Quaternion *q){
  float q2q3 = q->q2 * q->q3;
  float q1q4 = q->q1 * q->q4;
  float q1q1 = q->q1 * q->q1;
  float q2q2 = q->q2 * q->q2;
  float q3q3 = q->q3 * q->q3;
  float q4q4 = q->q4 * q->q4;
  float yaw;
  yaw = atan2f(2.0f * (q2q3 + q1q4), q1q1 + q2q2 - q3q3 - q4q4);

  //Declination of corvallis
  yaw += (15.14 * DEG_TO_RAD);

  return yaw;
}

//Returns pitch in radians
float quaternionToPitch(Quaternion *q){
  float q2q4 = q->q2 * q->q4;
  float q1q3 = q->q1 * q->q3;
  float pitch;
  pitch = -asinf(2.0f * (q2q4 - q1q3));

  return pitch;
}

//Returns roll in radians
float quaternionToRoll(Quaternion *q){
  float q1q2 = q->q1 * q->q2;
  float q3q4 = q->q3 * q->q4;
  float q1q1 = q->q1 * q->q1;
  float q2q2 = q->q2 * q->q2;
  float q3q3 = q->q3 * q->q3;
  float q4q4 = q->q4 * q->q4;

  float roll;
  roll = atan2f(2.0f * (q1q2 + q3q4), q1q1 - q2q2 - q3q3 + q4q4);

  return roll;
}


double quaternionToPhi(Quaternion *q){
  double q0 = q->q1;
  double q1 = q->q2;
  double q2 = q->q3;
  double q3 = q->q4;

  double R32 = 2.0 * (q2*q3 - q0*q1);
  double R33 = 2.0 * q0*q0 - 1.0 + 2.0 * q3*q3;
  double phi = atan2(R32, R33 );

  return phi;
}

double quaternionToTheta(Quaternion *q){
  double q0 = q->q1;
  double q1 = q->q2;
  double q2 = q->q3;
  double q3 = q->q4;

  double R31 = 2.0 * (q1*q3 + q0*q2);
  double theta = -atan(R31 / sqrt(1.0 - R31*R31));

  return theta;
}

double quaternionToPsi(Quaternion *q){
  double q0 = q->q1;
  double q1 = q->q2;
  double q2 = q->q3;
  double q3 = q->q4;

  double R11 = 2.0 * q0*q0 - 1.0 + 2.0 * q1*q1;
  double R21 = 2.0 * (q1*q2 - q0*q3);
  double psi = atan2(R21, R11 );

  return psi;

}

Quaternion* q_difference(Quaternion *q1, Quaternion *q2){
  Quaternion *q_diff;
  Quaternion *q_inv;
  q_inv = q_inverse(q1);
  q_diff = q_mult(q2, q_inv);

  //Deconstruct
  //q_inv
  if(q_inv){
    free(q_inv);
    q_inv = NULL;
  }

  return q_diff;
}

Quaternion* q_inverse(Quaternion *q){
  Quaternion *q_di;
  Quaternion *q_co;
  float dot = 1.0;
  q_co = q_conjugate(q);
  //dot = q_dot(q, q);
  q_di = q_div(q_co, dot);

  //Deconstruct
  //q_co, q_do
  if(q_co){
    free(q_co);
    q_co = NULL;
  }
  return q_di;
}

Quaternion* q_conjugate(Quaternion *q){
  Quaternion *q_c = new Quaternion();
  float q1 = q->q1;
  float q2 = -(q->q2);
  float q3 = -(q->q2);
  float q4 = -(q->q2);

  q_c->q1 = q1;
  q_c->q2 = q2;
  q_c->q3 = q3;
  q_c->q4 = q4;

  return q_c;
}

float q_dot(Quaternion *a, Quaternion *b){
  float a1b1 = a->q1 * b->q1;
  float a2b2 = a->q2 * b->q2;
  float a3b3 = a->q3 * b->q3;
  float a4b4 = a->q4 * b->q4;

  float result = a1b1 + a2b2 + a3b3 + a4b4;
  return result;
}

Quaternion* q_mult(Quaternion *a, Quaternion *b){
  Quaternion *q_m = new Quaternion();
  float a1b1 = a->q1 * b->q1;
  float a2b2 = a->q2 * b->q2;
  float a3b3 = a->q3 * b->q3;
  float a4b4 = a->q4 * b->q4;

  float a2b1 = a->q2 * b->q1;
  float a1b2 = a->q1 * b->q2;
  float a4b3 = a->q4 * b->q3;
  float a3b4 = a->q3 * b->q4;

  float a3b1 = a->q3 * b->q1;
  float a4b2 = a->q4 * b->q2;
  float a1b3 = a->q1 * b->q3;
  float a2b4 = a->q2 * b->q4;

  float a4b1 = a->q4 * b->q1;
  float a3b2 = a->q3 * b->q2;
  float a2b3 = a->q2 * b->q3;
  float a1b4 = a->q1 * b->q4;

  q_m->q1 = a1b1 - a2b2 - a3b3 - a4b4;
  q_m->q2 = a2b1 + a1b2 - a4b3 + a3b4;
  q_m->q3 = a3b1 + a4b2 + a1b3 - a2b4;
  q_m->q4 = a4b1 - a3b2 + a2b3 + a1b4;
  return q_m;
}

Quaternion* q_div(Quaternion *q, float c){
  Quaternion *q_d = new Quaternion();
  float q1_c = q->q1 / c;
  float q2_c = q->q2 / c;
  float q3_c = q->q3 / c;
  float q4_c = q->q4 / c;

  q_d->q1 = q1_c;
  q_d->q2 = q2_c;
  q_d->q3 = q3_c;
  q_d->q4 = q4_c;

  return q_d;
}
/*
void q_print(Quaternion *q){
  int precision = 5;
  Serial.print("q1: ");
  Serial.print(q -> q1, precision);
  Serial.print(" q2: ");
  Serial.print(q -> q2, precision);
  Serial.print(" q3: ");
  Serial.print(q -> q3, precision);
  Serial.print(" q4: ");
  Serial.println(q -> q4, precision);
}
*/
