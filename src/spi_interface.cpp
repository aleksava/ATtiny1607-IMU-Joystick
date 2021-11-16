#include <avr/io.h>
#include <avr/interrupt.h>
#include "spi_interface.h"
#include <Arduino.h>
#include "SPI.h"



uint8_t writedata = 0xFF;
uint8_t buf[100];
int8_t writebuf[3];
volatile uint8_t pos;
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
    SPI0.CTRLA = SPI_ENABLE_bm;
    // /* Set up buffer mode */
    SPI0.CTRLB = SPI_BUFEN_bm | SPI_BUFWR_bm | SPI_MODE_0_gc;

    /* SPI interrupt enable for message recieved and SS active */
    SPI0.INTCTRL = SPI_DREIE_bm | SPI_RXCIF_bm | SPI_TXCIE_bm | SPI_IE_bm;//SPI_RXCIF_bm;
    pos = 0;
    spi_flag = false;
}

void spi_load_buffer(int8_t* data)
{
    for (uint8_t i = 0; i < sizeof(writebuf); i++)
    {
        //writebuf[i] = 42;//*data++;
        SPI0.DATA = 42;//writebuf[i];
    }
}
// unsigned char spi_tranceiver (unsigned char data)
// {
//     SPDR = data;                                  //Load data into buffer
//     while(!(SPSR & (1<<SPIF) ));                  //Wait until transmission complete
//     return(SPDR);                                 //Return received data
// }

void spi_reset_flag(void)
{
    spi_flag = false;
    pos = 0;
}

bool is_spi_flag(void)
{
    return spi_flag;
}

void spi_print_buf(void)
{
    for (uint8_t i = 0; i < pos; i++)
    {
        Serial.print("bufferval: "); Serial.print(buf[i]);
        Serial.println();
    }
}


ISR(SPI0_INT_vect)
{
    if(SPI0.INTFLAGS & SPI_DREIF_bm)
    {
        SPI0.DATA = 42;
    }

    if(SPI0.INTFLAGS & SPI_RXCIF_bm)
    {
        byte c = SPI0.DATA;
        Serial.print("spi interrupt: "); 
        Serial.print(c); 
        Serial.println(); 
    }

    // if(SPI0.INTFLAGS & SPI_TXCIF_bm)
    // {
    //     Serial.print("send done");
    //     pos = 0;
    // }

    // /* Write controller data to bus */
    // // for (uint8_t i = 0; i < sizeof(writebuf); i++)
    // // {
    // //     byte c = SPI0.DATA;

    // //     SPI0.DATA = writebuf[i];

    // //     while(!(SPI0.INTFLAGS & SPI_IF_bm));
    // // }
    // //SPI0.DATA = writedata;
    // //Serial.print("intr");
    // SPI0.DATA = 10;
    // //while(!(SPI0.INTFLAGS & SPI_IF_bm));

    // uint8_t c = SPI0.DATA;
    // buf[pos++] = c;
    // Serial.print("spi interrupt: "); 
    // Serial.print(c); 
    // Serial.println(); 
    // if (pos == 4){
    //     spi_flag = true;
    // }
    
    // /* clear interrupt flag */
    // SPI0.INTFLAGS |= SPI_IF_bm;
}