#include <softing_base.h>
#include <ptb.h>

#include "hartdef.h"
#include "bufutils.h"
#include "ipcdefs.h"
#include "faultpublic.h"

#include "mnipcff.h"
#include "mntbmspff.h"
#include "mndoswff.h"
#include "appl_alert.h"
// Include the LIST of IPC communications Variable IDs
#include "ipcvarids.h"
#include "hartfunc.h"
#include "mn_advanced.h"
#include "ff_devicevars.h"
#include "hw_if.h"

// Offsets in the complete status array

#define COMPLETE_STATUS_TRANSFER_BUFFER_SIZE    (sizeof(u32))
#define COMPLETE_STATUS_BUFFER_SIZE             (COMPLETE_STATUS_TRANSFER_BUFFER_SIZE * 3)
CONST_ASSERT(FAULT_CODE_BYTES_REQUIRED <= COMPLETE_STATUS_BUFFER_SIZE);

#define COMPLETE_STATUS_CUR_LO_OFFSET           (0)
#define COMPLETE_STATUS_CUR_HI_OFFSET           (COMPLETE_STATUS_CUR_LO_OFFSET   + COMPLETE_STATUS_TRANSFER_BUFFER_SIZE)
#define COMPLETE_STATUS_CUR_HIHI_OFFSET         (COMPLETE_STATUS_CUR_HI_OFFSET   + COMPLETE_STATUS_TRANSFER_BUFFER_SIZE)
#define COMPLETE_STATUS_HIST_LO_OFFSET          (COMPLETE_STATUS_CUR_HIHI_OFFSET + COMPLETE_STATUS_TRANSFER_BUFFER_SIZE)
#define COMPLETE_STATUS_HIST_HI_OFFSET          (COMPLETE_STATUS_HIST_LO_OFFSET  + COMPLETE_STATUS_TRANSFER_BUFFER_SIZE)
#define COMPLETE_STATUS_HIST_HIHI_OFFSET        (COMPLETE_STATUS_HIST_HI_OFFSET  + COMPLETE_STATUS_TRANSFER_BUFFER_SIZE)


/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:

  This function is used to return the state of the Remote WP jumper.
  The function is called from module ffbl_res.c

PARAMETERS:
  None

RETURN_VALUES:
  BOOL - State of the Remote WP Jumper. The state is peridically updated via cmd 177.
----------------------------------------------------------------------------*/

// This is the Local buffer -- reflects the state of the Remote WP jumper.
static bool_t  Remote_WP_Jumper_State = true;

bool_t Remote_WP_Jumper(VOID)
{
    return Remote_WP_Jumper_State;
}

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:

  This function is used to return the state of the LUI Simulation.
  The function is called from module ffbl_res.c

PARAMETERS:
  None

RETURN_VALUES:
  BOOL - State of the LUI Simulation. The state is peridically updated via cmd 177.
----------------------------------------------------------------------------*/

// This is the Local buffer -- reflects the state of the LUI Simulation.
static bool_t  lui_Simulation = false;

bool_t Lui_Simulation(VOID)
{
    return lui_Simulation;
}

//---------------------------------------------------------------------------------------------
// IP Conversion definitions

// Counts at 0.0 mA -- 0 counts
//#define IP_CONV_ZERO_MA_COUNTS              (0.0f)

// Counts at 1.6 mA -- 16384 counts
#define IP_CONV_FP_1_6_MA                   (1.6f)
#define IP_CONV_1_6_MA_COUNTS               (16384.0f)

// MAX IP current in Float representation 1.67 mA
#define IP_CONV_FP_MAX_CURRENT              (1.67f)

//---------------------------------------------------------------------------------------------

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:

  This function is used to convert IP from counts to mA in FP

PARAMETERS:
  FLOAT - IP In counts (already converted from INT)

RETURN_VALUES:
  FLOAT   -- IP current in mA FP
----------------------------------------------------------------------------*/
FUNCTION LOCAL FLOAT IP_Conv_to_mA(FLOAT    fIPCounts)
{
    FLOAT   retval;

    // Conversion: retval = (Counts * 1.6mA) / 16384 cnts
    retval  = fIPCounts * IP_CONV_FP_1_6_MA;
    retval /= IP_CONV_1_6_MA_COUNTS;
    return retval;
}


