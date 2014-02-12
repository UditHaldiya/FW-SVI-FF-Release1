/*
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file sysiolopwr.h
    \brief Public interfaces of the Low Power handling

    CPU: Any

    $Revision: 4 $
    OWNER: AP
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/includes/sysiolopwr.h $
    $Date: 1/30/12 12:56p $
    $Revision: 4 $
    $Author: Arkkhasin $

*/
/* $History: sysiolopwr.h $
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 1/30/12    Time: 12:56p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/includes
 * Removed control dependencies on loop signal and signal setpoint FBO
 * TFS:8782
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 11/04/11   Time: 4:50p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/includes
 * TFS:8072 NVMEM upgrade
 *
 * *****************  Version 1  *****************
 * User: Anatoly Podpaly Date: 2/16/11    Time: 5:15p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * Low Power handling Headers.
 *
*/
#ifndef SYSIOLOPWR_H_
#define SYSIOLOPWR_H_

typedef struct LowPowerData_t
{
    u16 uiHysteresis;
    u16 uiLo;
    u16 uiMarginal;
    u16 uiLoLo;
    s16 nPowerCoef;
    s16 nPressureCoef;
    u16 CheckWord;
} LowPowerData_t;


/** Public functions
 *
*/

extern bool_t sysio_CheckIPDisconnect(void);

ErrorCode_t sysio_SetLowPowerData(const LowPowerData_t* pLowPowerData);
const LowPowerData_t* sysio_GetLowPwrData(LowPowerData_t *dst);
UNTYPEACC(sysio_SetLowPowerData, sysio_GetLowPwrData);

extern void sysio_Handle_LowPowerLimiting(void);
extern u16 sysio_GetPWMHightLimit(void);

#endif // SYSIOLOPWR_H_
/* This line marks the end of the source */
