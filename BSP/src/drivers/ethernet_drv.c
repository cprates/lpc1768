/**
 * @file     ethernet_drv.c
 * @brief    Ethernet driver
 * @version  1.0
 * @date     15 Jun. 2017
 *
 **/

#include "drivers/ethernet_drv.h"
#include "devices/timer_dev.h"
#include <string.h>

//#define DEBUG_ETH

static eth_addr mAddr;

extern void DelayPort(unsigned int ms);
extern void YieldPort(void);


static void WriteToPHY(int reg, int writeval) {
    unsigned int loop;
    // Set up address to access in MII Mgmt Address Register
    LPC_EMAC->MADR = (PHY_DEF_ADR << PHY_DEF_ADR_SHIFT) | reg;
    // Write value into MII Mgmt Write Data Register
    LPC_EMAC->MWTD = writeval;
    // Loop whilst write to PHY completes
    for (loop = 0; loop < MII_WR_TOUT; loop++) {
        if ((LPC_EMAC->MIND & MIND_BUSY) == 0) {
            break;
        }
    }
}

static unsigned short ReadFromPHY(unsigned char reg) {
    unsigned int loop;
    // Set up address to access in MII Mgmt Address Register
    LPC_EMAC->MADR = (PHY_DEF_ADR << PHY_DEF_ADR_SHIFT) | reg;
    // Trigger a PHY read via MIIMgmt Command Register
    LPC_EMAC->MCMD = MCMD_READ;
    // Loop whilst read from PHY completes
    for (loop = 0; loop < MII_RD_TOUT; loop++) {
        if ((LPC_EMAC->MIND & MIND_BUSY) == 0)  {
            break;
        }
    }
    LPC_EMAC->MCMD = 0;
    // Cancel read
    // Returned value is in MIIMgmt Read Data Register
    return (LPC_EMAC->MRDD);
}

static void ETH_Reset(void) {
    LPC_EMAC->MAC1 = 0x1 << 8 |     // RESET TX
                     0x1 << 9 |     // RESET MCS / TX
                     0x1 << 10 |    // RESET RX
                     0x1 << 11 |    // RESET MCS / RX
                     0x1 << 14 |    // SIMULATION RESET
                     0x1 << 15;     // Soft Reset

    LPC_EMAC->Command = 0x1 << 3 |  // RegReset
                        0x1 << 4 |  // TxReset
                        0x1 << 5;   // RxReset
}

static void update_consume_idx(void) {
    unsigned int idx = LPC_EMAC->RxConsumeIndex;
    LPC_EMAC->RxConsumeIndex = (++idx) % NUM_RX_FRAG;
}

static void update_produce_idx(void) {
    unsigned int idx = LPC_EMAC->TxProduceIndex;
    LPC_EMAC->TxProduceIndex = (++idx) % NUM_TX_FRAG;
}



