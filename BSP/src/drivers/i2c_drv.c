/**
 * @file     i2c_drv.c
 * @brief    I2C driver
 * @version  1.0
 * @date     15 Jun. 2017
 *
 **/
#include "drivers/i2c_drv.h"
#include "devices/timer_dev.h"

void I2C0_Init(void);



static int check_status(void) {
	int status = I2C0_GET_STATUS;

	switch(status) {
	case ACK_OK:
	case CONTROLLER_IDLE:
	case SLAW_OK:
	case SLAR_OK:
	case RECEIVE_ACK_OK:
	case RESTART_COND_OK:
	case START_COND_OK:
		return I2C_OPERATION_OK;

	case ACK_NOK:
	case ARBITRATION_LOST:
	case SLAW_NOT_OK:
	case SLAR_NOT_OK:
	case RECEIVE_ACK_NOK:
	case I2C_BUS_ERROR:
		I2C0_SET_CONF(I2C0_BIT_STO | I2C0_BIT_AA);
		I2C0_CLR_SI;
		return I2C_OPERATION_NOK;

	default:
		// Unknown error! Restart I2C controller
		I2C0_Init();
		return I2C_OPERATION_NOK;
	}
}

static int I2C0_Send_Raw_Byte(char *data) {
	I2C0_SET_DATA(data);
	I2C0_SEND;
	while(!I2C0_IS_BUS_READY);

	return check_status();
}

static int I2C0_Select_Slave(char address, int operation) {
	int retVal;
	char data = (address << 1) | operation;
	retVal = I2C0_Send_Raw_Byte(&data);

	return retVal;
}



void I2C0_Init(void) {
	SET_I2C0_POWER_OFF;
	Delay(10);
	SET_I2C0_POWER_ON;
	SET_PIN_GROUP_FUNCTION(1, I2C0_PINSEL_MASK, I2C0_PINSEL_FUNCTION);
	//
	SET_PERIPHERAL_CLOCK_MODE(0, I2C0_CLOCK_MODE_SHIFT, CLOCK_MODE_ONE_FOURTH_OF_CCLOCK);

	//
	I2C0_CLR_CONF(I2C0_BIT_STA | I2C0_BIT_STO | I2C0_BIT_SI | I2C0_BIT_AA);

	// Normal mode
	LPC_PINCON->I2CPADCFG &= ~((0x1<<0)|(0x1<<2));

	// Clock
	// 128 because it's using 1/4 of CCLOCK (25MHz),
	// Accourding to datasheet's pag. 457, table 395, the sum of both must be around 250, hance 256
	// TODO: Could be calculated based on the value in I2CPCLOCK
	//LPC_I2C0->I2SCLL   = 0x80;
	//LPC_I2C0->I2SCLH   = 0x80;
	I2C0_SET_CLOCK(0x80, 0x80);

	I2C0_INIT_AS_MASTER;
}

int I2C0_Start_Comunication(int isRestart, char address, int operation) {
	//
	I2C0_SET_START_CONDITION;
	if(isRestart) {
		I2C0_SEND;
	}
	while(!I2C0_IS_BUS_READY);
	//
	I2C0_CLR_START_CONDITION;

	// Start Condition Done!
	int retVal = check_status();
	if(retVal != I2C_OPERATION_OK) {
		return retVal;
	}

	// Select Slave
	return I2C0_Select_Slave(address, operation);
}

int I2C0_Stop_Comunication(void) {
	I2C0_SET_STOP_CONDITION;
	I2C0_CLR_SI;

	return check_status();
}


int I2C0_Send_Data(char *data, int len) {
	int retVal;

	// Send Data
	int i;
	for(i = 0; i< len; i++, data++) {
		retVal = I2C0_Send_Raw_Byte(data);
		if(retVal == I2C_OPERATION_NOK) {
			return retVal;
		}
	}

	return retVal;
}

int I2C0_Receive_Byte(char *buff) {
	int retVal;

	// Send ACK
	I2C0_SET_ACK;
	I2C0_CLR_SI;
	while(!I2C0_IS_BUS_READY);
	retVal = check_status();
	if(retVal != I2C_OPERATION_OK) {
		return retVal;
	}

	// Read data
	*buff = I2C0_GET_DATA;

	// Send a NACK
	I2C0_CLR_CONF(I2C0_BIT_AA | I2C0_BIT_SI);
	if(I2C0_GET_STATUS != RECEIVE_ACK_NOK) {
		return I2C_OPERATION_NOK;
	}

	return retVal;
}


int I2C0_Receive_Data(char *buff, int len) {
	int retVal;

	int i;
	for(i = 0; i < len; i++) {
		// ACK
		I2C0_SET_ACK;
		I2C0_CLR_SI;
		while(!I2C0_IS_BUS_READY);
		retVal = check_status();
		if(retVal != I2C_OPERATION_OK) {
			return retVal;
		}

		buff[i] = I2C0_GET_DATA;
	}


	// Send a NACK
	I2C0_CLR_CONF(I2C0_BIT_AA | I2C0_BIT_SI);
	while(!I2C0_IS_BUS_READY);
	if(I2C0_GET_STATUS != RECEIVE_ACK_NOK) {
		return I2C_OPERATION_NOK;
	}

	return I2C_OPERATION_OK;
}
