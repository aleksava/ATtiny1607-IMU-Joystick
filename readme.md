Funksjonsbeskrivelse Joystick
Systemet skal hente data fra MPU vha i2c. Denne skal konverteres til samme dataformat som joysticken på spillbrettet har, og deretter sendes ned til Node 1 via SPI. 
Tanken er at systemet skal ligge i dvale inntil den får en pinchange interrupt (blir satt når node 1 ønsker å bruke ekstra funk.)

Virkemåte til joysticken er som følger. En tilting til en av sidene styrer den store lineærmotoren frem og tilbake. 
Vridning skal styre servomotor.
Knapp på toppen av enheten skal aktiver solinoiden.


Oppkobling:
SDA - PB0
SCK - PB1 (sjekk disse)


Moduler:
MPU:
- Init
- Kalibrering
- Lese data
- konvertere til styredata
- Lage en integrasjons-funksjon for gyroskopet, slik at vi kan styre lineærmotor fra side til side. 
SPI:
- Sende joystick data
Timer:
- init(frekvens)
Interrupt:
- vekke enheten når interrupt på pin
- kjøre datainnhenting fra MPU (timer)



Inspo:
https://create.arduino.cc/projecthub/MissionCritical/mpu-6050-tutorial-how-to-program-mpu-6050-with-arduino-aee39a
https://electronoobs.com/eng_arduino_tut76.php
https://howtomechatronics.com/tutorials/arduino/arduino-and-mpu6050-accelerometer-and-gyroscope-tutorial/
