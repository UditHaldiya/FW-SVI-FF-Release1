/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/** \addtogroup HARTapp
\brief HART application layer component

\htmlonly
<a href="../../../Doc/DesignDocs/Physical Design_Hart Module.doc"> Design document </a><br>
Unit Test Credit claim is found <a href="../../../Doc/TestDocs/UnitTests.doc"> here </a><br>
\endhtmlonly
*/
/**
    \file hart_insignal.c
    \brief Glue functions to support range values in AO domain for AO output

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/glue/hart_insignal.c $
    $Date: 1/12/12 12:41p $
    $Revision: 6 $
    $Author: Arkkhasin $

    \ingroup HARTapp
*/
/* $History: hart_insignal.c $
 * 
 * *****************  Version 6  *****************
 * User: Arkkhasin    Date: 1/12/12    Time: 12:41p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/glue
 * TFS:8674 - added AO interface
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 3/30/11    Time: 1:39p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart/glue
 * Reverted to old API for old fixed-length commands
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 1/29/10    Time: 4:20p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart/glue
 * HART commands implementations rearranged for better modularity and less
 * interdependencies
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 6/10/09    Time: 12:33p
 * Created in $/MNCB/Dev/FIRMWARE/interface/hart/glue
 * Added subcommand 129/99 to read input signal data
*/
#include "mnwrap.h"
#include "hartcmd.h"
#include "hartfunc.h"
#include "hartdef.h"
#include "fpconvert.h"
#include "insignal.h"
#include "bufutils.h"

#if 0 //future - command 1 in xml
/**
\brief Reports various stages of input signal processing

Notes:
This command is allowed in all modes
*/
s8_least hartcmd_ReadInputSignal(const u8 *src, u8 *dst)
{
    UNUSED_OK(src);
    Rsp_ReadInputSignal_t *d = (void *)dst;

    (void)fpconvert_IntToFloatBuffer(sig_GetTCompSignal(), UNITSID_SIGNAL, d->TCInputSignal[0]);
    (void)fpconvert_IntToFloatBuffer(sig_GetSignal(), UNITSID_SIGNAL, d->InputSignal[0]);
    (void)fpconvert_IntToFloatBuffer(sig_GetSmoothedSignal(), UNITSID_SIGNAL, d->SmoothedInputSignal[0]);

    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}
#endif

/** \brief Read signal user trim applies equally to both standalone
input signal and a input signal linked as a read-back to an analog output
*/
s8_least hartcmd_ReadSignalReadbackTrim(const u8 *src, u8 *dst)
{
    UNUSED_OK(src);
    Rsp_ReadSignalReadbackTrim_t *d = (void *)dst;
    UserTrimRecord_t *p = (void *)d->UserTrimRecord[0];
    SignalCalData_t sigcal;
    (void)sig_GetSignalCalData(&sigcal);
    for(u8_least i=Xlow; i<Xends; i++)
    {
        util_PutS16(p->UserRangeValues[i], sigcal.uCalSignal[i]);
        util_PutS16(p->DeviceRangeValues[i], sigcal.nCalSignal[i]);
    }
    util_PutU32(p->UserTrimTag[0], sigcal.SeqNumber);

    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

/* This line marks the end of the source */
