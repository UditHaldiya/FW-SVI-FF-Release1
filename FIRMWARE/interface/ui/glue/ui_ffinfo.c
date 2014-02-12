/*
Copyright 2012 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file ui_ffinfo.c
    \brief UI service functions for "foundation field bus" support (MNCB)

     CPU: Any

    OWNER: EJ
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue/ui_ffinfo.c $
    $Date: 1/07/12 3:14p $
    $Revision: 1 $
    $Author: EricJ $

    \ingroup UI
*/
/* $History: ui_ffInfo.c $

 * *****************  Version 1  *****************
 * User: Eric J    Date: 2/07/12    Time: 3:29p
 * Created in $/MNCB/Dev/FIRMWARE/interface/ui/glue
 * First version
*/

#include "mnwrap.h"
#include "numutils.h"
#include "ui_config.h"
#include "ui_ffinfo.h"
#include "uidef.h"
#include "uishow.h"
#include "lcd.h"
#include "utilities.h"
#include "ff_resblockvars.h"
#include "ff_aoblockvars.h"
#include "ff_doblockvars.h"
#include "ff_pidblockvars.h"
#include "ff_tbblockvars.h"
#include "ff_pid2blockvars.h"
#include "ff_do2blockvars.h"
#include "ff_devicevars.h"

#define FFSTR_LENGTH_MODE  (sizeof(FFSTR_MODE_NA)-1) /* 5 */
#define FFSTR_OFFSET_ERR   FFSTR_LENGTH_MODE         /* 5 */
#define FFSTR_LENGTH_ERR  (NUMBER_OF_DIGITS_PER_LINE - FFSTR_OFFSET_ERR) /* 4 */

/** \brief convert the mode from IPC to string
        input:
            mode: mode from IPC
            pbuf: the string
*/
static void ff_Mode2Str(const u8 mode, u8* pbuf)
{
    UNUSED_OK(pbuf);
    switch (mode)
    {
    case IPC_MODE_AUTO:
        mn_memcpy((void*)pbuf, FFSTR_MODE_AUTO, FFSTR_LENGTH_MODE);
        break;
    case IPC_MODE_MANUAL:
        mn_memcpy((void*)pbuf, FFSTR_MODE_MAN, FFSTR_LENGTH_MODE);
        break;
    case IPC_MODE_LOVERRIDE:
        mn_memcpy((void*)pbuf, FFSTR_MODE_LO, FFSTR_LENGTH_MODE);
        break;
    case IPC_MODE_OOS:
        mn_memcpy((void*)pbuf, FFSTR_MODE_OOS, FFSTR_LENGTH_MODE);
        break;
    case IPC_MODE_ROUT:
        mn_memcpy((void*)pbuf, FFSTR_MODE_ROUT, FFSTR_LENGTH_MODE);
        break;
    case IPC_MODE_RCAS:
        mn_memcpy((void*)pbuf, FFSTR_MODE_RCAS, FFSTR_LENGTH_MODE);
        break;
    case IPC_MODE_IMAN:
        mn_memcpy((void*)pbuf, FFSTR_MODE_IMAN, FFSTR_LENGTH_MODE);
        break;
    case IPC_MODE_CAS:
        mn_memcpy((void*)pbuf, FFSTR_MODE_CAS, FFSTR_LENGTH_MODE);
        break;

    default:
        mn_memcpy((void*)pbuf, FFSTR_MODE_NA, FFSTR_LENGTH_MODE);
        break;
    }
}

/** \brief convert the mode from IPC to string
        input:
            mode: err from IPC
            pbuf: the string
*/
static void ff_Err2Str(const u16 err, u8* pbuf)
{
    UNUSED_OK(pbuf);
    if (FF_BLK_ERR_NOERR != err)
    {
        mn_memcpy((void*)pbuf, FFSTR_ERR_ERR, FFSTR_LENGTH_ERR);
    }
    else
    {
        mn_memcpy((void*)pbuf, FFSTR_ERR_OK, FFSTR_LENGTH_ERR);
    }
}

