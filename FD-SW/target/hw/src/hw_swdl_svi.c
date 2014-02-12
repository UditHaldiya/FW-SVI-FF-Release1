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

FILE_NAME          hw_swdl_svi.c

FUNCTIONAL_MODULE_DESCRIPTION

Module description

=========================================================================== */
#include "keywords.h"   /* keyword defines */
#define  MODULE_ID      (COMP_HW + MOD_HWSWDL)

INCLUDES
#include <string.h>

#include "base.h"
#include "osif.h"
#include "except.h"
#include "hw_cfg.h"
#include "hw_if.h"
#include "hw_svi_swdl_if.h"

GLOBAL_DEFINES

LOCAL_DEFINES

EXPORT_TYPEDEFS

LOCAL_TYPEDEFS

FUNCTION_DECLARATIONS

EXPORT_DATA

IMPORT_DATA

LOCAL_DATA




/*---- HW Flash Erase/Write Functions -------------------------------------*/




FUNCTION GLOBAL BOOL flash_GetActiveBank (void)
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

  This function is used to determine in which partition the active (running) firmware resides.

PARAMETERS
  None

RETURN_VALUES
  The return value is true (1) if the current partition is 1; false (0) if the current partition is 0. 

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  return FALSE;
}




FUNCTION GLOBAL BOOL flash_OpenPartition(void)
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

  This function opens a flash firmware partition for update. It unlocks the bank
  in which the partition resides, and erases the first page (2048 bytes) of the 
  partition thus invalidating the old image, if any. 

PARAMETERS
  None

RETURN_VALUES
  

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  
FUNCTION_BODY

  /*
  1.	A bank is not specified because it is only permitted to select the opposite bank 
      from which the firmware is currently running.
  2.	See flash_ProgramBlock() for more information. 
  3.	If the return is false use flash_GetStatus() to determine error.  
  */
  
  return TRUE;
}




FUNCTION GLOBAL BOOL flash_ClosePartition(void)
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

  This function closes the flash bank after an update. It locks the bank that was 
  previously opened and updated.  The first 8 bytes of the partition are written 
  from a saved area thus validating the new image (assuming one was downloaded). 

PARAMETERS
  None

RETURN_VALUES
  

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  /*
  If the return is false use flash_GetStatus() to determine error.
  A bank is not specified because the currently open bank is implied.
  */
  
  return TRUE;
}




FUNCTION GLOBAL USIGN32 flash_GetStatus (void)
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

  This function returns the result code from a previously failed flash_OpenPartition, 
  flash_ClosePartition, flash_ProgramBlock() or flash_SetPartition() call.

PARAMETERS
  None

RETURN_VALUES
  A bitmap of errors that occurred during one of the above calls.
  #define EOP (1u << 5)         // normal - not an error
  #define WRPRTERR (1u << 4)    // device write protected
  #define PGERR (1u << 2)       // program or erase error
  #define BSY (1u << 0)         // device still busy after timeout period 
  #define RNG (1u << 6)         // attempt to program/erase outside of bank address range

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  return EOP;
}




FUNCTION GLOBAL BOOL flash_ProgramBlock
(
  const USIGN16 * data,
  USIGN16 *       adr,
  USIGN32         len
)
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

  Allows programming the flash memory. data This function returns a bitmap of 
  error codes pertaining to the most recent call to functions:  flash_ProgramBlock() 
  and flash_SetPartition().

PARAMETERS
  data - a pointer to the beginning of data in RAM to be programmed
  adr  - a pointer to the beginning flash address
  len  - the length of the block in bytes - must be an even number.


RETURN_VALUES
  true (1) if successful; false (0) if error.  Use flash_GetStatus() to determine error.

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  /*
  1.	data and adr must be aligned on a 16 bit boundary.
  2.	The block must reside entirely within the open partition.
  3.	See the Bank and Partition diagram for addresses.
  4.	Writes to the first 8 bytes of the partition are stored in a temporary buffer 
      until the flash_ClosePartition() is performed.
  5.	Blocks must be written sequentially and page erase is automatically performed 
      at page boundaries.
  6.	If the return is false use flash_GetStatus() to determine error.
  */
  
  _UNREF_PARAM(data);
  _UNREF_PARAM(adr);
  _UNREF_PARAM(len);

  return TRUE;
}




FUNCTION GLOBAL BOOL flash_SetPartition
(
  BOOL bootFromBank2
)
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

  Allows selection of the preferred boot partition. 

PARAMETERS
  BootFromBank2 - false (0) selects partition 0 as preferred; true (1) sets 
                  partition 1 as preferred. 

RETURN_VALUES
  true (1) if successful; false (0) if error. 

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  /*
  If the return is false use flash_GetStatus() to determine error.
  */
  
  return TRUE;
}




FUNCTION GLOBAL USIGN16 * flash_PageSum
(
  BOOL bank
)
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

  Creates an array of CRCs for the specified firmware bank. A 16-bit CRC is 
  computed for each 2048 byte page.

PARAMETERS
  bank - false (0) selects bank 0; true (1) selects bank 1.

RETURN_VALUES
  A pointer to an array of 16-bit CRC values.

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  /*
  1.	This function may be used to verify and/or restart a firmware download.
  2.	If this function is used to restart an interrupted download, the partition must be opened (flash_OpenPartition()) and the first page must be rewritten in addition to any other pages that require update.  Other pages that already have the correct checksum need not be rewritten
  3.	The CRC polynomial is the same as that used for FRAM integrity (CRC-CCITT  x16 + x12 + x5 + 1)  
  4.	This operation computes the CRC on 256 Kbytes of flash.
  5.	This function takes several seconds to complete and should only be called from the Idle Task context.
  */

  return NULL;
}




FUNCTION GLOBAL USIGN32 flash_GetBaseForThisBank(void)
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

  Returns the lowest (base) address of the bank from which code is currently executing.

PARAMETERS
  None

RETURN_VALUES
  

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  /*
  The address returned is the hardware base of the bank.  This is where the 
  boot manager code resides. The firmware, if any, occurs at a higher address.
  */
  
  return 0x8000800uL;
}




FUNCTION GLOBAL USIGN16 flash_pageCRC
(
  USIGN32 Address
)
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

  Returns the CRC of the 2 Kbyte page indicated by Address.

PARAMETERS
  Address - the base (flash) address of the 2 Kbyte page.

RETURN_VALUES
  The computed 16-bit CRC value.

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  
  return 0;
}




FUNCTION GLOBAL BOOL flash_VerifyAddress
(
  USIGN32 Address
)
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

  Returns true if the specified Address is within the limits of the page the 
  code is NOT executing from.

PARAMETERS
  Address - the memory address to be tested within the limits.

RETURN_VALUES
  True if the address is within the page that we are NOT executing from; otherwise false.

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  /*
  The limits are inclusive of firmware partition address and do not include the boot 
  manager at the beginning of the bank.
  */  
  
  return TRUE;
}




FUNCTION GLOBAL USIGN32 flash_GetPartitionBaseAddress(void)
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

  Returns the first (base) address of partition from which code is NOT executing.

PARAMETERS
  None.

RETURN_VALUES
  The address of the firmware partition from which the code is NOT executing. 
  I.E. the partition that could be programmed with a new version of firmware.

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  /*
  The base address is for the firmware partition address and does not include 
  the boot manager at the beginning of the bank.
  */  
  
  return 0x8080800;
}
