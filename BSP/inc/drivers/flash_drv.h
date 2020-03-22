/**
 * @file     flash_drv.h
 * @brief    Headers for flash driver
 * @version  1.0
 * @date     15 Jun. 2017
 *
 **/

#ifndef __FLASH_DRV_H__
#define __FLASH_DRV_H__

#include "LPC17xx.h"
#include "drivers/i2c_drv.h"

/** @addtogroup DRIVERS
* @{
*/

 /** @defgroup FLASH_DRV Flash drivers
 * @{
 */

/* Geral */
#define FLASH_SIZE                                      (8 * 1024)      //!< Bytes - 8KB
#define DEVICE_ADDR                                     (0b1010000)     //!< EEPROM addr
#define FLASH_DEFAULT_VALUE                             0xFF            //!< Flash value after erase position

#define FLASH_PAGE_SIZE                                 (64)            //!< 64 Bytes
#define FLASH_BASE_ADDR                                 0x0000U


/* Result Codes */
#define FLASH_OPER_SUCCESS                              0
#define FLASH_OPER_FAIL                                 1 

/* Macros */
#define ADDR_TO_PAGE(addr)                              ((addr)/(FLASH_PAGE_SIZE))
#define PAGE_TO_ADDR(page)                              ((page)*(FLASH_PAGE_SIZE))
//#define TO_BIG_ENDIAN_SHORT(addr)                     (0xFFFF & (addr >> 8 | addr << 8))
#define ushort                                          unsigned short
#define TO_BIG_ENDIAN_SHORT(addr)                       (ushort)((((ushort) (addr)) << 8) | (((ushort) (addr)) >> 8))

/**
 * @brief   Flash init
 * @return  nothing
 */
 void FLASH_Init(void);

/** Erases one or more pages. Pass the same start and end address to erase only one page

\param startPage Start page
\param endPage End page
\return unsigned int
<br>
Command's result
*/
unsigned int FLASH_ErasePages(unsigned int startPage, unsigned int endPage);

/** Writes data from 'srcAddr' of FLASH_PAGE_SIZE size into 'dstAddr'
* 
* \param dstAddr Destination address, page aligned
* \param srcAddr Source address
* 
* \return Command's result
* */
unsigned int FLASH_WritePage( void *dstAddr, void *srcAddr);

/**
 * Reads data from srcAddr' of FLASH_PAGE_SIZE size into 'dstAddr'
 *
 * \param dstAddr Destination buffer address
 * \param srcAddr Source address of th data to be copies from flash, page aligned
 * \return unsigned int
 * <br>
 * Command's result
 */
unsigned int FLASH_ReadPage(void *dstAddr, void *srcAddr);


/**
 * Reads size bytes from a page
 *
 * \param dstAddr Destination buffer address
 * \param srcAddr Source address of the data to be copied from flash
 * \param size Buffer's size
 */
unsigned int FLASH_ReadData(void *dstAddr, void *srcAddr, unsigned int size);

/** Writes 'size' bytes into a page
* 
* \param dstAddr Destination address, page bounded
* \param srcAddr Buffer address
* \param size Buffer size
*
* \return Command's result
*/
unsigned int FLASH_WriteData( void *dstAddr, void *srcAddr, unsigned int size);

/** Compares a block of data referenced by 'srcAddr' with a block referenced bu 'dstAddr' of size 'size'
* 
* \param dstAddr First block address
* \param srcAddr Second block address
* \param size Size of the blocks
*
* \return Command's result
*/
unsigned int FLASH_VerifyData(void *dstAddr, void *srcAddr, unsigned int size);


 /**
 * @}
 */
  /**
 * @}
 */

#endif  /*  __FLASH_DRV_H__  */