/** \brief called when display exit a menu cycle of FF info, to
        infor mation of resource block.
*/
bool_t ui_ff_InfoRB(const uistate_t *state)
{
    const IPC_FFResourceParams_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1];

    pVar = GetResBlockVar();
    // display the orignal digits on lcd, and will be replaced by string
    ff_Mode2Str(pVar->ModeActual, ln2);
    ff_Err2Str(pVar->BlockErr, &ln2[FFSTR_OFFSET_ERR]);
    ui_setDispmode(DISPMODE_FF_INFO, ln2);

    UNUSED_OK(state);
    return false;
}

/** \brief called when display exit a menu cycle of FF info, to
        part 1 of resource block.
*/
bool_t ui_ff_Tag1RB(const uistate_t *state)
{
    const IPC_FFResourceParams_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1];

    pVar = GetResBlockVar();
    mn_memcpy(ln2, pVar->tag, NUMBER_OF_DIGITS_PER_LINE);
    ui_setDispmode(DISPMODE_FF_INFO, ln2);

    UNUSED_OK(state);
    return false;
}

/** \brief called when display exit a menu cycle of FF info, to
        show part 2 of resource block.
*/
bool_t ui_ff_Tag2RB(const uistate_t *state)
{
    const IPC_FFResourceParams_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1];

    pVar = GetResBlockVar();
    mn_memcpy(ln2, pVar->tag + NUMBER_OF_DIGITS_PER_LINE, NUMBER_OF_DIGITS_PER_LINE);
    ui_setDispmode(DISPMODE_FF_INFO, ln2);

    UNUSED_OK(state);
    return false;
}

/** \brief called when display exit a menu cycle of FF info, to
        show part 3 of resource block.
*/
bool_t ui_ff_Tag3RB(const uistate_t *state)
{
    const IPC_FFResourceParams_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1];

    pVar = GetResBlockVar();
    mn_memcpy(ln2, pVar->tag + NUMBER_OF_DIGITS_PER_LINE *2, NUMBER_OF_DIGITS_PER_LINE);

    ui_setDispmode(DISPMODE_FF_INFO, ln2);
    UNUSED_OK(state);
    return false;
}

/** \brief called when display exit a menu cycle of FF info, to
        show part 4 of resource block.
*/
bool_t ui_ff_Tag4RB(const uistate_t *state)
{
    const IPC_FFResourceParams_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1]  = BLANK_STR_LCD_1LINE;

    pVar = GetResBlockVar();
    CONST_ASSERT ((FF_RB_TAG_MAX_LEN -NUMBER_OF_DIGITS_PER_LINE *3) <= NUMBER_OF_DIGITS_PER_LINE);
    mn_memcpy(ln2, pVar->tag + NUMBER_OF_DIGITS_PER_LINE *3, FF_RB_TAG_MAX_LEN -NUMBER_OF_DIGITS_PER_LINE *3);
    ui_setDispmode(DISPMODE_FF_INFO, ln2);

    UNUSED_OK(state);
    return false;
}

/** \brief called when display enter a menu cycle of FF info, to
        show device ID part 1.
*/
bool_t ui_ff_devID1(const uistate_t *state)
{
    const IPC_FFDeviceParams_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1];

    pVar = GetDeviceVar();
    mn_memcpy(ln2, pVar->dev_id, NUMBER_OF_DIGITS_PER_LINE);
    ui_setDispmode(DISPMODE_FF_INFO, ln2);

    UNUSED_OK(state);
    return false;
}

/** \brief called when display enter a menu cycle of FF info, to
        show device ID part 2.
*/
bool_t ui_ff_devID2(const uistate_t *state)
{
    const IPC_FFDeviceParams_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1];

    pVar = GetDeviceVar();
    mn_memcpy(ln2, pVar->dev_id + NUMBER_OF_DIGITS_PER_LINE, NUMBER_OF_DIGITS_PER_LINE);
    ui_setDispmode(DISPMODE_FF_INFO, ln2);

    UNUSED_OK(state);
    return false;
}