int ETH_Init(eth_addr *macAddr) {
    // Power On
    SET_ETH_POWER_ON;
    // Pin function
    SET_PIN_GROUP_FUNCTION(2, ETH_PINSEL_MASK1, ETH_PINSEL_FUNCTION1);
    SET_PIN_GROUP_FUNCTION(3, ETH_PINSEL_MASK2, ETH_PINSEL_FUNCTION2);

    ETH_Reset();
    DelayPort(100);


    // Initialisation
    // Pass all received frames - Control frames included - AND sets bit 15 to 0
#ifdef DEBUG_ETH
    LPC_EMAC->MAC1 = 0x1 << 1;
#else
    LPC_EMAC->MAC1 = 0;
#endif
    // enable RMII interface after unset MAC1 bit 15
    LPC_EMAC->Command = (LPC_EMAC->Command & (~ETH_REG_MASK_Command)) | (0x1 << 9);
    //
    LPC_EMAC->MAC2 = 0x1 << 4 | // Set CRC to all frames
                     0x1 << 5;  // Padding to short 60B frames. bit 6 and 7 MUST be set to 0
                     //0x1 << 6;
    //
    LPC_EMAC->IPGR = 0x12 << 0 |
                     0x8  << 8;
    // These are the default values, but lets enforce it
    LPC_EMAC->CLRT = 0xF <<  0 |
                     0x37 << 8;
    // / These are the default values, but lets enforce it - 1536 or 0x600...
    LPC_EMAC->MAXF = ETH_MAX_FLEN;
    // Clock
    // TODO: it should be done dynamically base on SystemCoreClock and table 140 on pág. 163 from the datasheet
    LPC_EMAC->MCFG = (LPC_EMAC->MCFG & (~ETH_REG_MASK_MCFG)) |
                     ((0xA << 2) | (0x1 << 15) ); // 0xA - Host Clock divided by 44 --- 0x15 MII reset
    // Unsets the reset of MII set in the line above
    LPC_EMAC->MCFG &= ~(0x1 << 15);

    // RxEnable/TxEnable = 0
    LPC_EMAC->Command &= ~0x3;

#ifdef DEBUG_ETH
    // Pass Rx filter - bit 7
    LPC_EMAC->Command |= 0x1 << 7;
#endif



    // Reset PHY - bit 15 is self-clearing
    WriteToPHY(0, 0x1 << 15);
    // See section 5.3.6.2  pag. 51 of datasheet
    DelayPort(600);

    //**************** Start auto-negotiation *****************/
    unsigned short regData = ReadFromPHY(PHY_REG_0);
    WriteToPHY(PHY_REG_0, PHY_AUTONEGOTIATION(regData));

    // Set auto-negotiation for full, half, 10 and 100 - All capable
    regData = ReadFromPHY(PHY_REG_18);
    WriteToPHY(PHY_REG_18, PHY_BUILD_MODE(regData, PHY_MODE_ALL_CAPABLE));
    unsigned int startTicks = timer_get_ticks();
    while(1) {
        regData = ReadFromPHY(PHY_REG_1);
        if(PHY_AUTONEGOTIATION_DONE(regData)) {
            break;
        }
        // timeout?
        else if(TicksToMS(timer_elapsed_ticks(startTicks)) > AUTO_N_TIMEOUT) {
            return AUTO_N_ERROR;
        }
        YieldPort();
    }

    startTicks = timer_get_ticks();
    do {
        regData = ReadFromPHY(PHY_REG_1);
        if(TicksToMS(timer_elapsed_ticks(startTicks)) > LINK_TIMEOUT) {
            return LINK_ERROR;
        }
        YieldPort();
    } while(!PHY_LINK_UP(regData));

    /***********  Config speed and duplex  ***********/
    regData = ReadFromPHY(PHY_REG_31);
    // Duplex
    switch(PHY_DUPLEX(regData)) {
    case PHY_HALF_DUPLEX_10:
    case PHY_HALF_DUPLEX_100:
        // MAC2 - bit0 = 0
        LPC_EMAC->MAC2 &= ~0x1;
        // IPGT - bit6:0 = 0x12
        LPC_EMAC->IPGT = (LPC_EMAC->IPGT & (~ETH_REG_MASK_IPGT)) | IPGT_HALF_DUP_VAL;
        // Command - bit 10 = 0
        LPC_EMAC->Command &= ~(0x1 << 10);
        break;
    case PHY_FULL_DUPLEX_10:
    case PHY_FULL_DUPLEX_100:
        // MAC2 - bit0 = 1
        LPC_EMAC->MAC2 |= 0x1;
        // IPGT - bit6:0 = 0x15
        LPC_EMAC->IPGT = (LPC_EMAC->IPGT & (~ETH_REG_MASK_IPGT)) | IPGT_FULL_DUP_VAL;
        // Command - bit 10 = 1
        LPC_EMAC->Command |= 0x1 << 10;
        break;
    }

    // Speed
    switch(PHY_SPEED(regData)) {
    case PHY_SPEED_10:
        LPC_EMAC->SUPP &= ~(0x1 << 8);
        break;
    case PHY_SPEED_100:
        LPC_EMAC->SUPP |= 0x1 << 8;
        break;
    }


    /********* Config MAC Address **********/
    //memcpy(&mAddr, macAddr, sizeof(eth_addr));
    mAddr.addr16[0] = HTONS_(macAddr->addr16[2]);
    mAddr.addr16[1] = HTONS_(macAddr->addr16[1]);
    mAddr.addr16[2] = HTONS_(macAddr->addr16[0]);
    //
    LPC_EMAC->SA0 = mAddr.addr16[0];
    LPC_EMAC->SA1 = mAddr.addr16[1];
    LPC_EMAC->SA2 = mAddr.addr16[2];

    /*********   Rx Descriptor   **********/
    LPC_EMAC->RxDescriptor = RX_DESC_BASE;
    LPC_EMAC->RxStatus = RX_STAT_BASE;
    LPC_EMAC->RxDescriptorNumber = NUM_RX_FRAG - 1;
    LPC_EMAC->RxConsumeIndex = 0;

    int i;
    desc_entry *tmp = (desc_entry *)LPC_EMAC->RxDescriptor;
    for(i = 0; i < NUM_RX_FRAG; i++, tmp++) {
        tmp->addr = RX_BUF_BASE + ETH_FRAG_SIZE*i;
        tmp->control = (ETH_FRAG_SIZE - 1) | (0x1 << 31); // with interrupt
        //tmp->control = ETH_FRAG_SIZE - 1; // without interrupt
    }

    // Status
    memset( (char *) RX_STAT_BASE, 0x0, sizeof(desc_entry) * NUM_RX_FRAG);

    /*********   Tx Descriptor   **********/
    LPC_EMAC->TxDescriptor = TX_DESC_BASE;
    LPC_EMAC->TxStatus = TX_STAT_BASE;
    LPC_EMAC->TxDescriptorNumber = NUM_TX_FRAG - 1;
    LPC_EMAC->TxProduceIndex = 0;

    tmp = (desc_entry *)LPC_EMAC->TxDescriptor;
    for(i = 0; i < NUM_TX_FRAG; i++, tmp++) {
        tmp->addr = TX_BUF_BASE + ETH_FRAG_SIZE * i;
        //tmp->control = (ETH_FRAG_SIZE - 1) | (0x1 << 31); // with interrupt
        tmp->control = 0;
    }

    // Status
    memset( (char *) TX_STAT_BASE, 0x0, sizeof(int) * NUM_TX_FRAG);

    /*********   Perfect Match   **********/
    LPC_EMAC->RxFilterCtrl = 0x3F;

    /*********   Interrupts   **********/
    LPC_EMAC->IntEnable = 0x1 << 3;
    LPC_EMAC->IntClear = 0x30FF;

    /********   Enable Rx/TxPaths   ********/
    LPC_EMAC->Command |= 0x3;
    LPC_EMAC->MAC1 |= 0x1;

    // init OK
    return INIT_OK;
}

