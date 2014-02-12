#ifndef _BIOS_DEF_H
/*******************************************************************************
* Copyright (c) 1999/2000 by Dresser. All rights reserved.
* No part of this document or its associated software may be reproduced, transcribed,
* stored in a retrieval system, translated into any language or computer language,
* or transmitted in any form whatsoever without the prior written consent.
********************************************************************************
* Modulheader    : constant definitions
*
* Filename   : bios_def.h
* Storage    : \dresser.181\vc_mncb1.01\fw\work\driver\inc\
*
* Project    : 18101
*
* Class/Type     : driver
*
* Date       : 29.06.04, 12:42:23
*
* Version    : 0.01
*
* Author     : M.Rimkus, Mesco Engineering GmbH
*
* Comment    : constants to set or reset microcontroller I/O pins.
*
*******************************************************************************/

/* (Optional) $History: bios_def.h $
 * 
 * *****************  Version 41  *****************
 * User: Anatoly Podpaly Date: 1/12/12    Time: 1:48p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_LCX
 * TFS:8604 - moved the #defines for the Reference Voltage.
 *
 * *****************  Version 40  *****************
 * User: Justin Shriver Date: 11/29/11   Time: 5:37p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_LCX
 * -- AP -- LINT corrections.
 *
 * *****************  Version 38  *****************
 * User: Anatoly Podpaly Date: 12/06/10   Time: 5:32p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * Real H/W build
 *
 * *****************  Version 37  *****************
 * User: Anatoly Podpaly Date: 11/19/10   Time: 5:05p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * Build for proto.
 *
 * *****************  Version 36  *****************
 * User: Anatoly Podpaly Date: 11/19/10   Time: 4:19p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * Build for Real H/W (Beta 4)
 *
 * *****************  Version 35  *****************
 * User: Anatoly Podpaly Date: 11/17/10   Time: 4:47p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * Build for Proto restored.
 *
 * *****************  Version 34  *****************
 * User: Anatoly Podpaly Date: 11/17/10   Time: 4:36p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * Build for Real H/W
 *
 * *****************  Version 33  *****************
 * User: Anatoly Podpaly Date: 11/12/10   Time: 4:51p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * Build for Proto H/W (SVi 1000)
 *
 * *****************  Version 32  *****************
 * User: Anatoly Podpaly Date: 11/12/10   Time: 4:49p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * Build for Actual H/W (SVI 100)
 *
 * *****************  Version 31  *****************
 * User: Sergey Kruss Date: 10/29/10   Time: 4:55p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * Modified to build FW for Beta2 for Proto
 *
 * *****************  Version 30  *****************
 * User: Sergey Kruss Date: 10/29/10   Time: 4:17p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * Modified to build FW for Beta-2 Real SVI-1000
 *
 * *****************  Version 29  *****************
 * User: Anatoly Podpaly Date: 10/22/10   Time: 6:10p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * Build for proto
 *
 * *****************  Version 28  *****************
 * User: Anatoly Podpaly Date: 10/22/10   Time: 5:20p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 *
 * *****************  Version 27  *****************
 * User: Anatoly Podpaly Date: 10/15/10   Time: 6:01p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * Build for Proto HW.
 *
 * *****************  Version 26  *****************
 * User: Anatoly Podpaly Date: 10/15/10   Time: 5:42p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * Prepare for Alpha 13 build for Real H/W
 *
 * *****************  Version 25  *****************
 * User: Anatoly Podpaly Date: 10/12/10   Time: 2:22p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * Bug 4359 - Added definition for the Platform Hardware.
 *
 * *****************  Version 24  *****************
 * User: Anatoly Podpaly Date: 10/08/10   Time: 4:39p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * Build for Proto.
 *
 * *****************  Version 23  *****************
 * User: Anatoly Podpaly Date: 10/08/10   Time: 3:08p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * Alpha 12 build for Real H/W
 *
 * *****************  Version 22  *****************
 * User: Anatoly Podpaly Date: 10/01/10   Time: 11:56a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * Enable Prototype compilation.
 *
 * *****************  Version 21  *****************
 * User: Justin Shriver Date: 9/16/10    Time: 11:20a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * TFS:4126 handle both prototype and preproduction hardware
 *
 * *****************  Version 20  *****************
 * User: Anatoly Podpaly Date: 9/09/10    Time: 12:21p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * Updates for ACtual H/W.
 *
 * *****************  Version 19  *****************
 * User: Anatoly Podpaly Date: 8/25/10    Time: 11:52a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * Added H/W GPIO to control PB / RS switch
 *
 * *****************  Version 18  *****************
 * User: Anatoly Podpaly Date: 8/20/10    Time: 5:17p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * Packaged I2C as a module: added #defines for compatibility sake.
 *
 * *****************  Version 17  *****************
 * User: Anatoly Podpaly Date: 7/30/10    Time: 7:07p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * Updeta for the actuial H/W
 *
 * *****************  Version 16  *****************
 * User: Anatoly Podpaly Date: 6/18/10    Time: 11:20a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * Added mods (conditionally compiled) for new H/W
 *
 * *****************  Version 15  *****************
 * User: Anatoly Podpaly Date: 6/16/10    Time: 2:24p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * Added #define for the proto H/W
 *
 * *****************  Version 13  *****************
 * User: Anatoly Podpaly Date: 5/28/10    Time: 3:15p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * LUI Req numbers.
 *
 * *****************  Version 12  *****************
 * User: Anatoly Podpaly Date: 5/21/10    Time: 11:41a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * 3263, 3262 - placed #defines to properly initialize and enable PWM
 * channels here. Moved definitions from inituc.c. Corrected GPIO 0.7 name
 * (was thoutght to be incorrect setting).
 *
*/

 /*******************************************************************************
*   Changes
********************************************************************************
* Date                 SgnVer.  Changes
* 29.06.04    MR    0.01       Erstellt
* 29.11.04   MR 0.1     module checks
*******************************************************************************/

