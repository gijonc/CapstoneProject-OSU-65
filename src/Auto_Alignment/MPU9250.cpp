#include "stdafx.h"

//==============================================================================
//====== Set of useful function to access acceleration. gyroscope, magnetometer,
//====== and temperature data
//==============================================================================

// If intPin goes high, all data registers have new data
// On interrupt, check if data ready interrupt
bool MPU9250::hasData(){
  return (readByte(i2c_address, INT_STATUS) & 0x01);
}

void MPU9250::getMres()
{
  switch (Mscale)
  {
    // Possible magnetometer scales (and their register bit settings) are:
    // 14 bit resolution (0) and 16 bit resolution (1)
    case MFS_14BITS:
      mRes = 10.0f * 4912.0f / 8190.0f; // Proper scale to return milliGauss
      break;
    case MFS_16BITS:
      mRes = 10.0f * 4912.0f / 32760.0f; // Proper scale to return milliGauss
      break;
  }
}

void MPU9250::getGres()
{
  switch (Gscale)
  {
    // Possible gyro scales (and their register bit settings) are:
    // 250 DPS (00), 500 DPS (01), 1000 DPS (10), and 2000 DPS (11).
    // Here's a bit of an algorith to calculate DPS/(ADC tick) based on that
    // 2-bit value:
    case GFS_250DPS:
      gRes = 250.0f / 32768.0f;
      break;
    case GFS_500DPS:
      gRes = 500.0f / 32768.0f;
      break;
    case GFS_1000DPS:
      gRes = 1000.0f / 32768.0f;
      break;
    case GFS_2000DPS:
      gRes = 2000.0f / 32768.0f;
      break;
  }
}

void MPU9250::getAres()
{
  switch (Ascale)
  {
    // Possible accelerometer scales (and their register bit settings) are:
    // 2 Gs (00), 4 Gs (01), 8 Gs (10), and 16 Gs  (11).
    // Here's a bit of an algorith to calculate DPS/(ADC tick) based on that
    // 2-bit value:
    case AFS_2G:
      aRes = 2.0f / 32768.0f;
      break;
    case AFS_4G:
      aRes = 4.0f / 32768.0f;
      break;
    case AFS_8G:
      aRes = 8.0f / 32768.0f;
      break;
    case AFS_16G:
      aRes = 16.0f / 32768.0f;
      break;
  }
}

uint8_t MPU9250::getAscale(){
  return Ascale;
}

uint8_t MPU9250::getGscale(){
  return Gscale;
}

uint8_t MPU9250::getMscale(){
  return Mscale;
}

uint8_t MPU9250::getM_Mode(){
  return Mmode;
}

uint16_t MPU9250::getSampleCount(){
  uint16_t sample_count = 0;
  // shoot for ~fifteen seconds of mag data
  // at 8 Hz ODR, new mag data is available every 125 ms
  if (Mmode == M_8HZ)
  {
    sample_count = 128;
  }
  // at 100 Hz ODR, new mag data is available every 10 ms
  if (Mmode == M_100HZ)
  {
    sample_count = 1500;
  }

  return sample_count;
}

uint16_t MPU9250::getSampleDelay(){
  uint16_t sample_delay = 0;

  if (Mmode == M_8HZ)
  {
    sample_delay = 135; // At 8 Hz ODR, new mag data is available every 125 ms
  }
  if (Mmode == M_100HZ)
  {
    sample_delay = 12;  // At 100 Hz ODR, new mag data is available every 10 ms
  }

  return sample_delay;
}


bool MPU9250::set_i2c_address(uint8_t address){
  i2c_address = address;
}

