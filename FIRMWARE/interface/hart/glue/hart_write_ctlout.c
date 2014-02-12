/*
Copyright 2010 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file hart_write_ctlout.c
    \brief HART glue layer for writing to FW the CtlOut value, specified by a user
           CtlOut value

    CPU: Any

    OWNER:     Sergey
    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue/hart_write_ctlout.c $
    $Date: 4/13/11 4:53p $
    $Revision: 1 $
    $Author: Sergey Kruss $

    \ingroup HARTapp
*/

/* $History: hart_write_ctlout.c $
 *
 * *****************  Version 1  *****************
 * User: Sergey Kruss Date: 4/13/11    Time: 4:53p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * TFS:6074 - added new file with declaration of a HART command to write
 * CtlOut for validating Normalization.
*/

#include "mnwrap.h"
#include "position.h"
#include "hartfunc.h"
#include "hartdef.h"
#include "bufutils.h"
#include "sysio.h"

/* \brief A helper for common HART commands functionality
\param value - PWM value; 0 means "turn control back on"
\param mode - one of the pre-defined output modes
\return an error code
*/
static s8_least write_pwm(u16 value, enumPWMNORM_t mode)
{
    ErrorCode_t err = ERR_OK;
    if(value == 0)
    {
        //special case: cancel open-loop
        mode_SetControlMode(CONTROL_MANUAL_POS, vpos_GetScaledPosition());
    }
    else
    {
        err = sysio_SetForcedCtlOutput(value, mode);
    }
    return err2hart(err);
}



/**
    \brief Write Control Output - subcommand 160 of Factory command 130
           Note: This command is allowed in OOS and Failsafe modes
           Note: zero parameter takes the device to controlled mode.
           The function is added per TFS:6074
*/
s8_least hartcmd_WriteControlOutput(const u8 *src, u8 *dst)
{
    UNUSED_OK(dst);
    const Req_WriteControlOutput_t *s = (const void *)src;
    u16 n2HartCtlOut;   //CtlOut value received from the host

    //get the CtlOut value from the wire
    n2HartCtlOut = util_GetU16(s->ControlOutput[0]);
    //send obtained CtlOut to the device; n2HartCtlOut=0 means "turn control back on"

    return write_pwm(n2HartCtlOut, PWMNORMALIZED);
} //-----  end of hartcmd_WriteControlOutput() ------

/**
\brief Sets the IP output to a commanded value

A value of 0 returns the I/P to normal operation
*/
s8_least hartcmd_WriteIPValue(const u8 *src, u8 *dst)
{
    UNUSED_OK(dst);
    const Req_WriteIPValue_t *s = (const void *)src;
    u16 IPOut = util_GetU16(s->ipValue_IPOut[0]);

    return write_pwm(IPOut, PWMEXACT);
}


/* This line marks the end of the source */