/*******************************************************************************
*** local definitions
*******************************************************************************/
#define _BIOS_DEF_H

// If this defined - we have a proto H/W
#define LCX_PROTOTYPE_HARDWARE      (1u)
#define LCX_REAL_HARDWARE           (2u)

//-------------------------------------------------------------------------
// #define PLATFORM_HARDWARE           (LCX_PROTOTYPE_HARDWARE)
#define PLATFORM_HARDWARE           (LCX_REAL_HARDWARE)

#if ((PLATFORM_HARDWARE != LCX_PROTOTYPE_HARDWARE) && (PLATFORM_HARDWARE != LCX_REAL_HARDWARE))
#error Incorrect Platform Hardware option specified
#endif
//-------------------------------------------------------------------------

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
#if     PLATFORM_HARDWARE == LCX_PROTOTYPE_HARDWARE

#define GPIO0_AUTO      MASK_32BIT_D12  /* P0.12 input PB AUTO */
#define GPIO0_OPEN      MASK_32BIT_D13  /* P0.13 input PB OPEN */
#define GPIO0_CLOSE     MASK_32BIT_D15  /* P0.15 input PB CLOSE */
#define GPIO0_CANCEL    MASK_32BIT_D16  /* P0.16 input PB CANCEL */
#define GPIO0_SCLK0     MASK_32BIT_D4   /* P0.4 SPIO0 clock output */
#define GPIO0_SP1       MASK_32BIT_D5   /* P0.5 is not used */
#define GPIO0_MOSI0     MASK_32BIT_D6   /* P0.6 SPIO0 data output to LCD */

#else

#define GPIO0_RS_B0     MASK_32BIT_D4   /* P0.4  Rotary Switch B0 */
#define GPIO0_RS_B1     MASK_32BIT_D5   /* P0.5  Rotary Switch B1 */
#define GPIO0_RS_B2     MASK_32BIT_D6   /* P0.6  Rotary Switch B2 */
#define GPIO0_RS_B3     MASK_32BIT_D15  /* P0.15 Rotary Switch B3 */

#define RS_JUMPERS_BIT_0    (1u << 0)
#define RS_JUMPERS_BIT_1    (1u << 1)
#define RS_JUMPERS_BIT_2    (1u << 2)
#define RS_JUMPERS_BIT_3    (1u << 3)

#endif  // platform hardware

