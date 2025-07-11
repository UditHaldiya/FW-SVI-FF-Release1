/*
Copyright 2010 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file adtable.h
    \brief Required project-independent interfaces of project-specific
        device activation verification

    CPU: Any

    OWNER: AP
    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes/adtable.h $
    $Date: 12/20/10 5:31p $
    $Revision: 1 $
    $Author: Anatoly Podpaly $

*/
/* $History: adtable.h $
 * 
 * *****************  Version 1  *****************
 * User: Anatoly Podpaly Date: 12/20/10   Time: 5:31p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * This jheader file exposes teh periodically invoked function that tests
 * the AD channel table. 
 * 
*/
#ifndef ADTABLE_H_
#define ADTABLE_H_

/**\brief A mandatory plugin for periodic process invoked 
  checks of the AD channel table.
*/

extern void Periodic_Measurement_Table_Test(void);

#endif // ADTABLE_H_
/* This line marks the end of the source */
