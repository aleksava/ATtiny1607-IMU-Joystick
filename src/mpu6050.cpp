/* This code is inspired by: https://howtomechatronics.com/tutorials/arduino/arduino-and-mpu6050-accelerometer-and-gyroscope-tutorial/ */


#include <Arduino.h>
#include <Wire.h>
#include "mpu6050.h"

#define MPU_address 0x68

#define ACC_SCALE2g         8192.0
#define ACC_SCALE4g         4096.0
#define ACC_SCALE8g         2048.0
#define ACC_SCALE16g        1024.0


#define GYRO_SCALE250       131.0
#define GYRO_SCALE500       65.5
#define GYRO_SCALE1000      32.8
#define GYRO_SCALE2000      16.4

//#define DEBUG               

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

void MPU::reset(void)
{
    accX = 0;
    accY = 0;
    accZ = 0;
    accXerr = 0;
    accYerr = 0;
    accZerr = 0;
    accangleX = 0;
    accangleY = 0;
    gyroX = 0;
    gyroY = 0;
    gyroXerr = 0;
    gyroYerr = 0;
    gyroangleX = 0;
    gyroangleY = 0;
    pitch = 0;
    roll = 0;
    previous_time_ms = 0;
    current_time_ms = 0;
    elapsedTime = 0;
    calibration_done = 0;

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
        accangleX = ((atan((-accZ) / sqrt(pow((-accY), 2) + pow((accX), 2))) * 180 / PI)) - accXerr;
        accangleY = ((atan((accY) / sqrt(pow((-accZ), 2) + pow((accX), 2))) * 180 / PI)) - accYerr;
    }
    #ifdef DEBUG
        if(calibration_done)
        {
            Serial.print("AccX: "); Serial.print(accX,2);
            Serial.print("\tAccY: "); Serial.print(accY,2);
            Serial.print("\tAccZ: "); Serial.println(accZ,2);
            Serial.print("AccangleX: "); Serial.print(accangleX,2);
            Serial.print("\tAccangleY: "); Serial.println(accangleY,2);
        }
    #endif
}

void MPU::readGyro(void){
    previous_time_ms = current_time_ms;
    current_time_ms = millis();
    elapsedTime = (current_time_ms - previous_time_ms) / 1000.0;
    Wire.beginTransmission(MPU_address);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_address, 4, true);
    gyroX = (Wire.read() << 8 | Wire.read()) / GYRO_SCALE250;
    gyroY = (Wire.read() << 8 | Wire.read()) / GYRO_SCALE250;

    /* Correct for gyroErrors */
    if(calibration_done)
    {
        gyroX -= gyroXerr;
        gyroY -= gyroYerr;

        gyroangleX += gyroX * elapsedTime;
        gyroangleY += gyroY * elapsedTime;
    }
    #ifdef DEBUG
        if(calibration_done)
        {
            Serial.print("gyroX: "); Serial.print(gyroX,2);
            Serial.print("\tgyroY: "); Serial.print(gyroY,2);
            Serial.print("gyroAngleX: "); Serial.print(gyroangleX,2);
            Serial.print("\tgyroAnlgeY: "); Serial.println(gyroangleY,2);
        }
    #endif
}

void MPU::calculate_roation(void)
{
    roll = 0.96 * gyroangleX + 0.04 * accangleX;
    pitch = 0.96 * gyroangleY + 0.04 * accangleY;
    
    #ifdef DEBUG
        if(calibration_done)
        {
            Serial.print("Pitch: ");    Serial.print(pitch,2);
            Serial.print("\tRoll: ");     Serial.print(roll,2);
            Serial.print("\tYaw: ");      Serial.println(yaw,2);
        }
    #endif
}

float MPU::getPitch(void)
{
    return pitch;
}

float MPU::getRoll(void)
{
    return roll;
}

void MPU::calibrate(void)
{
    calibration_done = 0;
    accXerr, accYerr, gyroXerr, gyroYerr =  0;
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
    }

    /* Normalize the error */
    gyroXerr = gyroXerr / calibration_reads;
    gyroYerr = gyroYerr / calibration_reads;

    #ifdef DEBUG
        Serial.print("AccErrorX: ");
        Serial.print(accXerr);
        Serial.print("\tAccErrorY: ");
        Serial.print(accYerr);
        Serial.print("\tGyroErrorX: ");
        Serial.print(gyroXerr);
        Serial.print("\tGyroErrorY: ");
        Serial.print(gyroYerr);
    #endif
    calibration_done = 1;
}


void MPU::print_orientation(void)
{
    Serial.print("Pitch: ");    Serial.print(pitch,2);
    Serial.print("\tRoll: ");   Serial.print(roll,2);
}