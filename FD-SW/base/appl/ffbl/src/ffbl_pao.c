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

FILE_NAME          ffbl_pao.c



FUNCTIONAL_MODULE_DESCRIPTION

=========================================================================== */
  #include "keywords.h"
  #define  MODULE_ID      (COMP_FFBL + MOD_FBLKPAO)

INCLUDES
  #include "base.h"
  #include "fbap.h"
  #include "ffbl_int.h"

LOCAL_DEFINES

LOCAL_TYPEDEFS

FUNCTION_DECLARATIONS

IMPORT_DATA

EXPORT_DATA

LOCAL_DATA







/****************************************************************************/

FUNCTION PUBLIC VOID Get_setpoint
  (
    OUT FLOAT_S *  setpoint,
    IN  FLOAT_S    source
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  USIGN8  source_status;

FUNCTION_BODY

  source_status = source.status & QUALITY_MASK;

  if ((source_status == SQ_GOOD_CAS)||(source_status == SQ_GOOD_NC))
  {
    setpoint->status = (SQ_GOOD_CAS | (source.status & LIMIT_MASK));
  }

  else if (source_status == SQ_UNCERTAIN)
  {
    if (((source.status & SUB_STATUS_MASK) == SUB_SUBSTITUTE)   ||
        ((source.status & SUB_STATUS_MASK) == SUB_INITIAL_VALUE)  )
    {
      setpoint->status = (SQ_GOOD_CAS  | (source.status & LIMIT_MASK));
    }
    else
    {
      setpoint->status = SQ_BAD;
    }
  }

  else /* (source_status == SQ_BAD) */
  {
    setpoint->status = SQ_BAD;
  }

  setpoint->value  = source.value;
}

