/**
 * @file     i2c_drv.h
 * @brief    I2C headers
 * @version  1.0
 * @date     15 Jun. 2017
 *
 **/

/** @addtogroup DRIVERS
* @{
*/
 /** @defgroup I2C I2C Drivers
 * @{
 */

#include "drivers/gpio_drv.h"
#include "common.h"



#ifndef DRIVERS_I2C_DRV_H_
#define DRIVERS_I2C_DRV_H_

#define PCONP_I2C0_BIT_SHIFT                (0x7)
#define I2C0_PINSEL_MASK                    (0xF << 22)
#define I2C0_PINSEL_FUNCTION                (0x5 << 22)
//Bits
#define I2C0_EN_BIT                         (0x1 << 6)
#define I2C0_BIT_SI                         (0x1 << 3)
#define I2C0_BIT_STA                        (0x1 << 5)
#define I2C0_BIT_STO                        (0x1 << 4)
#define I2C0_BIT_AA                         (0x1 << 2)
// Clock
#define I2C0_CLOCK_MODE_SHIFT               (14)
// init
#define I2C0_INIT_MASK                      (I2C0_EN_BIT | I2C0_BIT_AA)
#define I2C0_OPER_MODE_MASTER               (0x0)
#define I2C0_OPER_MODE_SLAVE                (0x1)
// Status for user
#define I2C_OPERATION_OK                    (0x1)
#define I2C_OPERATION_NOK                   (0x0)
// Controler Status
#define I2C_BUS_ERROR                       (0x0)
#define CONTROLLER_IDLE                     (0xf8)
#define START_COND_OK                       (0x8)
#define RESTART_COND_OK                     (0x10)
#define SLAW_OK                             (0x18)
#define SLAW_NOT_OK                         (0x20)
#define ARBITRATION_LOST                    (0x38)
#define ACK_OK                              (0x28)
#define ACK_NOK                             (0x30)
#define SLAR_OK                             (0x40)
#define SLAR_NOT_OK                         (0x48)
#define RECEIVE_ACK_OK                      (0x50)
#define RECEIVE_ACK_NOK                     (0x58)
// Operations
#define WRITE_OPERATION                     (0x0)
#define READ_OPERATION                      (0x1)


#define SET_I2C0_POWER_ON                   (LPC_SC->PCONP |= (0x1 << PCONP_I2C0_BIT_SHIFT))
#define SET_I2C0_POWER_OFF                  (LPC_SC->PCONP &= ~(0x1 << PCONP_I2C0_BIT_SHIFT))
// Init
// TODO: mode...
#define I2C0_INIT(mode)                     (LPC_I2C0->I2CONSET = I2C0_EN_BIT)
#define I2C0_INIT_AS_MASTER                 (I2C0_INIT(I2C0_OPER_MODE_MASTER))
#define I2C0_INIT_AS_SLAVE                  (I2C0_INIT(I2C0_OPER_MODE_SLAVE))
#define I2C0_SET_CLOCK(ch, cl) \
 do { \
     LPC_I2C0->I2SCLL = cl; \
     LPC_I2C0->I2SCLH = ch; \
 } while(0)

//
#define I2C0_IS_BUS_READY                   (LPC_I2C0->I2CONSET & I2C0_BIT_SI)
#define I2C0_SET_START_CONDITION            (LPC_I2C0->I2CONSET = I2C0_BIT_STA)
#define I2C0_CLR_START_CONDITION            (LPC_I2C0->I2CONCLR = I2C0_BIT_STA)
#define I2C0_SET_STOP_CONDITION             (LPC_I2C0->I2CONSET = I2C0_BIT_STO)
#define I2C0_CLR_STOP_CONDITION             (LPC_I2C0->I2CONCLR = I2C0_BIT_STO)
#define I2C0_SEND                           (LPC_I2C0->I2CONCLR = I2C0_BIT_SI)
#define I2C0_CLR_SI                         (LPC_I2C0->I2CONCLR = I2C0_BIT_SI)
#define I2C0_GET_STATUS                     (LPC_I2C0->I2STAT)
#define I2C0_SET_DATA(data)                 (LPC_I2C0->I2DAT = *data)
#define I2C0_GET_DATA                       ((char)LPC_I2C0->I2DAT)
#define I2C0_SET_CONF(conf)                 (LPC_I2C0->I2CONSET = conf)
#define I2C0_CLR_CONF(conf)                 (LPC_I2C0->I2CONCLR = conf)
#define I2C0_SET_ACK                        (LPC_I2C0->I2CONSET = I2C0_BIT_AA)
#define I2C0_CLR_ACK                        (LPC_I2C0->I2CONCLR = I2C0_BIT_AA)


/**
 * @brief   I2C controller init
 * @return  nothing
 */
void I2C0_Init(void);

/**
 * Gen start and restart conditions
 * \param isRestart If set to 1 gen restart condition
 * \param address Peripheral's address
 * \param operation Zero for write, 1 for read
 * \return int
 * <br>
 * The operation result
 */
int I2C0_Start_Comunication(int isRestart, char address, int operation);

/**
 * @brief   Stops an ongoing communication and frees the BUS
 * @return  int
 * <br>
 * The operation result
 */
int I2C0_Stop_Comunication(void);

/**
 * Sends len bytes
 * \param data Source Buffer
 * \param len Buffer size
 * \return int
 * <br>
 * The operation result
 */
int I2C0_Send_Data(char *data, int len);


/**
 * Reads one byte
 * \param buff Destination buffer
 * \return int
 * <br>
 * The operation result
 */
int I2C0_Receive_Byte(char *buff);

/*
 * Receives 'len' bytes
 *
 * \param buff Destination buffer
 * \param len Buffer's size
 * \return int
 * <br>
 * The operation result
 */
int I2C0_Receive_Data(char *buff, int len);

#endif /* DRIVERS_I2C_DRV_H_ */


/**
 * @}
 */

/**
 * @}
 */