/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:

  This function is used to convert IP from mA to %

PARAMETERS:
  FLOAT - IP In counts (already converted from INT)

RETURN_VALUES:
    FLOAT   -- IP current in % FP
----------------------------------------------------------------------------*/
FUNCTION LOCAL FLOAT IP_Conv_to_Percent(INT32   IPCounts)
{
    FLOAT retval;
    FLOAT fIPCounts = (FLOAT)IPCounts;

    if (fIPCounts < 0.0f)
    {   // Drop negative current
        fIPCounts = 0.0f;
    }

    // Conversion
    // Retval in counts -> retval in mA
    // (Retval_in_mA * 100%) / Max_IP_Current_1.67mA -> retval in %
    retval  = IP_Conv_to_mA(fIPCounts);             // Convert to mA
    retval *= ONE_HUNDRED_PERCENT_FLOAT;
    retval /= IP_CONV_FP_MAX_CURRENT;               // Convert to %
    return retval;
}

//-------------------------------------------------------------------------------------
// State machine for the cyclic IPC Parameter Exchange
//
//-------------------------------------------------------------------------------------


// Define the Dynamic Exchange command state machine
typedef struct IPC_Dynamic_StateMachine_t
{
    // Variable ID's to place into the  177 cmd request buffer for each cycle
    IPC_Variable_IDs_t     VarID1;
    IPC_Variable_IDs_t     VarID2;
} IPC_Dynamic_StateMachine_t;

//------------------------------------------------------------------------------------------
// State machine definitions

#define FIRST_PARAMETER_CYCLE               (0u)
#define NUMBER_OF_PARAMETER_CYCLES          (16u)
#define MAX_PARAMETER_CYCLE                 (NUMBER_OF_PARAMETER_CYCLES - 1u)

// State machine variable (state)
static USIGN8   IPCDynamicStateNumber = FIRST_PARAMETER_CYCLE;

// The actual state machine
static const IPC_Dynamic_StateMachine_t IPC_Dynamic_StateMachine[NUMBER_OF_PARAMETER_CYCLES] =
{
    [ 0u] = {IPC_PRESSURE_SUPPLY,               IPC_PRESSURE_ATMOS          },      //  1
    [ 1u] = {IPC_PRESSURE_ACTUATOR_A,           IPC_PRESSURE_ACTUATOR_B     },      //  2
    [ 2u] = {IPC_WORKING_SP,                    IPC_WORKING_POS             },      //  3
    [ 3u] = {IPC_DISCRETE_SWITCH_1,             IPC_DISCRETE_SWITCH_2       },      //  4
    [ 4u] = {IPC_DISCRETE_INPUT,                IPC_LUI_SIMULATION          },      //  5
    [ 5u] = {IPC_PRESSURE_ACTUATOR_A,           IPC_PRESSURE_ACTUATOR_B     },      //  6
    [ 6u] = {IPC_COMPLETE_STATUS_CUR_HI,        IPC_COMPLETE_STATUS_CUR_LO  },      //  7
    [ 7u] = {IPC_RAW_POSITION_SENSOR,           IPC_VARIABLE_ID_NOTUSED     },      //  8
    [ 8u] = {IPC_PRESSURE_SUPPLY,               IPC_COMPLETE_STATUS_CUR_HIHI},      //  9
    [ 9u] = {IPC_PRESSURE_ACTUATOR_A,           IPC_PRESSURE_ACTUATOR_B     },      // 10
    [10u] = {IPC_WORKING_SP,                    IPC_WORKING_POS             },      // 11
    [11u] = {IPC_DISCRETE_SWITCH_1,             IPC_DISCRETE_SWITCH_2       },      // 12
    [12u] = {IPC_IP_CURRENT,                    IPC_PRESSURE_PILOT          },      // 13
    [13u] = {IPC_PRESSURE_ACTUATOR_A,           IPC_PRESSURE_ACTUATOR_B     },      // 14
    [14u] = {IPC_COMPLETE_STATUS_HIST_HI,       IPC_COMPLETE_STATUS_HIST_LO },      // 15
    [15u] = {IPC_TEMPERATURE,                   IPC_COMPLETE_STATUS_HIST_HIHI }     // 16
};