/** \brief called when display enter a menu cycle of FF info, to
        show device ID part 3.
*/
bool_t ui_ff_devID3(const uistate_t *state)
{
    const IPC_FFDeviceParams_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1];

    pVar = GetDeviceVar();
    mn_memcpy(ln2, pVar->dev_id + NUMBER_OF_DIGITS_PER_LINE*2, NUMBER_OF_DIGITS_PER_LINE);
    ui_setDispmode(DISPMODE_FF_INFO, ln2);

    UNUSED_OK(state);
    return false;
}

/** \brief called when display enter a menu cycle of FF info, to
        show device ID part 4.
*/
bool_t ui_ff_devID4(const uistate_t *state)
{
    const IPC_FFDeviceParams_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1] = BLANK_STR_LCD_1LINE;

    pVar = GetDeviceVar();
    CONST_ASSERT ((FF_DEVICE_ID_LEN -NUMBER_OF_DIGITS_PER_LINE *3) <= NUMBER_OF_DIGITS_PER_LINE);
    mn_memcpy(ln2, pVar->dev_id + NUMBER_OF_DIGITS_PER_LINE*3, FF_DEVICE_ID_LEN -NUMBER_OF_DIGITS_PER_LINE *3);
    ui_setDispmode(DISPMODE_FF_INFO, ln2);

    UNUSED_OK(state);
    return false;
}

/** \brief called when display exit a menu cycle of FF info, to
        show device address.
*/
bool_t ui_ff_devAddr(const uistate_t *state)
{

    const IPC_FFDeviceParams_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1] = BLANK_STR_LCD_1LINE;

    pVar = GetDeviceVar();
    ut_bin2asc(pVar->dev_addr, (p8*)ln2, 0);
    ui_setDispmode(DISPMODE_FF_INFO, ln2);

    UNUSED_OK(state);



    return false;
}

/** \brief called when display exit a menu cycle of FF info, to
        show part 1 of device tag.
*/
bool_t ui_ff_devTag1(const uistate_t *state)
{
    const IPC_FFDeviceParams_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1];

    pVar = GetDeviceVar();
    mn_memcpy(ln2, pVar->dev_tag, NUMBER_OF_DIGITS_PER_LINE);
    ui_setDispmode(DISPMODE_FF_INFO, ln2);

    UNUSED_OK(state);
    return false;
}

/** \brief called when display exit a menu cycle of FF info, to
        show part 2 of device tag.
*/
bool_t ui_ff_devTag2(const uistate_t *state)
{
    const IPC_FFDeviceParams_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1];

    pVar = GetDeviceVar();
    mn_memcpy(ln2, pVar->dev_tag + NUMBER_OF_DIGITS_PER_LINE, NUMBER_OF_DIGITS_PER_LINE);
    ui_setDispmode(DISPMODE_FF_INFO, ln2);

    UNUSED_OK(state);
    return false;
}

/** \brief called when display exit a menu cycle of FF info, to
        show part 3 of device tag.
*/
bool_t ui_ff_devTag3(const uistate_t *state)
{
    const IPC_FFDeviceParams_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1];

    pVar = GetDeviceVar();
    mn_memcpy(ln2, pVar->dev_tag + NUMBER_OF_DIGITS_PER_LINE*2, NUMBER_OF_DIGITS_PER_LINE);
    ui_setDispmode(DISPMODE_FF_INFO, ln2);

    UNUSED_OK(state);
    return false;
}

/** \brief called when display exit a menu cycle of FF info, to
        show part 4 of device tag.
*/
bool_t ui_ff_devTag4(const uistate_t *state)
{
    const IPC_FFDeviceParams_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1] = BLANK_STR_LCD_1LINE;

    pVar = GetDeviceVar();
    CONST_ASSERT ((FF_DEVICE_TAG_LEN -NUMBER_OF_DIGITS_PER_LINE *3) <= NUMBER_OF_DIGITS_PER_LINE);
    mn_memcpy(ln2, pVar->dev_tag + NUMBER_OF_DIGITS_PER_LINE*3, FF_DEVICE_TAG_LEN -NUMBER_OF_DIGITS_PER_LINE *3);
    ui_setDispmode(DISPMODE_FF_INFO, ln2);

    UNUSED_OK(state);
    return false;
}

