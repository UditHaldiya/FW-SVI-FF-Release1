#MP If 0 ;Tombstone header
/*
Copyright 2008 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file unitsdef.inc
    \brief Definitions of engineering units ids - AP version.

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/inc_SVI2AP/unitsdefs.inc $
    $Date: 1/23/12 2:19p $
    $Revision: 2 $
    $Author: Arkkhasin $

    \ingroup proj
*/
/* $History: unitsdefs.inc $
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 1/23/12    Time: 2:19p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_SVI2AP
 * Exposed FP PWM normalization FBO TFS:8762,8753 (Units Id)
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/06/12    Time: 12:06a
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_SVI2AP
 * TFS:8370 - AP units table
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 11/17/09   Time: 6:08p
 * Updated in $/MNCB/Dev/FIRMWARE/inc_SVI2AP
 * Units definition constants should be brought here (via C headers)
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 2/09/09    Time: 6:12p
 * Updated in $/MNCB/Dev/FIRMWARE/inc_SVI2AP
 * Defined UNITSID_TIME/UNITS_SEC
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 10/17/08   Time: 7:07p
 * Updated in $/MNCB/Dev/FIRMWARE/inc_SVI2AP
 * Removed AO units. Added TEMPR units
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 6/13/08    Time: 12:32a
 * Created in $/MNCB/Dev/FIRMWARE/inc_SVI2AP
 * Engineering units definitions (AP)
#MP Endif ;Tombstone header

#MP ;------------------ User-editable part ---------------------
#MP ;Put here all C headers which define the necessary stuff for the units
#include "pressuredef.h"
#include "signaldef.h"
#include "percent.h"
#include "position.h"
#include "inpv.h"
#include "param.h"
#include "timedef.h"
#include "controldef.h"
#include "tempr.h"

    #MP Units[PRESSURE, 1, PSI, BAR, KPA]
    #MP Units[SIGNAL, 0, mA]
    #MP Units[POSITION, 0, PERCENT]
    #MP Units[PV, 0, VOLT]
    #MP Units[DUMMY, 0, NOT_USED] ;do not change (yet) - a handy entry for unused stuff; UI displays as int
    #MP Units[MN_POSITION_SPEED, 0, PERCENT_PER_SEC] ;MN proprietary format (internal: %/4sec); not used (yet?) for floating point
    #MP Units[PERCENT, 0, PUREPERCENT]
    #MP Units[MNTIME, 0, MNSEC]
    #MP Units[TEMPR, 1, DEGC, DEGF]
    #MP Units[TIME, 0, SEC]
    #MP Units[CTLOUT, 0, CTLPSI]
    #MP Units[SHEDTIME, 0, SHEDTIME_SEC]
#MP ;---------------- End User-editable part -------------------
#MP ;/* This line marks the end of the source */