//--------------------------------------------------------------------------------
// Code below handles the HART response Time outs for periodic messages

#define HART_RESPONSE_ABSENSE_TIMEOUT       (501u)

// Storage for the Time Stamp -- in millieseconds
static  u32 mn_HART_ReceiveTimeStamp        = 0u;

// Storage for the Time Stamp -- in millieseconds
static  bool_t  mn_HART_NeedToCheckTimeout  = false;

// Storage for the Time Stamp -- in millieseconds
static  u16 mn_HART_ResponseTimedOut    = 0;

#define RESPONSE_FAILURE_THRESHOLD 4

/** Function to latch the Time Stamp for the Response Receive
\param - No parameters
\return - Nothing
*/
void        mn_HART_ReceivedResponse(void)
{
    mn_HART_NeedToCheckTimeout = true;
    mn_HART_ReceiveTimeStamp = osif_get_time_in_ms();               // Latch the time
}

/** Function to detect if there was a Time Out on the HART response
\param - No parameters
\return -- Nothing
*/
void    mn_HART_VerifyResponseTO(void)
{
    u32     CurrentTime;
    u32     TimePassed;
    u32     TimeOutValue;

    if (mn_HART_NeedToCheckTimeout)
    {   // Need to cvheck the T/O on response

        CurrentTime = osif_get_time_in_ms();                            // Get current Time
        TimePassed  = CurrentTime - mn_HART_ReceiveTimeStamp;           // Calculate time from the Last Response

        TimeOutValue = HART_RESPONSE_ABSENSE_TIMEOUT;                   // TimeOut value
        if(TimePassed > TimeOutValue)
        {
            if(mn_HART_ResponseTimedOut < RESPONSE_FAILURE_THRESHOLD)
            {
                mn_HART_ResponseTimedOut++;
            }
            else
            {
                mn_HART_ResponseTimedOut = RESPONSE_FAILURE_THRESHOLD + NELEM(IPC_Dynamic_StateMachine);
                mn_HART_NeedToCheckTimeout = false;
            }
        }
        else
        {
            if(mn_HART_ResponseTimedOut != 0)
            {
                --mn_HART_ResponseTimedOut;
            }
        }
    }
}

/** Function to detect if there was a Time Out on the HART response
\param - No parameters
\return -- State of the Response Time Out: false - NO T/O; true - HART Response timed out
*/
bool_t  mn_HART_ResponseTimeOut(void)
{
    // -- Go OOS!
    // -- Update WRONG_CONFIGURATION
    // -- AI,DI channels shall receive BAD status

    return mn_HART_ResponseTimedOut >= RESPONSE_FAILURE_THRESHOLD + NELEM(IPC_Dynamic_StateMachine);
    /* EXPLANATION:
    The purpose of this diagnostic is to jam TB actual mode to OOS and status of dynamic
    parameters to BAD|OOS. It this already happened, mode and parameters will come out
    unjammed one at a time, as they arrive.
    */
}

//------------------------------------------------------------------------------------------
// Get the current Dynamic Exchange state
static USIGN8   GetStateNumber(void)
{
    USIGN8  Retval;

    if (IPCDynamicStateNumber > MAX_PARAMETER_CYCLE)
    {   // This is an error. We just fix it, should we do anything else?
        IPCDynamicStateNumber = FIRST_PARAMETER_CYCLE;
    }

    Retval = IPCDynamicStateNumber;

    return Retval;
}

// Set the NEXT state -- Dynamic Exchange state
static void NextStateNumber(void)
{
    IPCDynamicStateNumber++;
    if (IPCDynamicStateNumber > MAX_PARAMETER_CYCLE)
    {   // WRap the state machine for the next cycle
        IPCDynamicStateNumber = FIRST_PARAMETER_CYCLE;

        // This is done to inform the Alerts that the IPC state machine went
        // through one full cycle
        Alert_IPCStateMachineExecuted();
    }
}

