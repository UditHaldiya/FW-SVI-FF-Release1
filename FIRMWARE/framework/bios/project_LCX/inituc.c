/**
Copyright 2004-2005 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file inituc.c
    \brief BIOS routines for initializing the microprocessor

    CPU: Philips LPC2114

    OWNER: EP
    \n RCS: $Id$
    Created: 9/27/04 Author:  EP
*/

/* (Optional) $History: inituc.c $
 * 
 * *****************  Version 31  *****************
 * User: Justin Shriver Date: 11/29/11   Time: 6:53p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/framework/bios/project_LCX
 * -- AP -- LINT change.
 * 
 * *****************  Version 29  *****************
 * User: Anatoly Podpaly Date: 10/12/10   Time: 2:23p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * Bug 4359 - Added definition for the Platform Hardware. Added
 * conditional compilation based on the type of the Platform H/W.
 * 
 * *****************  Version 28  *****************
 * User: Justin Shriver Date: 9/16/10    Time: 11:08a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * TFS:4121, TFS:4124
 * 
 * *****************  Version 27  *****************
 * User: Anatoly Podpaly Date: 9/09/10    Time: 12:01p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * Updates for actual H/W
 * 
 * *****************  Version 26  *****************
 * User: Anatoly Podpaly Date: 8/25/10    Time: 11:53a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * Added H/W GPIO to control PB / RS power.
 * 
 * *****************  Version 25  *****************
 * User: Anatoly Podpaly Date: 8/17/10    Time: 11:37a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * Bug 3839 - added function to get VPB divider.
 * 
 * *****************  Version 24  *****************
 * User: Anatoly Podpaly Date: 6/18/10    Time: 11:19a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * Conditional Compilation for the New Hardware.
 * 
 * *****************  Version 23  *****************
 * User: Anatoly Podpaly Date: 5/21/10    Time: 11:40a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * 3263 - fixed GPIO setting, that ended up being incorrect name. Moved
 * definitions to bios_def.h. Corrected header.
 * 
*/

#include "mnwrap.h"
#include "bios_def.h"
#include "lpc2114io.h"
#include "inituc.h"

/* generate a interrupt if Timer0 reaches Match0 value */
/* 5ms interrupt rate for operating system */
/* system clock and peripherial divider	see: bios_def.h */
/* 5ms/2/1.8432e6 = 4608  (-1 as we need one cycle to reset TC) */
//#define TIMER0_MATCH_VALUE_OS (u32)4607

/* Timer1 match output is toggled if Timer counter is equal to match register */
/* 115khz = 1.8432e6/(2*2*4)  2= VPD divider 2=2 clock phases 4=Match value   */
/* After match cycle TC is reset so we need 3+1 cycles for correct output freq. */
#define TIMER1_MATCH_VALUE  ((u32)3)

/* number of fetch cycles for reading FLASH memory (2 processor clocks per access) */
#define MAMTIM_INIT_VALUE ((u32)2)

/*
    \brief Set the peripheral clock divider

    \param bool_t divval - true = div2; else div1

*/
void bios_SetVPBDividier_to2(bool_t divval)
{
    VPBDIV = (true == divval)
        ? VPBDIV_CLOCKDIV2     /* VPB = processor clock / 2 */
        : VPBDIV_CLOCKDIV1;    /* VPB = processor clock */
}

/*
    \brief Get the peripheral clock divider

    \param void
    \returns bool_t divval - true = div2; else div1

*/
bool_t bios_GetVPBDividier_to2(void)
{
    bool_t  retval = true;  // Assume Normal speed

    if (VPBDIV == VPBDIV_CLOCKDIV1)
    {   // The Fast speed - retur false
        retval = false;
    }
    return retval;
}

