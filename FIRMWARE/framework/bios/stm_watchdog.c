/**
Copyright 2011 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file stm_watchdog.c

    \brief Driver for the internal watchdogs of the STM32F10xx CPU

    OWNER:
    CPU: STM32F10xx Cortex

    $Revision:  $
*/

#include "mnwrap.h"
#include "stmcommon.h"
#include "watchdog.h"
#include "gpio.h"
#include "process.h"
#include "crc.h"
#include "mnassert.h"
#include "pwm.h" //for inplace reset

#define WATCHDOG_NORMAL         (1u)
#define WATCHDOG_2S             (2u)

#define WATCHDOG_BUILD          (WATCHDOG_NORMAL)
// #define WATCHDOG_BUILD          (WATCHDOG_2S)

#if ((WATCHDOG_BUILD != WATCHDOG_NORMAL) && (WATCHDOG_BUILD != WATCHDOG_2S))
     #error IWD configuration incorrect!
#endif

//------------------------------------------------------------------------------------
// Window Watchdog Registers and masks and other definitions

//lint ++flb

#define  WWDG_CR_T                           ((uint8_t)0x7F)               /*!<T[6:0] bits (7-Bit counter (MSB to LSB)) */
#define  WWDG_CR_T0                          ((uint8_t)0x01)               /*!<Bit 0 */
#define  WWDG_CR_T1                          ((uint8_t)0x02)               /*!<Bit 1 */
#define  WWDG_CR_T2                          ((uint8_t)0x04)               /*!<Bit 2 */
#define  WWDG_CR_T3                          ((uint8_t)0x08)               /*!<Bit 3 */
#define  WWDG_CR_T4                          ((uint8_t)0x10)               /*!<Bit 4 */
#define  WWDG_CR_T5                          ((uint8_t)0x20)               /*!<Bit 5 */
#define  WWDG_CR_T6                          ((uint8_t)0x40)               /*!<Bit 6 */

#define  WWDG_CR_WDGA                        ((uint8_t)0x80)               /*!<Activation bit */

typedef struct
{
    IO_REG32(CR);
    IO_REG32(CFR);
    IO_REG32(SR);
} WWDG_TypeDef;

#define WWDG_BASE                           (APB1PERIPH_BASE + 0x2C00u)
#define WWDG                                HARDWARE(WWDG_TypeDef *, WWDG_BASE)

//------------------------------------------------------------------------------------
// Independent Watchdog Registers and masks and other definitions

typedef struct
{
    IO_REG32(KR);                           // Key register -- Start, ACcess and Tickle IWDG via the key
    IO_REG32(PR);                           // Prescaler register
    IO_REG32(RLR);                          // Reload register -- contains teh value to be reloaded
    IO_REG32(SR);                           // Status Register
} IWDG_TypeDef;

#define IWDG_BASE                           (APB1PERIPH_BASE + 0x3000u)
#define IWDG                                HARDWARE(IWDG_TypeDef *, IWDG_BASE)

//------------------------------------------------------------------------------------
// Special values
// Pre-defined in the uC specification.
#define IWDG_START_KEY                      (0xCCCCu)
#define IWDG_TICKLE_KEY                     (0xAAAAu)
#define IWDG_ACCESS_UNLOCK_KEY              (0x5555u)

//------------------------------------------------------------------------------------
// Prescaler values
// LO 3 bits in prescaler register
#define IWDG_PRESCALER_DIV_BY_4             (0u)
#define IWDG_PRESCALER_DIV_BY_8             (1u)
#define IWDG_PRESCALER_DIV_BY_16            (2u)
#define IWDG_PRESCALER_DIV_BY_32            (3u)
#define IWDG_PRESCALER_DIV_BY_64            (4u)
#define IWDG_PRESCALER_DIV_BY_128           (5u)
#define IWDG_PRESCALER_DIV_BY_256           (6u)
#define IWDG_PRESCALER_DIV_BY_256a          (7u)

//lint --flb

//------------------------------------------------------------------------------------
// Pre-scaler configuration

#if (WATCHDOG_BUILD == WATCHDOG_NORMAL)
    #define IWDG_PRESCALER_CONFIG               (IWDG_PRESCALER_DIV_BY_4)
#else
    // 2sec IWD
    #define IWDG_PRESCALER_CONFIG               (IWDG_PRESCALER_DIV_BY_64)

    // 1250 counts * 1.6ms = 2000ms = 2s
    // And add one more cycle
    #define IWDG_2S_LOADVALUE                   (1250u + 1u)
#endif

// Find the Prescaler divider value
#if (IWDG_PRESCALER_CONFIG >= IWDG_PRESCALER_DIV_BY_256a)
    #define IWDG_PRESCALER_SHIFT            (IWDG_PRESCALER_DIV_BY_256 + 2u)
#else
    #define IWDG_PRESCALER_SHIFT            (IWDG_PRESCALER_CONFIG + 2u)
#endif

#define IWDG_PRESCALER_DIVIDER              (1u << IWDG_PRESCALER_SHIFT)

