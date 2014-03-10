/*****************************************************************************
*                                                                            *
*                     SOFTING Industrial Automation GmbH                     *
*                          Richard-Reitzner-Allee 6                          *
*                                D-85540 Haar                                *
*                        Phone: ++49-89-4 56 56-0                            *
*                          Fax: ++49-89-4 56 56-3 99                         *
*                                                                            *
*                            SOFTING CONFIDENTIAL                            *
*                                                                            *
*                     Copyright (C) SOFTING IA GmbH 2012                     *
*                             All Rights Reserved                            *
*                                                                            *
* NOTICE: All information contained herein is, and remains the property of   *
*   SOFTING Industrial Automation GmbH and its suppliers, if any. The intel- *
*   lectual and technical concepts contained herein are proprietary to       *
*   SOFTING Industrial Automation GmbH and its suppliers and may be covered  *
*   by German and Foreign Patents, patents in process, and are protected by  *
*   trade secret or copyright law. Dissemination of this information or      *
*   reproduction of this material is strictly forbidden unless prior         *
*   written permission is obtained from SOFTING Industrial Automation GmbH.  *
******************************************************************************
******************************************************************************
*                                                                            *
* PROJECT_NAME             Softing FF/PA FD 2.42                             *
*                                                                            *
* VERSION                  FF - 2.42                                         *
*                          PA - 2.42 (beta)                                  *
*                                                                            *
* DATE                     16. April 2012                                    *
*                                                                            *
*****************************************************************************/


/* ===========================================================================

FILE_NAME          appl_ptb.c



FUNCTIONAL_MODULE_DESCRIPTION

  This module contains the Positioner transducer block and Interface functions
  to the HART Master.

  For each transducer block there must be the following interface functions:

  - Start_<transducer-block-type>
  - Execute_<transducer-block-type>
  - Read_handler_<transducer-block-type>
  - Write_handler_<transducer-block-type>

  As there are AI or AO blocks in the function block layer, the application must
  provide a function that checks the valid combinations of CHANNEL and XD_SCALE.
  In this implementation, the check routine is called 'Appl_check_channel_unit'.

=========================================================================== */
#include <stdlib.h>
#include <string.h>
#include <softing_base.h>
#define  MODULE_ID      (COMP_FAPP + MOD_APPL_TRN)


//#include <hw_cfg.h> not used but implementation relies on certain conf (e.g. HART to APP)
#include <hw_if.h>
#include <osif.h>
#include <fbif_dcl.h>
#include <appl_int.h>
#include <hm_tools.h>

#include <ffbl_res.h>
#include <ptb.h>

#include "hartfunc.h"
#include "hartdef.h"
#include "bufutils.h"
#include "mnhart2ff.h"
#include "mnassert.h"
#include "ipcdefs.h"

#include "lui_ipc_variables.h"
#include "mnctllimff.h"
#include "mncharactff.h"
#include "histogram.h"
#include "appl_alert.h"
#include "ctrl_param_set.h"
#include "ui_language.h"
#include "app_mode.h"
#include "mndoswff.h"
#include "pressure_range.h"
#include "mn_travel.h"
//#include "faultdef.h"
#include "mnfault2alert.h"
#include "mn_travel_accumulation.h"
#include "mn_cycle_counter_alert.h"
#include "mn_trends.h"
#include "mn_clear_status.h"
#include "mnfindstopsff.h"
#include "mn_actuator_3.h"
#include "mn_near_closed_alert.h"
#include "mn_travel_units.h"
#include "mn_working_times.h"
#include "mndiagprocff.h"
#include "mntbmspff.h"
#include "mn_xd_errors.h"
#include "mn_fstate.h"
#include "mn_errorbits.h"
#include "mn_ui_access_control.h"
#include "mnipcff.h"
#include "mn_supply_pressure_alert.h"
#include "mn_pressure_extremes.h"
#include "mn_ip_drive_current_alert.h"
#include "ffactoryuse.h"
#include "mn_temperature_alert.h"
#include "mn_remote_sensor.h"
#include "mn_checkprocess.h"
#include "mn_block_header.h"
#include "mn_deviation_alert.h"
#include "mn_advanced.h"
#include "mn_diagnostic_configuration.h"
//---------------------------------------------------------------------------------------------
// Verification of COMMON definition that exist in FF and AP code bases

// Block modes
CONST_ASSERT(MODE_OS    == IPC_MODE_OOS);
CONST_ASSERT(MODE_IMAN  == IPC_MODE_IMAN);
CONST_ASSERT(MODE_LO    == IPC_MODE_LOVERRIDE);
CONST_ASSERT(MODE_MAN   == IPC_MODE_MANUAL);
CONST_ASSERT(MODE_AUTO  == IPC_MODE_AUTO);
CONST_ASSERT(MODE_CAS   == IPC_MODE_CAS);
CONST_ASSERT(MODE_RCAS  == IPC_MODE_RCAS);
CONST_ASSERT(MODE_ROUT  == IPC_MODE_ROUT);

// Parameter Quality
CONST_ASSERT(QUALITY_MASK   == IPC_QUALITY_MASK);
CONST_ASSERT(LIMIT_MASK     == IPC_STATUS_LIMITED);
CONST_ASSERT(LIMIT_LOW      == IPC_STATUS_LIMITED_LOWER);
CONST_ASSERT(LIMIT_HIGH     == IPC_STATUS_LIMITED_UPPER);

CONST_ASSERT(SQ_BAD         == IPC_QUALITY_BAD);
CONST_ASSERT(SQ_UNCERTAIN   == IPC_QUALITY_UNCERTAIN);
CONST_ASSERT(SQ_GOOD_NC     == SQ_GOOD);
CONST_ASSERT(SQ_GOOD_CAS    == IPC_QUALITY_GOODCASCADE);
CONST_ASSERT(SQ_GOOD        == IPC_QUALITY_GOOD);

/* Substatus for all Qualities */
CONST_ASSERT(SUB_NON_SPECIFIC               == IPC_SUB_NON_SPECIFIC);

/* Substatus for BAD Quality */
CONST_ASSERT(SUB_CONFIGURATION_ERROR        == IPC_SUB_CONFIGURATION_ERROR);
CONST_ASSERT(SUB_NOT_CONNECTED              == IPC_SUB_NOT_CONNECTED);
CONST_ASSERT(SUB_DEVICE_FAILURE             == IPC_SUB_DEVICE_FAILURE);
CONST_ASSERT(SUB_SENSOR_FAILURE             == IPC_SUB_SENSOR_FAILURE);
CONST_ASSERT(SUB_NO_COMM_WITH_LUV           == IPC_SUB_NO_COMM_WITH_LUV);
CONST_ASSERT(SUB_NO_COMM_NO_LUV             == IPC_SUB_NO_COMM_NO_LUV);
CONST_ASSERT(SUB_OUT_OF_SERVICE             == IPC_SUB_OUT_OF_SERVICE);
CONST_ASSERT(SUB_BAD_TB_IN_MANUAL           == IPC_SUB_BAD_TB_IN_MANUAL);

/* Substatus for UNCERTAIN Quality */
CONST_ASSERT(SUB_LUV                        == IPC_SUB_LUV);
CONST_ASSERT(SUB_SUBSTITUTE                 == IPC_SUB_SUBSTITUTE);
CONST_ASSERT(SUB_INITIAL_VALUE              == IPC_SUB_INITIAL_VALUE);
CONST_ASSERT(SUB_SENSOR_CONV_NOT_ACCURATE   == IPC_SUB_SENSOR_CONV_NOT_ACCURATE);
CONST_ASSERT(SUB_EU_RANGE_VIOLATION         == IPC_SUB_EU_RANGE_VIOLATION);
CONST_ASSERT(SUB_SUB_NORMAL                 == IPC_SUB_SUB_NORMAL);
CONST_ASSERT(SUB_UNCERT_TB_IN_MANUAL        == IPC_SUB_UNCERT_TB_IN_MANUAL);

/* Substatus for GOOD (Non-cascade) Quality */
CONST_ASSERT(SUB_ACTIVE_BLOCK_ALARM         == IPC_SUB_ACTIVE_BLOCK_ALARM);
CONST_ASSERT(SUB_ACTIVE_ADVISORY_ALARM      == IPC_SUB_ACTIVE_ADVISORY_ALARM);
CONST_ASSERT(SUB_ACTIVE_CRITICAL_ALARM      == IPC_SUB_ACTIVE_CRITICAL_ALARM);
CONST_ASSERT(SUB_UNACK_BLOCK_ALARM          == IPC_SUB_UNACK_BLOCK_ALARM);
CONST_ASSERT(SUB_UNACK_ADVISORY_ALARM       == IPC_SUB_UNACK_ADVISORY_ALARM);
CONST_ASSERT(SUB_UNACK_CRITICAL_ALARM       == IPC_SUB_UNACK_CRITICAL_ALARM);