/** \brief called when display exit a menu cycle of FF info, to clear
        string on line 3.
*/
bool_t ui_Clr_Lin3(const uistate_t *state)
{
    ui_clearLine3();
    UNUSED_OK(state);
    return false;
}

/** \brief called when display exit a menu cycle of FF info, to clear
        string on line 3. Return true means disable current node
*/
bool_t ui_Clr_Lin3Silent(const uistate_t *state)
{
    ui_clearLine3();
    UNUSED_OK(state);
    return true;
}

/** \brief called when display exit a menu cycle of FF info, to
        infor mation of TB block.
*/
bool_t ui_ff_InfoTB(const uistate_t *state)
{
    const IPC_FFTBParams_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1];

    pVar = GetTbBlockVar();
    // display the orignal digits on lcd, and will be replaced by string
    ff_Mode2Str(pVar->ModeActual, ln2);
    ff_Err2Str(pVar->BlockErr, &ln2[FFSTR_OFFSET_ERR]);
    ui_setDispmode(DISPMODE_FF_INFO, ln2);

    UNUSED_OK(state);
    return false;
}

/** \brief called when display exit a menu cycle of FF info, to
        part 1 of TB block.
*/
bool_t ui_ff_Tag1TB(const uistate_t *state)
{
    const IPC_FFTBParams_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1];

    pVar = GetTbBlockVar();
    mn_memcpy(ln2, pVar->tag, NUMBER_OF_DIGITS_PER_LINE);
    ui_setDispmode(DISPMODE_FF_INFO, ln2);

    UNUSED_OK(state);
    return false;
}

/** \brief called when display exit a menu cycle of FF info, to
        show part 2 of TB block.
*/
bool_t ui_ff_Tag2TB(const uistate_t *state)
{
    const IPC_FFTBParams_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1];

    pVar = GetTbBlockVar();
    mn_memcpy(ln2, pVar->tag + NUMBER_OF_DIGITS_PER_LINE, NUMBER_OF_DIGITS_PER_LINE);
    ui_setDispmode(DISPMODE_FF_INFO, ln2);

    UNUSED_OK(state);
    return false;
}

/** \brief called when display exit a menu cycle of FF info, to
        show part 3 of TB block.
*/
bool_t ui_ff_Tag3TB(const uistate_t *state)
{
    const IPC_FFTBParams_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1];

    pVar = GetTbBlockVar();
    mn_memcpy(ln2, pVar->tag + NUMBER_OF_DIGITS_PER_LINE *2, NUMBER_OF_DIGITS_PER_LINE);

    ui_setDispmode(DISPMODE_FF_INFO, ln2);
    UNUSED_OK(state);
    return false;
}

/** \brief called when display exit a menu cycle of FF info, to
        show part 4 of TB block.
*/
bool_t ui_ff_Tag4TB(const uistate_t *state)
{
    const IPC_FFTBParams_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1]  = BLANK_STR_LCD_1LINE;

    pVar = GetTbBlockVar();
    CONST_ASSERT ((FF_RB_TAG_MAX_LEN -NUMBER_OF_DIGITS_PER_LINE *3) <= NUMBER_OF_DIGITS_PER_LINE);
    mn_memcpy(ln2, pVar->tag + NUMBER_OF_DIGITS_PER_LINE *3, FF_RB_TAG_MAX_LEN -NUMBER_OF_DIGITS_PER_LINE *3);
    ui_setDispmode(DISPMODE_FF_INFO, ln2);

    UNUSED_OK(state);
    return false;
}
/** \brief called when display exit a menu cycle of FF info, to
        infor mation of AO block.
*/
bool_t ui_ff_InfoAO(const uistate_t *state)
{
    const IPC_FFAOParams_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1];

    pVar = GetAoBlockVar();
    // display the orignal digits on lcd, and will be replaced by string
    ff_Mode2Str(pVar->ModeActual, ln2);
    ff_Err2Str(pVar->BlockErr, &ln2[FFSTR_OFFSET_ERR]);
    ui_setDispmode(DISPMODE_FF_INFO, ln2);

    UNUSED_OK(state);
    return false;
}

