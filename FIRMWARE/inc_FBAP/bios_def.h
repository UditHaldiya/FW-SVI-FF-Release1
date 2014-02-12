#ifndef _BIOS_DEF_H
/*******************************************************************************
* Copyright (c) 1999/2000 by Dresser. All rights reserved.
* No part of this document or its associated software may be reproduced, transcribed,
* stored in a retrieval system, translated into any language or computer language,
* or transmitted in any form whatsoever without the prior written consent.
********************************************************************************
* Modulheader    : constant definitions
*
* Filename	 : bios_def.h
* Storage	 : \dresser.181\vc_mncb1.01\fw\work\driver\inc\
*
* Project	 : 18101
*
* Class/Type     : driver
*
* Date		 : 29.06.04, 12:42:23
*
* Version	 : 0.01
*
* Author	 : M.Rimkus, Mesco Engineering GmbH
*
* Comment	 : constants to set or reset microcontroller I/O pins.
*
*******************************************************************************/

 /*******************************************************************************
*	Changes
********************************************************************************
* Date			       SgnVer.	Changes
* 29.06.04	  MR	0.01	   Erstellt
* 29.11.04   MR 0.1     module checks
*******************************************************************************/

/*******************************************************************************
*** local definitions
*******************************************************************************/
#define _BIOS_DEF_H
/*******************************************************************************
*** data type definitions
*******************************************************************************/

/* bitmasks for I/O port access */
/* there is a seperate port register for Bitset and Bitclear */
#define MASK_32BIT_D0   ((u32)(1u << 0))
#define MASK_32BIT_D1   ((u32)(1u << 1))
#define MASK_32BIT_D2   ((u32)(1u << 2))
#define MASK_32BIT_D3   ((u32)(1u << 3))
#define MASK_32BIT_D4   ((u32)(1u << 4))
#define MASK_32BIT_D5   ((u32)(1u << 5))
#define MASK_32BIT_D6   ((u32)(1u << 6))
#define MASK_32BIT_D7   ((u32)(1u << 7))
#define MASK_32BIT_D8   ((u32)(1u << 8))
#define MASK_32BIT_D9   ((u32)(1u << 9))
#define MASK_32BIT_D10  ((u32)(1u << 10))
#define MASK_32BIT_D11  ((u32)(1u << 11))
#define MASK_32BIT_D12  ((u32)(1u << 12))
#define MASK_32BIT_D13  ((u32)(1u << 13))
#define MASK_32BIT_D14  ((u32)(1u << 14))
#define MASK_32BIT_D15  ((u32)(1u << 15))
#define MASK_32BIT_D16  ((u32)(1u << 16))
#define MASK_32BIT_D17  ((u32)(1u << 17))
#define MASK_32BIT_D18  ((u32)(1u << 18))
#define MASK_32BIT_D19  ((u32)(1u << 19))
#define MASK_32BIT_D20  ((u32)(1u << 20))
#define MASK_32BIT_D21  ((u32)(1u << 21))
#define MASK_32BIT_D22  ((u32)(1u << 22))
#define MASK_32BIT_D23  ((u32)(1u << 23))
#define MASK_32BIT_D24  ((u32)(1u << 24))
#define MASK_32BIT_D25  ((u32)(1u << 25))
#define MASK_32BIT_D26  ((u32)(1u << 26))
#define MASK_32BIT_D27  ((u32)(1u << 27))
#define MASK_32BIT_D28  ((u32)(1u << 28))
#define MASK_32BIT_D29  ((u32)(1u << 29))
#define MASK_32BIT_D30  ((u32)(1u << 30))
#define MASK_32BIT_D31  ((u32)(1u << 31))
#define MASK_32BIT_D32  ((u32)(1u << 32))

/* bitdefinition of digital input/output pins Port0 */
#define GPIO0_TX0       MASK_32BIT_D0    /* P0.0 TX0 output */
#define GPIO0_RX0       MASK_32BIT_D1    /* P0.1 RX0 input  */

                                        /* P0.2 is I2c SCL */
                                        /* P0.3 is I2c SDA */
#define GPIO0_SCLK0     MASK_32BIT_D4    /* P0.4 SPIO0 clock output */
                                        /* P0.5 is MISO SPI0 input (not used) */