bool MPU9250::initMPU9250_Magnetometer(){
  // First extract the factory calibration for each magnetometer axis
  uint8_t rawData[3];  // x/y/z gyro calibration data stored here

  // Point to magnetometer before writing data
  // Set to write to slave address AK8963_ADDRESS
  writeByte(i2c_address, I2C_SLV0_ADDR, AK8963_ADDRESS);
  delay(delay_s);
  
  // Now point to a megnetometer register that data will be written to
  // Point slave 0 register at AK8963's control 2 (soft reset) register
  writeByte(i2c_address, I2C_SLV0_REG, AK8963_CNTL2);
  delay(delay_s);

  // Send data
  // Soft restart via AK8963's control 2
  writeByte(i2c_address, I2C_SLV0_DO, CNTL2_SOFT_RESTART);
  delay(delay_s);

  // Point to a different magnetometer register for a seperate write
  // Point slave 0 register at AK8963's control 1 (mode) register
  writeByte(i2c_address, I2C_SLV0_REG, AK8963_CNTL1);
  delay(delay_s);
  
  // Send data
  // Enter Fuse ROM access mode
  writeByte(i2c_address, I2C_SLV0_DO, CNTL1_FUSE_ROM_MODE);
  delay(delay_s);

  // Enable read from magnometer
  // Set to read from slave address AK8963_ADDRESS
  writeByte(i2c_address, I2C_SLV0_ADDR, AK8963_ADDRESS | READ_FLAG);
  delay(delay_s);

  // Now point to a megnetometer register that data will be read from
  // Point slave 0 register at AK8963's fuse rom x-axis
  writeByte(i2c_address, I2C_SLV0_REG, AK8963_ASAX);
  delay(delay_s);

  // Enable simple 3-byte I2C read from slave 0
  writeByte(i2c_address, I2C_SLV0_CTRL, READ_FLAG | 0x03);
  delay(delay_l);

  // Read the x-, y-, and z-axis calibration values
  readBytes(i2c_address, EXT_SENS_DATA_00, 3, &rawData[0]);
  delay(delay_l);

  if(SERIAL_DEBUG){
    Serial.print("rawData[0]: "); Serial.print(rawData[0]);
    Serial.print(" ");
    Serial.print("rawData[1]: "); Serial.print(rawData[1]);
    Serial.print(" ");
    Serial.print("rawData[2]: "); Serial.print(rawData[2]);
    Serial.println("");
  }

  //Return x-axis sensitivity adjustment values, etc.
  factoryMagCalibration[0] =  (float)(rawData[0] - 128)/256. + 1.;
  factoryMagCalibration[1] =  (float)(rawData[1] - 128)/256. + 1.;
  factoryMagCalibration[2] =  (float)(rawData[2] - 128)/256. + 1.;

  if(SERIAL_DEBUG){
    for(int i = 0; i < 3; i++){
    Serial.print("factoryMagCalibration[");
    Serial.print(i);
    Serial.print("]: ");
    Serial.print(factoryMagCalibration[i]);
    Serial.println();
    }
  }

  // Set to write to slave address AK8963_ADDRESS
  writeByte(i2c_address, I2C_SLV0_ADDR, AK8963_ADDRESS);
  delay(delay_s);
  
  // Point slave 0 register at AK8963's control 1 (mode) register
  writeByte(i2c_address, I2C_SLV0_REG, AK8963_CNTL1);
  delay(delay_s);

  // 16-bit continuous measurement mode via AK8963's control 1
  writeByte(i2c_address, I2C_SLV0_DO, (CNTL1_16BIT_OUTPUT | CNTL1_CONTINUOUS_MEASUREMENT_MODE_2));
  delay(delay_l);

  // Setup for read
  writeByte(i2c_address, I2C_SLV0_ADDR, AK8963_ADDRESS | READ_FLAG);

  // Setup to read actual mag data
  // Point slave to the data regs
  writeByte(i2c_address, I2C_SLV0_REG, AK8963_XOUT_L);
  delay(delay_s);

  // Enable simple 7-byte I2C reads from slave 0
  writeByte(i2c_address, I2C_SLV0_CTRL, READ_FLAG | 0x07);
  delay(delay_s);

}

bool MPU9250::initMPU9250_Accelerometer(){
  // Set accelerometer full-scale range configuration
  // Get current ACCEL_CONFIG register value
  uint8_t c = readByte(i2c_address, ACCEL_CONFIG);
  // c = c & ~0xE0; // Clear self-test bits [7:5]
  c = c & ~0x18;  // Clear AFS bits [4:3]
  c = c | getAscale() << 3; // Set full scale range for the accelerometer
  // Write new ACCEL_CONFIG register value
  writeByte(i2c_address, ACCEL_CONFIG, c);

  // Set accelerometer sample rate configuration
  // It is possible to get a 4 kHz sample rate from the accelerometer by
  // choosing 1 for accel_fchoice_b bit [3]; in this case the bandwidth is
  // 1.13 kHz
  // Get current ACCEL_CONFIG2 register value
  c = readByte(i2c_address, ACCEL_CONFIG2);
  c = c & ~0x0F; // Clear accel_fchoice_b (bit 3) and A_DLPFG (bits [2:0])
  c = c | 0x03;  // Set accelerometer rate to 1 kHz and bandwidth to 41 Hz
  // Write new ACCEL_CONFIG2 register value
  writeByte(i2c_address, ACCEL_CONFIG2, c);
}

