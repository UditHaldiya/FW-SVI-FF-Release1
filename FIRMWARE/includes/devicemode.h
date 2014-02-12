/*
Copyright 2004-2007 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file devicemode.h
    \brief public interfaces of device mode manager

     CPU: Any

    OWNER: AK
    OWNER: AK
    $Archive: /MNCB/Dev/FIRMWARE/includes/devicemode.h $
    $Date: 12/14/09 7:08p $
    $Revision: 44 $
    $Author: Arkkhasin $

    \ingroup Modeman
*/
/* (Optional) $History: devicemode.h $
 *
 * *****************  Version 44  *****************
 * User: Arkkhasin    Date: 12/14/09   Time: 7:08p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * Reordered typedefs and #include so that it would compile
 *
 * *****************  Version 43  *****************
 * User: Arkkhasin    Date: 5/05/09    Time: 4:38p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * Eliminated devemodewrap.h
 * mode_Get... declared pure
 *
 * *****************  Version 42  *****************
 * User: Arkkhasin    Date: 3/07/09    Time: 12:32p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * Defined MODE_NONE and MODE_ALL for convenience
 *
 * *****************  Version 41  *****************
 * User: Arkkhasin    Date: 1/13/09    Time: 3:37p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * Lint (type annoyance)
 *
 * *****************  Version 40  *****************
 * User: Arkkhasin    Date: 1/11/09    Time: 8:05p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * Improved internal representation of device modes
 * Device mode is now formatted for HART independently from internal
 * representation
 * Removed mode_Correct()
 * Added NVMEM interface for mode
 *
 * *****************  Version 39  *****************
 * User: Arkkhasin    Date: 4/24/08    Time: 12:14p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * mode_TransitionHook() moved from devicemode.c to okctlmodes.c (too
 * project-dependent)
 * SUBMODE_LOWPOWER value corrected
 * posctlmode.h extracted from devicemode.h - this may or may not stick
 *
 * *****************  Version 38  *****************
 * User: Arkkhasin    Date: 3/05/08    Time: 6:23p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * mode_{Set,Get}Mode() API and new mode_GetSubmode() API replace old
 * mode-centered API
 * Removed mode_Mopup() - last version did nothing at all
 * modes and submodes renamed to more appropriate names
 * Removed ancient fluff
 *
 * *****************  Version 37  *****************
 * User: Arkkhasin    Date: 2/25/08    Time: 3:01p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * Went back from error_ClearLowPower() to mode_ClearLowPower() because of
 * UI notification
 *
 * *****************  Version 36  *****************
 * User: Arkkhasin    Date: 2/15/08    Time: 4:49p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * Added tentative (?) aliases mode_Is...
 *
 * *****************  Version 35  *****************
 * User: Arkkhasin    Date: 1/25/08    Time: 9:23a
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * Lint (Init removed)
 *
 * *****************  Version 34  *****************
 * User: Arkkhasin    Date: 11/16/07   Time: 1:45p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * AP part of table-based process configuration
 *
 * *****************  Version 33  *****************
 * User: Arkkhasin    Date: 8/07/07    Time: 4:06p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * Removed deprecated control_SetPositionSP (and mode_SetSetpoint along
 * the way)
 *
 * *****************  Version 32  *****************
 * User: Arkkhasin    Date: 8/07/07    Time: 2:31p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * 1. Removed dangerous control_SetControlMode API
 * 2. Removed shaky control_GetEffectiveControlMode in favor of new
 * mode_GetEffectiveControlMode
 * 3. control now asks for effective control mode; there is no way to
 * force-feed it to control.
 * 4. A bug in ramp_SetSP (using raw instead of effective control mode) is
 * fixed.
 *
 * *****************  Version 31  *****************
 * User: Arkkhasin    Date: 5/18/07    Time: 5:39p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * New tombstone header
*/
#ifndef DEVICEMODE_H_
#define DEVICEMODE_H_
#include <limits.h>
#include "errcodes.h"
/*-------------- device mode types ---------------*/
//AK: future lint -strong(AJX, devmode_t)
typedef u8 devmode_t; //! MSByte=mode LSB=submode
typedef u8 devsubmode_t; //! A comatibility alias

#include "posctlmode.h"
#include "faultdef.h" //for users of fwords

typedef enum devmodebit_t
{
    MODEBIT_FAILSAFE,
    MODEBIT_SETUP,
    MODEBIT_MANUAL,
    MODEBIT_OPERATE,
    MODEBIT_TOTAL //must be the last
} devmodebit_t;

#define MODEMAP(modebit) ((devmode_t)(1U<<(u32)(modebit)))

#define MODE_SETUP        MODEMAP(MODEBIT_SETUP)
#define MODE_MANUAL       MODEMAP(MODEBIT_MANUAL)
#define MODE_OPERATE      MODEMAP(MODEBIT_OPERATE)
#define MODE_FAILSAFE     MODEMAP(MODEBIT_FAILSAFE)
#define MODE_INVALID      ((devmode_t)0x00) //! No special meaning

//BIG NOTE: Submode is a bitwise OR of (some of) those flag bits!
#define SUBMODE_NORMAL   ((devsubmode_t)0x0000)
#define SUBMODE_LOWPOWER ((devsubmode_t)0x00010)
#ifdef FAULT_MARGINAL_POWER
  #define SUBMODE_MARGINAL_POWER ((devsubmode_t)0x0080U) //! an artificial indicator of marginal power for ValVue
#endif
#define SUBMODE_FAILED_HIGH ((devsubmode_t)0x0040U) //! an artificial indicator of failhigh faults for ValVue
#ifdef FAULT_ESD_TRIP
  #define SUBMODE_ESD_TRIP ((devsubmode_t)0x0020U) //! an artificial indicator of ESD trip faults for ValVue
#endif
//lint -sem(mode_GetMode, pure)
extern devmode_t mode_GetMode(void);
//lint -sem(mode_GetSubmode, pure)
extern devsubmode_t mode_GetSubmode(void);

extern ErrorCode_t mode_SetMode(devmode_t dmode); //discouraged for setting normal mode

MN_DECLARE_API_FUNC(mode_SetModeEx) //Not all projects requre temporary mode change
extern ErrorCode_t mode_SetModeEx(devmode_t dmode, bool_t save);

extern ErrorCode_t mode_SetNormalMode(void);

extern void mode_ClearLowPower(void);

//Implementation is project-dependent so Doxygen header is here at the point of declaration
/** \brief A wrapper function encapsulating actions required on device mode change
MUST be called from within a critical section.
\param newmode - new device mode
\return true iff mode change is allowed
*/
extern bool_t mode_TransitionHook(devmode_t newmode);

typedef struct ModeData_t
{
    devmode_t mode; //!< the mode of the device
    devmode_t tmode; //!< the mode to return to after unusual conditions go away
    u16 CheckWord;
} ModeData_t;

//Target mode save business
SAFEGET(mode_GetModeData, ModeData_t);
//Temporary compatibiilty
#define TypeUnsafe_mode_InitTargetMode TypeUnsafe_mode_SetModeData
#define TypeUnsafe_mode_GetTargetMode TypeUnsafe_mode_GetModeData

//But do not provide safe wrapper for "Set" - it is needed only to "commit" data to NVMEM
extern ErrorCode_t TypeUnsafe_mode_SetModeData(const void *src);

extern void mode_Mopup(void);

/** \brief Returns effective device mode, backward-compatible
\param mode - local device mode
\return effective device mode
*/
extern devmode_t mode_GetEffectiveMode(devmode_t mode);

#endif //DEVICEMODE_H_
/* This line marks the end of the source */
