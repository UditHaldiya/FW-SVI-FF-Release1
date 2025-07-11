/**
Copyright 2011 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file bootman.c

    \brief The flash bank boot manager for the FF project

    OWNER: 
    CPU: STM32F10xx Cortex

    $Revision:  $
*/
#define TESTING 0

#define BANK_SIZE       0x80000u                    // 512 Kb
#define BANK_0_LOCN     0x08000000u                 // physical base of bank 0
#define BANK_1_LOCN     (BANK_0_LOCN + BANK_SIZE)   // physical base of bank 1
#define FLASH_PAGE_SIZE 0x800                       // 2048 bytes

#define BOOTSTRAP_LOCN  BANK_0_LOCN                 // boot manager is at base of bank 0

#define FW_0_LOCN       (BOOTSTRAP_LOCN + FLASH_PAGE_SIZE)  // one page for boot manager
#define FW_1_LOCN       (BANK_1_LOCN    + FLASH_PAGE_SIZE) 

typedef void (*pFunction)(void);
typedef unsigned long  u32;
typedef   signed long  s32;
typedef unsigned short u16;
typedef unsigned char  u8;
typedef _Bool          bool_t;

#define RAM_START           0x20000000u
#define RAM_END             0x20010000u
#define FW_LOCN_DIFF        (FW_0_LOCN ^ FW_1_LOCN)
#define OPTION_BYTES_LOCN   0x1ffff800u
#define BOOT_BYTE_LOCN      (OPTION_BYTES_LOCN + 4u)    // offset 4 in option bytes
#define BOOT_FLAG           (1u << 7)                   // use reserved bit for flag

#define BFB2        ((u16)0x7f)          // MSB clear is signal to boot from bank 2

// Flash Control Register values
#define CR_PG_Set               (1u << 0)
#define CR_OPTER_Set            (1u << 5)
#define CR_OPTWRE_SET           (1u << 9)
#define CR_STRT_Set             (1u << 6)
#define CR_OPTPG_Set            (1u << 4)

#define IO_REG32(x)  volatile u32 x

// first bank controller - partial occurence for bank 1 at offset 0x40
typedef struct
{                       // bank 1 regs
    IO_REG32(ACR);
    IO_REG32(KEYR);     // KEYR
    IO_REG32(OPTKEYR);
    IO_REG32(SR);       // SR2
    IO_REG32(CR);       // CR2
    IO_REG32(AR);       // AR2
} FLASHB_Typedef;

#define BFB1        ((u16)0xff)          // MSB set   is signal to boot from bank 1

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
#define FLASH_KEY1              ((u32)0x45670123u)
#define FLASH_KEY2              ((u32)0xCDEF89ABu)
#define true (bool_t)1

#define PERIPH_BASE             0x40000000u
#define AHBPERIPH_BASE          (PERIPH_BASE + 0x20000u)
#define FLASH_R_BASE            (AHBPERIPH_BASE + 0x2000u) /*!< Flash registers base address */

#define BSY                     (1u << 0)
#define WRPRTERR                (1u << 4)
#define PGERR                   (1u << 2)
#define ProgramTimeout          ((u32)0x00002000u)
#define EraseTimeout            ((u32)0x000B0000u)
#define FLASHB1                 ((FLASHB_Typedef *)FLASH_R_BASE)

static __root u32                      SR_FlashResult;         // status register (SR) from most recent operation

typedef struct vector_t
{
    u32         stackAdr;   // initial stack pointer
    pFunction   go;         // entry address
} vector_t;

/** \brief load the passed value into the Main Stack Pointer register
    
    \param topOfMainStack - 32-bit value to be loaded
*/
static void __set_MSP(u32 topOfMainStack)
{
    __asm("msr msp, r0");
}

/** \brief See if this partition is bootable.  If so, load the stack pointer
    and jump

    \param fw - pointer to a partition vector table 
*/
static void BootStrategy(vector_t *fw)      
{
    // current strategy only looks for valid stack pointer
    if ((fw->stackAdr >= RAM_START) && (fw->stackAdr <= RAM_END))
    {
        __set_MSP(fw->stackAdr);
        fw->go();
    }
}

/** \brief Try to find a suitable firmware partition to boot
*/
static void TryToBoot(void)
{
    u8  bf   = *(u8*)BOOT_BYTE_LOCN;    // fetch option byte
    u32 bla  = FW_0_LOCN;               // assume partition 0

    if ((bf & BOOT_FLAG) == 0)          // if flag is reset ...
    {
        bla ^= FW_LOCN_DIFF;            //     try partition 1 first
    }
    BootStrategy((vector_t*)bla);                   // won't come back if acceptable partition
    BootStrategy((vector_t*)(bla ^ FW_LOCN_DIFF));  // won't come back if acceptable partition

    for (;;)                            // no suitable firmware found
    {
        // can't boot
    }    
}

/** \brief Write the two key words to the specified key register

    \param - key - pointer to the register
*/
static void sendkey(volatile u32 *key)
{
    *key = FLASH_KEY1;
    *key = FLASH_KEY2;
}

