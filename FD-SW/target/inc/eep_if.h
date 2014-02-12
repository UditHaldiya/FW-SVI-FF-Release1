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

FILE_NAME          eep_if.h



FUNCTIONAL_MODULE_DESCRIPTION

  This module contains definitions and function prototypes for the
  higher level access of the eeprom.

=========================================================================== */
#ifndef __EEP_IF__
#define __EEP_IF__

#include <keywords.h>   /* keyword defines */
#include "errcodes.h"

INCLUDES

GLOBAL_DEFINES

/* ------------------------------------------------------------------------ */
/* --- Block IDs to access the EEPROM data blocks ------------------------- */
/* ------------------------------------------------------------------------ */

#define EEP_APPL_BLK1           1
#define EEP_APPL_BLK2           2
#define EEP_APPL_BLK3           3
#define EEP_APPL_BLK4           4       /* Reserved for FF block instantiation              */

#define EEP_PROT_BLK1           5       /* DO NOT CHANGE!! Referred by protocol software    */
#define EEP_PROT_BLK2           6       /* DO NOT CHANGE!! Referred by protocol software    */
#define EEP_PROT_BLK3           7       /* DO NOT CHANGE!! Referred by protocol software    */
#define EEP_PROT_BLK4           8       /* DO NOT CHANGE!! Referred by protocol software    */
#define EEP_PROT_BLK5           9       /* DO NOT CHANGE!! Referred by protocol software    */
#define EEP_PROT_BLK6          10       /* DO NOT CHANGE!! Referred by protocol software    */
#define EEP_PROT_BLK7          11       /* DO NOT CHANGE!! Referred by protocol software    */
#define EEP_PROT_BLK8          12       /* DO NOT CHANGE!! Referred by protocol software    */
#define EEP_PROT_BLK9          13       /* DO NOT CHANGE!! Referred by protocol software    */
#define EEP_PROT_BLK10         14       /* DO NOT CHANGE!! Referred by protocol software    */
#define EEP_PROT_BLK11         15       /* DO NOT CHANGE!! Referred by protocol software    */

#define EEP_FBL_BLK1           16
#define EEP_FBL_BLK2           17
#define EEP_FBL_BLK3           18
#define EEP_FBL_BLK4           19
#define EEP_FBL_BLK5           20
#define EEP_FBL_BLK6           21
#define EEP_FBL_BLK7           22
#define EEP_FBL_BLK8           23
#define EEP_FBL_BLK9           24
#define EEP_FBL_BLK10          25
#define EEP_FBL_BLK11          26
#define EEP_FBL_BLK12          27
#define EEP_FBL_BLK13          28
#define EEP_FBL_BLK14          29
#define EEP_FBL_BLK15          30
#define EEP_FBL_BLK16          31
#define EEP_FBL_BLK17          32
#define EEP_FBL_BLK18          33
#define EEP_FBL_BLK19          34
#define EEP_FBL_BLK20          35
#define EEP_FBL_BLK21          36
#define EEP_FBL_BLK22          37
#define EEP_FBL_BLK23          38
#define EEP_FBL_BLK24          39
#define EEP_FBL_BLK25          40
#define EEP_FBL_BLK26          41
#define EEP_FBL_BLK27          42
#define EEP_FBL_BLK28          43
#define EEP_FBL_BLK29          44
#define EEP_FBL_BLK30          45
#define EEP_FBL_BLK31          46
#define EEP_FBL_BLK32          47

#define EEP_NV_BLK1            48
#define EEP_NV_BLK2            49
#define EEP_NV_BLK3            50
#define EEP_NV_BLK4            51       /* IDs must not exceed EEP_NBR_OF_BLOCKS - 1        */

#define EEP_DWNLD_BLK1         52

#define EEP_NBR_OF_BLOCKS      53       /* May be adjusted, if needed                       */


#define NV_EEP_BLK             EEP_NV_BLK1


#ifdef USE_NV_DATA_DOUBLE_BUFFER

#define MAX_NV_DATA_BLOCKS     2
#define NV_EXTRA_EEP_BLK       EEP_NV_BLK3

#else

#define MAX_NV_DATA_BLOCKS     1
#define NV_EXTRA_EEP_BLK       EEP_NV_BLK2

#endif

#define EXTRA_NV_ADJUST        sizeof(T_CHKSUM)      /* Used to calculate the address of the checksum */

LOCAL_DEFINES

EXPORT_TYPEDEFS

/* EEP function return values */
typedef enum _T_EEP_RESULT
{
    EEP_OK                  = 0,
    EEP_NOT_AVAIL           = 1,
    EEP_NOT_EXIST,
    EEP_MEM_INSUFF,
    EEP_PARAM_ERR,
    EEP_READ_ERR,
    EEP_PRG_ERR,
    EEP_CS_ERR,
    EEP_INCONSISTENT,
    EEP_INVALID_SERVICE     = 0x20,
    EEP_IN_CHANGE           = 0xF0
} T_EEP_RESULT;

