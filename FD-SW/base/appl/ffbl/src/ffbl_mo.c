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
*                     Copyright (C) SOFTING IA GmbH 2013                     *
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
* PROJECT_NAME             Softing FF/PA FD 2.42.A                           *
*                                                                            *
* VERSION                  FF - 2.42.A                                       *
*                          PA - 2.42.A                                       *
*                                                                            *
* DATE                     14. Aug. 2013                                     *
*                                                                            *
*****************************************************************************/


/* ===========================================================================

FILE_NAME          ffbl_mo.c



FUNCTIONAL_MODULE_DESCRIPTION

=========================================================================== */
  #include "keywords.h"
  #define  MODULE_ID      (COMP_FFBL + MOD_FBLKMO)

INCLUDES
  #include <math.h>

  #include "base.h"
  #include "except.h"
  #include "vfd.h"
  #include "fbap.h"
  #include "ffbl_int.h"
  #include "ffbl_res.h"
  #include "ffbl_mo.h"

LOCAL_DEFINES
  #define PID_DOWNSTREAM_NOT_ACTIVE 0x00
  #define PID_DOWNSTREAM_INIT       0x03
  #define PID_DOWNSTREAM_ACTIVE     0x01

LOCAL_TYPEDEFS

FUNCTION_DECLARATIONS
  FUNCTION LOCAL VOID Return_in_manual_mode (USIGN8 actual_mode);
  FUNCTION LOCAL VOID Update_fault_state (VOID);
  FUNCTION LOCAL VOID Check_shedding_condition (USIGN8 rmode_in_status,USIGN8 * rmode_out_status,USIGN32 shed_timeout);
  FUNCTION LOCAL USIGN8 Initialize_cascade (USIGN8 cas_in_status,USIGN8 * cas_out_status);
  FUNCTION LOCAL VOID Check_current_fault_state (VOID);
  FUNCTION LOCAL VOID Check_fault_state_timeout (VOID);
  FUNCTION LOCAL USIGN8 Set_shedding_mode (USIGN8 permit);

IMPORT_DATA

EXPORT_DATA
#include "da_near.h"                             /* DATA SEGMENT DEFINITION */
  NO_INIT T_MO_DATA mo;
#include "da_def.h"                              /* DEFAULT DATA SEGMENT */

LOCAL_DATA


/****************************************************************************/

