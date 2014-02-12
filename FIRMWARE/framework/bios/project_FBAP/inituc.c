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
#define TIMER1_MATCH_VALUE  (u32)3

/* number of fetch cycles for reading FLASH memory (2 processor clocks per access) */
#define MAMTIM_INIT_VALUE (u32)2

#define DRIVE_JTAG 0
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
    PINSEL0 = PINSEL0_SSEL_GPIO;
    PINSEL1 = PINSEL1_SSEL_GPIO;
    /* Set pin selection of I/O port 1 */
    /* By defaultof PINSEL2 all pins are GPIO (JTAG and TRACE port are disabled) */

    /* Set microcontroller default output values */
    GPIO0->IOCLR = 0xFFFFFFFFUL ; /* reset all */
    /* Set pins that are high by default */
    GPIO0->IOSET = (

        GPIO0_SCLK0        | /* P0.4 SPIO0 clock output */
//        GPIO0_MOSI0        | /* P0.6 SPIO0 data output */
        GPIO0_SCLK1        | /* P0.17 SPIO1 clock output */
//        GPIO0_MOSI1        | /* P0.19 SPIO1 data output */

        GPIO0_P_POS_REM    | /* P0.23 output POS_REM power supply */
        GPIO0_P_POS_INT    | /* P0.24 output POS_INT power supply */
  	    GPIO0_P_PRESS);      /* P0.25 output pressure sensor power supply */

    GPIO1->IOCLR = 0xFFFFFFFFUL ; /* reset all */

    /* Set pins that are high by default */
    GPIO1->IOSET = (
        GPIO1_RS_LCD       | /* P1.18 output register select of LCD */
        GPIO1_CSB_LCD      | /* P1.19 output chip select of LCD */
        GPIO1_EN_IP        | /* P1.20 output for test purposes */
//      GPIO1_CS_AD        | /* P1.21 output chip select of A/D converter */
        GPIO1_CS_DA        | /* P1.22 output chip select of D/A converter */
        GPIO1_HART_RTS     | /* P1.23 output RTS of HART */
        GPIO1_FRAM_WP      | /* P1.24 output FRAM write protect */
#if DRIVE_JTAG
        GPIO1_JTAG_1       | /* P1.26 JTAG RTCK - activation pin (do not use) */
        GPIO1_JTAG_2       | /* P1.27 JTAG TDO  */
        GPIO1_JTAG_3       | /* P1.28 JTAG TDI  */
        GPIO1_JTAG_4       | /* P1.29 JTAG TCK  */
        GPIO1_JTAG_5       | /* P1.30 JTAG TMS  */
        GPIO1_JTAG_6       | /* P1.31 JTAG TRST */
#endif
        0u);

    /* Set microcontroller pins to input or output (input after reset) */
    GPIO0->IODIR = (
        GPIO0_TX0          | /* P0.0 output  */
                             /* P0.1 input  RX0 of HART */
  		                     /* P0.2 input  I2c SCL */
  		                     /* P0.3 input  I2c SDA */
		GPIO0_SCLK0        | /* P0.4 output SPI0 SCLKO */
		                     /* P0.5 input  SPI0 MISO0 */
		GPIO0_MOSI0        | /* P0.6 output SPI0 MOSI0 */
		GPIO0_SSEL0        | /* P0.7 output SPIO0 SSEL */
		GPIO0_PWM_IP0      | /* P0.8 output PWM channel4 */
		GPIO0_DI_SUP       | /* P0.9 output DI_SUP */
        GPIO0_WDI          | /* P0.10 output digital output WDI */
		GPIO0_PCON_SW      | /* P0.11 output digital output PCON_SW */
		GPIO0_DO_1         | /* P0.12 output Match Timer1 DO_1 */
		GPIO0_DO_2         | /* P0.13 output Match Timer1 DO_2 */
		GPIO0_BOOT         | /* P0.14 output */
		GPIO0_PRESS_MUX0   | /* P0.15 output multiplexer select */
		GPIO0_PRESS_MUX1   | /* P0.16 output multiplexer select */
		GPIO0_SCLK1        | /* P0.17 output SPI1 SCLK1 (until SPI1 initialized) */
			                 /* P0.18 input  SPI1 MISO1 */
		GPIO0_MOSI1        | /* P0.19 output SPI0 MOSI0 */
		GPIO0_SSEL1        | /* P0.20 output SPIO1 SSEL */
		GPIO0_PWM_IP1      | /* P0.21 output PWM channel 5 */
		                                 /* P0.22 input DI */
		GPIO0_P_POS_REM    | /* P0.23 output POS_REM power supply */
		GPIO0_P_POS_INT    | /* P0.24 output POS_INT power supply */
		GPIO0_P_PRESS      | /* P0.25 output pressure sensor power supply */
		GPIO0_ADC_MUX0     | /* P0.27 multiplexer select */
		GPIO0_ADC_MUX1     | /* P0.28 multiplexer select */
		GPIO0_ADC_MUX2     | /* P0.29 multiplexer select */
		GPIO0_TEMP_REF_MUX); /* P0.30 multiplexer select */

    GPIO1->IODIR = (
                             /* P1.16 input WP */
                             /* P1.17 input of pushbutton shift register */
        GPIO1_RS_LCD       | /* P1.18 output register select of LCD */
        GPIO1_CSB_LCD      | /* P1.19 output chip select of LCD */
        GPIO1_EN_IP        | /* P1.20 output for test purposes */
        GPIO1_CS_AD        | /* P1.21 output chip select of A/D converter */
        GPIO1_CS_DA        | /* P1.22 output chip select of D/A converter */
        GPIO1_HART_RTS     | /* P1.23 output RTS of HART */
        GPIO1_FRAM_WP      | /* P1.24 output FRAM write protect */
                             /* P1.25 input for HART modem reset */
#if DRIVE_JTAG
        GPIO1_JTAG_1       | /* P1.26 JTAG RTCK - activation pin (do not use) */
        GPIO1_JTAG_2       | /* P1.27 JTAG TDO  */
        GPIO1_JTAG_3       | /* P1.28 JTAG TDI  */
        GPIO1_JTAG_4       | /* P1.29 JTAG TCK  */
        GPIO1_JTAG_5       | /* P1.30 JTAG TMS  */
        GPIO1_JTAG_6       | /* P1.31 JTAG TRST */
#endif
        0u);
    GPIO1->IOSET = (GPIO1_CS_AD);
}

/* End of the source */

