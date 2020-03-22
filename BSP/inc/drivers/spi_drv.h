/**
 * @file     spi_drv.h
 * @brief    SPI headers
 * @version  1.0
 * @date     15 Jun. 2017
 *
 **/

#ifndef __SPI_DRV_H__
#define __SPI_DRV_H__

#include <stdint.h>

#include "gpio_drv.h"

/** @addtogroup DRIVERS
* @{
*/
 /** @defgroup SPI SPI Driver
 * @{
 */

// SPI pins defenition
#define SCK_SHIFT                           30                  //!< GPIO P0.15 - LPC2106 Pin 13 PINSEL0
#define MISO_SHIFT                          2                   //!< GPIO P0.17 - LPC2106 Pin 12 PINSEL1
#define MOSI_SHIFT                          4                   //!< GPIO P0.18 - LPC2106 Pin 11 PINSEL1
#define SSEL_SHIFT                          0                   //!< GPIO P0.16 - LPC2106 Pin 14 PINSEL1
// Clock
#define SPI_CLOCK_MODE_SHIFT                16


#define PCONP_SPI_BIT_SHIFT                 (0x8)
//BIT_TRANSFER_RATE_MASK    (0x0004)
#define SPCCR_MASK                          (0x93C)

#define SPI_POWER_ON                        (LPC_SC->PCONP |= (0x1 << PCONP_SPI_BIT_SHIFT))
#define SPI_POWER_OFF                       (LPC_SC->PCONP &= ~(0x1 << PCONP_SPI_BIT_SHIFT))
#define SPI_SET_CLOCK(freq)                 (LPC_SPI->SPCCR = (freq))
#define SPI_GET_STATUS                      (LPC_SPI->SPSR)
//#define SPI_SET_BIT_TRANSFER_RATE(rate)       (SPI->SPCR = (SPI->SPCR & ~(CTCR_MODE_MASK)) | ((mode) << (CTCR_MODE_BITS_SHIFT)))

/// handy struct to access SPSR
typedef struct spsr_register_t {
    uint32_t reserved_1 : 3;
    uint32_t ABRT       : 1;
    uint32_t MODF       : 1;
    uint32_t ROVR       : 1;
    uint32_t WCOL       : 1;
    uint32_t SPIF       : 1;
} spsr_register;


/** SPI init
* \param frequency Transmission rate
* \param bitData Number of bits per transmission
*/
void SPI_Init(int frequency, int bitData);

/** Set the chip select of the slave device
 * 
 * \param csBitId PIN id of the CS in the GPIO
 */
void SPI_BeginTransfer(int csBitId);

/** Unset the chip select of the slave device
 * 
 * \param csBitId PIN id of the CS in the GPIO
 */
void SPI_EndTransfer(int csBitId);

/** Transmits data in 'tsBuffer'
 If rxBuffer == 0, discards reads
 * 
 * \param txBuffer Buffer's address to store the received data
 * \param rxBuffer Buffer's address containing the data to be sent
 * \param lenght Size in bytes of the data to be sent and received
 */
void SPI_Transfer(unsigned short *txBuffer, unsigned short *rxBuffer, int lenght);


/**
 * @}
 */
 /**
 * @}
 */

#endif /* __SPI_DRV_H__ */
