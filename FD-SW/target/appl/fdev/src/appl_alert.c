/*
Copyright 2012 by GE Energy / Dresser, Inc., as an unpublished trade secret. All rights reserved.

This document and all information herein are the property of GE Energy.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon demand.
*/

/**
    \file appl_alert.c
    \brief Position and other types of Alerts (Level monitors)

    CPU: Any

    OWNER: AP
*/

#include <softing_base.h>
#include <ptb.h>
#include <hw_if.h>
#include "base.h"
#include "time_defs.h"
#include "numutils.h"

#include "appl_int.h"

// #include "mnassert.h"

#include "mnhart2ff.h"
// #include "appl_alert.h"
#include "mnfault2alert.h"
// #include "eep_if.h"
#include "osif.h"

#include "mn_travel.h"

#include "eep_blk2.h"

//------------------------------------------------------------------

  #define  MODULE_ID      (COMP_FAPP + MOD_APPL_TRN)

//------------------------------------------------------------------
// The Alarm / Alert handling structure designed to provide unified Alarm
// Handling API

typedef enum AlertType_t
{
    ALERT_TYPE_DEVIATION,
    ALERT_TYPE_POSITION_HIHI,
    ALERT_TYPE_POSITION_HI,
    ALERT_TYPE_POSITION_LO,
    ALERT_TYPE_POSITION_LOLO,
    ALERT_TYPE_SP_TIMEOUT,

    ALERT_TYPE_NEAR_CLOSE,

    ALERT_TYPE_TRAVELACC_A,
    ALERT_TYPE_TRAVELACC_B,
    ALERT_TYPE_CYCLECNT_A,
    ALERT_TYPE_CYCLECNT_B,

    ALERT_TYPE_WORKTIME,

    ALERT_TYPE_SUP_PRES_HI,
    ALERT_TYPE_SUP_PRES_LO,
    ALERT_TYPE_SUP_PRES_LOLO,
    ALERT_TYPE_TEMP_HI,
    ALERT_TYPE_TEMP_LO,
    ALERT_TYPE_IPCUR_HI,
    ALERT_TYPE_IPCUR_LO,

    ALERT_TYPE_SENSOR_FAILED,
    ALERT_TYPE_PROCESSOR,
    ALERT_TYPE_VALVE_CONTROL,
    ALERT_TYPE_MISSION,
    ALERT_TYPE_AIR_SUPPLY,
    ALERT_TYPE_HARDWARE,

    ALERT_TYPE_MAX
} AlertType_t;

#define ALERT_FIRST_ALERT   ALERT_TYPE_DEVIATION

//-----------------------------------------------------------------------------------------------
// MACRO's for comparison

#define ABOVE_THHOLD(val,rng)               ((val) > (rng))
#define BELOW_THHOLD(val,rng)               ((val) < (rng))

#define ABOVE_DBAND(val,rng,db)             ((val) > ((rng) + (db)))
#define BELOW_DBAND(val,rng,db)             ((val) < ((rng) - (db)))

//-----------------------------------------------------------------------------------------------
// Type definition for Time handling

typedef struct Alert_TimeHandling_t
{
    USIGN32 TimeStamp;  // Used for latching teh time of the condition POSITIVE
    bool_t  Latch;

} Alert_TimeHandling_t;

static u32 Alert_SPTimeout_TimeStamp;

//-----------------------------------------------------------------------------------------------
// Copy of PTB instance. ALerts do not need it and use PTB instead, calculated once, except
// few cases when instance mightbe needed.

static const T_FBIF_BLOCK_INSTANCE * p_b_instance;

//-----------------------------------------------------------------------------------------------

// This function checks the Variable Status.
// Returns:
//  FALSE - BAD status, condition for FState activation;
//  TRUE  - No, GOOD status, no reasons to activate FState
static bool_t   ff_AlertSPTimeout_StatusOK(u8 VStatus)
{
    bool_t  result = true;
    u8      VQuality;

    VQuality    = VStatus & QUALITY_MASK;            // Extract the Quality
    switch (VQuality)
    {
        case SQ_BAD :
            // Bad value status -- activate FSTATE!
            result = false;
            break;
        default :
            // Nothing
            break;
    }

    return result;
}

/* \brief This function updates Alert_SPTimeout_TimeStamp
    param in:
        p_block_instance: pointer to function block
        Channel: channel
*/
void ff_AlertSPTimeout_Execute_fromFB(const T_FBIF_BLOCK_INSTANCE *p_block_instance, u16 Channel)
{
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;
    bool_t      UpdateTimeStamp = false;

    // Need to update the Time stamp, but!
    // -- If the status is BAD - do not update.
    switch (Channel)
    {
        case CH_POSITION :                      // AO block
            if(p_PTB->setpoint_source == SP_SELECT_AOFB)
            {
                UpdateTimeStamp = ff_AlertSPTimeout_StatusOK(p_PTB->final_value.status);
            }
            break;

        case CH_POSITION_OPEN_CLOSE :           // DO block BoolSP
            // Get the SP Time Stamp -- to be used later
            if(p_PTB->setpoint_source == SP_SELECT_DOBOOL)
            {
                UpdateTimeStamp = ff_AlertSPTimeout_StatusOK(p_PTB->final_value_d.status);
            }
            break;

        case CH_POSITION_DISCRETE_POSITION :    // DO block IntSP
            if(p_PTB->setpoint_source == SP_SELECT_DOINT)
            {
                UpdateTimeStamp = ff_AlertSPTimeout_StatusOK(p_PTB->final_value_dint.status);
            }
            break;

        default :
            break;
    }

    MN_ENTER_CRITICAL();

        // Here we have a local flag that indicates that we should or should not update the TimeStamp
        if (UpdateTimeStamp)
        {
            Alert_SPTimeout_TimeStamp = osif_get_time_in_ms();
        }

    MN_EXIT_CRITICAL();
}

