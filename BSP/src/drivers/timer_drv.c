/**
 * @file     timer_drv.c
 * @brief    Generic Timer driver
 * @version  1.0
 * @date     15 Jun. 2017
 *
 **/

#include <limits.h>

#include "drivers/timer_drv.h"
#include "drivers/common.h"


unsigned int calc_prescaler_value(unsigned int frequency) {
    unsigned int retVal = SystemCoreClock / (frequency * 1.0);
    return retVal;
}

void TIMER0_Init(unsigned int frequency) {
    SET_TIMER0_POWER_ON;
    RESET_TIMER0;
    SET_TIMER0_DISABLED;
    // same clck as the CPU
    SET_PERIPHERAL_CLOCK_MODE(0, TIMER0_CLOCK_MODE_SHIFT, CLOCK_MODE_SAME_AS_CCLOCK);
    //
    SET_TIMER0_MODE(TIMER_MODE_TIMER);
    LPC_TIM0->PR = calc_prescaler_value(frequency) - 1;
    //
    UNRESET_TIMER0;
    SET_TIMER0_ENABLED;
}

unsigned int TIMER0_GetValue(void) {
    return LPC_TIM0->TC;
}

unsigned int TIMER0_Elapse(unsigned int lastRead) {
    unsigned int currVal = TIMER0_GetValue();
    unsigned int elapsed;
    if(currVal < lastRead) {
        currVal = currVal + (UINT_MAX - lastRead);
    }
    else {
        elapsed = currVal - lastRead;
    }
    
    return elapsed + 1;
}