/* Substatus for GOOD (Cascade) Quality */
CONST_ASSERT(SUB_IA                         == IPC_SUB_IA);
CONST_ASSERT(SUB_IR                         == IPC_SUB_IR);
CONST_ASSERT(SUB_NI                         == IPC_SUB_NI);
CONST_ASSERT(SUB_NS                         == IPC_SUB_NS);
CONST_ASSERT(SUB_DNS                        == IPC_SUB_DNS);
CONST_ASSERT(SUB_LO                         == IPC_SUB_LO);
CONST_ASSERT(SUB_FSA                        == IPC_SUB_FSA);
CONST_ASSERT(SUB_IFS                        == IPC_SUB_IFS);

//---------------------------------------------------------------------------------------------

LOCAL_DEFINES


  #define MAX_RCV_BUF_SIZE                        100
  #define MAX_SND_BUF_SIZE                        100

  #define MODE_TRN_VALID                         (MODE_OS | MODE_AUTO | MODE_MAN) //permitted modes for PTB

#define E_UNIT_PERCENT                          1342 /* Percent */
#define E_UNIT_PSE                              1141 //psi
#define E_UNIT_BAR                              1137 //bar
#define E_UNIT_KPA                              1133 //kPa
#define E_UNIT_CGRAD                            1001 //degrees Celsius
#define E_UNIT_MA                               1211 //mA


  /* Definitions for FF diagnosis numbers */
//  #define FF_DIAG_NUMBER_OUT_FAILURE_DETECTED     27
//  #define FF_DIAG_NUMBER_BLOCK_CFG_ERROR          25
//  #define FF_DIAG_NUMBER_OTHER                    24
//  #define FF_DIAG_NUMBER_STATIC_DATA              29
//  #define FF_DIAG_NUMBER_NV_DATA                  30

  /* XD_ERROR Enumeration */
//  #define XD_ERROR_CONFIGURATION_ERROR            19

  /* Specify HART cmds to be executed -------------------------------------------------- */
//  #define CMD_0_INITIALIZE_HART_COMM              0
  #define CMD_177_READ_PROCESS_PARAMETERS         177

LOCAL_TYPEDEFS
#if 0 //AK: not used
typedef struct
{
    F_BLOCK            blk_data;
    USIGN16            st_rev;
    OSTRING            tag_desc[32];
    USIGN16            strategy;
    USIGN8             alert_key;
    MODE               mode_blk;
    BSTRING            block_err;
    EVENT              update_evt;
    ALARM_DISCRETE     block_alm;
} T_APPL_TB;
#endif //0

FUNCTION_DECLARATIONS

  /* Global Functions called by HART Master ------------------------------- */

static void Appl_handle_HART_cmd
    (
      IN USIGN8 cmd,
      IN USIGN8 access,
      IN USIGN8 *send_buff,
      IN USIGN8 *send_length,
      IN USIGN8 *receive_buff,
      IN  USIGN16 receive_len
    );

  FUNCTION GLOBAL VOID Appl_set_HART_status
    (
      IN  USIGN8  status
    );

  FUNCTION GLOBAL BOOL Appl_check_restart_flag (VOID);


  FUNCTION GLOBAL VOID Appl_reset_restart_flag (VOID);


  /* Local functions ------------------------------------------------------ */
#if 0
  FUNCTION LOCAL VOID appl_trn_start_tb(INOUT T_FBIF_BLOCK_INSTANCE *    p_block_instance);
#endif
#if 0
  FUNCTION LOCAL VOID appl_trn_update_mode (T_FBIF_BLOCK_INSTANCE *);
#endif

#if 0
  FUNCTION LOCAL VOID appl_trn_set_xd_error (const T_FBIF_BLOCK_DESCR *, USIGN8);
#endif


LOCAL_DATA

  NO_INIT USIGN8 comm_state; /* variable used for HART communication state */


  /* Cyclic commands */
  // Invoke CMD 177 4 times -- the CMD 177 will be invoked several times
  // Internally this command will read different Process parameters, this
  // is controlled by the CMD 177 parameter state machine.
  const USIGN8 hart_cycl[] = {  CMD_177_READ_PROCESS_PARAMETERS,
                                CMD_177_READ_PROCESS_PARAMETERS,
                                CMD_177_READ_PROCESS_PARAMETERS,
                                CMD_177_READ_PROCESS_PARAMETERS
                             };

  /* Regular commands */
  const USIGN8 hart_reg[] =  {
                               //CMD_216_READ_CONTROL_PARAMETERS,
                               //CMD_249_READ_MODE,  //APP MODE has been handled by Read_handler_PTB()
                               CMD_186_WRITE_SHORT_VARIABLES,
                               CMD_187_WRITE_ARRAY,
                               CMD_170_176_READ_UNIT
                             };

  #include <da_fdev.h>                           /*lint !e961 DATA SEGMENT DEFINITION */

/* We are not shy to say that HART_data_base is pre-inititialized and never
  changed again, i.e. const. That takes care of the order of initialization
  and, incidentally, makes Appl_get_HART_database() extraneous so it is removed
*/
const HART_DB HART_data_base =
{
    .cyc_cmd                  = hart_cycl,
    .regular_cmd              = hart_reg,
    .cyc_cmd_list_length      = sizeof (hart_cycl),
    .regular_cmd_list_length  = sizeof (hart_reg),
    .cycl_min_period          = 0,                  /* high prior (cyclic) command list is executed as fast as possible       */
    .startup_timeout          = 500,                /* IF 0: the HART Master never declares a device as not available         */
    .keep_regular             = TRUE,               /* low prior (regular) command list is further executed after init phase  */
};

  NO_INIT STATIC USIGN8 rcv_buf[MAX_RCV_BUF_SIZE];
  NO_INIT STATIC USIGN8 snd_buf[MAX_SND_BUF_SIZE];

  #include <da_def.h>                            /*lint !e961 DEFAULT DATA SEGMENT */

//
// This structure is used for convertsion of WorkingPosition to WorkingPosition in Travel Units
typedef struct
{
    FLOAT       Conversion_Value;                   // Conversion coeeficient, TRAVEL_RANGE / 100%
    FLOAT_S     WorkingPosition_InTravelUnits;      // self explanatory

} PTB_WorkingPositionConversion_t;

static PTB_WorkingPositionConversion_t PTB_WorkingPositionConversion;




/***************************************************************************************/

FUNCTION VOID PTB_ConvertWorkPos_InTravelUnits(const T_FBIF_PTB *p_PTB)
{
    // Work with Working Position in TU
    osif_disable_all_tasks();
        PTB_WorkingPositionConversion.WorkingPosition_InTravelUnits.status = p_PTB->working_pos.status;
        PTB_WorkingPositionConversion.WorkingPosition_InTravelUnits.value  = p_PTB->working_pos.value * PTB_WorkingPositionConversion.Conversion_Value;
    osif_enable_all_tasks();
}

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
  This function is used to calculate teh conversion coefficient.

PARAMETERS:
  ptb block instance

RETURN_VALUES:
    Nothing
----------------------------------------------------------------------------*/
FUNCTION LOCAL  VOID    PTB_CalculateTravelConversion
  (
      const T_FBIF_BLOCK_INSTANCE *p_block_instance
  )

{
LOCAL_VARIABLES
    T_FBIF_PTB  *                   p_PTB;
    FLOAT                           TempConversion;

FUNCTION_BODY
    p_PTB = p_block_instance->p_block_desc->p_block;

    osif_disable_all_tasks();
        // perform the calculation
        TempConversion = p_PTB->travel.component_0 / ONE_HUNDRED_PERCENT_FLOAT;

        // Store conversion value
        PTB_WorkingPositionConversion.Conversion_Value = TempConversion;

        // Work with Working Position in TU
        PTB_ConvertWorkPos_InTravelUnits(p_PTB);
    osif_enable_all_tasks();
}

/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
  Returns pointer to FLOAT_S parameter -- represnts Working Position in Engineering Units
  defined by the Travel parameter.

  NO ACTUAL CALCULATIONS ARE PERFORMED IN THIS FUNCTION.

PARAMETERS:
  p_block_instance         Pointer to the block to be started

RETURN_VALUES:
  Pointer to FLOAT_S structure containing WorkingPositino In Travel Units and its status.

------------------------------------------------------------------------------*/
FUNCTION PUBLIC FLOAT_S *TB_Get_WorkingPos_InTravelUnits
  (
    const T_FBIF_BLOCK_INSTANCE *    p_block_instance
  )
{
    UNUSED_OK(p_block_instance);

    return &PTB_WorkingPositionConversion.WorkingPosition_InTravelUnits;
}

/***************************************************************************************/
/***************************************************************************************/

