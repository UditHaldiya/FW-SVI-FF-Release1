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

FILE_NAME          appl_dia.c



FUNCTIONAL_MODULE_DESCRIPTION
  With FF-912, Field Diagnostics Profile, a new application-related
  functionality is available. This module shows in a simple example how
  the application can use field diagnostics.

=========================================================================== */
  #include "keywords.h"
  #define  MODULE_ID      (COMP_FAPP + MOD_APPL_DIA)

INCLUDES
  #include "base.h"
  #include "except.h"
  #include "osif.h"
  #include "fbs_api.h"
  #include "ffbl_api.h"
  #include "fbif_idx.h"
  #include "appl_int.h"
  #include "resb.h"

#include "mnfault2alert.h"

LOCAL_DEFINES
#ifdef USE_FIELD_DIAGNOSTICS /* -------------------------------------------- */

  #define FD_NO_RECOMM_ACT          1
  #define FD_SIMULATE_RECOMM_ACT    2
  #define FD_RECOMM_ACT_TB          3
  #define FD_NO_HART_COMMUNICATION  4
  #define FD_RECOMM_ACT_UNKNOWN     68

  #define FD_HART_COM_DIAG_COND     31


LOCAL_TYPEDEFS
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
  }
    T_APPL_TB;

  typedef struct
  {
    USIGN8      block_id;
    USIGN16     index;
  }
  T_APPL_DIA_CHECK_FCT;

FUNCTION_DECLARATIONS
  FUNCTION LOCAL VOID    appl_dia_check_function (VOID);
  FUNCTION LOCAL USIGN16 appl_dia_check_transducer_block (USIGN8);

EXPORT_DATA

IMPORT_DATA

LOCAL_DATA
#include "da_fdev.h"                             /* DATA SEGMENT DEFINITION */
  NO_INIT STATIC USIGN32                  appl_diagnostic_value;
  NO_INIT STATIC T_APPL_DIA_CHECK_FCT     appl_dia_check_fct;
  NO_INIT STATIC USIGN16                  fd_alarm_source[32];
#include "da_def.h"                              /* DEFAULT DATA SEGMENT */


/* ------------------------------------------------------------------------ */
/* --- Definition of 'Source Block Index' for Field Diagnostics alerts ---- */
/* ------------------------------------------------------------------------ */

/* FF-912, Field Diagnostics Profile, defines a element 'Source Block Index'
  for Field Diagnostics alerts. The element 'Source Block Index' contains the
  OD Index of the block that is the source of the Field Diagnostics condition.
  'Source Block Index' is 0 if the condition does not relate to a certain block.

  In this Field Diagnostics example the following array is used to assign the
  resource block and the available transducer blocks to certain Field Diagnostics
  conditions.
*/


/* ***************************************************************************

  Note that the following section implements an example only!

  THIS EXAMPLE HAS TO BE REMOVED FROM ANY REAL DEVICE !

  The sources of the Field Diagnostics conditions require a device-specific
  re-definition !

*************************************************************************** */