bool MPU9250::initMPU9250_Gyroscope(){
  // Configure Gyro and Thermometer
  // Disable FSYNC and set thermometer and gyro bandwidth to 41 and 42 Hz,
  // respectively;
  // minimum delay time for this setting is 5.9 ms, which means sensor fusion
  // update rates cannot be higher than 1 / 0.0059 = 170 Hz
  // DLPF_CFG = bits 2:0 = 011; this limits the sample rate to 1000 Hz for both
  // With the MPU9250, it is possible to get gyro sample rates of 32 kHz (!),
  // 8 kHz, or 1 kHz
  writeByte(i2c_address, CONFIG, 0x03);

  // Set sample rate = gyroscope output rate/(1 + SMPLRT_DIV)
  // Use a 200 Hz rate; a rate consistent with the filter update rate
  // determined inset in CONFIG above.
  writeByte(i2c_address, SMPLRT_DIV, 0x04);

  // Set gyroscope full scale range
  // Range selects FS_SEL and AFS_SEL are 0 - 3, so 2-bit values are
  // left-shifted into positions 4:3

  // get current GYRO_CONFIG register value
  uint8_t c = readByte(i2c_address, GYRO_CONFIG);
  // c = c & ~0xE0; // Clear self-test bits [7:5]
  c = c & ~0x02; // Clear Fchoice bits [1:0]
  c = c & ~0x18; // Clear AFS bits [4:3]
  c = c | getGscale() << 3; // Set full scale range for the gyro
  // Set Fchoice for the gyro to 11 by writing its inverse to bits 1:0 of
  // GYRO_CONFIG
  // c =| 0x00;
  // Write new GYRO_CONFIG value to register
  writeByte(i2c_address, GYRO_CONFIG, c );

}

//Copied from xxx
bool MPU9250::calibrateMPU9250_Magnetometer(){
  uint16_t ii = 0, sample_count = 0, sample_delay = 0;
  int32_t mag_bias[3]  = {0, 0, 0},
          mag_scale[3] = {0, 0, 0};
  int16_t mag_max[3]  = {0x8000, 0x8000, 0x8000},
          mag_min[3]  = {0x7FFF, 0x7FFF, 0x7FFF};

  // Make sure resolution has been calculated
  getMres();

  if(SERIAL_DEBUG){
    Serial.println(F("Mag Calibration: Wave device in a figure 8 until done!"));
    Serial.println(F("  4 seconds to get ready followed by 15 seconds of sampling)"));
  }
  delay(4000);

  // Determine the number of samples to take
  sample_count = 128;//getSampleCount();

  // Determine the delay between each sample
  sample_delay = getSampleDelay();

  for (ii = 0; ii < sample_count; ii++)
  {
    read_Magnetometer();

    for (int jj = 0; jj < 3; jj++)
    {
      if (magCount[jj] > mag_max[jj])
      {
        mag_max[jj] = magCount[jj];
      }
      if (magCount[jj] < mag_min[jj])
      {
        mag_min[jj] = magCount[jj];
      }
    }

    delay(13);
  }

  if(SERIAL_DEBUG){
    Serial.println("mag x min/max:"); Serial.println(mag_max[0]); Serial.println(mag_min[0]);
    Serial.println("mag y min/max:"); Serial.println(mag_max[1]); Serial.println(mag_min[1]);
    Serial.println("mag z min/max:"); Serial.println(mag_max[2]); Serial.println(mag_min[2]);
  }

  // Get hard iron correction
  // Get 'average' x mag bias in counts
  mag_bias[0]  = (mag_max[0] + mag_min[0]) / 2;
  // Get 'average' y mag bias in counts
  mag_bias[1]  = (mag_max[1] + mag_min[1]) / 2;
  // Get 'average' z mag bias in counts
  mag_bias[2]  = (mag_max[2] + mag_min[2]) / 2;

  // Save mag biases in G for main program
  magBias[0] = (float)mag_bias[0] * mRes * factoryMagCalibration[0];
  magBias[1] = (float)mag_bias[1] * mRes * factoryMagCalibration[1];
  magBias[2] = (float)mag_bias[2] * mRes * factoryMagCalibration[2];

  // Get soft iron correction estimate
  // Get average x axis max chord length in counts
  mag_scale[0]  = (mag_max[0] - mag_min[0]) / 2;
  // Get average y axis max chord length in counts
  mag_scale[1]  = (mag_max[1] - mag_min[1]) / 2;
  // Get average z axis max chord length in counts
  mag_scale[2]  = (mag_max[2] - mag_min[2]) / 2;

  float avg_rad = mag_scale[0] + mag_scale[1] + mag_scale[2];
  avg_rad /= 3.0;

  magScale[0] = avg_rad / ((float)mag_scale[0]);
  magScale[1] = avg_rad / ((float)mag_scale[1]);
  magScale[2] = avg_rad / ((float)mag_scale[2]);

  if(SERIAL_DEBUG){
    Serial.println(F("Mag Calibration done!"));
  }
}