static bool_t   AlertHandle_Position(T_FBIF_PTB        *p_PTB,
                                     AlertType_t        AlertType)
{
    // 0 POSITION
    // 1 ALERT_POINT
    // 2 DEADBAND
    // 3 ALERT
    // 4 HISTORIC ALERT
    // 5 ENABLE

    FLOAT_S            *pWPosInTU;
    FLOAT               Position;
    _POSITION_ALERT    *pAlert;
    USIGN8              VarQuality;
    bool_t              AlertDirectionUP = false;
    bool_t              OutsideOfRange;
    bool_t              InsideDeadBand;
    bool_t              AlertEnabled;
    bool_t              SetAlert;
    bool_t              ClearAlert;
    bool_t              retval = false;

    // Preset the data for the alert
    switch (AlertType)
    {
        case ALERT_TYPE_POSITION_HIHI :
            pAlert              = &p_PTB->position_hihi_alert;
            AlertDirectionUP    = true;
            AlertEnabled        = (bool_t)pAlert->component_5;
            break;

        case ALERT_TYPE_POSITION_HI :
            pAlert              = &p_PTB->position_hi_alert;
            AlertDirectionUP    = true;
            AlertEnabled        = (bool_t)pAlert->component_5;
            break;

        case ALERT_TYPE_POSITION_LO :
            pAlert              = &p_PTB->position_lo_alert;
            AlertDirectionUP    = false;
            AlertEnabled        = (bool_t)pAlert->component_5;
            break;

        case ALERT_TYPE_POSITION_LOLO :
            pAlert              = &p_PTB->position_lolo_alert;
            AlertDirectionUP    = false;
            AlertEnabled        = (bool_t)pAlert->component_5;
            break;

        default :
            pAlert              = NULL;
            AlertEnabled        = false;
            break;
    }

    if (pAlert != NULL)
    {   // Alert is present

        // Using the Working Position in Travel Units
        pWPosInTU = TB_Get_WorkingPos_InTravelUnits(p_b_instance);

        Position            = pWPosInTU->value;
        VarQuality          = pWPosInTU->status & QUALITY_MASK;  // Separate the Parameter Quality
        pAlert->component_0 = Position;

        if (AlertEnabled)
        {   // Check the Var Quality
            if ((VarQuality == SQ_BAD) || (VarQuality == SQ_UNCERTAIN))
            {   // Quality NOT GOOD
                AlertEnabled = false;
            }
        }

        if (AlertEnabled)
        {   // If the Alert enabled
            if (AlertDirectionUP)
            {
                OutsideOfRange = ABOVE_THHOLD(Position,pAlert->component_1);        // In range or OUTSIDE?
                InsideDeadBand = BELOW_DBAND(Position,pAlert->component_1,pAlert->component_2);
            }
            else
            {
                OutsideOfRange = BELOW_THHOLD(Position,pAlert->component_1);        // In range or OUTSIDE?
                InsideDeadBand = ABOVE_DBAND(Position,pAlert->component_1,pAlert->component_2);
            }

            SetAlert   = (!((bool_t)pAlert->component_3)) && OutsideOfRange;
            ClearAlert =   ((bool_t)pAlert->component_3)  && InsideDeadBand;

            if (SetAlert)
            {   // Alert is NOT set and is outside -> SET ALERT!
                pAlert->component_3 = (BOOL)true;
                pAlert->component_4 = (BOOL)true;
            }

            if (ClearAlert)
            {   // Alert is set and is inside deadband -- clear alert
                pAlert->component_3 = (BOOL)false;
            }
        }
        else
        {   // Alert disabled --
            pAlert->component_3 = (BOOL)false;
            pAlert->component_4 = (BOOL)false;
        }

        retval = (bool_t)pAlert->component_3;
    }
    else
    {   // Alert is not present
        // Nothing
    }

    return retval;
}

//----------------------------------------------------------------------------------

#define TRAVEL_ACC_A        (0u)
#define TRAVEL_ACC_B        (TRAVEL_ACC_A + 1u)
#define TOTAL_TRAVEL_ACCS   (TRAVEL_ACC_B + 1u)

typedef struct TravelAcc_t
{
    FLOAT   LastPos;
    bool_t  Initialized;
    FLOAT   Acc;

} TravelAcc_t;

static TravelAcc_t TravelAlertsData[TOTAL_TRAVEL_ACCS] =
{
    [TRAVEL_ACC_A] = {0.0f, false, 0.0f},
    [TRAVEL_ACC_B] = {0.0f, false, 0.0f}
};

static bool_t   AlertHandle_TravelAcc(T_FBIF_PTB           *p_PTB,
                                      AlertType_t           AlertType)
{
    // 0 TRAVEL_ACCUMULATION
    // 1 ALERT_POINT
    // 2 ALERT
    // 3 HISTORIC ALERT
    // 4 ENABLE

    bool_t        retval = false;
    _TRAVEL_ALERT *pAlert;
    USIGN8        alertIndx = 0;

    if (ALERT_TYPE_TRAVELACC_A == AlertType)
    {
        pAlert = &p_PTB->travel_accumulation_a_alert;
        alertIndx = TRAVEL_ACC_A;
    }
    else if (ALERT_TYPE_TRAVELACC_B == AlertType)
    {
        pAlert = &p_PTB->travel_accumulation_b_alert;
        alertIndx = TRAVEL_ACC_B;
    }
    else
    {   //Non of expected alerts
        // Nothing
        pAlert = NULL;
    }

    if (pAlert != NULL)
    {
        if ((bool_t)pAlert->enable)
        {
            FLOAT travel = 0.0f;
            if ((bool_t)TravelAlertsData[alertIndx].Initialized)
            {
                travel = ABS(p_PTB->working_pos.value - TravelAlertsData[alertIndx].LastPos) * (p_PTB->travel.component_0) * 0.01f;
            }
            else
            {
                TravelAlertsData[alertIndx].Initialized = true;
                TravelAlertsData[alertIndx].LastPos = p_PTB->working_pos.value;
            }

            //TravelAlertsData[alertIndx].LastPos = p_PTB->working_pos.value;

            if(ABOVE_THHOLD(travel,pAlert->deadband_ab))
            {
                TravelAlertsData[alertIndx].LastPos = p_PTB->working_pos.value;
                TravelAlertsData[alertIndx].Acc += travel;
                USIGN32 u32_travel = (USIGN32)TravelAlertsData[alertIndx].Acc;
                TravelAlertsData[alertIndx].Acc -= (FLOAT)u32_travel;
                pAlert->travel_accumulation += u32_travel;
            }

            if(ABOVE_THHOLD(pAlert->travel_accumulation,pAlert->alert_point))
            {
                pAlert->alert = pAlert->historic_alert = (BOOL)true;
            }
            else
            {
                pAlert->alert = (BOOL)false;
            }
        }
        else
        {   // Alert disabled --
            pAlert->alert = (BOOL)false;
            pAlert->historic_alert = (BOOL)false;
        }

        retval = (bool_t)pAlert->alert;
    }
    return retval;
}

//----------------------------------------------------------------------------------

#define CYC_COUNTER_A           (0u)
#define CYC_COUNTER_B           (CYC_COUNTER_A + 1u)
#define TOTAL_CYC_COUNTERS      (CYC_COUNTER_B + 1u)

typedef struct CycCounter_t
{
    FLOAT   LastPos;
    bool_t  UpDirection;            // true - is UP, false is DOWN

} CycCounter_t;

