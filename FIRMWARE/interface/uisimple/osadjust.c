/**
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/

/**
    \file osadjust.c
    \brief Contains functions used by several other modules. Focus: OS adjustment.

    CPU: Any

    OWNER: Sergey

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/uisimple/osadjust.c $
    $Date: 1/30/12 5:43p $
    $Revision: 13 $
    $Author: Arkkhasin $

    \ingroup Process
*/
/* $History: osadjust.c $
 * 
 * *****************  Version 13  *****************
 * User: Arkkhasin    Date: 1/30/12    Time: 5:43p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/uisimple
 * Restore LCX build FBO TFS:8782
 *
 * *****************  Version 12  *****************
 * User: Arkkhasin    Date: 1/05/12    Time: 3:03p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/uisimple
 * TFS:8577 - header refactoring
 *
 * *****************  Version 10  *****************
 * User: Arkkhasin    Date: 11/15/11   Time: 1:29p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/services
 * Preliminary check-in for
 * TFS:8202 decouple I/O subsystem
 *
 * *****************  Version 8  *****************
 * User: Sergey Kruss Date: 4/12/11    Time: 1:55p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * TFS:6018 - Overflow of manual setpoint with low OS adjusment and high
 * OS adjust Overrun.
 *
 * *****************  Version 7  *****************
 * User: Sergey Kruss Date: 1/14/11    Time: 10:48a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * TFS:5323 - Added entries for "OS Adj overrun" functionality.
 *
 * *****************  Version 6  *****************
 * User: Sergey Kruss Date: 1/05/11    Time: 2:45p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * TFS:5241 - osadj_RunOpenStopAdjustment() should return type void.
 * TFS:4293 - Initial SP should be set to current position instead of
 * Normal Mode SP.
 * TFS:5001(after redo) - Cancel button must cancel OS Adj process even if
 * the OS Adj button is pressed (simulated break)
 * TFS:5240 -- Remove superfluous call to process_CheckProcess()
 * TFS:5253 -- Remove redundant call to mode_SetControlMode() at the end
 * of process.
 *
 * *****************  Version 5  *****************
 * User: Sergey Kruss Date: 12/14/10   Time: 1:59p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * TFS:5021 - Removed call to function UI_GetLaunchButton() and replaced
 * it with the constant UI_BUTTON_OPENSTOPADJUST
 * TFS:5045 - Removed extra and obsolete comments.
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 12/09/10   Time: 7:11p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * TFS:5012 - Added Lint semantics for f2i_Scaled; removed suprefluous
 * protection code
 *
 * *****************  Version 3  *****************
 * User: Sergey Kruss Date: 12/09/10   Time: 12:10p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * TFS:5001 - Make sure OSadjustment is cancelled by cancel button, and
 * even if cancel button is held down (simulating break) it is cancelled.
 *
 * *****************  Version 2  *****************
 * User: Sergey Kruss Date: 12/08/10   Time: 11:57a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * TFS:4991 - Removed function: control_ResetBiasChangeFlag() from the
 * function "osadj_RunOpenStopAdjustment()", line 269
 *
 * *****************  Version 1  *****************
 * User: Sergey Kruss Date: 10/26/10   Time: 2:23p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/services
 * TFS:4294  - OSadj moved from calib.c to osadjust.c
 * TFS:4288 - OS Adj is called as a "normal" process via proc table.
 * TFS:4287 - OS Adj must fail if cannot disable limits.

*/

#include "mnwrap.h" //for intrinsic functions used by the OS port
#include "mncbtypes.h"
#include "errcodes.h"
#include "process.h"
#include "param.h"
#include "configure.h"
#include "uipublic.h"
#include "uistartnodes.h"
#include "devicemode.h"
#include "quad.h"
#include "osadjust.h"
#include "position.h"
#include "uiprivate.h"
#include "nvram.h"

