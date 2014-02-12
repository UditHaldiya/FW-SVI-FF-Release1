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

FILE_NAME          ffbl_api.h



DESCRIPTION
  This file contains macros, definitions and declarations of functions
  that describe the interface between application layer (APPL) and FF function
  block layer (FFBL).
  The focus of this interface is the acyclic read and write access to
  block parameter during the operational phase.

=========================================================================== */
#ifndef __FFBL_API_H__
#define __FFBL_API_H__

#include "fbap.h"
#include "fbif.h"

/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    Interface Functions of the Application Layer                          */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/

/* The application provides the FF function block layer with two
   callback functions. The following structure of function pointers makes
   the callback functions available to the FF function block layer
*/

/* ------------------------------------------------------------------------ */
/*    Type Definitions                                                      */
/* ------------------------------------------------------------------------ */
  typedef enum
  {
    CHANNEL_UNIT_OK,
    INVALID_ENGINEERING_UNIT,
    INVALID_CHANNEL_UNIT
  }
    T_CHAN_UNIT_CHECK;
  /* ------------------------------------------------------------------------
  TYPE_DESCRIPTION
    Return values of the check-channel-unit routine.
  ------------------------------------------------------------------------- */


  typedef struct
  {
    GLOBAL T_CHAN_UNIT_CHECK (* a_of_chan_unit_check)   (USIGN16, SCALE *);

    #ifdef USE_FIELD_DIAGNOSTICS /* ---------------------------------------- */
    GLOBAL USIGN16 (* a_of_fd_get_recomm_act) (USIGN32, BOOL);
    GLOBAL USIGN32 (* a_of_fd_get_diag_value) (VOID);
    #endif /* USE_FIELD_DIAGNOSTICS */
  }
    T_FFBL_CALL_FDEV_FUNCT;

  /* -------------------------------------------------------------------------
  TYPE_DESCRIPTION
    There is a global variable 'ffbl_call_pdev_funct' of this type. This global
    variable has to be used by the application to provide interface functions to
    the function block layer.

    The interface function are as follows:

    --------------------------------------------------------------------------
    --------------------------------------------------------------------------

    FUNCTION GLOBAL T_CHAN_UNIT_CHECK (* a_of_chan_unit_check)
      (
        IN USIGN16   channel,
        IN SCALE   * p_xd_scale
      )

    FUNCTIONAL_DESCRIPTION
      This function is called by Analog Input and Analog Output blocks during the
      device startup phase and each time the CHANNEL or the unit of the XD_SCALE
      parameter is written. It checks whether the CHANNEL matches the XD_SCALE unit.
      If there is a mismatch it returns with a negative result. In this case the
      calling function block will indicate a configuration error and stay in
      Out-of-Service.

      It is assumed that each device has a set of supported engineering units and
      that a subset of the supported engineering units is valid for each channel.
      a_of_chan_unit_check() validates the combination of channel and engineering
      unit. It has two sections:

      1) It checks if the engineering unit is supported
      2) It checks if the engineering unit is valid for the selected channel

    PARAMETERS
      channel                   The channel that sould be combined with an
                                engineering unit
      p_xd_scale                Pointer to the XD_SCALE parameter that should be
                                combined with a channel

    RETURN_VALUES
      CHANNEL_UNIT_OK           The combination of channel and engineering unit
                                is supported
      INVALID_ENGINEERING_UNIT  The engineering unit is not supported
      INVALID_CHANNEL_UNIT      The combination of channel and engineering unit
                                is not valid

    --------------------------------------------------------------------------
    --------------------------------------------------------------------------

    FUNCTION GLOBAL USIGN16 (* a_of_fd_get_recomm_act)
      (
      USIGN32           field_diag_value,
      BOOL              simulation_active
      )

    FUNCTIONAL_DESCRIPTION
      The resource block parameter FD_RECOMMEN_ACT is an enumerated summarization
      of the most severe condition or conditions detected. The DD should describe
      by enumerated action, what should be done to alleviate the condition or
      conditions. 0 is defined as ‘Not Initialized’, 1 is defined as
      ‘No Action Required’; all other values have to be defined device-specific.

      The FFBL component calls this function each time the field diagnostics
      conditions change. The field diagnostics condition may change when the
      application changes the diagnostic value of the FD_SIMULATE parameter or
      when a remote operator enables field diagnostics simulation.

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

  ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------ */