// Retrieve the Variable ID 1 for the Dynamic Exchange state machine
static u16  GetVariableID1(USIGN8   ThisState)
{
#ifndef _lint //inexplicable lint bug
    CONST_ASSERT(sizeof(IPC_Variable_IDs_t)<=sizeof(u16));
#endif
    return (u16)IPC_Dynamic_StateMachine[ThisState].VarID1;
}

// Retrieve the Variable ID 2 for the Dynamic Exchange state machine
static u16  GetVariableID2(USIGN8   ThisState)
{
#ifndef _lint //inexplicable lint bug
    CONST_ASSERT(sizeof(IPC_Variable_IDs_t)<=sizeof(u16));
#endif
    return (u16)IPC_Dynamic_StateMachine[ThisState].VarID2;
}

// Handle Min and Max values
// Function returns true    if the MIN / MAX parameters WERE updated,
// Function returns false   if the MIN / MAX parameters WERE NOT updated,

bool_t Handle_MinMax(IN const FLOAT_S  *pfsCurrent,
                          OUT FLOAT    *ppfMaxValue,
                          OUT FLOAT    *ppfMinValue)
{
    bool_t    retval = false;

    if (IsQuality_ACCEPT(pfsCurrent->status))
    {   // Only update if the quality is GOOD!
        if (pfsCurrent->value > *ppfMaxValue)
        {   // Check MAX
            *ppfMaxValue = pfsCurrent->value;    // Update the MAX parameter
            retval = true;
        }

        if (pfsCurrent->value < *ppfMinValue)
        {   // Check MIN
            *ppfMinValue = pfsCurrent->value;    // Update the MIN parameter
            retval = true;
        }
        else
        {
            // Do nothing!
        }
    }

    return retval;
}

//-----------------------------------------------------------------------------
// Attributes of the Variables -- use for conversion
#define FLOAT_VAR                           (0u)
#define UINT32_VAR                          (1u)
#define SIGINT32_VAR                        (2u)

// This function returns the Variable format associated with the specific variable
// ID -- to be used for conversion
static USIGN16  GetVariableFormat(USIGN16   VarID)
{
    USIGN16 Retval;

    switch (VarID)
    {
        case IPC_DISCRETE_SWITCH_1 :
        case IPC_DISCRETE_SWITCH_2 :
        case IPC_COMPLETE_STATUS_CUR_HI :
        case IPC_COMPLETE_STATUS_CUR_LO :
        case IPC_COMPLETE_STATUS_CUR_HIHI :
        case IPC_COMPLETE_STATUS_HIST_HIHI :
        case IPC_COMPLETE_STATUS_HIST_HI :
        case IPC_COMPLETE_STATUS_HIST_LO :
        case IPC_CYCLE_CTR_A :
        case IPC_CYCLE_CTR_B :
        case IPC_DISCRETE_INPUT :
        case IPC_LUI_SIMULATION :
            Retval = UINT32_VAR;
            break;

        case IPC_RAW_POSITION_SENSOR :
        case IPC_IP_CURRENT :
            Retval = SIGINT32_VAR;
            break;

        default :
            Retval = FLOAT_VAR;
            break;
    }

    return Retval;
}

/** \brief returns the indicator that says what to do with the var status
if the FF uC is in OOS mode -- make it bad or keep it as is.
\return true: -- Modify it accordingly to the FF uC wishes; false: -- APP uC knows better, leave as is.
*/
static bool_t  NeedToChangeVarStatus(USIGN16   VarID)
{
    bool_t    Retval = true;

    switch (VarID)
    {
        case IPC_COMPLETE_STATUS_CUR_LO :
        case IPC_COMPLETE_STATUS_CUR_HI :
        case IPC_COMPLETE_STATUS_CUR_HIHI :
        case IPC_COMPLETE_STATUS_HIST_LO :
        case IPC_COMPLETE_STATUS_HIST_HI :
        case IPC_COMPLETE_STATUS_HIST_HIHI :
        case IPC_LUI_SIMULATION :
            Retval = false;
            break;

        case IPC_WORKING_SP :
        default :
            Retval = true;
            break;
    }

    return Retval;
}

