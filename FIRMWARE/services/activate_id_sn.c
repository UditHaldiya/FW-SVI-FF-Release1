/*
Copyright 2010 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file activate_id_sn.c
    \brief Device activation/ Signature verification support based on device id and serial number

    CPU: Any

    OWNER: AK

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/services/activate_id_sn.c $
    $Date: 11/29/11 2:18p $
    $Revision: 14 $
    $Author: Arkkhasin $

    \ingroup Process
*/
/* $History: activate_id_sn.c $
 *
 * *****************  Version 14  *****************
 * User: Arkkhasin    Date: 11/29/11   Time: 2:18p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/services
 * TFS:8313 Lint
 *
 * *****************  Version 12  *****************
 * User: Arkkhasin    Date: 3/11/11    Time: 1:37a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * TFS:5841 Missing fram mutex sandwich; Don't save RAM to FRAM  on init
 *
 * *****************  Version 11  *****************
 * User: Arkkhasin    Date: 1/10/11    Time: 2:49p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * TFS:5299 Step 1 - Implemented fault AUX_MEM0
 *
 * *****************  Version 10  *****************
 * User: Arkkhasin    Date: 11/19/10   Time: 4:41p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * Disabled activation on all builds for the prototype HW (No TFS -
 * Justin's request)
 *
 * *****************  Version 9  *****************
 * User: Arkkhasin    Date: 10/28/10   Time: 2:43p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * Enabled activation test on target HW
 *
 * *****************  Version 8  *****************
 * User: Arkkhasin    Date: 10/12/10   Time: 7:09p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * TFS:3996 (Code review items)
 *
 * *****************  Version 7  *****************
 * User: Arkkhasin    Date: 10/12/10   Time: 3:53p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * TFS:3838 - now *NO* activation code is good if a SN chip is missing
 * TFS:3891 - rely on the SN driver to do the right clocking
 *
 * *****************  Version 6  *****************
 * User: Arkkhasin    Date: 9/25/10    Time: 3:57p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * TFS:4159 - Added time length of activation calculation
 *
 * *****************  Version 5  *****************
 * User: Arkkhasin    Date: 9/14/10    Time: 8:03p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * (Partially) commonized API of hart_GetAndTestHartData (for the benefit
 * of TFS 4067)
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 8/13/10    Time: 5:20p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * Periheral clock speed hack to accommodate serial number chip API
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 8/12/10    Time: 2:29p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * Temporarily removed activation test from Release build
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 8/09/10    Time: 10:15p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * First realistic implementation of signing
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 7/18/10    Time: 2:00p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * First mock-up implementation of Device activation/ Signature
 * verification support based on device id and serial number (First
 * activation code is considered signature of the second one)
*/

#include <limits.h>
#include "mnwrap.h"
#include "oswrap.h"
#include "errcodes.h"
#include "mnassert.h"
#include "mntrapdef.h"
#include "nvram.h"
#include "crc.h"
#include "activate_id_sn.h"
#include "cryptutils.h"
#include "devicemode.h"
#include "hart.h"
#include "sernumchip.h"
#include "timer.h"
#include "faultpublic.h"

#define SERIAL_NUM_TRIES 3 //RED_FLAG move elsewhere (sernumchip.h?)

static ActivateVerifyData_t ActivateVerifyData;

typedef struct serialnum_t
{
    u64 sn; //!< serial number from hardware
    SerialNumberError_t err; //!< error code reading from hardware
    u16 CheckWord;
} serialnum_t;

static serialnum_t serialnum;

//------------ Standard set/get fare ----------------------

const void *TypeUnsafe_actver_GetData(void *dst)
{
    return STRUCT_TESTGET(&ActivateVerifyData, dst);
}

ErrorCode_t TypeUnsafe_actver_SetData(const void *src)
{
    const ActivateVerifyData_t *p = src;
    ActivateVerifyData_t temp;
    if(p==NULL)
    {
        for(s8_least i=0; i<SIGNATURE_COUNT; i++)
        {
            temp.avdata[i].signature = 0ULL;
            temp.k[i] = 1;
        }
        p = &temp;
    }

    //For now, only k is checked
    for(s8_least i=0; i<SIGNATURE_COUNT; i++)
    {
        if(p->k[i] > 4) //Otherwise 2**(2**k) + 1 is not prime
        {
            return ERR_INVALID_PARAMETER;
        }
    }

    if(!oswrap_IsOSRunning())
    {
        //On initialization, read the serial number chip
        SerialNumberError_t err;
        for(u8_least i=0; i<SERIAL_NUM_TRIES; i++)
        {
            err = bios_ReadSerialization(&serialnum.sn);
            if(err == SERIALNUMBER_OK)
            {
                break;
            }
        }
        serialnum.err = err;
        STRUCT_CLOSE(serialnum_t, &serialnum);
#ifdef FAULT_AUX_MEM0
        if(err != SERIALNUMBER_OK)
        {
            error_SetFault(FAULT_AUX_MEM0);
        }
#endif
    }

    Struct_Copy(ActivateVerifyData_t, &ActivateVerifyData, p);
#ifdef OLD_NVRAM
//Backward compatibility
    if(oswrap_IsOSRunning())
    {
        actver_SaveData();
    }
#endif
    return ram2nvramAtomic(NVRAMID_ActivateVerifyData);
}

