/*
Copyright 2008 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file signalsp.h
    \brief API for conversion of input signal to position setpoint w.r.t.
            split range and characterization.

    CPU: Any

    OWNER: AK

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/includes/signalsp.h $
    $Date: 11/29/11 2:09p $
    $Revision: 2 $
    $Author: Arkkhasin $

    \ingroup posctl
*/
/* $History: signalsp.h $
 * 
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 11/29/11   Time: 2:09p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/includes
 * TFS:8313 Lint
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 11/15/11   Time: 10:57a
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/includes
 * Preliminary check-in for
 * TFS:8202 decouple I/O subsystem
 * Not yet ready to package as a module
 *
 * *****************  Version 5  *****************
 * User: Arkkhasin    Date: 7/05/08    Time: 1:33a
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * sigsp_ConvertSignalToSetpoint yields setpoint in "hard stops" domain
 * (instead of previous convert_... which was aware of ATO/ATC and stops
 * adjustments)
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 4/08/08    Time: 6:18p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * New NVMEM strategy:
 * Standard-pattern type-unsafe accessors
 * Old-style accessors removed
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 3/17/08    Time: 5:07p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * Second step toward uniform support of engineering units:
 * Valid range limits for any parameter are owned by a corresponding
 * module and exposed as an array of 2 elements (low and high limits)
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 2/25/08    Time: 3:08p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * signalsp(_min).h goes by its symbolic name SIGNALSP_H_
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 2/01/08    Time: 6:39p
 * Created in $/MNCB/Dev/FIRMWARE/includes
 * Consolidated API for signal to position setpoint conversion
*/

#ifndef SIGNALSP_G_
#define SIGNALSP_G_
#ifdef SIGNALSP_H_

#include "errcodes.h"
#include "signalsp_min.h"

#define LOW_SIGNAL_LOW_LIMIT 3.8F //! Low limit for low signal range value, mA
#define LOW_SIGNAL_HIGH_LIMIT 14.0F //! High limit for low signal range value, mA
#define HIGH_SIGNAL_LOW_LIMIT 8.0F //! Low limit for high signal range value, mA
#define HIGH_SIGNAL_HIGH_LIMIT 20.2F //! High limit for high signal range value, mA
#define SIGNAL_DIFFERENCE 5.0F //! Min signal range, mA

/*lint -emacro((912,917), SIGNAL_DIFFERENCE_INT,LOW_SIGNAL_LOW_LIMIT_INT,LOW_SIGNAL_HIGH_LIMIT_INT)
  -emacro((912,917), HIGH_SIGNAL_LOW_LIMIT_INT,HIGH_SIGNAL_HIGH_LIMIT_INT)
  MISRA 2004 allows float and integer (same signedness) type promotions
*/
#define SIGNAL_DIFFERENCE_INT ((s16)((SIGNAL_DIFFERENCE*SIGNAL_TO_COUNTS_SCALE)+0.5)) //5000, between low and high
#define LOW_SIGNAL_LOW_LIMIT_INT ((s16)((LOW_SIGNAL_LOW_LIMIT*SIGNAL_TO_COUNTS_SCALE)+0.5)) //3800, between low and high
#define LOW_SIGNAL_HIGH_LIMIT_INT ((s16)((LOW_SIGNAL_HIGH_LIMIT*SIGNAL_TO_COUNTS_SCALE)+0.5)) //8000, between low and high
#define HIGH_SIGNAL_LOW_LIMIT_INT ((s16)((HIGH_SIGNAL_LOW_LIMIT*SIGNAL_TO_COUNTS_SCALE)+0.5)) //14000, between low and high
#define HIGH_SIGNAL_HIGH_LIMIT_INT ((s16)((HIGH_SIGNAL_HIGH_LIMIT*SIGNAL_TO_COUNTS_SCALE)+0.5)) //20200, between low and high


typedef struct SignalData_t
{
    sig_t LowSignal; //!< signal LRV
    sig_t HighSignal; //!< signal URV
    u16 CheckWord;
} SignalData_t;			

//LS 12/20/04 changed slopes to scale/offsets
typedef struct ComputedSignalData_t
{
    s32  SlopeStandard; //!< scaling factor to convert signal to percent of range
    sig_t   nLowSignal; //!< a copy of signal LRV
    sig_t   nHighSignal; //!< a copy of signal URV
    u16 CheckWord;
} ComputedSignalData_t;  //! The type of internal data for signal conversion


extern s32 sigsp_ConvertSignalToSetpoint(sig_t n2Signal);

SAFESET(cnfg_SetSignalData, SignalData_t);
SAFEGET(cnfg_GetSignalData, SignalData_t);

extern const ComputedSignalData_t *sig_GetComputedSignalData(void);

extern const sig_t SignalLimits_range[Xends][Xends];
extern const sig_t SignalSP_range[Xends];

#endif //SIGNALSP_H_
#endif //SIGNALSP_G_
/* This line marks the end of the source */