#define GPIO0_MOSI0     MASK_32BIT_D6   /* P0.6 SPIO0 data output */
#define GPIO0_SSEL0     MASK_32BIT_D7   /* P0.7 SSEL0 slave select input	*/
#define GPIO0_PWM_IP0   MASK_32BIT_D8   /* P0.8 PWM channel4 output */
#define GPIO0_DI_SUP    MASK_32BIT_D9   /* P0.9 output DI_SUP */
#define GPIO0_WDI       MASK_32BIT_D10  /* P0.12 digital output trigger watchdog */
#define GPIO0_PCON_SW   MASK_32BIT_D11  /* P0.13 output bypass switch */
#define GPIO0_DO_1      MASK_32BIT_D12  /* P0.10 digital output */
#define GPIO0_DO_2      MASK_32BIT_D13  /* P0.11 digital output */
#define GPIO0_BOOT      MASK_32BIT_D14  /* P0.14 output */
#define GPIO0_PRESS_MUX0 MASK_32BIT_D15 /* P0.15 output multiplexer select */
#define GPIO0_PRESS_MUX1 MASK_32BIT_D16 /* P0.16 output multiplexer select */
#define GPIO0_SCLK1     MASK_32BIT_D17  /* P0.17 SPIO1 clock output */
                                        /* P0.18 is MISO SPI1 input */
#define GPIO0_MOSI1     MASK_32BIT_D19  /* P0.19 SPIO1 data output */
#define GPIO0_SSEL1     MASK_32BIT_D20  /* P0.20 SSEL1 slave select input */
#define GPIO0_PWM_IP1   MASK_32BIT_D21  /* P0.21 PMW channel5 output */
#define GPIO0_DI        MASK_32BIT_D22  /* P0.22 input DI */
#define GPIO0_P_POS_REM MASK_32BIT_D23  /* P0.23 POS_REM power supply */
#define GPIO0_P_POS_INT MASK_32BIT_D24  /* P0.24 POS_INT power supply */
#define GPIO0_P_PRESS   MASK_32BIT_D25  /* P0.25 POS_PRESS power supply */
#define GPIO0_ADC_MUX0  MASK_32BIT_D27  /* P0.27 output multiplexer select */
#define GPIO0_ADC_MUX1  MASK_32BIT_D28  /* P0.28 output multiplexer select */
#define GPIO0_ADC_MUX2  MASK_32BIT_D29  /* P0.29 output multiplexer select */
#define GPIO0_TEMP_REF_MUX   MASK_32BIT_D30 /* P0.30 output multiplexer select */

/* bitdefinition of digital input/output pins Port1 */
#define GPIO1_WP        MASK_32BIT_D16  /* P1.16 input WP */
#define GPIO1_SI_PB     MASK_32BIT_D17  /* P1.17 input of pushbutton shift register */
#define GPIO1_RS_LCD    MASK_32BIT_D18  /* P1.18 output register select of LCD */
#define GPIO1_CSB_LCD   MASK_32BIT_D19  /* P1.19 output chip select of LCD */
#define GPIO1_EN_IP     MASK_32BIT_D20  /* P1.20 output for test purposes */
#define GPIO1_CS_AD     MASK_32BIT_D21  /* P1.21 output chip select of A/D converter */
#define GPIO1_CS_DA     MASK_32BIT_D22  /* P1.22 output chip select of D/A converter */
#define GPIO1_HART_RTS  MASK_32BIT_D23  /* P1.23 output RTS of HART */
#define GPIO1_FRAM_WP   MASK_32BIT_D24  /* P1.24 output for test purposes */
#define GPIO1_HART_CD   MASK_32BIT_D25  /* P1.25 output for HART modem reset */
                                        /* P1.26 JTAG activation pin (do not use) */

#define SHIFT8 (8) /* shifting of data (left or right) */
#define SHIFT4 (4)
#define SHIFT1 (1)
#define SHIFT2 (2)
#define SHIFT28 (28)
#define SHIFT32 (32)

#define U8_NULL ((u8)0) /* zero constants */
#define U32_NULL ((u32)0)
#define S32_NULL ((s32)0)
#define NULL_N    (0u)

#ifdef DEBUG_BIOS
  #define SET_TEST1PIN  (GPIO1->IOSET = GPIO1_TEST1);
  #define CLR_TEST1PIN  (GPIO1->IOCLR = GPIO1_TEST1);
#else
  #define SET_TEST1PIN
  #define CLR_TEST1PIN
#endif

