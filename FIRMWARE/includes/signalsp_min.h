/*
Copyright 2008 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file signalsp_min.h
    \brief minimalist API for conversion of input signal to position setpoint
    CPU: Any

    OWNER: AK

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/includes/signalsp_min.h $
    $Date: 12/09/11 2:03p $
    $Revision: 2 $
    $Author: Arkkhasin $

    \ingroup posctl
*/
/* $History: signalsp_min.h $
 * 
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 12/09/11   Time: 2:03p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/includes
 * TFS:8370 - header untangling - no code change
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 11/15/11   Time: 10:57a
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/includes
 * Preliminary check-in for
 * TFS:8202 decouple I/O subsystem
 * Not yet ready to package as a module
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 7/05/08    Time: 1:32a
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * sig_ConvertSignalToStd simply converts signal to percent of signal
 * range (instead of previous convert_... which was aware of ATO/ATC)
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 4/12/08    Time: 1:43a
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * Limited refactoring required by Lint: some types changed, errors either
 * propagate up or explicitly ignored; a new HART error code 43 added to
 * indicate failed NVMEM write
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 2/25/08    Time: 4:16p
 * Created in $/MNCB/Dev/FIRMWARE/includes
 * Minimal signal-related API
 *
*/

#ifdef SIGNALSP_H_
#ifndef SIGNALSP_MIN_G_
#define SIGNALSP_MIN_G_
#include "signaldef.h"
#include "percent.h"

extern percent_least_t sig_ConvertSignalToStd(sig_t n2Signal);

#endif //SIGNALSP_MIN_G_
#endif //SIGNALSP_H_
/* This line marks the end of the source */