FUNCTION PUBLIC VOID appl_trn_init (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:

  This function is used to initialze the variables which are local to this
  module. The function is called by the function appl_init() in module
  appl_if.c.

PARAMETERS:
  None

RETURN_VALUES:
  None
----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  /* Initialize/Setup HART Data Base used to be here; now it is const -- */

  hm_call_appl_funct.a_of_handle_HART_cmd     = Appl_handle_HART_cmd;
  hm_call_appl_funct.a_of_get_HART_database   = NULL;
  hm_call_appl_funct.a_of_set_HART_status     = Appl_set_HART_status;
  /* If the FBK2/HW serial number shall/can be used as SN within Device ID a customized function has to be defined here to handle the serial
     number received from connected HART slave. */

  // hm_call_appl_funct.a_of_set_dev_id          = NULL;
  hm_call_appl_funct.a_of_set_dev_id          = Appl_UpdateSerialNumber;

  hm_call_appl_funct.a_of_check_restart_flag  = Appl_check_restart_flag;
  hm_call_appl_funct.a_of_reset_restart_flag  = Appl_reset_restart_flag;

  comm_state = NO_HART_COMM;

}


FUNCTION GLOBAL T_CHAN_UNIT_CHECK Appl_check_channel_unit
  (
    IN USIGN16     channel,
    IN SCALE *     p_xd_scale
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
  This function is called each time the CHANNEL or XD_SCALE parameter of an
  AI or AO block should be written.

  It is assumed that each device has a set of supported engineering units and
  a subset of the supported engineering units is valid for each channel. This
  function is meant to check the combination of channel and engineering unit.
  It is structured into two sections:

  a) Check if a supported engineering unit should be written
  b) Check if the engineering unit is valid for the selected channel

  If the result of the checks indicate that there is a valid combination of
  channel and engineering unit, the application must store the engineering unit
  code. All process values that are transfered by following execute transducer
  function calls are encoded in the engineering unit that was checked in the most
  recent call of this check function.

PARAMETERS:
  channel                   The channel that sould be combined with an
                            engineering unit
  p_xd_scale                Pointer to the engineering unit that should be
                            combined with a channel

RETURN_VALUES:
  CHANNEL_UNIT_OK           The combination of channel and engineering unit
                            is supported
  INVALID_ENGINEERING_UNIT  The engineering unit is not supported
  INVALID_CHANNEL_UNIT      The combination of channel and engineering unit
                            is not supported
----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

    static BOOL aofb_start = TRUE;
    T_CHAN_UNIT_CHECK chanUnitCheck;

FUNCTION_BODY
    // Check if the engineering unit is valid for the selected channel/
    chanUnitCheck = CHANNEL_UNIT_OK; //good if not found bad later in the code

    switch (channel)
    {
        case CH_SUPPLY_PRESSURE:
        case CH_ACTUATOR_PRESSURE_1:
        case CH_ACTUATOR_PRESSURE_2:
        case CH_PILOT_PRESSURE:
            if ( !( (p_xd_scale->units == E_UNIT_PSE) ||
                    (p_xd_scale->units == E_UNIT_BAR) ||
                    (p_xd_scale->units == E_UNIT_KPA) ) )
            {
                chanUnitCheck = INVALID_ENGINEERING_UNIT;
            }
            break;

        case CH_TEMPERATURE:
            if ( p_xd_scale->units != E_UNIT_CGRAD )
            {
                chanUnitCheck = INVALID_ENGINEERING_UNIT;
            }
            break;

        case CH_IP_CURRENT:
            if ( p_xd_scale->units != E_UNIT_MA )
            {
                chanUnitCheck = INVALID_ENGINEERING_UNIT;
            }
            break;

        case CH_FINAL_VALUE:
        case CH_FINAL_POSITION_VALUE:
        case CH_WORKING_SP:
        case CH_WORKING_POSITION:
            if ( p_xd_scale->units != E_UNIT_PERCENT )
            {
                chanUnitCheck = INVALID_ENGINEERING_UNIT;
            }
            break;

        case SETPOINT_AOFB_CHANNEL:
            if (p_xd_scale->units != E_UNIT_PERCENT)
            {
                chanUnitCheck = INVALID_ENGINEERING_UNIT;
            }
            else if (aofb_start == FALSE)
            {
                /* update FINAL_VALUE_RANGE with XD_SCALE parameter value */
                T_FBS_WRITE_PARAM_LOCAL p_write_loc;

                T_FBIF_BLOCK_INSTANCE *p_block_instance = fbs_get_block_inst(ID_PTB_1);
                _ASSERT(p_block_instance != NULL);

                p_write_loc.rel_idx = REL_IDX_PTB_FINAL_VALUE_RANGE;
                p_write_loc.subindex = 0;
                p_write_loc.length   = sizeof(SCALE) - 1;
                p_write_loc.source   = (USIGN8*) (p_xd_scale);
                fbs_write_param_loc( p_block_instance, &p_write_loc );
            }
            else   /* aofb_start == TRUE */
            {
                aofb_start = FALSE;
            }
            break;

        default:
            break;
    } /* end switch */

    return chanUnitCheck;
}



  /*************************************************************************************/
  /*************************************************************************************/
  /*                                                                                   */
  /*                             Interface Functions of a                              */
  /*                                                                                   */
  /*                         T R A N S D U C E R    B L O C K                          */
  /*                                                                                   */
  /*************************************************************************************/
  /*************************************************************************************/

/* REVIEW ITEM: Only parameters of FLOAT_S or DISCRETE_S can go into
this table
*/
static const USIGN8 dyn_param_list[] = //type matches Softing's
{
    REL_IDX_PTB_FINAL_POSITION_VALUE,
    REL_IDX_PTB_WORKING_SP,
    REL_IDX_PTB_WORKING_POS,
    REL_IDX_PTB_SUPPLY_PRESSURE,
    REL_IDX_PTB_ACTUATOR_A_PRESSURE,
    REL_IDX_PTB_ACTUATOR_B_PRESSURE,
    REL_IDX_PTB_ATMOSPHERIC_PRESSURE,
    REL_IDX_PTB_PILOT_PRESSURE,
    REL_IDX_PTB_TEMPERATURE,
    REL_IDX_PTB_IP_DRIVE_CURRENT,
    REL_IDX_PTB_FINAL_POSITION_VALUE_D,
    REL_IDX_PTB_FINAL_POSITION_VALUE_DINT,
    REL_IDX_PTB_DISCRETE_OUTPUT_1_STATE,
    REL_IDX_PTB_DISCRETE_OUTPUT_2_STATE,
    REL_IDX_PTB_DISCRETE_INPUT,
    REL_IDX_PTB_ACTUAL_POSITION,
};

static void ptb_JamDynParamStatus(u8 status)
{
    T_FBIF_BLOCK_INSTANCE *p_block_instance = fbs_get_block_inst(ID_PTB_1);
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;

    for(u8_least i=0; i<NELEM(dyn_param_list); i++)
    {
        USIGN8 idx = dyn_param_list[i];
        switch(ptb_param_dir[idx].par_size)
        {
            case sizeof(FLOAT_S):
            {
                FLOAT_S *dst = (void *)((u8 *)p_PTB + ptb_param_dir[idx].par_offset);
                dst->status = status;
                break;
            }
            case sizeof(DISCRETE_S):
            {
                DISCRETE_S *dst = (void *)((u8 *)p_PTB + ptb_param_dir[idx].par_offset);
                dst->status = status;
                break;
            }
            default:
                //It is a misconfigured index table
                break;
        }
    }
}


FUNCTION GLOBAL VOID Start_PTB
  (
    T_FBIF_BLOCK_INSTANCE *    p_block_instance
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
  This function is called during the device startup phase and when the
  'Restart with defaults' is executed. It should perform all AITB-specific
  actions that are necessary if the device is started or restarted with defaults.

  If the block starts with actual mode Out-of-Service, the update events should
  be disabled

PARAMETERS:
  p_block_instance         Pointer to the block to be started

RETURN_VALUES:
  none
----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  T_FBIF_PTB *       p_PTB;
  T_APPL_TB_DATA *  p_PTB_data;

FUNCTION_BODY

  /* Allocate memory for the local PTB data ------------------------------- */
  if (p_block_instance->p_block_data == NULL)
  {
    p_block_instance->p_block_data = osif_malloc (sizeof(T_APPL_TB_DATA));
  }

  _ASSERT (p_block_instance->p_block_data != NULL);

  p_PTB      = p_block_instance->p_block_desc->p_block;
  p_PTB_data = p_block_instance->p_block_data;

    mnipc_InitSpAndMode(p_PTB);

  /* Initialize variables for Softing's transducer block example ---------- */
  p_PTB_data->old_block_alarm_bits = 0;
  p_PTB_data->rect_period          = 0;

  /* Disable update events ------------------------------------------------ */
  ffbl_disable_update_events (&p_PTB->update_evt);

  /* Control HART schedule according to block mode - NOT!------------------------ */
  HART_set_active (ACTIVATE, ACTIVATE);

  u8 status;
  if ( (p_PTB->mode_blk.target & MODE_OS) != 0)
  {
      status = SQ_BAD | SUB_OUT_OF_SERVICE;
  }
  else
  {
      status = SQ_UNCERTAIN | SUB_INITIAL_VALUE;
  }
  ptb_JamDynParamStatus(status);


#if 0
  if ((p_PTB->mode_blk.target & MODE_MAN) != 0u)
  {     // Starting in MANUAL mode! Force the SP to Act POS
      PTB_Force_SP_to_ActPOS = TRUE;
  }
#endif

  // Need to process the XD_FSTATE here.
  // If XD_FSTATE is not configured, need to set the flag above to TRUE

  PTB_CalculateTravelConversion(p_block_instance);

  // AP ----------------------------------------------------------
  Alert_InitAlertLog(p_block_instance);

  InitialWorkingTimes(p_block_instance);

  return;
}



