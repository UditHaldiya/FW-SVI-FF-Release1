/*
Copyright 2008 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file ctllimits.h
    \brief API of Limits for Position Control

     CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/includes/ctllimits.h $
    $Date: 1/06/12 4:19p $
    $Revision: 6 $
    $Author: Arkkhasin $

    \ingroup control
*/
/* (Optional) $History: ctllimits.h $
 *
 * *****************  Version 6  *****************
 * User: Arkkhasin    Date: 1/06/12    Time: 4:19p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/includes
 * TFS:8577 positionsp_range added
 *
 * *****************  Version 5  *****************
 * User: Arkkhasin    Date: 11/04/11   Time: 4:31p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/includes
 * TFS:8072 NVMEM upgrade
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 2/03/11    Time: 7:11p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * TFS:5518 TFS:5160 Setpoint used for tight sutoff is range-limited but
 * not rate-limited; Setpoint used for control is constrained by range
 * values always
 *
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 10/22/10   Time: 2:51a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * TFS:4206 fix - Step 2: (a, also TFS:2514) checksums in rate limits
 * stuff which moved to ctllimits.c (b) removed /some/ compiled-out code
 * (c, FOB TFS:2616) rate limits can be reset to any setpoint
 *
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 2/25/10    Time: 1:15a
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * Back-port from the trunk
 *
 * *****************  Version 6  *****************
 * User: Arkkhasin    Date: 2/11/10    Time: 1:54a
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * Light linting
 *
 * *****************  Version 5  *****************
 * User: Arkkhasin    Date: 1/29/10    Time: 2:46p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * Implemented modular configuration of setpoint rate limits
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 12/14/09   Time: 7:06p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * Setpoint rate limits moved from PIDData_t (and ExtPIDData_t) to
 * CtlLimits_t
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 4/08/08    Time: 6:12p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * New NVMEM strategy:
 * Standard-pattern type-unsafe accessors
 * Old-style accessors removed
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 3/17/08    Time: 4:32p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * Second step toward uniform support of engineering units:
 * Valid range limits for any parameter are owned by a corresponding
 * module and exposed as an array of 2 elements (low and high limits)
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 3/12/08    Time: 11:47a
 * Created in $/MNCB/Dev/FIRMWARE/includes
 * Control limits (setpoint and tight shutoff) moved from
 * ConfigurationData_t to CtlLimits_t.
 * ESD doesn't have those limits nad doesn't know about CtlLimits_t.
 * New source - ctllimits.{c,h} - instead of configure.c now takes care of
 * keeping track of the limits
*/
#ifndef CTLLIMITS_H_
#define CTLLIMITS_H_

//lint -d__SIGNED_INT_MIN__=(-0x7FFFFFFF-1) for now
#include <limits.h>

#include "process.h"
#include "errcodes.h"
#include "param.h"
#include "posctlmode.h"

/** This struct is only of interest to control. It may need to be further
split into tight shutoff and setpoint limits parts.
Historically, it's been derived from ConfigurationData_t (ver. 129)
*/
typedef struct CtlLimits_t
{
    u8 EnableTightShutoff[Xends]; //!< Tight shutoff enable flags
    pos_t TightShutoff[Xends];       //!< Tight shutoff limits in % open
    u8 EnableSetpointLimit[Xends];   //!< Lower and Upper setpoint limits enable flags
    pos_t SetpointLimit[Xends];      //!< Lower and Upper setpoint limit in % open
    u8 EnableSetpointRateLimit[Xends];   //!< Lower and Upper setpoint rate limits enable flags
    pos_t SetpointRateLimit[Xends];      //!< Lower and Upper setpoint rate limit in % travel
    u8 Protected;           //!< a flag indicating whether temporary changes are enabled
    u16 CheckWord;
} CtlLimits_t;

/*
typedef enum ProtectTest_t ProtectTest_t;
enum ProtectTest_t
{
    PROTECTED_LIMITS,
    NO_LIMITS_SET,
    UNPROTECTED_LIMITS
};
*/

//For interface layer (UI and HART)
#define TIGHT_SHUTOFF_MASK 1U
#define TIGHT_SHUTOFF_VALUE_ON TIGHT_SHUTOFF_MASK
#define TIGHT_SHUTOFF_VALUE_OFF 0U

extern const pos_t spposlimits_range[Xends][Xends];
extern const pos_t tso_limits[Xends];
extern const pos_t tco_limits[Xends];
extern const pos_t spratelim_range[Xends];

SAFEGET(control_GetLimits, CtlLimits_t);
SAFESET(control_SetLimits, CtlLimits_t);
//gone static extern ErrorCode_t control_SetTentativeLimits(const CtlLimits_t *lims);
typedef enum ProtectTest_t ProtectTest_t;
enum ProtectTest_t
{
    PROTECTED_LIMITS,
    NO_LIMITS_SET,
    UNPROTECTED_LIMITS
};

extern ProtectTest_t  control_TestProtectLimits(void);
//gone static: extern ErrorCode_t control_TestCtlLimits(const CtlLimits_t *lims);
extern procresult_t control_ProcWithNoLimits(procresult_t (*procf)(s16 *procdetails), s16 *procdetails);

#define SETPOINT_INVALID INT_MIN
#define POS_SETPOINT_LOW_INT INT_PERCENT_OF_RANGE(POS_SETPOINT_LOW)
#define POS_SETPOINT_HIGH_INT INT_PERCENT_OF_RANGE(POS_SETPOINT_HIGH)

/** defines FBO OS Adjust */ /*TFS:4713 - moved SP limits here from hartfunc.h*/
#define POS_SETPOINT_LOW -5.0F
//should be 5% higher than max which is 167 due to open stop adj
//#define POS_SETPOINT_HIGH 160.0F
#define POS_SETPOINT_HIGH 175.0F

extern const pos_t positionsp_range[Xends];

extern void ctllim_Reinit(pos_least_t sp);

extern ctlmode_t ctllim_ConditionControlMode(ctlmode_t ctlmode, s32 *pSetpoint);
extern pos_t ctllim_GetRangeBoundedSetpoint(void);

extern ErrorCode_t cnfg_DisableLimits(void);
extern void cnfg_ReEnableLimits(void);
extern ErrorCode_t control_SetTentativeLimits(const CtlLimits_t *lims);


//---------------- old-style NVMEM interface -------------------
#ifdef OLD_NVRAM
extern void  ctllim_SaveControlLimits(void);
extern void  ctllim_InitControlLimits(InitType_t Type);
#endif

#endif //CTLLIMITS_H_
/* This line marks the end of the source */