// Validate the Prescaler divider value
CONST_ASSERT((IWDG_PRESCALER_DIVIDER >= 4u) && (IWDG_PRESCALER_DIVIDER <= 256u));

//------------------------------------------------------------------------------------
// Reload register definitions
#define IWDG_MAX_COUNT                      (4095u)         // max reload register value in counts

//------------------------------------------------------------------------------------
// Status Register definitions
// Bit 0 PVU:   Watchdog prescaler value update
//              This bit is set by hardware to indicate that an update of the prescaler value is ongoing. It is
//              reset by hardware when the prescaler update operation is completed in the VDD voltage
//              domain (takes up to 5 RC 40 kHz cycles).
//              Prescaler value can be updated only when PVU bit is reset.
//
// Bit 1 RVU:   Watchdog counter reload value update
//              This bit is set by hardware to indicate that an update of the reload value is ongoing. It is reset
//              by hardware when the reload value update operation is completed in the VDD voltage domain
//              (takes up to 5 RC 40 kHz cycles).
//              Reload value can be updated only when RVU bit is reset.

#define IWDG_STATUS_PVU                     (0x1u)
#define IWDG_STATUS_RVU                     (0x2u)

//------------------------------------------------------------------------------------
// External WD Timeout = 267ms
//
// Note: the IWDG is clocked by internal 40kHz clock that actually could vary from 30kHz to 60kHz.
// The reload value and prescaler are chosen to have the IWDG timeout over 150ms but under
// External WD timeout (267ms).
//
// Internal WD is set to shorter time =
//
// WD Times         MIN timeout     MAX timeout
// Prescaler             ms             ms
//    /4                0.1            409.6
//    /8                0.2            819.2
//   /16                0.4           1638.4
//   /32                0.8           3276.8
//   /64                1.6           6553.6
//  /128                3.2          13107.2
//  /256                6.4          26214.4

//------------------------------------------------------------------------------------
// Frequency parameters for WD calculations
// The nominal frequency for the IWDG is specified as 40kHz; but the specification
// indicates that the frequency may vary from 30kHz to 60kHz.
// Defining MAX frequency here:

#define LSI_MAXIMUM_FREQUENCY               (60000u)

//------------------------------------------------------------------------------------
// This is the MIN IWDG timeout time. Set in MILLISECONDS, shall be lower than the
// T/O of the External WD, whioch is defined by H/W and is defined here in MILLISECONDS.

#define IWDG_TIMEOUT_mSEC                   (160u)

// This is the MAX IWDG timeout time - defined by the External WD T/O
// The value derived form H/W Development Team

#define EXT_WD_TIMEOUT_mSEC                 (267u)

// Validate the Timeout!
CONST_ASSERT(IWDG_TIMEOUT_mSEC < EXT_WD_TIMEOUT_mSEC);

//------------------------------------------------------------------------------------
// Constants and definitions
#define IWDG_SHIFT                      (17u)                                   // could be 18 but 17 rounds better

#define IWDG_MIN_INT                    ((IWDG_MAX_COUNT * IWDG_PRESCALER_DIVIDER) / (LSI_MAXIMUM_FREQUENCY / 1000u))
#define IWDG_FACTOR                     (((IWDG_TIMEOUT_mSEC * (1u << IWDG_SHIFT)) + 500u) / 1000u)
#define IWDG_FRNG                       (0xffffffff / LSI_MAXIMUM_FREQUENCY)

// if this fails we need a smaller shift factor (IWD_SHIFT)
CONST_ASSERT(IWDG_FACTOR < IWDG_FRNG);
// if this fails we need a larger prescale value (IWD_PS_VAL)
CONST_ASSERT(IWDG_TIMEOUT_mSEC < IWDG_MIN_INT);

//------------------------------------------------------------------------------------
// Calculating special RLR value and Divider
#define IWDG_LONG_PRESCALER_CONFIG          (IWDG_PRESCALER_DIV_BY_32)
#define IWDG_LONG_PRESCALER_SHIFT           (IWDG_LONG_PRESCALER_CONFIG + 2u)
#define IDWG_LONG_TIMEOUT_mSEC              (701u)
#define IWDG_LONG_FACTOR                    (((IDWG_LONG_TIMEOUT_mSEC * (1u << IWDG_SHIFT)) + 500u) / 1000u)

//------------------------------------------------------------------------------------
typedef struct IWDG_MeasuredFrequency_t
{
    u32     MeasuredFrequency;
    u16     CheckWord;

} IWDG_MeasuredFrequency_t;

static IWDG_MeasuredFrequency_t IWDG_MeasuredFrequency =
{
    .MeasuredFrequency = 0u,
    .CheckWord = 0u
};

//------------------------------------------------------------------------------------

