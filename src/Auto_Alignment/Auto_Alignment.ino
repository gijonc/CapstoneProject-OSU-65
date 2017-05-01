/*
 Drew Hamm
 Jiongcheng Luo
 Krisna Irawan
 Project information:
 Auto_Alignment.ino
 CS462 CS Senior Capstone
 Fall 2017 - Spring 2017
 Group 65

 Program description:
 Initializes two SparkFun MPU-9250 Breakout boards to communicate with a Adafruit Metro Mini 328 - 5V 16MHz board.
 Communication via a single I2C channel.
 Initializes both MPU-9250s with onboard error correcting values.
 Both MPU-9250 accelerometer, gyroscope and magnetometer is requested by and sent to the Metro Mini 328.
 A sensor fusion algorithm is applied to data recieved from each MPU-2950.
 Data is converted into the quaternion form.
 The quaternion data from each sensor is used to find the realtime alignment offset between each MPU-9250.

 TODO:
 Output the error variance along with alignment offset data.
 Apply statistical analysis to alignment offset data until error variance is reduced to an acceptable range.
 Transition from finding and recording the initial alignment offset to finding the dynamic alignment offset.
 note: dynamic alignment offset is in regards to to airframe droop problem.

 Note:
 SDA and SCL should have external pull-up resistors (to 3.3V).
 10k resistors are on the EMSENSR-9250 breakout board.

 Hardware Devices:
 Adafruit Metro Mini 328 - 5V 16MHz  => MetroMini328
 SparkFun MPU-9250 Breakout          => MPU[0]
 SparkFun MPU-9250 Breakout          => MPU[1]

 Hardware Modifications:
 MetroMini328   -> Convert GPIO Logic level to 3.3V down from 5V
                => Cut and solder closed a jumper on the bottom of the board
 MPU[0], MPU[1] -> Enable AD0 to control the I2C address
                => Resolder the SJ2 jumper to connect the middle and left pads.

 Hardware Connections:
 MetroMini328(3.3v) -> (MPU[0], MPU[1])(VDD)
 MetroMini328(GND)  -> (MPU[0], MPU[1])(GND)
 MetroMini328(A4)   -> (MPU[0], MPU[1])(SDA)
 MetroMini328(A5)   -> (MPU[0], MPU[1])(SCL)
 MetroMini328(12)   -> MPU[0](AD0)
 MetroMini328(13)   -> MPU[1](AD0)
 */
#include "stdafx.h"

// The two digital login pins on the MetroMini328 that will be used
// to set the AD0 pins of each MPU9250
int AD0[2] = {12, 13};

// The two mpus connected to the microcontroller
// One should have their AD0 pin set HIGH wheras the other should
// have their AD0 pin set LOW
MPU9250 mpu[2];

int numberOfMPUs = 2;

void setup()
{ 
  // Begin Wire to enable I2C communication
  Wire.begin();

  // Begin Serial to enable recieve output from microcontroller to pc
  Serial.begin(38400);

  // Set AD0 pins for each MPU in order to change their address
  // Store the respective address within each individual IMU
  if(SERIAL_DEBUG){
    Serial.println("Set ADO pins");
  }

  for(int i = 0; i < numberOfMPUs; i++){
    // Set AD0 
    pinMode(AD0[i], OUTPUT);
    digitalWrite(AD0[i], i);
    delay(100);

    // Set address for each device per its ADO pin value
    if(i){
      mpu[i].set_i2c_address(MPU9250_ADDRESS_ADO_1);
    }else{
      mpu[i].set_i2c_address(MPU9250_ADDRESS_ADO_0);
    }
  }

  delay(100);

  // Setup both mpus
  if(SERIAL_DEBUG){
    Serial.println("Setup both MPUS");
  }
  for(int i = 0; i < numberOfMPUs; i++){
    mpu[i].initMPU9250();
  }

  if(SERIAL_DEBUG){
    Serial.println("End Setup");
  }
}

