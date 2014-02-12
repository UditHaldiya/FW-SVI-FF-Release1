/**
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/tasks/diagnostics.c $
    $Date: 1/20/12 10:51a $
    $Revision: 117 $
    $Author: Arkkhasin $

    CPU: Any

    \ingroup Diagnostics
    \brief Performs valve diagnositcs.

*/

/*
    $History: diagnostics.c $
 *
 * *****************  Version 117  *****************
 * User: Arkkhasin    Date: 1/20/12    Time: 10:51a
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/tasks
 * TFS:8748 - remove old pressure-related interfaces
 * TFS:8748 - Confine pressure in floating point to the interface layer
 *
 * *****************  Version 116  *****************
 * User: Arkkhasin    Date: 11/29/11   Time: 2:26p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/tasks
 * TFS:8313 Lint
 * Range bug found by lint
 *
 * *****************  Version 115  *****************
 * User: Arkkhasin    Date: 11/15/11   Time: 10:38a
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/tasks
 * Preliminary check-in for
 * TFS:8051 new tempcomp
 * TFS:8202 decouple I/O subsystem
 * min/max position see 8200
 *
 * *****************  Version 114  *****************
 * User: Arkkhasin    Date: 11/04/11   Time: 7:34p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/tasks
 * TFS:8072 NVMEM upgrade
 *
 * *****************  Version 112  *****************
 * User: Arkkhasin    Date: 5/16/11    Time: 6:34p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks
 * TFS:6336: dummy faults necessary for the build moved away from the
 * fault matrix to dummyfaults.h
 *
 *
 * *****************  Version 111  *****************
 * User: Justin Shriver Date: 7/28/10    Time: 6:59p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks
 * TFS:3659 Number of curves
 *
 * *****************  Version 110  *****************
 * User: Justin Shriver Date: 7/28/10    Time: 6:22p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks
 * TFS:3543 -- Integrate 3.2.1 diagnostics into LCX.
 * TFS:3599 -- Remove unnecessary header guards
 * TFS:3656 -- Integration into new LCX structures
 *
 * *****************  Version 113  *****************
 * User: Anatoly Podpaly Date: 6/10/10    Time: 11:39a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/tasks
 * Bug 3137: added code to copy the Speed to he structure used for
 * reporting.
 *
 * *****************  Version 112  *****************
 * User: Justin Shriver Date: 4/12/10    Time: 1:26p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/tasks
 * Remove unnecessary comment
 *
 * *****************  Version 110  *****************
 * User: Justin Shriver Date: 3/09/10    Time: 12:41p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/tasks
 * TFS: 2720
 * REQ: 21.26
 *
 * *****************  Version 109  *****************
 * User: Arkkhasin    Date: 3/30/10    Time: 6:40p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks
 * Constrained the use of nvramtypes.h
 *
 * *****************  Version 108  *****************
 * User: Arkkhasin    Date: 3/27/10    Time: 7:13a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks
 * charact_CharacterizationTableLookup() no longer takes Characterization
 * as a parameter
 *
 * *****************  Version 107  *****************
 * User: Arkkhasin    Date: 3/24/10    Time: 2:30p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks
 * single-acting detection routed to pneumatic params
 *
 * *****************  Version 106  *****************
 * User: Arkkhasin    Date: 3/08/10    Time: 1:57p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks
 *         - remove #if NVRAM_NUMBER_DIAGNOSTIC_CURVES !=
 * NUMBER_DIAGNOSTIC_CURVES
 *
 * *****************  Version 105  *****************
 * User: Arkkhasin    Date: 2/25/10    Time: 12:34a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks
 * TestProtectLimits() got a new prefix
 *
 * *****************  Version 104  *****************
 * User: Justin Shriver Date: 2/01/10    Time: 3:52p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks
 * TFS:2489
 * TFS:2259
 * TFS:2543
 *
 * *****************  Version 103  *****************
 * User: Justin Shriver Date: 2/01/10    Time: 3:49p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/tasks
 * TFS:2489
 * TFS:2259
 * TFS:2543
 *
 * *****************  Version 102  *****************
 * User: Justin Shriver Date: 1/27/10    Time: 7:22p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/tasks
 * TFS:2543
 *
 * *****************  Version 101  *****************
 * User: Justin Shriver Date: 1/20/10    Time: 3:54p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/tasks
 * TFS:2500
 *
 * *****************  Version 100  *****************
 * User: Justin Shriver Date: 1/15/10    Time: 4:03p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/tasks
 * TFS:2489
 *
 * *****************  Version 99  *****************
 * User: Justin Shriver Date: 1/15/10    Time: 3:25p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/tasks
 * TFS:2484
 * TFS:2485
 * TFS:2486
 * TFS:2487
 *
 * *****************  Version 98  *****************
 * User: Anatoly Podpaly Date: 1/12/10    Time: 3:14p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/tasks
 * Bug 2305: the file has been updated to the standard headers.
 *
 * *****************  Version 91  *****************
 * User: Anatoly Podpaly Date: 1/06/10    Time: 6:33p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/tasks
 * LINT correction
 *
 * *****************  Version 90  *****************
 * User: Anatoly Podpaly Date: 1/06/10    Time: 6:17p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/tasks
 * Merge: added ExtSig Universal.
 *
 * *****************  Version 89  *****************
 * User: Arkkhasin    Date: 1/05/10    Time: 10:46p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/tasks
 * Interim check-in for further cleanup and refactoring
 * 1. Data sampling into linear self-pruning buffer
 * 2. Aux termination conditions removed (JS)
 * 3. Bias_Low, Bias_High are static duration again
 * 4. Misc. minor cleanup
 *
 * *****************  Version 88  *****************
 * User: Anatoly Podpaly Date: 12/23/09   Time: 4:29p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/tasks
 * Bug #2305: corrected headers.
 * Bug #2331: execute fnction (void)CheckForChange(Position,
 * &LastPosition, Pressure, &LastPressure, &bHasBeenOffStops);
*/