#define GPIO0_TX0       MASK_32BIT_D0   /* P0.0 TX0 output */
#define GPIO0_RX0       MASK_32BIT_D1   /* P0.1 RX0 input  */
#define GPIO0_SCL       MASK_32BIT_D2   /* P0.2 is I2c SCL */
#define GPIO0_SDA       MASK_32BIT_D3   /* P0.3 is I2c SDA */
#define GPIO0_PHASEA    MASK_32BIT_D7   /* P0.7 quad input A */
#define GPIO0_PWM_IP0   MASK_32BIT_D8   /* P0.8 PWM channel 4 output */
#define GPIO0_WDI       MASK_32BIT_D10  /* P0.10 digital output trigger watchdog */
#define GPIO0_PCON_SW   MASK_32BIT_D11  /* P0.11 output bypass switch */
#define GPIO0_BOOT      MASK_32BIT_D14  /* P0.14 output */
#define GPIO0_SCLK1     MASK_32BIT_D17  /* P0.17 SPIO1 clock output */
#define GPIO0_MISO1     MASK_32BIT_D18  /* P0.18 is data input from ADC */
#define GPIO0_SP2       MASK_32BIT_D19  /* P0.19 not used */
#define GPIO0_PHASEB    MASK_32BIT_D20  /* P0.20 quad input B */
#define GPIO0_PWM_IP1   MASK_32BIT_D21  /* P0.21 PMW channel5 output */
#define GPIO0_P_POS_INT MASK_32BIT_D24  /* P0.24 POS_INT power supply */
#define GPIO0_X26       MASK_32BIT_D26  /* P0.26 is reserved */
#define GPIO0_ADC_MUX0  MASK_32BIT_D27  /* P0.27 output multiplexer select */
#define GPIO0_ADC_MUX1  MASK_32BIT_D28  /* P0.28 output multiplexer select */
#define GPIO0_ADC_MUX2  MASK_32BIT_D29  /* P0.29 output multiplexer select */

//---------------------------------------
// LED Controls, DOs, Enable Switch, P_PRESS
#if     PLATFORM_HARDWARE == LCX_PROTOTYPE_HARDWARE

#define GPIO0_LED_DUTY  MASK_32BIT_D9   /* P0.9 PWM channel 6 LED_DUTY */
#define GPIO0_LED0      MASK_32BIT_D22  /* P0.22 output LED select */
#define GPIO0_LED1      MASK_32BIT_D23  /* P0.23 output LED select */
#define GPIO0_LED_CTL   MASK_32BIT_D25  /* P0.25 output LED control */

#define GPIO0_TEMP_REF_MUX   MASK_32BIT_D30 /* P0.30 output multiplexer select */

#else

#define GPIO0_LED_DUTY  MASK_32BIT_D9   /* P0.9 PWM channel 6 LED_DUTY */
#define GPIO0_LED0      MASK_32BIT_D22  /* P0.22 output LED select */
#define GPIO0_LED1      MASK_32BIT_D23  /* P0.23 output LED select */
#define GPIO0_LED_CTL   MASK_32BIT_D19  /* P0.25 output LED control */

#define GPIO0_EN_SWITCH      MASK_32BIT_D30 /* P0.30 Enable Switch for PBs, Rotary Switch */

#define GPIO0_DO_1      MASK_32BIT_D12  /* P0.12 OUTPUT DO 1 */
#define GPIO0_DO_2      MASK_32BIT_D13  /* P0.13 OUTPUT DO 2 */
#define GPIO0_P_PRESS   MASK_32BIT_D25  /* P0.25 Power for P_Atmos */

#endif

//---------------------------------------
#define GPIO0_X31       MASK_32BIT_D31  /* P0.31 is reserved */

#if     PLATFORM_HARDWARE == LCX_PROTOTYPE_HARDWARE

#define GPIO1_WP        MASK_32BIT_D16  /* P1.16 input WP */
#define GPIO1_SI_HDR    MASK_32BIT_D17  /* P1.17 input of pushbutton shift register */
#define GPIO1_HDR_CLK   MASK_32BIT_D18  /* P1.18 output clock for HDR shift reg */
#define GPIO1_HDR_LOAD  MASK_32BIT_D19  /* P1.19 output LOAD signal for HDR shift reg*/
#define GPIO1_SP1       MASK_32BIT_D22  /* P1.22 is not used */

#define HDR_GPIO_PIN    17u     // in port 1 - input from shift register
#define PB_ALIGN_SFT    12u     // in port 0 - lcation of AUTO pushhbutton

#else

#define GPIO1_PB_AUTO   MASK_32BIT_D16      /* P1.16 input PB AutoTune */
#define GPIO1_PB_OPEN   MASK_32BIT_D17      /* P1.17 input PB Open Stop Adjustment */
#define GPIO1_PB_FIND   MASK_32BIT_D18      /* P1.18 input PB FindStops */
#define GPIO1_PB_CANCEL MASK_32BIT_D19      /* P1.19 input PB Cancel */

#define GPIO1_TERMINAL_CARD     MASK_32BIT_D22  /* P1.22 Input - Terminal Card present */

#endif

#define LED_SHIFT       22u     // location of LED0 select
#define QPA_PIN         7u
#define QPB_PIN         20u     // in LCx mockup