/** \brief called when display exit a menu cycle of FF info, to
        part 1 of AO block.
*/
bool_t ui_ff_Tag1AO(const uistate_t *state)
{
    const IPC_FFAOParams_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1];

    pVar = GetAoBlockVar();
    mn_memcpy(ln2, pVar->tag, NUMBER_OF_DIGITS_PER_LINE);
    ui_setDispmode(DISPMODE_FF_INFO, ln2);

    UNUSED_OK(state);
    return false;
}

/** \brief called when display exit a menu cycle of FF info, to
        show part 2 of AO block.
*/
bool_t ui_ff_Tag2AO(const uistate_t *state)
{
    const IPC_FFAOParams_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1];

    pVar = GetAoBlockVar();
    mn_memcpy(ln2, pVar->tag + NUMBER_OF_DIGITS_PER_LINE, NUMBER_OF_DIGITS_PER_LINE);
    ui_setDispmode(DISPMODE_FF_INFO, ln2);

    UNUSED_OK(state);
    return false;
}

/** \brief called when display exit a menu cycle of FF info, to
        show part 3 of AO block.
*/
bool_t ui_ff_Tag3AO(const uistate_t *state)
{
    const IPC_FFAOParams_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1];

    pVar = GetAoBlockVar();
    mn_memcpy(ln2, pVar->tag + NUMBER_OF_DIGITS_PER_LINE *2, NUMBER_OF_DIGITS_PER_LINE);

    ui_setDispmode(DISPMODE_FF_INFO, ln2);
    UNUSED_OK(state);
    return false;
}

/** \brief called when display exit a menu cycle of FF info, to
        show part 4 of AO block.
*/
bool_t ui_ff_Tag4AO(const uistate_t *state)
{
    const IPC_FFAOParams_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1]  = BLANK_STR_LCD_1LINE;

    pVar = GetAoBlockVar();
    CONST_ASSERT ((FF_RB_TAG_MAX_LEN -NUMBER_OF_DIGITS_PER_LINE *3) <= NUMBER_OF_DIGITS_PER_LINE);
    mn_memcpy(ln2, pVar->tag + NUMBER_OF_DIGITS_PER_LINE *3, FF_RB_TAG_MAX_LEN -NUMBER_OF_DIGITS_PER_LINE *3);
    ui_setDispmode(DISPMODE_FF_INFO, ln2);

    UNUSED_OK(state);
    return false;
}
/** \brief called when display exit a menu cycle of FF info, to
        infor mation of PID block.
*/
bool_t ui_ff_InfoPID(const uistate_t *state)
{
    const IPC_FFPIDParams_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1];

    pVar = GetPidBlockVar();
    // display the orignal digits on lcd, and will be replaced by string
    ff_Mode2Str(pVar->ModeActual, ln2);
    ff_Err2Str(pVar->BlockErr, &ln2[FFSTR_OFFSET_ERR]);
    ui_setDispmode(DISPMODE_FF_INFO, ln2);

    UNUSED_OK(state);
    return false;
}

/** \brief called when display exit a menu cycle of FF info, to
        part 1 of PID block.
*/
bool_t ui_ff_Tag1PID(const uistate_t *state)
{
    const IPC_FFPIDParams_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1];

    pVar = GetPidBlockVar();
    mn_memcpy(ln2, pVar->tag, NUMBER_OF_DIGITS_PER_LINE);
    ui_setDispmode(DISPMODE_FF_INFO, ln2);

    UNUSED_OK(state);
    return false;
}

/** \brief called when display exit a menu cycle of FF info, to
        show part 2 of PID block.
*/
bool_t ui_ff_Tag2PID(const uistate_t *state)
{
    const IPC_FFPIDParams_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1];

    pVar = GetPidBlockVar();
    mn_memcpy(ln2, pVar->tag + NUMBER_OF_DIGITS_PER_LINE, NUMBER_OF_DIGITS_PER_LINE);
    ui_setDispmode(DISPMODE_FF_INFO, ln2);

    UNUSED_OK(state);
    return false;
}

