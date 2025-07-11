/**
Copyright 2011 by Masoneilan - GE Energy, as an unpublished work.  All rights resesrved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file stm_flash.c

    \brief Driver for the system stm32f flash

    OWNER:
    CPU: STM32F10xx Cortex

    $Revision:  $
*/
#include "mnwrap.h"
#include "stmcommon.h"
#include "mn_flash.h"
#include "crc.h"

//lint ++flb
#define FLASH_R_BASE            (AHBPERIPH_BASE + 0x2000u) /*!< Flash registers base address */
#define FLASH                   HARDWARE(FLASH_TypeDef *, FLASH_R_BASE)

#define FLASHB1                 HARDWARE(FLASHB_Typedef *, FLASH_R_BASE)
#define FLASHB2                 HARDWARE(FLASHB_Typedef *, (FLASH_R_BASE + 0x40u))

typedef struct
{
    IO_REG32(ACR);
    IO_REG32(KEYR);
    IO_REG32(OPTKEYR);
    IO_REG32(SR);
    IO_REG32(CR);
    IO_REG32(AR);
} FLASHB_Typedef;

typedef struct
{
    IO_REG32(ACR);
    IO_REG32(KEYR);
    IO_REG32(OPTKEYR);
    IO_REG32(SR);
    IO_REG32(CR);
    IO_REG32(AR);
    IO_REG32(RESERVED);
    IO_REG32(OBR);
    IO_REG32(WRPR);
#ifdef STM32F10X_XL
    u32 RESERVED1[8];
    IO_REG32(KEYR2);
    u32 RESERVED2;
    IO_REG32(SR2);
    IO_REG32(CR2);
    IO_REG32(AR2);
#endif /* STM32F10X_XL */
} FLASH_TypeDef;

typedef struct
{
    volatile u16 RDP;
    volatile u16 USER;
    volatile u16 Data0;
    volatile u16 Data1;
    volatile u16 WRP0;
    volatile u16 WRP1;
    volatile u16 WRP2;
    volatile u16 WRP3;
} OB_TypeDef;
#define OB                      HARDWARE(OB_TypeDef *, OB_BASE)
#define OB_BASE                 0x1FFFF800u    /*!< Flash Option Bytes base address */



#define FLASH_Latency_0                ((u32)0x00000000)  /*!< FLASH Zero Latency cycle */
#define FLASH_Latency_1                ((u32)0x00000001)  /*!< FLASH One Latency cycle */
#define FLASH_Latency_2                ((u32)0x00000002)  /*!< FLASH Two Latency cycles */
#define IS_FLASH_LATENCY(LATENCY) (((LATENCY) == FLASH_Latency_0) || \
                                   ((LATENCY) == FLASH_Latency_1) || \
                                   ((LATENCY) == FLASH_Latency_2))

#define FLASH_HalfCycleAccess_Enable   ((u32)0x00000008)  /*!< FLASH Half Cycle Enable */
#define FLASH_HalfCycleAccess_Disable  ((u32)0x00000000)  /*!< FLASH Half Cycle Disable */
#define IS_FLASH_HALFCYCLEACCESS_STATE(STATE) (((STATE) == FLASH_HalfCycleAccess_Enable) || \
                                               ((STATE) == FLASH_HalfCycleAccess_Disable))

#define FLASH_PrefetchBuffer_Enable    ((u32)0x00000010)  /*!< FLASH Prefetch Buffer Enable */
#define FLASH_PrefetchBuffer_Disable   ((u32)0x00000000)  /*!< FLASH Prefetch Buffer Disable */
#define IS_FLASH_PREFETCHBUFFER_STATE(STATE) (((STATE) == FLASH_PrefetchBuffer_Enable) || \
                                              ((STATE) == FLASH_PrefetchBuffer_Disable))