// Performance modification variables
int maxTempSample = 100;
int maxSample = 2000;
float max_offset_filter = 60.0;
float max_change_filter = 10.0;

int currentSample = 0;
int currentTempSample = 0;

unsigned long last = millis();
unsigned long now = last;
unsigned long change_in_time = 0;
unsigned long d_t = 50;

bool gotDataFrom_A = 0;
bool gotDataFrom_B = 0;
bool q_diff_found = 0;
bool static_offset_unknown = true;

float static_yaw_offset = 0.0;
float static_pitch_offset = 0.0;
float static_roll_offset = 0.0;
float dynamic_yaw_offset = 0.0;
float dynamic_pitch_offset = 0.0;
float dynamic_roll_offset = 0.0;

Quaternion *q_diff;
float yaw_offset = 0.0;
float pitch_offset = 0.0;
float roll_offset = 0.0;

float yaw_magnitude = 0.0;
float pitch_magnitude = 0.0;
float roll_magnitude = 0.0;
float last_yaw_magnitude = 0.0;
float last_pitch_magnitude = 0.0;
float last_roll_magnitude = 0.0;

float accumulated_yaw_offset = 0.0;
float accumulated_pitch_offset = 0.0;
float accumulated_roll_offset = 0.0;
float accumulated_yaw_offset_temp = 0.0;
float accumulated_pitch_offset_temp = 0.0;
float accumulated_roll_offset_temp = 0.0;
float accumulated_change_in_yaw_mag = 0.0;
float accumulated_change_in_pitch_mag = 0.0;
float accumulated_change_in_roll_mag = 0.0;