bool MPU9250::calibrateMPU9250_Accelerometer(){
  uint8_t data[6]; // data array to hold accelerometer data
  uint16_t ii, packet_count, fifo_count;
  int32_t accel_bias[3] = {0, 0, 0};
  int16_t acc_max[3]  = {0x8000, 0x8000, 0x8000},
          acc_min[3]  = {0x7FFF, 0x7FFF, 0x7FFF};

  if(SERIAL_DEBUG){
    Serial.println("calibrateMPU9250_Accelerometer");
  }

  // Make sure resolution has been calculated
  getAres();

  // reset device
  // Write a one to bit 7 reset bit; toggle reset device
  writeByte(i2c_address, PWR_MGMT_1, READ_FLAG);
  delay(100);

  // get stable time source; Auto select clock source to be PLL gyroscope
  // reference if ready else use the internal oscillator, bits 2:0 = 001
  writeByte(i2c_address, PWR_MGMT_1, 0x01);
  writeByte(i2c_address, PWR_MGMT_2, 0x00);
  delay(200);

  // Configure device for bias calculation
  // Disable all interrupts
  writeByte(i2c_address, INT_ENABLE, 0x00);
  // Disable FIFO
  writeByte(i2c_address, FIFO_EN, 0x00);
  // Turn on internal clock source
  writeByte(i2c_address, PWR_MGMT_1, 0x00);
  // Disable I2C master
  writeByte(i2c_address, I2C_MST_CTRL, 0x00);
  // Disable FIFO and I2C master modes
  writeByte(i2c_address, USER_CTRL, 0x00);
  // Reset FIFO and DMP
  writeByte(i2c_address, USER_CTRL, 0x0C);
  delay(15);

  // Configure MPU6050 gyro and accelerometer for bias calculation
  // Set low-pass filter to 188 Hz
  writeByte(i2c_address, CONFIG, 0x01);
  // Set sample rate to 1 kHz
  writeByte(i2c_address, SMPLRT_DIV, 0x00);
  // Set accelerometer full-scale to 2 g, maximum sensitivity
  writeByte(i2c_address, ACCEL_CONFIG, 0x00);

  uint16_t  accelsensitivity = 16384; // = 16384 LSB/g

  // Configure FIFO to capture accelerometer data for bias calculation
  writeByte(i2c_address, USER_CTRL, 0x40);  // Enable FIFO
  // Enable accelerometer sensors for FIFO  (max size 512 bytes in
  // MPU-9150)
  writeByte(i2c_address, FIFO_EN, 0x08);
  delay(80);  // accumulate 80 samples in 80 milliseconds = 480 bytes

  // At end of sample accumulation, turn off FIFO sensor read
  // Disable accelerometer sensors for FIFO
  writeByte(i2c_address, FIFO_EN, 0x00);
  // Read FIFO sample count
  readBytes(i2c_address, FIFO_COUNTH, 2, &data[0]);
  fifo_count = ((uint16_t)data[0] << 8) | data[1];
  // How many sets of full accelerometer data for averaging

  packet_count = fifo_count/6;
  if(SERIAL_DEBUG){
    Serial.print("fifo_count: ");
    Serial.println(fifo_count);
    Serial.print("packet_count: ");
    Serial.println(packet_count);
  }

  for (ii = 0; ii < packet_count; ii++)
  {
    int16_t accel_temp[3] = {0, 0, 0};
    // Read data for averaging
    readBytes(i2c_address, FIFO_R_W, 6, &data[0]);
    // Form signed 16-bit integer for each sample in FIFO
    accel_temp[0] = (int16_t) (((int16_t)data[0] << 8) | data[1]  );
    accel_temp[1] = (int16_t) (((int16_t)data[2] << 8) | data[3]  );
    accel_temp[2] = (int16_t) (((int16_t)data[4] << 8) | data[5]  );

    // Sum individual signed 16-bit biases to get accumulated signed 32-bit
    // biases.
    accel_bias[0] += (int32_t) accel_temp[0];
    accel_bias[1] += (int32_t) accel_temp[1];
    accel_bias[2] += (int32_t) accel_temp[2];

    for (int jj = 0; jj < 3; jj++)
    {
      if (accel_temp[jj] > acc_max[jj])
      {
        acc_max[jj] = accel_temp[jj];
      }
      if (accel_temp[jj] < acc_min[jj])
      {
        acc_min[jj] = accel_temp[jj];
      }
    }
  }

  if(SERIAL_DEBUG){
    Serial.println("acc x min/max:"); Serial.println(acc_max[0]); Serial.println(acc_min[0]);
    Serial.println("acc y min/max:"); Serial.println(acc_max[1]); Serial.println(acc_min[1]);
    Serial.println("acc z min/max:"); Serial.println(acc_max[2]); Serial.println(acc_min[2]);
  }

  // Sum individual signed 16-bit biases to get accumulated signed 32-bit biases
  accel_bias[0] /= (int32_t) packet_count;
  accel_bias[1] /= (int32_t) packet_count;
  accel_bias[2] /= (int32_t) packet_count;

  // Sum individual signed 16-bit biases to get accumulated signed 32-bit biases
  if (accel_bias[2] > 0L)
  {
    accel_bias[2] -= (int32_t) accelsensitivity;
  }
  else
  {
    accel_bias[2] += (int32_t) accelsensitivity;
  }

  // Construct the accelerometer biases for push to the hardware accelerometer
  // bias registers. These registers contain factory trim values which must be
  // added to the calculated accelerometer biases; on boot up these registers
  // will hold non-zero values. In addition, bit 0 of the lower byte must be
  // preserved since it is used for temperature compensation calculations.
  // Accelerometer bias registers expect bias input as 2048 LSB per g, so that
  // the accelerometer biases calculated above must be divided by 8.

  // A place to hold the factory accelerometer trim biases
  int32_t accel_bias_reg[3] = {0, 0, 0};
  // Read factory accelerometer trim values
  readBytes(i2c_address, XA_OFFSET_H, 2, &data[0]);
  accel_bias_reg[0] = (int32_t) (((int16_t)data[0] << 8) | data[1]);
  readBytes(i2c_address, YA_OFFSET_H, 2, &data[0]);
  accel_bias_reg[1] = (int32_t) (((int16_t)data[0] << 8) | data[1]);
  readBytes(i2c_address, ZA_OFFSET_H, 2, &data[0]);
  accel_bias_reg[2] = (int32_t) (((int16_t)data[0] << 8) | data[1]);

  // Define mask for temperature compensation bit 0 of lower byte of
  // accelerometer bias registers
  uint32_t mask = 1uL;
  // Define array to hold mask bit for each accelerometer bias axis
  uint8_t mask_bit[3] = {0, 0, 0};

  for (ii = 0; ii < 3; ii++)
  {
    // If temperature compensation bit is set, record that fact in mask_bit
    if ((accel_bias_reg[ii] & mask))
    {
      mask_bit[ii] = 0x01;
    }
  }

  // Construct total accelerometer bias, including calculated average
  // accelerometer bias from above
  // Subtract calculated averaged accelerometer bias scaled to 2048 LSB/g
  // (16 g full scale)
  accel_bias_reg[0] -= (accel_bias[0]/8);
  accel_bias_reg[1] -= (accel_bias[1]/8);
  accel_bias_reg[2] -= (accel_bias[2]/8);

  data[0] = (accel_bias_reg[0] >> 8) & 0xFF;
  data[1] = (accel_bias_reg[0])      & 0xFF;
  // preserve temperature compensation bit when writing back to accelerometer
  // bias registers
  data[1] = data[1] | mask_bit[0];
  data[2] = (accel_bias_reg[1] >> 8) & 0xFF;
  data[3] = (accel_bias_reg[1])      & 0xFF;
  // Preserve temperature compensation bit when writing back to accelerometer
  // bias registers
  data[3] = data[3] | mask_bit[1];
  data[4] = (accel_bias_reg[2] >> 8) & 0xFF;
  data[5] = (accel_bias_reg[2])      & 0xFF;
  // Preserve temperature compensation bit when writing back to accelerometer
  // bias registers
  data[5] = data[5] | mask_bit[2];

  // Apparently this is not working for the acceleration biases in the MPU-9250
  // Are we handling the temperature correction bit properly?
  // Push accelerometer biases to hardware registers
  writeByte(i2c_address, XA_OFFSET_H, data[0]);
  writeByte(i2c_address, XA_OFFSET_L, data[1]);
  writeByte(i2c_address, YA_OFFSET_H, data[2]);
  writeByte(i2c_address, YA_OFFSET_L, data[3]);
  writeByte(i2c_address, ZA_OFFSET_H, data[4]);
  writeByte(i2c_address, ZA_OFFSET_L, data[5]);

  // Output scaled accelerometer biases for display in the main program
  accelBias[0] = (float)accel_bias[0]/(float)accelsensitivity;
  accelBias[1] = (float)accel_bias[1]/(float)accelsensitivity;
  accelBias[2] = (float)accel_bias[2]/(float)accelsensitivity;

  if(SERIAL_DEBUG){
    for(int i = 0; i < 3; i++){
      Serial.print("accelBias[");
      Serial.print(i);
      Serial.print("] = ");
      Serial.println(accelBias[i]);
    }
  }
}

