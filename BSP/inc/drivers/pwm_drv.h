/**
 * @file     pwm_drv.h
 * @brief    PWM headers
 * @version  1.0
 * @date     15 Jun. 2017
 *
 **/


#ifndef __PWM_DRV_H__
#define __PWM_DRV_H__

#include "LPC17xx.h"


 /** @addtogroup DRIVERS
* @{
*/
 
/** @defgroup PWM Pulse Width Modulation (PWM) Drivers
 * @{
 */

#define PCONP_PWM_BIT_SHIFT             (0x6)

#define SET_PWM_POWER_ON                (LPC_SC->PCONP |= (0x1 << PCONP_PWM_BIT_SHIFT))
#define SET_PWM_POWER_OFF               (LPC_SC->PCONP &= ~(0x1 << PCONP_PWM_BIT_SHIFT))
#define PWM_PRESCALER                   (0)                                                     //!< PWM prescaler. Devides PCLOCK by this value
#define PWM_DUTYCYCLE_MIN               (0)
#define PWM_DUTYCYCLE_MAX               (100)

#define PWM_MR1_LER                     (0x1 << 1)


/** PWM init
 \param shift Needed shift to select the right PIN in PINSEL register
 \param dutyCycle PWM Duty Cycle from PWM_DUTYCYCLE_MIN to PWM_DUTYCYCLE_MAX
 */
void PWM_Init(int shift, int dutyCycle);


/**
 * Increments PWM Duty Cycle by 1 up to PWM_DUTYCYCLE_MAX
 * \return Returns the resulting Duty Cycle
 */
int PWM_IncrementDutyCycle(void);

/**
 * Decrements PWM Duty Cycle by 1 down to PWM_DUTYCYCLE_MIN
 * \return Returns the resulting Duty Cycle
 */
int PWM_DecrementDutyCycle(void);

/**
 * Sets the PWM Duty Cycle to the given value
 * 
 * \param dutyCycle From PWM_DUTYCYCLE_MIN to PWM_DUTYCYCLE_MAX
 */
void PWM_SetDutyCycle(int dutyCycle);


/**
 * Returns the current PWM Duty Cycle
 * 
 * \return The current PWM Duty Cycle
 */
int PWM_GetDutyCycle(void);

/**
 * @}
 */
 
#endif  /* __PWM_DRV_H__ */
