/**
    \file hart_sigsp.c
    \brief HART glue layer for signal setpoint interfaces

    CPU: Any

    OWNER: AK

    \ingroup HARTapp
*/
#include "mnwrap.h"
#include "mncbtypes.h"
#include "numutils.h"
#include "hartfunc.h"
#include "hartcmd.h"
#include "hartdef.h"
#include "ctllimits.h"
#include "devicemode.h"
#include "bufutils.h"
#include "charact.h"
#include "insignal.h"
#include "fpconvert.h"
#include "signalsp.h"

/** \brief A deprecated legacy function for not-necessarily-factory test:
I/P current directly follows input signal (loop current)
*/
s8_least hartcmd_SetDiagnosticSetpoint(const u8 *src, u8 *dst)
{
    const Req_SetDiagnosticSetpoint_t *s = (const void *)src;
    Rsp_SetDiagnosticSetpoint_t *d = (void *)dst;
    UNUSED_OK(s); //no parameters
    UNUSED_OK(d); //no parameters
    mode_SetControlMode(CONTROL_IP_DIAGNOSTIC, 0); //setpoint don't care
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}


s8_least hartcmd_WriteSimulatedSignalSetpoint(const u8 *src, u8 *dst)
{
    s8_least ret;
    sig_least_t nSignal;
    const Req_WriteSimulatedSignalSetpoint_t *s = (const void *)src;
    Rsp_WriteSimulatedSignalSetpoint_t *d = (void *)dst;

    nSignal = fpconvert_FloatBufferToInt(s->SimulatedSignalSetpoint[0], UNITSID_SIGNAL_ENTRY);
    if( (nSignal < LOW_SIGNAL_LOW_LIMIT_INT) ||
       (nSignal > HIGH_SIGNAL_HIGH_LIMIT_INT) )
    {
        ret = HART_INVALID_DATA;
    }
    else
    {
        //pSignalData = cnfg_GetComputedSignalData();
        pos_least_t Setpoint = sigsp_ConvertSignalToSetpoint(nSignal);
        mode_SetControlMode(CONTROL_MANUAL_POS, Setpoint);
        (void)fpconvert_IntToFloatBuffer(nSignal, UNITSID_SIGNAL_ENTRY, d->SimulatedSignalSetpoint[0]);

        ret = HART_NO_COMMAND_SPECIFIC_ERRORS;
    }
    return ret;
}

/* This line marks the end of the source */