/** \brief Wait for timout or !flashBusy. Store result in SR_FlashResult

     param - u32 - Timeout - Actually, number of retries.
    \return true if no errors; else false
*/
static bool_t FLASH_Wait(u32 Timeout)
{
    u32 rslt = FLASHB1->SR & BSY;

    while ((rslt == BSY) && (Timeout != 0u))
    {
        Timeout--;
        rslt = FLASHB1->SR & BSY;
    }
    SR_FlashResult = rslt;
    return (rslt & (BSY | WRPRTERR | PGERR)) == 0u;
}

/** \brief Update the 16 options bytes.  Must have been erased
    before write!
*/
static bool_t WriteOptionsBytes(u16 *adr, const u16 *data, s32 len)
{
    bool_t status = true;

    while ((len > 0) && status)
    {
        FLASHB1->CR |= CR_OPTPG_Set;
        *adr++       = *data++;
        status       = FLASH_Wait(ProgramTimeout);
        FLASHB1->CR &= ~CR_OPTPG_Set;
        len         -= (s32)sizeof(u16);
    }
    return status;
}

/** \brief Erase the option bytes
*/
static bool_t EraseOptionsBytes(void)
{
    bool_t status;

    sendkey(&FLASHB1->KEYR);        // unlock the controller
    sendkey(&FLASHB1->OPTKEYR);     // unlocj the options bytes

    FLASHB1->CR |= CR_OPTER_Set;    // 
    FLASHB1->CR |= CR_STRT_Set;     //
    status       = FLASH_Wait(EraseTimeout);
    FLASHB1->CR &= ~CR_OPTER_Set;
    return status;
}

static void wcopy (u32 *d, u32 *s, u32 len)
{
    while (len--)
        *d++ = *s++;
} 

/** \brief Set the preferred boot partition Bank1 or Bank2

    \param BootFromBank2 - true sets preferred Bank2; false sets preferred Bank1
    \return true if no errors; othrwise false
*/
bool_t SetPartition(u8 userval, u8 dataval)
{
    OB_TypeDef lob;
    
    // fetch the current options bytes
    wcopy((u32*)&lob, (u32*)OB_BASE, sizeof(lob) / sizeof(u32));

#if TESTING
    if (((lob.USER & 0xff) == userval) && ((lob.Data0 & 0xff) == dataval))
    {
        return true;
    }
#endif
    bool_t status  = EraseOptionsBytes();        
    if (status)
    {
        lob.USER  = userval;                   // no user options active (BFB1)
        lob.Data0 = dataval;
        status    = WriteOptionsBytes((u16*)OB_BASE, (u16*)&lob, sizeof(lob) / sizeof(u16));
    }
    FLASHB1->CR  &= ~CR_OPTWRE_SET;
    return status;
}

/** \brief The C entry point to the partition manager
*/
void main (void) 
{
    // if running from Bank2 somehow the STMicro BFB2 state was set 
    if ((u32)main >= BANK_1_LOCN) 
    {   
        SetPartition(0xff, *(u8*)BOOT_BYTE_LOCN | 0x7f);   // clear STM BFB2
        (void)SR_FlashResult;
    }
#if TESTING
    else
    {   
        SetPartition(0xf7, 0xff);   // logically set STM BFB2
    }
#endif
    TryToBoot();
}

extern void __iar_program_start(void);

typedef            void     (*CPU_FNCT_VOID)(void);             /* See Note #2a.                                        */

typedef struct vectors_t
{
    void           *stackPtr;
    CPU_FNCT_VOID   Fnct[15];   
} vectors_t;


typedef void (*vd_t) (void);

/** \brief Trap handler for Cortex vectors

    \param aa - dummy parameter so debugger will analyze the stack
*/
static void Cortex_Trap(void)
{
    for (;;)
    {
    }
}

static const __root  u32 dummy @ "checksum" = 0;        // a dummy so the CRC won't clobber any code

#define TRAP1   (vd_t)Cortex_Trap
#pragma segment="CSTACK"

const  vectors_t __vector_table @ ".intvec" = 
{
    __sfe("CSTACK"),                                   //  0, Initial Stack Pointer
    {
        __iar_program_start,                            //  1, PC start value.                                  
        TRAP1,                                          //  2, NMI.                                             
        TRAP1,                                          //  3, Hard Fault.                                      
        TRAP1,                                          //  4, Memory Management.                               
        TRAP1,                                          //  5, Bus Fault.                                       
        TRAP1,                                          //  6, Usage Fault.                                     
        (vd_t)0xdeadbeef,                               //  7, partition manager signature                                        
        (vd_t)FLASH_PAGE_SIZE,                          //  8, offset to firmware
        TRAP1,                                          //  9, Reserved.                                        
        TRAP1,                                          // 10, Reserved.                                        
        TRAP1,                                          // 11, SVCall.                                          
        TRAP1,                                          // 12, Debug Monitor.                                   
        TRAP1,                                          // 13, Reserved.                                        
        TRAP1,                                          // 14, PendSV Handler.
        TRAP1                                           // 15, uC/OS-II Tick ISR Handler.                       
    }
};

// end of source

// end of source
