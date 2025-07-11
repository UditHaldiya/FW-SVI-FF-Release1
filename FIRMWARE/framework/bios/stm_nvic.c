/**
Copyright 2011 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file stm_nvic.c
    \brief Handler for the STM32F10xx NVIC including  the System
    Tick timer and

    OWNER:
    CPU: STM32F10xx Cortex

    $Revision:  $
*/
#include "mnwrap.h"
#include "oswrap.h"
#include "stmcommon.h"
#include "timer.h"
#include "bsp.h"

//lint ++flb
typedef struct NVIC_Type
{
    IO_REG32(ISER[8]);                  /*!< Offset: 0x000 (R/W)  Interrupt Set Enable Register           */
    IO_REG32(RESERVED0[24]);
    IO_REG32(ICER[8]);                  /*!< Offset: 0x080 (R/W)  Interrupt Clear Enable Register         */
    IO_REG32(RSERVED1[24]);
    IO_REG32(ISPR[8]);                  /*!< Offset: 0x100 (R/W)  Interrupt Set Pending Register          */
    IO_REG32(RESERVED2[24]);
    IO_REG32(ICPR[8]);                  /*!< Offset: 0x180 (R/W)  Interrupt Clear Pending Register        */
    IO_REG32(RESERVED3[24]);
    IO_REG32(IABR[8]);                  /*!< Offset: 0x200 (R/W)  Interrupt Active bit Register           */
    IO_REG32(RESERVED4[56]);
    u8      IP[240];                    /*!< Offset: 0x300 (R/W)  Interrupt Priority Register (8Bit wide) */
    IO_REG32(RESERVED5[644]);
    IO_REG32(STIR);                     /*!< Offset: 0xE00 ( /W)  Software Trigger Interrupt Register     */
}  NVIC_Type;
//lint --flb

#define STANDARD_IRQ_PRIORITY           (15u)

#define NVIC                HARDWARE(NVIC_Type *, NVIC_BASE)        /*!< NVIC configuration struct         */

#define NVIC_BASE           (SCS_BASE +  0x0100UL)                    /*!< NVIC Base Address                 */
#define __NVIC_PRIO_BITS    4                                         /*!< STM32 uses 4 Bits for the Priority Levels    */

#define SysTick_IRQn -1

static void NVIC_SetPriority(IRQn_Type IRQn, u32 priority)
{
    u8  val = ((priority << (8u - (u32)__NVIC_PRIO_BITS)) & 0xffu);
    u32 irq = (u32)IRQn;    //lint !e571

    if(IRQn < (IRQn_Type)0)
    {
        SCB->SHP[(irq & 0xFu) - 4u] = val;
    }
    else
    {
        NVIC->IP[irq] = val;
    }
}

void SysTick_Config(u32 ticks)
{
    if (ticks > (u32)SysTick_LOAD_RELOAD_Msk)                        /* Reload value impossible */
    {
        return;
    }

    SysTick->LOAD  = (ticks & (u32)SysTick_LOAD_RELOAD_Msk) - 1u;     /* set reload register */
    // NVIC_SetPriority (SysTick_IRQn, (1u <<__NVIC_PRIO_BITS) - 2u); /* set Priority for Cortex-M0 System Interrupts */
    NVIC_SetPriority (SysTick_IRQn, STANDARD_IRQ_PRIORITY);     /* set Priority for Cortex-M0 System Interrupts */
    SysTick->VAL   = 0u;                                         /* Load the SysTick Counter Value */
    SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                     SysTick_CTRL_ENABLE_Msk;                   /* Enable SysTick IRQ and SysTick Timer */
}

void bsp_StartTicks(void)
{
    SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                     SysTick_CTRL_TICKINT_Msk   |
                     SysTick_CTRL_ENABLE_Msk;                   /* Enable SysTick IRQ and SysTick Timer */
}

