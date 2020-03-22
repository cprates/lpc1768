/**
 * @file     gpio_drv.h
 * @brief    GPIO headers
 * @version  1.0
 * @date     15 Jun. 2017
 *
 **/

#ifndef __GPIO_H__
#define __GPIO_H__

#include "LPC17xx.h"


/** @addtogroup DRIVERS
* @{
*/

 /** @defgroup GPIO GPIO drivers
 * @{
 */

#define SET_PIN_DIR_OUTPUT(gpio_idx, pinId)         (LPC_GPIO##gpio_idx->FIODIR |= (0x1 << (pinId)))    //!< Word access
#define SET_PIN_DIR_INPUT(gpio_idx, pinId)          (LPC_GPIO##gpio_idx->FIODIR &= ~(0x1 << (pinId)))   //!< Word access
//
#define SET_PIN_GROUP_DIR_INPUT(gpio_idx, mask)     (LPC_GPIO##gpio_idx->FIODIR &= ~(mask))             //!< Word access
#define SET_PIN_ON(gpio_idx, pinId)                 (LPC_GPIO##gpio_idx->FIOSET |= (0x1 << (pinId)))    //!< Word access
#define SET_PIN_OFF(gpio_idx, pinId)                (LPC_GPIO##gpio_idx->FIOCLR |= (0x1 << (pinId)))    //!< Word access
#define GET_PIN_STATE(gpio_idx, pinId)              (LPC_GPIO##gpio_idx->FIOPIN & ((0x1) << (pinId)))   //!< Word access
#define GET_PIN_GROUP_STATE(gpio_idx, mask)         (LPC_GPIO##gpio_idx->FIOPIN & (mask))               //!< Word access

 /**
 * @}
 */


 /** @defgroup PIN_CONNECT_BLOCK Pin connect Block
 * @{
 */

//
#define PINSEL_SEL_DEFAULT                          (0x00)          //!< Default Pin Function
#define PINSEL_SEL_ALT1                             (0x01)          //!< Alternate Pin Function #1
#define PINSEL_SEL_ALT2                             (0x02)          //!< Alternate Pin Function #2
#define PINSEL_SEL_ALT3                             (0x03)          //!< Alternate Pin Function #3


#define SET_PIN_FUNCTION(pinsel, shiftId, func)     (LPC_PINCON->PINSEL##pinsel = (LPC_PINCON->PINSEL##pinsel & (~(0x3 << (shiftId)))) | (func << (shiftId)))
#define SET_PIN_GROUP_FUNCTION(pinsel, mask, func)  (LPC_PINCON->PINSEL##pinsel = (LPC_PINCON->PINSEL##pinsel & ~(mask)) | (func))

//
#define PULLUP_MODE                                 (0x0)
#define REPEATER_MODE                               (0x1)
#define NORMAL_MODE                                 (0x2)           /*!< No pull-up nor pull-down */
#define PULLDOWN_MODE                               (0x3)

#define SET_PINMDODE(idx, shift, mode)              (LPC_PINCON->PINMODE##idx = (LPC_PINCON->PINMODE##idx & (~0x3 << shift)) | ((mode) << (shift)))         //!< controls pull-up/pull-down resistor configuration for Port 0 pins 0 to 15
#define SET_GROUP_PINMDODE(idx, mask, mode)         (LPC_PINCON->PINMODE##idx = (LPC_PINCON->PINMODE##idx & (~mask)) | (mode))                              //!< For pin groups. Controls pull-up/pull-down resistor configuration for Port 0 pins 0 to 15
 /**
 * @}
 */

#endif  /*  __GPIO_H__  */