#include "cs_fdev.h"                         /* CONSTANT SEGMENT DEFINITION */


  /* Softing's FBK-Start application -------------------------------------- */

  #define FIELD_DIAGNOSTIC_FBK_START_ACTIVE /* Just an example and for test! */

  const USIGN16 fd_alarm_source_const[] = {
                   0,                         /* Source of Cond00 can vary */
                   0,                         /* Cond01 does not relate to a certain block */
                   0,                         /* Cond02 does not relate to a certain block */
                   0,                         /* Cond03 does not relate to a certain block */
                   0,                         /* Cond04 does not relate to a certain block */
                   0,                         /* Cond05 does not relate to a certain block */
                   0,                         /* Cond06 does not relate to a certain block */
                   0,                         /* Cond07 does not relate to a certain block */
                   0,                         /* Cond08 does not relate to a certain block */
                   0,                         /* Cond09 does not relate to a certain block */
                   0,                         /* Cond10 does not relate to a certain block */
                   0,                         /* Cond11 does not relate to a certain block */
                   0,                         /* Cond12 does not relate to a certain block */
                   0,                         /* Cond13 does not relate to a certain block */
                   0,                         /* Cond14 does not relate to a certain block */
                   0,                         /* Cond15 does not relate to a certain block */
                   0,                         /* Cond16 does not relate to a certain block */
                   0,                         /* Cond17 does not relate to a certain block */
                   0,                         /* Cond18 does not relate to a certain block */
                   0,                         /* Cond19 does not relate to a certain block */
                   0,                         /* Cond20 does not relate to a certain block */
                   0,                         /* Cond21 does not relate to a certain block */
                   0,                         /* Cond22 does not relate to a certain block */
                   0,                         /* Cond23 does not relate to a certain block */
                   0,                         /* Cond24 does not relate to a certain block */
                   0,                         /* Cond25 does not relate to a certain block */
                   0,                         /* Cond26 does not relate to a certain block */
                   0,                         /* Cond27 does not relate to a certain block */
                   0,                         /* Cond28 does not relate to a certain block */
                   0,                         /* Cond29 does not relate to a certain block */
                   0,                         /* Cond30 does not relate to a certain block */
                   BLOCK_14_PTB               /* Cond31 relates to Positioner Transducer Block */
                   };

  const T_DIAG_ALARM_SOURCES  diag_alarm_sources [SIZEOF_DIAG_ALARM_SOURCES] =
  {
    /* FF DIAG NUMBER - CMD #136 BYTE - BIT - RECOMMENDED ACTION NUMBER - PRIORITY */

    /* Device Needs Maintenance Now - priority 1 */
    { 30, 1, 6, 19, 1 }, /*- SUPPLY_HIGH             byte 1 bit 6 */
    { 30, 2, 0, 21, 1 }, /*! BIAS_OUT_OF_RANGE       byte 2 bit 0 */
    { 30, 2, 1, 22, 1 }, /*! IP_OUT_OF_RANGE         byte 2 bit 1 */
    { 30, 6, 3, 56, 1 }, /*- IP_SERVO_LOW            byte 6 bit 3 */
    { 30, 6, 4, 57, 2 }, /*- IP_SERVO_HIGH           byte 6 bit 4 */
    { 30, 7, 1, 62, 4 }, /*- LINKAGE_DRIFT           byte 7 bit 1 */
    { 30, 7, 2, 63, 4 }, /*- VALVE_STUCK_CLOSED      byte 7 bit 2 */
    { 30, 7, 3, 64, 4 }, /*- VALVE_STUCK_OPENED      byte 7 bit 3 */

    /* Lost NV Data - priority 2 */
    { 29, 3, 0, 29, 1 }, /*! NVM_CHECKSUM0           byte 3 bit 0 */
    { 29, 3, 5, 34, 2 }, /*! NVM_TEST                byte 3 bit 5 */

    /* Lost Static Data - priority 3 */
    { 28, 3, 2, 31, 1 },  /*! FW_CHECKSUM            byte 3 bit 2 */

    /* Memory Failure - priority 4 */
    { 27, 3, 1, 30, 2 },  /*! RAM_CHECKSUM           byte 3 bit 1 */

    /* Output Failure Detected - priority 5 */
    { 26, 4, 0, 37, 1 }, /*! REF_VOLTAGE             byte 4 bit 0 */
    { 26, 4, 1, 38, 1 }, /*! POSITION_SENSOR         byte 4 bit 1 */
    { 26, 4, 2, 39, 1 }, /*- CURRENT_SENSOR          byte 4 bit 2 */
    { 26, 4, 3, 40, 1 }, /*! TEMPERATURE_SENSOR      byte 4 bit 3 */
    { 26, 5, 0, 45, 1 }, /*! PRESSURE4               byte 5 bit 0 */
    { 26, 5, 6, 51, 1 }, /*! SELF_CHECK              byte 5 bit 6 */
    { 26, 4, 4, 41, 2 }, /*! CURRENT_SENSOR_2ND      byte 4 bit 4 */

    /* Device Needs Maintenance Soon - priority 6 */
    { 25, 4, 5, 42, 1 }, /*! PRESSURE1               byte 4 bit 5 */
    { 25, 4, 6, 43, 1 }, /*! PRESSURE2               byte 4 bit 6 */
    { 25, 4, 7, 44, 1 }, /* PRESSURE3               byte 4 bit 7 */
    { 25, 6, 0, 53, 1 }, /*- ESD_TRIP                byte 6 bit 0 */
    { 25, 0, 2, 7,  2 }, /*! ACTUATOR                byte 0 bit 2 */
    { 25, 0, 3, 8,  2 }, /*! AIR_SUPPLY_LOW          byte 0 bit 3 */
    { 25, 0, 4, 9,  2 }, /*! POSITION_ERROR          byte 0 bit 4 */
    { 25, 0, 6, 11, 2 }, /*! KEYPAD                  byte 0 bit 6 */
    { 25, 0, 7, 12, 2 }, /*- MARGINAL_POWER          byte 0 bit 7 */
    { 25, 2, 2, 23, 2 }, /*! TEMPR_OUT_OF_RANGE      byte 2 bit 2 */
    { 25, 5, 1, 46, 2 }, /*! PRESSURE5               byte 5 bit 1 */

    /* Block Configuration Error - priority 7 */
    { 24, 2, 4, 25, 3 }, /*- CONF_1                  byte 2 bit 4 */
    { 24, 0, 5, 10, 4 }, /*! I2CBUS                  byte 0 bit 5 */

    /* Other - priority 8 */
    { 23, 1, 0, 13, 3 }, /*! CALIBRATION_FAILED      byte 1 bit 0 */
    { 23, 1, 1, 14, 3 }, /*! FIND_STOPS_FAILED       byte 1 bit 1 */
    { 23, 1, 2, 15, 3 }, /*! AUTOTUNE_FAILED         byte 1 bit 2 */
    { 23, 1, 3, 16, 3 }, /*! STD_DIAGNOSTICS_FAILED  byte 1 bit 3 */
    { 23, 1, 4, 17, 3 }, /*! EXT_DIAGNOSTICS_FAILED  byte 1 bit 4 */

    /* Faults, reported in Detected_Faults only - priority 9 */
    { 22, 3, 4, 33, 1 }, /*! FACTORYWRITE            byte 3 bit 4 */
    { 22, 3, 7, 36, 1 }, /*! FACTORYMODE             byte 3 bit 7 */
    { 22, 1, 5, 18, 2 }, /*! RTOS_SCHEDULING         byte 1 bit 5 */
    { 22, 3, 3, 32, 2 }, /*! STACK                   byte 3 bit 3 */
    { 22, 5, 2, 47, 2 }, /*! WATCHDOG_TIMEOUT        byte 5 bit 2 */
    { 22, 5, 3, 48, 2 }, /*! NVM_WRITE               byte 5 bit 3 */
    { 22, 5, 4, 49, 2 }, /*! IRQ_FAULT               byte 5 bit 4 */
    { 22, 5, 5, 50, 2 }, /*! TMOUT_FLASH_TEST        byte 5 bit 5 */
    { 22, 5, 7, 52, 2 }, /*! SOFTWARE                byte 5 bit 7 */
    { 22, 6, 1, 54, 2 }, /*- AI_PV_LOW               byte 6 bit 1 */
    { 22, 6, 2, 55, 2 }, /*- AI_PV_HIGH              byte 6 bit 2 */
    { 22, 0, 0, 5,  3 }, /*! RESET                   byte 0 bit 0 */
    { 22, 0, 1, 6,  3 }, /*- LOW_POWER               byte 0 bit 1 */
    { 22, 2, 3, 24, 3 }, /*- DI_ABNORMAL             byte 2 bit 3 */
    { 22, 2, 5, 26, 4 }, /*- LATCH_FSAFE             byte 2 bit 5 */
    { 22, 2, 6, 27, 4 }, /*! TIGHT_SHUTOFF           byte 2 bit 6 */
    { 22, 6, 6, 59, 4 }, /*- DIAGPROC1               byte 6 bit 6 */
    { 22, 6, 7, 60, 4 }, /*- RANGE1                  byte 6 bit 7 */
    { 22, 7, 0, 61, 4 }, /*- RANGE2                  byte 7 bit 0 */
    { 22, 7, 4, 65, 4 }, /*! UI_OFF                  byte 7 bit 4 */
    { 22, 7, 5, 66, 4 }, /*- RANGE3                  byte 7 bit 5 */
    { 22, 7, 6, 67, 4 }, /*- RANGE4                  byte 7 bit 6 */
  };