bool MPU9250::calibrateMPU9250_Gyroscope(){
  uint8_t data[6]; // data array to hold gyro x, y, z, data
  uint16_t ii, packet_count, fifo_count;
  int32_t gyro_bias[3]  = {0, 0, 0};
  int16_t gyro_max[3]  = {0x8000, 0x8000, 0x8000},
          gyro_min[3]  = {0x7FFF, 0x7FFF, 0x7FFF};

  if(SERIAL_DEBUG){
    Serial.println("calibrateMPU9250_Gyroscope");
  }

  // Make sure resolution has been calculated
  getGres();

  // reset device
  // Write a one to bit 7 reset bit; toggle reset device
  writeByte(i2c_address, PWR_MGMT_1, READ_FLAG);
  delay(100);

  // get stable time source; Auto select clock source to be PLL gyroscope
  // reference if ready else use the internal oscillator, bits 2:0 = 001
  writeByte(i2c_address, PWR_MGMT_1, 0x01);
  writeByte(i2c_address, PWR_MGMT_2, 0x00);
  delay(200);

  // Configure device for bias calculation
  // Disable all interrupts
  writeByte(i2c_address, INT_ENABLE, 0x00);
  // Disable FIFO
  writeByte(i2c_address, FIFO_EN, 0x00);
  // Turn on internal clock source
  writeByte(i2c_address, PWR_MGMT_1, 0x00);
  // Disable I2C master
  writeByte(i2c_address, I2C_MST_CTRL, 0x00);
  // Disable FIFO and I2C master modes
  writeByte(i2c_address, USER_CTRL, 0x00);
  // Reset FIFO and DMP
  writeByte(i2c_address, USER_CTRL, 0x0C);
  delay(15);

  // Configure MPU6050 gyro and accelerometer for bias calculation
  // Set low-pass filter to 188 Hz
  writeByte(i2c_address, CONFIG, 0x01);
  // Set sample rate to 1 kHz
  writeByte(i2c_address, SMPLRT_DIV, 0x00);
  // Set gyro full-scale to 250 degrees per second, maximum sensitivity
  writeByte(i2c_address, GYRO_CONFIG, 0x00);

  uint16_t  gyrosensitivity  = 131;   // = 131 LSB/degrees/sec

  // Configure FIFO to capture gyro data for bias calculation
  writeByte(i2c_address, USER_CTRL, 0x40);  // Enable FIFO
  // Enable gyro and accelerometer sensors for FIFO  (max size 512 bytes in
  // MPU-9150)

  // Enable gyro sensor for FIFO
  writeByte(i2c_address, FIFO_EN, 0x70);
  delay(80);  // accumulate 80 samples in 80 milliseconds = 480 bytes

  // At end of sample accumulation, turn off FIFO sensor read
  // Disable gyro sensor for FIFO
  writeByte(i2c_address, FIFO_EN, 0x00);
  // Read FIFO sample count
  readBytes(i2c_address, FIFO_COUNTH, 2, &data[0]);
  fifo_count = ((uint16_t)data[0] << 8) | data[1];
  // How many sets of full gyro and accelerometer data for averaging
  packet_count = fifo_count/6;

  packet_count = fifo_count/6;
  if(SERIAL_DEBUG){
    Serial.print("fifo_count: ");
    Serial.println(fifo_count);
    Serial.print("packet_count: ");
    Serial.println(packet_count);
  }
  for (ii = 0; ii < packet_count; ii++)
  {
    int16_t gyro_temp[3] = {0, 0, 0};
    // Read data for averaging
    readBytes(i2c_address, FIFO_R_W, 6, &data[0]);
    // Form signed 16-bit integer for each sample in FIFO
    gyro_temp[0] = (int16_t) (((int16_t)data[0] << 8) | data[1]  );
    gyro_temp[1] = (int16_t) (((int16_t)data[2] << 8) | data[3]  );
    gyro_temp[2] = (int16_t) (((int16_t)data[4] << 8) | data[5]  );

    // Sum individual signed 16-bit biases to get accumulated signed 32-bit
    // biases.
    gyro_bias[0]  += (int32_t) gyro_temp[0];
    gyro_bias[1]  += (int32_t) gyro_temp[1];
    gyro_bias[2]  += (int32_t) gyro_temp[2];

  for (int jj = 0; jj < 3; jj++)
    {
      if (gyro_temp[jj] > gyro_max[jj])
      {
        gyro_max[jj] = gyro_temp[jj];
      }
      if (gyro_temp[jj] < gyro_min[jj])
      {
        gyro_min[jj] = gyro_temp[jj];
      }
    }
  }

  if(SERIAL_DEBUG){
    Serial.println("gyro x min/max:"); Serial.println(gyro_max[0]); Serial.println(gyro_min[0]);
    Serial.println("gyro y min/max:"); Serial.println(gyro_max[1]); Serial.println(gyro_min[1]);
    Serial.println("gyro z min/max:"); Serial.println(gyro_max[2]); Serial.println(gyro_min[2]);
  }

  // Sum individual signed 16-bit biases to get accumulated signed 32-bit biases
  gyro_bias[0]  /= (int32_t) packet_count;
  gyro_bias[1]  /= (int32_t) packet_count;
  gyro_bias[2]  /= (int32_t) packet_count;

  // Construct the gyro biases for push to the hardware gyro bias registers,
  // which are reset to zero upon device startup.
  // Divide by 4 to get 32.9 LSB per deg/s to conform to expected bias input
  // format.
  data[0] = (-gyro_bias[0]/4  >> 8) & 0xFF;
  // Biases are additive, so change sign on calculated average gyro biases
  data[1] = (-gyro_bias[0]/4)       & 0xFF;
  data[2] = (-gyro_bias[1]/4  >> 8) & 0xFF;
  data[3] = (-gyro_bias[1]/4)       & 0xFF;
  data[4] = (-gyro_bias[2]/4  >> 8) & 0xFF;
  data[5] = (-gyro_bias[2]/4)       & 0xFF;

  // Push gyro biases to hardware registers
  writeByte(i2c_address, XG_OFFSET_H, data[0]);
  writeByte(i2c_address, XG_OFFSET_L, data[1]);
  writeByte(i2c_address, YG_OFFSET_H, data[2]);
  writeByte(i2c_address, YG_OFFSET_L, data[3]);
  writeByte(i2c_address, ZG_OFFSET_H, data[4]);
  writeByte(i2c_address, ZG_OFFSET_L, data[5]);

  // Output scaled gyro biases for display in the main program
  gyroBias[0] = (float) gyro_bias[0]/(float) gyrosensitivity;
  gyroBias[1] = (float) gyro_bias[1]/(float) gyrosensitivity;
  gyroBias[2] = (float) gyro_bias[2]/(float) gyrosensitivity;
  if(SERIAL_DEBUG){
    for(int i = 0; i < 3; i++){
      Serial.print("gyroBias[");
      Serial.print(i);
      Serial.print("] = ");
      Serial.println(gyroBias[i]);
    }
  }
}

