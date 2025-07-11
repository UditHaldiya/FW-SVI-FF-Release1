/*
Copyright 2010 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file hart_ext_analysis.c
    \brief HART glue layer for control extended analysis command
           ("X-ray view" into control process)

    CPU: Any

    OWNER: SK
    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue/hart_ext_analysis.c $
    $Date: 5/20/11 8:17p $
    $Revision: 20 $
    $Author: Justin Shriver $

    \ingroup HARTapp
*/

/* $History: hart_ext_analysis.c $
 *
 * *****************  Version 20  *****************
 * User: Justin Shriver Date: 5/20/11    Time: 8:17p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * TFS:6337
 *
 * *****************  Version 19  *****************
 * User: Justin Shriver Date: 5/20/11    Time: 5:21p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * TFS:6337
 *
 * *****************  Version 18  *****************
 * User: Justin Shriver Date: 5/18/11    Time: 10:21a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * TFS:6337
 *
 * *****************  Version 17  *****************
 * User: Justin Shriver Date: 5/16/11    Time: 6:05p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * TFS:6337
 *
 * *****************  Version 16  *****************
 * User: Justin Shriver Date: 5/16/11    Time: 3:26p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * TFS:6337
 *
 * *****************  Version 15  *****************
 * User: Justin Shriver Date: 5/16/11    Time: 9:19a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * TFS:6305 LINT
 *
 * *****************  Version 14  *****************
 * User: Justin Shriver Date: 5/13/11    Time: 6:13p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * TFS:6305 LINT
 *
 * *****************  Version 13  *****************
 * User: Justin Shriver Date: 5/13/11    Time: 5:35p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * TFS:5384 Instrument Autotune Temporarily
 *
 * *****************  Version 12  *****************
 * User: Sergey Kruss Date: 3/18/11    Time: 2:27p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * TFS:5899 Changed cmd-242 major version to 0xB9
 *
 * *****************  Version 11  *****************
 * User: Sergey Kruss Date: 3/02/11    Time: 12:18p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * TFS:5746 - CalcFreq is an erroneous term: it must be period of adding
 * integral.
 *
 * *****************  Version 10  *****************
 * User: Justin Shriver Date: 1/13/11    Time: 12:01p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * TFS:5384 Add debug capabilities to autotune
 *
 * *****************  Version 9  *****************
 * User: Sergey Kruss Date: 11/19/10   Time: 12:01p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * TFS:4761--Final checkin for command 242,2
 *
 * *****************  Version 8  *****************
 * User: Sergey Kruss Date: 11/17/10   Time: 4:20p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * TFS:4761--Added command 242,2 for observing position propagation
 * through all transformations from A/D converter.
 *
 * *****************  Version 7  *****************
 * User: Sergey Kruss Date: 10/01/10   Time: 11:52a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * TFS:4226. Changed Cotnrol Output related variables to less misleading.
 *
 * *****************  Version 6  *****************
 * User: Sergey Kruss Date: 9/24/10    Time: 10:09a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * TFS:4180
 * Fixed cmd-242,1 parameters types.
 *
 * *****************  Version 5  *****************
 * User: Sergey Kruss Date: 8/31/10    Time: 12:44p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart/glue
 * TFS:4016
 * -Added non-normalized value of PWM
 * -Changed the version to Alpha 9
 *
 * *****************  Version 4  *****************
 * User: Sergey Kruss Date: 8/07/10    Time: 3:17p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart/glue
 * TFS:3520, TFS:3596 -- Separate terms P and D and add Boost term
 *
 * *****************  Version 3  *****************
 * User: Sergey Kruss Date: 7/28/10    Time: 12:42p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart/glue
 * TFS-3637: Command 242 is modified so that subcommand 0 returns version.
 * Workhorse is moved to subcommand 1.
 *
 * *****************  Version 2  *****************
 * User: Sergey Kruss Date: 7/16/10    Time: 5:07p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart/glue
 * Fixed for Lint problems
 *
 * *****************  Version 1  *****************
 * User: Sergey Kruss Date: 6/25/10    Time: 1:51p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart/glue
 * Modified HART command 242, implemented it with XML and moved from
 * hartfunc.c
 *

*/

#include "mnwrap.h"
#include "hartfunc.h"
#include "hartdef.h"
#include "bufutils.h"
#include "ext_analysis.h"

#include "adtype.h"
#include "adtbl.h"
#include "sysio.h"
#include "pressures.h"
#include "tempr.h"
#include "posint.h"
#include "posext.h"
#include "position.h"


/**
    \brief Extended Analysis Data Collector
           Note:
           This command is allowed in all modes
*/
//this is a command shell -- does nothing
s8_least hartcmd_ExtendedAnalysis(const u8 *src, u8 *dst)
{
    UNUSED_OK(src);
    UNUSED_OK(dst);
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
} //-----  end of hart_Command_242_ExtendedAnalysis() ------

//subcommand 0 -- returns a current version
//  first word -- version of the firmware, such as: 0xA8 -- Alpha_8, 0xB2 -- Beta_2, 0xC1 -- Release Candidate_1
//  second word -- if needed, the sub-version within the firmware version, if not needed -- zero
#define CMD242_VERSION_FW  0xB9   // Beta-9  /** TFS:5899 */
#define CMD242_VERSION_CMD  0x01 //row-based tempcomp and no magnet comp

s8_least hartcmd_ExtendedAnalysisVersion(const u8 *src, u8 *dst)
{
    UNUSED_OK(src);
    Rsp_ExtendedAnalysisVersion_t *d = (void *)dst;

    util_PutU16(d->eav_Version[0], (u16)CMD242_VERSION_FW);     //FW version
    util_PutU16(d->eav_Version[1], (u16)CMD242_VERSION_CMD);    //sub-version

    return HART_NO_COMMAND_SPECIFIC_ERRORS;
} //-----  end of hartcmd_ExtendedAnalysisVersion() ------


