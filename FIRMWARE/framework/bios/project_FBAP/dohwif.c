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
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/framework/bios/project_FFAP/dohwif.c $
    $Date: 1/30/12 12:24p $
    $Revision: 5 $
    $Author: Arkkhasin $

    \ingroup BIOS
*/

/*
    $History: dohwif.c $
 * 
 * *****************  Version 5  *****************
 * User: Arkkhasin    Date: 1/30/12    Time: 12:24p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/framework/bios/project_FFAP
 * Adapted to the number of DO channels FBO TFS:8782
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 12/05/11   Time: 4:21p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/framework/bios/project_SVI2AP
 * TFS:8347 - Restore AP build
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 12/03/10   Time: 12:46a
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_SVI2AP
 * TFS:4922 - Added back DO HW interface
*/

#include "mnwrap.h"
#include BIOS_DEF_H_
#include "dohwif.h"
#include "lpc2114io.h"
#include "mnassert.h"
#include "dimensions.h"


/** \brief Set digital output 1 or 2 to specified on/off state

    \param[in] param - select DO_1 or DO_2
    \param[in] value: true = closed (FET on), false = open
*/
void bios_SetDigitalOutput(u8 param, bool_t value)
{
    switch (param)
    {
#if DO_NUM_CHANNELS > 0
        case DO_1:
            if (true == value)
            {   /* D0_1 115khz output */
                T1->EMR = (T1->EMR | EMR_C0_TOGGLE);
            }
            else
            {   /* DO_1 output freq. stopped */
                T1->EMR = (T1->EMR &  ~(EMR_C0_TOGGLE));
            }
            break;
#endif
#if DO_NUM_CHANNELS > 1
        case DO_2:
            if (true == value)
            {   /* D0_2 115khz output */
                T1->EMR = (T1->EMR | EMR_C1_TOGGLE);
            }
            else
            { /* DO_2 output freq. stopped */
                T1->EMR = (T1->EMR &  ~(EMR_C1_TOGGLE));
            }
            break;
#endif

        default:
            break;

    }
}

/** \brief This function checks DO expansion card presense and returns the resukt of the check
    \returns bool_t true - card present, false - no card.
*/
bool_t bios_DOCardPresent(void)
{
    return true; //always present
}
/* This line marks the end of the source */