static void ipc_ReadOneVar(T_FBIF_PTB *p_PTB, u16 VarID, u8 VarStatus, const void *pVar)
{
    USIGN8  VarQuality;
    FLOAT   VarValue = 0.0F;
    USIGN32 VarValueInt = 0;
    INT32   VarValueSignedInt = 0;

    if (NeedToChangeVarStatus(VarID))
    {   //if PTB is in OOS Mode all parameters are reported with bad status
        if ( ( p_PTB->mode_blk.actual & MODE_OS) != 0)
        {
            VarStatus &= LIMIT_MASK;                        // Preserve the LIMIT bits
            VarStatus |= SQ_BAD | SUB_OUT_OF_SERVICE;
        }
    }

    VarQuality = VarStatus & QUALITY_MASK;  // Separate the Parameter Quality

    switch (GetVariableFormat(VarID))
    {
        case FLOAT_VAR :                // FLOAT vars
            VarValue = util_GetFloat(pVar);
            break;

        case UINT32_VAR :               // INT vars
            VarValueInt = util_GetU32(pVar);
            break;

        case SIGINT32_VAR :             // SIGNED INT vars
            VarValueSignedInt = util_GetS32(pVar);
            break;

        default :                       // Nothing will be done here
            return;
    }

    switch (VarID)      // Process the FBlock parameters
    {
        case IPC_PRESSURE_SUPPLY :
            p_PTB->supply_pressure.status = VarStatus;
            if(SQ_BAD == VarStatus)
            {
                p_PTB->supply_pressure.value = 0.0f;
            }
            //if it is standard device type, return value 0.0 and
            //BAD of not connected status
            else if (a_false == ffcheck_GetAdvancedFlag())
            {
                p_PTB->supply_pressure.value = 0.0f;
                p_PTB->supply_pressure.status = SUB_NOT_CONNECTED;
            }
            else
            {
                p_PTB->supply_pressure.value  = VarValue;
            }
            //if the mode is LO,stop to save the extremes
            if (!(p_PTB->mode_blk.actual == MODE_LO))
            {
                (void)Handle_MinMax(&p_PTB->supply_pressure,
                                &p_PTB->pressure_extremes.supply_pressure_max,
                                &p_PTB->pressure_extremes.supply_pressure_min);
            }
            break;

        case IPC_PRESSURE_ATMOS :
            p_PTB->atmospheric_pressure.status = VarStatus;
            if(SQ_BAD == VarStatus)
            {
                p_PTB->atmospheric_pressure.value = 0.0f;
            }
            //if it is standard device type, return value 0.0 and
            //BAD of not connected status
            else if (a_false == ffcheck_GetAdvancedFlag())
            {
                p_PTB->atmospheric_pressure.value = 0.0f;
                p_PTB->atmospheric_pressure.status = SUB_NOT_CONNECTED;

            }
            else
            {
                p_PTB->atmospheric_pressure.value = VarValue;
            }
            break;

        case IPC_PRESSURE_ACTUATOR_A :
            p_PTB->actuator_a_pressure.status = VarStatus;
            if(SQ_BAD == VarStatus)
            {
                p_PTB->actuator_a_pressure.value = 0.0f;
            }
            //if it is standard device type, return value 0.0 and
            //BAD of not connected status
            else if (a_false == ffcheck_GetAdvancedFlag())
            {
                p_PTB->actuator_a_pressure.value = 0.0f;
                p_PTB->actuator_a_pressure.status = SUB_NOT_CONNECTED;
            }
            else
            {
                p_PTB->actuator_a_pressure.value  = VarValue;
            }
            //if the mode is LO,stop to save the extremes
            if (!(p_PTB->mode_blk.actual == MODE_LO))
            {
                (void)Handle_MinMax(&p_PTB->actuator_a_pressure,
                                &p_PTB->pressure_extremes.actuator_a_max,
                                &p_PTB->pressure_extremes.actuator_a_min);
            }
            break;

        case IPC_PRESSURE_ACTUATOR_B :
            p_PTB->actuator_b_pressure.status = VarStatus;
            if(SQ_BAD == VarStatus)
            {
                p_PTB->actuator_b_pressure.value = 0.0f;
            }
            //if it is standard device type, return value 0.0 and
            //BAD of not connected status
            else if (a_false == ffcheck_GetAdvancedFlag())
            {
                p_PTB->actuator_b_pressure.value = 0.0f;
                p_PTB->actuator_b_pressure.status = SUB_NOT_CONNECTED;
            }
            else
            {
                p_PTB->actuator_b_pressure.value  = VarValue;
            }
            //if the mode is LO,stop to save the extremes
            if (!(p_PTB->mode_blk.actual == MODE_LO))
            {
                (void)Handle_MinMax(&p_PTB->actuator_b_pressure,
                                &p_PTB->pressure_extremes.actuator_b_max,
                                &p_PTB->pressure_extremes.actuator_b_min);
            }
            break;

        case IPC_PRESSURE_PILOT :
            p_PTB->pilot_pressure.status = VarStatus;
            if(SQ_BAD == VarStatus)
            {
                p_PTB->pilot_pressure.value = 0.0f;
            }
            //if it is standard device type, return value 0.0 and
            //BAD of not connected status
            else if (a_false == ffcheck_GetAdvancedFlag())
            {
                p_PTB->pilot_pressure.value = 0.0f;
                p_PTB->pilot_pressure.status = SUB_NOT_CONNECTED;
            }
            else
            {
                p_PTB->pilot_pressure.value  = VarValue;
            }
            //if the mode is LO,stop to save the extremes
            if (!(p_PTB->mode_blk.actual == MODE_LO))
            {
                (void)Handle_MinMax(&p_PTB->pilot_pressure,
                                &p_PTB->pressure_extremes.pilot_max,
                                &p_PTB->pressure_extremes.pilot_min);
            }
            break;

        case IPC_IP_CURRENT :
            //if it is standard device type, return value 0.0 and
            //BAD of not connected status
            if (a_false == ffcheck_GetAdvancedFlag())
            {
                p_PTB->ip_drive_current.value = 0.0f;
                p_PTB->ip_drive_current.status = SUB_NOT_CONNECTED;
            }
            else
            {
                p_PTB->ip_drive_current.value  = IP_Conv_to_Percent(VarValueSignedInt);
                p_PTB->ip_drive_current.status = VarStatus;
            }
            (void)Handle_MinMax(&p_PTB->ip_drive_current,
                                &p_PTB->ip_current_extremes.ip_current_max,
                                &p_PTB->ip_current_extremes.ip_current_min);
            break;

        case IPC_TEMPERATURE :
            //if it is standard device type, return value 0.0 and
            //BAD of not connected status
            if (a_false == ffcheck_GetAdvancedFlag())
            {
                p_PTB->temperature.value = 0.0f;
                p_PTB->temperature.status = SUB_NOT_CONNECTED;
            }
            else
            {
                p_PTB->temperature.value  = VarValue;
                p_PTB->temperature.status = VarStatus;
            }
            (void)Handle_MinMax(&p_PTB->temperature,
                                &p_PTB->temperature_extremes.temperature_max,
                                &p_PTB->temperature_extremes.temperature_min);
            break;

        case IPC_WORKING_SP :
            p_PTB->working_sp.value  = VarValue;
            p_PTB->working_sp.status = VarStatus;
            (void)Handle_MinMax(&p_PTB->working_sp,
                                &p_PTB->position_extremes.working_sp_max,
                                &p_PTB->position_extremes.working_sp_min);
            break;

        case IPC_WORKING_POS :
            p_PTB->working_pos.value  = VarValue;
            p_PTB->working_pos.status = VarStatus;
            (void)Handle_MinMax(&p_PTB->working_pos,
                                &p_PTB->position_extremes.working_pos_max,
                                &p_PTB->position_extremes.working_pos_min);

            // Work with Working Position in TU
            PTB_ConvertWorkPos_InTravelUnits(p_PTB);
            break;

        case IPC_RAW_POSITION_SENSOR :
            p_PTB->raw_position = VarValueSignedInt;
            break;

        case IPC_DISCRETE_SWITCH_1 :
            p_PTB->discrete_output_1_state.value  = (USIGN8)VarValueInt;
            p_PTB->discrete_output_1_state.status = VarStatus;
            break;

        case IPC_DISCRETE_SWITCH_2 :
            p_PTB->discrete_output_2_state.value  = (USIGN8)VarValueInt;
            p_PTB->discrete_output_2_state.status = VarStatus;
            break;

        case IPC_DISCRETE_INPUT :
            // Check the DI status in the bitmap
            p_PTB->discrete_input.status = VarStatus;
            p_PTB->discrete_input.value  = ((VarValueInt & IPC_BIT_DI_STATE) != 0u) ? 1u : 0;

            // Write Lock does not have status, just value.
            // The vaule from the Remote WP Jumper is NOT directlt written into the PTB or
            // Resource blocks, instead the local "hardware" jumper is simulated.
            Remote_WP_Jumper_State = ((VarValueInt & IPC_BIT_WP_STATE) != 0u);
            break;

        case IPC_COMPLETE_STATUS_CUR_LO :
            if ((VarQuality != SQ_BAD) && (VarQuality != SQ_UNCERTAIN))
            {   // Status OK, store
#ifndef _lint //inexplicable lint bug
                CONST_ASSERT((COMPLETE_STATUS_BUFFER_SIZE * 2) == sizeof(p_PTB->complete_status));
#endif
                u8 *dst = (void *)&p_PTB->complete_status;
                mn_memcpy(&dst[COMPLETE_STATUS_CUR_LO_OFFSET],
                          (void *)&VarValueInt,
                          COMPLETE_STATUS_TRANSFER_BUFFER_SIZE);
            }
            break;

        case IPC_COMPLETE_STATUS_CUR_HI :
            if ((VarQuality != SQ_BAD) && (VarQuality != SQ_UNCERTAIN))
            {   // Status OK, store
#ifndef _lint //inexplicable lint bug
                CONST_ASSERT((COMPLETE_STATUS_BUFFER_SIZE * 2) == sizeof(p_PTB->complete_status));
#endif
                u8 *dst = (void *)&p_PTB->complete_status;
                mn_memcpy(&dst[COMPLETE_STATUS_CUR_HI_OFFSET],
                          (void *)&VarValueInt,
                          COMPLETE_STATUS_TRANSFER_BUFFER_SIZE);
            }
            break;

        case IPC_COMPLETE_STATUS_CUR_HIHI :
            if ((VarQuality != SQ_BAD) && (VarQuality != SQ_UNCERTAIN))
            {   // Status OK, store
#ifndef _lint //inexplicable lint bug
                CONST_ASSERT((COMPLETE_STATUS_BUFFER_SIZE * 2) == sizeof(p_PTB->complete_status));
#endif
                u8 *dst = (void *)&p_PTB->complete_status;
                mn_memcpy(&dst[COMPLETE_STATUS_CUR_HIHI_OFFSET],
                          (void *)&VarValueInt,
                          COMPLETE_STATUS_TRANSFER_BUFFER_SIZE);
            }
            break;

        case IPC_COMPLETE_STATUS_HIST_LO :
            if ((VarQuality != SQ_BAD) && (VarQuality != SQ_UNCERTAIN))
            {   // Status OK, store
                CONST_ASSERT((COMPLETE_STATUS_BUFFER_SIZE * 2) == sizeof(p_PTB->complete_status));
                u8 *dst = (void *)&p_PTB->complete_status;
                mn_memcpy(&dst[COMPLETE_STATUS_HIST_LO_OFFSET],
                          (void *)&VarValueInt,
                          COMPLETE_STATUS_TRANSFER_BUFFER_SIZE);
            }
            break;

        case IPC_COMPLETE_STATUS_HIST_HI :
            if ((VarQuality != SQ_BAD) && (VarQuality != SQ_UNCERTAIN))
            {   // Status OK, store
#ifndef _lint //inexplicable lint bug
                CONST_ASSERT((COMPLETE_STATUS_BUFFER_SIZE * 2) == sizeof(p_PTB->complete_status));
#endif
                u8 *dst = (void *)&p_PTB->complete_status;
                mn_memcpy(&dst[COMPLETE_STATUS_HIST_HI_OFFSET],
                          (void *)&VarValueInt,
                          COMPLETE_STATUS_TRANSFER_BUFFER_SIZE);
            }
            break;

        case IPC_COMPLETE_STATUS_HIST_HIHI :
            if ((VarQuality != SQ_BAD) && (VarQuality != SQ_UNCERTAIN))
            {   // Status OK, store
#ifndef _lint //inexplicable lint bug
                CONST_ASSERT((COMPLETE_STATUS_BUFFER_SIZE * 2) == sizeof(p_PTB->complete_status));
#endif
                u8 *dst = (void *)&p_PTB->complete_status;
                mn_memcpy(&dst[COMPLETE_STATUS_HIST_HIHI_OFFSET],
                          (void *)&VarValueInt,
                          COMPLETE_STATUS_TRANSFER_BUFFER_SIZE);
            }
            break;

    case IPC_LUI_SIMULATION:
        lui_Simulation = (VarValueInt == SIMULATION_ACTIVE);
        break;

        case IPC_VARIABLE_ID_NOTUSED :
        case IPC_VARID_TEST :
        default :                               // This should not be invoked at all!
            break;
    }
}

