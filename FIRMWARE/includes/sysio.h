/**
Copyright 2005 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file sysio.h
    \brief System-level I/O

    Public header of sysio.c.
    See details there.

    CPU: Any (with standard C compiler)

    OWNER: AK
    \n RCS: $Id$
    Created:  02/24/ $Change: $ Author:
*/

#ifndef SYSIO_H_
#define SYSIO_H_
#include "errcodes.h"
// -------------------- UI support --------------------------
extern /*uiinput_t*/ u32 sysio_GetPushButtons(void);
extern void sysio_UpdateLcd(void);

#ifdef NDEBUG
#define sysio_ShowCrash(message, line) /*nothing*/
#else
extern void sysio_ShowCrash(const void *message, s32 line);
#endif //NDEBUG


//----------------------------- I/P output -----------------------------
typedef enum enumPWMNORM_t { //for normalization of PWM
    /** TFS:4108 **/
    PWMSTRAIGHT,
    PWMNORMALIZED,
    PWMEXACT  /** TFS:8756 */
} enumPWMNORM_t;

extern ErrorCode_t sysio_SetForcedCtlOutput(u16 ctlOut, enumPWMNORM_t output_mode);

/** \brief A deprecated legacy API for processes; no limiting or error return
\param ipout - raw PWM value to write.
*/
MN_INLINE void sysio_SetForcedOutput_OOS(u16_least ipout) //deprecated
{
    (void)sysio_SetForcedCtlOutput((u16)MIN(UINT16_MAX, ipout), PWMEXACT);
}


extern u16 sysio_GetRealPWM(void);
extern u8 sysio_WritePwm(const s32 value, const enumPWMNORM_t enPWMcorr);
//bits defining control output limiting for sysio_WritePwm
#define CTLOUTPUT_LIMITED_LOW 0x01U
#define CTLOUTPUT_LIMITED_HIGH 0x02U

extern s32 sysio_CtlOut2PWMdomain(const s32 ctlOut);

#endif //SYSIO_H_
/* This line marks the end of the source */