#include "mnwrap.h"
#include "oswrap.h"
//#include "mnassert.h"

#include "nvram.h"
#include "timer.h"
//#include "pwm.h"
#include "faultpublic.h"
//#include "faultdef.h"
#include "timebase.h"
#include "utility.h"
#include "control.h"
#include "cycle.h"
#include "process.h"
#include "bufferhandler.h"
#include "uipublic.h"
#include "uistartnodes.h"
#include "sysio.h"

#include "param.h"
#include "devicemode.h"
#include "pneumatics.h"
#include "ctllimits.h"
#include "poscharact.h"
//#include "dummyfaults.h"

#include "diagnostics.h"
#include "vramptest.h"

#include "diagnosticsUniversal_sys.h"
/** private defines */




#define COMPLETE ((u8)100)
//#define HALF_COMPLETE ((u8)50)

//START REQ--21.00.0030 note speed in % per 4 seconds
//#define SPEED_LOW_ACT 1u
//#define SPEED_HIGH_ACT 200u
//END REQ--21.00.0030
#define SPEED_LOW_STEP 1u  //used by autotune
#define DIAG_SPEED_LOW_STEP 2u  //used by diagnostics
#define SPEED_HIGH_STEP 60u
//#define DIAG_POS_DIF ((s32)((float32)STANDARD_RANGE*0.10F))  //10.0F
#define DIAG_SAMPLE_RATE_LOW_LIMIT 2u
#define DIAG_SAMPLE_RATE_HIGH_LIMIT 20u
#define STEPTEST_HEADERSZ 24 //in diag_t entries; a dogmatic number to please FF params

#define EXTDIAG_HEADER_FILLER ((diag_t)0x8081)
#define DIAG_EXTDIAG_VERSION 0
CONST_ASSERT((EXTDIAG_HEADERSZ%2U)==0); //must be even

//----------- step test -------------------
#define STEPTEST_HEADER_FILLER ((diag_t)0x8081)
#define DIAG_STEP_VERSION 0
#define MILISEC_PER_SEC 1000u


//#define SKIP_COUNT 20u
/** private macros */

//lint -emacro( {725}, LIMIT_MAX,LIMIT_MIN,LIMIT) Don't require extra indentation (remove space to activate)
//lint -emacro( {640}, LIMIT_MAX,LIMIT_MIN,LIMIT) Don't break the do{}while(0) idiom
//lint -emacro( {717}, LIMIT_MAX,LIMIT_MIN,LIMIT) Don't break the do{}while(0) idiom
/*
#define LIMIT_MAX(x,y) do{ if((x) > (y)) \
{ \
    x = (y); \
} \
}while(false)

#define LIMIT_MIN(x,y) do{ if((x) < (y)) \
{ \
    x = (y); \
} \
}while(false)

#define LIMIT(l,x,h) do{ \
  LIMIT_MIN((x),(l)); \
  LIMIT_MAX((x),(h)); \
}while(false)


//#define BOOL_NOT(x)  ((x==false)?true:false)
#define BOOL_NOT(x)  (!x)
//#define BOOL(x)      ((x)?true:false)
#define BOOL(x) (x)
*/
//defaults


/** global constants */
/** member data */
        //some defaults just in case (shouldn't need them since they must be set up
        //      before being used by call to SetDiagnosticParameters)
//AK: remove initializations when robust interface is developed.
static u8      m_Type=0;
static u16     m_nSpeed=2;                  //( %/4sec) -  time for step tests
static pos_t   m_nStartPosition=6554;       //40.0F;
static pos_t   m_nEndPosition= 9830;        //60.0F;
//static u8      m_Flag=0u;                   //(both direction flag for step test - Phase III)
static bool_t  m_bParametersAreValid = false;
static pos_t   m_SetpointRampSpeed = 164u;    //default to 1%
static u16     m_SamplesFirstDirection = 0u;  //the number of samples in the first direction
static u16     m_FirstDirectionPruneScale;    //the prune scale in the first direction
static u16     m_FinalPruneScale;             //the prune scale in the second direction (or 1st if only one way)
static u16     m_NumberOfPoints = 0;          //number of points in the diag buffer
static u16     m_SamplingTime = 2u;           //sample time for step tests

static u8 m_DiagDirection=0u;                 //both direction flag
static u8 m_DiagControlOption;


/** private function prototypes */

static procresult_t diag_Perform_StepTest_Internal(size_t fill_func(void),
                                   pos_t StartPosition, pos_t EndPosition,
                                   u16 SamplingTime);

static procresult_t diag_Perform_ExtActuatorSignature(s16 *procdetails);
static procresult_t diag_Perform_ExtActuatorSignatureClosed(void);

static procresult_t diag_Run_RampTest_InternalSP(s16* procdetails);
static procresult_t diag_Run_RampTest_Internal(void (*sample_func)(diag_t data[2]), taskid_t TaskContext);
static void sample_pos_sp(diag_t *dst);
static void dsampler_SamplePosPres(diag_t data[2]);
static void ClearExtDiagHeader(void);