bool_t tb_HasRun = false;


FUNCTION GLOBAL VOID Background_PTB
   (
     T_FBIF_BLOCK_INSTANCE *    p_block_instance
   )


/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:

  The function is cyclically called by the background task and updates
  the mode of the transducer block.

PARAMETERS:

  p_block_instance        Pointer to the transducer block to be executed.

RETURN_VALUES:
  none
----------------------------------------------------------------------------*/
{
    static const T_FBIF_BLOCK_DESCR *   p_block_desc = NULL;

    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;
    p_block_desc = fbs_get_block_inst (ID_PTB_1)->p_block_desc;

    UNUSED_OK(p_block_desc);

#if 0
    /* set xd_error */
    if (appl_get_comm_state() == DEV_COMM_NOT_OK)
    {
        appl_trn_set_xd_error(p_block_desc, XD_ERROR_I_O_FAILURE); /* I/O Failure */
    }
    else if ( flag_data_integrity == TRUE)
    {
        appl_trn_set_xd_error(p_block_desc, XD_ERROR_DATA_INTEGRITY_ERROR); /* Data Integrity Error */
    }
    else if ( flag_config_err == TRUE)
    {
        appl_trn_set_xd_error(p_block_desc, XD_ERROR_CONFIGURATION_ERROR); /* Configuration error */
    }
    else if ( flag_other == TRUE)
    {
        appl_trn_set_xd_error(p_block_desc, XD_ERROR_CALIBRATION_ERROR); /* Calibration error */
    }
    else
    {
        appl_trn_set_xd_error(p_block_desc, XD_ERROR_NO_ERROR); /* No Error */
    }
#endif

    mnptb_ProcErrors(p_block_instance); //A good idea to have it before DOSW

    ffdosw_TbBackgroundPatrol();

    //---------------------------------------------------------------------
    // This section of the code is invoked peridically
    // Call the MN code to verify that the Responses did not overfill teh TO

    mn_HART_VerifyResponseTO();

    bool_t rsp_timeout = mn_HART_ResponseTimeOut();

    u8 dyn_status = SQ_GOOD;
    osif_disable_all_tasks();
    /* set process values in case that PTB is in out of service;
     rsp_timeout takes precedence*/
    if ( (p_PTB->mode_blk.actual & MODE_OS) != 0 )
    {
        dyn_status = SQ_BAD | SUB_OUT_OF_SERVICE;
    }
    if (rsp_timeout)
    {   // OK, Bad thing happened. HART Response timed out
        // Mark some parameters statuses as BAD
        dyn_status = SQ_BAD | SUB_DEVICE_FAILURE;

        // Set OOS mode!
        //DON'T! p_PTB->mode_blk.target = MODE_OS;
        p_PTB->mode_blk.actual = MODE_OS;
        /* EXPLANATION:
        Normally, .actual comes from APP, but we lost communications.
        When IPC is restored, the mode will be repaired, with dyn param status
        restored on arrival.
        */
    }
    osif_enable_all_tasks();

    mn_ErrorBits_Set(ERRB_HART_RESPONSE_TIMEOUT, rsp_timeout);

    if(dyn_status != SQ_GOOD)
    {
        ptb_JamDynParamStatus(dyn_status);
    }

    mn_ErrorBits_Process(p_block_instance);

    //---------------------------------------------------------------------

    ff_fstate_Background(p_block_instance);

    //if the device is advanced type, update the trend and hisogram.
    if (a_true == ffcheck_GetAdvancedFlag())
    {
        UpdateHistogram(p_block_instance);
        UpdateTrends(p_block_instance);
    }
    UpdateWorkingTimes(p_block_instance);
	mn_ExpireAssert();

    tb_HasRun = true;
}


FUNCTION GLOBAL USIGN16 Read_handler_PTB
  (
    T_FBIF_BLOCK_INSTANCE *     p_block_instance,
    T_FBIF_READ_DATA *          p_read
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:

  This function is called when a AITB parameter should be read.

  It should perform two actions
  a) Check if the read access is possible.
    (There may be block-specific reasons to deny read access temporary.)

  b) Update the parameter to be read.
     (There may be dynamic parameters that are not updated permanently but
     only if there is a read access to these parameters.)


PARAMETERS:
  p_block_instance      Pointer to the block which contains the parameter to
                        be read

  p_read                Pointer to a read description block. Components of
                        read description block:
    rel_idx               Relative index of the parameter to be read
    subindex              Subindex within the parameter to be read
                            (Subindex 0 indicates that the whole
                            parameter is read)


RETURN_VALUES:
  E_OK                  Read access is approved (parameter value is up-to-date)
  E_PENDING_RES         Response is generated later
  E_FB_TEMP_ERROR       Read access is not allowed at the moment

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
    fferr_t fferr = E_OK;
    //T_FBIF_PTB      *p_PTB = NULL;
    //INT8            i;

FUNCTION_BODY

    switch(p_read->rel_idx)
    {
        case REL_IDX_PTB_CHAR_SELECTION:
            fferr = ffcharact_ReadSelection(p_block_instance, p_read, snd_buf, rcv_buf);
            break;

        case REL_IDX_PTB_CURRENT_CHAR_POINTS:
            fferr = ffcharact_ReadCharacterizationCurve(p_block_instance, p_read, snd_buf, rcv_buf);
            break;

        case REL_IDX_PTB_OPEN_STOP_ADJUSTMENT:
            fferr = ffposcal_ReadOpenStopAdjustment(p_block_instance, p_read, snd_buf, rcv_buf);
            break;

        case REL_IDX_PTB_TRAVEL_CALIBRATION:
            fferr = ffposcal_ReadFindStopsInfo(p_block_instance, p_read, snd_buf, rcv_buf);
            break;

        case REL_IDX_PTB_FIND_STOPS:
            fferr = ffposcal_ReadFindStops(p_block_instance, p_read, snd_buf, rcv_buf);
            break;

        case REL_IDX_PTB_OFFLINE_DIAGNOSTIC:
            fferr = ffdiag_ReadDiagnosticCommand(p_block_instance, p_read, snd_buf, rcv_buf);
            break;

        case REL_IDX_PTB_DIAGNOSTIC_DATA:
            fferr = ffdiag_ReadDiagnosticData(p_block_instance, p_read, snd_buf, rcv_buf);
            break;

        case REL_IDX_PTB_FINAL_VALUE_CUTOFF_LO:
        case REL_IDX_PTB_FINAL_VALUE_CUTOFF_HI:
        case REL_IDX_PTB_POSITION_LIMITS:
            fferr = ffctllim_ReadControlLimits(p_block_instance, p_read, snd_buf, rcv_buf);
            break;

        case REL_IDX_PTB_ACTIVE_CONTROL_SET:
            fferr = ctrl_param_ReadActiveCtrlSet(p_block_instance, p_read, snd_buf, rcv_buf);
            if(E_OK == fferr)
            {
                fferr = ctrl_param_ReadCtrlSetParam(p_block_instance, REL_IDX_PTB_ACTIVE_CONTROL_SET, snd_buf, rcv_buf);
            }
            break;
#if 0
        case REL_IDX_PTB_CUSTOM_CONTROL_SET:
            fferr = ctrl_param_ReadCtrlSetParam(p_block_instance, REL_IDX_PTB_CUSTOM_CONTROL_SET, snd_buf, rcv_buf);
            break;
#endif
        case REL_IDX_PTB_UI_LANGUAGE:
            fferr = ReadUILanguage(p_block_instance, snd_buf, rcv_buf);
            break;

        case REL_IDX_PTB_APP_MODE:
            fferr = ReadAPPMode(p_block_instance, snd_buf, rcv_buf);
            break;

        case REL_IDX_PTB_DISCRETE_SWITCH_1_CONF:
        case REL_IDX_PTB_DISCRETE_SWITCH_2_CONF:
            fferr = ffdosw_ReadConfiguration(p_block_instance, p_read, snd_buf, rcv_buf);
            break;

        case REL_IDX_PTB_PRESSURE_RANGE:
            fferr = ReadPressureRange(p_block_instance, snd_buf, rcv_buf);
            break;
        case REL_IDX_PTB_ACTUATOR_3:
            fferr = ReadActuator3(p_block_instance, snd_buf, rcv_buf);
            break;
        case REL_IDX_PTB_UI_ACCESS_CONTROL:
            fferr = ReadUIAccessControl(p_block_instance, snd_buf, rcv_buf);
            break;

        case REL_IDX_PTB_CUSTOM_CHAR: //default handling OK
        case REL_IDX_PTB_CUSTOM_CHAR_POINTS: //default handling OK
            break;


        case REL_IDX_PTB_ALERT_LOG :
            {
                AlertBufferControl_t    Action = ALERT_NO_ACTION;

                if ((p_read->subindex == 0u) || (p_read->subindex == 1u))
                {
                    Action = ALERT_AUTO_INC;
                }

                fferr = Alert_ReadAlertLogEntry(p_block_instance, Action);
            }
            break;

        case REL_IDX_PTB_FACTORY_USE_1:
        case REL_IDX_PTB_FACTORY_USE_2:
            fferr = ffact_ReadFactoryUse1or2(p_block_instance, snd_buf, rcv_buf);
            break;

        case REL_IDX_PTB_AUTOTUNE:
            fferr = ctrl_param_ReadAutotuneParam(p_block_instance, snd_buf, rcv_buf);
            break;

        case  REL_IDX_PTB_XD_ERROR_POS:
            fferr = UpdateXDErrorPos(p_block_instance);
            break;

        case  REL_IDX_PTB_BLOCK_ERR_DESC_1:
            fferr = Update_FIND_STOPS_Error_bit(p_block_instance);
            break;

        case REL_IDX_PTB_XD_ERROR_PRESSURE:
            fferr = UpdateXDErrorPressure(p_block_instance);
            break;

        case REL_IDX_PTB_XD_ERROR_TEMPERATURE:
            fferr = UpdateXDErrorTemperature(p_block_instance);
            break;
        case REL_IDX_PTB_POSITION_HISTOGRAM:
            fferr = OnReadPositionHistogram(p_block_instance);
            break;
        case REL_IDX_PTB_ACCESSORY:
            fferr = ffrps_ReadAccessory(p_block_instance, snd_buf, rcv_buf);
            break;

        case REL_IDX_PTB_CHECK_PROCESS:
            fferr = chkpro_CheckProcess(p_block_instance, p_read, snd_buf, rcv_buf);
            break;

        default:
            break;
    }

    return fferr;
}