/** \brief called when display exit a menu cycle of FF info, to
        show part 3 of PID block.
*/
bool_t ui_ff_Tag3PID(const uistate_t *state)
{
    const IPC_FFPIDParams_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1];

    pVar = GetPidBlockVar();
    mn_memcpy(ln2, pVar->tag + NUMBER_OF_DIGITS_PER_LINE *2, NUMBER_OF_DIGITS_PER_LINE);

    ui_setDispmode(DISPMODE_FF_INFO, ln2);
    UNUSED_OK(state);
    return false;
}

/** \brief called when display exit a menu cycle of FF info, to
        show part 4 of PID block.
*/
bool_t ui_ff_Tag4PID(const uistate_t *state)
{
    const IPC_FFPIDParams_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1]  = BLANK_STR_LCD_1LINE;

    pVar = GetPidBlockVar();
    CONST_ASSERT ((FF_RB_TAG_MAX_LEN -NUMBER_OF_DIGITS_PER_LINE *3) <= NUMBER_OF_DIGITS_PER_LINE);
    mn_memcpy(ln2, pVar->tag + NUMBER_OF_DIGITS_PER_LINE *3, FF_RB_TAG_MAX_LEN -NUMBER_OF_DIGITS_PER_LINE *3);
    ui_setDispmode(DISPMODE_FF_INFO, ln2);

    UNUSED_OK(state);
    return false;
}
/** \brief called when display exit a menu cycle of FF info, to
        infor mation of DO block.
*/
bool_t ui_ff_InfoDO(const uistate_t *state)
{
    const IPC_FFDOParams_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1];

    pVar = GetDoBlockVar();
    // display the orignal digits on lcd, and will be replaced by string
    ff_Mode2Str(pVar->ModeActual, ln2);
    ff_Err2Str(pVar->BlockErr, &ln2[FFSTR_OFFSET_ERR]);
    ui_setDispmode(DISPMODE_FF_INFO, ln2);

    UNUSED_OK(state);
    return false;
}

/** \brief called when display exit a menu cycle of FF info, to
        part 1 of DO block.
*/
bool_t ui_ff_Tag1DO(const uistate_t *state)
{
    const IPC_FFDOParams_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1];

    pVar = GetDoBlockVar();
    mn_memcpy(ln2, pVar->tag, NUMBER_OF_DIGITS_PER_LINE);
    ui_setDispmode(DISPMODE_FF_INFO, ln2);

    UNUSED_OK(state);
    return false;
}

/** \brief called when display exit a menu cycle of FF info, to
        show part 2 of DO block.
*/
bool_t ui_ff_Tag2DO(const uistate_t *state)
{
    const IPC_FFDOParams_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1];

    pVar = GetDoBlockVar();
    mn_memcpy(ln2, pVar->tag + NUMBER_OF_DIGITS_PER_LINE, NUMBER_OF_DIGITS_PER_LINE);
    ui_setDispmode(DISPMODE_FF_INFO, ln2);

    UNUSED_OK(state);
    return false;
}

/** \brief called when display exit a menu cycle of FF info, to
        show part 3 of DO block.
*/
bool_t ui_ff_Tag3DO(const uistate_t *state)
{
    const IPC_FFDOParams_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1];

    pVar = GetDoBlockVar();
    mn_memcpy(ln2, pVar->tag + NUMBER_OF_DIGITS_PER_LINE *2, NUMBER_OF_DIGITS_PER_LINE);

    ui_setDispmode(DISPMODE_FF_INFO, ln2);
    UNUSED_OK(state);
    return false;
}