//subcommand 1 -- working horse
s8_least hartcmd_ExtendedAnalysisDataCollector(const u8 *src, u8 *dst)
{
    UNUSED_OK(src);
    Rsp_ExtendedAnalysisDataCollector_t *d = (void *)dst;
    ExtAnalysParams_t Params;

    ctlmode_t ctlmode;
    s32 sp;

    ctlmode = mode_GetEffectiveControlMode(&sp); //provides conditioned setpoint and translated control mode

    //obtain pointer to parameters from control.c
    control_FillExtAnalysParams(&Params);

    //assign the whole structure to HART buffer
    util_PutU8(d->ControlAlgorithmMode[0], (u8)ctlmode);
    util_PutS16(d->eadc_RateLimitedSetpoint[0], (s16)sp);

    (void)mode_GetIntendedControlMode(&sp); //user setpoint
    util_PutS16(d->eadc_UserSetpoint[0], (s16)sp);

    util_PutS16(d->eadc_PositionScaled[0], vpos_GetScaledPosition());
    util_PutU16(d->eadc_PWM[0], sysio_GetRealPWM());
    /** TFS:4016 **/  /** TFS:4226 */
    util_PutU16(d->eadc_CtlOutput[0], Params.CtlOutput);

    const BoardPressure_t *pr = pres_GetRawPressureData();
    util_PutS16(d->eadc_P_pilot[0], pr->Pressures[PRESSURE_PILOT_INDEX]);
    util_PutS16(d->eadc_P_1[0], pr->Pressures[PRESSURE_ACT1_INDEX]);
    util_PutS16(d->eadc_P_2[0], pr->Pressures[PRESSURE_ACT2_INDEX]);
    util_PutS16(d->eadc_P_supply[0], pr->Pressures[PRESSURE_SUPPLY_INDEX]);

    util_PutS16(d->eadc_AvgErr[0], Params.AvgErr);
    util_PutS16(d->eadc_MinErr[0], Params.MinErr);
    util_PutS16(d->eadc_MaxErr[0], Params.MaxErr);
    util_PutU16(d->eadc_PosComp[0], Params.PosComp);
    util_PutS16(d->eadc_P_element[0], Params.P_term);
    util_PutS16(d->eadc_D_element[0], Params.D_term);
    util_PutS16(d->eadc_Boost_element[0], Params.Boost_term);
    util_PutS16(d->eadc_Fast_elements[0], Params.Fast_terms);
    util_PutS32(d->eadc_Integral[0], Params.Integral);
    util_PutU16(d->eadc_Bias[0], Params.Bias);
    util_PutU16(d->eadc_Overshoots[0], Params.Overshoots);
    util_PutU16(d->eadc_JigglesPassed[0], Params.JigglesPassed);
    util_PutU16(d->eadc_CalcPeriod[0], Params.CalcPeriod);    // TFS:5746
    util_PutU8(d->eadc_IntegalCount[0], Params.IntegalCount);
    util_PutU8(d->eadc_CtrlByte[0], Params.CtrlByte);
    util_PutU16(d->eadc_BitPack[0], Params.BitPack);

    return HART_NO_COMMAND_SPECIFIC_ERRORS;
} //-----  end of hartcmd_ExtendedAnalysisDataCollector() ------


//subcommand 2 -- returns position propagation chain:
//ADraw --> TempComp --> Magnetic Corrected --> Linearized --> Std Scaled
s8_least hartcmd_ExtendedAnalysisPosPropagation(const u8 *src, u8 *dst)
{
    UNUSED_OK(src);
    Rsp_ExtendedAnalysisPosPropagation_t *d = (void *)dst;

    OffsetSpanS32_t curveseg;
    PosState_t PosState;
    (void)pos_GetPosState(&PosState);

    u8 index;
    s32 pos_prelin;
    if(ad_GetSensorType(NULL)->IsRemotePositionSensor)
    {
        posext_GetInstantTempCompParams(&curveseg);
        index = AD_POS_REM;
        pos_prelin = PosState.hardpos;
    }
    else
    {
        posint_GetInstantTempCompParams(&curveseg);
        index = AD_POS_INT;
        pos_prelin = posint_GetPreLinearized();
    }
    util_PutS32(d->eapp_posTCcalibrated[0], pos_prelin);
    const AdDataRaw_t* pRawData = bios_GetAdRawData();
    util_PutS32(d->eapp_posADraw[0], pRawData->AdValue[index]);
    util_PutU16(d->eapp_posLinearized[0], (u16)PosState.hardpos);
    util_PutS32(d->eapp_posScaled[0], PosState.pos_adjusted);

    const BoardPressure_t* pBoardPressure = pres_GetPressureData();
    util_PutS16(d->eapp_P_pilot[0], pBoardPressure->Pressures[PRESSURE_PILOT_INDEX]);

    util_PutS32(d->eapp_temperRaw[0], pRawData->AdValue[AD_TEMP_INT]);
    util_PutS32(d->eapp_temperTCcomp[0], tempr_GetInstantTempr(0));

    util_PutU16(d->eapp_PWM[0], sysio_GetRealPWM());

    util_PutS32(d->eapp_posTCspan[0], curveseg.span);
    util_PutS32(d->eapp_posTCoffset[0], curveseg.offset);

    return HART_NO_COMMAND_SPECIFIC_ERRORS;
} //-----  end of hartcmd_ExtendedAnalysisPosPropagation() ------



