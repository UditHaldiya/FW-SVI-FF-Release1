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

FILE_NAME     appl_int.h


=========================================================================== */

#ifndef __APPL_INT_H__
#define __APPL_INT_H__


  #define APPL_DEVICE_ID            EEP_APPL_BLK1 /* Application uses 1 EEPROM block by default */

  #define SIZEOF_DIAG_ALARM_SOURCES 59

  typedef struct
  {
    USIGN32   manufacturer_id;
    USIGN16   device_type;
    USIGN8    device_revision;
    USIGN8    dd_revision;
    USIGN16   config_revision;
  }T_APPL_DEVICE_ID;

  typedef struct _T_DIAG_ALARM_SOURCES
  {
    USIGN8   diag_alarm_number;
    USIGN8   byte;
    USIGN8   bit;
    USIGN8   recommended_action;
    USIGN8   priority;
  } T_DIAG_ALARM_SOURCES;

   /* Positioner TB Channels */
#if 0
  #define SETPOINT_AOFB_CHANNEL                   1
  #define SUPPLY_PRESSURE_AIFB_CHANNEL            2
  #define ACTUATOR_PRESSURE_AIFB_CHANNEL          3
  #define SETPOINT_DOINTFB_CHANNEL                4
  #define SETPOINT_DOBOOLFB_CHANNEL               5
#else
  #define SETPOINT_AOFB_CHANNEL                   CH_POSITION
  #define SUPPLY_PRESSURE_AIFB_CHANNEL            CH_SUPPLY_PRESSURE
  #define ACTUATOR_PRESSURE_AIFB_CHANNEL          CH_ACTUATOR_PRESSURE_1
  #define SETPOINT_DOINTFB_CHANNEL                CH_POSITION_POSITION
  #define SETPOINT_DOBOOLFB_CHANNEL               CH_POSITION_OPEN_CLOSE
#endif

// Setpoint select
#define SP_SELECT_AOFB                  (1)
#define SP_SELECT_DOBOOL                (2)
#define SP_SELECT_DOINT                 (3)

/*****************************************************************************/
/* Public Function Declarations of Module appl_if.c                          */
/*****************************************************************************/

FUNCTION PUBLIC VOID appl_if_modify_config_rev (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function used to implement a non-standard reset of the non-volatile
  memory.

  It increments the Config Revision, which is part of the device identifier,
  and stores the modified value in the non-volatile memory.

PARAMETERS
  None

RETURN_VALUES
  None
----------------------------------------------------------------------------*/
;


/*****************************************************************************/
/* Public Function Declarations of Module appl_trn.c                         */
/*****************************************************************************/

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
;


//#if (RES_VAL_DEV_TYPE == 0x00FE) || (RES_VAL_DEV_TYPE == 0x00FF)

/* Softing's FBK-Start application ---------------------------------------- */

/* Current state of HART / MODB communication */
#define DEV_COMM_NOT_OK               0
#define DEV_COMM_OK                   1


FUNCTION PUBLIC USIGN8 appl_get_comm_state (VOID)

/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
  Provide current state of HART / MODB communication to caller.

PARAMETERS:

GLOBALS_AFFECTED:
  none

RETURN_VALUES:
  DEV_COMM_OK
  DEV_COMM_NOT_OK

------------------------------------------------------------------------------*/
;

//#endif /* FBK-Start application */



/*****************************************************************************/
/* Public Function Declarations of Module appl_dia.c                         */
/*****************************************************************************/

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
;


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
;


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
;


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
;


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
;






/*****************************************************************************/
/* Public Function Declarations of Module appl_nrv.c                         */
/*****************************************************************************/

FUNCTION PUBLIC VOID appl_nrv_init (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

PARAMETERS
  None

RETURN_VALUES
  None

----------------------------------------------------------------------------*/
;




/*@Dresser project modified */
FUNCTION extern GLOBAL VOID get_sm_config_data
(
  USIGN8  * node_addr,
  STRINGV * pd_tag,
  STRINGV * dev_id
)
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  Access function to provide the Node Address, PD Tag and Device ID to the application.

PARAMETERS
  node_addr:  current node address
  pd_tag:     pointer to 32byte STRINGV array for current PD Tag
  dev_id:     pointer to 32byte STRINGV array for current Device ID

RETURN_VALUES
  none

----------------------------------------------------------------------------*/
;



#ifdef SW_DOWNLOAD /* ------------------------------------------------------ */

/*****************************************************************************/
/* Public Function Declarations of Module appl_dlh.c                         */
/*****************************************************************************/

FUNCTION PUBLIC VOID appl_dhl_init (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

PARAMETERS
  None

RETURN_VALUES
  None

----------------------------------------------------------------------------*/
;

FUNCTION PUBLIC VOID appl_dlh_start (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

PARAMETERS
  None

RETURN_VALUES
  None

----------------------------------------------------------------------------*/
;

#endif /* SW_DOWNLOAD */

#endif /* __APPL_INT_H__ */