/** \brief called when display exit a menu cycle of FF info, to
        show part 4 of DO block.
*/
bool_t ui_ff_Tag4DO(const uistate_t *state)
{
    const IPC_FFDOParams_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1]  = BLANK_STR_LCD_1LINE;

    pVar = GetDoBlockVar();
    CONST_ASSERT ((FF_RB_TAG_MAX_LEN -NUMBER_OF_DIGITS_PER_LINE *3) <= NUMBER_OF_DIGITS_PER_LINE);
    mn_memcpy(ln2, pVar->tag + NUMBER_OF_DIGITS_PER_LINE *3, FF_RB_TAG_MAX_LEN -NUMBER_OF_DIGITS_PER_LINE *3);
    ui_setDispmode(DISPMODE_FF_INFO, ln2);

    UNUSED_OK(state);
    return false;
}
/** \brief called when display exit a menu cycle of FF info, to
        infor mation of PID2 block.
*/
bool_t ui_ff_InfoPID2(const uistate_t *state)
{
    const IPC_FFPID2Params_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1];

    pVar = GetPid2BlockVar();
    // display the orignal digits on lcd, and will be replaced by string
    ff_Mode2Str(pVar->ModeActual, ln2);
    ff_Err2Str(pVar->BlockErr, &ln2[FFSTR_OFFSET_ERR]);
    ui_setDispmode(DISPMODE_FF_INFO, ln2);

    UNUSED_OK(state);
    return false;
}

/** \brief called when display exit a menu cycle of FF info, to
        part 1 of PID2 block.
*/
bool_t ui_ff_Tag1PID2(const uistate_t *state)
{
    const IPC_FFPID2Params_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1];

    pVar = GetPid2BlockVar();
    mn_memcpy(ln2, pVar->tag, NUMBER_OF_DIGITS_PER_LINE);
    ui_setDispmode(DISPMODE_FF_INFO, ln2);

    UNUSED_OK(state);
    return false;
}

/** \brief called when display exit a menu cycle of FF info, to
        show part 2 of PID2 block.
*/
bool_t ui_ff_Tag2PID2(const uistate_t *state)
{
    const IPC_FFPID2Params_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1];

    pVar = GetPid2BlockVar();
    mn_memcpy(ln2, pVar->tag + NUMBER_OF_DIGITS_PER_LINE, NUMBER_OF_DIGITS_PER_LINE);
    ui_setDispmode(DISPMODE_FF_INFO, ln2);

    UNUSED_OK(state);
    return false;
}

/** \brief called when display exit a menu cycle of FF info, to
        show part 3 of PID2 block.
*/
bool_t ui_ff_Tag3PID2(const uistate_t *state)
{
    const IPC_FFPID2Params_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1];

    pVar = GetPid2BlockVar();
    mn_memcpy(ln2, pVar->tag + NUMBER_OF_DIGITS_PER_LINE *2, NUMBER_OF_DIGITS_PER_LINE);

    ui_setDispmode(DISPMODE_FF_INFO, ln2);
    UNUSED_OK(state);
    return false;
}

/** \brief called when display exit a menu cycle of FF info, to
        show part 4 of PID2 block.
*/
bool_t ui_ff_Tag4PID2(const uistate_t *state)
{
    const IPC_FFPID2Params_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1]  = BLANK_STR_LCD_1LINE;

    pVar = GetPid2BlockVar();
    CONST_ASSERT ((FF_PID2_TAG_MAX_LEN -NUMBER_OF_DIGITS_PER_LINE *3) <= NUMBER_OF_DIGITS_PER_LINE);
    mn_memcpy(ln2, pVar->tag + NUMBER_OF_DIGITS_PER_LINE *3, FF_PID2_TAG_MAX_LEN -NUMBER_OF_DIGITS_PER_LINE *3);
    ui_setDispmode(DISPMODE_FF_INFO, ln2);

    UNUSED_OK(state);
    return false;
}
/** \brief called when display exit a menu cycle of FF info, to
        infor mation of DO2 block.
*/
bool_t ui_ff_InfoDO2(const uistate_t *state)
{
    const IPC_FFDO2Params_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1];

    pVar = GetDo2BlockVar();
    // display the orignal digits on lcd, and will be replaced by string
    ff_Mode2Str(pVar->ModeActual, ln2);
    ff_Err2Str(pVar->BlockErr, &ln2[FFSTR_OFFSET_ERR]);
    ui_setDispmode(DISPMODE_FF_INFO, ln2);

    UNUSED_OK(state);
    return false;
}

