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

FILE_NAME          ffbl_mo.h



DESCRIPTION

=========================================================================== */
#ifndef __FFBL_MO_H__
#define __FFBL_MO_H__

INCLUDES

GLOBAL_DEFINES
#define FAULT_STATE_DETECTED        3
#define FAULT_STATE_INACTIVE        4

#define FAULT_BY_RESOURCE_STATE     5
#define FAULT_BY_CAS_NO_COMM        6
#define FAULT_FORCED_BY_CAS_STATE   7
#define FAULT_FORCED_BY_RCAS_STATE  8

#define MODE_FAULT_STATE_DETECTED  10
#define MODE_FAULT_STATE_ACTIVE    11


LOCAL_DEFINES

EXPORT_TYPEDEFS

LOCAL_TYPEDEFS

typedef struct
{
  BOOL              active;
  USIGN8            reason;
  USIGN16           no_of_cycles;
}
  T_FAULT_DATA;


typedef struct
{
  BOOL              active;
  USIGN8            mode;
  USIGN32           current_time;
}
  T_SHED_DATA;


typedef struct
{
  USIGN16           profile;
  USIGN8            channel;
  USIGN8            linked_block;
  USIGN32           period_of_execution;
  BOOL              config_error;
  BOOL              mode_was_written;
  BOOL              new_target_mode;
  USIGN8            pid_downstream;
  MODE              mode;
  T_FAULT_DATA      fault;
  FLOAT             fstate_time;
  T_SHED_DATA       shed;
  USIGN8            shed_opt;
  USIGN16           io_opts;
  USIGN16           control_opts;
  USIGN16           status_opts;
  USIGN8            in_status;
  USIGN8            bkcal_in_status;
  USIGN8            cas_in_status;
  USIGN8            rcas_in_status;
  USIGN8            rout_in_status;
  USIGN8            cas_out_status;
  USIGN8            rcas_out_status;
  USIGN8            rout_out_status;
  USIGN8            out_status;
  DISCRETE_S        trk_in_d;
  USIGN8            tracking;
}
T_MO_DATA;

FUNCTION_DECLARATIONS

FUNCTION PUBLIC VOID Calc_block_mode (VOID);

EXPORT_DATA

#include "da_near.h"
NO_INIT  extern T_MO_DATA  mo;
#include "da_def.h"

IMPORT_DATA

LOCAL_DATA

#endif /* __FFBL_MO_H__ */