/** \brief validate and set starting and ending positions

  validate and set starting and ending positions
  \param[in] pos_t StartPosition - proposed start position
  \param[in] pos_t EndPosition - proposed end position
  \return ErrorCode_t - OK or ERR_DIAG_LIMITS_ON or ERR_INVALID_PARAMETER
*/
static ErrorCode_t diag_SetPositionRange(pos_t StartPosition, pos_t EndPosition)
{
    pos_t nStartPosition, nEndPosition;
    pos_t nMaxPosition, nMinPosition;

    ///check to see if limits are on - can't run diagnostics with these on
    // if the protect limits is set.  If it is not set, the diagnostic routine
    // will turn these off automatically and then back on when it is done.
    if(control_TestProtectLimits() == PROTECTED_LIMITS)
    {
        /* Legitimate early return: Pre-requisites check */
        return ERR_DIAG_LIMITS_ON;
    }

    //convert start and end to standard range
    nStartPosition = StartPosition; //convert_FPositionToStdPosition(fStartPosition);
    nEndPosition = EndPosition; //convert_FPositionToStdPosition(fEndPosition);

    //convert the stops to standard range and extend by 5%
    ComputedPositionConf_t PosConf;
    (void)pos_GetComputedPositionConf(&PosConf);
    nMinPosition = pos_AdjustStdPositionExt(STANDARD_ZERO, &PosConf);
    nMaxPosition = pos_AdjustStdPositionExt(STANDARD_100, &PosConf);
    if(nMinPosition > nMaxPosition)
    {
        pos_t nTemp = nMinPosition;
        nMinPosition = nMaxPosition;
        nMaxPosition = nTemp;
    }
    nMinPosition -= (POSITION_05+1);
    nMaxPosition += (POSITION_05+1);

    ///validate the input data

    //validate the start and end
    if( (nStartPosition < nMinPosition) ||
        (nStartPosition > nMaxPosition) ||
        (nEndPosition < nMinPosition) ||
        (nEndPosition > nMaxPosition)   )
    {
        /* Legitimate early return: Pre-requisites check */
        return ERR_INVALID_PARAMETER;
    }
    m_nStartPosition = nStartPosition;
    m_nEndPosition = nEndPosition;
    return ERR_OK;
}

//------------ legacy step test for autotune -------------------------
static u16     m_SampleRate=10u;          //samples per second for step test

/** \brief stores the diagnostic parameters prior to running a diagnostic

  returns stores the diagnostic parameters prior to running a diagnostic
  \param[in] u8_least Type
  \param[in] u16 Speed - test speed in ( %/4sec), time for step tests
  \param[in] pos_t StartPosition - starting test position
  \param[in] pos_t EndPosition - ending test position
  \param[in] u16 SampleRate - number of test samples per second for step test
  \param[in] u8 n1Flag - reserved
  \param[in] pos_t Parameter
  \return an error code
*/
ErrorCode_t diag_SetDiagnosticParameters(u8_least Type, u16_least nSpeed, pos_t StartPosition,
    pos_t EndPosition, u16_least SampleRate)
{

    m_bParametersAreValid = false;

    ErrorCode_t err = diag_SetPositionRange(StartPosition, EndPosition);
    if(err != ERR_OK)
    {
        return err;
    }

    //validate diagnostic type specific parameter limits
#if 0
    if(Type == DIAG_STEP)
#endif
    {
        if( (nSpeed < SPEED_LOW_STEP) || (nSpeed > SPEED_HIGH_STEP) )
        {
            /* Legitimate early return: Pre-requisites check */
            return ERR_INVALID_PARAMETER;
        }
        if( (SampleRate < DIAG_SAMPLE_RATE_LOW_LIMIT) ||
            (SampleRate > DIAG_SAMPLE_RATE_HIGH_LIMIT) )
        {
            /* Legitimate early return: Pre-requisites check */
            return ERR_INVALID_PARAMETER;
        }

        //check max number of samples for step test
        if((nSpeed * SampleRate) > ((DIAGNOSTIC_BUFFER_SIZE - STEPTEST_HEADERSZ) - 1)) //-1 because last entry is not usable
        {
            /* Legitimate early return: Pre-requisites check */
            return ERR_INVALID_PARAMETER;
        }
    }

    ///store the input data
    m_Type = (u8)Type;
    m_nSpeed = (u16)nSpeed;
    m_SampleRate = (u16)SampleRate;
    //UNUSED_OK(m_SampleRate); //may be used in the future

    m_bParametersAreValid = true;
    return ERR_OK;
}