FUNCTION PUBLIC VOID Calc_block_mode (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  mo.new_target_mode = 0;

  if ( (p_resource->mode_blk.actual == MODE_OS) ||
       (mo.mode.target & MODE_OS)               )
  {
    mo.mode.actual = MODE_OS;

    return;
  }


  /* Target mode is not O/S ----------------------------------------------- */

  if (mo.profile == PID_PROFILE_NUMBER)
  {
    USIGN8 bkcal_in_status;

    /* Check tracking condition ------------------------------------------- */
    if (mo.control_opts & CNTRL_OPT_TRACK_ENABLE)
    {
      if (mo.trk_in_d.status & SQ_GOOD)
      {
        mo.tracking = mo.trk_in_d.value;  /* Store good tracking value */
      }
      else if ((mo.trk_in_d.status & QUALITY_MASK) == SQ_BAD)
      {
        if (mo.control_opts & CNTRL_OPT_TRACK_IF_BAD_TRK_IN_D)
        {
          mo.tracking = TRUE;
        }
      }
    }
    else
    {
      mo.tracking = FALSE;
    }

    /* Check if PID has to go to mode IMAN -------------------------------- */
    bkcal_in_status = mo.bkcal_in_status & (QUALITY_MASK|SUB_STATUS_MASK);

    if ( (bkcal_in_status != (SQ_GOOD_CAS|SUB_NON_SPECIFIC)) &&
        ((bkcal_in_status & QUALITY_MASK) != SQ_GOOD_NC)        )
    {
      if (bkcal_in_status == (SQ_GOOD_CAS|SUB_IR))
      {
        mo.pid_downstream = PID_DOWNSTREAM_INIT;

        Return_in_manual_mode (MODE_IMAN);
        return;
      }

      mo.pid_downstream = PID_DOWNSTREAM_NOT_ACTIVE;

      /* The following section is because of ITK 5.2, test case i1500_00:
         This test case requires the PID to switch to manual mode though
         the BKCAL_IN status is BAD                                         */
      if (mo.mode.target == MODE_MAN)
      {
        if ((bkcal_in_status & QUALITY_MASK) != SQ_BAD)
        {
          Return_in_manual_mode (MODE_IMAN);
          return;
        }
      }
      else  /* (mo.mode.target != MODE_MAN) */
      {
        Return_in_manual_mode (MODE_IMAN);
        return;
      }
    }
    else /* (bkcal_in_status == (SQ_GOOD_CAS|SUB_NON_SPECIFIC) ||
           ((bkcal_in_status & QUALITY_MASK) != SQ_GOOD_NC)        */
    {
      if (mo.pid_downstream == PID_DOWNSTREAM_NOT_ACTIVE)
      {
        mo.pid_downstream = PID_DOWNSTREAM_INIT;

        /* Don't switch to IMAN in this case (ITK 5.2) */
      }
    }


    /* Check if PID has to go to mode LO ---------------------------------- */
    if (mo.tracking)
    {
      if ((mo.trk_in_d.status & QUALITY_MASK) == SQ_BAD)
      {
        /* Apply status option 'Target to Man if BAD TRK_IN_D' ------------ */
        if (mo.status_opts & STATUS_OPT_TARG_MAN_IF_BAD_TRK_IN)
        {
          if (!(mo.mode.target & MODE_MAN))
          {
            mo.new_target_mode = MODE_MAN;
          }
        }
      }

      if ((mo.mode.target & MODE_MAN)||(mo.new_target_mode))
      {
        if (mo.control_opts & CNTRL_OPT_TRACK_IN_MANUAL)
        {
          Return_in_manual_mode (MODE_LO);

          return;
        }
        else
        {
          Return_in_manual_mode (MODE_MAN);

          return;
        }
      }
      else
      {
        Return_in_manual_mode (MODE_LO);

        return;
      }
    }
  } /* if (mo.profile == PID_PROFILE_NUMBER) */



  if ((mo.profile == AO_PROFILE_NUMBER)||(mo.profile == DO_PROFILE_NUMBER))
  {
    /* Check if the output block has to go to mode IMAN ------------------- */
    USIGN8 readback_status = mo.rout_in_status & (QUALITY_MASK|SUB_STATUS_MASK);

    if ((readback_status == (SQ_BAD|SUB_OUT_OF_SERVICE))||
        (readback_status == (SQ_BAD|SUB_DEVICE_FAILURE))  )
    {
      Return_in_manual_mode (MODE_IMAN);
      return;
    }

    /* Check if the output block has to go to mode LO --------------------- */
    if (readback_status == (SQ_GOOD_CAS|SUB_LO))
    {
      Return_in_manual_mode (MODE_LO);
      return;
    }


    /* Check if the output block has to go to mode LO --------------------- */

    Update_fault_state (); /* Check fault state conditions */

    if ((mo.fault.active)&&(p_resource->feature_sel & RES_FEAT_FAULT_STATE_SUPP))
    {
      if (mo.mode.actual != MODE_LO)
      {
        if (mo.io_opts & IO_OPT_TARGET_TO_MAN_IF_FSTATE)
        {
          mo.new_target_mode = MODE_MAN;
        }
      }

      Return_in_manual_mode (MODE_LO);
      return;
    }

  } /* if (mo.profile == AO_PROFILE_NUMBER|DO_PROFILE_NUMBER) */



  /* Target mode is not O/S and the block is not in state IMAN or LO ------ */

  if ((mo.mode.actual == MODE_OS)||(mo.mode.target & MODE_MAN))
  {
    Return_in_manual_mode (MODE_MAN);
    return;
  }


  /* Target mode is an automatic mode ------------------------------------- */

  if (mo.mode_was_written)
  {
    mo.mode.actual       = MODE_AUTO;
    mo.shed.active       = FALSE;
    mo.shed.current_time = 0;

    mo.rcas_out_status   = SQ_GOOD_CAS|SUB_NI;
    mo.cas_out_status    = SQ_GOOD_CAS|SUB_NI;
    mo.rout_out_status   = SQ_GOOD_CAS|SUB_NI;
  }
  else
  {
    if ((mo.cas_out_status & QUALITY_MASK) == SQ_BAD)
    {
      if (mo.mode.actual == MODE_CAS)
      {
        mo.cas_out_status = SQ_GOOD_CAS;
      }
      else
      {
        mo.cas_out_status = SQ_GOOD_CAS|SUB_NI;
      }
    }


    if ((mo.rcas_out_status & QUALITY_MASK) == SQ_BAD)
    {
      if (mo.mode.actual == MODE_RCAS)
      {
        mo.rcas_out_status = SQ_GOOD_CAS;
      }
      else
      {
        mo.rcas_out_status = SQ_GOOD_CAS|SUB_NI;
      }
    }
  }



  /* Calculate target mode ------------------------------------------------ */

  if (mo.profile == PID_PROFILE_NUMBER)
  {
    /* Try to go to ROUT mode ----------------------------------------------- */

    if (mo.mode.actual == MODE_ROUT)
    {
      /* Check shedding condition ------------------------------------------- */

      Check_shedding_condition
        (mo.rout_in_status,&mo.rout_out_status,p_resource->shed_rout);

      if (mo.shed.active) mo.rout_out_status = SQ_GOOD_CAS|SUB_NI;
    }

    if ((!mo.shed.active)&&(mo.mode.target & MODE_ROUT))
    {
      if (mo.mode.actual == MODE_ROUT)
      {
        mo.cas_out_status = SQ_GOOD_CAS|SUB_NI;

        return;
      }

      if (Initialize_cascade (mo.rout_in_status,&mo.rout_out_status))
      {
        mo.mode.actual = MODE_ROUT;

        return;
      }
    }
  } /* if (mo.profile == PID_PROFILE_NUMBER) */




  /* Try to go to RCAS mode ----------------------------------------------- */
  if (mo.mode.target & MODE_RCAS)
  {
    /* Check shedding condition ------------------------------------------- */
    Check_shedding_condition
      (mo.rcas_in_status,&mo.rcas_out_status,p_resource->shed_rcas);

    if (mo.shed.active) mo.rcas_out_status = SQ_GOOD_CAS|SUB_NI;

    if ((!mo.shed.active)&&(mo.mode.target & MODE_RCAS))
    {
      if (mo.mode.actual == MODE_RCAS)
      {
        mo.cas_out_status = SQ_GOOD_CAS|SUB_NI;

        return;
      }

      if (Initialize_cascade (mo.rcas_in_status,&mo.rcas_out_status))
      {
        mo.mode.actual = MODE_RCAS;

        return;
      }
    }
  }



  /* Try to go to CAS mode ------------------------------------------------ */

  if ( ((!mo.shed.active)&&(mo.mode.target & MODE_CAS)) ||
       (( mo.shed.active)&&(mo.shed.mode   & MODE_CAS))     )
  {
    if (mo.mode.actual == MODE_CAS)
    {
      if ((mo.cas_in_status & QUALITY_MASK) != SQ_BAD)
      {
        return;
      }

      else /* (mo.cas_in_status & QUALITY_MASK) == SQ_BAD) */
      {
        if (mo.profile == PID_PROFILE_NUMBER)
        {
          /* Apply status option 'Target to Auto if BAD CAS_IN' ----------- */

          if (mo.status_opts & STATUS_OPT_TARG_NEXT_IF_BAD_CAS_IN)
          {
            if (mo.mode.permit & MODE_AUTO)
            {
              mo.new_target_mode = MODE_AUTO;
            }
            else if (mo.mode.permit & MODE_MAN)
            {
              mo.new_target_mode = MODE_MAN;
            }
            else
            {
              mo.new_target_mode = MODE_OS;
            }
          }
        }

        else if ((mo.profile == AO_PROFILE_NUMBER)||(mo.profile == DO_PROFILE_NUMBER))
        {
          if (mo.fault.reason != FAULT_STATE_CLEAR)
          {
            /* A fault condition was detected but FSTATE_TIME isn't expired yet */

            return;
          }
        }

        /* Shed to next permitted mode ------------------------------------ */
        Return_in_manual_mode (MODE_AUTO);

        return;
      }
    }

    else /* (mo.mode.actual != MODE_CAS) */
    {

      if (Initialize_cascade (mo.cas_in_status,&mo.cas_out_status))
      {
        mo.mode.actual = MODE_CAS;

        return;
      }
    }
  } /* Try to go to CAS mode */



  /* Try to go to AUTO mode ----------------------------------------------- */

  if ( ((!mo.shed.active)&&(mo.mode.target & MODE_AUTO)) ||
       (( mo.shed.active)&&(mo.shed.mode   & MODE_AUTO))    )
  {
    mo.mode.actual = MODE_AUTO;

    return;
  }



  /* Try to go to MAN mode ------------------------------------------------ */

  if ( ((!mo.shed.active)&&(mo.mode.target & MODE_MAN)) ||
       (( mo.shed.active)&&(mo.shed.mode   & MODE_MAN))    )
  {
    mo.mode.actual = MODE_MAN;

    return;
  }


  /* Go to O/S mode ------------------------------------------------------- */

  mo.mode.actual = MODE_OS;

  return;
}






