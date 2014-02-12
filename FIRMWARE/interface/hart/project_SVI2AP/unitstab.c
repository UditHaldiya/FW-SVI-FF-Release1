/*
Copyright 2012 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file unitstab.c
    \brief HART units mapping to internal units

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/project_SVI2AP/unitstab.c $
    $Date: 1/23/12 2:08p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup HART
*/
/* $History: unitstab.c $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/23/12    Time: 2:08p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/project_SVI2AP
 * TFS:8762 - HART command for conversion coefficients
*/

#include "mnwrap.h"
#include "hartdef.h"
#include "hartpriv.h"
#include "unitsids.h"

//Note: for HART presentation we always use higher resolution
//(..._ENTRY flavor). We also take advantage of this in that
//a correctly populated entry is never a 0
const UnitsId_t HartUnitsTab[] =
{
    [HPF_Id_Pressure] = UNITSID_PRESSURE_ENTRY,
    [HPF_Id_Signal] = UNITSID_SIGNAL_ENTRY,
    [HPF_Id_Position] = UNITSID_POSITION_ENTRY,
    [HPF_Id_PV] = UNITSID_PV_ENTRY,
    [HPF_Id_Dummy] = UNITSID_DUMMY_ENTRY,
    [HPF_Id_PosSpeed] = UNITSID_MN_POSITION_SPEED_ENTRY,
    [HPF_Id_Percent] = UNITSID_PERCENT_ENTRY,
    [HPF_Id_MNTime] = UNITSID_MNTIME_ENTRY,
    [HPF_Id_Tempr] = UNITSID_TEMPR_ENTRY,
    [HPF_Id_Time] = UNITSID_TIME_ENTRY,
    [HPF_Id_CtlOut] = UNITSID_CTLOUT_ENTRY,
};

const u8 HartUnitsTab_size = NELEM(HartUnitsTab);

/* This line marks the end of the source */
