/**
    \file hart_digsp.c
    \brief HART interfaces for digital setpoint API

    CPU: Any

    OWNER: AK

    \ingroup HARTapp
*/
#include "mnwrap.h"
#include "numutils.h"
#include "hartfunc.h"
#include "hartcmd.h"
#include "hartdef.h"
#include "fpconvert.h"
#include "bufutils.h"
#include "hartpriv.h"
#include "digitalsp.h"
#include "devicemode.h"
#include "ctllimits.h"
#include "faultpublic.h"
#include "poscharact.h"

s8_least hartcmd_WriteDigitalSetpoint(const u8 *src, u8 *dst)
{
    const Req_WriteDigitalSetpoint_t *s = (const void *)src;
    Rsp_WriteDigitalSetpoint_t *d = (void *)dst;
    s32 sp = fpconvert_FloatBufferToInt32Clamped(s->DigitalSetpoint[0], UNITSID_PERCENT, digitalsp_range);
    u8 xmode = util_GetU8(s->TargetBlockMode[0]);
    ErrorCode_t err = digsp_SetDigitalSetpointEx(xmode, sp); //direct characterization done there
    UNUSED_OK(err);
    //Now, stick the result to manual setpoint
    sp = digsp_GetDigitalPosSetpoint();
    mode_SetControlMode(CONTROL_MANUAL_POS, sp);

    (void)fpconvert_IntToFloatBuffer(digsp_GetDigitalSetpoint(), UNITSID_PERCENT, d->DigitalSetpoint[0]);

    util_PutU8(d->TargetBlockMode[0], digsp_GetData(NULL)->xmode);
    xmode = digsp_GetExternalMode();
    util_PutU8(d->EffectiveBlockMode[0], (u8)xmode);
    util_PutU8(d->SetEffectiveBlockModeAsTarget[0], 0u);

    sp = digsp_GetDigitalPosSetpoint();
    (void)fpconvert_IntToFloatBuffer(sp, UNITSID_POSITION, d->CharacterizedDigitalSetpoint[0]);
    pos_least_t pos = vpos_GetScaledPosition();
    pos = poscharact_Inverse(pos);  //inverse characterization
    (void)fpconvert_IntToFloatBuffer(pos, UNITSID_POSITION, d->InverselyCharacterizedPosition[0]);

    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

s8_least hartcmd_WriteBlockModeConfiguration(const u8 *src, u8 *dst)
{
    const Req_WriteBlockModeConfiguration_t *s = (const void *)src;

    Rsp_WriteBlockModeConfiguration_t *d = (void *)dst;
    //Rely on read function to fill in the data referenced here
    UNUSED_OK(d->TBFixedSetpoint);
    UNUSED_OK(d->TBShedTime);
    UNUSED_OK(d->TBInitTime);
    UNUSED_OK(d->TBSPFailureTargetMode);
    UNUSED_OK(d->TBSPoption);

    DigitalSpConf_t conf;
    (void)digsp_GetConf(&conf);
    s32 sp = fpconvert_FloatBufferToInt32Clamped(s->TBFixedSetpoint[0], UNITSID_PERCENT, digitalsp_range);
    conf.FixedSetpoint = poscharact_Direct(sp);
    conf.ShedTime = (tick_t)fpconvert_FloatBufferToInt32Clamped(s->TBShedTime[0], UNITSID_SHEDTIME, DigitalSP_ShedTime_range);
    conf.InitTime = (tick_t)fpconvert_FloatBufferToInt32Clamped(s->TBInitTime[0], UNITSID_SHEDTIME, DigitalSP_ShedTime_range);
    conf.IsTargetToManual = int2bool(util_GetU8(s->TBSPFailureTargetMode[0]));
    conf.sp_option = util_GetU8(s->TBSPoption[0]);
    //Complete the command

    ErrorCode_t err = digsp_SetConf(&conf);
    s8_least ret;
    if(err != ERR_OK)
    {
        ret = err2hart(err);
    }
    else
    {
        ret = hartcmd_ReadBlockModeConfiguration(src, dst);
    }
    return ret;
}

s8_least hartcmd_ReadBlockModeConfiguration(const u8 *src, u8 *dst)
{
    const Req_ReadBlockModeConfiguration_t *s = (const void *)src;
    UNUSED_OK(s);
    Rsp_ReadBlockModeConfiguration_t *d = (void *)dst;
    DigitalSpConf_t conf;
    (void)digsp_GetConf(&conf);
    (void)fpconvert_IntToFloatBuffer(poscharact_Inverse(conf.FixedSetpoint), UNITSID_PERCENT, d->TBFixedSetpoint[0]);

    //casts OK because of allowed range
    (void)fpconvert_IntToFloatBuffer((s32)conf.ShedTime, UNITSID_SHEDTIME, d->TBShedTime[0]);
    (void)fpconvert_IntToFloatBuffer((s32)conf.InitTime, UNITSID_SHEDTIME, d->TBInitTime[0]);

    util_PutU8(d->TBSPFailureTargetMode[0], bool2int(conf.IsTargetToManual));
    util_PutU8(d->TBSPoption[0], conf.sp_option);

    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

s8_least hartcmd_SetFailedState(const u8 *src, u8 *dst)
{
    const Req_SetFailedState_t *s = (const void *)src;
    Rsp_SetFailedState_t *d = (void *)dst;
    UNUSED_OK(s);
    UNUSED_OK(d);
    error_SetFault(FAULT_FSTATE_REQ);
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

s8_least hartcmd_ClearFailedState(const u8 *src, u8 *dst)
{
    const Req_ClearFailedState_t *s = (const void *)src;
    Rsp_ClearFailedState_t *d = (void *)dst;
    UNUSED_OK(s);
    UNUSED_OK(d);
    error_ClearFaults(false); //TBD if a finer clearing mechanism is required
    error_Mopup(); //do it here in case reset is coming
    ErrorCode_t err = mode_SetMode(MODE_OPERATE); //Note: Bypassing mode_SetNormalMode wrapper intentionally
    return err2hart(err);
}

/* This line marks the end of the source */
