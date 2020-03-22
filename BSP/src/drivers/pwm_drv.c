/**
 * @file     pwm_drv.c
 * @brief    PWM driver
 * @version  1.0
 * @date     15 Jun. 2017
 *
 **/

#include "drivers/gpio_drv.h"
#include "drivers/pwm_drv.h"
#include "drivers/common.h"

static int _dutyCycle;

void PWM_Init(int shift, int dutyCycle) {
    _dutyCycle = dutyCycle;
    
    SET_PWM_POWER_ON;
    //
    SET_PIN_FUNCTION(4, shift, PINSEL_SEL_ALT1);

    LPC_PWM1->PR = PWM_PRESCALER;
    // bit 1 : 1 . Reset on PWMMR0: the PWMTC will be reset if PWMMR0 matches it.
    LPC_PWM1->MCR = 0x2;
    LPC_PWM1->MR0 = 100;
    LPC_PWM1->MR1 =  _dutyCycle;
    // bit 0 e 1 - Writing a one to this bit allows the last value written to the PWM Match 0 and 1 register
    // to be become effective when the timer is next reset by a PWM Match event.
    LPC_PWM1->LER = 0x3;
    // PWM1 output enabled
    LPC_PWM1->PCR = (0x1 << 9);
    /*
     * bit 0: 1 - The PWM Timer Counter and PWM Prescale Counter are enabled for counting.
     * bit 1: 0 - The PWM Timer Counter and the PWM Prescale 0 Counter are synchronously reset on the next positive edge of PCLK. The counters remain reset until this bit is returned to zero.
     * bit 2: 0 - Reserved.
     * bit 3: 1 - PWM mode is enabled (counter resets to 1). IMPORTANT!: Note that the PWM Match register that determines the PWM rate (PWM Match Register 0 - MR0) must be setup prior to the PWM being enabled.
     */
    LPC_PWM1->TCR = 0x2; // reset
    LPC_PWM1->TCR = 0x9;
}

static void setMR(void) {
    LPC_PWM1->MR1 = _dutyCycle;
    LPC_PWM1->LER = PWM_MR1_LER;
}

int PWM_IncrementDutyCycle(void) {
    if(_dutyCycle < PWM_DUTYCYCLE_MAX) {
        _dutyCycle++;
    }
    setMR();

    return _dutyCycle;
}


int PWM_DecrementDutyCycle(void) {
    if(_dutyCycle > PWM_DUTYCYCLE_MIN) {
        _dutyCycle--;
    }
    setMR();
    
    return _dutyCycle;
}


void PWM_SetDutyCycle(int dutyCycle) {
    _dutyCycle = dutyCycle;
    setMR();
}

int PWM_GetDutyCycle(void) {
    return _dutyCycle;
}