void mnipc_WriteVars(T_FBIF_PTB *p_PTB, u8 *send_buff, u8 *send_length)
{
    UNUSED_OK(p_PTB);
    u8    StateNumber;
    u16   VarID;                            //

    StateNumber = GetStateNumber();             // This state

    Req_IPCDynamicParametersExchange_t *s = (void *)send_buff;

    /* write I/P Operation stub value to HART */
    util_PutU8(s->IPCIPActionStatus[0], 0);
    util_PutU8(s->IPCIPAction[0], 0);
    //WRITE_HART_CMD( CMD_177_I_P_OPERATION_OFFSET, TempVal, USIGN16);

    /* write ID1 value to HART */
    VarID = GetVariableID1(StateNumber);
    util_PutU16(s->IPCVariableID1[0], VarID);
    //WRITE_HART_CMD( CMD_177_ID1_REQ_OFFSET, VarID, USIGN16);
    /* write ID2 value to HART */
    VarID = GetVariableID2(StateNumber);
    util_PutU16(s->IPCVariableID2[0], VarID);
    //WRITE_HART_CMD( CMD_177_ID2_REQ_OFFSET, VarID, USIGN16);

    *send_length = MN_OFFSETOF(Req_IPCDynamicParametersExchange_t, endmark);
}

void mnipc_WriteDOswitches(const T_FBIF_BLOCK_INSTANCE *p_block_instance, u8 *send_buff)
{
    // Prepare TempVal to write switches            T_FBIF_BLOCK_INSTANCE *  p_block_instance;
    u16   TempVal;                     // Placeholder
    u8 swvalue[DO_NUM_CHANNELS];
    Req_IPCDynamicParametersExchange_t *s = (void *)send_buff;

    TempVal = 0u;                           // Clear the value

    ffdosw_Runtime(p_block_instance, swvalue);

    if (swvalue[0u] == DSS_Active)
    {
        TempVal |= DO_1_IPC_STATE;          // Set the bit
    }

    if (swvalue[1u] == DSS_Active)
    {
        TempVal |= DO_2_IPC_STATE;          // Set the bit
    }

    /* write Buffer (DO switches) */
    util_PutU16(s->IPCUnused2byteBuffer[0], TempVal);
    //WRITE_HART_CMD( CMD_177_U16_BUFFER_OFFSET, TempVal, USIGN16);
}

void mnipc_ReadVars(T_FBIF_PTB *p_PTB, const u8 *receive_buff)
{
    u16 VarID;
    u8 VarStatus;

    const Rsp_IPCDynamicParametersExchange_t *d = (const void *)(receive_buff + 2); //skip HART response flags
    VarID = util_GetU16(d->IPCVariableID1[0]);
    VarStatus = util_GetU8(d->IPCVar1Status[0]);
    const void *pVar = d->IPCVar1Buffer[0];
    ipc_ReadOneVar(p_PTB, VarID, VarStatus, pVar);

    VarID = util_GetU16(d->IPCVariableID2[0]);
    VarStatus = util_GetU8(d->IPCVar2Status[0]);
    pVar = d->IPCVar2Buffer[0];
    ipc_ReadOneVar(p_PTB, VarID, VarStatus, pVar);

    NextStateNumber();                  // Read is done -- next command
}

