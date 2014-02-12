/*
Copyright 2009 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**

    \file nobumpless.c
    \brief wrapper(s) for functionality without bumpless transfer

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/FIRMWARE/services/nobumpless.c $
    $Date: 5/05/09 4:09p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup Modeman
*/
/* $History: nobumpless.c $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 5/05/09    Time: 4:09p
 * Created in $/MNCB/Dev/FIRMWARE/services
 * Device mode wrapper for projects without bumpless transfer
*/
#include "mnwrap.h"
#include "devicemode.h"

/** \brief A feature-independent API for setting normal mode.
For projects without bumpless transfer
\return an error code
*/
ErrorCode_t mode_SetNormalMode(void)
{
    return mode_SetMode(MODE_OPERATE);
}

/* This line marks the end of the source */
