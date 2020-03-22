/**
 * @file     spi_drv.c
 * @brief    SPI driver
 * @version  1.0
 * @date     15 Jun. 2017
 *
 **/

#include "drivers/spi_drv.h"
#include "drivers/common.h"

void SPI_Init(int frequency, int bitData) { // TODO: bitData to the SPI->SPCR
    SPI_POWER_ON;
    
    unsigned int mask;
    mask = (0x3 << (MISO_SHIFT)) |
           (0x3 << (MOSI_SHIFT));
    unsigned int functions;
    functions = ((PINSEL_SEL_ALT3) << (MISO_SHIFT)) |
                ((PINSEL_SEL_ALT3) << (MOSI_SHIFT));
    SET_PIN_GROUP_FUNCTION(1, mask, functions);
    SET_PIN_FUNCTION(0, SCK_SHIFT, PINSEL_SEL_ALT3);
    // same clock as the CPU
    SET_PERIPHERAL_CLOCK_MODE(0, SPI_CLOCK_MODE_SHIFT, CLOCK_MODE_SAME_AS_CCLOCK);
    // Conf clock divider - PCLOCK / frequency
    SPI_SET_CLOCK(frequency);
    // Conf mode 
    LPC_SPI->SPCR = SPCCR_MASK;
    // Reset flags
    SPI_GET_STATUS;
}

void SPI_BeginTransfer(int csBitId) {
    SET_PIN_OFF(0, csBitId);
}

void SPI_EndTransfer(int csBitId) {
    SET_PIN_ON(0, csBitId);
}

void SPI_Transfer(unsigned short *txBuffer, unsigned short *rxBuffer, int lenght) {
    int i;
    for (i = 0; i < lenght; i++) {
        LPC_SPI->SPDR = *txBuffer++;

        spsr_register *status;
        unsigned int spsr_val;
        do {
            spsr_val = SPI_GET_STATUS;
            status = (spsr_register *)&spsr_val;
        } while (!(status->SPIF));
        
        if(rxBuffer)
            *rxBuffer++ = LPC_SPI->SPDR;
    }
}
