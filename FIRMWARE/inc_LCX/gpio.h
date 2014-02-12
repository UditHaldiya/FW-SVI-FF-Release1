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
 * *****************  Version 3  *****************
 * User: Anatoly Podpaly Date: 1/16/12    Time: 1:35p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_LCX
 * TFS:8575 - removed unused in LCX GPIO.
 *
 * *****************  Version 2  *****************
 * User: Anatoly Podpaly Date: 1/12/12    Time: 4:26p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_LCX
 * TFS:8607 - moved wprotect GPIO.
 *
 * *****************  Version 1  *****************
 * User: Anatoly Podpaly Date: 1/10/12    Time: 4:23p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_LCX
 * TFS:8574 - added uC specific GPIO support.
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

#endif // GPIO_H_

/* End of the source */
