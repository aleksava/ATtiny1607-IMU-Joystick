#include <Arduino.h>
#include "mpu6050.h"
#include "spi_interface.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"

/* Todo: figure out how to send the controller data over SPI;
* should i have the data as uint8_t 0-255 and convert after, or try to send it as a float?
* How should the master operate in order for the slave to send all the desired data?
* 2 floats + 1 bool = 2*4 bytes + 1 byte = 9 bytes; */


MPU mpu6050;
float writeData[3];

void setup() {
  Serial.begin(9600);
  mpu6050.init();
  spi_init();
  timer_init(freq1);
  sei();
  delay(20);
  mpu6050.calibrate();
  delay(100);
}

void loop() {
  if(is_timer_triggered())
  {
    Serial.println(); Serial.println();
    mpu6050.readAcc();
    mpu6050.readGyro();
    mpu6050.calculate_roation();
    mpu6050.print_orientation();
    writeData[0] = mpu6050.getRoll();
    writeData[1] = mpu6050.getPitch();
    writeData[2] = mpu6050.getYaw();
    spi_load_buffer(writeData);
    timer_handled();
  }
  delay(1);
}
