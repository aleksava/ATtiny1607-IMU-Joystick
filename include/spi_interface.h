void spi_init();
void spi_load_buffer(int8_t* data);

void spi_reset_flag(void);
bool is_spi_flag(void);
void spi_print_buf(void);

ISR(SPI0_INT_vect);