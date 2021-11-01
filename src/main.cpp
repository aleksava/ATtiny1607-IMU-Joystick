#include <Arduino.h>
#include "IMU.h"

/* TODO: 
      -Implement Calibration function IMU.cpp 
      -Make an gyro integration function for sideways rotation
      */


MPU imu;
char tmp_str[7];
char* convert_int16_to_str(int16_t i){
  sprintf(tmp_str, "%6d",i);
  return tmp_str;
}

void setup() {
  Serial.begin(9600);
  imu.init();
  delay(20);
}

void loop() {
  imu.readAcc();
  imu.readGyro();
  Serial.printf("\r\nAcc X,Y,Z: %i\t%i\t%i", imu.getAccX(), imu.getAccY(), imu.getAccZ());
  delay(1100);

  // // Calculating Roll and Pitch from the accelerometer data
  // accAngleX = (atan(AccY / sqrt(pow(AccX, 2) + pow(AccZ, 2))) * 180 / PI) - 0.58; // AccErrorX ~(0.58) See the calculate_IMU_error()custom function for more details
  // accAngleY = (atan(-1 * AccX / sqrt(pow(AccY, 2) + pow(AccZ, 2))) * 180 / PI) + 1.58; // AccErrorY ~(-1.58)
  
  // // Currently the raw values are in degrees per seconds, deg/s, so we need to multiply by sendonds (s) to get the angle in degrees
  // gyroAngleX = gyroAngleX + GyroX * elapsedTime; // deg/s * s = deg
  // gyroAngleY = gyroAngleY + GyroY * elapsedTime;
  // yaw =  yaw + GyroZ * elapsedTime;
  // // Complementary filter - combine acceleromter and gyro angle values
  // roll = 0.96 * gyroAngleX + 0.04 * accAngleX;
  // pitch = 0.96 * gyroAngleY + 0.04 * accAngleY;
  
  // Print the values on the serial monitor
  // Serial.print(roll);
  // Serial.print("/");
  // Serial.print(pitch);
  // Serial.print("/");
  // Serial.println(yaw);
}
