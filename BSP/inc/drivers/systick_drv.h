/**
 * @file     systick_drv.h
 * @brief    Systick headers
 * @version  1.0
 * @date     15 Jun. 2017
 *
 **/

 /** @addtogroup DRIVERS
 * @{
 */

  /** @defgroup SYSTICK Systick Driver
 * @{
 */

#ifndef DRIVERS_SYSTICK_DRV_H_
#define DRIVERS_SYSTICK_DRV_H_

#include "LPC17xx.h"

#define SYSTICK_ENABLE(state)                       SysTick->CTRL = ((SysTick->CTRL & (~SysTick_CTRL_ENABLE_Msk)) | (state) << SysTick_CTRL_ENABLE_Pos)
#define SYSTICK_TICKINT(state)                      SysTick->CTRL = ((SysTick->CTRL & (~SysTick_CTRL_TICKINT_Msk)) | (state) << SysTick_CTRL_TICKINT_Pos)
#define SYSTICK_CLKSOURCE(source)                   SysTick->CTRL = ((SysTick->CTRL & (~SysTick_CTRL_CLKSOURCE_Msk)) | (source) << SysTick_CTRL_CLKSOURCE_Pos)
#define SYSTICK_COUNTFLAG                           (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)

#define SYSTICK_SET_RELOAD_VALUE(value)             SysTick->LOAD = ((SysTick->LOAD & (~SysTick_LOAD_RELOAD_Msk)) | (value) << SysTick_LOAD_RELOAD_Pos)

void SYSTICK_Init(unsigned value);
unsigned int SYSTICK_Read();
void SYSTICK_Set(unsigned int value);

#endif /* DRIVERS_SYSTICK_DRV_H_ */

/**
 * @}
 */
 /**
 * @}
 */