typedef  USIGN16 (FAR_C * EEP_INI_FCT)(USIGN8 FAR_D *, T_EEP_RESULT);

typedef enum
{
    NO_INI,                             /* No initialisation required               */
    DATA_INI,                           /* memcopy from data area                   */
    FCT_INI                             /* call initialsation function              */
} T_EEP_INI_TYPE;

typedef union
{
    void FAR_D *        ptr_data;       /* pointer to initialisation data block     */
    EEP_INI_FCT         ptr_fct;        /* pointer to initialisation function       */
} T_EEP_INI_PTR;

/* offset and length of an EEPROM block */
typedef struct T_EEP_BLOCK_DSC
{
    USIGN8              block_id;       /* block id                                 */
    USIGN8              auto_write_length;  /* size if auto-store data              */
    USIGN16             block_size;     /* size of block                            */
}
T_EEP_BLOCK_DSC;

typedef USIGN16         T_CHKSUM;       /* type of the checksum             */

typedef enum {
    BLK_OK                  = 0,
    BLK_NOT_RDY,
    BLK_ERR,
    BLK_WRITE,
    BLK_VERIFIED
} T_BLK_STATE;

typedef enum {
    EEP_VALID               = 0,
    EEP_UNAVAIL,
    EEP_IN_CONFIG
} T_EEP_STATE;

typedef struct _T_EEP_DSC {
    USIGN8              id;             /* block id                         */
    USIGN8              auto_size;      /* size of auto-store data          */
    USIGN16             len;            /* size of data block               */
    USIGN8 FAR_D *      start_addr;     /* start_addr of data block         */
    T_CHKSUM FAR_D *    checksum;       /* pointer to block checksum        */
    T_BLK_STATE         state;          /* current block state              */
} T_EEP_DSC;

IMPORT_DATA

EXPORT_DATA

FUNCTION_DECLARATIONS

FUNCTION GLOBAL T_EEP_RESULT eep_provide_block
(
    IN  USIGN8      block_id,           /* ID of block                      */
    IN  USIGN16     block_len,          /* size of block                    */
    OUT VOID * *    p_ram_image         /* pointer to data image            */
)
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

Checks existence / consistency of a block and provides a pointer to the block
start address, if successful, NULL in case of error.

RETURN_VALUE
    EEP_OK              => no error, '*p_ram_image' points to start of block
    EEP_NOT_EXIST       => block with ID 'block_id' does not exist
    EEP_INCONSISTENT    => size of block does not agree with 'block_len'
    EEP_CS_ERR          => checksum of block invalid -> data probably corrupt
    EEP_READ_ERR        => EEPROM could not be read
----------------------------------------------------------------------------*/
;



FUNCTION GLOBAL T_EEP_RESULT eep_config_change
(
    IN USIGN8               no_of_eep_block_dsc, /* number of descriptors       */
    IN T_EEP_BLOCK_DSC *    p_eep_block_dsc      /* pointer to descriptor array */
)
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
Inserts 'no_of_eep_block_dsc' block descriptions into the global descriptor
structure. If descriptions for certain block IDs are already existing, they
are overwritten.

possible return values:
    EEP_OK              => completed successfully
    EEP_PARAM_ERR       => block ID(s) exceed limit
------------------------------------------------------------------------------*/
;


FUNCTION GLOBAL T_EEP_RESULT eep_config_act (VOID)
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
Activates the new EEP configuration, i.e. builds new management structures and
writes the structure layout information to the EEPROM (if successful)

possible return values:
EEP_OK          => activation succeeded
EEP_MEM_INSUFF  => data does not fit into EEPROM
EEP_PRG_ERR     => error while programming EEPROM
------------------------------------------------------------------------------*/
;


FUNCTION GLOBAL T_EEP_RESULT eep_start_write
(
    IN  USIGN8   block_id               /* ID of block                       */
)
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
Prepares a block for write operations.

RETURN_VALUE
    EEP_OK          => no error occured
    EEP_PARAM_ERR   => wrong parameter
    EEP_IN_CHANGE   => EEP configuration not consistent temporarily
---------------------------------------------------------------------------- */
;



/* mode value for eep_write (), eep_restore () */
#define EEP_WAIT          0x01          /* wait until writing is finished    */

FUNCTION GLOBAL T_EEP_RESULT eep_write
(
    IN  USIGN8   block_id,              /* ID of block                       */
    IN  USIGN16  offset,                /* data offset inside the block      */
    IN  USIGN16  length,                /* length of data in bytes           */
    IN  USIGN8   mode                   /* selected write mode               */
)
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

Writes data to the RAM image and, if needed, to the EEPROM. The only valid
value for the 'mode' parameter is :
  EEP_WAIT          the function returns after the final write cycle is
                    completed in the EEPROM device