//-------------------------------------------------------------
// Note:
//  SysTick->VAL    - DECREMENTING(!) 24 bit counter
//  SysTick->LOAD   - Reload value.
//
// The SysTick timer starts from the reload value, counts to ZERO,
// THEN reloads the reload value.
// It also generates an interrupt.
void systickdly_with_start(u32 StartTime, u32 clocks)
{
    u32 CurrentTime;            // Curent Ticks from SysTickTimer
    u32 OldTime;                // Previous Ticks from SysTickTimer
    u32 ElapsedTime;            // Total Elapsed ticks from the start time
    u32 MAXSysTickValue;        // MAX SysTick Timer value

    OldTime     = StartTime;    // Start from somewhere
    ElapsedTime = 0u;           // Sum of elapsed time from the StartTime

    // Read the reload value from SysTick Timer - do not calculate!
    // Calculations can be incorrect, the H/W is not.
    MAXSysTickValue = SysTick->LOAD;

    // Delay loop...
    do
    {
        // Get Current time from SysTick Timer
        CurrentTime = SysTick->VAL;

        if (CurrentTime <= OldTime)
        {   // Counter either did not change (==)
            // Or, changed (<), but did not wrap.
            // Thus, NEW time is LOWER than the OLD one.
            // Add the difference to the Elapsed time.
            ElapsedTime += OldTime - CurrentTime;
        }
        else
        {   // The counter WRAPPED
            // Using MAX reload time to find out how much time we ran from
            // the wrap then add Old time to zero. And one to wrap.
            // Add this to the Elapsed time
            //
            // This Debug Assert should not occur, it is an indicator of the 5ms
            // timer wrap or a significant delay from the SysTick timer interrupt.
            MN_DBG_ASSERT(CurrentTime > OldTime);
            ElapsedTime += (MAXSysTickValue - CurrentTime) + OldTime + 1u;
        }

        // Remember the time for the next cycle.
        OldTime = CurrentTime;

    // Repeat the DelayLoop as needed
    } while (ElapsedTime < clocks);
}

void systickdly(u32 clocks)
{
#if 0

    u32 endtime, startime = SysTick->VAL;

    do
    {
        endtime = SysTick->VAL;
        if(endtime <= startime)
        {
            //This means timer 0 was reset, adjust endtime by the reset period
            endtime += (CORE_FREQ / TICKS_PER_SEC);
        }
    } while((startime + clocks) > endtime);

#else

    u32 startime = SysTick->VAL;

    systickdly_with_start(startime, clocks);

#endif
}

u32 timer_EstimateFunctionLength(void (*func)(void))
{
    u32   startime = 0xffffffu;         // 8.38 seconds @ 2 Mhz
    u32   TempReload;

    TempReload      = SysTick->LOAD;    // Store he reload value

    SysTick->LOAD   = startime;         // set reload register
    SysTick->VAL    = startime;         // Load the SysTick Counter Value
    func();                             // call routine
    startime       -= SysTick->VAL;     // elapsed time
    SysTick->LOAD   = TempReload;       // Restore reload register
    SysTick->VAL    = 0u;               // Load the SysTick Counter Value
    return startime;
}




/** \brief cause a software reset (causes a 20 microsecond low on the NRST pin)
    Is supposed to reset both CPU's
*/
void scb_DoHardReset(void)
{
    u32 aircr = SCB->AIRCR & 0xffffu;

    SCB->AIRCR = aircr | (0x5fau << 16) | SCB_AIRCR_SYSRESETREQ;
}

void NVIC_SetVectors(void)
{
    SCB->VTOR = __vector_table;
}

void NVIC_EnableIRQ(IRQn_Type IRQn)
{
    NVIC_SetPriority(IRQn, STANDARD_IRQ_PRIORITY);
    NVIC->ISER[((u32)(IRQn) >> 5)] = (1u << ((u32)(IRQn) & 0x1Fu)); /* enable interrupt */
}

/*----------------------------------------------------------------------------
  Systick Interrupt Handler
  SysTick interrupt happens every 5 ms
 *----------------------------------------------------------------------------*/

MN_IRQ void SysTick_Handler (void)            /* SysTick Interrupt Handler (5ms)   */
{
    //In bios_ProcessTick: maintain_timers ();             // HART timers
    bios_ProcessTick ();      // do 5 millisecond things
}

// end of source
