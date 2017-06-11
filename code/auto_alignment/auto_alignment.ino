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

#define YAW 0
#define PITCH 1
#define ROLL 2
#define maxSample 60



void output_mag();
void output_gyro();
void output_acc();
void output_Yaw_Pitch_Roll();


// The two digital login pins on the MetroMini328 that will be used
// to set the AD0 pins of each MPU9250
int AD0[2] = {12, 13};

// The two mpus connected to the microcontroller
// One should have their AD0 pin set HIGH wheras the other should
// have their AD0 pin set LOW
MPU9250 mpu[2];

int numberOfMPUs = 2;
float samples[maxSample];

void setup()
{ 
  // Begin Wire to enable I2C communication
  Wire.begin();
  delay(1000);
  
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

  if(1){
    Serial.println("DONE");
  }

  for(int i = 0; i < maxSample; i++){
    samples[i] = 0.0;
  }

}
int c_sample=0;

unsigned long last = millis();
unsigned long now = last;
unsigned long change_in_time = 0;
//unsigned long d_t = 100;
unsigned long d_t = 20;

bool gotDataFrom_A = 0;
bool gotDataFrom_B = 0;
bool q_diff_found = 0;
bool static_offset_unknown = true;

float yaw_confidence_level = 0.0;
float pitch_confidence_level = 0.0;
float roll_confidence_level = 0.0;
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
float yaw = 0.0;
float pitch = 0.0;
float roll = 0.0;

int axis = YAW;
int j = 0;


float g_x = 0.0;
float g_y = 0.0;
float g_z = 0.0;
void loop()
{
  for(int i = 0; i < numberOfMPUs; i++){
    
    // Check if new data as been accumulated
    if (mpu[i].hasData()){ 
      //Serial.print(i);
      //Serial.println(" has data");
      mpu[i].retrieve_data();
      if(i == 0){
        gotDataFrom_A = 1;
      }else{
        gotDataFrom_B = 1;
      }
    }
    
    //updates time and the quaternion
    mpu[i].updateQuaternion();

    if(gotDataFrom_A == 1 && gotDataFrom_B == 1){
      q_diff_found = 1;
      gotDataFrom_A = 0;
      gotDataFrom_B = 0;
      now = millis();
      change_in_time = now - last;
      if(change_in_time > d_t){
        last = millis();
        output_Yaw_Pitch_Roll();
      }
    }

    /*
    if(static_offset_unknown){
      // Take the difference from the latest samples
      if(gotDataFrom_A == 1 && gotDataFrom_B == 1){
        // Update flags
        //q_diff_found = true;
        gotDataFrom_A = 0;
        gotDataFrom_B = 0;
        //if(q_diff){
        //  free(q_diff);
        //  q_diff = NULL;
        //}

        //Get the next sample
        switch(axis){
          case YAW:
            samples[c_sample] = (quaternionToYaw(mpu[0].q_m.q) - quaternionToYaw(mpu[1].q_m.q));
            break;
          case PITCH:
            samples[c_sample] = (quaternionToPitch(mpu[0].q_m.q) - quaternionToYaw(mpu[1].q_m.q));
            break;
          case ROLL:
            samples[c_sample] = (quaternionToRoll(mpu[0].q_m.q) - quaternionToRoll(mpu[1].q_m.q));
            break;
        }

        //Update current sample count
        c_sample = c_sample + 1;

        //Check if last sample was received
        if(c_sample == maxSample){

          for(int k = 0; k < maxSample; k++){
            Serial.println(samples[k]);
          }
          Serial.print("Max sample reached for ");
          switch(axis){
            case YAW:
              Serial.println("yaw.");
              break;

            case PITCH:
              Serial.println("pitch.");
              break;

            case ROLL:
              Serial.println("roll.");
              break;
          }

          //Get mean
          float mean = getMean(samples, maxSample);
          Serial.print("mean: ");
          Serial.println(mean);

          //Get standard deviation
          float standard_deviation = getStandardDeviation(samples, mean, maxSample);
          Serial.print("standard_deviation: ");
          Serial.println(standard_deviation);
          //Get confidence interval
          float confidence_level = getConfidenceInterval(standard_deviation, maxSample, 1.96);

          //Check confidence interval before continuing
          //Output results when passing
          if(confidence_level < 0.001){
            switch(axis){
              case YAW:
                yaw_confidence_level = confidence_level;
                static_yaw_offset = mean;
                axis = PITCH;
                c_sample = 0;

                Serial.print("Yaw offset: ");
                Serial.print(static_yaw_offset, 5);
                Serial.print(" +- ");
                Serial.println(yaw_confidence_level, 5);
                break;
              case PITCH:
                pitch_confidence_level = confidence_level;
                static_pitch_offset = mean;
                axis = ROLL;
                c_sample = 0;

                Serial.print("Pitch offset: ");
                Serial.print(static_pitch_offset, 5);
                Serial.print(" +- ");
                Serial.println(pitch_confidence_level, 5);
                break;
              case ROLL:
                roll_confidence_level = confidence_level;
                static_roll_offset = mean;
                static_offset_unknown = false;

                Serial.print("Roll offset: ");
                Serial.print(static_roll_offset, 5);
                Serial.print(" +- ");
                Serial.println(roll_confidence_level, 5);
                Serial.println("DONE");
                numberOfMPUs = 1;
                //free(samples);
                break;
            }
          }else{
            Serial.println("Confidence level insufficient");
            c_sample = 0;
          }//confidence interval
        }//max sample reached
      }//both have data
    }//static offset unknown
    else{
      // Output data
      now = millis();
      change_in_time = now - last;
      if(change_in_time > d_t){
        last = millis();
        yaw = quaternionToYaw(mpu[0].q_m.q);// * PI / 180.0f;
        pitch = quaternionToPitch(mpu[0].q_m.q);// * PI / 180.0f;
        roll = quaternionToRoll(mpu[0].q_m.q);//  * PI / 180.0f;

        //Output 'Airplane data'
        Serial.print(yaw, 5);
        Serial.print(",");
        Serial.print(pitch, 5);
        Serial.print(",");
        Serial.print(roll, 5);
        Serial.print(",");
        //Output 'Corrected data'
        Serial.print(yaw + static_yaw_offset, 5);
        Serial.print(",");
        Serial.print(pitch + static_pitch_offset, 5);
        Serial.print(",");
        Serial.println(roll + static_roll_offset, 5);
      }// d_t
    }
    */
  }//for

}//loop