static CycCounter_t CycCounter[TOTAL_CYC_COUNTERS] =
{
    [CYC_COUNTER_A] = {0.0f, false},
    [CYC_COUNTER_B] = {0.0f, false}
};

static bool_t   AlertHandle_Counter(T_FBIF_PTB       *p_PTB,
                                    AlertType_t       AlertType)
{
    // 0 CYCLE_COUNTER
    // 1 ALERT_POINT
    // 2 DEADBAND
    // 3 ALERT
    // 4 HISTORIC ALERT
    // 5 ENABLE

    _CYCLE_COUNTER_ALERT *pAlert;
    bool_t               retval = false;
    USIGN8               alertIndx = 0;

    if (ALERT_TYPE_CYCLECNT_A == AlertType)
    {
        pAlert = &p_PTB->cycle_counter_a_alert;
        alertIndx = CYC_COUNTER_A;
    }
    else if (ALERT_TYPE_CYCLECNT_B == AlertType)
    {
        pAlert = &p_PTB->cycle_counter_b_alert;
        alertIndx = CYC_COUNTER_B;
    }
    else
    {   //Non of expected alerts
        // Nothing
        pAlert = NULL;
    }

    if (pAlert != NULL)
    {
        if ((bool_t)pAlert->enable)
        {
            FLOAT travel = p_PTB->working_pos.value - CycCounter[alertIndx].LastPos;

            if(ABOVE_THHOLD(ABS(travel),pAlert->deadband_ab))
            {
                CycCounter[alertIndx].LastPos = p_PTB->working_pos.value;

                if((travel < 0 && CycCounter[alertIndx].UpDirection) || (travel > 0 && !(CycCounter[alertIndx].UpDirection)))
                {
                    pAlert->cycle_counter++;
                    CycCounter[alertIndx].UpDirection = !(CycCounter[alertIndx].UpDirection);
                }
            }

            if (ABOVE_THHOLD(pAlert->cycle_counter,pAlert->alert_point))
            {
                pAlert->alert = pAlert->historic_alert = (BOOL)true;
            }
            else
            {
                pAlert->alert = (BOOL)false;
            }
        }
        else
        {   // Alert disabled --
            pAlert->alert = (BOOL)false;
            pAlert->historic_alert = (BOOL)false;
        }

        retval = (bool_t)pAlert->alert;
    }
    else
    {   // Alert is not present
        // Nothing
    }
    return  retval;
}

//----------------------------------------------------------------------------------

static bool_t   AlertHandle_System(T_FBIF_PTB          *p_PTB,
                                   AlertType_t          AlertType)
{
// PRESSURE or TEMPERATURE
// ALERT_POINT
// DEADBAND
// ALERT
// HISTORIC ALERT
// ENABLE

    FLOAT               Value;
    _SYSTEM_ALERT      *pAlert;
    USIGN8              VarQuality;
    bool_t              AlertDirectionUP = false;
    bool_t              OutsideOfRange;
    bool_t              InsideDeadBand;
    bool_t              AlertEnabled;
    bool_t              SetAlert;
    bool_t              ClearAlert;
    bool_t              retval = false;

    // Preset the data for the alert
    switch (AlertType)
    {
        case ALERT_TYPE_SUP_PRES_HI :
            pAlert              = &p_PTB->supply_pressure_hi_alert;
            AlertEnabled        = (bool_t)pAlert->component_5;
            AlertDirectionUP    = true;

            Value               = p_PTB->supply_pressure.value;
            pAlert->component_0 = Value;
            VarQuality          = p_PTB->supply_pressure.status;  // Separate the Parameter Quality
            break;

        case ALERT_TYPE_SUP_PRES_LO :
            pAlert              = &p_PTB->supply_pressure_lo_alert;
            AlertEnabled        = (bool_t)pAlert->component_5;
            AlertDirectionUP    = false;

            Value               = p_PTB->supply_pressure.value;
            pAlert->component_0 = Value;
            VarQuality          = p_PTB->supply_pressure.status;  // Separate the Parameter Quality
            break;

        case ALERT_TYPE_SUP_PRES_LOLO :
            pAlert              = &p_PTB->supply_pressure_lolo_alert;
            AlertEnabled        = (bool_t)pAlert->component_5;
            AlertDirectionUP    = false;

            Value               = p_PTB->supply_pressure.value;
            pAlert->component_0 = Value;
            VarQuality          = p_PTB->supply_pressure.status;  // Separate the Parameter Quality
            break;

        case ALERT_TYPE_TEMP_HI :
            pAlert              = &p_PTB->temperature_hi_alert;
            AlertEnabled        = (bool_t)pAlert->component_5;
            AlertDirectionUP    = true;

            Value               = p_PTB->temperature.value;
            pAlert->component_0 = Value;
            VarQuality          = p_PTB->temperature.status;  // Separate the Parameter Quality
            break;

        case ALERT_TYPE_TEMP_LO :
            pAlert              = &p_PTB->temperature_lo_alert;
            AlertEnabled        = (bool_t)pAlert->component_5;
            AlertDirectionUP    = false;

            Value               = p_PTB->temperature.value;
            pAlert->component_0 = Value;
            VarQuality          = p_PTB->temperature.status;  // Separate the Parameter Quality
            break;

        default :
            Value               = 0.0F;
            AlertEnabled        = false;
            pAlert              = NULL;
            VarQuality          = SQ_BAD;
            break;
    }

    if (pAlert != NULL)
    {   // Alert is present
        if (AlertEnabled)
        {   // Check the Var quality
            VarQuality = VarQuality & QUALITY_MASK;
            if ((VarQuality == SQ_BAD) || (VarQuality == SQ_UNCERTAIN))
            {   // Quality NOT GOOD
                AlertEnabled = false;
            }
        }

        // Now process the Alert
        if (AlertEnabled)
        {   // If the Alert enabled
            if (AlertDirectionUP)
            {
                OutsideOfRange = ABOVE_THHOLD(Value,pAlert->component_1);        // In range or OUTSIDE?
                InsideDeadBand = BELOW_DBAND(Value,pAlert->component_1,pAlert->component_2);
            }
            else
            {
                OutsideOfRange = BELOW_THHOLD(Value,pAlert->component_1);        // In range or OUTSIDE?
                InsideDeadBand = ABOVE_DBAND(Value,pAlert->component_1,pAlert->component_2);
            }

            SetAlert   = (!((bool_t)pAlert->component_3)) && OutsideOfRange;
            ClearAlert =   ((bool_t)pAlert->component_3)  && InsideDeadBand;

            if (SetAlert)
            {   // Alert is NOT set and is outside -> SET ALERT!
                pAlert->component_3 = (BOOL)true;
                pAlert->component_4 = (BOOL)true;
            }

            if (ClearAlert)
            {   // Alert is set and is inside deadband -- clear alert
                pAlert->component_3 = (BOOL)false;
            }
        }
        else
        {   // Alert disabled --
            pAlert->component_3 = (BOOL)false;
            pAlert->component_4 = (BOOL)false;
        }

        retval = (bool_t)pAlert->component_3;
    }
    else
    {   // Alert is not present
        // Nothing
    }

    return retval;
}

