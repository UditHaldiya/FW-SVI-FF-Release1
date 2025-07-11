/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/** \addtogroup mnhartcmd HART Command Module
\brief Dispatcher of HART commands

\htmlonly
<a href="../../../Doc/DesignDocs/Physical Design_??.doc"> Design document </a><br>
<a href="../../inhouse/unit_test/Dbg/project_ESD/??.html"> Unit Test Report [1]</a><br>
<a href="../../inhouse/unit_test/Dbg/project_ESD/??.html"> Unit Test Report [2]</a><br>
\endhtmlonly
*/
/**
    \file hcstatus.c
    \brief Project-specific HART helpers

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/AP_Release_3.1.x/FIRMWARE/interface/hart/project_SVI2AP/hcstatus.c $
    $Date: 7/19/11 1:30p $
    $Revision: 4 $
    $Author: Arkkhasin $

    \ingroup mnhartcmd
*/
/* $History: hcstatus.c $
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 7/19/11    Time: 1:30p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/interface/hart/project_SVI2AP
 * Re-sync to TFS:C20251
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 7/15/11    Time: 2:07p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/interface/hart/project_SVI2AP
 * Sync to TFS:C20251 (FBO TFS:7036)
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 3/16/11    Time: 11:08p
 * Created in $/MNCB/Dev/HART6Devel/FIRMWARE/interface/hart/project_LCX
 * Borrowed from AP (trunk version, same functionality as AP)
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 3/23/09    Time: 7:10p
 * Created in $/MNCB/Dev/FIRMWARE/interface/hart/project_SVI2AP
 * Project-specific HART response status plugin
*/

#include "mnwrap.h"
#include "hartfunc.h"
#include "faultpublic.h"
#include "hart.h"

//Device-specific implementation of common API - doc in header
//This implementation is a carryover of 3.1.x implementation
u8 hart_StatusPlugin(u8 rsp2)
{
    //if(error_IsFault(FAULT_RANGE1) || error_IsFault(FAULT_RANGE2) || error_IsFault(FAULT_RANGE3) || error_IsFault(FAULT_RANGE4))
    if(error_IsFault(FAULT_POSITION_ERROR))
    {
        rsp2 |= HART_PRIMARYVAR_OUT_OF_LIMITS;
    }

    return rsp2;
}
/* This line marks the end of the source */
