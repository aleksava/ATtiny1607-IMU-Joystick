#include <avr/io.h>
#include <avr/interrupt.h>
#include "spi_interface.h"
#include <Arduino.h>
#include "SPI.h"


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
    PORTC.PIN3CTRL |= PIN3_bm;

    /* Configure enable SPI, slave mode  */
    SPI0.CTRLA = SPI_ENABLE_bm;
}