static bool_t   AlertHandle_SPTimeout(T_FBIF_PTB          *p_PTB,
                                   AlertType_t          AlertType)
{
// TIME_SINCE_UPDATE - component_0
// ALERT_POINT
// MAXTIME
// ALERT
// HISTORIC ALERT
// ENABLE

    _SYSTEM_ALERT      *pAlert;
    bool_t              retval = false;

    if (AlertType == ALERT_TYPE_SP_TIMEOUT)
    {
        pAlert = &p_PTB->setpoint_timeout_alert;
    }
    else
    {
        pAlert = NULL;
    }

    if (pAlert != NULL)
    { // alert present
        if ((bool_t)pAlert->component_5)
        { //alert enabled
            float32 duration_time_s;
            u32 current_timestamp;

            current_timestamp = osif_get_time_in_ms();
            duration_time_s = MS_TO_SEC((float32)current_timestamp - Alert_SPTimeout_TimeStamp);
            pAlert->component_0 = duration_time_s;
            if (pAlert->component_0 > pAlert->component_2)
            {
                pAlert->component_2 = pAlert->component_0;
            }
            //check the alert
            if (ABOVE_THHOLD(pAlert->component_0,pAlert->component_1))
            {
                pAlert->component_3 = pAlert->component_4 = (BOOL)true;
            }
            else
            {
                pAlert->component_3 = (BOOL)false;
            }
            retval = (bool_t)pAlert->component_3;
        }
    }

    return retval;

}
//------------------------------------------------------------------------------------------------
// Time stamps for each

#define TIME_HANDLING_STRUC_HI      (0u)
#define TIME_HANDLING_STRUC_LO      (1u)

static Alert_TimeHandling_t Alert_IPCurHandle[] =
{
    [0u] = {0u,    false},
    [1u] = {0u,    false},
};

static bool_t   AlertHandle_IPCurrent(T_FBIF_PTB           *p_PTB,
                                      AlertType_t           AlertType)
{
    _IP_DRIVE_CURRENT_ALERT    *pAlert;
    FLOAT                       Value;
    USIGN8                      VarQuality;
    bool_t                      AlertDirectionUP = false;
    bool_t                      OutsideOfRange;
    bool_t                      InsideDeadBand;
    bool_t                      SetAlert;
    bool_t                      ClearAlert;
    bool_t                      AlertEnabled;
    USIGN32                     CurrentTime;
    USIGN32                     TimeDiff;
    USIGN32                     TimeDiffInSec;
    USIGN32                     TimeHandlingIndex = TIME_HANDLING_STRUC_LO;
    bool_t                      retval = false;

    // Preset the data for the alert
    switch (AlertType)
    {
        case ALERT_TYPE_IPCUR_HI :
            pAlert              = &p_PTB->ip_drive_current_hi_alert;
            AlertEnabled        = (bool_t)pAlert->enable;
            AlertDirectionUP    = true;
            TimeHandlingIndex   = TIME_HANDLING_STRUC_HI;
            break;

        case ALERT_TYPE_IPCUR_LO :
            pAlert              = &p_PTB->ip_drive_current_lo_alert;
            AlertEnabled        = (bool_t)pAlert->enable;
            AlertDirectionUP    = false;
            TimeHandlingIndex   = TIME_HANDLING_STRUC_LO;
            break;

        default :
            AlertEnabled        = false;
            pAlert              = NULL;
            break;
    }

    if (pAlert != NULL)
    {   // Alert is present
        Value = p_PTB->ip_drive_current.value;
        pAlert->current = Value;                                     //Type mismatch between IP parameter and Alert
        VarQuality = p_PTB->ip_drive_current.status & QUALITY_MASK;  // Separate the Parameter Quality

        if (AlertEnabled)
        {   // Check Var Quality
            if ((VarQuality == SQ_BAD) || (VarQuality == SQ_UNCERTAIN))
            {   // Quality NOT GOOD
                AlertEnabled = false;
            }
        }

        if (AlertEnabled)
        {   // If the Alert enabled
            if (AlertDirectionUP)
            {
                OutsideOfRange = ABOVE_THHOLD(Value,pAlert->alert_point);        // In range or OUTSIDE?
                InsideDeadBand =  BELOW_DBAND(Value,pAlert->alert_point,pAlert->deadband);
            }
            else
            {
                OutsideOfRange = BELOW_THHOLD(Value,pAlert->alert_point);        // In range or OUTSIDE?
                InsideDeadBand =  ABOVE_DBAND(Value,pAlert->alert_point,pAlert->deadband);
            }

            SetAlert   = (!((bool_t)pAlert->alert)) && OutsideOfRange;
            ClearAlert =   ((bool_t)pAlert->alert)  && InsideDeadBand;

            if (SetAlert)
            {   // Alert is NOT set and is outside -> SET ALERT!
                CurrentTime = osif_get_time_in_ms();

                if (!Alert_IPCurHandle[TimeHandlingIndex].Latch)
                {
                    Alert_IPCurHandle[TimeHandlingIndex].Latch     = true;
                    Alert_IPCurHandle[TimeHandlingIndex].TimeStamp = CurrentTime;
                }

                if (Alert_IPCurHandle[TimeHandlingIndex].Latch)
                {
                    TimeDiff      = TIME_DIFF(Alert_IPCurHandle[TimeHandlingIndex].TimeStamp,CurrentTime);
                    TimeDiffInSec = MS_TO_SEC(TimeDiff);
                    if (TimeDiffInSec >= pAlert->time)
                    {   // Time expired
                        pAlert->alert          = (BOOL)true;
                        pAlert->historic_alert = (BOOL)true;
                    }
                }
            }
            else
            {
                Alert_IPCurHandle[TimeHandlingIndex].Latch     = false;
                Alert_IPCurHandle[TimeHandlingIndex].TimeStamp = 0u;
            }

            if (ClearAlert)
            {   // Alert is set and is inside deadband -- clear alert
                pAlert->alert = (BOOL)false;
                Alert_IPCurHandle[TimeHandlingIndex].Latch     = false;
                Alert_IPCurHandle[TimeHandlingIndex].TimeStamp = 0u;
            }
        }
        else
        {   // Alert disabled --
            pAlert->alert          = (BOOL)false;
            pAlert->historic_alert = (BOOL)false;
            Alert_IPCurHandle[TimeHandlingIndex].Latch     = false;
            Alert_IPCurHandle[TimeHandlingIndex].TimeStamp = 0u;
        }

        retval = (bool_t)pAlert->alert;
    }
    else
    {   // Alert is not present
        // Nothing
    }

    return retval;
}