void MPU9250::initMPU9250()
{
  // Initialize IMU
  // Reset registers
  writeByte(i2c_address, PWR_MGMT_1, 0x80);
  delay(100);
  
  // Disable sensors
  // Disabled by reset?
  writeByte(i2c_address, PWR_MGMT_2, 0x1F);
  delay(100);
  
  // Disable slaves
  writeByte(i2c_address, I2C_SLV0_CTRL, 0x00);
  delay(100);
  
  // Disable bypass mode
  // 0x22?
  writeByte(i2c_address, INT_PIN_CFG, 0x00);
  delay(100);
  
  // Get stable time source
  // Auto select clock source to be PLL gyroscope reference if ready else
  writeByte(i2c_address, PWR_MGMT_1, 0x00);
  delay(200);

  // Delays the data ready interrupt until external sensor data is loaded
  // Stop between reads when transitioning from one slave read to the next
  // I2C master clock speed = 400 kHz
  // 8MHz Clock Divider = 20
  writeByte(i2c_address, I2C_MST_CTRL, 0x0D);
  delay(100);

  // Enable I2C master
  writeByte(i2c_address, USER_CTRL, 0x20);
  delay(100);

  // Calibrate gyroscope
  calibrateMPU9250_Gyroscope();
  delay(100);

  // Calibrate accelerometer
  calibrateMPU9250_Accelerometer();
  delay(100);

  // Initialize gyroscope
  initMPU9250_Gyroscope();
  delay(100);

  // Temp to ensure regs are still set...
  // Initialize accelerometer
  initMPU9250_Accelerometer();
  delay(100);

  // Disable bypass mode
  // 0x22?
  writeByte(i2c_address, INT_PIN_CFG, 0x00);
  delay(100);

  // Delays the data ready interrupt until external sensor data is loaded
  // Stop between reads when transitioning from one slave read to the next
  // I2C master clock speed = 400 kHz
  // 8MHz Clock Divider = 20
  writeByte(i2c_address, I2C_MST_CTRL, 0x0D);
  delay(100);

  // Enable I2C master
  writeByte(i2c_address, USER_CTRL, 0x20);
  delay(100);

  // Initialize magnetometer
  initMPU9250_Magnetometer();
  delay(100);

  // Calibrate magnetometer
  calibrateMPU9250_Magnetometer();
  delay(100);
}