/** \brief runs a legacy step test for autotune

  runs a step test. stores the results in the diagnostic buffer
*/
bool_t diag_Perform_StepTest(void)
{
    s16 Position;
    u32 Time, TimeStart, TimeToSample, ElapsedTime, TimePerSample;
    u16 Samples, SampleCount = 0;
    bool_t bBreak = false, ret = false;

    //Store start position, end position
    buffer_InitializeDiagnosticBuffer();
#if 0
    if(m_bForAutotune)
#endif
    {
        if(buffer_AddDiagnosticData1((s16)0))
        {
            //Legitimate early return: - error cleanup handled in calling routine - diag_Run_StepTest
            return false;
        }
        if((m_nEndPosition - m_nStartPosition)> 0)
        {
            if(buffer_AddDiagnosticData1((s16)(m_nEndPosition - m_nStartPosition)))
            {
                //Legitimate early return: - error cleanup handled in calling routine - diag_Run_StepTest
                return false;
            }
        }
        else
        {
            if(buffer_AddDiagnosticData1((s16)(m_nStartPosition - m_nEndPosition)))
            {
                //Legitimate early return: - error cleanup handled in calling routine - diag_Run_StepTest
                return false;
            }
        }
    }
#if 0
    else
    {
        if(buffer_AddDiagnosticData1((s16)m_nStartPosition))
        {
            //Legitimate early return: - error cleanup handled in calling routine - diag_Run_StepTest
            return false;
        }
        if(buffer_AddDiagnosticData1((s16)m_nEndPosition))
        {
            //Legitimate early return: - error cleanup handled in calling routine - diag_Run_StepTest
            return false;
        }
    }
#endif

    //store the time and samples
    Samples = (u16)(m_nSpeed * m_SampleRate);
    if(buffer_AddDiagnosticData1((s16)m_nSpeed))
    {
        //Legitimate early return: - error cleanup handled in calling routine - diag_Run_StepTest
        return false;
    }
    if(buffer_AddDiagnosticData1((s16)Samples))
    {
        //Legitimate early return: - error cleanup handled in calling routine - diag_Run_StepTest
        return false;
    }

    //set the starting position
    mode_SetControlMode(CONTROL_MANUAL_POS, (s32)m_nStartPosition);
    if(process_WaitForTime(T4_000))
    {
        //Legitimate early return: - error cleanup handled in calling routine - diag_Run_StepTest
        return false;
    }

    //LS 7-1-05 - changed from util_WaitStablePosPres to util_WaitStablePos
    if(!util_WaitForPos(T0_500,  STABLE_POS_TEST, true))
    {
        //Legitimate early return: - error cleanup handled in calling routine - diag_Run_StepTest
        return false;
    }
    //LS 8/4/05 added additional wait
    if(process_WaitForTime(T2_000))
    {
        //Legitimate early return: - error cleanup handled in calling routine - diag_Run_StepTest
        return false;
    }

    //loop while gathering data
    TimePerSample = MILISEC_PER_SEC/(u32)m_SampleRate;
    TimeToSample = 0u;
    TimeStart = bios_GetTimer0Ticker();


    for(;;)
    {
        if (process_WaitForTime(WAIT_TICKS) == true)
        {
            ret = false;    // early exit - user cancelled
            bBreak = true;
        }

        if(bBreak)
        {
            break;
        }

        //check for time to sample
        Time = bios_GetTimer0Ticker();
        ElapsedTime = (Time - TimeStart) * TB_MS_PER_TICK;
        if(ElapsedTime  >= TimeToSample)
        {
            //sample a point
            //Position = control_GetPosition();  //replace 8/3/05 with smoothed version
            //note - this introduces a 60ms lag which should still be ok in all but the case
            //        where the sample rate is 20 times per second.
            Position = vpos_GetScaledPosition(); //control_GetPosition();  // was smooth_GetSmoothedData(SELECTION_POSITION);
#if 0
            if(m_bForAutotune)
#endif
            {
                if(m_nEndPosition > m_nStartPosition)
                {
                    if(buffer_AddDiagnosticData1((s16)(Position - m_nStartPosition)))
                    {
                        ret = false;
                        bBreak = true;
                    }
                }
                else
                {
                    if(buffer_AddDiagnosticData1((s16)(m_nStartPosition - Position)))
                    {
                        ret = false;
                        bBreak = true;
                    }
                }
            }
#if 0
            else
            {
                if(buffer_AddDiagnosticData1((s16)Position))
                {
                    ret = false;
                    bBreak = true;
                }
            }
#endif


            if(!bBreak)
            {
                SampleCount++;
                if(SampleCount >= Samples)
                {
                    //we're finished
                    ret = true;
                    bBreak = true;
                }
                else if(SampleCount >= 11)
                {
                    //move the setpoint to the end position
                    mode_SetControlMode(CONTROL_MANUAL_POS, (s32)m_nEndPosition);
                }
                else
                {
                }
            }

            //update the timers
            TimeToSample += TimePerSample;
        }
    }

    return ret;
}



/** \brief runs a step test

  runs a step test. stores the results in the diagnostic data buffer
  \param[in] pos_t StartPosition - starting position of the test
  \param[in] pos_t EndPosition - ending position of the test
  \param[in] u16 SamplingTime - the total time of the test
  \return ErrorCode_t = ERR_OK or ERR_INVALID_PARAMETER
*/
ErrorCode_t diag_LaunchStepTest(pos_t StartPosition, pos_t EndPosition, u16 SamplingTime)
{
    //check position range values
    ErrorCode_t err = diag_SetPositionRange(StartPosition, EndPosition);

    //check sample time value
    if( (SamplingTime < DIAG_SPEED_LOW_STEP)|| (SamplingTime > SPEED_HIGH_STEP))
    {
       err = ERR_INVALID_PARAMETER;
    }

    //if OK start the test
    if(err == ERR_OK)
    {
        m_SamplingTime = SamplingTime;
        m_bParametersAreValid = true;
        m_Type = DIAG_STEP;
        err = process_SetProcessCommand(PROC_STEP_TEST);
    }
    return err;
}

/** \brief control of a step test

  control of a step test. stores the results in the diagnostic data buffer
  \param[in] size_t fill_func(void) - the function that fills in the header
  \param[in] pos_t StartPosition - starting position of the test
  \param[in] pos_t EndPosition - ending position of the test
  \param[in] u16 SamplingTime - the total time of the test
  \return ErrorCode_t = ERR_OK or ERR_INVALID_PARAMETER
*/
//new version
static procresult_t diag_Perform_StepTest_Internal(size_t fill_func(void),
                                   pos_t StartPosition, pos_t EndPosition,
                                   u16 SamplingTime)
{

    bool_t endreq;
    ErrorCode_t err;
    procresult_t ret = PROCRESULT_OK;
    tick_t SampleTimeAfterStep;

    //Store the header
    buffer_InitializeDiagnosticBuffer();

    //calculate sample time in ticks and subtract off the 1 sec before the step
    SampleTimeAfterStep = SamplingTime*LCL_TICKS_PER_SEC - T1_000;

    //set the starting position
    mode_SetControlMode(CONTROL_MANUAL_POS, StartPosition);

    //LS 7-1-05 - changed from util_WaitStablePosPres to util_WaitStablePos
    endreq = !util_WaitForPos(T0_500,  STABLE_POS_TEST, true);

    //LS 8/4/05 added additional wait
    if(!endreq)
    {
        endreq = process_WaitForTime(T2_000);
    }

    //do the test
    if(!endreq)
    {
      //start sampling
      buffer_SuspendSampling(DIAGBUF_DEFAULT);   //just in case (and to set skip=0)
      err = buffer_StartSampling(DIAGBUF_DEFAULT,
                               TASKID_CONTROL,
                               sample_pos_sp,
                               MAX_NUM_DSAMPLES(STEPTEST_HEADERSZ),
                               STEPTEST_HEADERSZ/2U, // diag_t --> dsample_t
                               NULL);
      if(err != ERR_OK)
      {
        ret = PROCRESULT_FAILED; //need to propogate this error to return
      }

      if(ret == PROCRESULT_OK)
      {
          //sample for 1 second
          endreq = process_WaitForTime(T1_000);

          //step to end position
          if(!endreq)
          {
            mode_SetControlMode(CONTROL_MANUAL_POS, EndPosition);

            //sample to the end of the test
            endreq = process_WaitForTime(SampleTimeAfterStep);
          }

          //test done - stop sampling
          buffer_StopSampling(DIAGBUF_DEFAULT);
          size_t header_len = fill_func();
          UNUSED_OK(header_len);
      }

    }

    if(endreq)
    {
      ret = PROCRESULT_CANCELED;
    }

    return ret;
}