//----------------------------------------------------------------------------------

#if 0

static Alert_WorkingTime_t  Alert_WorkingTime =
{
    .TimeStamp_ms   = 0u,
    .TimeAccum_ms   = 0u,
};

#endif

static bool_t   AlertHandle_WorkingTime(T_FBIF_PTB           *p_PTB,
                                        AlertType_t           AlertType)
{
// TOTAL_TIME - in hours
// ALERT_POINT - in hours
// ALERT
// HISTORIC ALERT
// ENABLE

//    UNUSED_OK(AlertType);
//    UNUSED_OK(p_PTB);

//    return false;

    _WORKING_TIME_ALERT *pAlert;
    bool_t              alertEnabled;
    bool_t              retval = false;

    if (AlertType == ALERT_TYPE_WORKTIME)
    {
        pAlert = &p_PTB->working_time_alert;
        alertEnabled = (bool_t)pAlert->enable;
    }
    else
    {
        pAlert       = NULL;
        alertEnabled = false;
    }

    if (pAlert != NULL)
    {   // Alert is present
        //transfer the working total_time to the alert
        pAlert->total_time = p_PTB->working_times.total_time;
        // Now process the Alert
        if (alertEnabled)
        {   // If the Alert enabled

            if (ABOVE_THHOLD(pAlert->total_time,pAlert->alert_point))
            {   // Alert is active
                pAlert->alert = (BOOL)true;
                pAlert->historic_alert = (BOOL)true;
            }
            else
            {
                pAlert->alert = (BOOL)false;
            }
        }
        else
        {   // Alert disabled --
            pAlert->alert = (BOOL)false;
            pAlert->historic_alert = (BOOL)false;
        }

        retval = (bool_t)pAlert->alert;
    }
    else
    {   // Alert is not present
        // Nothing
    }

    return retval;

}

static bool_t   AlertHandle_NearClosed(T_FBIF_PTB           *p_PTB,
                                        AlertType_t           AlertType)
{
    bool_t              retval = false;
    _NEAR_CLOSED_ALERT* pAlert;

    static USIGN32 enter_time = 0;
    static USIGN32 total_time = 0;

    if (AlertType == ALERT_TYPE_NEAR_CLOSE)
    {
        pAlert = &p_PTB->near_closed_alert;
    }
    else
    {   //Non of expected alert
        // Nothing
        pAlert = NULL;
    }

    if (pAlert != NULL)
    {
        USIGN8 sp_quality = p_PTB->working_sp.status & QUALITY_MASK;
        if ((bool_t)pAlert->enable
            && (sp_quality != SQ_UNCERTAIN)
            && (sp_quality != SQ_BAD)
            && (MODE_AUTO == p_PTB->mode_blk.actual || MODE_LO == p_PTB->mode_blk.actual || MODE_MAN == p_PTB->mode_blk.actual)
             )
        {
            USIGN32 current_time = osif_get_time_in_ms();

            if(BELOW_THHOLD(p_PTB->working_pos.value,pAlert->position_closed) && !mn_IsAppFault(p_PTB, FAULT_POS_CUTOFF_LO))
            {
                //valve is near closed - start measuring the time
                if (enter_time != 0)
                {
                    total_time += (current_time - enter_time);

                    if (TIME_INSECOND_ONEHOUR <= MS_TO_SEC(total_time))
                    {
                        pAlert->near_closed++;
                        total_time = 0;
                    }
                }
                //set initial time point
                enter_time = current_time;
            }
            else
            {
                //valve go out of 'near closed' position
                //reset measuring
                enter_time = 0;
            }

            //check the alert
            if (ABOVE_THHOLD(pAlert->near_closed,pAlert->alert_point))
            {
                pAlert->alert = pAlert->historic_alert = (BOOL)true;
            }
            else
            {
                pAlert->alert = (BOOL)false;
            }

            retval = (bool_t)pAlert->alert;
        }
        else if(!(bool_t)pAlert->enable)
        {
            pAlert->alert = (BOOL)false;
        }
        else
        {
            //nothing
        }
    }

    return retval;
}

//---------------------------------------------------------------------
// Time stamps for each

static Alert_TimeHandling_t Alert_Deviation =
{
    0u,     false
};

static bool_t   AlertHandle_Deviation(T_FBIF_PTB           *p_PTB,
                                      AlertType_t           AlertType)
{
// DEVIATION_VALUE
// ALERT_POINT
// DEADBAND
// TIME - in seconds
// ALERT
// HISTORIC ALERT
// ENABLE

    FLOAT               Deviation;
    FLOAT               AbsDeviation;
    _DEVIATION_ALERT   *pAlert;
    USIGN8              VarQuality;
    bool_t              OutsideOfRange;
    bool_t              InsideDeadBand;
    bool_t              SetAlert;
    bool_t              ClearAlert;
    bool_t              AlertEnabled;
    USIGN32             CurrentTime;
    USIGN32             TimeDiff;
    bool_t              retval = false;

    if (AlertType == ALERT_TYPE_DEVIATION)
    {
        pAlert = &p_PTB->deviation_alert;
        AlertEnabled = (bool_t)pAlert->component_6;
    }
    else
    {
        pAlert       = NULL;
        AlertEnabled = false;
    }

    if (pAlert != NULL)
    {
        if (AlertEnabled)
        {   // Check Var Quality
            VarQuality = p_PTB->working_pos.status;  // Separate the Parameter Quality
            if ((VarQuality == SQ_BAD) || (VarQuality == SQ_UNCERTAIN))
            {   // Quality NOT GOOD
                AlertEnabled = false;
            }

            VarQuality = p_PTB->working_sp.status;         // Separate the Parameter Quality
            if ((VarQuality == SQ_BAD) || (VarQuality == SQ_UNCERTAIN))
            {   // Quality NOT GOOD
                AlertEnabled = false;
            }
        }

        if (AlertEnabled)
        {   // If the Alert enabled
            Deviation = p_PTB->working_sp.value - p_PTB->working_pos.value;
            AbsDeviation = ABS(Deviation);
            pAlert->component_0 = AbsDeviation;

            OutsideOfRange = ABOVE_THHOLD(AbsDeviation,pAlert->component_1);        // In range or OUTSIDE?
            InsideDeadBand = BELOW_DBAND(AbsDeviation,pAlert->component_1,pAlert->component_2);

            SetAlert   = (!((bool_t)pAlert->component_4)) && OutsideOfRange;
            ClearAlert =   ((bool_t)pAlert->component_4)  && InsideDeadBand;

            if (SetAlert)
            {
                CurrentTime = osif_get_time_in_ms();

                if (!Alert_Deviation.Latch)
                {   // Did we hit the condition FIRST time -- YES
                    Alert_Deviation.Latch     = true;
                    Alert_Deviation.TimeStamp = CurrentTime;
                }

                if (Alert_Deviation.Latch)
                {   // Latched condition -- check the T/O

                    TimeDiff = TIME_DIFF(Alert_Deviation.TimeStamp,CurrentTime);
                    if (MS_TO_SEC(TimeDiff) >= pAlert->component_3)
                    {   // Time expired
                        pAlert->component_4 = (BOOL)true;
                        pAlert->component_5 = (BOOL)true;
                    }
                }
            }
            else
            {
                Alert_Deviation.Latch     = false;
                Alert_Deviation.TimeStamp = 0u;
            }

            if (ClearAlert)
            {   // Alert is set and is inside deadband -- clear alert
                pAlert->component_4 = (BOOL)false;
                Alert_Deviation.Latch     = false;
                Alert_Deviation.TimeStamp = 0u;
            }
        }
        else
        {   // Alert disabled --
            pAlert->component_4 = (BOOL)false;
            pAlert->component_5 = (BOOL)false;
            Alert_Deviation.Latch     = false;
            Alert_Deviation.TimeStamp = 0u;
        }

        retval = (bool_t)pAlert->component_4;
    }
    else
    {   // Alert is not present
        // Nothing
    }

    return retval;
}