/* bitdefinition of digital input/output pins Port1 */
#define GPIO1_EN_IP     MASK_32BIT_D20  /* P1.20 output for I/P cutoff */
#define GPIO1_CS_AD     MASK_32BIT_D21  /* P1.21 output chip select of A/D converter */
#define GPIO1_HART_RTS  MASK_32BIT_D23  /* P1.23 output RTS of HART */
#define GPIO1_FRAM_WP   MASK_32BIT_D24  /* P1.24 output FRAM write protect */
#define GPIO1_HART_CD   MASK_32BIT_D25  /* P1.25 input for HART carrier detect */
                                        /* P1.26 JTAG activation pin (do not use) */
#define SHIFT8 8 /* shifting of data (left or right) */
#define SHIFT4 4
#define SHIFT1 1
#define SHIFT2 2
#define SHIFT28 28
#define SHIFT32 32

#define U8_NULL  ((u8)0) /* zero constants */
#define U32_NULL ((u32)0)
#define S32_NULL ((s32)0)
#define NULL_N      0

#ifdef DEBUG_BIOS
  #define SET_TEST1PIN  GPIO1->IOSET = GPIO1_TEST1;
  #define CLR_TEST1PIN  GPIO1->IOCLR = GPIO1_TEST1;
#else
  #define SET_TEST1PIN
  #define CLR_TEST1PIN
#endif

/* BIOS internal definitions */
/* is used for initialization and dynamic reprogramming of SIOs SSEL pins */

/* Pin selection of I/O port 0 (Pin selection register 0) */
#define PIN0_0_TXD       (1u << 0)   /* P0.0 TX0 output */
#define PIN0_1_RXD       (1u << 2)   /* P0.1 RX0 input  */
#define PIN0_2_SCL       (1u << 4)   /* P0.2 is I2c SCL */
#define PIN0_3_SDA       (1u << 6)   /* P0.3 is I2c SDA */
#define PIN0_4_SCK       (1u << 8)   /* P0.4 SPIO0 clock output */
#define PIN0_4_GPIO      (0u << 8)   /* P0.4 Set as GPIO */
#define PIN0_5_GPIO      (0u << 10)  /* P0.5 is not used */
#define PIN0_6_MOSI      (1u << 12)  /* P0.6 SPIO0 data output to LCD */
#define PIN0_6_GPIO      (0u << 12)  /* P0.6 Set as GPIO */
#define PIN0_7_EXTINT    (3u << 14)  /* P0.7 quad input A */
#define PIN0_7_GPIO      (0u << 14)  /* P0.7 Set as GPIO */
#define PIN0_8_PMW4      (2u << 16)  /* P0.8 PWM channel 4 output */
#define PIN0_9_PWM6      (2u << 18)  /* P0.9 PWM channel 6 LED_DUTY */
#define PIN0_10_GPIO     (0u << 20)  /* P0.10 digital output trigger watchdog */
#define PIN0_11_GPIO     (0u << 22)  /* P0.11 output bypass switch */
#define PIN0_12_GPIO     (0u << 24)  /* P0.12 input PB AUTO */
#define PIN0_13_GPIO     (0u << 26)  /* P0.13 input PB OPEN */
#define PIN0_14_GPIO     (0u << 28)  /* P0.14 output */
#define PIN0_15_GPIO     (0u << 30)  /* P0.15 input PB CLOSE */

/* Pin selection of I/O port 0 (Pin selection register 1) */
#define PIN0_16_GPIO     (0u << 0)   /* P0.16 input PB CANCEL */
#define PIN0_17_SCK      (2u << 2)   /* P0.17 SPIO1 clock output */
#define PIN0_18_MISO     (2u << 4)   /* P0.18 is data input from ADC */
#define PIN0_19_GPIO     (0u << 6)   /* P0.19 not used */
#define PIN0_20_GPIO     (0u << 8)   /* P0.20 quad input B */
#define PIN0_21_PWM5     (1u << 10)  /* P0.21 PMW channel5 output */
#define PIN0_22_GPIO     (0u << 12)  /* P0.22 output LED select */
#define PIN0_23_GPIO     (0u << 14)  /* P0.23 output LED select */
#define PIN0_24_GPIO     (0u << 16)  /* P0.24 POS_INT power supply */
#define PIN0_25_GPIO     (0u << 18)  /* P0.25 output LED control */
#define PIN0_26_GPIO     (0u << 20)  /* P0.26 is reserved */
#define PIN0_27_GPIO     (0u << 22)  /* P0.27 output multiplexer select */
#define PIN0_28_GPIO     (0u << 24)  /* P0.28 output multiplexer select */
#define PIN0_29_GPIO     (0u << 26)  /* P0.29 output multiplexer select */
#define PIN0_30_GPIO     (0u << 28)  /* P0.30 output multiplexer select Or Enable Switch */
#define PIN0_31_GPIO     (0u << 30)  /* P0.31 is reserved */