/** \brief runs a ramp test

  runs a ramp test. stores the results in the diagnostic data buffer
  \param[in] pos_t StartPosition - starting position of the test
  \param[in] pos_t EndPosition - ending position of the test
  \param[in] pos_t SetpointRampSpeed - speed to ramp in position units
  \param[in] u8 DiagDirectio - 0=both directions, 1= one direction determined by start and end
  \return ErrorCode_t = ERR_OK or ERR_INVALID_PARAMETER or ERR_PROCESS_START
*/
ErrorCode_t diag_LaunchRampTest(pos_t StartPosition, pos_t EndPosition, pos_t SetpointRampSpeed, u8 DiagDirection)
{
    //check and store the position range of the test
    ErrorCode_t err = diag_SetPositionRange(StartPosition, EndPosition);
    if(err == ERR_OK)
    {
        //check the ramp speed and direction parameters
        if((SetpointRampSpeed < spratelim_range[Xlow]) || (SetpointRampSpeed > spratelim_range[Xhi]))
        {
            err = ERR_INVALID_PARAMETER;
        }
        if((DiagDirection != DIAGDIR_UPDOWN) && (DiagDirection != DIAGDIR_ONEWAY))
        {
             err = ERR_INVALID_PARAMETER;
        }
    }
    if(err == ERR_OK)
    {
        //store all parameters and start the test in the process task
        m_bParametersAreValid = true;
        m_Type = DIAG_RAMP;
        m_SetpointRampSpeed = SetpointRampSpeed;
        m_DiagDirection = DiagDirection;

        err = process_SetProcessCommand(PROC_DIAG_RAMP_TEST);
    }
    return err;
}

/** \brief runs a signatuare test

  runs a signatuare test. stores the results in the diagnostic data buffer
  \param[in] pos_t StartPosition - starting position of the test
  \param[in] pos_t EndPosition - ending position of the test
  \param[in] pos_t SetpointRampSpeed - the speed to move the valve during the test in pos/control cycle
  \param[in] u8 DiagDirection - 0=both directions, 1= one direction determined by start and end
  \param[in] u8 DiagControlOption - open or closed loop flag
  \return ErrorCode_t = ERR_OK or ERR_INVALID_PARAMETER or ERR_PROCESS_START
*/
ErrorCode_t diag_LaunchExtDiagnostics(pos_t StartPosition, pos_t EndPosition, pos_t SetpointRampSpeed, u8 DiagDirection, u8 DiagControlOption)
{
    //check and save the start and end positions
    ErrorCode_t err = diag_SetPositionRange(StartPosition, EndPosition);
    if(err == ERR_OK)
    {
        //check the ramp speed, direction, and options
        if((SetpointRampSpeed < spratelim_range[Xlow]) || (SetpointRampSpeed > spratelim_range[Xhi])
           || !((DiagDirection == DIAGDIR_UP) || (DiagDirection == DIAGDIR_UPDOWN))
           || !((DiagControlOption == DIAGOPT_OPENLOOP) || (DiagControlOption == DIAGOPT_CLOSEDLOOP))
               )
        {
            err = ERR_INVALID_PARAMETER;
        }
    }

    //start and end must be at least 20% apart
    if(err == ERR_OK)
    {
        if( mn_abs(m_nStartPosition - m_nEndPosition)< POSITION_20)
        {
            err = ERR_INVALID_PARAMETER;
        }
    }

    //if ok save the data and start the test in the process task
    if(err == ERR_OK)
    {
        m_bParametersAreValid = true;
        m_Type = DIAG_EXT_ACT;
        m_SetpointRampSpeed = SetpointRampSpeed;
        m_DiagDirection = DiagDirection;
        m_DiagControlOption = DiagControlOption;
        err = process_SetProcessCommand(PROC_EXT_ACT_SIGNATURE);
    }
    return err;
}



