/**
 * @file     ethernet_drv.h
 * @brief    Headers for ethernet driver
 * @version  1.0
 * @date     15 Jun. 2017
 *
 **/

 /** @addtogroup DRIVERS
* @{
*/

/** @defgroup Ethernet_DRIVER Ethernet Drivers
* @{
*/

#ifndef DRIVERS_ETHERNET_DRV_H_
#define DRIVERS_ETHERNET_DRV_H_

#include "LPC17xx.h"
#include "drivers/gpio_drv.h"

/* *******************  Constants  ******************** */
#define DWORD                           8
#define WORD                            4
#define ETH_PCONP_BIT_SHIFT             (0x1E) // 30
//#define ETH_PINSEL_MASK                   (0xF03F030F)
#define ETH_PINSEL_MASK1                (0x3 << 0  | \
                                         0x3 << 2  | \
                                         0x3 << 8  | \
                                         0x3 << 16 | \
                                         0x3 << 18 | \
                                         0x3 << 20 | \
                                         0x3 << 28 | \
                                         0x3 << 30)

#define ETH_PINSEL_MASK2                (0x3 << 0  | \
                                         0x3 << 2)
//
#define ETH_PINSEL_FUNCTION1            (PINSEL_SEL_ALT1 << 0 | \
                                         PINSEL_SEL_ALT1 << 2 | \
                                         PINSEL_SEL_ALT1 << 8 | \
                                         PINSEL_SEL_ALT1 << 16 | \
                                         PINSEL_SEL_ALT1 << 18 | \
                                         PINSEL_SEL_ALT1 << 20 | \
                                         PINSEL_SEL_ALT1 << 28 | \
                                         PINSEL_SEL_ALT1 << 30)

#define ETH_PINSEL_FUNCTION2            (PINSEL_SEL_ALT1 << 0 | \
                                         PINSEL_SEL_ALT1 << 2) //!< EMAC Memory Buffer configuration for 16K Ethernet RAM.

#define NUM_RX_FRAG                     4 //!< Num.of RX Fragments 4*1536= 6.0kB
#define NUM_TX_FRAG                     2 //!< Num.of TX Fragments 3*1536= 4.6kB
#define ETH_FRAG_SIZE                   1536 //!< Packet Fragment size 1536 Bytes
#define ETH_MAX_FLEN                    1536 //!< ax. Ethernet Frame Size
// EMAC variables located in AHB SRAM bank 1
#define RX_DESC_BASE                    0x2007C000
#define RX_STAT_BASE                    (RX_DESC_BASE + NUM_RX_FRAG*DWORD)
#define TX_DESC_BASE                    (RX_STAT_BASE + NUM_RX_FRAG*DWORD)
#define TX_STAT_BASE                    (TX_DESC_BASE + NUM_TX_FRAG*DWORD)
#define RX_BUF_BASE                     (TX_STAT_BASE + NUM_TX_FRAG*WORD)
#define TX_BUF_BASE                     (RX_BUF_BASE  + NUM_RX_FRAG*ETH_FRAG_SIZE)
//
#define PHY_DEF_ADR                     1  //!< EPHY address
#define PHY_DEF_ADR_SHIFT               8
#define MII_WR_TOUT                     SystemCoreClock * 2 //!< wait ~2 seconds
#define MII_RD_TOUT                     SystemCoreClock * 2 //!< wait ~2 seconds
#define MCMD_WRITE                      0
#define MCMD_READ                       1
#define MIND_BUSY                       0x1
#define IPGT_HALF_DUP_VAL               0x12
#define IPGT_FULL_DUP_VAL               0x15
// Reg Masks
#define ETH_REG_MASK_MCFG               0x803F
#define ETH_REG_MASK_Command            0x7FB
#define ETH_REG_MASK_IPGT               0x7F
// ***** PHY *****
#define PHY_REG_0                       0
#define PHY_REG_1                       1
#define PHY_REG_18                      18
#define PHY_REG_29                      29
#define PHY_REG_31                      31
#define PHY_MODE_ALL_CAPABLE            0x7
#define PHY_MODE_SHIFT                  5
#define PHY_MODE_MASK                   (7 << PHY_MODE_SHIFT)
#define PHY_HALF_DUPLEX_10              0x0
#define PHY_HALF_DUPLEX_100             0x1
#define PHY_FULL_DUPLEX_10              0x2
#define PHY_FULL_DUPLEX_100             0x3
#define PHY_SPEED_10                    0x1
#define PHY_SPEED_100                   0x0
// For R.0
#define PHY_R0_AUTONEGOTIATION_SHIFT    12
// For R.1
#define PHY_R1_ATNEGOTIATION_DONE_SHIFT 5
#define PHY_R1_LINK_STAT_SHIFT          2
// For R.31
#define PHY_R31_DUPLEX_SHIFT            3
#define PHY_R31_DUPLEX_MASK             (0x3 << PHY_R31_DUPLEX_SHIFT)
#define PHY_R31_SPEED_SHIFT             2
#define PHY_R31_SPEED_MASK              (0x1 << PHY_R31_SPEED_SHIFT)
//
#define RX_FRAME_ERRORS_MASK            0x1
#define RX_FRAME_ERRORS_SHIFT           31
#define FRAME_SIZE_MASK                 0x7FF
#define FRAME_SIZE_SHIFT                0
//
#define AUTO_N_TIMEOUT                  5000 //!< Auto-negotiation timeout in milliseconds
#define LINK_TIMEOUT                    5000 //!< Link timeout in milliseconds
// Errors
#define INIT_OK                         0
#define AUTO_N_ERROR                    -1
#define LINK_ERROR                      -2