static OSAdjConf_t m_OSAConf;
static const OSAdjConf_t OSAConf_default =
{
    .OSAdjOverrun = STANDARD_ZERO,
    .CheckWord = 0, //don't care
};


const OSAdjConf_t *osadj_GetConf(OSAdjConf_t *dst)
{
    return STRUCT_TESTGET(&m_OSAConf, dst);
}

ErrorCode_t osadj_SetConf(const OSAdjConf_t *src)
{
    if(src == NULL)
    {
        src = &OSAConf_default;
    }
    if( (src->OSAdjOverrun < OS_ADJ_MIN_INT_OVERRUN) || (src->OSAdjOverrun > OS_ADJ_MAX_INT_OVERRUN) )
    {
        return ERR_INVALID_PARAMETER;
    }
    Struct_Copy(OSAdjConf_t, &m_OSAConf, src);
    return ram2nvramAtomic(NVRAMID_OSAdjConf);
}



#if 0 //replaced by the above
/** \brief set the open stop adjustment overrun value
  \param[in] float32 fOSAdjOvrrun - the open stop adjustment overrun value
  \return ErrorCode_t - ERR_OK if no error,
  \ERR_INVALID_PARAMETER if adjustment out of range -10% to 80%
*/
ErrorCode_t cnfg_SetOSAdjOverrun(const float32 fOSAdjOvrrun)  /** TFS:5323 */
{
    ErrorCode_t ret;
    const s32 i4OSadjOverrun =
              f2i_Scaled( fOSAdjOvrrun, OS_ADJ_MIN_INT_OVERRUN-1,
                          OS_ADJ_MAX_INT_OVERRUN+1, STD_SCALE );

    if ( i4OSadjOverrun < OS_ADJ_MIN_INT_OVERRUN || i4OSadjOverrun > OS_ADJ_MAX_INT_OVERRUN)
    {
        ret = ERR_INVALID_PARAMETER;
    }
    else
    {
        PositionStop_t positionStop = m_PositionStop;
        positionStop.fOSAdjOverrun = fOSAdjOvrrun;
        Struct_Test(PositionStop_t, &m_PositionStop);
        Struct_Copy(PositionStop_t, &m_PositionStop, &positionStop);

#ifdef OLD_NVRAM
        MN_FRAM_ACQUIRE();
            ram2nvram((void*)&positionStop, NVRAMID_PositionStop);
        MN_FRAM_RELEASE();
        ret = ERR_OK;
#else
        ret = ram2nvramAtomic(NVRAMID_PositionStop);
#endif //OLD_NVRAM
    }

    return ret;
} //-----  end of cnfg_SetOSAdjOverrun() ------
#endif



/** TFS:4293 -- OS Adj SP is initiated to current position (instead of Normal Mode SP) */

/** TFS:5001 */
/** \brief This function waits for the "OS Adj" PB or for the "Cancel" PB to be released.
     It returns the value of the PB pressed.
    This function also periodically call WAIT FOR TIME!!
  \param[in] none
  \return none
*/
static LogicalButtons_t WaitButtonsReleaseOrCancel(void)
{
    LogicalButtons_t    LogicalButton;

    do
    { //wait until let go the button
        LogicalButton = UI_Button();
        (void)process_WaitForTime(T0_050);
    } while ( (LogicalButton != BUTTON_LOG_NONE) && (LogicalButton != BUTTON_LOG_CANCEL) ); /** TSF:5001 */

    return LogicalButton;   /** TFS:5001 after "redo" */
} //-----  end of WaitButtonsReleaseOrCancel() ------

//Standard maximum travel of the SP by the rotary encoder
#define OSADJ_TRAVEL_STANDARD      ((s32)INT_PERCENT_OF_RANGE(110.0F)) /** TFS:5323 */
#define OSADJ_ROTARY_ACCEL          4   /** TFS:5342 */

