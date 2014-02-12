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

FILE_NAME          fdc_api.h



DESCRIPTION
  This file contains macros, definitions and declarations of functions
  that describe the interface between application layer and communication
  stack.

=========================================================================== */
#ifndef __FDC_API_H__
#define __FDC_API_H__


/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    Device Startup Phase                                                  */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/

/* ------------------------------------------------------------------------ */
/*    Macros                                                                */
/* ------------------------------------------------------------------------ */

  /* Device type ---------------------------------------------------------- */
  #define DLL_BASICDEVICE               0x01
  #define DLL_LINKMASTER                0x02

  /* Serial Number and PD Tag --------------------------------------------- */
  #define FDC_SER_NO_LEN                  22
  #define DEFAULT_PD_TAG_SIZE             16

  /* Error codes ---------------------------------------------------------- */
  #define E_FDC_CFG_NOT_AVAILABLE     0x0001
  #define E_FDC_MIB_INCONSISTENT      0x0002
  #define E_FDC_EEP_INSUFF            0x0003
  #define E_FDC_EEP_ACCESS_FAILED     0x0004
  #define E_FDC_BUSPAR_INVALID        0x0005


/* ------------------------------------------------------------------------ */
/*    Type Definitions                                                      */
/* ------------------------------------------------------------------------ */
  typedef struct
  {
    STRINGV   default_pd_tag[DEFAULT_PD_TAG_SIZE];
    BOOL      sm_operational;
    USIGN8    default_node_address;
    USIGN8    default_dev_type;

  }
    T_FDC_DEVICE_START;
  /* -------------------------------------------------------------------------
  TYPE_DESCRIPTION:
    A parameter of this type is used as input of the function 'fdc_load_new_config'.
    The structure contains components that determine the behavior of the device
    if it starts with default data.

    default_pd_tag        These 16 octets are concatenated with the last 16 octects
                          of the serial number to a 32 octet default PD tag

    sm_operational        Default SM state of the device:
                            TRUE:  Device start in SM state OPERATIONAL
                            FALSE: Device start in SM state INITIALIZED

    default_node_address  Default station address of device

    default_dev_type      Default device type: DLL_BASICDEVICE | DLL_LINKMASTER
                          (without meaning for basic devices)
  ------------------------------------------------------------------------- */


  typedef struct
  {
    USIGN16   slot_time_min;
    USIGN8    min_inter_pdu_delay_min;
    USIGN8    max_reply_delay_min;
  }
    T_FDC_BUSPARAM;
  /* -------------------------------------------------------------------------
  TYPE_DESCRIPTION:
    A parameter of this type is used as input of the function 'fdc_load_new_config'.
    The structure contains components that are used as 'fastest' busparameters
    the device is able to handle with.
  ------------------------------------------------------------------------- */



/* ------------------------------------------------------------------------ */
/*    Interface Functions of the Field Device Communication Stack (FDC)     */
/* ------------------------------------------------------------------------ */

FUNCTION GLOBAL USIGN16 fdc_check_nv_ram (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function is called by the application layer during the device startup
  phase. It triggers the communication stack to check if it has got consistent
  and valid configuration data from the non-volatile memory.

  If the communication stack has got no valid configuration data, the application
  has to load default configuration data to the communication stack!

PARAMETERS
  None

RETURN_VALUES
  E_OK                      There are valid and consistent configuration data
  E_FDC_CFG_NOT_AVAILABLE   Configuration data are not available
  E_FDC_MIB_INCONSISTENT    The MIB configuration data are inconsistent
----------------------------------------------------------------------------*/
;


FUNCTION GLOBAL USIGN16 fdc_apply_for_nv_ram (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  If the preceding call of fdc_check_nv_ram() returned with a negative result,
  the application has to trigger the communication stack to apply for new non-
  volatile memory. The non-volatile memory is used to store the non-volatile
  database of the communication stack. The database contains MIB objects as
  FB schedule, VCR entries, LAS schedule etc.

PARAMETERS
  None

RETURN_VALUES
  E_OK          The request for non-volatile memory was accepted
 !E_OK          The request for non-volatile memory was not accepted
----------------------------------------------------------------------------*/
;



FUNCTION GLOBAL USIGN16 fdc_load_new_config
  (
    IN T_FDC_DEVICE_START *   p_fdc_device_start,
    IN T_FDC_BUSPARAM *       p_fdc_busparam
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  If the preceding call of fdc_check_nv_ram() returned with a negative result
  the application has to load new configuration data to the communication stack.

PARAMETERS
  p_fdc_device_start      Pointer to default data of the System Management

  p_fdc_busparam          Pointer to the 'fastest' busparameters the device
                          can handle with

RETURN_VALUES
  E_OK                    Input data are correct. Storage of the data in
                          the nv-memory was successful.

  E_FDC_EEP_INSUFF        There is a severe error of the non-volatile-memory.
                          Correct access to the nv-memory is not possible.
                          Because of missing or inconsistent configuration it
                          is not possible to communicate via FF bus. The device
                          should be taken into a safe state!
----------------------------------------------------------------------------*/
;


FUNCTION GLOBAL USIGN16 fdc_start
  (
    IN STRINGV  *           p_serial_number
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function is used to start the field device communication stack.

  A device will not enter the livelist before this function is executed!

PARAMETERS:
  p_serial_number     Device serial number (*)


  (*) The device serial number is a visible string of 22 octets. It is used to
  build the Device-Id and a default PD-Tag.

  The Device-Id is built from the Manufacturer-ID (six octets; provided by the
  resource block), the Device-Type (four octets; provided by the resource block)
  and the 22 octets of the serial number. The Device-ID is not stored in non-
  volatile memory, it is built during each device startup phase in a subroutine
  of fdc_start().

  The default PD-Tag is built from the 'default_pd_tag' (16 octets from the device
  start structure) and the last 16 octets of the serial number. The communication
  stack will build a default PD-Tag only if fdc_load_new_config() is called before
  fdc_start(). In all other cases the communication stack uses the PD-Tag stored
  in the nv-memory.

RETURN_VALUES
  E_OK        The start of the communcation stack was successful
 !E_OK        The start of the communcation stack was not successful
----------------------------------------------------------------------------*/
;


#ifdef DOWNLOAD_DEVICE /* ------------------------------------------------- */

FUNCTION GLOBAL VOID fdc_load_device_ident
  (
    IN USIGN32    manufac_id,
    IN USIGN32    device_type
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function is available in the download device software only.

  The application has to call this function in the device startup phase.
  The communication stack uses the inputs to build the device-id of the download
  device.

PARAMETERS
  manufac_id    Manufacturer-ID of the device
  device_type   Device type

RETURN_VALUES
  None
----------------------------------------------------------------------------*/
;

#endif /* DOWNLOAD_DEVICE */



/* ------------------------------------------------------------------------ */
/*    Global Variables of the Field Device Communication Stack              */
/* ------------------------------------------------------------------------ */

#include "da_fdc.h"                              /* DATA SEGMENT DEFINITION */
  NO_INIT extern BOOL   tag_len_16;
#include "da_def.h"                              /* DEFAULT DATA SEGMENT */

#endif /* __FDC_API_H__ */