/**
\brief  This function is called if is called when a TB parameter should be written.

  The function is structured into three parts
  a) Check if the new value is correct. At least the parameters ALERT_KEY and
     MODE_BLK should be checked
  b) Write the parameter by means of the FBL write function
  c) Perform block-specific actions that depend on the new value

-- AK: Masoneilan logic update --
Writing parameters (including commands) owned by APP is completely and entirely
delegated to APP for error/conditions checking and execution.
For them, calling the FBL write function has a single purpose of incrementing ST_REV
[ST_REV *could* be done differently (see TFS:15400) but carries a risk of breaking
Softing code using ST_REV]
It would therefore be possible not to populate TB parameters in the individual
handler functions. However, this is discouraged at this point, in case ST_REV
strategy changes in the future
--
Softing's intent is that each REL_IDX is handled twice:
1. To check conditions/parameter values before writing to block params
2. To perform additional actions as needed after the write to block params
Unfortunately, it is
1. Inherently non-atomic w.r.t. conditions and therefore not robust
2. Incompatible with APP-owned parameters: they are checked/written in APP, so
writing "by means of the FBL write function", if fails, produces an error OK, but
the parameters are already written to TB parameters.
--
A (partial) workaround is to handle each REL_IDX before "the FBL write function"
and ignore the write error for parameters owned by APP.
This still bears failure modes:
1. Parameters owned by FFP may trigger/take action before the write fails
2.
--

\param  p_block_instance      Pointer to the block which contains the parameter to
                        be written

\param  p_write               Pointer to a write description block. Some of the
                        components of the write description block are used
                        by the function block layer only. Components relevant
                        for the application layer are:

    rel_idx               Relative index of the parameter to be written

    subindex              Subindex within the parameter to be written
                            (Subindex 0 indicates that the whole parameter
                            is written)

    length                Number of bytes to be written

    dest                  Address where the new value should be written to
                            (If Subindex is 0, 'dest' is the address of the
                            parameter, if Subindex is not 0, 'dest' points
                            to the selected component within the parameter)
    source                Pointer to the new value

\return One of the enumeration values:
 E_OK                   Write access was successful
 E_PENDING_RES          Response is generated later
 E_FB_PARA_CHECK        Parameter check was not successful
 E_FB_PARA_LIMIT        Parameter exceeds a limit
 E_FB_WRONG_MODE        Parameter is not writable in the current target mode
 E_FB_DATA_NO_WRITE     The component selected by subindex is read-only
 E_FB_EEP_NO_WRITE      It was not possible to write the parameter to EEPROM
                        (Possible return value of the FBL write function)

*/
USIGN16 Write_handler_PTB(T_FBIF_BLOCK_INSTANCE *p_block_instance,
                                          T_FBIF_WRITE_DATA *p_write)
{
    T_FBIF_PTB     *p_PTB;
    USIGN16        result;
    USIGN8         *source;

    p_PTB = p_block_instance->p_block_desc->p_block;
    //result = E_OK;
    bool_t ignore_FBS_write_err = true; //default for APP=owned params

#if 1
    /* Check if the device is work on Standard mode, reject some params,
    Check when the TB.actual mode is LO,reject the parameter write except the
    MODE_BLK and APP_MODE;*/
    result = ffcheck_WriteFilter(p_block_instance, p_write);
    if (result != E_OK)
    {
        return result;
    }
#else
    result = E_OK;
#endif

    /* Check if the new value is correct ------------------------------------ */
    switch (p_write->rel_idx)
    {
        case REL_IDX_PTB_PTB:
        {
            result = WriteBlockHeader(p_block_instance, p_write);
            break;
        }
        case REL_IDX_PTB_ALERT_KEY:
            if (*((USIGN8 *)p_write->source) == 0)
            {
                result = E_FB_PARA_LIMIT;
            }
            break;

        case REL_IDX_PTB_MODE_BLK:
            result = ffbl_check_mode (&p_PTB->mode_blk,
                                    p_write->subindex,
                                    p_write->source,
                                    MODE_TRN_VALID
                                   );
            break;

        case REL_IDX_PTB_UPDATE_EVT:
            result = ffbl_check_unack_flag (&p_PTB->update_evt.unack, p_write->source, p_write->subindex);
            break;


        case REL_IDX_PTB_OPEN_STOP_ADJUSTMENT:
        {
            result = ffposcal_WriteOpenStopAdjustment(p_block_instance, p_write, snd_buf, rcv_buf);
            break;
        }

        case REL_IDX_PTB_FIND_STOPS:
        {
            result = ffposcal_WriteFindStops(p_block_instance, p_write, snd_buf, rcv_buf);
            break;
        }

        case REL_IDX_PTB_CHAR_SELECTION:
        {
            result = ffcharact_WriteSelection(p_block_instance, p_write, snd_buf, rcv_buf);
            break;
        }

        case REL_IDX_PTB_CUSTOM_CHAR:
        {
            result = ffcharact_InitCustomCharacterization(p_block_instance, p_write, snd_buf, rcv_buf);
            return result; //break;
        }

        case REL_IDX_PTB_CUSTOM_CHAR_POINTS:
        {
            result = ffcharact_WriteCustomCharacterization(p_block_instance, p_write, snd_buf, rcv_buf);
            //OK to return: fbs_write_param is inside ffcharact_WriteCustomCharacterization, but we may want to revisit
            return result;  //break;
        }
        case REL_IDX_PTB_OFFLINE_DIAGNOSTIC:
        {
            result = ffdiag_WriteDiagnosticCommand(p_block_instance, p_write, snd_buf, rcv_buf);
            break;
        }
        case REL_IDX_PTB_FINAL_VALUE:
        {
            result = ffmansp_WriteManualSetpoint(p_block_instance, p_write, snd_buf, rcv_buf);
            break;
        }
        case REL_IDX_PTB_FINAL_VALUE_D:
        {
            result = ffmansp_WriteManualSetpointD(p_block_instance, p_write, snd_buf, rcv_buf);
            break;
        }
        case REL_IDX_PTB_FINAL_VALUE_DINT:
        {
            result = ffmansp_WriteManualSetpointDINT(p_block_instance, p_write, snd_buf, rcv_buf);
            break;
        }
        case REL_IDX_PTB_FINAL_VALUE_CUTOFF_HI:
        {
            result = ffctllim_WriteCutoffHi(p_block_instance, p_write, snd_buf, rcv_buf);
            break;
        }
        case REL_IDX_PTB_FINAL_VALUE_CUTOFF_LO:
        {
            result = ffctllim_WriteCutoffLo(p_block_instance, p_write, snd_buf, rcv_buf);
            break;
        }
        case REL_IDX_PTB_POSITION_LIMITS:
        {
            result = ffctllim_WriteControlLimits(p_block_instance, p_write, snd_buf, rcv_buf);
            break;
        }

        case REL_IDX_PTB_ACTIVATE_CONTROL_SET:
        {
            result = ctrl_param_WriteActivate(p_block_instance, p_write, snd_buf, rcv_buf);
            break;
        }

        case REL_IDX_PTB_CUSTOM_CONTROL_SET:
        {
            result = ctrl_param_WriteCustom(p_write);
            break;
        }

        case REL_IDX_PTB_PRESSURE_RANGE:
        {
            result = WritePressureRangeCheck(p_block_instance, p_write);
            break;
        }

        case REL_IDX_PTB_CYCLE_COUNTER_A_ALERT:
        case REL_IDX_PTB_CYCLE_COUNTER_B_ALERT:
        {
            result = SyncCycleDeadBand(p_block_instance, p_write);
            break;
        }

        case REL_IDX_PTB_TRAVEL_ACCUMULATION_A_ALERT:
        case REL_IDX_PTB_TRAVEL_ACCUMULATION_B_ALERT:
        {
            result = SyncTravelDeadBand(p_block_instance, p_write);
            break;
        }

        case REL_IDX_PTB_DISCRETE_SWITCH_1_CONF:
        case REL_IDX_PTB_DISCRETE_SWITCH_2_CONF:
        {
            result = ffdosw_WriteConfiguration(p_block_instance, p_write, snd_buf, rcv_buf);
            break;
        }
        case REL_IDX_PTB_CLEAR_STATUS:
        {
            result = WriteClearStatus(p_write, snd_buf, rcv_buf);
            break;
        }
        case REL_IDX_PTB_ACTUATOR_3:
        {
            result = WriteActuator3(p_block_instance, p_write, snd_buf, rcv_buf);
            break;
        }
        case REL_IDX_PTB_TRAVEL:
        {
            result = WriteTravel(p_block_instance, p_write);
            ignore_FBS_write_err = false; //post-processing available
            break;
        }

        case REL_IDX_PTB_XD_FSTATE:
        {
            result = ff_fstate_VerifyWriteConf(p_block_instance, p_write);
            ignore_FBS_write_err = false; //post-processing available
            break;
        }

        case REL_IDX_PTB_UI_CUSTOM_CONFIGURATION:
        {
            result = WriteUiCustomConfiguration(p_write);
            ignore_FBS_write_err = false; //post-processing available
            break;
        }
        case REL_IDX_PTB_UI_ACCESS_CONTROL:
        {
             result = WriteUIAccessControl(p_block_instance, p_write, snd_buf, rcv_buf);
             break;
        }
        case REL_IDX_PTB_POSITION_HISTOGRAM:
        {
            result = OnWritePositionHistogram(p_write);
             break;
        }
        case REL_IDX_PTB_NEAR_CLOSED_ALERT:
        {
            result = WriteNearClosedAlert(p_block_instance, p_write);
            break;
        }

        case REL_IDX_PTB_SUPPLY_PRESSURE_HI_ALERT:
        case REL_IDX_PTB_SUPPLY_PRESSURE_LO_ALERT:
        case REL_IDX_PTB_SUPPLY_PRESSURE_LOLO_ALERT:
        {
            result = fftbsp_WriteSupplyPressureAlert(p_block_instance, p_write);
            break;
        }
        case REL_IDX_PTB_IP_DRIVE_CURRENT_HI_ALERT:
        case REL_IDX_PTB_IP_DRIVE_CURRENT_LO_ALERT:
        {
            result = WriteIPDriveCurrentAlert(p_write);
            break;
        }
        case REL_IDX_PTB_SETPOINT_SOURCE:
        {
            //checking if SETPOINT_SOURCE is within its enumeration limits
            source = p_write->source;
            if ( ( *source < SP_SELECT_AOFB) || ( *source > SP_SELECT_DOINT) )
            {
                result = E_FB_PARA_LIMIT;
            }
            if ( p_PTB->mode_blk.target != MODE_OS )
            {
                result = E_FB_WRONG_MODE;
            }
            break;
        }
        case REL_IDX_PTB_TEMPERATURE_HI_ALERT:
        case REL_IDX_PTB_TEMPERATURE_LO_ALERT:
        {
            result = WriteTemperatureAlert(p_write);
            break;
        }
        case REL_IDX_PTB_ACCESSORY:
        {
            result = ffrps_WriteAccessory(p_block_instance, p_write, snd_buf, rcv_buf);
            break;
        }
        case REL_IDX_PTB_DEVIATION_ALERT:
        {
            result = ffda_WriteDeviationAlert(p_block_instance, p_write);
            break;
        }
        case REL_IDX_PTB_TRAVEL_CALIBRATION:
        {
            result = ffposcal_WriteStopPosition(p_block_instance, p_write, snd_buf, rcv_buf);
            break;
        }
        case REL_IDX_PTB_ADVANCED:
        {
            result = ffcheck_WriteAdvanced(p_block_instance, p_write);
            if (E_FB_PARA_LIMIT == result)
            {
                if (0 != p_write->subindex)
                {
                    return E_OK;
                }

            }
            else
            {
                break;
            }
            break;
        }
        case REL_IDX_PTB_DIAGNOSTIC_CONFIGURATION:
        {
            result = ffdc_WriteDiagnostic_Configuration(p_block_instance, p_write);
            break;
        }
        default:
        {
            ignore_FBS_write_err = false;
            break;
        }

    } /* end switch */

    //Note that the original returned void; I tried to change it but broke the pattern (see below)
    /* Write the parameter by means of the FBL write function --------------- */
    if (result == E_OK)
    {
        u16 result1 = fbs_write_param (p_block_instance, p_write);
        if(!ignore_FBS_write_err)
        {
            result = result1;
        }
    }

  /* Perform actions in dependence of the new value ----------------------- */
    if (result == E_OK)
    {
        switch (p_write->rel_idx)
        {
            case REL_IDX_PTB_AUTOTUNE:
                result = ctrl_param_Autotune(p_block_instance, snd_buf, rcv_buf);
                break;

            case REL_IDX_PTB_POSITION_ERROR_HISTOGRAM:
            {
                result = OnWritePosErrHistogram(p_PTB);
                break;
            }

            case REL_IDX_PTB_UI_LANGUAGE:
            {
                result = WriteUILanguage(p_block_instance, snd_buf, rcv_buf);
                break;
            }

            case REL_IDX_PTB_APP_MODE:
            {
                result = WriteAPPMode(p_block_instance, snd_buf, rcv_buf);
                break;
            }

            case REL_IDX_PTB_PRESSURE_RANGE:
            {
                result = WritePressureRange(p_block_instance, snd_buf, rcv_buf);
                break;
            }

            case REL_IDX_PTB_TRAVEL:
            {   // Travel parameter updated.
                // -- Calculate 1% value to be used for later conversions
                PTB_CalculateTravelConversion(p_block_instance);
                break;
            }

            case REL_IDX_PTB_ALERT_LOG :
                result = Alert_WriteAlertLogEntry(p_block_instance);
                break;

            case REL_IDX_PTB_XD_FSTATE:
            {
                result = ff_fstate_PostProcessWriteConf(p_block_instance, p_write);
                break;
            }

            case REL_IDX_PTB_FACTORY_USE_1:
                result = ffact_WriteFactoryUse1(p_block_instance, snd_buf, rcv_buf);
                break;
            default:
                break;

        }//end switch
    }//end if result == E_OK

    return (result);
}


