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
#include "lpc2114io.h"

//---------------------------------------------------------------------------
// Bypass Switch control
MN_INLINE void gpio_SetBypassSwitch(bool_t value)
{
    //Note negative logic
    if (value)
    {   /* PWM D/A to U/I converter */
        GPIO0->IOCLR = GPIO0_PCON_SW;
    }
    else
    {   /* PWM D/A to pressure controller */
        GPIO0->IOSET = GPIO0_PCON_SW;
    }
}

/** \brief Read I/P closed loop bypass
    \return true = bypas spressure controller , false = pressure controller active
*/
MN_INLINE bool_t gpio_ReadBypassSwitch(void)
{
    //Note negative logic
    return (GPIO0->IOPIN & GPIO0_PCON_SW) == 0;
}

//----------------------------------------------------------------------------------------------------------------------
// LCD and KBD controls

/** \brief Select External LCD controller

    \param  - None.
    \return - Nothing
*/
MN_INLINE void    lcd_SelectExtLCD(void)
{
    GPIO1->IOCLR = GPIO1_CSB_LCD;               /* select LCD controller */
}

/** \brief De-Select External LCD controller

    \param  - None.
    \return - Nothing
*/
MN_INLINE void    lcd_DeSelectExtLCD(void)
{
    GPIO1->IOSET = GPIO1_CSB_LCD;           /* deselect LCD controller and command mode */
}

/** \brief Set Command mode for External LCD

    \param  - None.
    \return - Nothing
*/
MN_INLINE void    lcd_SetCommandMode_ExtLCD(void)
{
    GPIO1->IOCLR = GPIO1_RS_LCD;            /* Set Command mode for External LCD */
}

/** \brief Set Data mode for External LCD

    \param  - None.
    \return - Nothing
*/
MN_INLINE void    lcd_SetDataMode_ExtLCD(void)
{
    GPIO1->IOSET = GPIO1_RS_LCD;            /* Set Data mode for external LCD */
}

/** \brief Check if LCD is selected (means - active)
    \param  - None.
    \return - bool_t - True - selected, False - not so.
*/
MN_INLINE bool_t    lcd_IsLCDSelected(void)
{
    return (0u == (GPIO1->IOPIN & GPIO1_CSB_LCD));
}

//----------------------------------------------------------------------------------------------------------------------
/** \brief Set External Shift Register Clock signal HI
    Used in KEY.C driver

    \param  - None.
    \return - Nothing
*/
MN_INLINE void    lcd_SetKeyShift_HI(void)
{
    GPIO1->IOCLR = GPIO1_RS_LCD;            /* Set Command mode for External LCD */
}

/** \brief Set External Shift Register Clock signal LO
    Used in KEY.C driver

    \param  - None.
    \return - Nothing
*/
MN_INLINE void    lcd_SetKeyShift_LO(void)
{
    GPIO1->IOSET = GPIO1_RS_LCD;            /* Set Data mode for external LCD */
}

//----------------------------------------------------------------------------------------------------------------------

/** \brief Start DI Power
    \param  - None.
    \return - Nothing
*/
MN_INLINE void    key_StartDIPower(void)
{
    GPIO0->IOSET = GPIO0_DI_SUP;
}

/** \brief Stop DI Power
    \param  - None.
    \return - Nothing
*/
MN_INLINE void    key_StopDIPower(void)
{
    GPIO0->IOCLR = GPIO0_DI_SUP;
}

/** \brief Return State of DI pin
    \param  - None.
    \return - true - ACtive, false - passive
*/
MN_INLINE bool_t  key_GetDIState(void)
{
    return (GPIO0_DI == (GPIO0->IOPIN &  GPIO0_DI));
}

//----------------------------------------------------------------------------------------------------------------------

MN_INLINE bool_t gpio_ReadWriteProtectInput(void)
{
    return (GPIO1_WP == (GPIO1->IOPIN &  GPIO1_WP)) ? false : true;
}

//----------------------------------------------------------------------------------------------------------------------

#endif // GPIO_H_

/* End of the source */