/* *******************  Functions  ******************** */
#define SET_ETH_POWER_ON                (LPC_SC->PCONP |= (0x1 << ETH_PCONP_BIT_SHIFT))
#define SET_ETH_POWER_OFF               (LPC_SC->PCONP &= ~(0x1 << ETH_PCONP_BIT_SHIFT))
#define MIN(val1, val2)                 ((val1 < val2)? val1 : val2)
// Network
#ifndef HTONS_
#define HTONS_(addr)                    (0xFFFF & (addr >> 8 | addr << 8))
#endif
//#define NTOHS(addr)                   (0xFFFF & (addr >> 8 | addr << 8))
// PHY
#define PHY_BUILD_MODE(currVal, mode)   ((currVal & (~PHY_MODE_MASK)) | (mode << PHY_MODE_SHIFT))
#define PHY_AUTONEGOTIATION(currVal)    (currVal | (0x1 << PHY_R0_AUTONEGOTIATION_SHIFT))
#define PHY_AUTONEGOTIATION_DONE(val)   (val & (0x1 << PHY_R1_ATNEGOTIATION_DONE_SHIFT))
#define PHY_LINK_UP(val)                (val & (0x1 << PHY_R1_LINK_STAT_SHIFT))
#define PHY_DUPLEX(val)                 ((val & PHY_R31_DUPLEX_MASK) >> PHY_R31_DUPLEX_SHIFT)
#define PHY_SPEED(val)                  ((val & PHY_R31_SPEED_MASK) >> PHY_R31_SPEED_SHIFT)
//
#define FRAME_RX_HAS_ERRORS(ptr)        ((*(int *)ptr) & (RX_FRAME_ERRORS_MASK << RX_FRAME_ERRORS_SHIFT))
#define FRAME_RX_GET_SIZE(ptr)          ((*(int *)ptr) & (FRAME_SIZE_MASK << FRAME_SIZE_SHIFT))
#define FRAME_GET_ADDR(base, idx)       ((void *)(((desc_entry *)base) + idx)->addr)
#define FRAME_GET(base, idx)            ((void *)(((desc_entry *)base) + idx))
#define FRAME_TX_SET_SIZE(control, val) control = ((control & ~(FRAME_SIZE_MASK << FRAME_SIZE_SHIFT)) | (val - 1) << FRAME_SIZE_SHIFT)


/// MAC addr
struct eth_addr_t {
    union {
        unsigned char addr8[6];
        unsigned short addr16[3];
    };
};
typedef struct eth_addr_t eth_addr;

/// ETH frame header
struct eth_header_t {
    eth_addr dstAddr;
    eth_addr srcAddr;
    short type;
};
typedef struct eth_header_t eth_header;

/// Rx/TxDescriptors
struct desc_entry_t {
    unsigned int addr;
    unsigned int control;
};
typedef struct desc_entry_t desc_entry;

/**
 * @brief   Ethernet controller init
 * @return  nothing
 */
int ETH_Init(eth_addr *macAddr);

/**
 * Returns the link and autonegotiation status
 *
 * \return Returns zero if no link or 1 is link and auto-negotiation ar up
 */
unsigned int ETH_ipUp(void);

/**
 * Checks if there are frames waiting to be read
 *
 * \return Returns 1 if there are frames to be read, zero otherwise
 */
unsigned int ETH_Data_Received();

/**
 * Copies the next frame to 'dst' address
 *
 * \param dst Destination buffer
 * \param dst Buffer's size
 *
 * \return Returns received frame's size
 */
unsigned int ETH_Receive_Frame(void *dst, unsigned int len);

/**
 * Send a frame
 *
 * \param src Origin buffer where the frame is stored
 * \param len Buffer size
 *
 * \return unsigned int
 * <br>
 * Returns len if successful, zero otherwise
 */
unsigned int ETH_Send_Frame(void *src, unsigned int len);

#endif /* DRIVERS_ETHERNET_DRV_H_ */


/**
* @}
*/
 /**
* @}
*/