FUNCTION LOCAL VOID Update_fault_state (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  /* Don't look to limit bits during fault state checking ----------------- */

  mo.cas_in_status  &= QUALITY_MASK|SUB_STATUS_MASK;
  mo.rcas_in_status &= QUALITY_MASK|SUB_STATUS_MASK;


  /* Start fault state checking ------------------------------------------- */

  if (mo.fault.active)
  {
    /* Fault state is active, check the current situation ----------------- */

    Check_current_fault_state ();

    if (mo.fault.reason != FAULT_STATE_CLEAR) return; /* Fault state still active */

    /* The fault state condition has cleared ------------------------------ */

    mo.fault.active = FALSE;
  }


  /* Currently, fault state is not active, check the fault state conditions */

  if (p_resource->fault_state == FAULT_STATE_ACTIVE)
  {
    /* Fault state parameter of the resource block is set ----------------- */

    mo.fault.active       = TRUE;
    mo.fault.reason       = FAULT_BY_RESOURCE_STATE;
    mo.fault.no_of_cycles = 0;

    return; /* Now fault state is active */
  }


  if (mo.fault.no_of_cycles)
  {
    /* There is a fault state condition but the fault state timer isn't expired yet */

    Check_current_fault_state ();

    if (mo.fault.reason == FAULT_STATE_CLEAR)
    {
      /* The fault state conditon has cleared, stop fault state timer ----- */

      mo.fault.no_of_cycles = 0;
    }
    else
    {
      /* The fault state conditon is still active, check fault state time - */

      Check_fault_state_timeout ();

      return;
    }
  }


  /* Currently no fault state condition is detected, ----------------------
     check fault state conditions ----------------------------------------- */

  if ((mo.mode.target & MODE_CAS) &&
           (mo.cas_in_status == (SQ_BAD|SUB_NO_COMM_WITH_LUV)))
  {
    mo.fault.reason = FAULT_BY_CAS_NO_COMM;
  }

  else if ((mo.mode.target & MODE_CAS) &&
           (mo.cas_in_status == (SQ_GOOD_CAS|SUB_IFS)))
  {
    mo.fault.reason = FAULT_FORCED_BY_CAS_STATE;
  }

  else if ((mo.mode.target & MODE_RCAS) &&
           (mo.rcas_in_status == (SQ_GOOD_CAS|SUB_IFS)))
  {
    mo.fault.reason = FAULT_FORCED_BY_RCAS_STATE;
  }


  if (mo.fault.reason != FAULT_STATE_CLEAR)
  {
    /* A new fault state condition is detected, start fault state timer --- */

    mo.fault.no_of_cycles = 0;

    Check_fault_state_timeout();
  }

  return;
}