/** \brief starts an extended signature test

  starts an extended signature test. stores the results in the diagnostic buffer
  \param[in] *procdetails - data sent from process task
  \return procresult_t
*/
procresult_t diag_Run_ExtActuatorSignature(s16 *procdetails)
{
    //ctlmode_t ControlMode;
    //s32 Setpoint;
    procresult_t procresult;

    ///make sure the parameters are set
    if( (!m_bParametersAreValid) || (m_Type != (u8)DIAG_EXT_ACT) )
    {
        return PROCRESULT_FAILED;
    }

    //display "DIAG"
    ui_setNext(UINODEID_DIAG_EX);

    ///save the control mode and setpoint
    //control_GetControlMode(&ControlMode, &Setpoint);   done by wrapper now

    // ************************ NEED TO SET A/D TABLE TO DIAGNOSTIC ********** //

    ///perform an extended signature test 
    procresult = control_ProcWithNoLimits(diag_Perform_ExtActuatorSignature, procdetails);

    // ************************ NEED TO SET A/D TABLE TO BACK TO NORMAL ********** //

    ///set the control back like it was when we entered
    //if( ((m_Flag==(u8)EXT_PREPARE)||(m_Flag==(u8)EXT_PHASE1)) && (procresult != PROCRESULT_OK) )
    //AK: All steps at once now
    if(procresult != PROCRESULT_OK)
    {
        //leave I/P where it is with control off
        //note - with control off, its ok to reenable the limits since
        //        control won't enforce them (control is off)
    }
    else
    {
        //done - reset control mode
        //mode_SetControlMode(ControlMode, Setpoint); //?? Do we still do this?   done in wrapper now
    }

    process_SetProcessProgress(COMPLETE);

    //display "SETUP"
    ui_setNext(UINODEID_SETUPNODE);

    return procresult;
}

/** \brief A data sampling function for diagnostic buffer in pruned samling mode.
Samples valve position and main pressure.
\param data - a pointer to a pair of data
REQ 21-43, 21-62
*/
static void dsampler_SamplePosPres(diag_t data[2])
{
    data[0] = vpos_GetScaledPosition();
    data[1] = pres_GetMainPressure();
}

/** \brief runs an extended signature test

  runs an extended signature test. stores the results in the diagnostic buffer
  \param[in] s16 *procdetails
  \return a competion code
*/
static procresult_t diag_Perform_ExtActuatorSignature(s16 *procdetails)
{
    UNUSED_OK(procdetails);
    procresult_t procresult;

    ClearExtDiagHeader();  //clear out heading before we start putting things in

    if(m_DiagControlOption == DIAGOPT_OPENLOOP)
    {
        //new open loot signature routine
        procresult = diag_Run_ExtActuatorSignatureOpen_Internal(dsampler_SamplePosPres, TASKID_CYCLE,
                     m_nStartPosition, m_nEndPosition, (u16)m_SetpointRampSpeed, m_DiagDirection);
    }
    else if(m_DiagControlOption == DIAGOPT_CLOSEDLOOP)
    {
        //new closed loop signature
        procresult = diag_Perform_ExtActuatorSignatureClosed();
    }
    else
    {
        procresult = PROCRESULT_FAILED;
    }

    return procresult;
}

/** \brief do a closed loop signature

  runs a closed loop extended signature test. stores the results in the diagnostic buffer
  \param[in] none
  \return a competion code
*/
static procresult_t diag_Perform_ExtActuatorSignatureClosed(void)
{
    procresult_t procresult = diag_Run_RampTest_Internal(dsampler_SamplePosPres, TASKID_CYCLE);
    //dsampler_SamplePosPres

    if(procresult == PROCRESULT_OK)
    {
        FillExtDiagHeader();
    }

    return procresult;
}

/** \brief put extended signature header data into buffer

  put extended signature header data into buffer
  \param[in] none
  \return none
*/
void FillExtDiagHeader(void)
{
        //Populate the buffer header
        const diag_t ExtDiag_Header[] =
        {
            [0] = DIAG_EXT_ACT, //test type
            [1] = DIAG_EXTDIAG_VERSION, //version
            [2] = EXTDIAG_HEADERSZ, //header size in diag_t entries
            [3] = (diag_t)m_NumberOfPoints, // #of samples
            [4] = (diag_t)m_nStartPosition,
            [5] = (diag_t)m_nEndPosition,
            [6] = (diag_t)m_SetpointRampSpeed, // setpoint rate in %/sec (STANDARD_100 means 100%/s)
            [7] = (diag_t)MIN(INT16_MAX, m_FinalPruneScale * CYCLE_TASK_DIVIDER * CTRL_TASK_DIVIDER), //Sampling interval in 5 ms ticks
            [8] = (diag_t)m_DiagDirection,
            [9] = (diag_t)m_DiagControlOption,
            [10] = (diag_t)m_SamplesFirstDirection,
        };
        CONST_ASSERT(NELEM(ExtDiag_Header) <= EXTDIAG_HEADERSZ);

        size_t i;
        diag_t *p = buffer_GetXDiagnosticBuffer(DIAGBUF_DEFAULT);
        for(i=0; i < NELEM(ExtDiag_Header); i++)
        {
            p[i] = ExtDiag_Header[i];
        }
        for(; i < EXTDIAG_HEADERSZ; i++)
        {
            p[i] = EXTDIAG_HEADER_FILLER; //just fill the space with a pleasing filler
        }
}

/** \brief remove left over heading from previous tests

  remove left over heading from previous tests
  \param[in] none
  \return none
*/
static void ClearExtDiagHeader(void)
{
    size_t i;
    diag_t *p = buffer_GetXDiagnosticBuffer(DIAGBUF_DEFAULT);

    for(i=0; i < EXTDIAG_HEADERSZ; i++)
    {
        p[i] = EXTDIAG_HEADER_FILLER; //just fill the space with a pleasing filler
    }
}


/** \brief puts step test header data into buffer
  puts step test header data into buffer
  \param[in] none
  \return size_t - the header size
*/
static size_t fill_func_steptest(void)
{
    const SamplerInfo_t *SamplerInfo = buffer_GetSamplerInfo(DIAGBUF_DEFAULT);

    //Populate the buffer header
    const diag_t StepTest_Header[] =
    {
        [0] = DIAG_STEP, //test type
        [1] = DIAG_STEP_VERSION, //version
        [2] = STEPTEST_HEADERSZ, //header size in diag_t entries
        [3] = (diag_t)SamplerInfo->num_points, // #of samples
        [4] = (diag_t)m_nStartPosition,
        [5] = (diag_t)m_nEndPosition,
        [6] = (diag_t)m_SamplingTime,
        [7] = (diag_t)MIN(INT16_MAX, SamplerInfo->prune_scale * CTRL_TASK_DIVIDER), //Sampling interval in 5 ms ticks
    };
    CONST_ASSERT(NELEM(StepTest_Header) <= STEPTEST_HEADERSZ);

    size_t i;
    diag_t *p = buffer_GetXDiagnosticBuffer(DIAGBUF_DEFAULT);
    for(i=0; i < NELEM(StepTest_Header); i++)
    {
        p[i] = StepTest_Header[i];
    }
    for(; i < STEPTEST_HEADERSZ; i++)
    {
        p[i] = STEPTEST_HEADER_FILLER; //just fill the space with a pleasing filler
    }

    return STEPTEST_HEADERSZ;
}


