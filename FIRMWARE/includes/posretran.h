/*
Copyright 2012 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file posretran.h
    \brief Position retransmit API

    CPU: Any

    OWNER: AK

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/includes/posretran.h $
    $Date: 1/12/12 1:56p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup Cycle
*/
/* $History: posretran.h $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/12/12    Time: 1:56p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/includes
 * TFS:8674 - added pos retrnasmit to LCX2AP
*/
#ifndef POSRETRAN_H_
#define POSRETRAN_H_

//Chores for the cycle task frame
extern void periodicjob_PositionRetransmit(void);
#endif //POSRETRAN_H_

/* This line marks the end of the source */