/*
    \brief Set Timer1 to run at 115.2 khz. This is used for control of the
    digital outputs.

*/
void bios_InitTimer1(void)
{
  T1->TCR = 0;
  T1->PR = (u32)0 ;  /* Set prescaler to divide by 1 */
  T1->PC = (u32)0 ;  /* reset prescale counter */
  T1->TC = (u32)0 ;  /* reset timer counter */
  T1->MR0 = TIMER1_MATCH_VALUE; /* value to compare with Timer counter. D0_1 output freq. */
  T1->MR1 = TIMER1_MATCH_VALUE; /* value to compare with Timer counter. D0_2 output freq. */
  T1->MCR = (MCR_RESET_MR0);  /* reset counter on match channel0 */
  T1->EMR = 0 ; /* Do nothing on match outputs */
  T1->TCR = COUNTER_ENABLE;   /* Start Timer1 */
}

//#pragma location="ICODE"
/*
    \brief Set up the micro controller including the initial state of the GPIO
    pins.

    \param none
*/
void bios_InitMicrocontroller(void)
{
    /* Configuration of memory accelerator module */
    /* MAM Control register = 0 (reset value) MAM function disabled */
    /* MAM Timing register. Set wait cycles of FLASH to save power */
    /* First command in Init function to speed up execution */
    MAMTIM = MAMTIM_INIT_VALUE;

    /* External interrupt flag register =0 (reset value) No external interrupts are used */
    /* External interrupt wakeup register=0 (reset value) No wakeup is used */
    /* External interrupt mode register=0 (reset value) not used */
    /* External interrupt polarity register=0 (reset value) not used */
    /* PLL not used (reset values) */
    /* Power control register=0 (reset value) no power down modes are used */

    /* Power control of peripherals */
    /* unused peripherals are switched to off */
    PCONP &= ~(
        PCCURT1     | /* Uart1 off */
        PRESERVED11 | /* Bit11 should be 0 to save power */
        PCAD        | /* internal A/D off */
        PCRTC);       /* RTC off */

    /* Memory mapping control. select destination of interrupt vectors */
    MEMMAP = MEMMAP_USER_FLASH_MODE;

    /* set VPB to one half of processor clock to save power */
    VPBDIV = VPBDIV_CLOCKDIV2;

    /* Set pin selection of I/O port 0 */
    /* By default all pins are GPIO (SSEL pins are GPIO to reduce power consumption) */

    /* 12/8/09 ERP Power and SSEL state is not an issue with the /01 LPC21x4 */
    
    PINSEL0 = PINSEL0_SSEL_SIO;
    PINSEL1 = PINSEL1_SSEL_SIO;
    /* Set pin selection of I/O port 1 */
    /* By defaultof PINSEL2 all pins are GPIO (JTAG and TRACE port are disabled) */

    /* Set microcontroller default output values */
    GPIO0->IOCLR = 0xFFFFFFFFUL ; /* reset all */
    /* Set pins that are high by default */

#if     PLATFORM_HARDWARE == LCX_PROTOTYPE_HARDWARE

    GPIO0->IOSET = (
        GPIO0_P_POS_INT    | /* P0.24 output POS_INT power supply */
        GPIO0_LED_CTL      | /* P0.25 or P0.19 output LED control */
        0u);

#else

    GPIO0->IOSET = (
        GPIO0_P_POS_INT    | /* P0.24 output POS_INT power supply */
        GPIO0_P_PRESS      | /* P0.25 Atmospheric Pressure Sensor Power */
        0u);

#endif

    GPIO1->IOCLR = 0xFFFFFFFFUL ; /* reset all */

    /* Set pins that are high by default */
#if     PLATFORM_HARDWARE == LCX_PROTOTYPE_HARDWARE
    GPIO1->IOSET = (
        GPIO1_HDR_CLK      | /* P1.18 output clock for HDR shift reg */
        // GPIO1_HDR_LOAD     | /* P1.19 LOAD signal for HDR shift reg */
        GPIO1_EN_IP        | /* P1.20 output for I/P cutoff */
        GPIO1_HART_RTS     | /* P1.23 output RTS of HART */
        GPIO1_FRAM_WP      | /* P1.24 output FRAM write protect */
        0u);

#else
    GPIO1->IOSET = (
        GPIO1_EN_IP        | /* P1.20 output for I/P cutoff */
        GPIO1_HART_RTS     | /* P1.23 output RTS of HART */
        GPIO1_FRAM_WP      | /* P1.24 output FRAM write protect */
        0u);
#endif

    /* Set microcontroller pins to input or output (input after reset) */
#if     PLATFORM_HARDWARE == LCX_PROTOTYPE_HARDWARE
    GPIO0->IODIR = (
        GPIO0_SP1          | /* P0.5 not used */
        GPIO0_WDI          | /* P0.10 digital output trigger watchdog */
        GPIO0_PCON_SW      | /* P0.11 output bypass switch */
        GPIO0_BOOT         | /* P0.14 output */
        GPIO0_SP2          | /* P0.19 not used */
        GPIO0_LED0         | /* P0.22 output LED select */
        GPIO0_LED1         | /* P0.23 output LED select */
        GPIO0_P_POS_INT    | /* P0.24 POS_INT power supply */
        GPIO0_LED_CTL      | /* P0.25 output LED control */
        GPIO0_ADC_MUX0     | /* P0.27 output multiplexer select */
        GPIO0_ADC_MUX1     | /* P0.28 output multiplexer select */
        GPIO0_ADC_MUX2     | /* P0.29 output multiplexer select */
        GPIO0_TEMP_REF_MUX | /* P0.30 output multiplexer select */
        0u);
#else
    GPIO0->IODIR = (
        GPIO0_WDI          | /* P0.10 digital output trigger watchdog */
        GPIO0_PCON_SW      | /* P0.11 output bypass switch */
        GPIO0_DO_1         | /* P0.12 DO 1 output */
        GPIO0_DO_2         | /* P0.13 DO 2 output */
        GPIO0_BOOT         | /* P0.14 output */
        GPIO0_LED_CTL      | /* P0.19 output LED control  */
        GPIO0_LED0         | /* P0.22 output LED select */
        GPIO0_LED1         | /* P0.23 output LED select */
        GPIO0_P_POS_INT    | /* P0.24 POS_INT power supply */
        GPIO0_P_PRESS      | /* P0.25 Pressure Sensor Power */
        GPIO0_ADC_MUX0     | /* P0.27 output multiplexer select */
        GPIO0_ADC_MUX1     | /* P0.28 output multiplexer select */
        GPIO0_ADC_MUX2     | /* P0.29 output multiplexer select */
        GPIO0_EN_SWITCH    | /* P0.30 Enable Switch */
        0u);
#endif

#if     PLATFORM_HARDWARE == LCX_PROTOTYPE_HARDWARE
    GPIO1->IODIR = (
        GPIO1_HDR_CLK      |  /* P1.18 output clock for HDR shift reg */
        GPIO1_HDR_LOAD     |  /* P1.19 output LOAD signal for HDR shift reg */
        GPIO1_EN_IP        |  /* P1.20 output for I/P cutoff */
        GPIO1_CS_AD        |  /* P1.21 output chip select of A/D converter */
        GPIO1_HART_RTS     |  /* P1.23 output RTS of HART */
        GPIO1_FRAM_WP      |  /* P1.24 output FRAM write protect */
        0u);
#else
    GPIO1->IODIR = (
        GPIO1_EN_IP        |  /* P1.20 output for I/P cutoff */
        GPIO1_CS_AD        |  /* P1.21 output chip select of A/D converter */
        GPIO1_HART_RTS     |  /* P1.23 output RTS of HART */
        GPIO1_FRAM_WP      |  /* P1.24 output FRAM write protect */
        0u);
#endif

#if     PLATFORM_HARDWARE == LCX_PROTOTYPE_HARDWARE

    GPIO1->IOSET= (
        GPIO0_LED_CTL      |  /* P0.25 or P0.19 output LED control */
        0u);
#endif

    // set here - not above in "GPIO1->IOSET =".  ADC needs 1 select cycle to
    // settle internally.
    GPIO1->IOSET = (GPIO1_CS_AD);
}

/* End of the source */