/** \brief Performs setting Adjusted Open Stop and calculation and storing
           the floating point Open Stop Adjustment value, residing in the global
           object "PositionStop_t m_PositionStop".
           This function is called by the Process task when the button "OS adj" is pressed for no less than 2 seconds.
           The goal is to move the position of the actuator to a place, desired to become the next Adjusted Open Stop
           and to calculate the ratio of the distance from this position to the lower hard stop over the full hard stops range.
           This ratio is saved in NVRam (as a floating point number) and is used by the program in linear transformation
           of the raw position to the standard (0 to 16384) position.
  \param[in] none
  \return none (TFS:5241)
*/
void osadj_RunOpenStopAdjustment(void)  /** TFS:5241 */
{
    LogicalButtons_t    LogicalButton;
    LogicalButtons_t    reasonStopWait;
    s16                 SPdelta;
    bool_t              ProcessDone;
    HardPos_t  Position; //real AtoD position in counts
    s32                 HardStopsRange;
    s32                 n4OpenStopAdjustment; //open stops adj in integer domain
    const s32           n4spLowerLimit = (s32)STANDARD_ZERO; //min limit for the setpoint (0%)
    s32                 n4spUpperLimit;  //max limit for the setpoint
#if 0
    PositionStop_t      positionStops;
#else
    PositionConf_t posconf;
#endif
    bool_t              bAto;
    s16_least           CallBackID;         // "Message" from the process to LUI
    /** REQ 31-037, 31-038 - External Interruption */
    bool_t              Success;
    s32                 n4MomentarySP;
    s32                 i4OSadjOverrun; //possible overrun of standard travel (-10% to 90%) /** TFS:5323 */


    //get and save position ASAP (before getting interrupted by some other process)..
    //..and stay there till the end of the process
    n4MomentarySP = control_GetPosition();  /** TFS:4293 */
    mode_SetControlMode(CONTROL_MANUAL_POS, n4MomentarySP);

    //TFS:4287 -- return immediately if cannot disable limits
    if ( cnfg_DisableLimits(false) == ERR_CNFG_PROTECTED_LIMITS )
    {
        //early return with a standard blinking error
        CallBackID = UINODEID_OPENSTOPADJ_FAILED;
    }
    else
    {
        //TFS:4294;  TFS:4288
        quad_EnableInt();

#if 0
        //GET ALL VALUES, CONSTANT WITHIN THE PROCESS
        bAto = cnfg_GetComputedConfigurationData()->bATO;
        //get info about hard stops structure (m_PositionStop)
        positionStops = *cnfg_GetPositionStop();
        n2HardStopsRange = positionStops.HighPositionStop - positionStops.LowPositionStop;
        //max value, which setpoint can have..
        //..it is equal to setpoint necessary for position to get to 100%
        //<FP-debug>n4spUpperLimit = ( ( (s32)STANDARD_100 * (s32)100 ) / (s32)positionStops.fOpenStopAdjustment );
        n4OpenStopAdjustment = f2i_Scaled(positionStops.fOpenStopAdjustment,
                                          OPEN_STOP_ADJ_MIN_INT_EXT, OPEN_STOP_ADJ_MAX_INT_EXT, STD_SCALE); //between 50% and 200%
        /** TFS:5323 */
#else
        (void)pos_GetPositionConf(&posconf);
        bAto = posconf.bATO;
        HardStopsRange = posconf.rangeval[Xhi] - posconf.rangeval[Xlow];
        n4OpenStopAdjustment = posconf.PositionRange[Xhi];
        /** TFS:5323 */
#endif
#if 0
        i4OSadjOverrun = f2i_Scaled(positionStops.fOSAdjOverrun,
                                          OS_ADJ_MIN_INT_OVERRUN-1, OS_ADJ_MAX_INT_OVERRUN+1, STD_SCALE);
#else
        i4OSadjOverrun = osadj_GetConf(NULL)->OSAdjOverrun;
#endif
        n4spUpperLimit = ( (OSADJ_TRAVEL_STANDARD + i4OSadjOverrun) * (s32)STANDARD_100 ) / n4OpenStopAdjustment;
        /** TFS:6018 */  //make sure we don't fall out the 16-bit container
        if ( n4spUpperLimit > MAX_ALLOWED_POSITION )
        {
            n4spUpperLimit = MAX_ALLOWED_POSITION;
        }

        // PREPARE FOR PROCESSING
        (void)quad_ReadAndClearValue();  // Read and clear the value of the encoder

        //START PROCESSING (OS Adj button is pressed and let go)
        ProcessDone = false;
        while (!ProcessDone)
        {
            /** REQ 29-??? */ /** REQ 31-034 */ //blink "good" if all is good
            CallBackID = UINODEID_OPENSTOPADJ_NORMAL;

            /** REQ 31-020 Interface with QE */
            SPdelta = quad_ReadAndClearValue();
            //one (unaccelerated) click of the encoder is only 4 counts..
            //..increase it to 16 to be ~0.01% of the range
            SPdelta *= OSADJ_ROTARY_ACCEL;  /** TFS:5342 */

            //increment SP with the encoder value
            n4MomentarySP += SPdelta;

            //prevent SP wind-up: make sure position doesn't go too far in any direction
            if ( (n4MomentarySP > n4spUpperLimit) && (SPdelta > 0) )
            {
                n4MomentarySP = n4spUpperLimit;
                SPdelta = 0;
            }
            if ( (n4MomentarySP < n4spLowerLimit) && (SPdelta < 0) )
            {
                n4MomentarySP = n4spLowerLimit;
                SPdelta = 0;
            }

            //enter the current SP to the system
            mode_SetControlMode(CONTROL_MANUAL_POS, n4MomentarySP);

            //get current raw position
            Position = vpos_GetRawPosition();
            //make sure we don't go beyond Hard Stops -- it will cause error
#if 0
            if ( positionStops.HighPositionStop > positionStops.LowPositionStop )
            {
                n2Position = CLAMP(n2Position, positionStops.LowPositionStop,
                                               positionStops.HighPositionStop);
            }
            else
            {
                Position = CLAMP(n2Position, positionStops.HighPositionStop,
                                               positionStops.LowPositionStop);
            }
#else
            if ( posconf.rangeval[Xhi] > posconf.rangeval[Xlow] )
            {
                Position = CLAMP(Position, posconf.rangeval[Xlow], posconf.rangeval[Xhi]) ;
            }
            else
            {
                Position = CLAMP(Position, posconf.rangeval[Xhi], posconf.rangeval[Xlow]);
            }
#endif

            //calculate integer OS Adjustment
#if 0
            n4OpenStopAdjustment = (((s32)STANDARD_100 * (s32)(n2Position - positionStops.LowPositionStop)) /
                                    (s32)n2HardStopsRange) + (s32)STANDARD_ZERO;
#else
            n4OpenStopAdjustment = (((s32)STANDARD_100 * (Position - posconf.rangeval[Xlow])) /
                                    HardStopsRange) + (s32)STANDARD_ZERO;
            /* Or, better yet
            n4OpenStopAdjustment = pos_MapPositionToStd(Position);
            (except that the function is static now; may need to change that)
            */
#endif

            //prevent accidental exceeding 100% due to rotary overrun: would cause warning+error
            if ( n4OpenStopAdjustment > OPEN_STOP_ADJ_MAX_INT )     /** TFS:5323 */
            {
                n4OpenStopAdjustment = OPEN_STOP_ADJ_MAX_INT;
            }

            //reverse direction for ATC configuration
            if (!bAto)
            {
                n4OpenStopAdjustment = (s32)STANDARD_100 - n4OpenStopAdjustment;
            }

            /** REQ 31-034 */  // Check the Range and send WARNING if needed
            if ((n4OpenStopAdjustment < OPEN_STOP_ADJ_MIN_INT) || (n4OpenStopAdjustment > OPEN_STOP_ADJ_MAX_INT))
            {   // Outside of range
                CallBackID = UINODEID_OPENSTOPADJ_WARNING;
                Success = false;
            }
            else
            {
                Success = true;
            }

            /** REQ 31-034 Call back mechanism to LUI */
            ui_setNext(CallBackID);         // Send the message to UI
            // --------------------------------------------------------------------------------------

            /** REQ 31-021 ACCEPT */
            // Read buttons
            LogicalButton = UI_Button();
            if ( LogicalButton == UI_BUTTON_OPENSTOPADJUST )  /** TFS:5021 */
            {   // This same button - now do Stop Adjust

                reasonStopWait = WaitButtonsReleaseOrCancel();
                if ( BUTTON_LOG_NONE == reasonStopWait )
                {
#if 0
                    // Note that n4OpenStopAdjustment already calculated, it is re-calculated every loop run
                    positionStops.fOpenStopAdjustment = std2f(n4OpenStopAdjustment * STANDARD_FP_PRESCALE);

                    //prevent Adjusted Open Stop from getting too close to Closed stop
                    if ( bAto && (ABS(n2Position - positionStops.LowPositionStop) < STOPS_MIN_RANGE) )
                    {
                        /** REQ 31-037, 31-038 - External Interruption */
                        Success = false;
                    }

                    if ( !bAto && (ABS(positionStops.HighPositionStop - n2Position) < STOPS_MIN_RANGE) )
                    {
                        /** REQ 31-037, 31-038 - External Interruption */
                        Success = false;
                    }

                    //all is OK -- run the function that sets OS Adj value to m_PositionStop..
                    //..and recalculates m_ComputedPositionStop
                    if ( Success )
                    {
                        (void)cnfg_SetOpenStopAdjustment(positionStops.fOpenStopAdjustment);
                    }
#else
                    // Note that n4OpenStopAdjustment already calculated, it is re-calculated every loop run
                    posconf.PositionRange[Xhi] = (pos_t)n4OpenStopAdjustment;

                    //prevent Adjusted Open Stop from getting too close to Closed stop
                    if ( bAto && (ABS(Position - posconf.rangeval[Xlow]) < STOPS_MIN_RANGE) )
                    {
                        /** REQ 31-037, 31-038 - External Interruption */
                        Success = false;
                    }

                    if ( !bAto && (ABS(posconf.rangeval[Xlow] - Position) < STOPS_MIN_RANGE) )
                    {
                        /** REQ 31-037, 31-038 - External Interruption */
                        Success = false;
                    }

                    //all is OK -- run the function that sets OS Adj value to m_PositionStop..
                    //..and recalculates m_ComputedPositionStop
                    if ( Success )
                    {
                        (void)pos_SetStopAdjustment(posconf.PositionRange[Xlow], n4OpenStopAdjustment);
                    }
#endif
                }//end if BUTTON_LOG _NONE or _CANCEL       /** TFS:5001 after "redo" */

                ProcessDone = true;
            }

            // Need refresh WD, etc. so we must call process_WaitForTime()
            // The Cancel function should come back via the process_WaitForTime() return.
            if (process_WaitForTime(T0_050))
            {   // CANCEL button has been pressed or anything happened...
                ProcessDone = true;
            }
        } // while

        // report to the UI whether the process succeeded
        // Do good or bad blinking...
        /** REQ 31-037, 31-038 - External Interruption */
        CallBackID = (Success) ? UINODEID_OPENSTOPADJ_OK : UINODEID_OPENSTOPADJ_FAILED;

        quad_DisableInt();
        cnfg_ReEnableLimits();
    }//end else ERR_CNFG_PROTECTED_LIMITS

    ui_setNext(CallBackID);

} //-----  end of osadj_RunOpenStopAdjustment() ------


/* end of the source */

