#include <avr/io.h>
#include <avr/interrupt.h>
#include "spi_interface.h"
#include <Arduino.h>

uint8_t writedata = 0xFF;
char buf[100];
float writebuf[3];
volatile byte pos;
volatile bool spi_flag;

void spi_init()
{
    PORTMUX.CTRLB |= PORTMUX_SPI0_bm;
    /* Configure pin direction:
    * MOSI PC2 - input
    * MISO PC1 - output
    * SCK  PC0 - input
    * SS   PC3 - input */
    PORTC.DIR &= ~PIN2_bm;
    PORTC.DIR |= PIN1_bm;
    PORTC.DIR &= ~PIN0_bm;
    PORTC.DIR &= ~PIN3_bm;

    /* Configure enable SPI, slave mode  */
    SPI0.CTRLA = SPI_ENABLE_bm & (~SPI_MASTER_bm);

    /* SPI interrupt enable for message recieved and SS active */
    SPI0.INTCTRL = SPI_IE_bm;
    pos = 0;
    spi_flag = false;
}

void spi_load_buffer(float* data)
{
    for (uint8_t i = 0; i < sizeof(writebuf); i++)
    {
        writebuf[i] = *data++;
    }
}



ISR(SPI0_INT_vect)
{
    /* Write controller data to bus */
    
    byte c = SPI0.DATA;
    // Serial.printf("\r\nspi interrupt, recieved: %c ", c);
    // for (uint8_t i = 0; i < sizeof(writebuf); i++)
    // {
    //     SPI0.DATA = writebuf[i];
    // }
    SPI0.DATA = writedata;

    /* clear interrupt flag */
    SPI0.INTFLAGS = SPI_IF_bm;
}