If 'offset' == 0 and 'length' == 0 the whole block is written.

RETURN_VALUE
    EEP_OK          => no error occured
    EEP_PARAM_ERR   => wrong parameter
    EEP_PRG_ERR     => error while programming EEPROM
    EEP_IN_CHANGE   => EEP configuration not consistent temporarily
----------------------------------------------------------------------------*/
;



FUNCTION GLOBAL T_EEP_RESULT eep_restore
(
    IN  USIGN8   block_id,              /* ID of block                       */
    IN  USIGN16  offset,                /* data offset inside the block      */
    IN  USIGN16  length,                /* length of data in bytes           */
    IN  USIGN8   mode                   /* selected write mode               */
)
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

Reads block data from the physical EEPROM device, overwriting existing
data in the RAM image. The only valid value for the 'mode' parameter is:
  EEP_WAIT          the function returns after the final write cycle is
                    completed in the EEPROM device
If 'offset' == 0 and 'length' == 0 the whole block is written.

RETURN_VALUE
    EEP_OK          => no error occured
    EEP_PARAM_ERR   => wrong parameter
    EEP_READ_ERR    => error while reading from EEPROM
    EEP_IN_CHANGE   => EEP configuration not consistent temporarily
----------------------------------------------------------------------------*/
;



FUNCTION GLOBAL T_EEP_RESULT eep_verify
(
    IN  USIGN8   block_id           /* ID of block                          */
)
/*----------------------------------------------------------------------------

FUNCTIONAL_DESCRIPTION

Performs verification of an EEPROM data block against the according RAM
image location. If the RAM image is altered concurrently, the function
returns 'EEP_IN_CHANGE'.

RETURN_VALUE
    EEP_OK          => no error occured
    EEP_PARAM_ERR   => wrong parameter
    EEP_READ_ERR    => error while reading from EEPROM
    EEP_IN_CHANGE   => EEP data verification temporarily not possible
    EEP_CS_ERR      => checksum of block invalid
    EEP_INCONSISTENT=> EEP data do not agree with RAM image data

----------------------------------------------------------------------------*/
;



FUNCTION GLOBAL T_EEP_RESULT eep_reset_extra_nv_ram
  (
    IN   USIGN8        unsorted_block_id,   /* EEP/FRAM block identifier (unsorted)   */
    IN   USIGN8    *   p_ram_addr,          /* Address in the extra NV block in RAM   */
    IN   USIGN32       eep_offset,          /* Offset of the extra NV block in NV RAM */
    IN   USIGN16       length               /* Length of the specific block in NV RAM */
  )
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

  The function is used to clear the extra NV RAM in EEPROM/FRAM and
  in the memory.

PARAMETERS

  unsorted_block_id               Unsorted block identifier of the extra NV
                                  RAM in EEPROM/FRAM.

  p_ram_addr                      Address of the extra NV ram.

  eep_offset                      Offset inside the EEPROM/FRAM.

  length                          Length of the sub-block.

RETURN_VALUE

   EEP_OK     No error
  !EEP_OK     Error

----------------------------------------------------------------------------*/
;

FUNCTION GLOBAL BOOL eep_startup_for_the_first_time (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

  The function checks if the device was started for the first time.

RETURN_VALUE

  TRUE    Device was started for the firt time or was re-initialized.

  FALSE   Device was NOT started for the first time.

----------------------------------------------------------------------------*/
;
extern GLOBAL T_EEP_RESULT eep_init (VOID);

//Additions for thread safety
extern void eep_CreateMutex(void);
extern void eep_AcquireMutex(void);
extern void eep_ReleaseMutex(void);

typedef struct _T_EEP_INFO {
    USIGN16             size_eep_data;  /* size of data area                */
    USIGN8              num_blocks;     /* number of existing blocks        */
    USIGN8              page_size;      /* size of EEPROM pages             */
    T_EEP_STATE         state;          /* current EEPROM state             */
    u16 /*T_EEP_ADDR */         start_data;     /* start address of data area       */
    T_CHKSUM            crc;            /* checksum over block descriptions */
    u16 CheckWord;
} T_EEP_INFO;

/** \brief returns a time stamp of write cycle start
\return a time stamp
*/
extern u32 eep_get_stamp(void);

//--------------- testing API -----------------
extern u8 eeptest_GetStats(u32 *repair_count, u32 *repair_failed, u32 *eep_write_time);
extern void eep_GetInfo(T_EEP_INFO *dst);
extern void eep_GetBlockInfo(u8 b, u32 *offset, u32 *length);
extern void eeptest_WriteAllBlocks(u8 clear_max_time, u8 full_load);
extern ErrorCode_t eep_read_block(void *dst_addr, u32 eeprom_addr, u16 count);
extern ErrorCode_t eep_write_block(u16 eeprom_addr, const void *src_addr, u16 count);

#endif /* __EEP_IF__ */

