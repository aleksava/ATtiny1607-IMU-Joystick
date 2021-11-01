#include <Arduino.h>
#include <Wire.h>
#include "IMU.h"

#define MPU_address 0x68

void MPU::init(void)
{
    /* Initialize I2C */
    Wire.begin();
    delay(20);
    /* Wake MPU */ 
    Wire.beginTransmission(MPU_address);
    Wire.write(0x6B);
    Wire.write(0x00);
    Wire.endTransmission(true);

    /* Set the ACC sensitivity to +/- 2g */
    Wire.beginTransmission(MPU_address);
    Wire.write(0x1C);
    Wire.write(0x00);
    Wire.endTransmission(true);

    /* Set the Gyro sensitivity to 1000deg/s  */
    Wire.beginTransmission(MPU_address);
    Wire.write(0x1B);
    Wire.write(0x10);
    Wire.endTransmission(true);
}

void MPU::readAcc(void){
    Wire.beginTransmission(MPU_address);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_address, 6 , true);
    accX = (Wire.read() << 8 | Wire.read()); /// 16384.0 ;
    accY = (Wire.read() << 8 | Wire.read());// / 16384.0 ;
    accZ = (Wire.read() << 8 | Wire.read());// / 16384.0 ;
}

void MPU::readGyro(void){
    Wire.beginTransmission(MPU_address);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_address, 6, true);
    gyroX = Wire.read() << 8 | Wire.read();
    gyroY = Wire.read() << 8 | Wire.read();
    gyroZ = Wire.read() << 8 | Wire.read();
}

int MPU::getAccX(void){
    return accX;
}
int MPU::getAccY(void){
    return accY;
}
int MPU::getAccZ(void){
    return accZ;
}


void MPU::calibrate(void)
{
    uint8_t calibration_reads = 100;
    for(uint8_t i = 0; i < calibration_reads; i++){
        readAcc();
        accXerr += ((atan((accY) / sqrt(pow((accX), 2) + pow((accZ), 2))) * 180 / PI));
        accYerr = accYerr + ((atan(-1 * (accX) / sqrt(pow((accY), 2) + pow((accZ), 2))) * 180 / PI));
    }
    accXerr = accXerr / calibration_reads;
    accYerr = accYerr / calibration_reads;

    for(uint8_t i = 0; i < calibration_reads; i++){
        readGyro();
        gyroXerr += gyroX / 131.0;

/********************************/
/* Continue working here */
            GyroErrorX = GyroErrorX + (GyroX / 131.0);
    GyroErrorY = GyroErrorY + (GyroY / 131.0);
    GyroErrorZ = GyroErrorZ + (GyroZ / 131.0);
        accXerr += ((atan((accY) / sqrt(pow((accX), 2) + pow((accZ), 2))) * 180 / PI));
        accYerr = accYerr + ((atan(-1 * (accX) / sqrt(pow((accY), 2) + pow((accZ), 2))) * 180 / PI));
    }
}


  // Read gyro values 200 times
  while (c < 200) {
    Wire.beginTransmission(MPU);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    GyroX = Wire.read() << 8 | Wire.read();
    GyroY = Wire.read() << 8 | Wire.read();
    GyroZ = Wire.read() << 8 | Wire.read();
    // Sum all readings
    GyroErrorX = GyroErrorX + (GyroX / 131.0);
    GyroErrorY = GyroErrorY + (GyroY / 131.0);
    GyroErrorZ = GyroErrorZ + (GyroZ / 131.0);
    c++;
  }
  //Divide the sum by 200 to get the error value
  GyroErrorX = GyroErrorX / 200;
  GyroErrorY = GyroErrorY / 200;
  GyroErrorZ = GyroErrorZ / 200;
  // Print the error values on the Serial Monitor
  Serial.print("AccErrorX: ");
  Serial.println(AccErrorX);
  Serial.print("AccErrorY: ");
  Serial.println(AccErrorY);
  Serial.print("GyroErrorX: ");
  Serial.println(GyroErrorX);
  Serial.print("GyroErrorY: ");
  Serial.println(GyroErrorY);
  Serial.print("GyroErrorZ: ");
  Serial.println(GyroErrorZ);
}