//---------------------------------------------------------------------
// Talbe of function pointers for Alert processing

typedef struct AlertFunctionTable_t
{
    // Pointer to an Alert processing function
    bool_t      (* const pAlert_Process)(T_FBIF_PTB *p_PTB, AlertType_t AlertType);
    u16         AlertBitMap;

} AlertFunctionTable_t;

static const AlertFunctionTable_t AlertFunctionTable[ALERT_TYPE_MAX] =
{
    [ALERT_TYPE_DEVIATION]      = {AlertHandle_Deviation,           ALERT_INDEX(deviation_alert)},

    [ALERT_TYPE_POSITION_HIHI]  = {AlertHandle_Position,            ALERT_INDEX(position_hihi_alert)},
    [ALERT_TYPE_POSITION_HI]    = {AlertHandle_Position,            ALERT_INDEX(position_hi_alert)},
    [ALERT_TYPE_POSITION_LO]    = {AlertHandle_Position,            ALERT_INDEX(position_lo_alert)},
    [ALERT_TYPE_POSITION_LOLO]  = {AlertHandle_Position,            ALERT_INDEX(position_lolo_alert)},

    [ALERT_TYPE_TRAVELACC_A]    = {AlertHandle_TravelAcc,           ALERT_INDEX(travel_accumulation_a_alert)},
    [ALERT_TYPE_TRAVELACC_B]    = {AlertHandle_TravelAcc,           ALERT_INDEX(travel_accumulation_b_alert)},

    [ALERT_TYPE_CYCLECNT_A]     = {AlertHandle_Counter,             ALERT_INDEX(cycle_counter_a_alert)},
    [ALERT_TYPE_CYCLECNT_B]     = {AlertHandle_Counter,             ALERT_INDEX(cycle_counter_b_alert)},

    [ALERT_TYPE_SUP_PRES_HI]    = {AlertHandle_System,              ALERT_INDEX(supply_pressure_hi_alert)},
    [ALERT_TYPE_SUP_PRES_LO]    = {AlertHandle_System,              ALERT_INDEX(supply_pressure_lo_alert)},
    [ALERT_TYPE_SUP_PRES_LOLO]  = {AlertHandle_System,              ALERT_INDEX(supply_pressure_lolo_alert)},
    [ALERT_TYPE_TEMP_HI]        = {AlertHandle_System,              ALERT_INDEX(temperature_hi_alert)},
    [ALERT_TYPE_TEMP_LO]        = {AlertHandle_System,              ALERT_INDEX(temperature_lo_alert)},
    [ALERT_TYPE_SP_TIMEOUT]     = {AlertHandle_SPTimeout,           ALERT_INDEX(set_point_timeout_alert)},

    [ALERT_TYPE_IPCUR_HI]       = {AlertHandle_IPCurrent,           ALERT_INDEX(ip_drive_current_alert_hi)},
    [ALERT_TYPE_IPCUR_LO]       = {AlertHandle_IPCurrent,           ALERT_INDEX(ip_drive_current_alert_lo)},

    [ALERT_TYPE_WORKTIME]       = {AlertHandle_WorkingTime,         ALERT_INDEX(working_time_alert)},

    [ALERT_TYPE_NEAR_CLOSE]     = {AlertHandle_NearClosed,          ALERT_INDEX(near_close_alert)},
    [ALERT_TYPE_SENSOR_FAILED]  = {NULL,                            0u},
    [ALERT_TYPE_PROCESSOR]      = {NULL,                            0u},
    [ALERT_TYPE_VALVE_CONTROL]  = {NULL,                            0u},
    [ALERT_TYPE_MISSION]        = {NULL,                            0u},
    [ALERT_TYPE_AIR_SUPPLY]     = {NULL,                            0u},
    [ALERT_TYPE_HARDWARE]       = {NULL,                            0u}
};

//----------------------------------------------------------------------------------------------

static void WriteEntry(const T_FBIF_BLOCK_INSTANCE * p_block_instance,
                       u32 BitMap);

//-----------------------------------------------------------------------------------------------
// Number of reads of the state machine
#define MAX_CYCLES_OF_STATEMACHINE           (2u)

static u8       IPC_ReadsOfStateMachine     = 0u;
static bool_t   Alerts_Stable               = false;

void    Alert_IPCStateMachineExecuted(void)
{
    if (IPC_ReadsOfStateMachine >= MAX_CYCLES_OF_STATEMACHINE)
    {
        Alerts_Stable = true;
    }
    else
    {
        Alerts_Stable = false;
        IPC_ReadsOfStateMachine++;
    }
}