/** \brief starts a step test

  starts a step test. stores the results in the diagnostic buffer
  \param[in] none
  \return none
*/
void diag_Run_StepTest(void)
{
    ctlmode_t ControlMode;
    s32 Setpoint;

    ///make sure the parameters are set
    if( (!m_bParametersAreValid) || (m_Type != (u8)DIAG_STEP) )
    {
        ErrorHandler(FAULT_EXT_DIAGNOSTICS_FAILED);
    }

    ///make sure that if limits are set, we are allowed to turn them off
    else if(cnfg_DisableLimits() == ERR_CNFG_PROTECTED_LIMITS)
    {
        ErrorHandler(FAULT_EXT_DIAGNOSTICS_FAILED);
    }

    else  //perform the diagnostic
    {
        //display "DIAG"
        ui_setNext(UINODEID_DIAG_EX);


        ///save the control mode and setpoint
        control_GetControlMode(&ControlMode, &Setpoint);

        procresult_t procresult = diag_Perform_StepTest_Internal(fill_func_steptest,
                                   m_nStartPosition, m_nEndPosition,
                                   m_SamplingTime);

        ///perform a step test
        if(procresult == PROCRESULT_OK)
        {
            //clear the fault bit
            error_ClearFault(FAULT_EXT_DIAGNOSTICS_FAILED);
        }
        else
        {
            //error
            error_SetFault(FAULT_EXT_DIAGNOSTICS_FAILED);

            ///set the control back like it was when we entered
            mode_SetControlMode(ControlMode, Setpoint);
        }

        //leave the valve positioned at the end position
        //to allow successive step tests

        //re-enable the limits if we turned them off
        cnfg_ReEnableLimits();
        process_SetProcessProgress(COMPLETE);

        //display "SETUP"
        ui_setNext(UINODEID_SETUPNODE);
    }
}


/** \brief check that the IP is within limits

  heck that the IP is within limits
  \param[in] s32 IP - the sampled IP
  \return bool_t - true if within limits
*/
bool_t CheckIPLimits(s32 IP)
{
    bool_t ret;
    if((IP < (s32)OUT_LOW_LIMIT) || (IP > (s32)OUT_HIGH_LIMIT))
    {
        ret = false;
    }
    else
    {
        ret = true;
    }
    return ret;
}

//----------- ramp test (standalone) -------------------

/* REQ 25-13 cause test to wait if error excessive */
//#define MAX_FINAL_WAIT_MIN (5u)
//#define MAX_FINAL_WAIT_SEC (MAX_FINAL_WAIT_MIN * 60u)
//#define MAX_FINAL_WAIT_MS (MAX_FINAL_WAIT_SEC*1000u)
//#define MAX_FINAL_WAIT_TIME  (MAX_FINAL_WAIT_MS/(TICK_MS*WAIT_TICKS))

/* REQ 25-14 keep setpoint between -5% and 105% */
//#define RANGE_OUTSIDE INT_PERCENT_OF_RANGE(10)

#define RAMPTEST_HEADER_FILLER ((diag_t)0x8081)
#define DIAG_RAMP_VERSION 0
#define RAMPTEST_HEADERSZ 24 //in diag_t entries; a dogmatic number to please FF params



/** \brief puts SP and POS in diagnostic buffer

  puts SP and POS in diagnostic buffer
  \param[in] diag_t *dst - the destination buffer
  \return none
*/
static void sample_pos_sp(diag_t *dst)
{
    s32 sp, pos;
    control_GetSetpointPosition(&sp, &pos);
    dst[0] = (diag_t)sp;
    dst[1] = (diag_t)pos;
}

/** \brief start a closed loop ramp test

  start a closed loop ramp test
  \param[in] s16* procdetails - return from actual ramp test
  \return procresult_t - return from actual ramp test
*/
static procresult_t diag_Run_RampTest_InternalSP(s16* procdetails)
{
    procresult_t pr = diag_Run_RampTest_Internal(sample_pos_sp, TASKID_CONTROL);
    *procdetails = (s16)pr;
    return pr;
}


