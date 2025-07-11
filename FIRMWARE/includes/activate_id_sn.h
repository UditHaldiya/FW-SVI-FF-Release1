/*
Copyright 2010 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file activate_id_sn.h
    \brief API for Device activation/ Signature verification support based on device id and serial number

    CPU: Any

    OWNER: AK

    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes/activate_id_sn.h $
    $Date: 9/25/10 3:57p $
    $Revision: 3 $
    $Author: Arkkhasin $

    \ingroup Process
*/
/* $History: activate_id_sn.h $
 * 
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 9/25/10    Time: 3:57p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * TFS:4159 - Added time length of activation calculation
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 8/09/10    Time: 10:10p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * Added  actver_GetKeyArtifact() interface; primarily for HART
 *
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 7/18/10    Time: 1:56p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * Device activation/ Signature verification support based on device id
 * and serial number
*/

#ifndef ACTIVATE_ID_SN_H_
#define ACTIVATE_ID_SN_H_

#include "activate.h" //Required public API to implement

/* The rest is specific to implementation of an activation scheme. That
means, the header might be private to the implementation, EXCEPT that
we need to publish NVRAM interface
*/

typedef struct ActivateVerify_t
{
    u64 signature;
    //u64 padding;
} ActivateVerify_t; //! verification is f(signature)

enum  //! Available artifacts
{
    SIGNATURE_SERIALNUM,
    SIGNATURE_DEVICEID,
    SIGNATURE_COUNT
};

typedef struct ActivateVerifyData_t
{
    ActivateVerify_t avdata[SIGNATURE_COUNT];
    u8 k[SIGNATURE_COUNT]; //!< exponent e=2**k+1; maybe only k==1 will be supported
    u16 CheckWord;
} ActivateVerifyData_t;

extern u64 actver_GetKeyArtifact(u8_least index);

SAFEGET(actver_GetData, ActivateVerifyData_t);
SAFESET(actver_SetData, ActivateVerifyData_t);

#ifdef OLD_NVRAM
extern void actver_InitData(InitType_t itype);
extern void actver_SaveData(void);

#endif //OLD_NVRAM

extern tick_t actver_GetAndClearLength(void);

#endif //ACTIVATE_ID_SN_H_

/* This line marks the end of the source */
