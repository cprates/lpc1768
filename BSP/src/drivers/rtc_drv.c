/**
 * @file     rtc_drv.c
 * @brief    RTC driver
 * @version  1.0
 * @date     15 Jun. 2017
 *
 **/

#include "drivers/rtc_drv.h"


static void set_time(struct tm *dateTime);
static void set_date(struct tm *dateTime);


void RTC_Init(struct tm *dateTime) {
    SET_RTC_POWER_ON;
    RTC_DISABLE;
    RTC_RESET;
    RTC_DISABLE_TEST;
    
    //LPC_RTC->PREINT = (SystemCoreClock / REFERENCE_CLOCK) - 1;
    //LPC_RTC->PREFRAC = SystemCoreClock - ((LPC_RTC->PREINT + 1) * REFERENCE_CLOCK);
    //
    RTC_CLEAR_INT;          /* Clear interrupts */
    RTC_DISABLE_INT;        /* Disable interrupts */
    RTC_DISABLE_CIIR;       /* disable Counter Increment Interrupt */
    RTC_DISABLE_ALARM;      /* Alarm disabled */
    //
    RTC_SetValue(dateTime);
    
    RTC_UNRESET;
    RTC_ENABLE;
}

void RTC_SetValue(struct tm *dateTime) {
    set_time(dateTime);
    set_date(dateTime);
}

void RTC_GetValue(struct tm *dateTime) {
    unsigned int ctime0;
    unsigned int ctime1;
    do {
        ctime0 = LPC_RTC->CTIME0;
        ctime1 = LPC_RTC->CTIME1;
        consolidate_time *time = (consolidate_time *)&(ctime0);
        consolidate_date *date = (consolidate_date *)&(ctime1);
        //
        dateTime->tm_sec = time->sec;
        dateTime->tm_min = time->min;
        dateTime->tm_hour = time->hour;
        //
        dateTime->tm_mday = date->mday;
        dateTime->tm_mon = date->month;
        dateTime->tm_year = date->year;
    } while((ctime0 != LPC_RTC->CTIME0) || (ctime1 != LPC_RTC->CTIME1));
}

void RTC_GetAlarmValue(struct tm *dateTime) {
    // TODO
}

void RTC_SetAlarmValue(struct tm *dateTime) {
    // TODO
}

static void set_time(struct tm *dateTime) {
    LPC_RTC->SEC = dateTime->tm_sec;
    LPC_RTC->MIN = dateTime->tm_min;
    LPC_RTC->HOUR = dateTime->tm_hour;
}

static void set_date(struct tm *dateTime) {
    LPC_RTC->DOM = dateTime->tm_mday;
    LPC_RTC->MONTH = dateTime->tm_mon;
    LPC_RTC->YEAR = dateTime->tm_year;
    LPC_RTC->DOW = 0; /* Day Of Week - not used*/
}
