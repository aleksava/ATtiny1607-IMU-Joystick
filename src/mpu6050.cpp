/* This code is inspired by: https://howtomechatronics.com/tutorials/arduino/arduino-and-mpu6050-accelerometer-and-gyroscope-tutorial/ */


#include <Arduino.h>
#include <Wire.h>
#include "mpu6050.h"

#define MPU_address 0x68

#define ACC_SCALE2g         16384.0
#define ACC_SCALE4g         8192.0
#define ACC_SCALE8g         4096.0
#define ACC_SCALE16g        2048.0

#define GYRO_SCALE250       131.0
#define GYRO_SCALE500       65.5
#define GYRO_SCALE1000      32.8
#define GYRO_SCALE2000      16.4

#define DEBUG               0

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

    /* Set the Gyro sensitivity to 250deg/s  */
    Wire.beginTransmission(MPU_address);
    Wire.write(0x1B);
    Wire.write(0x00);
    Wire.endTransmission(true);
}

void MPU::sleep(void)
{
    Wire.beginTransmission(MPU_address);
    Wire.write(0x6B);
    Wire.write(0x01);
    Wire.endTransmission(true);
}

void MPU::wake(void)
{
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
    accX = (Wire.read() << 8 | Wire.read()) / ACC_SCALE2g ;
    accY = (Wire.read() << 8 | Wire.read()) / ACC_SCALE2g ;
    accZ = (Wire.read() << 8 | Wire.read()) / ACC_SCALE2g ;

    /* Correct for accErrors */
    if(calibration_done)
    {
        accX += accXerr;
        accY += accYerr;
        accZ += accZerr;

        accangleX = ((atan((accY) / sqrt(pow((accX), 2) + pow((accZ), 2))) * 180 / PI)) + accXerr;
        accangleY = ((atan(-1 * (accX) / sqrt(pow((accY), 2) + pow((accZ), 2))) * 180 / PI)) + accYerr;
    }

    if(DEBUG && calibration_done)
    {
        Serial.print("AccX: "); Serial.print(accX,2);
        Serial.print("\tAccY: "); Serial.print(accY,2);
        Serial.print("\tAccZ: "); Serial.println(accZ,2);
        Serial.print("AccangleX: "); Serial.print(accangleX,2);
        Serial.print("\tAccangleY: "); Serial.println(accangleY,2);
    }
}

void MPU::readGyro(void){
    previous_time_ms = current_time_ms;
    current_time_ms = millis();
    elapsedTime = (current_time_ms - previous_time_ms) / 1000.0;
    Wire.beginTransmission(MPU_address);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_address, 6, true);
    gyroX = (Wire.read() << 8 | Wire.read()) / GYRO_SCALE250;
    gyroY = (Wire.read() << 8 | Wire.read()) / GYRO_SCALE250;
    gyroZ = (Wire.read() << 8 | Wire.read()) / GYRO_SCALE250;

    /* Correct for gyroErrors */
    if(calibration_done)
    {
        gyroX -= gyroXerr;
        gyroY -= gyroYerr;
        gyroZ -= gyroZerr;

        gyroangleX += gyroX * elapsedTime;
        gyroangleY += gyroY * elapsedTime;
    }

    if(DEBUG && calibration_done)
    {
        Serial.print("gyroX: "); Serial.print(gyroX,2);
        Serial.print("\tgyroY: "); Serial.print(gyroY,2);
        Serial.print("\tgyroZ: "); Serial.println(gyroZ,2);
        Serial.print("gyroAngleX: "); Serial.print(gyroangleX,2);
        Serial.print("\tgyroAnlgeY: "); Serial.println(gyroangleY,2);
    }
}

void MPU::calculate_roation(void)
{
    yaw     += gyroZ * elapsedTime;
    roll    = 0.96* gyroangleX + 0.04 * accangleX;
    pitch   = 0.96* gyroangleY + 0.04 * accangleY;

    if(DEBUG && calibration_done)
    {
        Serial.print("Pitch: ");    Serial.print(pitch,2);
        Serial.print("\tRoll: ");     Serial.print(roll,2);
        Serial.print("\tYaw: ");      Serial.println(yaw,2);
    }

}

float MPU::getPitch(void)
{
    return pitch;
}

float MPU::getRoll(void)
{
    return roll;
}

float MPU::getYaw(void)
{
    return yaw;
}

void MPU::calibrate(void)
{
    calibration_done = 0;
    accXerr, accYerr, gyroXerr, gyroYerr, gyroZerr =  0;
    uint8_t calibration_reads = 200;
    for(uint8_t i = 0; i < calibration_reads; i++){
        readAcc();
        accXerr += ((atan((accY) / sqrt(pow((accX), 2) + pow((accZ), 2))) * 180 / PI));
        accYerr += ((atan(-1 * (accX) / sqrt(pow((accY), 2) + pow((accZ), 2))) * 180 / PI));
    }
    accXerr = accXerr / calibration_reads;
    accYerr = accYerr / calibration_reads;

    for(uint8_t i = 0; i < calibration_reads; i++){
        readGyro();
        gyroXerr += gyroX;
        gyroYerr += gyroY;
        gyroZerr += gyroZ;
    }

    /* Normalize the error */
    gyroXerr = gyroXerr / calibration_reads;
    gyroYerr = gyroYerr / calibration_reads;
    gyroZerr = gyroZerr / calibration_reads;

    if(DEBUG)
    {
        Serial.print("AccErrorX: ");
        Serial.print(accXerr);
        Serial.print("\tAccErrorY: ");
        Serial.print(accYerr);
        Serial.print("\tGyroErrorX: ");
        Serial.print(gyroXerr);
        Serial.print("\tGyroErrorY: ");
        Serial.print(gyroYerr);
        Serial.print("\tGyroErrorZ: ");
        Serial.println(gyroZerr);
    }
    calibration_done = 1;
}


void MPU::print_orientation(void)
{
    Serial.print("Pitch: ");    Serial.print(pitch,2);
    Serial.print("\tRoll: ");       Serial.print(roll,2);
    Serial.print("\tYaw: ");      Serial.println(yaw,2);
}