bool MPU9250::read_Magnetometer()
{
  // x/y/z gyro register data, ST2 register stored here, must read ST2 at end
  // of data acquisition
  uint8_t rawData[7];

  // Read the six raw data and ST2 registers sequentially into data array
  readBytes(i2c_address, EXT_SENS_DATA_00, 7, &rawData[0]);
  //delay(delay_l);
  uint8_t c = rawData[6]; // End data read by reading ST2 register
  // Check if magnetic sensor overflow set, if not then report data
  if (!(c & 0x08))
  {
    // Turn the MSB and LSB into a signed 16-bit value
    magCount[0] = ((int16_t)rawData[1] << 8) | rawData[0];
    // Data stored as little Endian
    magCount[1] = ((int16_t)rawData[3] << 8) | rawData[2];
    magCount[2] = ((int16_t)rawData[5] << 8) | rawData[4];
  }

  //TODO: update on read success
  return true;
}

bool MPU9250::read_Accelerometer(){
  uint8_t rawData[6];  // x/y/z accel register data stored here
  // Read the six raw data registers into data array
  readBytes(i2c_address, ACCEL_XOUT_H, 6, &rawData[0]);

  // Turn the MSB and LSB into a signed 16-bit value
  accelCount[0] = ((int16_t)rawData[0] << 8) | rawData[1] ;
  accelCount[1] = ((int16_t)rawData[2] << 8) | rawData[3] ;
  accelCount[2] = ((int16_t)rawData[4] << 8) | rawData[5] ;

  //TODO: update on read success
  return true;
}