/* FLASH Keys */
#define RDP_Key                 ((u16)0x00A5u)
#define FLASH_KEY1              ((u32)0x45670123u)
#define FLASH_KEY2              ((u32)0xCDEF89ABu)

/* Delay definition */
#define EraseTimeout            ((u32)0x000B0000u)
#define ProgramTimeout          ((u32)0x00002000u)

//lint --flb

//lint ++flb
//Bit 5 EOP: End of operation
#define EOP (1u << 5)
//Set by hardware when a Flash operation (programming / erase) is completed. Reset by
//writing a 1
//Note: EOP is asserted at the end of each successful program or erase operation
//Bit 4 WRPRTERR: Write protection error
#define WRPRTERR (1u << 4)
//Set by hardware when programming a write-protected address of the Flash memory.
//Reset by writing 1.
//Bit 3 Reserved, must be kept cleared.
//Bit 2 PGERR: Programming error
#define PGERR (1u << 2)
//Set by hardware when an address to be programmed contains a value different from
//'0xFFFF' before programming.
//Reset by writing 1.
//Note: The STRT bit in the FLASH_CR register should be reset before starting a programming
//operation.
//Bit 1 Reserved, must be kept cleared
//Bit 0 BSY: Busy
#define BSY (1u << 0)
//This indicates that a Flash operation is in progress. This is set on the beginning of a Flash
//operation and reset when

#define RNG (1u << 6)

#define CR_PG_Set               (1u << 0)
#define CR_PER_Set              (1u << 1)


#define CR_OPTPG_Set            (1u << 4)
#define CR_OPTER_Set            (1u << 5)
#define CR_STRT_Set             (1u << 6)
#define CR_LOCK_Set             (1u << 7)
#define CR_OPTWRE_SET           (1u << 9)
//lint --flb


#define PAGE_SIZE   SECT_SIZE
#define PAGE_MASK   (PAGE_SIZE - 1u)

#define BFB2        ((u16)0x7f)          // MSB clear is signal to boot from bank 2
#define BFB1        ((u16)0xff)          // MSB set   is signal to boot from bank 1

static u32              SR_FlashResult;         // status register (SR) from most recent operation
static FLASHB_Typedef  *BankCtrlReg;            // pointer to active bank1/bank2 control registers
static u32              pgbase = 0u;            // base of current programming page
static u16              fCrc;

// structure for range checking flash write requests
typedef struct
{
    u32     base;
    u32     limit;
} banklimit_t;

#define OFFSET 0 // PAGE_SIZE

static const banklimit_t banklimits[] =
{
    {BANK1_BASE + OFFSET, BANK2_BASE},
    {BANK2_BASE + OFFSET, BANK2_BASE + BANK_SIZE}
};

/** \brief Set the Initial flash ACR
    This function is invoked from the Init code and configures the ACR register.
        Flash wait states:
        000 Zero wait state, if 0 < SYSCLK. 24 MHz
        001 One wait state, if 24 MHz < SYSCLK . 48 MHz
        010 Two wait states, if 48 MHz < SYSCLK . 72 MHz

		0x10 -- Enable the Prefetcher
		0x08 -- Enable the Half Cycle
 \param     - None.
 \return    - Nothing.

*/
void flash_SetACR(void)
{
    FLASH->ACR = 0x10;
}

static void sendkey(volatile u32 *key)
{
    *key = FLASH_KEY1;
    *key = FLASH_KEY2;
}

/** \brief Wait for timout or !flashBusy. Store result in result register

     param - Timeout - Actually, number of retries.
    \return - 0 if OK; otherwise error (BSY | WRPRTERR | PGERR)
*/
static u32 FLASH_Wait(u32 Timeout)
{
    u32 rslt = BankCtrlReg->SR & BSY;

    while ((rslt == BSY) && (Timeout != 0u))
    {
        Timeout--;
        rslt = BankCtrlReg->SR & BSY;
    }
    SR_FlashResult = rslt;
    return rslt & (BSY | WRPRTERR | PGERR);
}