FUNCTION PUBLIC USIGN8 appl_get_comm_state (VOID)

/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
  Provide current state of HART communication to caller.

PARAMETERS:

GLOBALS_AFFECTED:
  none

RETURN_VALUES:
  value of p_PTB->comm_state

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  return(comm_state);
}






/*************************************************************************************/
/*************************************************************************************/
/*                                                                                   */
/*                              Local Functions                                      */
/*                                                                                   */
/*************************************************************************************/
/*************************************************************************************/
#if 0
FUNCTION LOCAL VOID appl_trn_start_tb
  (
    INOUT T_FBIF_BLOCK_INSTANCE *    p_block_instance
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

PARAMETERS

RETURN_VALUES
----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  T_APPL_TB *         p_tb;
  T_APPL_TB_DATA *   p_tb_data;

FUNCTION_BODY

  if (p_block_instance->p_block_data == NULL)
  {
    p_block_instance->p_block_data = osif_malloc (sizeof(T_APPL_TB_DATA));
  }

  _ASSERT (p_block_instance->p_block_data != NULL);

  p_tb      = p_block_instance->p_block_desc->p_block;
  p_tb_data = p_block_instance->p_block_data;

  /* Disable update events ------------------------------------------------ */
  ffbl_disable_update_events (&p_tb->update_evt);

  /* Initialize variables for Softing's transducer block example ---------- */
  p_tb_data->old_block_alarm_bits = 0;

  /* Perform other block-specific start actions --------------------------- */

  /* ... */

  return;
}
#endif




