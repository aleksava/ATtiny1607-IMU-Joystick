#include <Arduino.h>
#include "mpu6050.h"
#include "spi_interface.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"


#define DEBUG             
#define BUTTON            PIN_PB3
#define BUTTON_RESET_TIME 2000

uint8_t is_long_press(int8_t button);
int8_t mapData(float x, float in_min, float in_max, float out_min, float out_max); 


int lastState = LOW;  // the previous state from the input pin
int currentState;     // the current reading from the input pin
unsigned long pressedTime  = 0;
unsigned long releasedTime = 0;
bool isPressing = false;
bool isLongDetected = false;
uint8_t spiID = 0;
MPU mpu6050;
int8_t writeData[3] = {0, 0, 0};

void setup() {
  
  Serial.begin(115200);
  mpu6050.init();
  delay(20);
  mpu6050.reset();
  spi_init();
  timer_init(freq64);
  pinMode(BUTTON, INPUT);
  sei();
  delay(20);
  mpu6050.calibrate();
  delay(100);
  Serial.print("*");
}

void loop() {
  if(is_long_press(digitalRead(BUTTON)))
  {
    Serial.println(".");
    mpu6050.sleep();
    delay(100);
    mpu6050.reset();
    mpu6050.wake();
    mpu6050.calibrate();
  }

  if(!digitalRead(PIN_PC3) && !spiID)
  {
    while(!(SPI0.INTFLAGS & SPI_IF_bm));
    spiID = SPI0.DATA;
  }

  if(!digitalRead(PIN_PC3) && spiID)
  {
    if(spiID == 1)
    {
      SPI0.DATA = writeData[0];
    }
    if(spiID == 2)
    {
      SPI0.DATA = writeData[1];
    }
    if(spiID == 3)
    {
      SPI0.DATA = writeData[2];
    }
    spiID = 0;
  }
  if(is_timer_triggered())
  {
    mpu6050.readAcc();
    mpu6050.readGyro();
    mpu6050.calculate_roation();
    //mpu6050.print_orientation();

    /* Writedata structure 
    0. Roll = joystickX: -100 - 100  
    1. Pitch = slider: 0 - 100
    2. Button pressed */
    writeData[0] = mapData(mpu6050.getRoll(), -50, 50, -100, 100);
    writeData[1] = mapData(mpu6050.getPitch(), -50, 50, 0, 100);
    writeData[2] = 1 - digitalRead(BUTTON);
    #ifdef DEBUG
    Serial.println(); Serial.println();
    Serial.print("Data0: ");        Serial.print(writeData[0]);
    Serial.print("\tData1: ");      Serial.print(writeData[1]);
    Serial.print("\tData2: ");      Serial.println(writeData[2]);
    #endif
    timer_handled();
  }
  delay(1);
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