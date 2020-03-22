/**
 * @file     timer_drv.h
 * @brief    Generic timer headers
 * @version  1.0
 * @date     15 Jun. 2017
 *
 **/


#ifndef __TIMER_H__
#define __TIMER_H__

#include "LPC17xx.h"

 /** @addtogroup DRIVERS
 * @{
 */
 
  /** @defgroup TIMER Generic Timer Driver
 * @{
 */

#define PCONP_TIMER0_BIT_SHIFT          (0x1)
#define PCONP_TIMER1_BIT_SHIFT          (0x2)
#define TCR_ENABLE_BIT_SHIFT            (0x0)
#define TCR_RESET_BIT_SHIFT             (0x1)
#define CTCR_MODE_BITS_SHIFT            (0x0)
#define CTCR_MODE_MASK                  (0x3)
#define TIMER_MODE_TIMER                (0X0)       //!< Timer Mode: every rising PCLK edge
#define TIMER_MODE_COUNTER1             (0X1)       //!< Counter Mode: TC is incremented on rising edges on the CAP input selected by bits 3:2.
#define TIMER_MODE_COUNTER2             (0X2)       //!< Counter Mode: TC is incremented on falling edges on the CAP input selected by bits 3:2.
#define TIMER_MODE_COUNTER3             (0X3)       //!< Counter Mode: TC is incremented on both edges on the CAP input selected by bits 3:2.
// Clock mode
#define TIMER0_CLOCK_MODE_SHIFT         (0x2)
#define TIMER1_CLOCK_MODE_SHIFT         (0x4)


// Power
#define SET_TIMER0_POWER_ON             (LPC_SC->PCONP |= (0x1 << PCONP_TIMER0_BIT_SHIFT))
#define SET_TIMER0_POWER_OFF            (LPC_SC->PCONP &= ~(0x1 << PCONP_TIMER0_BIT_SHIFT))
#define SET_TIMER1_POWER_ON             (LPC_SC->PCONP |= (0x1 << PCONP_TIMER1_BIT_SHIFT))
#define SET_TIMER1_POWER_OFF            (LPC_SC->PCONP &= ~(0x1 << PCONP_TIMER1_BIT_SHIFT))
// Enable - Disable - Reset
#define SET_TIMER0_ENABLED              (LPC_TIM0->TCR |= (0x1 << TCR_ENABLE_BIT_SHIFT))
#define SET_TIMER0_DISABLED             (LPC_TIM0->TCR &= ~(0x1 << TCR_ENABLE_BIT_SHIFT))
#define RESET_TIMER0                    (LPC_TIM0->TCR |= (0x1 << TCR_RESET_BIT_SHIFT))
#define UNRESET_TIMER0                  (LPC_TIM0->TCR &= ~(0x1 << TCR_RESET_BIT_SHIFT))
#define SET_TIMER1_ENABLED              (LPC_TIM1->TCR |= (0x1 << TCR_ENABLE_BIT_SHIFT))
#define SET_TIMER1_DISABLED             (LPC_TIM1->TCR &= ~(0x1 << TCR_ENABLE_BIT_SHIFT))
#define RESET_TIMER1                    (LPC_TIM1->TCR |= (0x1 << TCR_RESET_BIT_SHIFT))
#define UNRESET_TIMER1                  (LPC_TIM1->TCR &= ~(0x1 << TCR_RESET_BIT_SHIFT))
// Select mode
#define SET_TIMER0_MODE(mode)           (LPC_TIM0->CTCR = (LPC_TIM0->CTCR & ~(CTCR_MODE_MASK)) | ((mode) << (CTCR_MODE_BITS_SHIFT)))
#define SET_TIMER1_MODE(mode)           (LPC_TIM1->CTCR = (LPC_TIM1->CTCR & ~(CTCR_MODE_MASK)) | ((mode) << (CTCR_MODE_BITS_SHIFT)))


/** 
 * Timer 0 init
 * 
 * \param frequency Frequency to be configured
*/
void TIMER0_Init(unsigned int frequency);

/** 
 * \return Returns the current value of Timer 0 in ticks
 */
unsigned int TIMER0_GetValue(void);

/** 
 * Calculates the different between the current Timer 0 value and the given 'lastRead' 
 *
 * \param lastRead Last read value
 * \return Returns the different between the current Timer 0 value and the given 'lastRead'
* */
unsigned int TIMER0_Elapse(unsigned int lastRead);

/**
 * @}
 */
 /**
 * @}
 */

#endif  /* __TIMER_H__ */