FUNCTION LOCAL VOID Check_current_fault_state (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  if (mo.fault.reason == FAULT_BY_RESOURCE_STATE)
  {
    if (p_resource->fault_state != FAULT_STATE_ACTIVE)
    {
      mo.fault.reason = FAULT_STATE_CLEAR;
    }
  }

  else if (mo.fault.reason == FAULT_BY_CAS_NO_COMM)
  {
    if ((!(mo.mode.target & MODE_CAS)) ||
        (mo.cas_in_status  != (SQ_BAD|SUB_NO_COMM_WITH_LUV)))
    {
      mo.fault.reason = FAULT_STATE_CLEAR;
    }
  }

  else if (mo.fault.reason == FAULT_FORCED_BY_CAS_STATE)
  {
    if ((!(mo.mode.target & MODE_CAS)) ||
        (mo.cas_in_status != (SQ_GOOD_CAS|SUB_IFS)))
    {
      mo.fault.reason = FAULT_STATE_CLEAR;
    }
  }

  else if (mo.fault.reason == FAULT_FORCED_BY_RCAS_STATE)
  {
    if ((!(mo.mode.target & MODE_RCAS)) ||
        (mo.rcas_in_status != (SQ_GOOD_CAS|SUB_IFS)))
    {
      mo.fault.reason = FAULT_STATE_CLEAR;
    }
  }

return;
}







