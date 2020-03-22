/**
 * @file     systick_drv.c
 * @brief    Systick driver
 * @version  1.0
 * @date     15 Jun. 2017
 *
 **/

#include "drivers/systick_drv.h"

static unsigned int counter;

void SysTick_Handler(void)
{
    counter++;
}


void SYSTICK_Init(unsigned value){

    SYSTICK_SET_RELOAD_VALUE(value - 1);

    counter = 0;

    SYSTICK_CLKSOURCE(1); // CPU Selected
    SYSTICK_TICKINT(1); // Activate Interrupt
    SYSTICK_ENABLE(1); // Enable Systick
}

unsigned int SYSTICK_Read(){
    return counter;
}

void SYSTICK_Set(unsigned int value){

}
