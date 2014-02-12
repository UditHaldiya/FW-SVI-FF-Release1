/**
Copyright 2004-2005 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file key.h
    \brief header BIOS routines for support of the keyboard buttons
    keyboard/display reconnect is also supported

    CPU: Any (with standard C compiler)

    OWNER: EP
    \n RCS: $Id$
    Created: 9/27/04 Author:  EP
*/

/*
    $History: key.h $
 * 
 * *****************  Version 18  *****************
 * User: Anatoly Podpaly Date: 1/03/12    Time: 2:30p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_LCX
 * Removed unused #define.
 * 
 * *****************  Version 17  *****************
 * User: Arkkhasin    Date: 11/29/11   Time: 1:53p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_LCX
 * TFS:8313 Lint
 *
 * *****************  Version 15  *****************
 * User: Anatoly Podpaly Date: 1/11/11    Time: 11:12a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * TFS:5324 - removed obsoleted emulation-related function headers.
 *
 * *****************  Version 14  *****************
 * User: Anatoly Podpaly Date: 11/08/10   Time: 11:49a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * Bug 4637 - removed unused definitions and the prototype from the file.
 *
 * *****************  Version 12  *****************
 * User: Anatoly Podpaly Date: 4/28/10    Time: 5:35p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/inc
 * Development in progress: added typedef and a function protiotype to
 * read interface card ID.
 *
 * *****************  Version 11  *****************
 * User: Anatoly Podpaly Date: 4/01/10    Time: 5:44p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/inc
 * Bug #2930 - Enable UI emulation.
 *
 * *****************  Version 10  *****************
 * User: Anatoly Podpaly Date: 2/09/10    Time: 11:47a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/inc
 * LCX development - removed unused functions.
 *
 * *****************  Version 9  *****************
 * User: Anatoly Podpaly Date: 2/01/10    Time: 4:26p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/inc
 * LCX Implementation.

*/

#ifndef KEY_H_
#define KEY_H_

/* bitcoding of pushbuttons */
#define PUSHBUTTON_STAR     (1U << 0)	/* "*" Button -- AUTO */
#define PUSHBUTTON_MINUS    (1U << 1)	/* "-" Button -- OPEN */
#define PUSHBUTTON_PLUS     (1U << 2)	/* "+" Button -- CLOSED */
#define PUSHBUTTON_CANCEL   (1U << 3)	/* --- Button -- CANCEL */

#define PUSHBUTTON_AUTO     PUSHBUTTON_STAR
#define PUSHBUTTON_OPEN     PUSHBUTTON_MINUS
#define PUSHBUTTON_CLOSED   PUSHBUTTON_PLUS

#define PUSHBUTTON_DOWN_EDGE    0U       /* return down edges of pushbuttons */
#define PUSHBUTTON_RELEASE_EDGE 1U       /* return down edges of pushbutton */

// Jumper positions
#define JUMPER_SETTING_CUSTOM               (0x00U)
#define JUMPER_SETTING_LARGE                (0x01U)
#define JUMPER_SETTING_MEDIUM               (0x02U)
#define JUMPER_SETTING_SMALL                (0x03U)
#define JUMPER_SETTING_4                    (0x04U)
#define JUMPER_SETTING_5                    (0x05U)
#define JUMPER_SETTING_6                    (0x06U)
#define JUMPER_SETTING_7                    (0x07U)
#define JUMPER_PARAMETER_MASK               (0x07U)

#define JUMPER_SETTING_ATO                  (0x00U)
#define JUMPER_SETTING_ATC                  (0x08U)
#define JUMPER_ATO_ATC_MASK                 (0x08U)

extern u8       bios_ReadJumpers(void);
extern u8       bios_ReadPushButtonStatus(u8 *pUpdown);
extern void     bios_LockKeyboard(bool_t lock);
extern bool_t   bios_ReadDiState(void);
extern void     bios_HandleDigitalInput(void);
extern bool_t   bios_InitPushbuttons(void);
extern void     key_Mopup(void);


#endif // KEY_H_

/* End of the source */