#if 0
FUNCTION LOCAL VOID appl_trn_update_mode
  (
    INOUT T_FBIF_BLOCK_INSTANCE *    p_block_instance
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function is used to update the actual modes of a transducer block.

  In this example the transducer block does not follow the resource block to
  Out-of-Service. FF-891-1.6, 4.4.3.24 specifies:

  ".... if the mode of the resource block is changed to Out-of-Service (O/S) ....
  the actual mode of all function blocks in the resource will be forced to O/S mode.
  The mode of transducer blocks may not be affected."

  Thus the device developer may decide for each transducer block if it follows
  the resource block to Out-of-Service.

  However, it must be annotated that ITK-4.6 test-case i0603_06 does not pass
  if too many TBs follow the resource block: during the test the resouce block
  switches from Auto to Out-of-Service and back to Auto. If too many TBs follow
  the resource block, a lot of block-alarm-avtive and block-alarm-clear event are
  sent via FF bus. Because of an insufficient calculation of timings the test-case
  does not wait long enough for the events issued by the resource block and ends
  untimely with an unjustified FAILED.

  Test-case i0603_06 should pass if one or two TBs follow the resource block.
  It fails if four or more TBs follow the resource block.

PARAMETERS

RETURN_VALUES
----------------------------------------------------------------------------*/

#endif //appl_trn_update_mode()


#if 0

FUNCTION LOCAL VOID appl_trn_set_xd_error
  (
    IN const T_FBIF_BLOCK_DESCR *   p_block_desc,
    IN USIGN8                       xd_error
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:

  The function is called after occurrance of a device specific failure and
  sets the XD error in the associated transducer block (AITB).

PARAMETERS:

  xd_error          XD error to set.

RETURN_VALUES:

  None
----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  static T_FBIF_BLOCK_INSTANCE *  p_block_instance = NULL;
  T_FBIF_PTB  *                   p_PTB;
  T_APPL_TB_DATA *                p_PTB_data;

FUNCTION_BODY

  if (p_block_instance == NULL)
  {
    p_block_instance = fbs_get_block_inst(ID_PTB_1);
  }

  _ASSERT (p_block_instance != NULL);

  p_PTB      = p_block_instance->p_block_desc->p_block;
  p_PTB_data = p_block_instance->p_block_data;

  if ( xd_error != 0 )
  {
    p_PTB->xd_error_pos   = xd_error;                          /* Set XD error */
    p_PTB->block_err |= BLK_ERR_OTHER;            /* Inform about XD error */
  }
  else
  {
    p_PTB->xd_error_pos   = 0;                               /* Reset XD error */
    p_PTB->block_err &= ~BLK_ERR_OTHER;           /* Inform about XD error */
  }

  ffbl_update_block_alarm
    (
      p_block_desc->block_id,p_block_desc->idx_blk_alm,
      p_PTB->block_err,&p_PTB_data->old_block_alarm_bits
    );

  return;
}


#endif //



FUNCTION LOCAL const T_FBIF_BLOCK_DESCR * appl_trh_get_block_descr
(
  USIGN8   block_id
)
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:

  Get the pointer to the required block.

PARAMETERS:

  block_id    ID of the block the descriptor is requested. the range check that
              block_id is not greater than max number of blocks in device is done
              within fbs_get_block_inst()

RETURN_VALUES:

  p_block_desc  descriptor of block_id TB
----------------------------------------------------------------------------*/
{
  const T_FBIF_BLOCK_DESCR *p_block_desc = fbs_get_block_inst(block_id)->p_block_desc;

  _ASSERT (p_block_desc != NULL);

  return (p_block_desc);

}

#if 0 //AK: Unused
FUNCTION USIGN16 appl_send_acyc_HART_cmd
  (
    IN USIGN8   command
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
    Issues the HART number as passed by 'command'. Prepares the request buffer,
    starts the command execution and, after its completion, handles the HART
    response.

PARAMETERS:
    command:            HART command to be executed

GLOBALS_AFFECTED:
    send_buff:          buffer where the HART request frame will be prepared
    receive_buff:       Buffer which will receive the HART response

RETURN_VALUES:
    E_OK =>             command completed successfully
    E_FB_WRONG_MODE =>  HART communication not (yet) working
    E_FB_TEMP_ERROR =>  command did not succeed due to temporary communication
                        problem

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
    USIGN16 ret_code;
    USIGN8  hart_len;
    fferr_t err = E_OK;

FUNCTION_BODY

    /* Be sure that 'sendBuff' and 'receiveBuff' are large enough to */
    /* hold the HART request rsp. response frame                     */
    /*---------------------------------------------------------------*/
  Appl_handle_HART_cmd (command, SEND, snd_buf, &hart_len, rcv_buf, 0);


    /* Pass command to HART master */
    /*-----------------------------*/
  ret_code = HART_acyc_cmd(command, snd_buf, hart_len, rcv_buf);

    /* Check for errors during command execution */
    /*-------------------------------------------*/

  if(ret_code != 0)
  {
      err = E_FB_TEMP_ERROR; //Hmm... That's what Sofing does
  }

  if ( (ret_code != 0) || (rcv_buf[0] != 0) )
  {
    return (E_FB_TEMP_ERROR);
  }

  if(err == E_OK)
  {
        /* Handle HART response contained in 'rcv_buf'     */
        /*-------------------------------------------------*/
      Appl_handle_HART_cmd (command, RECEIVE, snd_buf, &hart_len, rcv_buf, 0);
  }
  return err;
} // ----- end of appl_send_acyc_HART_cmd() -----
#endif //0

/*************************************************************************************/
/*************************************************************************************/
/*                                                                                   */
/*                        Interface functions required                               */
/*                             by the HART-Master                                    */
/*                                                                                   */
/*************************************************************************************/
/*************************************************************************************/

static void Appl_handle_HART_cmd
  (
    IN USIGN8 cmd,
    IN USIGN8 access,
    IN USIGN8 *send_buff,
    IN USIGN8 *send_length,
    IN USIGN8 *receive_buff,
    IN USIGN16 receive_len
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
  Performs the mapping between HART commands and the parameters as contained in
  the tranducer block. This function will be called by the HM component in order
  to prepare a correct HART frame ('access = SEND') rsp. handle the according
  HART response ('access = RECEIVE') for HART command 'cmd'.

PARAMETERS:
  cmd:                  number of the HART command currently in process
  access:               access mode
                          'SEND' if a HART request frame is to be prepared
                          'RECEIVE' if a HART response is to be decoded
  send_buff:            buffer where to prepare the HART request frame
  send_length:          size of the prepared HART request frame
  receive_buff:         buffer with the HART response frame

GLOBALS_AFFECTED:
  p_block_inst_aitb:    pointer to instance of AITB

RETURN_VALUES:
  none

------------------------------------------------------------------------------*/
{
    UNUSED_OK(receive_len);
LOCAL_VARIABLES
    static T_FBIF_PTB *  p_PTB = NULL;
    static T_FBIF_BLOCK_INSTANCE * p_block_instance = NULL;

    USIGN8 * from_hart = receive_buff + 2;   /* Prepare variables used in HART access macros */
    USIGN8 * toHart = send_buff;

FUNCTION_BODY

  _ASSERT((access == RECEIVE) || (access == SEND));

  /* Set local receive / send buffer -------------------------------- */
    if ( access == RECEIVE )
    {
        _ASSERT(receive_buff != NULL);
        receive_buff[1] &= ~0x40;
        UNUSED_OK(from_hart); //clean it out later

        //---------------------------------------------------------------------
        // This section of the code is invoked by the HM for the RECEVIE operation
        // Call the MN code to register the fact that we have a Response
#if 1
        mn_HART_ReceivedResponse();
#endif
    }
    else
    {
        _ASSERT(send_buff != NULL);
    }

    if (p_block_instance == NULL)
    {
        /* get the block instance of the TB */
        p_block_instance = fbs_get_block_inst(ID_PTB_1);
    }

    _ASSERT(p_block_instance != NULL);

    if (p_PTB == NULL)
    {
        /* It would also be possible to get the pointer to the block over the block instance p_block_instance->p_block_desc->p_block,
           but here function appl_trh_get_block_descr is intentionally used as example to show the easier way to get the block pointer. */
        p_PTB = appl_trh_get_block_descr((USIGN8)ID_PTB_1)->p_block;
    }

    _ASSERT(p_PTB != NULL);

    /* Switch in case of the command ---------------------------------- */
    switch (cmd)
    {
#if 0 //AK: not used
        /*----------------------------------------------------------------*/
        /* HART command 0: Read unique identifier */
        /*----------------------------------------------------------------*/
        case CMD_0_INITIALIZE_HART_COMM:
        {
            if ( access == RECEIVE )
            {
            }
            else //already have an assert above: if ( access == SEND )
            {
                *send_length = 0;
            }
            break;
        } /* case 0 */
#endif //0
    /*----------------------------------------------------------------*/
    /* HART Cyclic Commands */
    /*----------------------------------------------------------------*/
    case CMD_177_READ_PROCESS_PARAMETERS:
    {
        if ( access == RECEIVE )
        {
            mnipc_ReceiveSpAndMode(receive_buff, p_resource, p_PTB);
            mnipc_ReadVars(p_PTB, receive_buff);
        }
        // CMD_177_READ_PROCESS_PARAMETERS  --  SEND segment
        else //already have an assert above: if ( access == SEND )
        {
            mnipc_SendSpAndMode(send_buff, p_resource, p_block_instance);
            mnipc_WriteDOswitches(p_block_instance, send_buff);
            mnipc_WriteVars(p_PTB, send_buff, send_length);
        }
        break;
    } /* end case CMD_177_READ_PROCESS_PARAMETERS */

    /*----------------------------------------------------------------*/
    /* HART Regular Commands */
    /*----------------------------------------------------------------*/
        case CMD_186_WRITE_SHORT_VARIABLES:
        {
            if ( access == RECEIVE )
            {
            }
            else //already have an assert above:if ( access == SEND )
            {
                cmd_186_handler(toHart, send_length);
            }
            break;
        }
        /* end case CMD_186_WRITE_SHORT_VARIABLES */

        case CMD_187_WRITE_ARRAY:
        {
            if ( access == RECEIVE )
            {
            }
            else //already have an assert above:if ( access == SEND )
            {
                cmd_187_handler(toHart, send_length);
            }
            break;
        }
        /* end case CMD_187_WRITE_ARRAY */

        case CMD_170_176_READ_UNIT:
        {
            cmd_170_handler(p_block_instance, from_hart, toHart, send_length, access);
            break;
        }

        /*----------------------------------------------------------------
        HART Acyclic Commands NOT ALLOWED IN THIS FUNCTION
        Use acyclic commands instead
        ----------------------------------------------------------------*/
        default:
            break;
    } /* end switch */

} /* FUNCTION Appl_handle_HART_cmd */





/******************************************************************************/


/******************************************************************************/

FUNCTION GLOBAL VOID Appl_set_HART_status
  (
    IN  USIGN8  status
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
  Callback function as required by the HART master.

PARAMETERS:
  status:               current status of the HART communication

GLOBALS_AFFECTED:
  none

RETURN_VALUES:
  none

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  static const T_FBIF_PTB *                 p_PTB  = NULL;
  //static const T_FBIF_BLOCK_DESCR *   p_block_desc = NULL;
  static const T_FBIF_BLOCK_INSTANCE *      p_block_instance = NULL;
  static HART_STATES                  hart_state = HART_UNINITIALIZED;

FUNCTION_BODY

  if (p_block_instance == NULL)
  {
      p_block_instance  = fbs_get_block_inst (ID_PTB_1);
      //p_block_desc      = p_block_instance->p_block_desc;
      p_PTB             = p_block_instance->p_block_desc->p_block;
  }

  _ASSERT (p_block_instance != NULL);
  //_ASSERT (p_block_desc != NULL);
  _ASSERT (p_PTB != NULL);

  switch (hart_state)
  {
    case HART_UNINITIALIZED:
      if (status == DATA_INIT_FINISHED)
      {
        hart_state = HART_COMM;
        comm_state = DEV_COMM_OK;
#if 0 //AK errors centralization effort
        p_PTB->block_err &= ~BLK_ERR_READBACK_CHECK_FAILED;
        appl_trn_set_xd_error(p_block_desc, XD_ERROR_NO_ERROR); /* No Error */
#endif
        /* Enable the application task to contiune with device startup */
        osif_set_event (APPL_TASK_ID, OS_IF_EVENT_RESUME);
      }
      else if (status == NO_HART_COMM)
      {
        comm_state = DEV_COMM_NOT_OK;

#if 0 //AK errors centralization effort
        p_PTB->block_err |= BLK_ERR_READBACK_CHECK_FAILED;
        appl_trn_set_xd_error(p_block_desc, XD_ERROR_I_O_FAILURE); /* I/O Failure */
#endif

        if ( (p_PTB->mode_blk.target & MODE_OS) == 0) //i.e. if MODE_OS not intended
        {
            ptb_JamDynParamStatus(SQ_BAD | SUB_DEVICE_FAILURE);
        }

        /* Enable the application task to contiune with device startup */
        osif_set_event (APPL_TASK_ID, OS_IF_EVENT_RESUME);
      }
      else
      {
          //Per Softing, nothing to do; so the else is for MISRA then
      }
      break;

    case HART_COMM:
      if (status == NO_HART_COMM)
      {
         hart_state = HART_COMM_ERROR;
         comm_state = DEV_COMM_NOT_OK;

#if 0 //AK errors centralization effort
         p_PTB->block_err |= BLK_ERR_READBACK_CHECK_FAILED;
         appl_trn_set_xd_error(p_block_desc, XD_ERROR_I_O_FAILURE); /* I/O Failure */
#endif
         if ( (p_PTB->mode_blk.target & MODE_OS) == 0) //i.e. if MODE_OS not intended
         {
            ptb_JamDynParamStatus(SQ_BAD | SUB_DEVICE_FAILURE);
         }
      }
      else
      {
          hart_state = HART_COMM;
          comm_state = DEV_COMM_OK;
      }

      break;

    case HART_COMM_ERROR:
      if (status == HART_COMM_OK)
      {
         hart_state = HART_COMM;
         comm_state = DEV_COMM_OK;
#if 0 //AK errors centralization effort
         p_PTB->block_err &= ~BLK_ERR_READBACK_CHECK_FAILED;
         appl_trn_set_xd_error(p_block_desc, XD_ERROR_NO_ERROR); /* No Error */
#endif
      }
      break;
    case HART_INIT_FINISHED:
    default:
      break;

  } /* end switch */
}

//Do not remove until we have source code for FDC task call stack
#ifdef _lint
//lint -specific( -esym(826,Write_handler_PIDFB))
//lint -save -e826 -e534 -e718 -e746 -e715
//Artificial "threads"
USIGN16 dummy_FDC_task(T_FBIF_BLOCK_INSTANCE *p_block_instance,
                     T_FBIF_READ_DATA *p_read,
                     T_FBIF_WRITE_DATA *p_write);
USIGN16 dummy_FDC_task(T_FBIF_BLOCK_INSTANCE *p_block_instance,
                     T_FBIF_READ_DATA *p_read,
                     T_FBIF_WRITE_DATA *p_write)
{
    return Read_handler_PTB(p_block_instance, p_read)
        | Read_handler_PIDFB(p_block_instance, p_read)
        | Read_handler_CSFB(p_block_instance, p_read)
        | Read_handler_AIFB(p_block_instance, p_read)
        | Read_handler_AOFB(p_block_instance, p_read)
        | Read_handler_DIFB(p_block_instance, p_read)
        | Read_handler_DOFB(p_block_instance, p_read)
        | Read_handler_ISFB(p_block_instance, p_read)
        | Read_handler_RESB(p_block_instance, p_read)
        | Read_handler_OSFB(p_block_instance, p_read)

        | Write_handler_PTB(p_block_instance, p_write)
        | Write_handler_PIDFB(p_block_instance, p_write)
        | Write_handler_CSFB(p_block_instance, p_write)
        | Write_handler_AIFB(p_block_instance, p_write)
        | Write_handler_AOFB(p_block_instance, p_write)
        | Write_handler_DIFB(p_block_instance, p_write)
        | Write_handler_DOFB(p_block_instance, p_write)
        | Write_handler_ISFB(p_block_instance, p_write)
        | Write_handler_RESB(p_block_instance, p_write)
        | Write_handler_OSFB(p_block_instance, p_write)

        ;
}

void dummy_appl_task(T_EVENT  event, T_FBIF_BLOCK_INSTANCE *p_block_instance);
void dummy_appl_task(T_EVENT  event, T_FBIF_BLOCK_INSTANCE *p_block_instance)
{
    appl_task(event);

    Start_AIFB(p_block_instance);
    Start_AOFB(p_block_instance);
    Start_DIFB(p_block_instance);
    Start_DOFB(p_block_instance);
    Start_ISFB(p_block_instance);
    Start_MAIFB(p_block_instance);
    Start_OSFB(p_block_instance);
    Start_PIDFB(p_block_instance);
    Start_PTB(p_block_instance);
    Start_RESB(p_block_instance);
}


void dummy_FB_exec_task(T_EVENT  event, T_FBIF_BLOCK_INSTANCE *p_block_instance,
                     T_FBIF_PROCESS_DATA *p_exec_tb);
void dummy_FB_exec_task(T_EVENT  event, T_FBIF_BLOCK_INSTANCE *p_block_instance,
                     T_FBIF_PROCESS_DATA *p_exec_tb)
{
    Execute_AIFB(p_block_instance);
    Execute_AOFB(p_block_instance);
    Execute_DIFB(p_block_instance);
    Execute_DOFB(p_block_instance);
    Execute_ISFB(p_block_instance);
    Execute_MAIFB(p_block_instance);
    Execute_OSFB(p_block_instance);
    Execute_PIDFB(p_block_instance);
    Execute_PTB(p_block_instance, p_exec_tb);
}

void dummy_Background_task(T_FBIF_BLOCK_INSTANCE *p_block_instance);
void dummy_Background_task(T_FBIF_BLOCK_INSTANCE *p_block_instance)
{
    Background_AIFB(p_block_instance);
    Background_AOFB(p_block_instance);
    Background_DIFB(p_block_instance);
    Background_DOFB(p_block_instance);
    Background_ISFB(p_block_instance);
    Background_MAIFB(p_block_instance);
    Background_OSFB(p_block_instance);
    Background_PIDFB(p_block_instance);
    Background_PTB(p_block_instance);
    Background_RESB(p_block_instance);
}

//lint -restore
#endif // _lint