/*   Global Variable for Application Interface Functions                    */
/* ------------------------------------------------------------------------ */
#include "da_ffbl.h"
  NO_INIT extern T_FFBL_CALL_FDEV_FUNCT   ffbl_call_fdev_funct;
#include "da_def.h"




/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    Functions Provided by the FF Function Block Layer (FFBL)              */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/

/* ------------------------------------------------------------------------ */
/*    Type Definitions                                                      */
/* ------------------------------------------------------------------------ */
  typedef enum
  {
    NO_BLOCK_ALARM,
    ACTIVATE_BLOCK_ALARM,
    CLEAR_BLOCK_ALARM
  }
    T_ALARM_ACTION;


  typedef enum
  {
    res_err_static_data_lost,
    res_err_dev_needs_maint_soon,
    res_err_dev_needs_maint_now,
    res_err_other
  }
    T_RESOURCE_ERROR;



/* ------------------------------------------------------------------------ */
/*    Function Prototypes                                                   */
/* ------------------------------------------------------------------------ */

FUNCTION GLOBAL USIGN16 ffbl_check_mode
  (
    IN MODE *    mode_ptr,
    IN USIGN8    subidx,
    IN USIGN8 *  source,
    IN USIGN8    valid_modes
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function may be used to check if a write access to the MODE_BLK para-
  meter is valid.

PARAMETERS
  mode_ptr          Pointer to the MODE_BLK parameter
  subidx            Subindex to be written
  source            Pointer to the new value
  valid_modes       Bitmask of valid modes.

RETURN_VALUES
  E_OK              The new mode value is valid
  E_FB_PARA_CHECK   The new mode value is not valid
----------------------------------------------------------------------------*/
;






FUNCTION GLOBAL VOID ffbl_generate_RESB_alarm
  (
    IN T_ALARM_ACTION   alarm,
    IN T_RESOURCE_ERROR error
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function allows to set/clear an error bit in the BLOCK_ERR parameter of
  the resource block. If necessary a block alarm message is sent to the host
  system.

PARAMETERS
  alarm               Set or clear the alarm
  error               Error code

RETURN_VALUES
  None
----------------------------------------------------------------------------*/
;





FUNCTION GLOBAL USIGN16 ffbl_check_unack_flag
  (
    IN USIGN8 *       p_alarm_unack,
    IN USIGN8 *       p_acknowledge,
    IN USIGN8         subindex
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function is called when the Unacknowledged flag of a alert parameter is
  written. It checks if the write access is valid.

PARAMETERS
  p_alarm_unack       Pointer to the alert parameter
  p_acknowledge       Pointer to the location where the acknowledge value
                      is buffered
  subindex            Subindex used in the write indication

RETURN_VALUES
  E_OK                Write access is accepted; alert is acknowledged
  E_FB_ALM_ACK        Alert is already acknowledged
  E_FB_DATA_NO_WRITE  Subindex is not valid
----------------------------------------------------------------------------*/
;



FUNCTION GLOBAL USIGN16 ffbl_check_block_alarm_unack_flag
  (
    IN T_FBIF_BLOCK_INSTANCE *    p_block_instance,
    IN USIGN8 *                   p_alarm_unack,
    IN USIGN8 *                   p_acknowledge,
    IN USIGN8                     subindex
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function is called when the Unacknowledged flag of a block alarm
  parameter is written. It checks if the write access is valid.

  Note: Due to modifications in FF-890 FS 1.8 and ITK 5.2 it is not necessary
  anymore to handle the alarm unack flag of block alarms in a different way as
  the unack flag of other alarms. Therefore it is not necessary anymore to use
  a separate function for the handling of a block alarm’s unack flag

  This function is just available to keep the compatibility with the FFBL
  interface of the FF-FD-SW 2.2x.

PARAMETERS
  p_block_instance    Pointer to the block whose block alarm should be
                      acknowledged
  p_alarm_unack       Pointer to the block alarm parameter
  p_acknowledge       Pointer to the location where the acknowledge value
                      is buffered
  subindex            Subindex used in the write indication

RETURN_VALUES
  E_OK                Write access is accepted; block alarm is acknowledged
  E_FB_ALM_ACK        Block alarm is already acknowledged
  E_FB_DATA_NO_WRITE  Subindex is not valid
----------------------------------------------------------------------------*/
;



FUNCTION GLOBAL VOID ffbl_update_block_alarm
  (
    IN USIGN16            block_id,
    IN USIGN16            blk_alm_idx,
    IN USIGN16            alarm_bits,
    IN USIGN16 *          old_alarm_bits
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function is used to update the block alarm parameter. If there is a change
  in the block error state this function will generate an alert notification.

PARAMETERS
  block_id          Block identfier as generated by GenVFD (defined in fbif_idx.h)
  blk_alm_idx       Relative index of the block alarm parameter within the block.
                    (The relative index of the block alarm parameter is calculated
                    by GenVFD.)
  alarm_bits        Current state of the block error parameter
  old_alarm_bits    Previous state of the block error parameter

RETURN_VALUES
  None
----------------------------------------------------------------------------*/
;




FUNCTION GLOBAL VOID ffbl_disable_update_events
  (
    IN EVENT *  p_update_evt
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  According to the FF specifications no update events are sent if a block is in
  mode out-of-service. As the update events are generated by the function block
  shell and the function block shell has no knowledge about the block state,
  application layer and function block layer have to disable and enable the
  generation of update events by interface functions.

  This function is called when a block goes to Out-of-Service. As after power-on
  a block is usually in Out-of-Service the function is also called by the start
  function of each block.

PARAMETERS
  p_update_evt      Pointer to the update event parameter

RETURN_VALUES
  None
----------------------------------------------------------------------------*/
;





FUNCTION GLOBAL VOID ffbl_enable_update_events
  (
    IN USIGN16  block_id,
    IN USIGN16  update_evt_idx,
    IN EVENT *  update_evt,
    IN USIGN16  static_revision
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  According to the FF specifications no update events are sent if a block is in
  mode out-of-service.  As the update events are generated by the function block
  shell and the function block shell has no knowledge about the block state,
  application layer and function block layer have to disable and enable the
  generation of update events by interface functions.

  This function is called when a block goes out of Out-of-Service. The function
  block shell will send an update event if a static parameter has been changed
  while the block was Out-of-Service.

PARAMETERS
  block_id          Block identfier as generated by GenVFD (defined in fbif_idx.h)
  update_evt_idx    Relative index of the update event parameter within the block.
                    (The relative index of the update event parameter is calculated
                    by GenVFD.)
  update_evt        Pointer to the update event
  static_revision   Current value of the static revision parameter

RETURN_VALUES
  None
----------------------------------------------------------------------------*/
;





#ifdef USE_FIELD_DIAGNOSTICS /* -------------------------------------------- */

FUNCTION GLOBAL VOID ffbl_load_fdiag_sources
  (
    IN USIGN16 const *   fdiag_sources
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  Field diagnostics alerts have got an element ‘Source Block Index’.
  ‘Source Block Index’ is the OD start index of the block that triggered the
  alert; it is zero if the alert does not relate to a certain block.

  This function allows the application to provide an array of 32 ‘Source Block
  Indexes’ for 32 field diagnostics conditions. The first element of the array
  relates to field diagnostics condition 0; the last element of the array
  relates to field diagnostics condition 31.

  The application must not call this function before fbs_start() has been
  executed.

  The Restart-with-defaults command reset the internal FFBL array of ‘Source
  Block Indexes’ to zero. After Restart-with-defaults the application has to
  re-load an array with 32 ‘Source Block Indexes’.

  The application is allowed to call this function more than one time. The FFBL
  component will always work with a copy of the newest array.

  The call of this function is optional. If the application does not call this
  function the FFBL component will use zero as ‘Source Block Index’ for all
  field diagnostics conditions.

PARAMETERS
  fdiag_sources   Pointer to an array of 32 ‘Source Block Indexes’.
                  The FFBL component works with an internal copy of the ‘Source
                  Block Index’ array. When this function returns the calling
                  function may free the buffer where fdiag_sources points to.

RETURN
  None

----------------------------------------------------------------------------*/
;

#endif /* USE_FIELD_DIAGNOSTICS */







#ifdef IT_BLOCK_AVAILABLE /* ---------------------------------------------- */

FUNCTION GLOBAL USIGN16 ffbl_it_get_extra_nv_ram_size (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  The function is used to return the size of the extra NV RAM used by
  the IT function block.

PARAMETERS
  None

RETURN_VALUES
  size in bytes used of the extra NV RAM by the IT function block

----------------------------------------------------------------------------*/
;

#endif /* IT_BLOCK_AVAILABLE */


#endif /* __FFBL_API_H__ */