void output_mag(){
  Serial.print(mpu[0].mx);
  Serial.print(",");
  Serial.print(mpu[0].my);
  Serial.print(",");
  Serial.print(mpu[0].mz);
  Serial.print(",");
  Serial.print(mpu[1].mx);
  Serial.print(",");
  Serial.print(mpu[1].my);
  Serial.print(",");
  Serial.println(mpu[1].mz);
}

void output_gyro(){
  Serial.print(mpu[0].gy);
  Serial.print(",");
  Serial.println(mpu[1].gy);
}

void output_acc(){
  Serial.print(mpu[0].ax);
  Serial.print(",");
  Serial.print(mpu[0].ay);
  Serial.print(",");
  Serial.print(mpu[0].az);
  Serial.print(",");
  Serial.print(mpu[1].ax);
  Serial.print(",");
  Serial.print(mpu[1].ay);
  Serial.print(",");
  Serial.println(mpu[1].az);
}

void output_Yaw_Pitch_Roll(){
  float yaw_0 = quaternionToYaw(mpu[0].q_m.q) * RAD_TO_DEG;
  float pitch_0 = quaternionToPitch(mpu[0].q_m.q) * RAD_TO_DEG;
  float roll_0 = quaternionToRoll(mpu[0].q_m.q) * RAD_TO_DEG;
  float yaw_1 = quaternionToYaw(mpu[1].q_m.q) * RAD_TO_DEG;
  float pitch_1 = quaternionToPitch(mpu[1].q_m.q) * RAD_TO_DEG;
  float roll_1 = quaternionToRoll(mpu[1].q_m.q) * RAD_TO_DEG;
  Serial.print(yaw_0);
  Serial.print(",");
  Serial.print(pitch_0);
  Serial.print(",");
  Serial.print(roll_0);
  Serial.print(",");
  Serial.print(yaw_1);
  Serial.print(",");
  Serial.print(pitch_1);
  Serial.print(",");
  Serial.println(roll_1);
}