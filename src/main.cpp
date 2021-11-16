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

#define BUTTON            PIN_PB3
#define BUTTON_RESET_TIME 3000

uint8_t is_long_press(int8_t button);
int8_t mapData(float x, float in_min, float in_max, float out_min, float out_max); 



int lastState = LOW;  // the previous state from the input pin
int currentState;     // the current reading from the input pin
unsigned long pressedTime  = 0;
unsigned long releasedTime = 0;
bool isPressing = false;
bool isLongDetected = false;


MPU mpu6050;
int8_t writeData[3];

void setup() {
  Serial.begin(9600);
  mpu6050.init();
  mpu6050.reset();
  spi_init();
  timer_init(freq64);
  pinMode(BUTTON, INPUT);
  sei();
  delay(20);
  mpu6050.calibrate();
  delay(100);
}

void loop() {

  if(is_spi_flag())
  {
    spi_print_buf();
    spi_reset_flag();
  }

  if(is_long_press(digitalRead(BUTTON)))
  {
    Serial.println("recalibrate");
    mpu6050.sleep();
    delay(100);
    mpu6050.reset();
    mpu6050.wake();
    mpu6050.calibrate();
  }

  if(is_timer_triggered())
  {
    
    mpu6050.readAcc();
    mpu6050.readGyro();
    mpu6050.calculate_roation();
    //mpu6050.print_orientation();

    /* Writedata structure 
    0. yaw  = joystickX: -100 - 100  
    1. roll = slider: 0 - 100
    2. Button pressed */

    writeData[0] = mapData(mpu6050.getYaw(), -70, 70, -100, 100);
    writeData[1] = mapData(mpu6050.getRoll(), -70, 70, 0, 100);
    writeData[2] = !digitalRead(BUTTON);
    //Serial.println(); Serial.println();
    // Serial.print("Data0: ");        Serial.print(writeData[0]);
    // Serial.print("\tData1: ");      Serial.print(writeData[1]);
    // Serial.print("\tData2: ");      Serial.println(writeData[2]);
    spi_load_buffer(writeData);
    timer_handled();
  }
  //delay(1);
}


uint8_t is_long_press(int8_t button)
{
    /* Check for long-press to recalibrate mpu */
    currentState = button;

    if(lastState == HIGH && currentState == LOW) 
    { 
      pressedTime = millis();
      isPressing = true;
      isLongDetected = false;
    } 
    else if(lastState == LOW && currentState == HIGH) 
    {
      isPressing = false;
    }

    if(isPressing == true && isLongDetected == false) 
    {
      long pressDuration = millis() - pressedTime;

      if( pressDuration > BUTTON_RESET_TIME ) 
      {
        isLongDetected = true;
        return 1;
      }
    }

    lastState = currentState;
    return 0;
}


int8_t mapData(float x, float in_min, float in_max, float out_min, float out_max) 
{
  if (x > in_max) x = in_max;
  if (x < in_min) x = in_min;
  
  float data = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;

  return (int8_t)(data);
}