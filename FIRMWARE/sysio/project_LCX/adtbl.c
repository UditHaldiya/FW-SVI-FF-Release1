/*
Copyright 2006 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/** \addtogroup ADTable  A/D sequence table module
\brief A/D sensor sequence table module
\htmlonly
<a href="../../../Doc/DesignDocs/Physical Design_ADTable Module.doc"> Design document </a><br>
<a href="../../../Doc/TestDocs/utplan_adtable_reset.doc"> Unit Test Report </a><br>
\endhtmlonly
*/
/**
    \file adtbl.c
    \brief A/D sensor sequence table

    CPU: Philips LPC21x4 (ARM)

    OWNER: Ernie Price

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/sysio/project_LCX/adtbl.c $
    $Date: 12/07/11 6:45p $
    $Revision: 5 $
    $Author: Arkkhasin $

    \ingroup ADTable
*/
/* $History: adtbl.c $
 * 
 * *****************  Version 5  *****************
 * User: Arkkhasin    Date: 12/07/11   Time: 6:45p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/sysio/project_LCX
 * TFS:8255 AP sequencing fix
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 12/07/11   Time: 1:02p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/sysio/project_LCX
 * TFS:8204 - features for I/O channels
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 11/29/11   Time: 2:21p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/sysio
 * TFS:8313 Lint
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 11/20/11   Time: 11:27p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/sysio
 * TFS:8255 - A/D channel sequence tables (preliminary)
*/
//#include "projectdef.h"
#include "mnwrap.h"
#include "adtype.h"
#include "adtbl.h"
#include "adtblgen.h"

#include "tempr.h"
#include "tempr_mncb.h"
#include "ipcurr.h"
#include "insignal.h"
#include "posint.h"

// Normal sequence for Internal Position
static const u8 measureSeqPosInt[] =
{
    ADTABLE(FAST_CHANNELS(AD_AI_SP, AD_POS_INT, AD_IP_CUR),
            AD_PRESS_PILOT,
            AD_PRESS_ATMOS,
            AD_TEMP_INT,
            AD_DIAG)
};

// Startup sequence - read every sensor once
static const u8 measureSeqAll[] =
{
    ADTABLE(FAST_CHANNELS(  AD_AI_SP,
                            AD_POS_INT,
                            AD_IP_CUR,
                            AD_PRESS_PILOT,
                            AD_PRESS_ATMOS,
                            AD_TEMP_INT,
                            AD_DIAG
                          ),
            NO_SLOW_CHANNELS
                )
};

// Startup helper sequence - read temperature sensor once
static const u8 measureSeqTempr[] =
{
    ADTABLE(FAST_CHANNELS(  AD_TEMP_INT),
            NO_SLOW_CHANNELS
                )
};


const u8 * const adtbl_seqTbl[AD_SEQ_LIMIT] = /*lint -e{785} OK to have uninitialized entries */
{
    [AD_SEQ_POS_INT]     = measureSeqPosInt,
    [AD_SEQ_CALIB]       = measureSeqPosInt,
    [AD_SEQ_POS_INT_INIT] = measureSeqAll,
    [AD_SEQ_TEMPR] = measureSeqTempr,
};


/** \brief A simple shim "channel function" to address prototype mismatch
\param advalue - a raw A/D read
*/
static void sig_Update16(ADRaw_t advalue)
{
    sig_Update(advalue);
}

void (* const ad_UpdateFunc[])(ADRaw_t advalue) =  /*lint -e{785} OK to have uninitialized entries */
{
    //----------- inputs only ------------
    [AD_AI_SP] = sig_Update16,
    [AD_TEMP_INT] = tempr_Update,
    [AD_POS_INT] = posint_Update,	
    [AD_IP_CUR] = ipcurr_Update,  	
    [AD_DIAG] = NULL,
};

/* This line marks the end of the source */
