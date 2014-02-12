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

FILE_NAME          fbs_api.h

DESCRIPTION
  This file contains macros, definitions and declarations of functions
  that are provided by the Function Block Shell.

=========================================================================== */
#ifndef __FBS_API_H__
#define __FBS_API_H__

#include "fbif.h"

/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    Device Startup Phase                                                  */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/

/* -------------------------------------------------------------------------
In the device startup phase the application performs different steps. It
1) loads the block list to the FB shell
2) checks the consistency of the non-volatile database (*)
3) starts the function block shell
4) synchronizes the transducer parameters with the application objects (**)
5) starts the communication stack

(*) Each component of the field device software has its own non-volatile
    database. There is a
    - non-volatile database for the application
    - non-volatile database for the FB shell
    - non-volatile database for the field device communication stack

(**) Step 4) is optional. It depends on the architecture of the application.
    In a two processor system it may be necessary two synchronize the database
    of both processors before the field device connects to the fieldbus.
    Step 4) is intended to synchronize both databases.
--------------------------------------------------------------------------- */


FUNCTION GLOBAL VOID fbs_load_fbif_dict
  (
    IN const T_FBIF_DICTIONARY *      p_fbif_dict
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This is the first FB shell function the application has to call. With this
  function the application provides the block list and additional VFD information
  to the FB shell.

PARAMETERS
  p_fbif_dict         Pointer to the FBIF dictionary

RETURN_VALUES
  None
----------------------------------------------------------------------------*/
;


FUNCTION GLOBAL USIGN16 fbs_check_nv_ram (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function checks the consistency of the non-volatile data of the FB shell.

PARAMETERS
  None

RETURN_VALUES
  E_OK          The non-volatile data of the FB shell are consistent
 !E_OK          The non-volatile data of the FB shell are not consistent
----------------------------------------------------------------------------*/
;


FUNCTION GLOBAL USIGN16 fbs_check_extra_nv_ram
  (
    IN USIGN16  extra_nv_ram_size
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function checks the consistency of the data in the extra non-volatile
  memory. (For the reason of the extra non-volatile memory see the functional
  description of fbs_apply_for_extra_nv_ram())

  The call of this function is optional. If there is no extra non-volatile
  memory this function needs not be called.

PARAMETERS
  None

RETURN_VALUES
  E_OK          The extra non-volatile data of the application are consistent
 !E_OK          The extra non-volatile data of the application are not consistent
----------------------------------------------------------------------------*/
;


FUNCTION GLOBAL USIGN16 fbs_apply_for_nv_ram (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function applies for non-volatile memory for the FB shell. The non-volatile
  database of the FB shell covers shell configuration data as link objects as
  well as the non-volatile data of the FF blocks.

PARAMETERS
  None

RETURN_VALUES
  E_OK          The request for non-volatile memory was accepted
 !E_OK          The request for non-volatile memory was not accepted
----------------------------------------------------------------------------*/
;


FUNCTION GLOBAL USIGN16 fbs_apply_for_extra_nv_ram
  (
    IN USIGN16  extra_nv_ram_size
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function applies for extra non-volatile memory. Extra non-volatile memory
  can to be used for non-volatile data that is not covered by block parameters
  or other standard configuration data.

  A typical value to be stored in extra non-volatile memory is the Total register
  of the IT block. The Total register is a double value (8 bytes) whereas the
  IT block parameter TOTAL is a float value (4 bytes). I.e. the TOTAL parameter
  can not be used to store the Total register value.

  The FB shell provides one extra non-volatile block. This block can be used by
  multiple FF blocks. The FB shell allows to subdivide the extra non-volatile
  block into a maximum of eight sub-blocks. I.e. of maximum of eight FF blocks
  are allowed to announce for extra non-volatile memory.

  During the device startup phase the application has to make sure that all block
  are asked for there need of extra non-volatile memory. The application has to
  summarize the need of all FF blocks and then to apply for the total sum by
  calling fbs_apply_for_extra_nv_ram().

  Currently the IT block is the only standard block that needs extra non-volatile
  memory. In dependance on the application there may be transducer blocks with
  a need of extra non-volatile memory

  The call of this function is optional. If there is no need for extra non-volatile
  memory this function needs not be called.

PARAMETERS
  extra_nv_ram_size        Size of extra non-volatile block

RETURN_VALUES
  E_OK          The request for non-volatile memory was accepted
 !E_OK          The request for non-volatile memory was not accepted
----------------------------------------------------------------------------*/
;


FUNCTION GLOBAL VOID fbs_set_nv_ram_cycle_time
  (
    IN USIGN32  nv_ram_cycle_time     /* nv cycle-time in seconds */
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function is used to set the nv cycle-time.

  The nv cycle-time corresponds with the resource block parameter NV_CYCLE_T.
  According to the FF specs, NV_CYCLE_T is the time interval for writing copies
  of N-parameters to non-volatile memory. If NV_CYCLE_T is zero N-parameters
  will never be automatically copied to NV memory.

  Previous versions of the FB shell did not support a NV_CYCLE_T different from
  zero. I.e. N-parameters were stored only when they were written via the fieldbus.
  The advantage of this approach was that a new value was immediately stored in
  the NV memory each time it was written via the fieldbus.


  If NV_CYCLE_T is different from zero a new parameter value may be lost if the
  device is powered down within the NV cycle-time interval. To minimize the risk
  of losing non-volatile values, the FB shell sets its NV cycle-time interval to
  be elapsed each time a N-parameter is written via the fieldbus. The elapsed
  NV cycle-time triggers the background task to store all N-parameters that have
  changed in the last NV cycle-time interval. As it take only a few seconds to
  store all modified N-parameters the latest values should be saved when after
  the last write access the device has power for some additional seconds.

  Please set the NV_CYCLE_T carefully! The setting depends on the requirement of
  the application and even more on the type of non-volatile memory available. A
  small NV_CYCLE_T  may destroy an EEPROM in an inacceptable short time!

  In conclusion: if there is no strong requirement for a NV_CYCLE_T different
  from zero, it is recommended to run the FB shell further on with a NV_CYCLE_T
  of zero.

  The call of fbs_set_nv_ram_cycle_time() is optional. If it is not called the
  FB shell will assume a NV_CYCLE_T of zero.

  Please note that the units of 'nv_ram_cycle_time' and NV_CYCLE_T are different.
  The unit of the nv cycle-time is second whereas the unit of NV_CYCLE_T is
  1/32 millisecond.

PARAMETERS
  nv_ram_cycle_time         nv cycle-time in seconds

RETURN_VALUES
  None
----------------------------------------------------------------------------*/
;


FUNCTION GLOBAL VOID fbs_set_extra_nv_ram_cycle_time
  (
    IN USIGN32  extra_nv_ram_cycle_time     /* seconds */
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function is used to set extra nv cycle-time.

  Similar to NV_CYCLE_T the extra nv cycle-time defines the interval for writing
  copies of extra nv-data to non-volatile memory. If the extra nv cycle-time is
  set to a value different from zero the background task will store all extra
  nv-data changed in the last interval. If the extra nv cycle-time is zero the
  background task will not automatically store the extra nv-data. In this case
  the routine fbs_write_extra_nv_data() must be called to signal to the back-
  ground task that the extra nv-data have to be stored.

  Please set the extra nv cycle-time carefully! The setting depends on the require-
  ment of the application and even more on the type of the non-volatile memory.
  A short extra NV cycle-time  may destroy an EEPROM in an inacceptable short time!
  Also the call of fbs_write_extra_nv_data() in a high frequency may damage the EEPROM.

  The call of fbs_set_extra_nv_ram_cycle_time() is optional. If it is not called
  the FB shell will assume a extra nv cycle-time of zero.

PARAMETERS
  extra_nv_ram_cycle_time         extra nv cycle-time in seconds

RETURN_VALUES
  None
----------------------------------------------------------------------------*/
;



FUNCTION GLOBAL BOOL fbs_auto_extra_nv_update (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function is used to check whether extra nv cycle-time is zero.

  If the extra nv cycle-time is set to a value different from zero the back-
  ground task will store all extra nv-data changed in the last interval.
  If the extra nv cycle-time is zero the background task will not automatically
  store the extra nv-data. In this case the routine fbs_write_extra_nv_data()
  must be called to signal to the background task that the extra nv-data
  have to be stored.

PARAMETERS
  None

RETURN_VALUES
  TRUE          The extra nv cycle-time is not zero. The extra nv-data are
                stored periodically in the background

  FALSE         The extra nv cycle-time is zero. Each storage of extra nv-data
                must be triggered by calling fbs_write_extra_nv_data()
----------------------------------------------------------------------------*/
;




FUNCTION GLOBAL VOID * fbs_alloc_extra_nv_ram
  (
    IN USIGN8  block_id,
    IN USIGN8  extra_nv_ram_size
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function is used to allocate one FF-block-specific sub-block of the extra
  non-volatile memory.

  The address returned by this function points to a RAM location that is used as
  image of the extra non-volatile memory. There are two ways to store the data in
  the extra non-volatile memory:

  a) the call of fbs_write_extra_nv_data() triggers the background task to store
     the extra nv-data in the non-volatile memory during its next execution. I.e.
     the extra nv-data are changed within a few seconds after the call of
     fbs_write_extra_nv_data() (immediate storage)

  b) if the extra extra nv cycle-time is different from zero the background task
     will store extra nv-data in the non-volatile memory at the end of the current
     extra-nv cycle-time interval (deferred storage)

  Before the background writes the extra nv-data into the non-volatile memory it
  always checks the data for changes. If the extra nv-data are unchanged the
  background task will not write data into the non-volatile memory.

PARAMETERS
  block_id              block identifier
  extra_nv_ram_size     size of the used extra nv ram

RETURN_VALUES
  !NULL                 Pointer to the extra NV RAM used by this block.
  NULL                  No allocation possible.
----------------------------------------------------------------------------*/
;




FUNCTION GLOBAL USIGN16 fbs_start
  (
    IN STRINGV  *   p_serial_number
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function is used to start the FB shell.

  In addition to the start of the FB shell this function also calls the start
  routine of all blocks and after its return all blocks have got their
  configuration data.

  Please note that the function blocks are not executed before the communication
  stack is started

PARAMETERS
  p_ser_no      Pointer to the device serial number. (*)

  (*) The device serial number is a visible string of 22 octets. If the block
  starts with the default configuration the FB shell will take the last 16 octets
  of the device serial number to build the default block-tags.

RETURN_VALUES
  E_OK          The FB shell and the block were started sucessfully
  !E_OK         The start of the FB shell and the blocks was not sucessfully
----------------------------------------------------------------------------*/
;







/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    Application Functions Called by the FB Shell                          */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/

/* The application provides the FF function block shell with callback functions.
   The following structure of function pointers makes the callback functions
   available to the FF function block shell
*/

  typedef struct
  {
    GLOBAL USIGN16           (* a_of_read_hndl_RESB)    (T_FBIF_BLOCK_INSTANCE *, T_FBIF_READ_DATA *);
    GLOBAL USIGN16           (* a_of_write_hndl_RESB)   (T_FBIF_BLOCK_INSTANCE *, T_FBIF_WRITE_DATA *);
    GLOBAL VOID              (* a_of_background_RESB)   (T_FBIF_BLOCK_INSTANCE *);
    GLOBAL USIGN16           (* a_of_read_hndl_FCTB)    (T_FBIF_BLOCK_INSTANCE *, T_FBIF_READ_DATA *);
    GLOBAL USIGN16           (* a_of_write_hndl_FCTB)   (T_FBIF_BLOCK_INSTANCE *, T_FBIF_WRITE_DATA *);
    GLOBAL VOID              (* a_of_get_default_value) (T_FBIF_BLOCK_INSTANCE *, T_FBIF_READ_DATA *);
    GLOBAL BOOL              (* a_of_blk_state_check)   (USIGN16, USIGN8);
    GLOBAL USIGN16           (* a_of_verify_nv_ram)     (VOID);
  }
    T_FBS_CALL_FDEV_FUNCT;

  /* -------------------------------------------------------------------------
  TYPE_DESCRIPTION:

    FUNCTION GLOBAL USIGN16 (* a_of_read_hndl_RESB)
      (
        T_FBIF_BLOCK_INSTANCE  * p_block_instance,
        T_FBIF_READ_DATA       * p_read
      )

    FUNCTIONAL_DESCRIPTION
      This function will be called by read handler of the FB shell if a resource
      block parameter with a 'A' or 'X' GenVFD-flag is read.

      As the function block layer handles read accesses to all standard parameters
      it is assumed that the 'A' and 'X' flags are set just for enhanced resource
      block parameters.

      The 'A' flag can be set for parameters that are temporary inconsistent
      or not accessible. Typically the 'A' flag is assigned to dynamic
      parameter that are updated just when they are read. In this case
      a_of_read_hndl_RESB() has to make sure that the parameter is up-to-date
      before it returns. If it is not possible to get an consistent state or value
      a_of_read_hndl_RESB() may return with E_FB_TEMP_ERROR. Please note that
      sporadic denials of read accesses are usually accepted by host systems.
      However, iterated negative read responses are indicated as device errors
      at the system-side.

      A parameter with an 'X' flag is completely under the control of the appli-
      cation. It is not stored by the funcion block layer. When it is called
      a_of_read_hndl_RESB() has to copy the current parameter value to the read
      response buffer. The read response buffer is referenced by the component
      'dest' of the read descriptor. Please note that access by subindex is
      supported. I.e. in case of record or array parameters  a_of_read_hndl_RESB()
      has to check whether the whole parameter or just one component is read.

    PARAMETERS
      p_block_instance      Pointer to the resource block instance

      p_read                Pointer to a read descriptor block

      Please find comments on block instance and read descriptor in file fbif.h

    RETURN_VALUES
      E_OK                  Read access is approved and the parameter is up-to-date
      E_FB_TEMP_ERROR       Read access is not possible at the moment

    --------------------------------------------------------------------------
    --------------------------------------------------------------------------

    FUNCTION GLOBAL USIGN16 (* a_of_write_hndl_RESB)
      (
        T_FBIF_BLOCK_INSTANCE  * p_block_instance,
        T_FBIF_WRITE_DATA      * p_write
      )

    FUNCTIONAL_DESCRIPTION
      This function will be called by write handler of the FB shell if a resource
      block parameter with a 'B' or 'X' GenVFD-flag has to be written.

      During the write handling this function is called two times. When it is
      called the first time, the component 'phase' of the write descriptor is
      set to PRE_WRITE_CHECK. In this phase the value to be written should be
      validated. When a_of_write_hndl_RESB() is called the second time, the
      component 'phase' of the write descriptor is set to POST_WRITE_ACTION.

      This phase is intended to perform actions in dependance on the new value.
      Please note that the write handler of the FB shell does not send the write
      response before a_of_write_hndl_RESB() returns from its POST_WRITE_ACTION.
      If complex actions has to be performed it is a good idea to just send a
      message to an other application task. This additional application task is
      intended to have a lower priority than the FDC task. When this additional
      application task receives the message from a_of_write_hndl_RESB() it can
      update the application in dependance on the new parameter value without
      deferring the sending of the write response via the fieldbus.

      A parameter with an 'X' flag is completely under the control of the
      application. Thus, the storage of the new parameter value is a mandatory
      POST_WRITE_ACTION. If the parameter to be written is static, the appli-
      cation has in addition to increment the static revision counter. The FB
      shell provides the fbs_incr_st_rev() routine to do this.

      If the parameter to be written has got no 'X' flag, the standard write
      handler steps - check of the new value,  storage of the new value,
      incrementation of the static revision counter - are done by the write
      handler of the FB shell.


    PARAMETERS
      p_block_instance      Pointer to the resource block instance

      p_write               Pointer to a write descriptor block
                            (For the structure of the write descriptor see fbif.h)

    RETURN_VALUES
      E_OK                   Parameter check was successful
      E_FB_PARA_CHECK        Parameter check was not successful
      E_FB_PARA_LIMIT        Parameter exceeds a limit
      E_FB_WRONG_MODE        Parameter is not writable in the current target mode
      E_FB_DATA_NO_WRITE     The component selected by subindex is read-only
      E_FB_TEMP_ERROR        It is not possible to write the parameter at the moment

    --------------------------------------------------------------------------
    --------------------------------------------------------------------------

    FUNCTION GLOBAL VOID (* a_of_background_RESB)
      (
        T_FBIF_BLOCK_INSTANCE  * p_block_instance
      )

    FUNCTIONAL_DESCRIPTION
      This function is called periocally by the background task. It can be used to
      perform resource block related background actions.

    PARAMETERS
      p_block_instance      Pointer to the resource block instance

    RETURN_VALUES
      None

    --------------------------------------------------------------------------
    --------------------------------------------------------------------------

    FUNCTION GLOBAL USIGN16 (* a_of_read_hndl_FCTB)
      (
        T_FBIF_BLOCK_INSTANCE  * p_block_instance,
        T_FBIF_READ_DATA       * p_read
      )

    FUNCTIONAL_DESCRIPTION
      This function will be called by read handler of the FB shell if a function
      block parameter with a 'A' or 'X' GenVFD-flag is read. As the function
      block layer handles read accesses to all standard parameters it is assumed
      that the 'A' and 'X' flags are set just for enhanced function block para-
      meters.

      a_of_read_hndl_FCTB() works in the same way as a_of_read_hndl_RESB()


    --------------------------------------------------------------------------
    --------------------------------------------------------------------------

    FUNCTION GLOBAL USIGN16 (* a_of_write_hndl_FCTB)
      (
        T_FBIF_BLOCK_INSTANCE  * p_block_instance,
        T_FBIF_WRITE_DATA      * p_write
      )

    FUNCTIONAL_DESCRIPTION
      This function will be called by write handler of the FB shell if a function
      block parameter with a 'B' or 'X' GenVFD-flag has to be written.

      a_of_write_hndl_FCTB() works analogous to a_of_write_hndl_RESB()


    --------------------------------------------------------------------------
    --------------------------------------------------------------------------

    FUNCTION GLOBAL VOID (* a_of_get_default_value)
      (
        T_FBIF_BLOCK_INSTANCE * p_block_instance,
        T_FBIF_READ_DATA *      p_read
      )

    FUNCTIONAL_DESCRIPTION
      This function will be called if the block parameter are set to their
      default values and if in the GenVFD script a parameter is attributed
      with the Y-flag. The 'Y' flag indicates that the value from the GenVFD
      script should not be used as default value. In this case the application
      has to provide the default value.

    PARAMETERS
      p_block_instance      Pointer to the block that conains the 'Y' parameter

      p_read                Pointer to a read descriptor. The read descriptor
                            specifies the parameter that needs a default value
                            (For the structure of the read descriptor see fbif.h)

    RETURN_VALUES
      None


    --------------------------------------------------------------------------
    --------------------------------------------------------------------------

    FUNCTION GLOBAL USIGN8 (* a_of_blk_state_check)
      (
        IN USIGN16     block_id,
        IN USIGN8      action
      )

    FUNCTIONAL_DESCRIPTION
     This function is called when a block is to be instantiated or deleted. By
     returning with TRUE or FALSE the application descides whether the requested
     action is performed or not.

     The application layer has to check instantiate or delete requests carefully.
     Especially - as the function block layer has no chance to check it - the
     application has to check if the request is conform to the CFF file keys
     'NumOfInstantiableBlocks' and 'NumOfPermanentBlocks'.

    PARAMETERS
      block_id          Block identifier generated by GENVFD.
      action            BLK_INACTIVE  An active (visible) block should be deleted
                        BLK_ACTIVE    An inactive (invisible) block should be
                                      instantiated

    RETURN_VALUES
      TRUE              The application agrees with the action
      FALSE             The application refuses the action


    --------------------------------------------------------------------------
    --------------------------------------------------------------------------

    FUNCTION GLOBAL USIGN16 (* a_of_verify_nv_ram) (VOID)

    FUNCTIONAL_DESCRIPTION
      This function is called by the background task periodically. The timer
      interval is defined by the first return value of this function (see
      RETURN_VALUES)

      This function is planed to scan the non-volatile memory and, in case of
      a problem, to notify the communication task to report a resource block
      error.

    PARAMETERS
      None

    RETURN_VALUES
      Returns the verification cycle time in 100ms unit. The background task
      stores the return value when the function returns for the first time.
      Afterwards, the background task calls this function every n-times * 100ms.

      The maximum value is 65535 (then the timer interval 65535 * 100ms). A return
      value of 0 requests the background task to stop calling this function

  ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------ */
/*    Global Variables for Interface Functions                              */
/* ------------------------------------------------------------------------ */

#include "da_fbs.h"    /* DATA SEGMENT_DEFINITION */

  NO_INIT extern T_FBS_CALL_FDEV_FUNCT    fbs_call_fdev_funct;

#include "da_def.h"    /* DEFAULT DATA SEGMENT */






/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    Local Access to Block Parameters                                      */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
  typedef struct
  {
    USIGN8          rel_idx;
    USIGN8          subindex;
    USIGN8 *        read_buffer;
    USIGN8          read_buffer_size;
    USIGN8          param_size;
  }
    T_FBS_READ_PARAM_LOCAL;

  /* -------------------------------------------------------------------------
  TYPE_DESCRIPTION

    This structure is used to describe a local read access to block parameter.
    A descriptor of this type is used as input parameter of the read routine
    fbs_read_param_loc().

    rel_idx           Relative index of the parameter within the block

    subindex          For records and arrays this parameter describes the component
                      of the parameter to be read. If subindex is zero the whole
                      parameter is read

    read_buffer       Pointer to the buffer where the parameter value is copied

    read_buffer_size  Size of the read buffer

    param_size        Size of the parameter / number of bytes that are copied
                      to the read buffer
  ----------------------------------------------------------------------------*/



FUNCTION GLOBAL USIGN16 fbs_read_param_loc
  (
    IN T_FBIF_BLOCK_INSTANCE *     p_block_instance,
    IN T_FBS_READ_PARAM_LOCAL *    p_read
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function can be used to read a local block parameter

PARAMETERS
  p_block_instance      Pointer to the block whose parameter should be read
  p_read                Pointer to the desriptor of the local read request

RETURN_VALUES
  E_OK                  Read acess was successful
  E_FB_PARA_CHECK       The read descriptor is invalid or inconsistent
  E_FB_READ_BUF_INSUFF  The 'read_buffer_size' is to short for the parameter
-----------------------------------------------------------------------------*/
;


  typedef struct
  {
    USIGN8          rel_idx;
    USIGN8          subindex;
    USIGN8          length;
    USIGN8 *        source;
    BOOL            startup_sync;
  }
    T_FBS_WRITE_PARAM_LOCAL;

  /* -------------------------------------------------------------------------
  TYPE_DESCRIPTION

    This structure is used to describe a local write access to block parameter.
    A descriptor of this type is used as input parameter of the write routine
    fbs_write_param_loc().

    rel_idx           Relative index of the parameter within the block

    subindex          For records and arrays this parameter describes the component
                      of the parameter to be written. If subindex is zero the whole
                      parameter is written

    length            Number of bytes to be written

    source            Pointer to the location where the new value is buffered

    startup_sync      This flag is TRUE if a write access is used to synchronize
                      an object of the application with an transducer or resource
                      block parameter. In this case the new value is copied to the
                      block parameter but the static revision counter is not
                      incremented and no update event is generated. Usually the
                      flag is TRUE when a parameter with a GenVFD V-flag is written
                      in the startup phase.

                      For a 'normal' local write accesses the flag is always FALSE.
  ----------------------------------------------------------------------------*/


FUNCTION GLOBAL USIGN16 fbs_write_param_loc
  (
    IN T_FBIF_BLOCK_INSTANCE *     p_block_instance,
    IN T_FBS_WRITE_PARAM_LOCAL *   p_write_loc
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function is used to write a block parameter locally.

  Please note that this function does not validate the new parameter value.
  An invalid value may lead to incorrect block or device behavior!

  IMPORTANT note on the increment of the static revision counter:

  There is the following FF-890 statement: "the associated block’s static revision
  parameter will be incremented and an update event will be generated each time a
  static parameter attribute value is changed. Also, the associated block’s static
  revision parameter may be incremented and an update event generated if a static
  parameter attribute is written but the value is not changed."

  In general the FB shell will increment the static revision counter each time a
  static parameter attribute value is changed, it will not increment the static
  revision counter if a static parameter attribute is written but the value is
  not changed.

  In GenVFD scripts static parameters are identified by a S- or a V-flag. Because
  of the internal structure of the device data base S- and V-flag parameter has
  to be handled differently to ensure a correct increment of the static revision
  counter:

  S-flag parameters have got an EEP ram image. If an S-flag parameter is written
  the FB shell will compare the block parameter with its EEP ram image. If it
  detects a difference the FB shell will increment static revision counter. If
  the block parameter equals its EEP ram image the FB shell will not increment
  the static revision counter.

  As V-flag parameters have got no EEP ram image the FB shell is unable to compare
  the block parameter with its image. Therefore, for V-flag parameters the FB shell
  compares the locations where the 'source' and 'dest' pointers of the write
  descriptor point to. Usually 'source' points to the receive buffer and 'dest'
  points to the block parameter. In this case the FB shell compares the new value
  (receive buffer) with the actual value (block parameter) and the FB shell can
  handle the static revision counter correctly. However, the comparison of 'source'
  and 'dest' will not work if 'source' is copied to 'dest' before fbs_write_param_loc()
  is called! If it is necessary to copy 'source' to 'dest' in advance the FB shell
  user is responsible for the correct increment of the static revision counter.
  He has to compare the new value with the actual value and in case of a
  difference he has to increment the static revision counter by calling the
  fbs_incr_st_rev() routine.

PARAMETERS
  p_block_instance      Pointer to the block whose parameter has to be written
  p_read                Pointer to the descriptor of the local write request

RETURN_VALUES
  E_OK                  The write access was successful
  E_FB_PARA_CHECK       The write descriptor is invalid or inconsistent
  E_FB_EEP_NO_WRITE     The storage of the parameter in the non-volatile memory
                        was not successful.
-----------------------------------------------------------------------------*/
;


/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    Write Routines                                                        */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/

FUNCTION GLOBAL USIGN16 fbs_write_param
  (
    IN T_FBIF_BLOCK_INSTANCE *    p_block_instance,
    IN T_FBIF_WRITE_DATA *        p_write
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function is used to write a block parameter. It is called by the write
  handler routines of the blocks and also by fbs_write_param_loc().

  This function
  - writes the new parameter value to the block
  - stores S- and N-parameters in non-volatile memory
  - increments the static revision counter if an S-parameter has got a new value
  - generate an update event if an S-parameter has got a new value

  IMPORTANT note on the increment of the static revision counter:

  There is the following FF-890 statement: "the associated block’s static revision
  parameter will be incremented and an update event will be generated each time a
  static parameter attribute value is changed. Also, the associated block’s static
  revision parameter may be incremented and an update event generated if a static
  parameter attribute is written but the value is not changed."

  In general the FB shell will increment the static revision counter each time a
  static parameter attribute value is changed, it will not increment the static
  revision counter if a static parameter attribute is written but the value is
  not changed.

  In GenVFD scripts static parameters are identified by a S- or a V-flag. Because
  of the internal structure of the device data base S- and V-flag parameter has
  to be handled differently to ensure a correct increment of the static revision
  counter:

  S-flag parameters have got an EEP ram image. If an S-flag parameter is written
  the FB shell will compare the block parameter with its EEP ram image. If it
  detects a difference the FB shell will increment static revision counter. If
  the block parameter equals its EEP ram image the FB shell will not increment
  the static revision counter.

  As V-flag parameters have got no EEP ram image the FB shell is unable to compare
  the block parameter with its image. Therefore, for V-flag parameters the FB shell
  compares the locations where the 'source' and 'dest' pointers of the write
  descriptor point to. Usually 'source' points to the receive buffer and 'dest'
  points to the block parameter. In this case the FB shell compares the new value
  (receive buffer) with the actual value (block parameter) and the FB shell can
  handle the static revision counter correctly. However, the comparison of 'source'
  and 'dest' will not work if 'source' is copied to 'dest' before fbs_write_param()
  is called! If it is necessary to copy 'source' to 'dest' in advance the FB shell
  user is responsible for the correct increment of the static revision counter.
  He has to compare the new value with the actual value and in case of a
  difference he has to increment the static revision counter by calling the
  fbs_incr_st_rev() routine.

PARAMETERS
  p_block_instance    Pointer to the block whose parameter is written

  p_write             Poitner to the write descriptor

RETURN_VALUES
  E_OK                Writing was successful
  E_FB_EEP_NO_WRITE   Storage in non-volatile memory was not successful
----------------------------------------------------------------------------*/
;


FUNCTION GLOBAL VOID fbs_write_extra_nv_data
  (
    IN USIGN8  block_id
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function triggers the background task to write the block-specific extra
  nv-data in the non-volatile memory. The extra nv-data will be stored during
  the next execution of the background task.

PARAMETERS
  block_id        Identifier of the block whose extra nv-data have to be stored
                  in the non-volatile memory

RETURN_VALUES
  None
----------------------------------------------------------------------------*/
;


FUNCTION GLOBAL USIGN16 fbs_incr_st_rev
  (
    IN T_FBIF_BLOCK_INSTANCE *     p_block_instance,
    IN USIGN16                     rel_idx
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function is used to increment the static revision counter of a block.

  Usually the incrementation of the static revision counter is done by the
  fbs_write_param() routine. However, there is one exception: if it is
  attributed with the GenVFD X-flag a parameter is completely under the
  control of the application. If a X-parameter is 'Static' the application
  has to call this function each time the parameter has got a new value.

  For all other parameters it is not necessary and even wrong to call this
  function!

PARAMETERS:
  p_block_instance    Pointer to the block whose static revision counter has
                      to be incremented

  rel_idx             Relative index of the block parameter which has been
                      changed

RETURN_VALUES:
  E_OK                Static revision counter has been incremented
  E_FB_EEP_NO_WRITE   It was not possible to store the new static revision
                      counter in the non-volatile memory.
--------------------------------------------------------------------------*/
;




/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    Alert Notifications                                                   */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/

  /* Types of alert notification */
  #define  UPDATE_EVENT              0
  #define  ALARM_CLEAR               0
  #define  ALARM_ACTIVE              1

  /* State of Update Event */
  #define STORE_UPDATE               0
  #define SEND_UPDATE                9

  typedef struct
  {
    USIGN16      subcode;
    FLOAT        value;
  }
    T_FBS_ALERT_NOTIFY_FLOAT_DATA;

  typedef struct
  {
    USIGN16      subcode;
    USIGN8       value;
    USIGN16      mbit_mask;
  }
    T_FBS_ALERT_NOTIFY_DISCRETE_DATA;

  typedef struct
  {
    USIGN16      static_revision;
    USIGN16      relative_index;
  }
    T_FBS_ALERT_NOTIFY_EVENT_DATA;

  typedef struct
  {
    USIGN32      subcode;
    USIGN8       value;
    USIGN16      alarm_src;
    USIGN32      mbit_mask;
  }
    T_FBS_ALERT_NOTIFY_FD_DIAG_DATA;

  typedef union
  {
    T_FBS_ALERT_NOTIFY_FLOAT_DATA     flt;
    T_FBS_ALERT_NOTIFY_DISCRETE_DATA  dsc;
    T_FBS_ALERT_NOTIFY_EVENT_DATA     evt;
    T_FBS_ALERT_NOTIFY_FD_DIAG_DATA   fld;
  }
    T_FBS_ALERT_NOTIFY_DATA;

  typedef struct
  {
    USIGN8                    action;
    USIGN8                    block_id;
    USIGN16                   rel_idx;
    USIGN16                   src_idx; /* to support field diag */
    USIGN8                    priority;
    T_FBS_ALERT_NOTIFY_DATA   data;
  }
    T_FBS_ALERT_NOTIFY;

  /* -------------------------------------------------------------------------
  TYPE_DESCRIPTION
    This structure is used to describe an alert notification.

    action        Type of alert. Possible values: ALARM_ACTIVE, ALARM_CLEAR,
                  UPDATE_EVENT

    block_id      Block identifier as generated by GenVFD

    rel_idx       Relative index of the alert parameter within the block

    src_idx       For field diagnostics only: OD Index of the block that
                  triggered the alarm; 0 if the FD alarm is not associated
                  to a certain block.

    priority      Priority of the alert

    data          Descriptor specific to the alert type.
                  Possible types: float alarm, discrete alarm, update event


    Descriptor for float alarms - T_FBS_ALERT_NOTIFY_FLOAT_DATA:

    subcode       Subcode of the float alarm

    value         Value of the associated float parameter at the time the
                  alert was detected


    Descriptor for discrete alarms - T_FBS_ALERT_NOTIFY_DISCRETE_DATA:

    subcode       Subcode of the discrete alarm

    value         Value of the associated discrete parameter at the time the
                  alert was detected

    mbit_mask     Bit-string that describe which bit became active or clear resp.
                  This bit-string MUST contain only one bit. If multiple bits are
                  set the FB will loose or duplicate multibit-alarms!


    Descriptor for update events - T_FBS_ALERT_NOTIFY_EVENT_DATA:

    static_revision   Static revision of the block whose static parameter was
                      changed

    relative_index    Relative index of the static parameter whose change caused
                      this alert. If the update event was caused by a write to
                      multiple parameters at the same time, then this attribute
                      will be zero.
  ----------------------------------------------------------------------------*/



FUNCTION GLOBAL VOID fbs_alert_notify
  (
    IN T_FBS_ALERT_NOTIFY *   p_alert_notify
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function is used for alert notifications

PARAMETERS
  p_alert_notify      Pointer to alert descriptor block

RETURN_VALUES
  None
----------------------------------------------------------------------------*/
;



/* #define fbs_update_mbit_unack_flag(block_id,rel_idx,p_unack_flag) */

/* Due to FF-890 FS 1.8 and ITK 5.2 it is not necessary anymore to update the
   alarm unack flag when an acknowledge event notification is received. Therefore
   the function fbs_update_mbit_unack_flag() is not required anymore.

   This empty macro is just defined to keep the compatibility with the FBS
   interface of the FF-FD-SW 2.2x
*/


/* #define fbs_acknowledge_mbit_alarm(block_id,rel_idx) */

/* Due to FF-890 FS 1.8 and ITK 5.2 it is not necessary anymore to update the
   alarm unack flag when an acknowledge event notification is received. Therefore
   the function fbs_acknowledge_mbit_alarm() is not required anymore.

   This empty macro is just defined to keep the compatibility with the FBS
   interface of the FF-FD-SW 2.2x
*/






/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    Access to Block Instances                                             */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/

FUNCTION GLOBAL T_FBIF_BLOCK_INSTANCE * fbs_get_block_inst
  (
    IN USIGN8     block_id
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function provides the pointer to a block instance.

  This function must not be called before fbs_start() is executed!

PARAMETERS
  block_id      Block identfier as generated by GenVFD (defined in fbif_idx.h)

RETURN_VALUES
  !NULL         Pointer to a block instance
  NULL          Invalid block identifier, block may be inactive/invisible
----------------------------------------------------------------------------*/
;




/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    Functions Called by the Resource Block                                */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/

FUNCTION GLOBAL VOID fbs_restart_with_defaults (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function is called by the write handler routine of the resource block.
  During its execution all blocks, trend objects, link objects and alert
  objects are reset to its default values. There is one exception: the block-
  tags are not reset.

PARAMETERS
  None

RETURN_VALUES
  None
----------------------------------------------------------------------------*/
;


FUNCTION GLOBAL BOOL fbs_links_online (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function checks whether the connections of all configured non-local links
  are established. It is called by the application when the RS_STATE of the
  resource block has to be updated (usually when the RS_STATE parameter is read).

PARAMETERS
  None

RETURN_VALUES
  TRUE      All connections are established. The state of the resource block
            is ONLINE

  FALSE     There are connections which are not established. The state of the
            resource block is ONLINE-LINKING
----------------------------------------------------------------------------*/
;




/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    Functions Called by Function Blocks                                   */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/

typedef struct
{
  T_FBIF_BLOCK_INSTANCE *   p_tb_instance;
  GLOBAL VOID               (* a_of_execute_tb) (struct _T_FBIF_BLOCK_INSTANCE *,struct _T_FBIF_PROCESS_DATA *);
}
  T_LINKED_TB;


FUNCTION GLOBAL USIGN16 fbs_link_transducer_block
  (
    IN USIGN16                      fb_channel,
    IN const T_FBIF_BLOCK_DESCR *   p_block_desc,
    OUT T_LINKED_TB *               p_linked_tb
  );

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function is called by the check-configuration routines of function blocks
  with a channel parameter.

PARAMETERS
  fb_channel      Value of the channel parameter

  p_block_desc    Pointer to the descriptor of the calling function block (*)

  p_linked_tb     Pointer to the linked transducer block


  (*) The function block descriptor contains a list with all
      'channel <-> transducer block' pairs valid for this function block. The
      list is generated from the CHANNEL_INFORMATION in the GenVFD scripts.


RETURN_VALUES
  !NULL           A transducer block that matches the channel was found
  NULL            No matching transducer block was found
----------------------------------------------------------------------------*/


#ifdef UNPACKED_ACCESS
FUNCTION GLOBAL USIGN8  fbsacc_get_st_dt_length
  (
    USIGN16     index_of_type
  );
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

  The function is used to read the length of an static data type description

PARAMETERS
  index_of_type  Index of the data type description



RETURN_VALUES
   length of the static data type description
----------------------------------------------------------------------------*/
#endif /* UNPACKED_ACCESS */



/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    Modular Devices and Block Instantiation                               */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/

FUNCTION GLOBAL USIGN8 fbs_set_block_state
  (
    IN USIGN16  block_id,
    IN USIGN8   action
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function may be used to set the active/inactive attribute of a block.

  The default value of the active/inactive attribute of a block is given by
  the input from the GenVFD script. If the GenVFD key BLOCK_INSTANCE is missing
  or set to VISIBLE the block is active by default. If BLOCK_INSTANCE has been
  set to HIDDEN the block is inactive by default.

  This function can be used to overwrite the default value of the active/inactive
  attribute of a block.

  The active /inactive attribute features capability levels. The block list
  described in the GenVFD script is the superset of all capability levels.
  This function allows to update the block list so that it matches the
  implemented capability level.

  In the GenVFD script you may set all blocks to be active. In this case you
  have to disable the unused blocks in the device startup phase. But it is
  also possible to set in the GenVFD script the blocks in the intersection
  of all capability levels to be active. In this case you have to enable the
  blocks completing the capability level during the device startup phase.

  IMPORTANT : The call of this function is allowed during startup phase only!
              It must be called  a f t e r  fbs_check_nv_ram() and
              fbs_apply_for_nv_ram() but  b e f o r e  the execution of
              fbs_start().

  This function works slightly different in device without and with block
  instantiation.

  If block instantiation is not supported then the FB shell does not store
  the active/inactive attribute persistently. I.e. the device will always
  run with the block list described in the GenVFD script and adapted by
  this function.

  If block instantiation is supported then the FB shell stores the
  active/inactive attribute in non-volatile memory. Block instantiation
  means that block activation and de-activation is done remotely via the
  FF bus. To avoid conflicts between the remote and the local settings the
  active/inactive attribute of a function block is taken from the GenVFD
  script only when the device starts with a new configuration: If the
  device starts with a new configuration then the initial function block
  list is build from the GenVFD script and from the settings done with
  this function. If the device re-starts with an existent configuration
  then the FB shell will work with the block list stored in non-volatile
  memory. In case of a re-start the calls of this function are ignored when
  they refer to function blocks.

  Please note: if your device supports block instantiation or has got
  capability levels then the resource block requires the common practice
  parameter CAPABILITY_LEV!

PARAMETERS
  block_id          Block identifier as generated by GenVFD

  action            BLK_INACTIVE  Block will be active/visible
                    BLK_ACTIVE    Block will be inactive/hidden

RETURN_VALUES
  E_OK              The requested action was successful
 !E_OK              The requested action was not successful
----------------------------------------------------------------------------*/
;




FUNCTION GLOBAL USIGN8 fbs_ask_block_state
  (
    IN USIGN16  block_id
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
  This function may be used to ask for if a block is currently active or inactive.

  This function must not be called before the execution of fbs_start()!

PARAMETERS:
  block_id            Block identifier as generated by GenVFD

RETURN_VALUES:
  BLK_ID_INVALID (0)  A block with identifier 'block_id' was not found OR
                      the block table is not accessible
  BLK_INACTIVE (1)    The block with identifier 'block_id' is inactive/not visible
  BLK_ACTIVE (2)      The block with identifier 'block_id' is active/visible
----------------------------------------------------------------------------*/
;

#endif /* __FBS_API_H__ */
