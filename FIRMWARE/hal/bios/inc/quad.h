/**
Copyright 2009 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file quad.h
    \brief header for BIOS routines for quadrature encoder

    CPU: NXP LPC2114 LPC2124

    OWNER: Ernie Price

    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/inc/quad.h $
    $Date: 4/28/10 5:26p $
    $Revision: 3 $
    $Author: Anatoly Podpaly $
*/

/* $History: quad.h $
 * 
 * *****************  Version 3  *****************
 * User: Anatoly Podpaly Date: 4/28/10    Time: 5:26p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/inc
 * Bug 3088: Enable Rotary Encoder interrupts only when they are needed.
 * Disable them otherwise.
 * 
 * *****************  Version 2  *****************
 * User: Anatoly Podpaly Date: 3/25/10    Time: 5:52p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/inc
 * Bug 2888: eliminated CearValue, introduced ReadAndClearValue function.
 * 
 * *****************  Version 1  *****************
 * User: Anatoly Podpaly Date: 2/01/10    Time: 4:27p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/inc
 * LCX implementation : Rotary ENcoder handler.
 * 
*/

#ifndef QUAD_H_
#define QUAD_H_

extern void    HART_EmulationSetSteps(s16 EmulatedSteps);

extern s16  quad_GetValue(void);
extern s16  quad_ReadAndClearValue(void);
extern void quad_Initialize(void);
extern void quad_EnableInt(void);
extern void quad_DisableInt(void);

#endif // QUAD_H_

/* End of the source */
