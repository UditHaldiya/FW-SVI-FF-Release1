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

FILE_NAME          ffbl_pai.h



DESCRIPTION

=========================================================================== */
#ifndef __FFBL_PAI_H__
#define __FFBL_PAI_H__

#include "keywords.h"   /* keyword defines */

INCLUDES

GLOBAL_DEFINES

LOCAL_DEFINES

EXPORT_TYPEDEFS
typedef struct
{
  USIGN8          block_id;
  BOOL            high;
  ALARM_FLOAT *   ptr;
  BOOL            disabled;
  BOOL            auto_ack;
  USIGN16         idx;
  USIGN8          pri;
  FLOAT           value;
  FLOAT           limit;
  FLOAT           hyst;
}
T_ALM_DATA;


LOCAL_TYPEDEFS

FUNCTION_DECLARATIONS

FUNCTION PUBLIC VOID Handle_inact_alarm (VOID);

FUNCTION PUBLIC VOID Handle_act_alarm (VOID);

FUNCTION PUBLIC USIGN8 Set_good_nc_substatus
  (
    USIGN16         alarm_unack,
    USIGN16         alarm_current,
    USIGN8          lo_pri,
    USIGN8          lo_lo_pri,
    USIGN8          hi_pri,
    USIGN8          hi_hi_pri
  );

EXPORT_DATA

#include "da_near.h"
  NO_INIT extern T_ALM_DATA alm;
#include "da_def.h"

IMPORT_DATA

LOCAL_DATA


#endif /* __FFBL_PAI_H__ */