/** \brief Program a half word (16 bits) at the specified address

    \param adr - pointer to the memory location (flash address) of the word to be written
    \para data - the 16 bits to be written
    \return - 0 if OK; otherwise error (BSY | WRPRTERR | PGERR)
*/
static u32 proghword(volatile u16 *adr, u16 data)
{
    u32 status;

    BankCtrlReg->CR |= CR_PG_Set;
    *adr             = data;
    status           = FLASH_Wait(ProgramTimeout);
    BankCtrlReg->CR &= ~CR_PG_Set;
    return status;
}

/** \brief Erase the specified page

    \param pg - the page address (address of first byte in page)
    \return true if no errors; otherwise false
*/
static u32 erasepage(u32 pg)
{
    u32 status;

    BankCtrlReg->CR |= CR_PER_Set;
    BankCtrlReg->AR  = pg;
    BankCtrlReg->CR |= CR_STRT_Set;

    status = FLASH_Wait(EraseTimeout);

    BankCtrlReg->CR &= ~CR_PER_Set;
    return status;
}


/** \brief Update the 16 options bytes.  Must have been erased
*/
static u32 optwrite(u16 *adr, const u16 *data, s32 len)
{
    u32 status = 0u;

    while ((len > 0) && (status == 0u))
    {
        FLASH->CR |= CR_OPTPG_Set;
        *adr++     = *data++;

        status     = FLASH_Wait(ProgramTimeout);

        FLASH->CR &= ~CR_OPTPG_Set;
        len       -= (s32)sizeof(u16);
    }
    return status;
}

/** \brief Erase the option bytes
*/
static u32 opterase(void)
{
    u32 status;

    sendkey(&FLASH->KEYR);
    sendkey(&FLASH->OPTKEYR);

    FLASH->CR |= CR_OPTER_Set;
    FLASH->CR |= CR_STRT_Set;

    status = FLASH_Wait(EraseTimeout);

    FLASH->CR &= ~CR_OPTER_Set;
    return status;
}

#if 0
static void wcopy (u32 *d, u32 *s, u32 len)
{
    while (len-- != 0u)
    {
        *d++ = *s++;
    }
}
#endif

/** \brief Set the preferred boot partition Bank1 or Bank 2

    \param BootFromBank2 - true sets prefered Bank2; false sets preferred Bank1
    \return 0 if OK; otherwise error ((BSY | WRPRTERR | PGERR)
*/
u32 flash_SetPartition(u32 BootFromBank2)
{
    OB_TypeDef lob = *OB;

    // fetch the current options bytes
   // wcopy((u32*)(void*)&lob, (u32*)(void*)OB_BASE, sizeof(lob) / sizeof(u32));

    rcc_enable_HSI();           // enable HSI for flash operations

    BankCtrlReg = FLASHB1;

    u32 status  = opterase();

    if (status == 0u)
    {
        lob.Data0 = (BootFromBank2 != 0) ? BFB2 : BFB1;
        u16 *lob6 = (u16*)(void*)&lob;
        status    = optwrite(HARDWARE(u16*, OB_BASE), lob6, sizeof(lob));
    }

    FLASH->CR &= ~CR_OPTWRE_SET;

    BankCtrlReg = FLASHB1;

    rcc_disable_HSI();           // enable HSI for flash operations

    return status;
}

/** \brief Get the partition base address

    \return - Base Address of the partition to be updated
*/
u32 flash_GetPartitionBaseAddress(u32 dummy)
{
    u32 bankbase = (HARDWARE(u32, flash_GetPartitionBaseAddress) & BANK_MASK) ^ BANK_SIZE;
    u32 *addr    = HARDWARE(u32*, bankbase);

    UNUSED_OK(dummy);
    if (addr[7] == (u32)0xdeadbeefu)         // is partition manager present?
    {
        bankbase += addr[8];                // yes, add offset
    }
    return HARDWARE(u32, bankbase);
}