u32     Execute_PTB_Alerts(const T_FBIF_BLOCK_INSTANCE * p_block_instance)
{
    T_FBIF_PTB* p_PTB = p_block_instance->p_block_desc->p_block;
    AlertType_t         AlertType;
    bool_t              funcval;
    u32                 TotalAlertBitMap = 0u;

    //STore instance
    p_b_instance = p_block_instance;

    if (Alerts_Stable)
    {
        if (p_PTB->mode_blk.actual == MODE_AUTO)
        {
            for (AlertType  = ALERT_FIRST_ALERT;
                 AlertType != ALERT_TYPE_MAX;
                 AlertType++)
            {
                if (AlertFunctionTable[AlertType].pAlert_Process != NULL)
                {   // Check if the processing function available
                    // And then invoke the function
                    funcval = AlertFunctionTable[AlertType].pAlert_Process(p_PTB, AlertType);
                    if (funcval)
                    {
                        TotalAlertBitMap |= 1u << AlertFunctionTable[AlertType].AlertBitMap;
                    }
                }
                else
                {
                    // If NULL - no function, do nothing.
                }
            }

            WriteEntry(p_block_instance, TotalAlertBitMap);
        }
    }

    return TotalAlertBitMap;
}

// -------------------------------------------------------------------
// Functions to get the states of specific alerts

bool_t  Alert_AirSupply(const T_FBIF_PTB *p_PTB)
{
    const _SYSTEM_ALERT    *pAlert;
    bool_t                  AlertEnabled;
    bool_t                  retval = false;

    // Note, there are 3 Air Supply Alerts, we return logical OR of them
    pAlert = &p_PTB->supply_pressure_hi_alert;
    AlertEnabled = (bool_t)pAlert->component_5;
    if (AlertEnabled)
    {
        retval = (retval || (bool_t)pAlert->component_3);
    }

    pAlert = &p_PTB->supply_pressure_lo_alert;
    AlertEnabled = (bool_t)pAlert->component_5;
    if (AlertEnabled)
    {
        retval = (retval || (bool_t)pAlert->component_3);
    }

    pAlert = &p_PTB->supply_pressure_lolo_alert;
    AlertEnabled = (bool_t)pAlert->component_5;
    if (AlertEnabled)
    {
        retval = (retval || (bool_t)pAlert->component_3);
    }

    return retval;
}

bool_t  Alert_TravelDeviation(const T_FBIF_PTB *p_PTB)
{
    const _DEVIATION_ALERT     *pAlert;
    bool_t                      AlertEnabled;
    bool_t                      retval = false;

    pAlert = &p_PTB->deviation_alert;
    AlertEnabled = (bool_t)pAlert->component_6;
    if (AlertEnabled)
    {
        retval = (bool_t)pAlert->component_4;
    }
    return retval;
}

bool_t  Alert_PositionHI(const T_FBIF_PTB *p_PTB)
{
    const _POSITION_ALERT      *pAlert;
    bool_t                      AlertEnabled;
    bool_t                      retval = false;

    pAlert = &p_PTB->position_hi_alert;
    AlertEnabled = (bool_t)pAlert->component_5;
    if (AlertEnabled)
    {
        retval = (bool_t)pAlert->component_3;
    }
    return retval;
}

bool_t  Alert_PositionLO(const T_FBIF_PTB *p_PTB)
{
    const _POSITION_ALERT      *pAlert;
    bool_t                      AlertEnabled;
    bool_t                      retval = false;

    pAlert = &p_PTB->position_lo_alert;
    AlertEnabled = (bool_t)pAlert->component_5;
    if (AlertEnabled)
    {
        retval = (bool_t)pAlert->component_3;
    }
    return retval;
}

//----------------------------------------------------------------------------------------------
// Alert Logger implementation
// Definitions for the alert log READ parameter

#define     ALERT_LOG_CMD_READ      (0x00)
#define     ALERT_LOG_CMD_CLEAR     (0xFA)
#define     ALERT_LOG_DATA_CLEAR    (0x00)
#define     ALERT_LOG_TIME_CLEAR    (0x00)

#define     ALERT_INVALID_INDEX     (0xFFFFu)

// Read index -- init to
static      u16     alert_ReadBufferIndex = ALERT_INVALID_INDEX;

// Read buffer. On a specific command the Alart Log Data will be copied
// to this buffer for reading
static  _ALERT_LOG      alert_ReadBuffer[ALERT_LOG_NUM_ENTRIES];

// The data are supposed to be initalized from FRAM
static  AlertStoreStruc_t   alert_AlertStorage;

//----------------------------------------------------------------------------------------------
void get_AlertLogPtr(void** pp_value, u16* p_len)
{
    *pp_value = &alert_AlertStorage;
    *p_len = sizeof(alert_AlertStorage);
}

//----------------------------------------------------------------------------------------------
// Copy Alerts for Reading

static void CopyAlertLog4Read(void)
{
    u16     TransferReadIndex;
    u16     TransferWriteIndex;

    TransferWriteIndex = 0u;
    osif_disable_all_tasks();
    TransferReadIndex  = alert_AlertStorage.StoreBufferOldIndex;
    while (TransferWriteIndex < ALERT_LOG_NUM_ENTRIES)
    {
        // Read the data
        alert_ReadBuffer[TransferWriteIndex].command = alert_AlertStorage.StoreDataSize;

        if (TransferWriteIndex < alert_AlertStorage.StoreDataSize)
        {   // Data entry is not empty
            alert_ReadBuffer[TransferWriteIndex].time    = alert_AlertStorage.Buffer[TransferReadIndex].time;
            alert_ReadBuffer[TransferWriteIndex].data    = alert_AlertStorage.Buffer[TransferReadIndex].data;
        }
        else
        {   // Data entry is empty
            alert_ReadBuffer[TransferWriteIndex].time.high = ALERT_LOG_TIME_CLEAR;
            alert_ReadBuffer[TransferWriteIndex].time.low  = ALERT_LOG_TIME_CLEAR;
            alert_ReadBuffer[TransferWriteIndex].data      = ALERT_LOG_DATA_CLEAR;
        }

        TransferWriteIndex++;
        TransferReadIndex++;
        if (TransferReadIndex >= ALERT_LOG_NUM_ENTRIES)
        {
            TransferReadIndex = 0u;
        }
    }

    // Reset the READ BUFFER
    alert_ReadBufferIndex = 0u;
    osif_enable_all_tasks();
}