/* PMW timer counter is reset to 0 if match0 register reaches PWM_FREQUENCY */
/* PWM freq = 1.8khz -> (2/1.84e6 * 512(9bit)) (0..511 = 512 steps) */
#define PWM_CHANNEL_6_FREQUENCY 0 //don't care (see PWM_CHANNELS_CFG_ENABLE)

/* PWM channel 4, 5 are single edge. and enabled */
#define PWM_CHANNELS_CFG_ENABLE     (ENA4 | ENA5)

/* reload match channel 0, 4, 5 */
#define PWM_CHANNELS_RELOAD         (EM0L | EM4L | EM5L)

//AK:RED_FLAG: Stolen from AP inituc.h; there could be a better place for these
/* SSEL pin is used as GPIO to reduce power consumption */
#define PIN0_0_TXD       ((u32)(1u << 0))  /* P0.0 is TXD UART0 */
#define PIN0_1_RXD 		 ((u32)(1u << 2))  /* P0.1 is RXD UART0 */
#define PIN0_2_SCL       ((u32)(1u << 4))  /* P0.2 is SCL I2C */
#define PIN0_3_SDA       ((u32)(1u << 6))  /* P0.3 is SDA I2C */
#define PIN0_4_SCK       ((u32)(1u << 8))  /* P0.4 is SCK SPI0 */
#define PIN0_5_MISO      ((u32)(1u << 10)) /* P0.5 is MISO SPI0 */
#define PIN0_6_MOSI      ((u32)(1u << 12)) /* P0.6 is MOSI SPI0 */
#define PIN0_7_GPIO      ((u32)(0u << 14)) /* P0.7	is GPIO */
#define PIN0_7_SSEL      ((u32)(1u << 14)) /* P0.7	is SSEL SPI0 */
#define PIN0_8_PMW4      ((u32)(2u << 16)) /* P0.8 is PWM output channel 4 */
#define PIN0_12_MATCH    ((u32)(2u << 24)) /* P0.12 is MATCH Timer1 DO_1 */
#define PIN0_13_MATCH    ((u32)(2u << 26)) /* P0.13 is MATCH Timer1 DO_2 */

#  define PINSEL0_SSEL_GPIO  (PIN0_0_TXD | PIN0_1_RXD | PIN0_2_SCL | PIN0_3_SDA |              \
                              PIN0_5_MISO |               PIN0_8_PMW4 | PIN0_12_MATCH | PIN0_13_MATCH)
/* SSEL pin of SPI0 is used for SPI0 master mode */
#define PINSEL0_SSEL_SIO (PINSEL0_SSEL_GPIO | PIN0_7_SSEL | PIN0_4_SCK | PIN0_6_MOSI)

/* SSEL pin is used as GPIO to reduce power consumption */
#define PIN0_17_SCK      ((u32)(2u << 2))  /* P0.17 is SPI1 SCK */
#define PIN0_18_MISO     ((u32)(2u << 4))  /* P0.18 is SPI1 MISO */
#define PIN0_19_MOSI     ((u32)(2u << 6))  /* P0.19 is SPI1 MOSI */
#define PIN0_20_SSEL     ((u32)(2u << 8))  /* P0.20 is SPI1 SSEL */
#define PIN0_21_PWM5     ((u32)(1u << 10)) /* P0.21 is PWM output channel 5 */

#  define PINSEL1_SSEL_GPIO (              PIN0_18_MISO |                PIN0_21_PWM5)
/* SSEL pin of SPI1 is used for SPI1 master mode */
#define PINSEL1_SSEL_SIO (PINSEL1_SSEL_GPIO | PIN0_20_SSEL | PIN0_17_SCK | PIN0_19_MOSI)
//AK:RED_FLAG_END

// I2C Enabling Macro's - for i2cpriv.h
#define I2C_BUS_ENABLE(slaveaddr)
#define I2C_BUS_DISABLE()

//--------------------------------------------------------------------

// defines for checking the 1.25 reference voltage (AD_DIAG)
#define NOMINAL_VREF    32768.0

//--------------------------------------------------------------------

#define VREF_PERCENT    (5.0 / 100.0)
#define VREF_DEV        ((u32)(NOMINAL_VREF * VREF_PERCENT))
#define VREF_LOW        ((u32)NOMINAL_VREF - VREF_DEV)
//#define VREF_HIGH     ((u32)NOMINAL_VREF + VREF_DEV)

//--------------------------------------------------------------------

#else
  /* no error */
#endif
