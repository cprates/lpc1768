/**
 * @file     flash_drv.c
 * @brief    Flash driver
 * @version  1.0
 * @date     15 Jun. 2017
 *
 **/

#include <string.h>

#include "drivers/flash_drv.h"
#include "devices/timer_dev.h"

// TODO: find another solution
static char erase_buffer[FLASH_PAGE_SIZE];
//
static char read_buffer[FLASH_PAGE_SIZE];
static unsigned char pageData[FLASH_PAGE_SIZE];


unsigned int FLASH_ErasePages(unsigned int startSector, unsigned int endSector) {
    int retVal;
    short addr = PAGE_TO_ADDR(startSector);

    while(startSector <= endSector) {
        retVal = FLASH_WritePage((void *)addr, (void *)erase_buffer);
        if(retVal != FLASH_OPER_SUCCESS) {
            return FLASH_OPER_FAIL;
        }

        startSector++;
        addr = PAGE_TO_ADDR(startSector);
        Delay(5);
    }


    // Stop
    retVal = I2C0_Stop_Comunication();
    if(retVal != I2C_OPERATION_OK) {
        return FLASH_OPER_FAIL;
    }

    return FLASH_OPER_SUCCESS;
}

void FLASH_Init(void) {
    I2C0_Init();
    memset(&erase_buffer, FLASH_DEFAULT_VALUE, FLASH_PAGE_SIZE);
}

unsigned int FLASH_WritePage(void *dstAddr, void *srcAddr) {
    int retVal = I2C0_Start_Comunication(0, DEVICE_ADDR, WRITE_OPERATION);
    if(retVal != I2C_OPERATION_OK) {
        return FLASH_OPER_FAIL;
    }

    // send write address
    unsigned short bigendianAddr = TO_BIG_ENDIAN_SHORT(dstAddr);
    retVal = I2C0_Send_Data((char*)&bigendianAddr, 2);
    if(retVal != I2C_OPERATION_OK) {
        return FLASH_OPER_FAIL;
    }
    // send data
    retVal = I2C0_Send_Data((char *)srcAddr, FLASH_PAGE_SIZE);
    if(retVal != I2C_OPERATION_OK) {
        return FLASH_OPER_FAIL;
    }
    
    // Stop
    retVal = I2C0_Stop_Comunication();
    if(retVal != I2C_OPERATION_OK) {
        return FLASH_OPER_FAIL;
    }


    // According to the datasheet writing to a page can take up to 5000ns
    // Because it's easy for the caller to forget, do the delay here
    Delay(5);
    return FLASH_OPER_SUCCESS;
}

unsigned int FLASH_ReadPage(void *dstAddr, void *srcAddr) {
    int retVal;

    retVal = I2C0_Start_Comunication(0, DEVICE_ADDR, WRITE_OPERATION);
    if(retVal != I2C_OPERATION_OK) {
        return FLASH_OPER_FAIL;
    }
    // Set read address
    unsigned short bigendianAddr = TO_BIG_ENDIAN_SHORT(srcAddr);
    retVal = I2C0_Send_Data((char*)&bigendianAddr, sizeof(short));
    if(retVal != I2C_OPERATION_OK) {
        return FLASH_OPER_FAIL;
    }
    // Begin read operation
    retVal = I2C0_Start_Comunication(1, DEVICE_ADDR, READ_OPERATION);
    if(retVal != I2C_OPERATION_OK) {
        return FLASH_OPER_FAIL;
    }
    // Read
    retVal = I2C0_Receive_Data(dstAddr, FLASH_PAGE_SIZE);
    if(retVal != I2C_OPERATION_OK) {
        return FLASH_OPER_FAIL;
    }
    Delay(1);
    // Stop
    I2C0_Stop_Comunication();

    return FLASH_OPER_SUCCESS;
}


