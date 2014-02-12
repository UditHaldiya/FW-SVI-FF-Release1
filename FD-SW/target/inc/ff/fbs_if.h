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

FILE_NAME          fbs_if.h



FUNCTIONAL_MODULE_DESCRIPTION

    Interface provided by FBS component to Function Block Layer

=========================================================================== */
#ifndef __FBS_IF_H__
#define __FBS_IF_H__

INCLUDES

GLOBAL_DEFINES

LOCAL_DEFINES

EXPORT_TYPEDEFS

LOCAL_TYPEDEFS

FUNCTION_DECLARATIONS


/* --- Interface functions provided by FB shell ------------------------------- */
/* ---------------------------------------------------------------------------- */

FUNCTION GLOBAL VOID fbs_init (VOID);
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

This function initializes the function block shell.

It reads the object description of the function block VFD and it writes -
among others - to trend objects.

Note: The function block shell does not store trend objects in EEPROM.
The application is responsible to copy the trend data from EEPROM to RAM
(i.e. into the variables trend_float, trend_disc) before it calls this init-
function.
----------------------------------------------------------------------------*/


FUNCTION GLOBAL VOID fbs_background (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

----------------------------------------------------------------------------*/
;


FUNCTION GLOBAL BOOL fbs_device_has_float_alarms (VOID);

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:

  This function returns TRUE if the device has float alarms.
  Otherwise FALSE.

PARAMETERS:

  none

GLOBALS_AFFECTED:

  none

RETURN_VALUES:

  TRUE        device has float alarms
  FALSE       device does not have float alarms

----------------------------------------------------------------------------*/




FUNCTION GLOBAL VOID fbs_load_link_defaults
  (
    IN VOID *   p_link_objects
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:

  This function is called if the link object configuration data
  are not available. It writes link object default values to the location
  where p_link_objects points to (usually to a RAM-image of the EEPROM).

PARAMETERS:

GLOBALS_AFFECTED:
  none

RETURN_VALUES:
  none
----------------------------------------------------------------------------*/
;




FUNCTION GLOBAL VOID fbs_load_trend_defaults
  (
    IN VOID *   p_trend_objects
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:

  This function is called if the trend configuration data
  are not available. It writes trend default values to the location
  where p_trend_objects points to (usually to a RAM-image of the EEPROM).

PARAMETERS:

GLOBALS_AFFECTED:
  none

RETURN_VALUES:
  none
----------------------------------------------------------------------------*/
;





FUNCTION GLOBAL VOID fbs_load_mvc_defaults
  (
    IN VOID *   p_mvc_objects
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:


PARAMETERS:

GLOBALS_AFFECTED:

RETURN_VALUES:
  none
----------------------------------------------------------------------------*/
;







FUNCTION GLOBAL VOID fbs_load_action_defaults
  (
    IN VOID *   p_action_object
  )
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:

PARAMETERS:

GLOBALS_AFFECTED:
  none

RETURN_VALUES:
  none
----------------------------------------------------------------------------*/
;



FUNCTION GLOBAL VOID fbsmvc_send_mvc (VOID);



FUNCTION GLOBAL VOID fbsscd_set_fb_start_changed (BOOL);


EXPORT_DATA

#endif /* __FBS_IF_H__ */
