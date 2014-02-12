/*
Copyright 2006 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/** \addtogroup BSP  Board Support Package
\brief Processor init, VIC and Timer setup
\htmlonly
<a href="../../../Doc/DesignDocs/Physical Design_Board_Support.doc"> Design document </a><br>
<a href="../../../Doc/TestDocs/utplan_BIOS.doc"> Unit Test Report </a><br>
\endhtmlonly
*/
/**
    \file bsp.c
    \brief Board Support Package, processor init, VIC and Timer setup

    CPU: Philips LPC21x4 (ARM)

    OWNER: Ernie Price

    $Archive: /MNCB/Dev/FIRMWARE/framework/bios/bsp.c $
    $Date: 7/28/11 11:57p $
    $Revision: 55 $
    $Author: Arkkhasin $

    \ingroup BSP
*/
/* $History: bsp.c $
 *
 * *****************  Version 55  *****************
 * User: Arkkhasin    Date: 7/28/11    Time: 11:57p
 * Updated in $/MNCB/Dev/FIRMWARE/framework/bios
 * Back to explicit "timebase.h"
 *
 * *****************  Version 54  *****************
 * User: Arkkhasin    Date: 7/28/08    Time: 1:38p
 * Updated in $/MNCB/Dev/FIRMWARE/framework/bios
 * Light linting
 *
 * *****************  Version 53  *****************
 * User: Arkkhasin    Date: 7/25/08    Time: 7:38p
 * Updated in $/MNCB/Dev/FIRMWARE/framework/bios
 * Extracted C environment init to support EWARM 5.20
*/
#include "mnwrap.h"
#include "bsp.h"
#include "timebase.h"
#include "serial.h"
#include "lpc2114io.h"

#define INTERNAL_WD_RESET OPTION_ON

/**
  \brief  Dummy interrupt handler, called as default in irqHandler() if no other
  vectored interrupt is called.
*/
static MN_IRQ void DefDummyInterrupt(void)
{
    VICVectAddr = 0;                // clear VIC interrupt
}

/**
  \brief  Interrupt controller (VIC) initalization.
*/
static void bsp_InitVIC(void)
{
    // Setup interrupt controller.
    VICProtection  = 0u;
    // Disable all interrupts
    VICIntEnClear  = ~0u;
    VICDefVectAddr = DefDummyInterrupt;
    VICVectAddr    = 0u;        // Clear interrupt in VIC.
}

/**
  \brief  Register a vectored interrupt in the VIC

  \param[in] prio - The priority of the device interrupt
  \param[in] dev - the device number e.g. VIC_TIMER0.  See manual
  \param[in] func - a pointer to the interrupt handler function
*/
static void bsp_InitInterrupt(u32 prio, u32 dev, irqf_t *func)
{
    VICIntSelect           &= ~(1u << dev);     // IRQ (as opposed to FIQ)
    VIC->ViCVectAddrX[prio] = func;             // store vector in VIC
    VIC->ViCVectCntlX[prio] = dev |             // Enable vector interrupt for device
        VIC_VECT_SLOT_ENABLE;
    VICIntEnable            = 1u << dev;        // Enable device interrupt.
}

#if 0
/**
  \brief  Register the SPIO device interrupt

  \param[in] spi_func - a pointer to the SPI interrupt handler
*/
void bsp_InitSPIInterrupt(irqf_t *spi_func)
{
    bsp_InitInterrupt(VIC_IRQ_PRIO_2, VIC_SPI + 0, spi_func);
}
#endif

#if (USE_INTERNAL_WATCHDOG == OPTION_ON) && (INTERNAL_WD_RESET == OPTION_OFF)

/**
  \brief  Register the internal watchdog device interrupt

  \param[in] wd_func - a pointer to the watchdog interrupt handler
*/
void bsp_InitWatchdogInterrupt(irqf_t *wd_func)
{
    bsp_InitInterrupt(VIC_IRQ_PRIO_3, VIC_WDT, wd_func);
}
#endif

/**
  \brief  Register the Timer0 device interrupt

  \param[in] timer_func - a pointer to the timer 0 interrupt handler
*/
static void bsp_InitTimerInterrupt(irqf_t *timer_func)
{
    bsp_InitInterrupt(VIC_IRQ_PRIO_1, VIC_TIMER0, timer_func);
}

/**
  \brief  Register the UART 0 (serial) device interrut

  \param[in] serial_func - a pointer to the UART 0 interrupt handler
*/
static void bsp_InitSerialInterrupt(irqf_t *serial_func)
{
    bsp_InitInterrupt(VIC_IRQ_PRIO_4, VIC_UART0, serial_func);
}

/**
  \brief  Initialize the Timer1 device as a free-running timer
        Only used for debugging as normally the Timer 1 device is used for
        digital output control. N.B. Timer1 is reinitialized later and may
        only be used for tick counting during startup.
*/
static void bsp_InitTimer1(void)
{
    T1->TCR   = 0;              // Disable timer 0.
    T1->PC    = 0;              // Prescaler is set to no division.
    T1->MCR   = 0;              // Reset and interrupt on MR0 (match register 0).
    T1->CCR   = 0;              // Capture is disabled.
    T1->TCR   = 1;              // Enable timer 0.
}

/**
  \brief  Initialize the Timer0 device to interrupt at the 5 ms rate
    Timer interrupts are not started here - we wait until all of the tasks
    are intialized and running normally.
*/
static void bsp_InitTimer0(void)
{
    T0->TCR    = 0;             // Disable timer 0.
    T0->TC     = 0;
    T0->PC     = 0;             // Prescaler is set to no division.
    T0->MR0    = (PCLKFREQ /
        LCL_TICKS_PER_SEC) - 1u;// rate
    T0->MCR    = 3;             // Reset and interrupt on MR0 (match register 0).
    T0->CCR    = 0;             // Capture is disabled.
}

/** \brief Computes the raw value of timer 1 ticks spent in a function.

    IMPORTANT: Can only be called during initialization
    \param func - function to estimate
    \return number of ticks
*/
u32 bsp_EstimateTime(void (*func)(void))
{
    u32 teststart = T1->TC;
    func();
    return T1->TC - teststart;
}

/**
  \brief  Start the Timer0 interrupt at the 5 ms rate
*/
void bsp_StartTicks(void)
{
    T0->TC  = 0;                // in case was left at some large value
    T0->TCR = 1;                // Enable timer 0.
}

/**
  \brief  Intialize the various hardware devices
*/
void bsp_Init(void)
{
    // Setup interrupt controller.
    bsp_InitVIC();

    bsp_InitTimerInterrupt(bsp_ServiceTimer);
    bsp_InitTimer0();
    bsp_InitTimer1();       // Note: only temporary,  Timer1 is reinitialized later
    bsp_InitSerialInterrupt(serial_int);
}

/* This line marks the end of the source */
