/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file hartapputils.c
    \brief API for HART App layer helper utils

     CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/DLT_Release_1.1.1/FIRMWARE/interface/hart/hartapputils.h $
    $Date: 7/16/09 1:44a $
    $Revision: 7 $
    $Author: Arkkhasin $

    \ingroup HART
*/
/*  $History: hartapputils.h $
 *
 * *****************  Version 7  *****************
 * User: Arkkhasin    Date: 7/16/09    Time: 1:44a
 * Updated in $/MNCB/Dev/DLT_Release_1.1.1/FIRMWARE/interface/hart
 * Rolled back Ark's solution to Action Items 1-4 per Justin's request.
 * dltamr.h is checked out and could not be rolled back so levelxmit.c
 * carries a prototype directly.
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 5/11/09    Time: 7:00p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Bug 929 fix (and now common routine to check for options key)
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 3/29/09    Time: 4:12p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * A few items moved from configure.h to pressures.h and errlimits.h
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 1/13/09    Time: 3:36p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Lint (types and prototypes)
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/11/09    Time: 8:13p
 * Created in $/MNCB/Dev/FIRMWARE/interface/hart
 * HART App layer helper utils
*/
#ifndef HARTAPPUTILS_H_
#define HARTAPPUTILS_H_

MN_DECLARE_API_FUNC(hart_IsFactoryCmdEnabled) //may or may not be used in a given project
extern bool_t hart_IsFactoryCmdEnabled(void);
//extern enum DeviceModeEnum_t hart_FormatDevModeToHART(devmode_t mode);
//extern devmode_t hart_FormatDevModeToInternal(u8 hmode);
//extern s8_least hart_CheckMode(u8 ModeFlag);
//extern bool_t hart_IsFactoryOptionKey(const u8 key[2]);

#endif //HARTAPPUTILS_H_
/* This line marks the end of the source */
