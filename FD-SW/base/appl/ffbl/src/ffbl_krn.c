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

/**
    \file ffbl_krn.c.
    \brief Interface between SOFTING files and custom specific files.

*/

/* ===========================================================================

FILE_NAME          ffbl_krn.c



FUNCTIONAL_MODULE_DESCRIPTION

Softing delopped a simple PID control algorithm for the PID FB-Interface. This
algorithm serves only to verify the correct functionality of the PID function
block and for demonstration issues. The kernel function "pid_control" may be
replaced easily by any more complex control algorithm.

=========================================================================== */
  #include "keywords.h"
  #define  MODULE_ID      (COMP_FFBL + MOD_FBLKKRN)

INCLUDES
  #include <math.h>

  #include "base.h"
  #include "except.h"
  #include "vfd.h"
  #include "fbap.h"
  #include "ffbl_int.h"
  #include "ffbl_pid.h"
  #include "..\..\..\..\target\appl\fdev\inc\controller_pid.h"

LOCAL_DEFINES

LOCAL_TYPEDEFS

IMPORT_DATA

EXPORT_DATA

LOCAL_DATA



/****************************************************************************/
/******************************************************************************/
/*! \brief   pid algorithm
*   \param   p_pidfb points to the FF parameters
*   \param   p_pidfb_data points to dynamic data
*   \param   p_control_data points to control data
*   \return  float the calculated PID value
*
*   This function only calls the PID algorithm.
*******************************************************************************/

FUNCTION PUBLIC FLOAT pid_control(T_PIDFB *p_pidfb, T_PIDFB_DATA * p_pidfb_data, T_PIDFB_CONTROL_DATA * p_control_data)
{
LOCAL_VARIABLES
  FLOAT   out_value;

FUNCTION_BODY

    /* *************************************** */
    /*  begin additional function for the PID  */

    out_value = pid_algo(p_pidfb_data, p_pidfb, p_control_data);

    /*   end additional function for the PID   */
    /* *************************************** */
 
  return (out_value);
}


  /* *************************************** */
  /*  begin additional function for the PID  */
/******************************************************************************/
/*! \brief   re-calculations of parameters
*   \param   p_pidfb_data points to dynamic data
*   \param   p_pidfb points to the FF parameters
*   \return  void
*
*   This function sends the parameters to the calculations.
*******************************************************************************/
 FUNCTION PUBLIC VOID pid_re_calc
  (
    IN T_PIDFB *                p_pidfb,
    IN T_PIDFB_DATA *           p_pidfb_data
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

    pid_prm(p_pidfb_data, p_pidfb);
    
  return;
}
 
 /*   end additional function for the PID   */
 /* *************************************** */


 
 
 
 
 
 
 
 
 
