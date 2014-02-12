/**
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file uimain.c
    \brief Implementation of MNCB LCD UI.

    CPU: Any

    OWNER: AK
    \n RCS: $Id: $
     $DateTime: $ $Change: $ $Author: Anatoly Podpaly $
*/

/*
    $History: uilock.c $
 * 
 * *****************  Version 3  *****************
 * User: Anatoly Podpaly Date: 1/20/11    Time: 6:02p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * TFS:5440 -- added conditional compilation to disable teh UI Lock
 * featuire.
 * 
 * *****************  Version 2  *****************
 * User: Anatoly Podpaly Date: 12/09/10   Time: 3:19p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Bug 4811 - Added reset of teh state UI machine on Lock change.
 * 
 * *****************  Version 1  *****************
 * User: Anatoly Podpaly Date: 10/01/10   Time: 11:15a
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Implementation of UI Lock.
*/

#include "config.h"
#include "mnwrap.h" //for MN_NOINIT
#include "oswrap.h"
#include "mnassert.h"
#include "mncbtypes.h" //for button lock business
#include "errcodes.h"
#include "configure.h"
#include "devicemode.h"
#include "control.h"
#include "bitutils.h"
#include "bios.h"
#include "nvram.h"
#include "uipublic.h"
#include "uiprivate.h"
#include "uilock.h"


//-------------------------------------------------------------------
/* This file implements the UI LOCK support requirements */
/** REQ 31-045 */
static UI_LockData_t           m_UI_LockData;

#define CHECK_LOCK_STATE_IN_RANGE(State)    ((State) <  LUI_STATE_LOCK_MAX)

#if UI_LOCK_FEATURE == UI_LOCK_FEATURE_ON
// Use this only if the feature is enabled
#define IS_UI_STATE_LOCKED(State)           ((State) == LUI_STATE_LOCKED)
#endif

//------------------------------------------------------------------
// NVRAM supporting functionality - LUI Lock support
/** \brief Local Function - Reset the LUI Lock data
*/
static void ResetLockData(void)
{
    m_UI_LockData.Password        = 0;
    m_UI_LockData.PasswordEnabled = false;
    m_UI_LockData.LockState       = LUI_STATE_UNLOCKED;
    STRUCT_CLOSE(UI_LockData_t, &m_UI_LockData);
}

/** \brief LCX UI Periodic Data Structure Test
*/
void    UI_PeriodicLockData_Test(void)
{
    // Test Lock structure
    Struct_Test(UI_LockData_t, &m_UI_LockData);
}

//------------------------------------------------------------------
// Extrenal API

/** \brief LCX UI Check if LUI is locked
    \return bool_t  true - if in Locked
*/
bool_t   UI_IsLUILocked(void)
{
#if UI_LOCK_FEATURE == UI_LOCK_FEATURE_OFF
    // TFS:5440 - always disable feature
    return false;
#else
    return (IS_UI_STATE_LOCKED(m_UI_LockData.LockState));
#endif
}

/** \brief UI Function - initializes the LUI parameters from NVRAM or to default
  \parameter[in] InitType_t Init type : from NVRAM or to default
*/
void UI_Init_LockData(InitType_t Type)
{
	// This function is externally called to initialize tune data
	// If input Type== INIT_FROM_NVRAM, it copies the tune data from NVRAM to RAM
	// Otherwise it sets the data to default values
	// parameters description
	// InitType_t Type: input value to determine to copy or initialized tune data.
    MN_ASSERT(!oswrap_IsOSRunning()); //and no FRAM mutex

    if (Type == INIT_FROM_NVRAM)
    {
        /** read fram to get the data */
        nvram2ram(&m_UI_LockData, NVRAMID_UI_LockData);
    }
    else
    {   //Type == INIT_TO_DEFAULT
        ResetLockData();
    }
}

/** \brief UI Function - saves the LUI parameters in NVRAM
*/
void UI_Save_LockData(void)
{
    MN_FRAM_ACQUIRE();
        // write the data to fram and release fram
        ram2nvram(&m_UI_LockData, NVRAMID_UI_LockData);
        cnfg_SetFRAMConfigurationChangedFlag(true);

    MN_FRAM_RELEASE();
}

/** \brief UI Interface - stores the New Password
\param password - a (user-supplied) password
\return an error code (ERR_OK if access is granted)
*/
ErrorCode_t UI_SetLockData(UI_LockData_t const *pLockData)
{
    ErrorCode_t err;

    MN_ASSERT(pLockData != NULL);

    if (CHECK_LOCK_STATE_IN_RANGE(pLockData->LockState))
    {   // New state is in range
        // Check if the UI Lock changes
        if (  (m_UI_LockData.PasswordEnabled  && (!pLockData->PasswordEnabled)) ||
            ((!m_UI_LockData.PasswordEnabled) &&   pLockData->PasswordEnabled) )
        {   // Reset the state machine to idle
            Reset_UI_StateMachineIdle();
        }

        MN_ENTER_CRITICAL();
            storeMemberInt(&m_UI_LockData, Password,        pLockData->Password);
            storeMemberInt(&m_UI_LockData, PasswordEnabled, pLockData->PasswordEnabled);
            storeMemberInt(&m_UI_LockData, LockState,       pLockData->LockState);
        MN_EXIT_CRITICAL();

        UI_Save_LockData();         // Store data

        err = ERR_OK;               // Return success
    }
    else
    {   // New Lock state is not valid
        err = ERR_INVALID_PARAMETER;
    }

    return err;
}

/** \brief UI Interface - gets the Lock data to teh caller (from the local var.
\param [out] ptr to the structure.
*/
void UI_GetLockData(UI_LockData_t *pLockData)
{
    MN_ASSERT(pLockData != NULL);

    pLockData->Password        = m_UI_LockData.Password;
    pLockData->PasswordEnabled = m_UI_LockData.PasswordEnabled;
    pLockData->LockState       = m_UI_LockData.LockState;   
}

/* This line marks the end of the source */
//------------------------------------------------------------------