/** \brief called when display exit a menu cycle of FF info, to
        part 1 of DO2 block.
*/
bool_t ui_ff_Tag1DO2(const uistate_t *state)
{
    const IPC_FFDO2Params_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1];

    pVar = GetDo2BlockVar();
    mn_memcpy(ln2, pVar->tag, NUMBER_OF_DIGITS_PER_LINE);
    ui_setDispmode(DISPMODE_FF_INFO, ln2);

    UNUSED_OK(state);
    return false;
}

/** \brief called when display exit a menu cycle of FF info, to
        show part 2 of DO2 block.
*/
bool_t ui_ff_Tag2DO2(const uistate_t *state)
{
    const IPC_FFDO2Params_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1];

    pVar = GetDo2BlockVar();
    mn_memcpy(ln2, pVar->tag + NUMBER_OF_DIGITS_PER_LINE, NUMBER_OF_DIGITS_PER_LINE);
    ui_setDispmode(DISPMODE_FF_INFO, ln2);

    UNUSED_OK(state);
    return false;
}

/** \brief called when display exit a menu cycle of FF info, to
        show part 3 of DO2 block.
*/
bool_t ui_ff_Tag3DO2(const uistate_t *state)
{
    const IPC_FFDO2Params_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1];

    pVar = GetDo2BlockVar();
    mn_memcpy(ln2, pVar->tag + NUMBER_OF_DIGITS_PER_LINE *2, NUMBER_OF_DIGITS_PER_LINE);

    ui_setDispmode(DISPMODE_FF_INFO, ln2);
    UNUSED_OK(state);
    return false;
}

/** \brief called when display exit a menu cycle of FF info, to
        show part 4 of DO2 block.
*/
bool_t ui_ff_Tag4DO2(const uistate_t *state)
{
    const IPC_FFDO2Params_t* pVar;
    u8 ln2[NUMBER_OF_DIGITS_PER_LINE + 1]  = BLANK_STR_LCD_1LINE;

    pVar = GetDo2BlockVar();
    CONST_ASSERT ((FF_DO2_TAG_MAX_LEN -NUMBER_OF_DIGITS_PER_LINE *3) <= NUMBER_OF_DIGITS_PER_LINE);
    mn_memcpy(ln2, pVar->tag + NUMBER_OF_DIGITS_PER_LINE *3, FF_DO2_TAG_MAX_LEN -NUMBER_OF_DIGITS_PER_LINE *3);
    ui_setDispmode(DISPMODE_FF_INFO, ln2);

    UNUSED_OK(state);
    return false;
}

/** \brief called when display enter a menu cycle of simulation, to
        enable/disable simulation sub node.

bool_t ui_IsSimulationActive(const uistate_t *state)
{
    IPC_FFDeviceParams_t* pVar;

    UNUSED_OK(state);
    pVar = GetDeviceVar();
    return (pVar->lui_simulation != SIMULATION_ACTIVE);

}*/

/** \brief called when display enter a menu cycle of simulation, to
        enable/disable simulation sub node.

bool_t ui_IsSimulationDeactive(const uistate_t *state)
{
    IPC_FFDeviceParams_t* pVar;

    UNUSED_OK(state);
    pVar = GetDeviceVar();
    return (pVar->lui_simulation != SIMULATION_DEACTIVE);
}*/

/** \brief called when display exit a menu cycle of simulation, to
        active simulation.
*/
bool_t ui_SimulationActive(const uistate_t *state)
{
    FFData_t ffdata;
    ErrorCode_t err;

    (void)ff_GetLocalFFData(&ffdata);
    ffdata.SimulationActive = true;
    err = ff_RepairSetFFData(&ffdata);

    UNUSED_OK(state);
    return (err != ERR_OK);
}

/** \brief called when display exit a menu cycle of simulation, to
        deactive simulation.
*/
bool_t ui_SimulationDeactive(const uistate_t *state)
{
    FFData_t ffdata;
    ErrorCode_t err;

    (void)ff_GetLocalFFData(&ffdata);
    ffdata.SimulationActive = false;
    err = ff_RepairSetFFData(&ffdata);
    UNUSED_OK(state);
    return (err != ERR_OK);
}

/* This line marks the end of the source */

