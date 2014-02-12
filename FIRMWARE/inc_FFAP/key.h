/*
Copyright 2004-2005 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**

    \file key.h
    \brief header BIOS routines for support of the keyboard buttons
    keyboard/display reconnect is also supported

    CPU: Any (with standard C compiler)

    OWNER: AP

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/inc_SVI2AP/key.h $
    $Date: 1/03/12 2:31p $
    $Revision: 11 $
    $Author: Anatoly Podpaly $

    \ingroup bios
*/
/* $History: key.h $
 *
 * *****************  Version 11  *****************
 * User: Anatoly Podpaly Date: 1/03/12    Time: 2:31p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_SVI2AP
 * Removed unused #define, corrected defines, corrected "protector" name.
 *
 * *****************  Version 9  *****************
 * User: Arkkhasin    Date: 12/02/10   Time: 2:46p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_SVI2AP
 * TFS:4922 Added dummy PUSHBUTTON_CANCEL FOB LCX-style uiinput.h
*/

#ifndef KEY_H_
#define KEY_H_

/* bitcoding of pushbuttons */
#define PUSHBUTTON_STAR     ((u8)(1U << 0))	/* "*" Button */
#define PUSHBUTTON_MINUS    ((u8)(1U << 1))	/* "-" Button */
#define PUSHBUTTON_PLUS     ((u8)(1U << 2))	/* "+" Button */
#define PUSHBUTTON_CANCEL   ((u8)(0U))	    /* --- No Button -- CANCEL but def is required now */

#define PUSHBUTTON_DOWN_EDGE    ((u8)0U)    /* return down edges of pushbuttons */
#define PUSHBUTTON_RELEASE_EDGE ((u8)1U)  /* return down edges of pushbutton */

extern u8       bios_ReadPushButtonStatus(u8 *pUpdown);
extern bool_t   bios_ReadDiState(void);
extern void     bios_InitKeyboardDriver(void);
extern void   bios_HandleDigitalInput(void);
extern void     key_Mopup(void);

#endif // KEY_H_

/* End of the source */