/** \brief Setup for write of the flash.  Will only enable the bank we are NOT
     running from.

    \return 0 if OK; otherwise error (BSY | WRPRTERR | PGERR)
*/
u32 flash_OpenPartition(u32 sCrc)
{
    // Select the OTHER bank!
    // Note: true  - running from B1, updateing B0;
    //       false - running from B0, updateing B1!
    BankCtrlReg = (HARDWARE(u32, flash_OpenPartition) >= BANK2_BASE) ? FLASHB1 : FLASHB2; //lint !e506 flash_OpenPartition is not constant when linked at a different address

    sendkey(&BankCtrlReg->KEYR);
    rcc_enable_HSI();           // enable HSI for flash operations
    pgbase = 0u;
    fCrc = (u16)sCrc;
    return 0;
}

/** \brief Lock both flash banks
 *
    \return - 0 if OK; otherwise error (BSY | WRPRTERR | PGERR)
*/
u32 flash_ClosePartition(u32 dummy)
{
    u32 rslt = 0u;

    UNUSED_OK(dummy);
//    SR_FlashResult = RNG;               // treat not unlocked as a range error

    if (BankCtrlReg != NULL)            // If selected
    {
        // Lock and "load"
        FLASH->CR  |= CR_LOCK_Set;
#ifdef STM32F10X_XL
        FLASH->CR2 |= CR_LOCK_Set;
#endif
        BankCtrlReg = NULL;
    }
    rcc_disable_HSI();           // enable HSI for flash operations
    return rslt;
}

/** \brief Program a 16 bit array into flash

    \param data - pointer to the data to be written to flash
    \param adr - starting address in flash
    \param len - length in bytes to program
    \return - 0 if OK; otherwise error (BSY | WRPRTERR | PGERR)
*/
u32 flash_ProgramBlock(const u16 *data,  u16 *adr, u32 len, u8_least flags)
{
    u32   anyError;
    const banklimit_t  *banklim = (BankCtrlReg == FLASHB2) ? (&banklimits[1]) : (&banklimits[0]);
    u32                 ad = HARDWARE(u32, adr);

    SR_FlashResult = RNG;           // pseudo hw bit - range error

    if ((flags & UPD_CRC) != 0u)
    {
        fCrc = Crc16((const u8*)data, len, fCrc);
    }
    if ((BankCtrlReg != NULL) &&            // Selected
        (ad >= banklim->base) &&            // Within the Bank Limits
        ((ad + len) <= banklim->limit))     //  ...
    {
        anyError = 0u;
        while (len > 0u)
        {
            ad      = HARDWARE(u32, adr);
            u32 adp = ad & ~PAGE_MASK;
            {
                if (((flags & NO_ERASE) == 0) && adp != pgbase)
                {
                    pgbase = adp;
                    anyError = erasepage(pgbase);
                }

                if (anyError == 0u)
                {   // No errors encountered = program the half word
                    anyError = proghword(adr++, *data++);
                }

                if (anyError != 0u)
                {   // We have errors! - Break!
                    break;
                }
            }
            len -= sizeof(u16);
        }
    }
    return (len == 0u)       // if len not 0 then did not finish, failed somewhere before
        ? fCrc
        : (SR_FlashResult << 16) | 0x80000000u;
}

/** \brief Get the CURRENT partition ID:
    0 - if running from BANK 1
    1 - if running from BANK 2

    \return 0 or 1 - Current FLASH Partition ID
*/
u8          flash_GetRunningPartitionID(void)
{
    u8      retval;
    u32     bankbase;

    bankbase  = HARDWARE(u32, flash_GetRunningPartitionID);
    bankbase &= BANK_MASK;
    if (bankbase == BANK1_BASE)
    {
        retval = 0u;
    }
    else
    {
        retval = 1u;
    }
    return retval;
}

// end of source