unsigned int ETH_isUp(void) {
    unsigned short regData = ReadFromPHY(PHY_REG_1);
    unsigned short link = (PHY_LINK_UP(regData) >> PHY_R1_LINK_STAT_SHIFT);
    unsigned short autoNeg = (PHY_AUTONEGOTIATION_DONE(regData) >> PHY_R1_ATNEGOTIATION_DONE_SHIFT);

    return (link & autoNeg);
}

unsigned int ETH_Data_Received() {
    return LPC_EMAC->RxConsumeIndex != LPC_EMAC->RxProduceIndex;
}

unsigned int ETH_Data_Full() {
    return (LPC_EMAC->TxProduceIndex == LPC_EMAC->TxConsumeIndex - 1)
            || (LPC_EMAC->TxProduceIndex - LPC_EMAC->TxConsumeIndex == NUM_TX_FRAG - 1);
}

unsigned int ETH_Receive_Frame(void *dst, unsigned int len) {
    // sanity check
    if(!ETH_Data_Received() || !ETH_isUp()) {
        return 0;
    }

    void *ptrFrameStatus = FRAME_GET(RX_STAT_BASE, LPC_EMAC->RxConsumeIndex);
    // TODO
//  if(FRAME_RX_HAS_ERRORS(ptrFrameStatus)) {
//      update_consume_idx();
//      return 0;
//  }

    // Stores it in the stack to make it safer. When using preemptive sistemms like FreeRTOS it can be a window 
    // between the update of the index and the function return
    //
    // Note: RxSize in status is '-1 encoded'
    // Note2: -4 to drop the CRC
    unsigned int frameSize = FRAME_RX_GET_SIZE(ptrFrameStatus) + 1 - 4;
    //
    memcpy(dst, FRAME_GET_ADDR(LPC_EMAC->RxDescriptor, LPC_EMAC->RxConsumeIndex), MIN(len, frameSize));
    //
    update_consume_idx();

    return frameSize;
}

static void set_header(void *hdrPtr) {
    memcpy(hdrPtr, &mAddr, sizeof(eth_header));
}

unsigned int ETH_Send_Frame(void *src, unsigned int len) {
    // sanity check
    if(ETH_Data_Full() || !ETH_isUp()){
        return 0;
    }

    desc_entry *ptrDescriptor = FRAME_GET(LPC_EMAC->TxDescriptor, LPC_EMAC->TxProduceIndex);

    ptrDescriptor->control = 0;
    FRAME_TX_SET_SIZE(ptrDescriptor->control, len);
    // Set as last frame and generate interrupt
    ptrDescriptor->control |= 1 << 30 | 0x1 << 31;

    memcpy((void *)ptrDescriptor->addr, src, len);
    //set_header((void *)ptrDescriptor->addr);

    update_produce_idx();

    return len;
}
