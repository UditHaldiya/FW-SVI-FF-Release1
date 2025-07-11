/*
Copyright 2004-2006 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/** \addtogroup Control Compute outputs and output them
\brief Position retransmit via AO0
\htmlonly
<a href="../../../Doc/DesignDocs/Physical Design_Cycle Module.doc"> Design document </a><br>
<a href="../../../Doc/TestDocs/utplan_cycle.doc"> Unit Test Report </a><br>
\endhtmlonly
*/
/**
    \file posretran.c
    \brief Position retransmit via AO0

    CPU: Any

    OWNER: AK

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/tasks/control/posretran.c $
    $Date: 1/12/12 12:46p $
    $Revision: 4 $
    $Author: Arkkhasin $

    \ingroup Control
*/
/* $History: posretran.c $
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 1/12/12    Time: 12:46p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/tasks/control
 * TFS:8674 - added pos retrnasmit
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 4/15/09    Time: 2:22p
 * Updated in $/MNCB/Dev/FIRMWARE/posctl
 * Header refactoring (for DLT lint)
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 4/15/09    Time: 1:07a
 * Created in $/MNCB/Dev/FIRMWARE/posctl
 * MNCB version of position retransmit (extracted from cycle.c)
*/
#include "config.h"
#include "mnwrap.h"
#include "ao.h"
#include "position.h"
#include "posretran.h"
#include "configure.h"

// Local option (moved from config.h since not used by anyone else)
#define POS_RETRANSMIT_FILTERED OPTION_ON //use filtered position?

/** \brief Call the position retransmit handler passing it the position
*/
void periodicjob_PositionRetransmit(void)
{
    /* only do this if position transmit is enabled */
    if(!cnfg_GetOptionConfigFlag(POSITION_RETRANSMIT_OPTION))
    {
        return; /* Legitimate early return: Pre-requisites check */
    }

    s16_least pos;
#if POS_RETRANSMIT_FILTERED == OPTION_OFF
    pos = vpos_GetScaledPosition();
#endif
#if POS_RETRANSMIT_FILTERED == OPTION_ON
    pos = vpos_GetSmoothedScaledPosition();
#endif
    ao_SetPositionRetransmit(AO_CHANNEL_POS_RETRANSMIT, pos);
}

/* This line marks the end of the source */
