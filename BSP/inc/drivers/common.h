/**
 * @file     common.h
 * @version  1.0
 * @date     27 Mar. 2017
 *
 **/

#ifndef DRIVERS_COMMON_H_
#define DRIVERS_COMMON_H_

 /** @defgroup COMMON_DRV Common macros for controllers
 * @{
 */
#define CLOCK_MODE_ONE_FOURTH_OF_CCLOCK                     (0x0)
#define CLOCK_MODE_SAME_AS_CCLOCK                           (0x1)
#define CLOCK_MODE_ONE_HALF_OF_CCLOCK                       (0x2)

#define SET_PERIPHERAL_CLOCK_MODE(pclocksel, shift, mode)   (LPC_SC->PCLKSEL##pclocksel = (LPC_SC->PCLKSEL##pclocksel & (~0x3)) | ((mode) << (shift)))

 /**
 * @}
 */


#endif /* DRIVERS_COMMON_H_ */