#include "cs_def.h"                             /* DEFAULT CONSTANT SEGMENT */

/****************************************************************************/

FUNCTION PUBLIC VOID appl_dia_start_field_diagnostics (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function starts the diagnosis module. If there is an array of 'Source
  Block Indexes’ this function shall load the array to the FFBL component

PARAMETERS
  None

RETURN_VALUES
  None

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  appl_diagnostic_value = 0;
  appl_dia_check_fct.block_id = 0;
  appl_dia_check_fct.index    = 0;

  memcpy (fd_alarm_source,fd_alarm_source_const,32*sizeof(USIGN16));

  ffbl_load_fdiag_sources (fd_alarm_source_const);

  return;
}





FUNCTION PUBLIC VOID appl_dia_update_field_diagnostics (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function is an example of how the application may check its diagnostic
  conditions periodically and how it updates the appropriate parameters.

PARAMETERS
  None

RETURN_VALUES
  None

----------------------------------------------------------------------------*/
{
#if 0 //AK: See replacement in mnfaults_Import()
LOCAL_VARIABLES

  USIGN8 index;
  BOOL   diag_alarm = FALSE;

FUNCTION_BODY

  /* If the communication with the slave is lost, clear all the previously set FD conditions */
  if ( appl_get_comm_state() == DEV_COMM_NOT_OK )
  {
    appl_dia_set_one_fd_cond ( FD_HART_COM_DIAG_COND ); /* set HART communication diagnosis condition */
    return;
  }
  else
  {
    appl_dia_reset_one_fd_cond ( FD_HART_COM_DIAG_COND ); /* reset HART communication diagnosis condition */

    /* here check status of all bits defined in faultCode on cmd #136 */
    for ( index = 0; index < SIZEOF_DIAG_ALARM_SOURCES; index++ )
    {
      /* here reset diagnosis bits */
      if ( ( diag_alarm_sources[index].diag_alarm_number != diag_alarm_sources[index -1].diag_alarm_number) && (index > 0) )
      {
        if ( diag_alarm == FALSE )appl_dia_reset_one_fd_cond ( diag_alarm_sources[index - 1].diag_alarm_number );
        else diag_alarm = FALSE;
      }

      /* here set diagnosis bits */
      if ( faultCode[diag_alarm_sources[index].byte] & ( (USIGN8) 1 << diag_alarm_sources[index].bit ) )
      {
        appl_dia_set_one_fd_cond ( diag_alarm_sources[index].diag_alarm_number );
        diag_alarm = TRUE;
      }
    }/* end - for () */
  }

  /* here reset diagnosis bits - condition is used for last structure element */
  if ( diag_alarm == FALSE )appl_dia_reset_one_fd_cond ( diag_alarm_sources[index - 1].diag_alarm_number );

  return;
#endif
}




FUNCTION GLOBAL USIGN32 Appl_get_diagnostic_value (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function is called periodically by the background routine of the resource
  block. When called by the resource block the application layer shall provide
  its 32 bit diagnostic value. The background routine copies this diagnostics value
  into the resource block is copied to the FD_SIMULATE.DiagnosticValue.

  In this example there is an additional call of a check function. The check
  function is required by FF-912. However, it is not necessary to call the check
  function as a sub-routine of this function. Alternatively, the check function can
  be called by an application-specific scheduler which checks the transducer block
  modes periodically.

PARAMETERS
  None

RETURN_VALUES
  The current diagnosis value.

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  appl_dia_check_function ();

  return (appl_diagnostic_value);
}







FUNCTION GLOBAL USIGN16 Appl_get_recommend_fd_action
  (
      USIGN32           field_diag_value,
      BOOL              simulation_active
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  The resource block parameter FD_RECOMMEN_ACT is an enumerated summarization
  of the most severe condition or conditions detected. The DD should describe
  by enumerated action, what should be done to alleviate the condition or
  conditions. 0 is defined as ‘Not Initialized’, 1 is defined as
  ‘No Action Required’; all other values have to be defined device-specific.

  The FFBL component calls this callback function each time the field diagnostics
  conditions change. Field diagnostics condition change when the application
  changes the FD_SIMULATE.diagnostic_value or when a remote operator enables
  field diagnostics simulation.

PARAMETERS
  field_diag_value        Bitstring with the currently active
                          field diagnostics conditions

  simulation_active       TRUE -  field diagnostics simulation is enabled,
                                  the field_diag_value was set by an remote
                                  operator
                          FALSE - field diagnostics simulation is disabled,
                                  the field_diag_value reflects the currently
                                  active field diagnostics conditions
RETURN_VALUES
  An enumeration value that describes the action, what should be done to
  alleviate the condition or conditions.

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  USIGN16 recomm_act = FD_NO_RECOMM_ACT; /* Code 1 is specified in FF-912 */

FUNCTION_BODY

  /* check if it is simulation active */
  if ( simulation_active == TRUE ) /* mandatory in accordance with FF-912 */
  {
    return (FD_SIMULATE_RECOMM_ACT);
  }

  /* Set the recommended action in case that there is no HART communication */
  if (appl_get_comm_state() == DEV_COMM_NOT_OK)
  {
    return (FD_NO_HART_COMMUNICATION);
  }

  if ( field_diag_value & 0x7FC00000ul ) /* is at least one bit from bit#30 ... bit#22 set? */
  {
#if 0
  USIGN8  index = 0;
    /* loop for all diagnosis conditions - start searching with highest priority group for diagnosis number 30 */
    while ( index < SIZEOF_DIAG_ALARM_SOURCES )
    {
      if ( faultCode[diag_alarm_sources[index].byte] & ( (USIGN8) 1 << diag_alarm_sources[index].bit ) )
      {
        recomm_act = diag_alarm_sources[index].recommended_action;
        break;  /* since the faults are ordered with decreasing priority in diag_alarm_sources the loop can exit at first match */
      }
      index++;
    }
#endif
    recomm_act = mnptb_GetTBRecommendedAction();
  }
  /* check TB mode diagnosis - mandatory in accordance with FF-912 */
  else if ( field_diag_value & 0x00000001ul  ) /* is bit #0 set? */
  {
    recomm_act = FD_RECOMM_ACT_TB;
  }
  /* check for unknown diagnosis error sources */
  else if ( field_diag_value != 0 )
  {
    recomm_act = FD_RECOMM_ACT_UNKNOWN;
  }

  return recomm_act;
}





FUNCTION PUBLIC VOID appl_dia_set_all_fd_conditions
  (
    IN USIGN32  diagnostic_value
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function is an example intended to show how the application may set and
  reset field diagnostics conditions.

  This function is intended to be called by the application when one or
  multiple field diagnostics conditions occurs or clears.

  If the field diagnostics alarms are configured properly each appearance or
  disappearance of a field diagnostics condition will trigger an alarm-active
  or an alarm-clear message resp.

  The field diagnostics value will be stored in the resource block’s
  FD_SIMULATE.diagnostic_value parameter. Restart-with-defaults reset the
  FD_SIMULATE parameter and all field diagnostics alarms become clear. After
  Restart-with-defaults the application is required to call this function
  again in order to provide the FFBL component with the currently active
  field diagnostics conditions.

PARAMETERS
  diagnostic_value    Contains all currently active field diagnotics
                      conditions
RETURN_VALUES
  None

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  /* Update field diagnostics value --------------------------------------- */
  osif_disable_all_tasks();

  appl_diagnostic_value = diagnostic_value;

  osif_enable_all_tasks();

  return;
}





FUNCTION PUBLIC VOID appl_dia_set_one_fd_cond
  (
    IN USIGN8  condition_number
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function is an example intended to show how the application may set a
  single field diagnostics condition.

  If the field diagnostics alarms are configured properly each appearance of
  a field diagnostics condition will trigger an alarm-active message.

  The field diagnostics value will be stored in the resource block’s
  FD_SIMULATE.diagnostic_value parameter. Restart-with-defaults reset the
  FD_SIMULATE parameter and all field diagnostics alarms become clear. After
  Restart-with-defaults the application is required to call this function
  again in order to provide the FFBL component with the currently active
  field diagnostics conditions.

PARAMETERS
  condition_number  0 is the first condition; 31 is the last condition

RETURN_VALUES
  None

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  _ASSERT (condition_number < 32);

  /* Update field diagnostics value --------------------------------------- */
  osif_disable_all_tasks();

  appl_diagnostic_value |= (USIGN32)(0x0001UL << condition_number);

  osif_enable_all_tasks();

  return;
}





FUNCTION PUBLIC VOID appl_dia_reset_one_fd_cond
  (
    IN USIGN8  condition_number
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function is an example intended to show how the application may reset a
  single field diagnostics condition.

  If the field diagnostics alarms are configured properly each disappearance of
  a field diagnostics condition will trigger an alarm-clear message.

PARAMETERS
  condition_number  0 is the first condition; 31 is the last condition

RETURN_VALUES
  None

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  _ASSERT (condition_number < 32);

  /* Update field diagnostics value --------------------------------------- */
  osif_disable_all_tasks();

  appl_diagnostic_value &= (USIGN32)~(0x0001UL << condition_number);

  osif_enable_all_tasks();

  return;
}








FUNCTION LOCAL VOID appl_dia_check_function (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  FF-912 requires the Check Function bit (bit 0) to be set if for any of the
  transducer blocks the Check Function is true.

  This function scans all transducer blocks. If a transducer block is found
  that is unable to run in its normal mode the Check Function bit in the
  diagnostic value is set.

PARAMETERS

RETURN_VALUES

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  USIGN16 check_function; /* Index of transducer block */

FUNCTION_BODY


  if (appl_dia_check_fct.index != 0)
  {
    /* During the last scan a transducer block with active Check Function was found */

    check_function = appl_dia_check_transducer_block (appl_dia_check_fct.block_id);

    if (check_function == 0)
    {
      /* Check Function has cleared for this block ------------------------ */

      appl_dia_reset_one_fd_cond (0);           /* Clear Check Function bit */

      appl_dia_check_fct.index = 0;
    }

    return;
  }


  /* Scan all transducer blocks ------------------------------------------- */

  check_function = 0;


  if (check_function = appl_dia_check_transducer_block (ID_PTB_1))
  {
    appl_dia_check_fct.index    = check_function;
    appl_dia_check_fct.block_id = ID_PTB_1;
  }


  if (check_function)
  {
    /* Check Function appeared -------------------------------------------- */

    appl_dia_set_one_fd_cond (0);                 /* Set Check Function bit */


    /* Update alarm source array ------------------------------------------ */
    fd_alarm_source[0] = check_function;


    /* Load updated alarm source array ------------------------------------ */
    ffbl_load_fdiag_sources ((USIGN16 const *)fd_alarm_source);
  }

  return;
}






FUNCTION LOCAL USIGN16 appl_dia_check_transducer_block
  (
      USIGN8   block_id
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  Currently (Jan 2012) the Condition is still under discussion. The latest
  proposal which is going to be inserted into the next revision of the Field
  Diagnostics specification is:

  The Check condition is true if the Normal mode is AUTO and the Target mode
  is not AUTO. Or the Actual mode is LO and the Normal mode is not OOS.

PARAMETERS
  block_id                        As generated by GenVFD

RETURN_VALUES
  Start index of the block        If check condition is true
  0                               If check condition is not true

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  USIGN16                     check_function;
  const T_FBIF_BLOCK_DESCR *  p_block_desc;
  T_APPL_TB *                 p_generic_tb;

FUNCTION_BODY

  check_function = 0;
  p_block_desc   = fbs_get_block_inst(block_id)->p_block_desc;

  if (p_block_desc != NULL)
  {
    /* Check transducer block mode ---------------------------------------- */

    p_generic_tb = (T_APPL_TB *)p_block_desc->p_block;

    if ((p_generic_tb->mode_blk.normal == MODE_AUTO) &&
        (p_generic_tb->mode_blk.target != MODE_AUTO)    )
    {
      check_function = p_block_desc->start_index;
    }

    /* This condition refers to positioner transducer blocks only --------- */

    else if ((p_generic_tb->mode_blk.normal != MODE_OS) &&
             (p_generic_tb->mode_blk.actual == MODE_LO)    )
    {
      check_function = p_block_desc->start_index;
    }
  }

  return check_function;
}

#endif /* USE_FIELD_DIAGNOSTICS */