void loop()
{
  for(int i = 0; i < numberOfMPUs; i++){
    // Check if new data as been accumulated
    if (mpu[i].hasData())
    {
      mpu[i].retrieve_data();
      if(i == 0){
        gotDataFrom_A = 1;
      }else{
        gotDataFrom_B = 1;
      }
    }

    //updates time and the quaternion
    mpu[i].updateQuaternion();

    if(static_offset_unknown)
    {
      // Take the difference from the latest samples
      if(gotDataFrom_A && gotDataFrom_B){
        // Update flags
        q_diff_found = true;
        gotDataFrom_A = false;
        gotDataFrom_B = false;
        if(q_diff){
          free(q_diff);
          q_diff = NULL;
        }

        // Calculate difference between each sample
        q_diff = q_difference(mpu[0].q_m.q, mpu[1].q_m.q);

        // Convert quaternion difference into yaw, pitch roll
        yaw_offset = quaternionToYaw(q_diff);
        pitch_offset = quaternionToPitch(q_diff);
        roll_offset = quaternionToRoll(q_diff);

        // Determine the magnitudes of difference
        yaw_magnitude = sqrt(yaw_offset*yaw_offset);
        pitch_magnitude = sqrt(pitch_offset*pitch_offset);
        roll_magnitude = sqrt(roll_offset*roll_offset);

        // Remove erroneous values
        if((yaw_magnitude < max_offset_filter) && (pitch_magnitude < max_offset_filter) && (roll_magnitude < max_offset_filter)){
          // Accumulate changes in magnitudes
          accumulated_change_in_yaw_mag   += fabs(last_yaw_magnitude - yaw_magnitude);
          accumulated_change_in_pitch_mag += fabs(last_pitch_magnitude - pitch_magnitude);
          accumulated_change_in_roll_mag  += fabs(last_roll_magnitude - roll_magnitude);

          // Update the last magnitudes
          last_yaw_magnitude   = yaw_magnitude;
          last_pitch_magnitude = pitch_magnitude;
          last_roll_magnitude   = roll_magnitude;

          accumulated_yaw_offset_temp   += yaw_offset;
          accumulated_pitch_offset_temp += pitch_offset;
          accumulated_roll_offset_temp  += roll_offset;
          currentTempSample++;

          // Check each interval for convergence
          if(currentTempSample == maxTempSample){
            accumulated_change_in_yaw_mag   = accumulated_change_in_yaw_mag   / (float) maxTempSample;
            accumulated_change_in_pitch_mag = accumulated_change_in_pitch_mag / (float) maxTempSample;
            accumulated_change_in_roll_mag  = accumulated_change_in_roll_mag  / (float) maxTempSample;

            currentTempSample = 0;

            // Stable reading check
            if((accumulated_change_in_yaw_mag < max_change_filter) && (accumulated_change_in_pitch_mag < max_change_filter) && (accumulated_change_in_roll_mag < max_change_filter)){
              accumulated_yaw_offset   += accumulated_yaw_offset_temp;
              accumulated_pitch_offset += accumulated_pitch_offset_temp;
              accumulated_roll_offset  += accumulated_roll_offset_temp;
              accumulated_yaw_offset_temp = 0.0;
              accumulated_pitch_offset_temp = 0.0;
              accumulated_roll_offset_temp  = 0.0;
              accumulated_change_in_yaw_mag = 0.0;
              accumulated_change_in_pitch_mag = 0.0;
              accumulated_change_in_roll_mag = 0.0;

              currentSample += maxTempSample;

              if(currentSample >= maxSample){
                static_offset_unknown = false;
                static_yaw_offset   = accumulated_yaw_offset   / (float) (currentSample);
                static_pitch_offset = accumulated_pitch_offset / (float) (currentSample);
                static_roll_offset  = accumulated_roll_offset  / (float) (currentSample);

                dynamic_yaw_offset   = static_yaw_offset;
                dynamic_pitch_offset = static_pitch_offset;
                dynamic_roll_offset  = static_roll_offset;
              }

            }else{
              // reset
              accumulated_yaw_offset_temp   = 0.0;
              accumulated_pitch_offset_temp = 0.0;
              accumulated_roll_offset_temp  = 0.0;
              accumulated_change_in_yaw_mag = 0.0;
              accumulated_change_in_pitch_mag = 0.0;
              accumulated_change_in_roll_mag = 0.0;
            }
          }
        }
      }
    }
    else{
      // Take the difference from the latest samples
      if(gotDataFrom_A && gotDataFrom_B){
        // Update flags
        q_diff_found = true;
        gotDataFrom_A = false;
        gotDataFrom_B = false;
        if(q_diff){
          free(q_diff);
          q_diff = NULL;
        }

        // Calculate difference between each sample
        q_diff = q_difference(mpu[0].q_m.q, mpu[1].q_m.q);

        // Convert quaternion difference into yaw, pitch roll
        yaw_offset = quaternionToYaw(q_diff);
        pitch_offset = quaternionToPitch(q_diff);
        roll_offset = quaternionToRoll(q_diff);

        // Update dynamic offset
        dynamic_yaw_offset = (dynamic_yaw_offset + yaw_offset + static_yaw_offset) / 3.0;
        dynamic_pitch_offset = (dynamic_pitch_offset + pitch_offset + static_pitch_offset) / 3.0;
        dynamic_roll_offset = (dynamic_roll_offset + roll_offset + static_roll_offset) / 3.0;
      }

      // Output data
      now = millis();
      change_in_time = now - last;
      if(change_in_time > d_t){
        yaw_offset = quaternionToYaw(mpu[0].q_m.q);
        pitch_offset = quaternionToPitch(mpu[0].q_m.q);
        roll_offset = quaternionToRoll(mpu[0].q_m.q);

        last = millis();
        Serial.print(yaw_offset);
        Serial.print(",");
        Serial.print(pitch_offset);
        Serial.print(",");
        Serial.print(roll_offset);
        Serial.print(",");
        Serial.print(yaw_offset + dynamic_yaw_offset);
        Serial.print(",");
        Serial.print(pitch_offset + dynamic_pitch_offset);
        Serial.print(",");
        Serial.println(roll_offset + dynamic_roll_offset);
      }
    }
  }
}