FUNCTION LOCAL VOID Check_fault_state_timeout (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

  FLOAT current_fault_time;

FUNCTION_BODY

  current_fault_time = ++mo.fault.no_of_cycles * (mo.period_of_execution/32000.0);

  if (current_fault_time > mo.fstate_time)
  {
    /* Fault state timer is expired, now fault state is active ------------ */

    mo.fault.active       = TRUE;
    mo.fault.no_of_cycles = 0;
  }

  return;
}





FUNCTION LOCAL VOID Check_shedding_condition
  (
    IN USIGN8          rmode_in_status,
    IN USIGN8 *        rmode_out_status,
    IN USIGN32         shed_timeout
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  BOOL shedding;

FUNCTION_BODY

  if ((rmode_in_status & QUALITY_MASK) == SQ_BAD)
  {
    /* Shedding because of status of input parameter ---------------------- */

    shedding = TRUE;
  }

  else if (((rmode_in_status & QUALITY_MASK) == SQ_UNCERTAIN) &&
           ((rmode_in_status & SUB_STATUS_MASK) != SUB_SUBSTITUTE)   &&
           ((rmode_in_status & SUB_STATUS_MASK) != SUB_INITIAL_VALUE)  )
  {
    /* Shedding because of status of input parameter ---------------------- */

    shedding = TRUE;
  }


  else if (shed_timeout == 0)
  {
    /* No shedding because of disabled shed timer ------------------------- */

    shedding = FALSE;
  }

  else
  {
    /* Check and update shedding timer ------------------------------------ */

    if (mo.shed.current_time > (0xFFFFFFFF - mo.period_of_execution))
    {
      shedding = TRUE;
    }
    else
    {
      mo.shed.current_time += mo.period_of_execution;

      if (mo.shed.current_time > shed_timeout)
      {
        shedding = TRUE;
      }
      else
      {
        shedding = FALSE;
      }
    }
  }


  /* Shedding condition detected ------------------------------------------ */

  if (shedding && mo.shed.active)
  {
    /* Shedding condition is still active -------------------------------- */

    return;
  }


  if (shedding)  /* && !mo.shed.active                                      */
  {
    /* Shedding condition is detected now --------------------------------- */

    *rmode_out_status = SQ_GOOD_CAS|SUB_NI;

    if ((mo.shed_opt == 0)||(mo.shed_opt == SHED_NORMAL_NORMAL_RETURN))
    {
      mo.shed.mode = Set_shedding_mode (mo.mode.permit);

      mo.shed.active = TRUE;
    }

    else if (mo.shed_opt == SHED_NORMAL_NO_RETURN)
    {
      mo.mode.target = Set_shedding_mode (mo.mode.permit);

      mo.new_target_mode = mo.mode.target;
    }

    else if (mo.shed_opt == SHED_TO_AUTO_NORMAL_RETURN)
    {
      mo.shed.mode = Set_shedding_mode (mo.mode.permit & ~MODE_CAS);

      mo.shed.active = TRUE;
    }

    else if (mo.shed_opt == SHED_TO_AUTO_NO_RETURN)
    {
      mo.mode.target = Set_shedding_mode (mo.mode.permit & ~MODE_CAS);

      mo.new_target_mode = mo.mode.target;
    }

    else if (mo.shed_opt == SHED_TO_MAN_NORMAL_RETURN)
    {
      mo.shed.mode = Set_shedding_mode (mo.mode.permit & ~(MODE_AUTO|MODE_CAS));

      mo.shed.active = TRUE;
    }

    else if (mo.shed_opt == SHED_TO_MAN_NO_RETURN)
    {
      mo.mode.target = Set_shedding_mode (mo.mode.permit & ~(MODE_AUTO|MODE_CAS));

      mo.new_target_mode = mo.mode.target;
    }

    else if (mo.shed_opt == SHED_TO_RETAINED_NO_RETURN)
    {
      USIGN8 retained_target = mo.mode.target;

      mo.mode.target  = Set_shedding_mode (mo.mode.permit);

      mo.mode.target &= retained_target;

      mo.new_target_mode = mo.mode.target;
    }

    else if (mo.shed_opt == SHED_TO_RETAINED_NORMAL_RETURN)
    {
      mo.shed.mode   = Set_shedding_mode (mo.mode.permit);

      mo.shed.mode  &= mo.mode.target;

      mo.shed.active = TRUE;
    }
  }
}