/** \brief Update regs in the internal WD
*/
static void wdog_UpdateRegs(u32  NewRLRVal,
                            u32  NewPRVal)
{
    MN_ENTER_CRITICAL();
        IWDG->KR = IWDG_ACCESS_UNLOCK_KEY;                  // Unlock the registers

        do
        {
            ;
        } while ((IWDG->SR & IWDG_STATUS_PVU) != 0u);       // Wait for teh end of the update
        IWDG->PR = NewPRVal;                                // Write the value

        do
        {
            ;
        } while ((IWDG->SR & IWDG_STATUS_RVU) != 0u);       // Wait for teh end of the update
        IWDG->RLR = NewRLRVal;

        IWDG->KR = IWDG_TICKLE_KEY;                         // Lock the registers
    MN_EXIT_CRITICAL();
}

/** \brief Start the Internal watchdog to cause a reset.
*/
void wdog_WatchdogInit(void)
{
#if USE_INTERNAL_WATCHDOG == OPTION_ON
    u32 wdCount;

    if (rcc_WDTimeout())
    {
        process_TrapWatchdog();
    }

    // Store the measured Frequency in the protected structuure
    IWDG_MeasuredFrequency.MeasuredFrequency = tmr_GetLSIFreq();
    STRUCT_CLOSE(IWDG_MeasuredFrequency_t, &IWDG_MeasuredFrequency);
    MN_ASSERT(IWDG_MeasuredFrequency.MeasuredFrequency != 0u);

    // Caclculate the proper LSI timing
    wdCount = (IWDG_MeasuredFrequency.MeasuredFrequency * IWDG_FACTOR) >> (IWDG_SHIFT + IWDG_PRESCALER_SHIFT);

#if (WATCHDOG_BUILD == WATCHDOG_2S)
    // Override it with 2s delay
    wdCount = IWDG_2S_LOADVALUE;
#endif

    MN_DBG_ASSERT(wdCount <= IWDG_MAX_COUNT);

    rcc_DisableTimer5();

    MN_ENTER_CRITICAL();
        IWDG->KR  = IWDG_ACCESS_UNLOCK_KEY;                 // Unlock the registers
        IWDG->PR  = IWDG_PRESCALER_CONFIG;                  // Set prescaler
        IWDG->RLR = wdCount;                                // Set the constant
        IWDG->KR  = IWDG_TICKLE_KEY;                        // Lock the registers

        IWDG->KR  = IWDG_START_KEY;
    MN_EXIT_CRITICAL();
#endif
}

static void wdog_TickleInternalWD(void)
{
#if USE_INTERNAL_WATCHDOG == OPTION_ON
    IWDG->KR = IWDG_TICKLE_KEY;                             // Tickle me elmo...
#endif
}

/** \brief Extend the time for the internal watchdog and wait for the
    external watchdog to timeout.
*/
void wdog_WaitExternalWatchdog(void)
{
    u32 wdCount;

    wdog_TickleInternalWD();
    mn_disable();

    Struct_Test(IWDG_MeasuredFrequency_t, &IWDG_MeasuredFrequency);
    wdCount = (IWDG_MeasuredFrequency.MeasuredFrequency * IWDG_LONG_FACTOR) >> (IWDG_SHIFT + IWDG_LONG_PRESCALER_SHIFT);
    MN_DBG_ASSERT(wdCount <= IWDG_MAX_COUNT);

    wdog_UpdateRegs(wdCount, IWDG_LONG_PRESCALER_CONFIG);     // Set LOOOOOONG T/O in IWDG

    for (;;)
    {
        ;
    }
}


/** \brief Tickle the MAX6301 watchdog timer.
*/
void wdog_WatchdogTrigger(void)
{
    // we really only have to send one edge to the MAX6301.  Here we send two
    gpio_SetWatchDogHI();
    gpio_SetWatchDogLO();

    // And also tickle the IWDG
    wdog_TickleInternalWD();
}

/** \brief cause a software reset - jumps to base of flash.  See
    assembly language routine jmpstart
*/
#define RESET_DLY (u32)((cdouble_t)CORE_FREQ * INPLACE_RESET_LENGTH)    // 10 milliseconds
#define FRAC ((CORE_FREQ / TICKS_PER_SEC) / 2u)         // half the timer interval
void wdog_HardwareReset(void)
{
    u32 tot = RESET_DLY;

    MN_ENTER_CRITICAL();
        // Tickle the dogs so they won't bark when the master goes to the bathrooom
        wdog_WatchdogTrigger();

        // 10ms delay
        while (tot != 0u)               // break it into pieces
        {
            u32 dly = MIN(FRAC, tot);   // no piece larger than .5 timer interval.

            systickdly(dly);
            tot -= dly;
        }

        // Restart!
        // jmpstart(flash_GetBaseFor_THIS_bank());

        // This does not work as we have actibve PendSV. Then on jump it will be active, and will not be de-activated and the
        // uC will not be taking interrupts.
        // It looks like setting VECTCLRACTIVE in SCB IARCR MIGHT clear it, but the specification prohibits using this bit
        // except for debugging purposes. Thus, resorting to the HARD reset until further change.

        scb_DoHardReset();             // Execute HARD reset -- jumpstart was not sufficient for uC reset.

    MN_EXIT_CRITICAL();                 // never get here but the compiler and lint
}

// End of the source