unsigned int FLASH_ReadData(void *dstAddr, void *srcAddr, unsigned int size) {
    // TODO: This could be improved by write byte by byte instead of writing an entire page
    // if size is less then half a FLASH_PAGE_SIZE. Otherwise write page

    unsigned int len;
    do {
        unsigned int retVal = FLASH_ReadPage(read_buffer, srcAddr);
        if(retVal != FLASH_OPER_SUCCESS) {
            return retVal;
        }

        // update size and len
        len = (size > FLASH_PAGE_SIZE)? FLASH_PAGE_SIZE : size;
        size -= len;
        // copy
        memcpy(dstAddr, read_buffer, len);

        // update pointers
        dstAddr = ((char *)dstAddr) + len;
        srcAddr = ((char *)srcAddr) + len;
    } while(size > 0);

    return FLASH_OPER_SUCCESS;
}


static unsigned int calcCopySize(const unsigned int startAddr, const unsigned int size) {
    unsigned int sector = ADDR_TO_PAGE((unsigned int)startAddr);
    unsigned int sectorStartAddr = sector * FLASH_PAGE_SIZE;
    unsigned int sectorEndAddr = sectorStartAddr + FLASH_PAGE_SIZE - 1;
    unsigned int offsetSize = (sectorEndAddr - startAddr + 1);

    return (offsetSize >= size)? size : offsetSize;
}

static unsigned int mapFlashAddrToStackAddr(unsigned int flashAddr, unsigned int stackAddr, unsigned int sector) {
    unsigned int sectorStartAddr = sector * FLASH_PAGE_SIZE;
    unsigned int flashOffset = flashAddr - sectorStartAddr;

    return stackAddr + flashOffset;
}

unsigned int FLASH_WriteData(void *dstAddr, void *srcAddr, unsigned int size) {
    if( ((unsigned int) dstAddr) + size > FLASH_BASE_ADDR + FLASH_SIZE - 1 ) return FLASH_OPER_FAIL;
    if(size > FLASH_SIZE) return FLASH_OPER_FAIL;

    int retVal;

    unsigned int sector;
    unsigned int pageBaseAddr;
    unsigned int sizeToCopy;
    unsigned int stackDstAddr;
    do {
        sector = ADDR_TO_PAGE((unsigned int)dstAddr);
        pageBaseAddr = PAGE_TO_ADDR(sector);

        FLASH_ReadPage(pageData, (void *)pageBaseAddr);

        sizeToCopy = calcCopySize((unsigned int)dstAddr, size);
        stackDstAddr = mapFlashAddrToStackAddr((unsigned int)dstAddr, (unsigned int)pageData, sector);
        memcpy((void *)stackDstAddr, srcAddr, sizeToCopy);

        retVal = FLASH_WritePage((void *)pageBaseAddr, pageData);
        if(retVal != FLASH_OPER_SUCCESS) {
            return retVal;
        }

        dstAddr = ((char *)dstAddr) + sizeToCopy; // Bytes
        srcAddr = ((char *)srcAddr) + sizeToCopy; // Bytes
        size -= sizeToCopy;

    } while(size > 0);
    
    return FLASH_OPER_SUCCESS;
}


unsigned int FLASH_VerifyData(void *dstAddr, void *srcAddr, unsigned int size) {
    int retVal;

    unsigned int sizeToCopy;
    do {
        sizeToCopy = calcCopySize((unsigned int)dstAddr, size);

        retVal = FLASH_ReadPage(read_buffer, dstAddr);
        if(retVal != FLASH_OPER_SUCCESS) {
            return retVal;
        }

        if(memcmp(srcAddr, read_buffer, sizeToCopy) != 0) {
            return FLASH_OPER_FAIL;
        }

        dstAddr = ((char *)dstAddr) + sizeToCopy;
        srcAddr = ((char *)srcAddr) + sizeToCopy;
        size -= sizeToCopy;

    } while(size > 0);

    return FLASH_OPER_SUCCESS;
}
