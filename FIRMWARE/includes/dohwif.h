/*
Copyright 2010 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file dohwif.h
    \brief BIOS routines API for optional digital outputs

    CPU: Any (with standard C compiler)

    OWNER: AP
    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes/dohwif.h $
    $Date: 10/06/10 9:31p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup BIOS
*/

/*
    $History: dohwif.h $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 10/06/10   Time: 9:31p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * TFS:4128 Dismantling io.{c,h} 
*/

#ifndef DOHWIF_H_
#define DOHWIF_H_

/* selection of digital output line */
#define DO_1              (u8) 0   /* digital output 1 */
#define DO_2              (u8) 1   /* digital output 2 */

extern bool_t bios_DOCardPresent(void);
extern bool_t bios_GetDigitalOutput(u8 param);
extern void bios_SetDigitalOutput(u8 param, bool_t value);

#endif //DOHWIF_H_
/* This line marks the end of the source */
