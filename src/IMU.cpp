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