#ifdef OLD_NVRAM
//Backward compatibility shims
void actver_InitData(InitType_t itype)
{
    const ActivateVerifyData_t *p;
    ActivateVerifyData_t temp;
    if(itype == INIT_FROM_NVRAM)
    {
        nvram2ram(&temp, NVRAMID_ActivateVerifyData);
        p = &temp;
    }
    else
    {
        p = NULL; //for default init case
    }
    (void)actver_SetData(p);
}

void actver_SaveData(void)
{
    MN_FRAM_ACQUIRE();
        ram2nvram(&ActivateVerifyData, NVRAMID_ActivateVerifyData);
    MN_FRAM_RELEASE();
}
#endif //OLD_NVRAM

/** \brief Reports what to activate
\param index - one of the predefined indexes of activation artifacts
\return the artifact to sign (activate) or all 1's if the index is invalid
*/
u64 actver_GetKeyArtifact(u8_least index)
{
    u64 key;
    const u8 *p;
    switch(index)
    {
        case SIGNATURE_SERIALNUM:
            Struct_Test(serialnum_t, &serialnum);
            if(serialnum.err != SERIALNUMBER_OK)
            {
                key = (~0ULL) ^ 0xFFULL ^ (u64)serialnum.err; //guaranteed not to match authentication (>modulus)
            }
            else
            {
                key = serialnum.sn;
            }
            break;
        case SIGNATURE_DEVICEID:
            //get device id:
            p = hart_GetAndTestHartData(NULL)->device_id;
            u32 id;
            MN_ENTER_CRITICAL();
                id = p[0] | (p[1]<<CHAR_BIT) | (p[2]<<(2*CHAR_BIT));
            MN_EXIT_CRITICAL();
            key = ((u64)mnhash(&id, 3, CRC_SEED)) | (((u64)id)<<32); //guaranteed < smallest N in cryptutils.c
            break;
        default:
            key = ~0ULL;
    }
    return key;
}

//=============== Mandatory project-specific implementations ====================

//--------------- verify signature -------------------------------------
static tick_t last_act_length = 0U;

/** \brief Max Activation Test time retrieved and cleared
\return max elapsed activation test cycle time in ticks
*/
tick_t actver_GetAndClearLength(void)
{
    tick_t t = last_act_length;
    last_act_length = 0U;
    return t;
}


//Doxygen annotation is in the common header activate.h

//Simple sample function to see how it goes
void actver_Verify(actverWork_t *workf, actverAct_t *actf)
{
    workf();
    if(actf())
    {
        tick_t timestamp = bios_GetTimer0Ticker();
        const ActivateVerifyData_t *p = actver_GetData(NULL);
        u64 signature;
        u64 key;
        u32 e;
        u8_least n; //test number
        for(n=0; n<SIGNATURE_COUNT; n++)
        {
            MN_ENTER_CRITICAL();
                signature = p->avdata[n].signature;
                e = p->k[n];
            MN_EXIT_CRITICAL();
            key = actver_GetKeyArtifact(n);

            e = 1U << e; //2**k
            e = 1U << e; //2**(2**k); In range; see limits on k
            e += 1U; //Finally, our Fermat prime

            if(!util64_VerifySignature(key, signature, e, workf))
            {
                mn_assert(NULL, -TRAPID_ACTIVATE);
            }
        }
        timestamp = bios_GetTimer0Ticker() - timestamp; //duration of test in ticks
        if(timestamp > last_act_length)
        {
            last_act_length = timestamp;
        }
    }
}

//--------- schedule of signature verification -----------------

/** An implementation of a mandatory "act" function for initial test of activation
Acts only if we caught the trap already.
*/
bool_t actver_ProcTaskInit(void)
{
    bool_t ret;
    ret = (mn_GetAssertId() == TRAPID_ACTIVATE);
    return ret;
}

/** An implementation of a mandatory "act" function for normal test of activation
Acts only if we are in normal or manual mode.
ACTUAL IMPLEMENTATION, per code review, is
"Run activation test always except explicitly enumerated conditions where you don't"
The purpose of this future-proofing: render the device unusable rather than let an
unactivated device useful.
*/
bool_t actver_ProcTaskRun(void)
{
    devmode_t m;
    bool_t ret;
#ifdef OLD_DEVMODE
    m = mode_GetCurrentMode();
    ret = m != MODE_OOS;
#else
    m = mode_GetMode();
    ret = (m != MODE_SETUP) && (m != MODE_FAILSAFE);
#endif
    return ret;
}

/* This line marks the end of the source */


