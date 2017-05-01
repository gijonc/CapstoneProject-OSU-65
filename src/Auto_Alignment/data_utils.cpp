#include "stdafx.h"

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
float quaternionToYaw(Quaternion *q){
  float yaw = atan2(2.0f * (q -> q2 * q -> q3 + q -> q1 * q -> q4), q -> q1 * q -> q1 + q -> q2 * q -> q2 - q -> q3 * q -> q3 - q -> q4 * q -> q4);
  yaw *= RAD_TO_DEG;

  // TODO: Change this
  // Declination of SparkFun Electronics (40°05'26.6"N 105°11'05.9"W) is
  //  8° 30' E  ± 0° 21' (or 8.5°) on 2016-07-19
  // - http://www.ngdc.noaa.gov/geomag-web/#declination
  yaw  -= 8.5;

  return yaw;
}

float quaternionToPitch(Quaternion *q){
  float pitch = -asin(2.0f * (q -> q2 * q -> q4 - q -> q1 * q -> q3));

  pitch *= RAD_TO_DEG;

  return pitch;
}

float quaternionToRoll(Quaternion *q){
  float roll  = atan2(2.0f * (q -> q1 * q -> q2 + q -> q3 * q -> q4), q -> q1 * q -> q1 - q -> q2 * q -> q2 - q -> q3 * q -> q3 + q -> q4 * q -> q4);

  roll *= RAD_TO_DEG;

  return roll;
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
  q_c -> q1 = (q -> q1);
  q_c -> q2 = -(q -> q2);
  q_c -> q3 = -(q -> q3);
  q_c -> q4 = -(q -> q4);
  return q_c;
}

float q_dot(Quaternion *q1, Quaternion *q2){
  float result = ((q1 -> q1) * (q2 -> q1) + (q1 -> q2) * (q2 -> q2) + (q1 -> q3) * (q2 -> q3) + (q1 -> q4) * (q2 -> q4));
  return result;
}

Quaternion* q_mult(Quaternion *q1, Quaternion *q2){
  Quaternion *q_m = new Quaternion();
  q_m -> q1 = (q2 -> q1) * (q1 -> q1) - (q2 -> q2) * (q1 -> q2) - (q2 -> q3) * (q1 -> q3) - (q2 -> q4) * (q1 -> q4);
  q_m -> q2 = (q2 -> q1) * (q1 -> q2) + (q2 -> q2) * (q1 -> q1) - (q2 -> q3) * (q1 -> q4) + (q2 -> q4) * (q1 -> q3);
  q_m -> q3 = (q2 -> q1) * (q1 -> q3) + (q2 -> q2) * (q1 -> q4) + (q2 -> q3) * (q1 -> q1) - (q2 -> q4) * (q1 -> q2);
  q_m -> q4 = (q2 -> q1) * (q1 -> q4) - (q2 -> q2) * (q1 -> q3) + (q2 -> q3) * (q1 -> q2) + (q2 -> q4) * (q1 -> q1);
  return q_m;
}

Quaternion* q_div(Quaternion *q, float c){
  Quaternion *q_d = new Quaternion();
  q_d -> q1 = (q -> q1) / c;
  q_d -> q2 = (q -> q2) / c;
  q_d -> q3 = (q -> q3) / c;
  q_d -> q4 = (q -> q4) / c;
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