static void WriteEntry(const T_FBIF_BLOCK_INSTANCE * p_block_instance, u32 BitMap)
{
    T_SYS_TIME                  SysTimeVal;
    TIME_VALUE_S                TimeVal;

    UNUSED_OK(p_block_instance);

    osif_disable_all_tasks();
    if (BitMap != alert_AlertStorage.Buffer[alert_AlertStorage.StoreBufferNewIndex].data)
    {   // Change detected! Add entry

        // Get the current time
        osif_get_system_time(&SysTimeVal);
        TimeVal.high = SysTimeVal.high;
        TimeVal.low  = SysTimeVal.low;

        if (alert_AlertStorage.StoreDataSize == 0u)
        {   // No data yet
            alert_AlertStorage.Buffer[alert_AlertStorage.StoreBufferNewIndex].data = BitMap;
            alert_AlertStorage.Buffer[alert_AlertStorage.StoreBufferNewIndex].time = TimeVal;
        }
        else
        {   // There are some data
            alert_AlertStorage.StoreBufferNewIndex++;                   // Next index
            if (alert_AlertStorage.StoreBufferNewIndex >= ALERT_LOG_NUM_ENTRIES)
            {
                alert_AlertStorage.StoreBufferNewIndex = 0u;
            }

            if (alert_AlertStorage.StoreDataSize >= ALERT_LOG_NUM_ENTRIES)
            {   // Pointing to the OLD entry (not empty)
                alert_AlertStorage.StoreBufferOldIndex++;
                if (alert_AlertStorage.StoreBufferOldIndex >= ALERT_LOG_NUM_ENTRIES)
                {
                    alert_AlertStorage.StoreBufferOldIndex = 0u;
                }
            }

            alert_AlertStorage.Buffer[alert_AlertStorage.StoreBufferNewIndex].data = BitMap;
            alert_AlertStorage.Buffer[alert_AlertStorage.StoreBufferNewIndex].time = TimeVal;
        }

        if (alert_AlertStorage.StoreDataSize < ALERT_LOG_NUM_ENTRIES)
        {   // If teh buffer is not full yet -- ncrement data counter
            alert_AlertStorage.StoreDataSize++;
        }

        (void)eep_blk2_store(MN_OFFSETOF(eep_blk2_t, eep_blk2_private)+MN_OFFSETOF(eep_blk2_private_t, AlertLog), sizeof(AlertStoreStruc_t), (void*)&alert_AlertStorage);
    }
    osif_enable_all_tasks();
}

// This function READS one entry from the Read Buffer. Supports Circular Reading
fferr_t     Alert_ReadAlertLogEntry(const T_FBIF_BLOCK_INSTANCE * p_block_instance,
                                    AlertBufferControl_t          Action)
{
    T_FBIF_PTB* p_PTB = p_block_instance->p_block_desc->p_block;

    if (alert_ReadBufferIndex == 0xFFFFu)
    {   // Read has not been initialized
        CopyAlertLog4Read();
    }

    osif_disable_all_tasks();
    // Initialized. Now do the read
    p_PTB->alert_log.command = alert_ReadBuffer[alert_ReadBufferIndex].command;
    p_PTB->alert_log.time    = alert_ReadBuffer[alert_ReadBufferIndex].time;
    p_PTB->alert_log.data    = alert_ReadBuffer[alert_ReadBufferIndex].data;

    if (Action == ALERT_AUTO_INC)
    {
        alert_ReadBufferIndex++;
        if (alert_ReadBufferIndex >= ALERT_LOG_NUM_ENTRIES)
        {
            alert_ReadBufferIndex = 0u;
        }
    }
    else
    {
        // No Action!!
    }
    osif_enable_all_tasks();

    return E_OK;
}

static void alert_ClearBuffer(void)
{
    u8              Index;

    osif_disable_all_tasks();
    for (Index = 0u;
         Index < ALERT_LOG_NUM_ENTRIES;
         Index++)
    {
        // Wipe out storage buffer
        alert_AlertStorage.Buffer[Index].time.high = ALERT_LOG_TIME_CLEAR;
        alert_AlertStorage.Buffer[Index].time.low  = ALERT_LOG_TIME_CLEAR;
        alert_AlertStorage.Buffer[Index].data      = ALERT_LOG_DATA_CLEAR;
        alert_AlertStorage.Buffer[Index].command   = 0u;
    }

    // Clear indexes
    alert_AlertStorage.StoreBufferOldIndex = 0u;
    alert_AlertStorage.StoreBufferNewIndex = 0u;
    alert_AlertStorage.StoreDataSize       = 0u;
    osif_enable_all_tasks();
}

// This function WRITES one entry from the Read Buffer. Supports Circular Reading
fferr_t     Alert_WriteAlertLogEntry(const T_FBIF_BLOCK_INSTANCE * p_block_instance)
{
    fferr_t         Retval = E_OK;
    T_FBIF_PTB*     p_PTB = p_block_instance->p_block_desc->p_block;

    switch (p_PTB->alert_log.command)
    {
        case ALERT_LOG_CMD_READ :
            // Prepare the Log Entry for reading
            CopyAlertLog4Read();
            break;

        case ALERT_LOG_CMD_CLEAR :
            // Clear the main Storage array
            alert_ClearBuffer();

            // Store data in NVRAM
            osif_disable_all_tasks();
            (void)eep_blk2_store(MN_OFFSETOF(eep_blk2_t, eep_blk2_private)+MN_OFFSETOF(eep_blk2_private_t, AlertLog), sizeof(AlertStoreStruc_t), (void*)&alert_AlertStorage);
            osif_enable_all_tasks();

            // Clear the Read buffer
            CopyAlertLog4Read();
            break;

        default :
            // Do nothing
            // Retval = E_ERROR;       // Bad parameter
            break;
    }

    return Retval;
}

void    Alert_InitAlertLog(const T_FBIF_BLOCK_INSTANCE * p_block_instance)
{
    UNUSED_OK(p_block_instance);

    // Read the Storage Array from NVRAM -- all 32 entries, indexes, size
    // This is done in the APPL_IF.C

    // Check if the data are legitimate -- size and indexes
    if ((alert_AlertStorage.StoreDataSize > ALERT_LOG_NUM_ENTRIES) ||
        (alert_AlertStorage.StoreBufferNewIndex >= ALERT_LOG_NUM_ENTRIES) ||
        (alert_AlertStorage.StoreBufferOldIndex >= ALERT_LOG_NUM_ENTRIES))
    {   // Data size is NOT valied or indexes are not valid.
        // clear the data
        alert_ClearBuffer();

        // Store data in NVRAM
        osif_disable_all_tasks();
        (void)eep_blk2_store(MN_OFFSETOF(eep_blk2_t, eep_blk2_private)+MN_OFFSETOF(eep_blk2_private_t, AlertLog), sizeof(AlertStoreStruc_t), (void*)&alert_AlertStorage);
        osif_enable_all_tasks();

        // Clear the Read buffer
        CopyAlertLog4Read();
    }
}

//----------------------------------------------------------------------------------------------
//                           -- -- -- -- End of file -- -- -- --
//----------------------------------------------------------------------------------------------

