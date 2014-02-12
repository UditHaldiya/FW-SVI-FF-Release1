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

FILE_NAME          ffbl_pai.c



FUNCTIONAL_MODULE_DESCRIPTION

=========================================================================== */
  #include "keywords.h"
  #define  MODULE_ID      (COMP_FFBL + MOD_FBLKPAI)

INCLUDES
  #include <math.h>

  #include "base.h"
  #include "except.h"
  #include "vfd.h"
  #include "fbs_api.h"
  #include "ffbl_int.h"
  #include "ffbl_res.h"
  #include "ffbl_pai.h"

LOCAL_DEFINES

LOCAL_TYPEDEFS

FUNCTION_DECLARATIONS

IMPORT_DATA
#include "da_ffbl.h"                             /* DATA SEGMENT DEFINITION */
  NO_INIT extern T_FBS_ALERT_NOTIFY    ffbl_alert_notify;
#include "da_def.h"                              /* DEFAULT DATA SEGMENT */

EXPORT_DATA
#include "da_near.h"                             /* DATA SEGMENT DEFINITION */
  NO_INIT T_ALM_DATA alm;
#include "da_def.h"                              /* DEFAULT DATA SEGMENT */

LOCAL_DATA

/****************************************************************************/

FUNCTION PUBLIC VOID Handle_inact_alarm (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  BOOL alm_detect;

FUNCTION_BODY

  if (alm.high)
       alm_detect = (BOOL)(alm.value >= alm.limit);
  else  /* (alm.low) */
       alm_detect = (BOOL)(alm.value <= alm.limit);

  if (alm_detect && !alm.disabled && alm.pri)
  {
    ffbl_alert_notify.action            = ALARM_ACTIVE;
    ffbl_alert_notify.block_id          = alm.block_id;
    ffbl_alert_notify.rel_idx           = alm.idx;
    ffbl_alert_notify.priority          = alm.pri;
    ffbl_alert_notify.data.flt.subcode  = 0;
    ffbl_alert_notify.data.flt.value    = alm.value;

    fbs_alert_notify (&ffbl_alert_notify);

    if (alm.auto_ack || (alm.pri == 1))
         alm.ptr->unack = ALARM_ACKED;
    else alm.ptr->unack = ALARM_UNACKED;
  }
}





FUNCTION PUBLIC VOID Handle_act_alarm (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  BOOL alm_cleared;

FUNCTION_BODY

  if (alm.disabled)
       alm_cleared = TRUE;
  else if (alm.high)
       alm_cleared = (BOOL)(alm.value < (alm.limit - alm.hyst));
  else /* (alm.low) */
       alm_cleared = (BOOL)(alm.value > (alm.limit + alm.hyst));

  if (alm_cleared || (alm.pri == 0))
  {
    ffbl_alert_notify.action            = ALARM_CLEAR;
    ffbl_alert_notify.block_id          = alm.block_id;
    ffbl_alert_notify.rel_idx           = alm.idx;
    ffbl_alert_notify.priority          = alm.pri;
    ffbl_alert_notify.data.flt.subcode  = 0;
    ffbl_alert_notify.data.flt.value    = alm.value;

    fbs_alert_notify (&ffbl_alert_notify);

    return;
  }
}




FUNCTION PUBLIC USIGN8 Set_good_nc_substatus
  (
    USIGN16         alarm_unack,
    USIGN16         alarm_current,
    USIGN8          lo_pri,
    USIGN8          lo_lo_pri,
    USIGN8          hi_pri,
    USIGN8          hi_hi_pri
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
USIGN8    substatus;

FUNCTION_BODY

  substatus = SUB_NON_SPECIFIC;

  if (alarm_unack)
  {
    if (alarm_unack & ALARM_SUM_HI_HI)
    {
      if (hi_hi_pri < 8)
             substatus = SUB_UNACK_ADVISORY_ALARM;
      else { substatus = SUB_UNACK_CRITICAL_ALARM;  escape; }
    }

    else if (alarm_unack & ALARM_SUM_LO_LO)
    {
      if (lo_lo_pri < 8)
             substatus = SUB_UNACK_ADVISORY_ALARM;
      else { substatus = SUB_UNACK_CRITICAL_ALARM; escape; }
    }

    if (alarm_unack & ALARM_SUM_HI)
    {
      if (hi_pri < 8)
           substatus = SUB_UNACK_ADVISORY_ALARM;
      else substatus = SUB_UNACK_CRITICAL_ALARM;
    }

    else if (alarm_unack & ALARM_SUM_LO)
    {
      if (lo_pri < 8)
           substatus = SUB_UNACK_ADVISORY_ALARM;
      else substatus = SUB_UNACK_CRITICAL_ALARM;
    }

    if (substatus < SUB_UNACK_ADVISORY_ALARM)
        substatus = SUB_UNACK_BLOCK_ALARM;
  }

  else if (alarm_current)
  {
    if (alarm_current & ALARM_SUM_HI_HI)
    {
      if (hi_hi_pri < 8)
             substatus = SUB_ACTIVE_ADVISORY_ALARM;
      else { substatus = SUB_ACTIVE_CRITICAL_ALARM;  escape; }
    }

    else if (alarm_current & ALARM_SUM_LO_LO)
    {
      if (lo_lo_pri < 8)
             substatus = SUB_ACTIVE_ADVISORY_ALARM;
      else { substatus = SUB_ACTIVE_CRITICAL_ALARM; escape; }
    }

    if (alarm_current & ALARM_SUM_HI)
    {
      if (hi_pri < 8)
           substatus = SUB_ACTIVE_ADVISORY_ALARM;
      else substatus = SUB_ACTIVE_CRITICAL_ALARM;
    }

    else if (alarm_current & ALARM_SUM_LO)
    {
      if (lo_pri < 8)
           substatus = SUB_ACTIVE_ADVISORY_ALARM;
      else substatus = SUB_ACTIVE_CRITICAL_ALARM;
    }

    if (substatus < SUB_ACTIVE_ADVISORY_ALARM)
        substatus = SUB_ACTIVE_BLOCK_ALARM;
  }


  endlbl:
  return (substatus);
}

