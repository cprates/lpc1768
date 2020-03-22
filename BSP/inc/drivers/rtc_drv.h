/**
 * @file     rtc_drv.h
 * @brief    RTC headers
 * @version  1.0
 * @date     15 Jun. 2017
 *
 **/

#ifndef __RTC_DRV_H__
#define __RTC_DRV_H__

#include <time.h>

#include "LPC17xx.h"


/** @addtogroup DRIVERS
* @{
*/
 /** @defgroup RTC Real Time Clock (RTC) Drivers
 * @{
 */

#define CLKEN_MASK                      (0x1)
#define CTCRST_MASK                     (0x2)
#define CTTEST_MASK                     (0xC)
#define REFERENCE_CLOCK                 (32768)    //!< Handy to calc the decimal and frac of the Prescaler
#define PCONP_RTC_BIT_SHIFT             (0x9)

// Power
#define SET_RTC_POWER_ON                (LPC_SC->PCONP |= (0x1 << PCONP_RTC_BIT_SHIFT))
#define SET_RTC_POWER_OFF               (LPC_SC->PCONP &= ~(0x1 << PCONP_RTC_BIT_SHIFT))
#define RTC_RESET                       (LPC_RTC->CCR |= CTCRST_MASK)
#define RTC_UNRESET                     (LPC_RTC->CCR &= ~CTCRST_MASK)
#define RTC_ENABLE                      (LPC_RTC->CCR |= CLKEN_MASK)
#define RTC_DISABLE                     (LPC_RTC->CCR &= ~CLKEN_MASK)
#define RTC_DISABLE_TEST                (LPC_RTC->CCR &= ~CTTEST_MASK)
#define RTC_DISABLE_INT                 (LPC_RTC->ILR = 0x0)
#define RTC_CLEAR_INT                   (LPC_RTC->ILR = 0x3)
#define RTC_DISABLE_CIIR                (LPC_RTC->CIIR = 0x0)
#define RTC_DISABLE_ALARM               (LPC_RTC->AMR |= 0xFF)

/// Time struct
typedef struct consolidate_time_t {
    char sec  : 6;
    char      : 2;
    char min  : 6;
    char      : 2;
    char hour : 5;
} consolidate_time;

/// Date struct
typedef struct consolidate_date_t {
    char mday  : 5;
    char       : 3;
    char month : 4;
    char       : 4;
    short year : 12;
} consolidate_date;


/** Faz a iniciação do sistema para permitir o acesso ao periférico RTC. O RTC é
* iniciado com os valores do parâmetro.
* 
* \param dateTime Valores iniciais para o RTC.
*/
void RTC_Init(struct tm *dateTime);

/** 
* Retrieves current date and time
* \param dateTime Destimation struct
*/
void RTC_GetValue(struct tm *dateTime);

/**
 * @brief   Sets current date and time
 * @param dateTime Values to be configured in the RTC
 * @return  nothing
 */
void RTC_SetValue(struct tm *dateTime);

/**
 * @brief   [TODO] Sets in 'dateTime' the current configured RTC alarm
 * @param dateTime : ponteiro para struct dateTime
 * @return  nothing
 */
void RTC_GetAlarmValue(struct tm *dateTime);

/**
 * @brief   [TODO]  Configures the RTC alarm
 * @param dateTime Alarm settings
 * @return  nothing
 */
void RTC_SetAlarmValue(struct tm *dateTime);


 /**
 * @}
 */
  /**
 * @}
 */

#endif  /* __RTC_DRV_H__ */