FUNCTION LOCAL USIGN8 Set_shedding_mode
  (
    IN USIGN8   permit
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  USIGN8  shed_mode;

FUNCTION_BODY

  shed_mode = ((MODE_AUTO|MODE_CAS) & permit);

  if (!shed_mode) shed_mode = (MODE_MAN & permit);

  if (!shed_mode) shed_mode = MODE_OS;

  return (shed_mode);
}






FUNCTION LOCAL USIGN8 Initialize_cascade
  (
    IN    USIGN8     cas_in_status,
    INOUT USIGN8 *   cas_out_status
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  if ((cas_in_status & QUALITY_MASK) == SQ_GOOD_CAS)
  {
    if ((cas_in_status & SUB_STATUS_MASK) == SUB_NON_SPECIFIC)
    {
      *cas_out_status = SQ_GOOD_CAS|SUB_IR;

      return (FALSE);
    }

    else if ((cas_in_status & SUB_STATUS_MASK) == SUB_IA)
    {
      if ((*cas_out_status & SUB_STATUS_MASK) == SUB_IR)
      {
        *cas_out_status = SQ_GOOD_CAS;

        return (TRUE);
      }
      else
      {
        *cas_out_status = SQ_GOOD_CAS|SUB_NI;

        return (FALSE);
      }
    }

    else /* (SUBSTATUS != SUB_NON_SPECIFIC and != SUB_IA) */
    {
      *cas_out_status = SQ_GOOD_CAS|SUB_NI;

      return (FALSE);
    }
  }

  else if ((cas_in_status & QUALITY_MASK) == SQ_GOOD_NC)
  {
    *cas_out_status = SQ_GOOD_CAS;

    return (TRUE);
  }

  else if ((cas_in_status & QUALITY_MASK) == SQ_UNCERTAIN)
  {
    if (((cas_in_status & SUB_STATUS_MASK) == SUB_SUBSTITUTE)   ||
        ((cas_in_status & SUB_STATUS_MASK) == SUB_INITIAL_VALUE)  )
    {
      *cas_out_status = SQ_GOOD_CAS;

      return (TRUE);
    }
    else
    {
      *cas_out_status = SQ_GOOD_CAS|SUB_NI;

      return (FALSE);
    }
  }

  else /* ((cas_in_status & QUALITY_MASK) ==  SQ_BAD) */
  {
    *cas_out_status = SQ_GOOD_CAS|SUB_NI;

    return (FALSE);
  }
}






FUNCTION LOCAL VOID Return_in_manual_mode
  (
    IN USIGN8 actual_mode
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  mo.rout_out_status = SQ_GOOD_CAS|SUB_NI;
  mo.rcas_out_status = SQ_GOOD_CAS|SUB_NI;
  mo.cas_out_status  = SQ_GOOD_CAS|SUB_NI;

  mo.mode_was_written = FALSE;

  if (actual_mode == MODE_AUTO)
  {
    if (mo.mode.permit & MODE_AUTO)
    {
      mo.mode.actual = MODE_AUTO;
    }
    else if (mo.mode.permit & MODE_MAN)
    {
      mo.mode.actual = MODE_MAN;
    }
    else
    {
      mo.mode.actual = MODE_OS;
    }
  }
  else if (actual_mode == MODE_MAN)
  {
    if (mo.mode.permit & MODE_MAN)
    {
      mo.mode.actual = MODE_MAN;
    }
    else
    {
      mo.mode.actual = MODE_OS;
    }
  }
  else
  {
    mo.mode.actual = actual_mode;
  }

  return;
}

