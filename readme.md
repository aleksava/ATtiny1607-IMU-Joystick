Funksjonsbeskrivelse Joystick
Systemet skal hente data fra MPU vha i2c. Denne skal konverteres til samme dataformat som joysticken på spillbrettet har, og deretter sendes ned til Node 1 via SPI. 
Tanken er at systemet skal ligge i dvale inntil den får en pinchange interrupt (blir satt når node 1 ønsker å bruke ekstra funk.)

Virkemåte til joysticken er som følger. En tilting til en av sidene styrer den store lineærmotoren frem og tilbake. 
Vridning skal styre servomotor.
Knapp på toppen av enheten skal aktiver solinoiden.


Oppkobling:
* I2C *
SDA - PB1
SCK - PB0

* SPI *
MOSI - PC2
MISO - PC1
SCK  - PC0
SS   - PC3


Moduler:
MPU:
- Init
- Kalibrering
- Lese data
- konvertere til styredata
- output pitch, roll, yaw
SPI:
- Sende joystick data
Timer:
- init(frekvens)
Interrupt:
- vekke enheten når interrupt på pin
- kjøre datainnhenting fra MPU (timer)



Inspo:
* MPU6050 *
https://create.arduino.cc/projecthub/MissionCritical/mpu-6050-tutorial-how-to-program-mpu-6050-with-arduino-aee39a
https://electronoobs.com/eng_arduino_tut76.php
https://howtomechatronics.com/tutorials/arduino/arduino-and-mpu6050-accelerometer-and-gyroscope-tutorial/

* SPI *
https://forum.arduino.cc/t/arduino-as-spi-slave/52206
https://github.com/microchip-pic-avr-examples/atmega4809-getting-started-with-spi-studio

* RTC *
https://www.avrfreaks.net/forum/attiny1617-periodic-interrupt-timer-trouble

* DataSheets *
http://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny807_1607-Data-Sheet-40002030A.pdf
http://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny1607-Curiosity-Nano-HW-UserGuide-DS50002897A.pdf