/** \brief perform a closed loop ramp test

  perform a closed loop ramp test
  \param[in] void (*sample_func)(diag_t data[2]) - pointer to the function to get ramp data
  \param[in] taskid_t TaskContext - the context from which to sample the data
  \return procresult_t - PROCRESULT_FAILED or PROCRESULT_OK
*/
static procresult_t diag_Run_RampTest_Internal(void (*sample_func)(diag_t data[2]), taskid_t TaskContext)
{
    procresult_t procresult;
    ErrorCode_t err;

    ///make sure the parameters are set
    if( (!m_bParametersAreValid) || ((m_Type != (u8)DIAG_RAMP) && (m_Type != (u8)DIAG_EXT_ACT)) )
    {
        return PROCRESULT_FAILED;
    }

    //perform the diagnostic

    //display "DIAG"
    ui_setNext(UINODEID_DIAG_EX);

    //no need to save setpoint - this test is always done in manual control mode
    //  and valve is left where it was

    //move the valve to starting position and stabalize
    procresult = diag_Prepare_VRampTest(false, (pos_t)m_nStartPosition, (pos_t)m_nEndPosition); //use false until we implement both directions

    ///perform an extended signature test one way
    if(procresult == PROCRESULT_OK)
    {
        /* Now, we can start sampling. Without a critical section at the start, we may pick up a couple
            of sample points due to task preemption. Who cares?
        */

        //start the sampling process
        m_SamplesFirstDirection = 0u;
        m_FirstDirectionPruneScale = 0;
        buffer_SuspendSampling(DIAGBUF_DEFAULT);   //just in case (and to set skip=0)
        err = buffer_StartSampling(DIAGBUF_DEFAULT,
                                     TaskContext,
                                     sample_func,
                                     MAX_NUM_DSAMPLES(RAMPTEST_HEADERSZ),
                                     RAMPTEST_HEADERSZ/2U, // diag_t --> dsample_t
                                     NULL);

        if(err == ERR_OK)
        {

          //start the actual ramp test
          procresult = diag_Perform_VRampTest(             m_SetpointRampSpeed,
                                                           (pos_t)m_nStartPosition,
                                                           (pos_t)m_nEndPosition
                                                           );

          // when both directions are selected, call again with start and end reversed
          if(procresult == PROCRESULT_OK)
          {
              SetSamplesFirstDirection();

              if(m_DiagDirection == DIAGDIR_UPDOWN)
              {
                  //dont' sample while we wait for valve to get to position and stable
                  buffer_SuspendSampling(DIAGBUF_DEFAULT);

                  //move the valve to starting position and stabalize
                  procresult = diag_Prepare_VRampTest(false, (pos_t)m_nEndPosition, (pos_t)m_nStartPosition); //use false until we implement both directions

                  if(procresult == PROCRESULT_OK)
                  {
                      //now start sampling
                      buffer_ResumeSampling(DIAGBUF_DEFAULT);

                      procresult = diag_Perform_VRampTest(     m_SetpointRampSpeed,
                                                               (pos_t)m_nEndPosition,
                                                               (pos_t)m_nStartPosition
                                                               );
                  }
              }

          }

          //stop the sampling
          buffer_StopSampling(DIAGBUF_DEFAULT);
          SetSamplesLastDirection();  //call this even for one-way tests

        }
        else
        {
            procresult = PROCRESULT_FAILED;
        }

    }

    //done - leave valve where it was - this test is always done in manual control mode

    process_SetProcessProgress(COMPLETE);

    //display "SETUP"
    ui_setNext(UINODEID_SETUPNODE);

    return procresult;
}

/** \brief saves the number of samples if the first direction when both directions are to be run

  saves the number of samples if the first direction when both directions are to be run
  \param[in] none - data is gathered from the buffer routine
  \return none
*/
void SetSamplesFirstDirection(void)
{
    const SamplerInfo_t *SamplerInfo = buffer_GetSamplerInfo(DIAGBUF_DEFAULT);
    m_SamplesFirstDirection = SamplerInfo->num_points;
    m_FirstDirectionPruneScale = SamplerInfo->prune_scale;
}

/** \brief saves the number of samples if the second direction

  saves the number of samples if the second direction and
    corrects 1st direction for change in prune scale
  \param[in] none - data is gathered from the buffer routine
  \return none
*/
void SetSamplesLastDirection(void)
{
    if((m_SamplesFirstDirection != 0) && (m_FirstDirectionPruneScale != 0))
    {
        const SamplerInfo_t *SamplerInfo = buffer_GetSamplerInfo(DIAGBUF_DEFAULT);
        m_NumberOfPoints = SamplerInfo->num_points;
        m_FinalPruneScale = SamplerInfo->prune_scale;

        if(m_FinalPruneScale != m_FirstDirectionPruneScale)
        {
          u16_least Difference = m_FinalPruneScale / m_FirstDirectionPruneScale;
          m_SamplesFirstDirection = (u16)((m_SamplesFirstDirection + (Difference-1))/Difference);
        }
    }
}

/** \brief start the test and save the header data

  start the test and save the header data
  \param[in] s16 *procdetails - the return from the actual ramp test routine
  \return procresult_t - PROCRESULT_FAILED or PROCRESULT_OK
*/
procresult_t diag_Run_RampTest(s16 *procdetails)
{
    procresult_t procresult = control_ProcWithNoLimits(diag_Run_RampTest_InternalSP, procdetails);

    //Populate the buffer header
    const diag_t RampTest_Header[] =
    {
        [0] = DIAG_RAMP, //test type
        [1] = DIAG_RAMP_VERSION, //version
        [2] = RAMPTEST_HEADERSZ, //header size in diag_t entries
        [3] = (diag_t)m_NumberOfPoints, // #of samples
        [4] = (diag_t)m_nStartPosition,
        [5] = (diag_t)m_nEndPosition,
        [6] = (diag_t)m_SetpointRampSpeed, // setpoint rate in %/sec (STANDARD_100 means 100%/s)
        [7] = (diag_t)MIN(INT16_MAX, m_FinalPruneScale * CTRL_TASK_DIVIDER), //Sampling interval in 5 ms ticks
        [8] = (diag_t)m_DiagDirection,
        [9] = (diag_t)m_SamplesFirstDirection
    };
    CONST_ASSERT(NELEM(RampTest_Header) <= RAMPTEST_HEADERSZ);

    size_t i;
    diag_t *p = buffer_GetXDiagnosticBuffer(DIAGBUF_DEFAULT);
    for(i=0; i < NELEM(RampTest_Header); i++)
    {
        p[i] = RampTest_Header[i];
    }
    for(; i < RAMPTEST_HEADERSZ; i++)
    {
        p[i] = RAMPTEST_HEADER_FILLER; //just fill the space with a pleasing filler
    }
    return procresult;
}

/* This line marks the end of the source */

