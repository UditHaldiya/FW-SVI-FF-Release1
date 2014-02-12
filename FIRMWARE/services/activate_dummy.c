/*
Copyright 2010 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file activate_template.c
    \brief A dummy based on Device activation/ Signature verification support template.

    CPU: Any

    OWNER: AK

    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services/activate_dummy.c $
    $Date: 12/02/10 9:16p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup Process
*/
/* $History: activate_dummy.c $
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 12/02/10   Time: 9:16p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * TFS:4922 - A dummy activation interface for AP
*/

/* Most likely, almost all headers here may be necessary.
You may need to add more
*/
#include "mnwrap.h"
#if 0 //sample of what may be needed
#include "oswrap.h"
#include "crc.h"
#include "mnassert.h"
#include "errcodes.h"
#include "mntrapdef.h"
#include "nvram.h"
#include "cryptutils.h"
#include "devicemode.h"
#endif
#include "activate.h"
/* Include your corresponding implementation header here.
It is necessary in non-trivial cases e.g. to interface
the data to HART or NVRAM
E.g.
#include "activate_id_sn.h"
Make sure that at least activate.h is included, directly or indirectly
*/



//------------ Standard set/get fare ----------------------
// --------- not needed for a dummy ----------

//=============== Mandatory project-specific implementations ====================
//Doxygen annotation is in the common header activate.h

//--------------- verify signature -------------------------------------

//Simple sample function to see how it goes
void actver_Verify(actverWork_t *workf, actverAct_t *actf)
{
    UNUSED_OK(actf);
    workf();
}

//--------- schedule of signature verification -----------------

/** An implementation of a mandatory "act" function for initial test of activation
*/
bool_t actver_ProcTaskInit(void)
{
    /*A trivial case: never activate*/
    return false;
}

/** An implementation of a mandatory "act" function for normal test of activation
*/
bool_t actver_ProcTaskRun(void)
{
    bool_t condition;
    /*
        Check the condition you DO NOT allow an unactivated device to be in.
        E.g. always false means no runtime activation test.
        A reasonable test might be that normal mode --> condition is true
    */
    condition = false;
    return condition;
}

/* This line marks the end of the source */


