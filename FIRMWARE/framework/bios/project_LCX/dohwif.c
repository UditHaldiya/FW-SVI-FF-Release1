/*
Copyright 2010 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file dohwif.c
    \brief BIOS routines for optional digital outputs

    CPU: Any (with standard C compiler)

    OWNER: AP
    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX/dohwif.c $
    $Date: 11/08/10 6:00p $
    $Revision: 3 $
    $Author: Anatoly Podpaly $

    \ingroup BIOS
*/

/*
    $History: dohwif.c $
 * 
 * *****************  Version 3  *****************
 * User: Anatoly Podpaly Date: 11/08/10   Time: 6:00p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * Bug 4639 - integrated results of code review.
 * 
 * *****************  Version 2  *****************
 * User: Anatoly Podpaly Date: 10/12/10   Time: 2:23p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * Bug 4359 - Added definition for the Platform Hardware. Added
 * conditional compilation based on the type of the Platform H/W.
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 10/06/10   Time: 9:30p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * TFS:4128 Dismantling io.{c,h} 
*/

#include "mnwrap.h"
#include BIOS_DEF_H_
#include "dohwif.h"
#include "lpc2114io.h"
#include "mnassert.h"


/** \brief This function checks DO expamnsion card presense and returns the resukt of the check
    \returns bool_t true - card present, false - no card.
*/
bool_t bios_DOCardPresent(void)
{
#if     PLATFORM_HARDWARE == LCX_REAL_HARDWARE
    bool_t retval;

    if ((GPIO1->IOPIN & GPIO1_TERMINAL_CARD) == 0u)
    {   // 0 - pin is LO, card is present
        retval = true;
    }
    else
    {   // Card is present
        retval = false;
    }

    return retval;
#else
    // Proto H/W - no card - false
    return false;
#endif
}


/** \brief Set digital output 1 or 2 to specified on/off state
    Also see the description of the command 134
    \param[in] u8 u8 param - select DO_1 or DO_2
    \param[in] bool_t value - value: true = closed (FET on), false = open
*/
void bios_SetDigitalOutput(u8 param, bool_t value)
{
#if     PLATFORM_HARDWARE == LCX_REAL_HARDWARE
    bool_t  CardPresent;
    u32     PinID;

    MN_DBG_ASSERT(param <= DO_2);       /* invalid parameter */

    CardPresent = bios_DOCardPresent();
    if (CardPresent)
    {   // Card present - drive DO's
        switch (param)
        {
            case DO_1 :
                PinID = GPIO0_DO_1;     // Pin mask
                break;

            case DO_2 :
                PinID = GPIO0_DO_2;     // Pin mask
                break;

            default :
                PinID = 0u;             // Nothing, pin mask = 0
                break;
        }

        if (value)
        {   // True - Activate DO
            GPIO0->IOSET = PinID;
        }
        else
        {   // False - deactivate DO
            GPIO0->IOCLR = PinID;
        }
    }
    else
    {   // Card is not present - clear the GPIO's
        GPIO0->IOCLR = GPIO0_DO_1 | GPIO0_DO_2;
    }
#else
    UNUSED_OK(param);
    UNUSED_OK(value);
#endif
}

/** \brief Returns the Physical state of the DOs:
    Also see the description of the command 134
    \param[in] u8 u8 param - select DO_1 or DO_2
    \returns bool_t true - DO closed, false - DO open
*/
bool_t  bios_GetDigitalOutput(u8 param)
{
    bool_t  retval;
    bool_t  CardPresent;

    MN_DBG_ASSERT(param <= DO_2);       /* invalid parameter */

    CardPresent = bios_DOCardPresent();
    if (CardPresent)
    {   // Card present - get DO state
#if     PLATFORM_HARDWARE == LCX_REAL_HARDWARE
        u32     PinID;

        switch (param)
        {
            case DO_1 :
                PinID = GPIO0_DO_1;
                break;

            case DO_2 :
                PinID = GPIO0_DO_2;
                break;

            default :
                PinID = 0u;         // Nothing
                break;
        }

        if ((GPIO0->IOPIN & PinID) != 0u)
        {     
            retval = true;
        }
        else
        {
            retval = false;
        }
#else
        // Proto H/W - return false
        retval = false;
#endif
    }
    else
    {   // Card is not present - no DO are set
        retval = false;
    }

    return retval;
}

/* This line marks the end of the source */