// One wire interface definition

#if     PLATFORM_HARDWARE == LCX_PROTOTYPE_HARDWARE
// LCX Prototype 1 Hardware - used the same I/O as AutoTune button
// The resistor on GPIO has to be between 1.5K ..  5K. The existing
// pullup for the PB is 120K, temporarily placed 5.1K in parallel.
// (~4.9K equivalent)

#define     GPIO0_OW_BUS    (15u)             // should be 16 for production units
#else
#define     GPIO0_OW_BUS    (16u)
#endif

// Bit mask for the pin
#define OW_PIN_MASK         (1u << GPIO0_OW_BUS)

/** REQ 31-033 GPIO 7 init as GPIO */
/** REQ 31-032 Interface to PB, QE, Rotary Switch, LEDs */

#if     PLATFORM_HARDWARE == LCX_PROTOTYPE_HARDWARE

#define PINSEL0_SSEL_SIO (\
            PIN0_0_TXD   | PIN0_1_RXD   | PIN0_2_SCL   | PIN0_3_SDA   |\
            PIN0_4_SCK   | PIN0_5_GPIO  | PIN0_6_MOSI  | PIN0_7_GPIO  |\
            PIN0_8_PMW4  | PIN0_9_PWM6  | PIN0_10_GPIO | PIN0_11_GPIO |\
            PIN0_12_GPIO | PIN0_13_GPIO | PIN0_14_GPIO | PIN0_15_GPIO )
#else

#define PINSEL0_SSEL_SIO (\
            PIN0_0_TXD   | PIN0_1_RXD   | PIN0_2_SCL   | PIN0_3_SDA   |\
            PIN0_4_GPIO  | PIN0_5_GPIO  | PIN0_6_GPIO  | PIN0_7_GPIO  |\
            PIN0_8_PMW4  | PIN0_9_PWM6  | PIN0_10_GPIO | PIN0_11_GPIO |\
            PIN0_12_GPIO | PIN0_13_GPIO | PIN0_14_GPIO | PIN0_15_GPIO )
#endif

#define PINSEL1_SSEL_SIO (\
            PIN0_16_GPIO | PIN0_17_SCK  | PIN0_18_MISO | PIN0_19_GPIO |\
            PIN0_20_GPIO | PIN0_21_PWM5 | PIN0_22_GPIO | PIN0_23_GPIO |\
            PIN0_24_GPIO | PIN0_25_GPIO | PIN0_26_GPIO | PIN0_27_GPIO |\
            PIN0_28_GPIO | PIN0_29_GPIO | PIN0_30_GPIO | PIN0_31_GPIO )

#if     PLATFORM_HARDWARE == LCX_PROTOTYPE_HARDWARE
/* PWM6 is for LED duty cycle */
#define PWM_CHANNEL_6_FREQUENCY     (PWM_FREQUENCY / 3u)
#else
// In real H/W the PWM 6 pin will be inverted due to H/W design - use 2/3 Duty
// cycle instead of 1//3 as on prototype H/W
#define PWM_CHANNEL_6_FREQUENCY     ((PWM_FREQUENCY * 2) / 3u)
#endif

/* PWM channel 4, 5, 6 are single edge. and enabled */
#define PWM_CHANNELS_CFG_ENABLE     (ENA4 | ENA5 | ENA6)

/* reload match channel 0,4,5,6 */
#define PWM_CHANNELS_RELOAD         (EM0L | EM4L | EM5L | EM6L)

// I2C Enabling Macro's - for i2cpriv.h
#define I2C_BUS_ENABLE(slaveaddr)
#define I2C_BUS_DISABLE()

//--------------------------------------------------------------------
// defines for checking the 1.25 reference voltage (AD_DIAG)
//#define NOMINAL_VREF    32768.0

// The value of reference voltage for SVi1000 has been changed:
// V ref = 1.00 V [AD_DIAG}
// Use the original definition of VREF to genertate the correct value.

#define NOMINAL_VREF    ((32768.0 * 4.0) / 5.0)
//--------------------------------------------------------------------

#define VREF_PERCENT    (5.0 / 100.0)
#define VREF_DEV        ((u32)(NOMINAL_VREF * VREF_PERCENT))
#define VREF_LOW        ((u32)NOMINAL_VREF - VREF_DEV)
//#define VREF_HIGH     ((u32)NOMINAL_VREF + VREF_DEV)

//--------------------------------------------------------------------

#else
  /* no error */
#endif


//----------------------------------------------------------------------