bool MPU9250::read_Gyroscope(){
  uint8_t rawData[6];  // x/y/z gyro register data stored here
  // Read the six raw data registers sequentially into data array
  readBytes(i2c_address, GYRO_XOUT_H, 6, &rawData[0]);

  // Turn the MSB and LSB into a signed 16-bit value
  gyroCount[0] = ((int16_t)rawData[0] << 8) | rawData[1] ;
  gyroCount[1] = ((int16_t)rawData[2] << 8) | rawData[3] ;
  gyroCount[2] = ((int16_t)rawData[4] << 8) | rawData[5] ;

  //TODO: update on read success
  return true;
}


void MPU9250::updateQuaternion(){
  q_m.update(ax, ay, az, gx * DEG_TO_RAD, gy * DEG_TO_RAD, gz * DEG_TO_RAD, my, mx, mz);
}

void MPU9250::retrieve_data(){
  // Read data from each sensor
  read_Accelerometer();
  read_Gyroscope();
  read_Magnetometer();

  // Retrieve the modified data
  // Accelerometer
  ax = (float)accelCount[0] * aRes;
  ay = (float)accelCount[1] * aRes;
  az = (float)accelCount[2] * aRes;

  // Gyroscope
  gx = (float)gyroCount[0] * gRes;
  gy = (float)gyroCount[1] * gRes;
  gz = (float)gyroCount[2] * gRes;

  // Magnetometer
  mx = (float)magCount[0] * mRes * factoryMagCalibration[0] - magBias[0];
  my = (float)magCount[1] * mRes * factoryMagCalibration[1] - magBias[1];
  mz = (float)magCount[2] * mRes * factoryMagCalibration[2] - magBias[2];
}

// Wire.h read and write protocols
uint8_t MPU9250::writeByte(uint8_t deviceAddress, uint8_t registerAddress,
                            uint8_t data)
{
  Wire.beginTransmission(deviceAddress);  // Initialize the Tx buffer
  Wire.write(registerAddress);      // Put slave register address in Tx buffer
  Wire.write(data);                 // Put data in Tx buffer
  Wire.endTransmission();           // Send the Tx buffer
  // TODO: Fix this to return something meaningful
  return 0;
}

// Read a byte from the given register address from device using I2C
uint8_t MPU9250::readByte(uint8_t deviceAddress, uint8_t registerAddress)
{
  uint8_t data; // `data` will store the register data

  // Initialize the Tx buffer
  Wire.beginTransmission(deviceAddress);
  // Put slave register address in Tx buffer
  Wire.write(registerAddress);
  // Send the Tx buffer, but send a restart to keep connection alive
  Wire.endTransmission(false);
  // Read one byte from slave register address
  Wire.requestFrom(deviceAddress, (uint8_t) 1);
  // Fill Rx buffer with result
  data = Wire.read();
  // Return data read from slave register
  return data;
}

// Read 1 or more bytes from given register and device using I2C
uint8_t MPU9250::readBytes(uint8_t deviceAddress, uint8_t registerAddress,
                        uint8_t count, uint8_t * dest)
{
  // Initialize the Tx buffer
  Wire.beginTransmission(deviceAddress);
  // Put slave register address in Tx buffer
  Wire.write(registerAddress);
  // Send the Tx buffer, but send a restart to keep connection alive
  Wire.endTransmission(false);

  uint8_t i = 0;
  // Read bytes from slave register address
  Wire.requestFrom(deviceAddress, count);
  while (Wire.available())
  {
    // Put read results in the Rx buffer
    dest[i++] = Wire.read();
  }

  return i; // Return number of bytes written
}