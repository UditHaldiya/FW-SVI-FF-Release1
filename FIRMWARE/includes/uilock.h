/**
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file uipublic.h
    \brief Public interfaces of the LCD/pushbuttons UI component

    Requires <cpu>types.h included before

    CPU: ANY

    OWNER: AK
    \n RCS: $Id: $
     $DateTime: $ $Change: $ $Author: Arkkhasin $
*/

#ifndef UILOCK_H_
#define UILOCK_H_

//--------------------------------------------------------
// Control for the feautre

#define     UI_LOCK_FEATURE_ON          (1)
#define     UI_LOCK_FEATURE_OFF         (0)
#define     UI_LOCK_FEATURE             (UI_LOCK_FEATURE_OFF)

//--------------------------------------------------------
// LUI Lock structure and definitions

#define     LUI_STATE_UNLOCKED      (0u)
#define     LUI_STATE_LOCKED        (1u)
#define     LUI_STATE_LOCK_MAX      (LUI_STATE_LOCKED + 1u)

typedef struct UI_LockData_t
{
    u16     Password;
    bool_t  PasswordEnabled;
    u8      LockState;
    u16     CheckWord;

} UI_LockData_t;

//!< API functions
#if UI_LOCK_FEATURE == UI_LOCK_FEATURE_OFF
#define UI_IsLUILocked() false
#define UI_PeriodicLockData_Test() /*nothing*/
#else
#define     LUI_STATE_UNLOCKED      (0u)
#define     LUI_STATE_LOCKED        (1u)
#define     LUI_STATE_LOCK_MAX      (LUI_STATE_LOCKED + 1u)

typedef struct UI_LockData_t
{
    u16     Password;
    bool_t  PasswordEnabled;
    u8      LockState;
    u16     CheckWord;

} UI_LockData_t;


bool_t          UI_IsLUILocked(void);
ErrorCode_t     UI_SetLockData(UI_LockData_t const *pLockData);
void            UI_GetLockData(UI_LockData_t       *pLockData);

//!< API - Periodic Structure Test
void            UI_PeriodicLockData_Test(void);

//!< NVRAM interface
void            UI_Save_LockData(void);
void            UI_Init_LockData(InitType_t Type);
#endif //UI_LOCK_FEATURE == UI_LOCK_FEATURE_OFF
#endif //UILOCK_H_
//--------------------------------------------------------
/* This line marks the end of the source */
