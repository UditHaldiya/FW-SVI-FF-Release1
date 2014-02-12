/**
Copyright 2004-2005 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file gpio.h
    \brief header BIOS routines for support of the GPIO control

    CPU: Any (with standard C compiler)

    OWNER: AP
    \n RCS: $Id$
    Created: 9/27/04 Author:  AP
*/

/*
    $History: gpio.h $
 *
 * *****************  Version 6  *****************
 * User: Anatoly Podpaly Date: 1/16/12    Time: 1:56p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_SVI2AP
 * TFS:8567 -- implemented the function to check if LCD is active, i.e.
 * selected.
 *
 * *****************  Version 5  *****************
 * User: Anatoly Podpaly Date: 1/12/12    Time: 4:26p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_SVI2AP
 * TFS:8607 - moved wprotect GPIO.
 *
 * *****************  Version 4  *****************
 * User: Anatoly Podpaly Date: 1/12/12    Time: 3:02p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_SVI2AP
 * TFS:8607 - moved LCD, kbd GPIO controls.
 *
 * *****************  Version 3  *****************
 * User: Anatoly Podpaly Date: 1/10/12    Time: 4:20p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_SVI2AP
 * TFS:8574 - uC specific added GPIO support
 *
*/

#ifndef GPIO_H_
#define GPIO_H_

#include "bios_def.h"

#define POSN(x)     ((u32)x & 15u)      // macro to extract but number from pin reference
#define BIT(x)      (1u << POSN(x))     // mask generator for POSN(x)

//---------------------------------------------------------------------------

void    gpio_SelectExtAD(void);
void    gpio_DeSelectExtAD(void);
void    gpio_SelectMUXPWRbits(u32   bmask);

//---------------------------------------------------------------------------
// Bypass Switch control
void    gpio_SetBypassSwitch(bool_t value);
bool_t  gpio_ReadBypassSwitch(void);

//----------------------------------------------------------------------------------------------------------------------
// LCD and KBD controls
void    lcd_SelectExtLCD(void);
void    lcd_DeSelectExtLCD(void);
void    lcd_SetCommandMode_ExtLCD(void);
void    lcd_SetDataMode_ExtLCD(void);

//----------------------------------------------------------------------------------------------------------------------

void    gpio_SetWatchDogHI(void);
void    gpio_SetWatchDogLO(void);

//----------------------------------------------------------------------------------------------------------------------

bool_t  gpio_ReadWriteProtectInput(void);

//----------------------------------------------------------------------------------------------------------------------

void    FRAM_Write_Enable(void);
void    FRAM_Write_Disable(void);

//----------------------------------------------------------------------------------------------------------------------

void    gpio_I2C_SetI2cClk(bool_t which);
bool_t  gpio_I2C_SDAState(void);

void    bios_SetEnableIP(bool_t on);

//----------------------------------------------------------------------------------------------------------------------

void gpio_PortInit(void);

//----------------------------------------------------------------------------------------------------------------------

MN_IRQ void EXTI_IRQHandler(void);
void        gpio_SetEXTI(void);

u32         gpio_ReadGPIOB(void);

//----------------------------------------------------------------------------------------------------------------------
// Definitions for the GPIO locations -- for the AD H/W driver
//
// Unused definitions are commnted out

// #define DO_1x               (0u)
// #define DO_2x               (1u)
// #define APP2FF_APP          (2u)
// #define FF2APP_APP          (3u)
#define WDT_APP             (4u)
// #define SCLK1               (5u)
// #define MISO1               (6u)
// #define notused0            (7u)
// #define notused1            (8u)
// #define ISP_TX_APP          (9u)
// #define ISP_RX_APP          (10u)
// #define notused2            (11u)
#define FRAM_WR             (12u)
// #define JTMS_APP            (13u)
// #define JTCK_APP            (14u)
// #define JTDI_APP            (15u)
// #define PWM_IP_0            (16u)
// #define PWM_IP_1            (17u)
// #define PULLDOWN            (18u)
// #define JTDO_APP            (19u)
// #define JRES_APP            (20u)
#define WP_APP              (21u)
#define SCL                 (22u)
#define SDA                 (23u)
#define SI_PB               (24u)
#define RS_LCD              (25u)
// #define BOOTLOAD_FF         (26u)
#define DI_SUP              (27u)
#define DI                  (28u)
#define SCL_LCD             (29u)
#define CSB_LCD             (30u)
// #define SO_LCD              (31u)
#define ADC_MUX0            (32u)
#define ADC_MUX1            (33u)
#define ADC_MUX2            (34u)
// #define notused3            (35u)
#define EN_IP               (36u)
#define CS_AD               (37u)
#define PRESS_MUX0          (38u)
#define PRESS_MUX1          (39u)
#define P_POS_REM           (40u)
#define P_POS_INT           (41u)
// #define DEBUG_TX_APP        (42u)
// #define DEBUG_RX_APP        (43u)
// #define notused4            (44u)
// #define notused5            (45u)
#define PCON_SW             (46u)
#define P_PRESS             (47u)

//----------------------------------------------------------------------------------------------------------------------

#endif // GPIO_H_

/* End of the source */
