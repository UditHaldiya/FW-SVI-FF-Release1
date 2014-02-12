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

FILE_NAME          fbif.h



DESCRIPTION
  This file contains type definitions used by GenVFD when it generates the
  block descriptions.

=========================================================================== */
#ifndef __FBIF_H__
#define __FBIF_H__


#include "fbap.h"
#include "vfd.h"

/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    Block Descriptor and Block Instance                                   */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/

/* ------------------------------------------------------------------------ */
/*     Macros                                                               */
/* ------------------------------------------------------------------------ */
  #define BLK_UNUSED        0
  #define BLK_ID_INVALID    0
  #define BLK_INACTIVE      1
  #define BLK_ACTIVE        2

  #define RES_BLK           1
  #define TRANS_BLK         2
  #define FUNCT_BLK         3

  #define EEP_NO_OFFSET  0xFFFF


/* ------------------------------------------------------------------------ */
/*    GenVFD flags                                                          */
/* ------------------------------------------------------------------------ */
  #define FB_STATIC_PARAM          0x0001   /* S flag */
  #define FB_APPL_READ_PARAM       0x0002   /* A flag */
  #define FB_APPL_WRITE_PARAM      0x0004   /* B flag */
  #define FB_NON_VOLATILE_PARAM    0x0008   /* N flag */
  #define FB_READ_PARAM            0x0010   /* R flag */
  #define FB_WRITE_PARAM           0x0020   /* W flag */
  #define FB_OUTPUT_PARAM          0x0040   /* O flag */
  #define FB_INPUT_PARAM           0x0080   /* I flag */
  #define FB_STATIC_VOLATILE_PARAM 0x0100   /* V flag */
  #define FB_PARAM_NOT_IN_FBIF     0x0200   /* X flag */
  #define FB_DEFAULT_FROM_APPL     0x0400   /* Y flag */
  #define FB_RESTART_NO_CHANGE     0x0800   /* Z flag */



/* ------------------------------------------------------------------------ */
/*    Type Definitions                                                      */
/* ------------------------------------------------------------------------ */
  typedef struct
  {
    F_BLOCK    blk_header;
    SCALE      xd_scale;
    USIGN16    channel;
  }
    T_BLOCK_INST_DEFAULT;


  typedef struct
  {
    USIGN16    flags;
    USIGN16    par_offset;
    USIGN16    eep_offset;
    USIGN16    par_size;
    USIGN32    SubIndexAccess;
  }
    T_FBIF_PARAM_DESCR;


  typedef struct
  {
    USIGN16   channel;
    USIGN8    linked_transducer_block;
  }
    T_FBIF_CHANNEL_LIST;


  struct _T_FBIF_BLOCK_INSTANCE;    /* Forward Declaration */
  struct _T_FBIF_PROCESS_DATA;      /* Forward Declaration */
  struct _T_FBIF_READ_DATA;         /* Forward Declaration */
  struct _T_FBIF_WRITE_DATA;        /* Forward Declaration */


  typedef struct _T_FBIF_BLOCK_DESCR
  {
    USIGN16                           block_id;
    USIGN16                           start_index;
    USIGN8                            block_status;
    USIGN8                            block_type;
    USIGN16                           block_len;
    USIGN16                           block_eep_len;
    USIGN16                           block_n_par_len;
    const VOID *                      p_block_type_dft;
    const T_BLOCK_INST_DEFAULT *      p_block_dft;
    VOID *                            p_block;
    USIGN16                           no_of_param;
    const T_FBIF_PARAM_DESCR *        param_dir;
    const T_BLK_OBJ_DESC * const *    list_of_obj_desc;
    const VAR_LIST_OBJ_DESC * const * list_of_views;
    USIGN16                           idx_upd_evt;
    USIGN16                           idx_blk_alm;
    USIGN16                           no_of_channels;
    const T_FBIF_CHANNEL_LIST *       channel_list;
    GLOBAL VOID                       (* a_of_start_block)(struct _T_FBIF_BLOCK_INSTANCE *);
    GLOBAL VOID                       (* a_of_stop_block) (struct _T_FBIF_BLOCK_INSTANCE *);
    GLOBAL USIGN16                    (* a_of_execute_fb) (struct _T_FBIF_BLOCK_INSTANCE *);
    GLOBAL VOID                       (* a_of_execute_tb) (struct _T_FBIF_BLOCK_INSTANCE *,struct _T_FBIF_PROCESS_DATA *);
    GLOBAL VOID                       (* a_of_background) (struct _T_FBIF_BLOCK_INSTANCE *);
    GLOBAL USIGN16                    (* a_of_read_hndl)  (struct _T_FBIF_BLOCK_INSTANCE *,struct _T_FBIF_READ_DATA *);
    GLOBAL USIGN16                    (* a_of_write_hndl) (struct _T_FBIF_BLOCK_INSTANCE *,struct _T_FBIF_WRITE_DATA *);
  }
    T_FBIF_BLOCK_DESCR;


  typedef struct _T_FBIF_BLOCK_INSTANCE
  {
    const T_FBIF_BLOCK_DESCR *        p_block_desc;
    VOID *                            p_block_data;
  }
    T_FBIF_BLOCK_INSTANCE;
  /* ------------------------------------------------------------------------
  TYPE_DESCRIPTION
    The components of the structure are

    p_block_desc      Pointer to the block descriptor.
    p_block_data      Pointer to the instance-specific data.
  ------------------------------------------------------------------------- */






/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    Interface between Function Blocks and Transducer Blocks               */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/

/* ------------------------------------------------------------------------ */
/*    Execute transducer blocks                                             */
/* ------------------------------------------------------------------------ */
  typedef struct _T_FBIF_PROCESS_DATA
  {
    USIGN16              channel;
    VOID *               p_process_value;
    SCALE *              p_xd_scale;
    USIGN8               tb_error;
  }
    T_FBIF_PROCESS_DATA;

  /* ------------------------------------------------------------------------
  TYPE_DESCRIPTION

  The Execute_<transducer> functions are called with a pointer to a structure
  of this type. The components of the structure are

    channel            Channel number

    p_process_value    Pointer to the process value,
                       for AI,AO:   pointer to FLOAT_S
                       for DI,DO:   pointer to DISCRETE_S
                       for MAI,MAO: pointer to an array of eight FLOAT_S
                       for MDI,MDO: pointer to an array of eight DISCRETE_S

    p_xd_scale         Pointer to the XD_SCALE parameter
                       (Available for AI and AO only)

    tb_error           May be used to indicate a transducer block error to
                       the connected output block. If tb_error is set to TRUE,
                       the output block set the output-failure bit in its block
                       error parameter
  ------------------------------------------------------------------------- */



/* ------------------------------------------------------------------------ */
/*    Read and Write Block Parameter                                        */
/* ------------------------------------------------------------------------ */
  typedef struct _T_FBIF_READ_DATA
  {
    USIGN16                  rel_idx;
    USIGN8                   subindex;
    USIGN8                   param_size;
    USIGN8 *                 dest;
  }
    T_FBIF_READ_DATA;

  /* ------------------------------------------------------------------------
  TYPE_DESCRIPTION

  The Read_Handler_xxx functions are called with a pointer to a structure
  of this type. The components of the structure are

    rel_idx           Relative index of the parameter to be read

    subindex          Subindex within the parameter to be read
                      (Subindex 0 indicates that the whole parameter is read)
    param_size        Size of the parameter to be read
    dest              Pointer to the buffer where the current parameter
                      value has to be copied.

  In case of an A-flag the application layer  m a y  copy the current parameter
  value to the location where 'dest' points to.

  In case of a X-flag the application layer  s h a l l  copy the current
  paramete value to the location where 'dest' points to.

  If neither the A- nor the X-flag is set 'param_size' and 'dest' are '0'
  and 'NULL'

  A descriptor of this type is also used by the 'a_of_get_default_value' routine.
  This function will be called if the device has to start or to restart with
  default values and if the a parameter has got a Y-flagin the GenVFD script.
  The 'a_of_get_default_value' routine can be used to set a default value that
  is different from the default value in the GenVFD script.
  ------------------------------------------------------------------------- */

  typedef enum
  {
    PRE_WRITE_CHECK,
    POST_WRITE_ACTION
  }
    T_FBIF_WRITE_PHASE;

  typedef struct _T_FBIF_WRITE_DATA
  {
    VOID *                   p_obj_desc;
    USIGN16                  rel_idx;
    USIGN8                   subindex;
    USIGN8                   length;
    USIGN8 *                 dest;
    USIGN8 *                 source;
    BOOL                     rem_acc;
    BOOL                     startup_sync;
    T_FBIF_WRITE_PHASE       phase;
  }
    T_FBIF_WRITE_DATA;

  /* ------------------------------------------------------------------------
  TYPE_DESCRIPTION

  The Write_Handler_xxx functions are called with a pointer to a structure
  of this type. The components of the structure are

    p_obj_desc        Pointer to the FMS object description of the parameter

    rel_idx           Relative index of the parameter to be written

    subindex          Subindex within the parameter to be written
                      (Subindex 0 indicates that the whole parameter is written)

    length            Number of bytes that should be written

    dest              Address where the new value should be written to
                      (If Subindex is 0, 'dest' is the address of the
                      parameter, if Subindex is not 0, 'dest' points
                      to the selected component within the parameter)

    source            Pointer to the location where the new value is currently
                      buffered

    rem_acc           Indicates whether the write request is remote or local

    startup_sync      This flag is TRUE if a write access is used to synchronize
                      an object of the application with an transducer or resource
                      block parameter. In this case the new value is copied to
                      the block parameter but the static revision counter is not
                      incremented and no update event is generated. Usually the
                      flag is TRUE when a parameter with a GenVFD V-flag is
                      written in the startup phase.

                      For a 'normal' write accesses the flag is always FALSE.

    phase             Phase of the write handling: PRE_WRITE_CHECK, POST_WRITE_ACTION
  ------------------------------------------------------------------------- */




/****************************************************************************/
/****************************************************************************/
/*                                                                          */
/*    FBIF Dictionary (Generated by GenVFD)                                 */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
  typedef struct
  {
    USIGN32                     vfd_ref;
    STRINGV                     vfd_tag[MAX_VFD_STRING_LENGTH];
    STRINGV                     v_name[MAX_VFD_STRING_LENGTH];
    STRINGV                     m_name[MAX_VFD_STRING_LENGTH];
    STRINGV                     revision[MAX_VFD_STRING_LENGTH];
    USIGN16                     start_of_link_obj;
    USIGN16                     no_of_link_obj;
    USIGN16                     start_of_trend_obj;
    USIGN16                     no_of_trend_float;
    USIGN16                     no_of_trend_discrete;
    USIGN16                     no_of_trend_bitstring;
    USIGN16                     no_of_mvc_objects;
    USIGN16                     max_number_of_mvc_elements;
    USIGN16                     start_index_mvc_objects;
    USIGN16                     start_index_mvc_var_lists;
    USIGN16                     start_of_alert_obj;
    USIGN16                     index_of_action_obj;
    USIGN16                     no_of_trans_blk;
    USIGN16                     no_of_funct_blk;
    USIGN16                     no_of_blocks;
    USIGN16                     no_of_ext_data_struct;
    const ST_DS_DESCR * const * list_of_ext_data_struct;
    const T_FBIF_BLOCK_DESCR *  fbif_block_dir;
  }
    T_FBIF_DICTIONARY;
  /* ------------------------------------------------------------------------
  TYPE_DESCRIPTION
    A structure of this type is generated by GenVFD. It contains pointers to
    all parts of the database generated by GenVFD. During the device startup
    phase the application has to provide a pointer to this structure to the FB
    shell. The structure enables the FB shell to build its internal database
    and to handle the FF blocks in a correct way.

    vfd_ref                             VFD reference number
    vfd_tag[MAX_VFD_STRING_LENGTH]      length, VFD TAG, \0
    v_name[MAX_VFD_STRING_LENGTH]       length, Vendor name, \0
    m_name[MAX_VFD_STRING_LENGTH]       length, Model name, \0
    revision[MAX_VFD_STRING_LENGTH]     length, Revision, \0
    start_of_link_obj                   Start index of link objects
    no_of_link_obj                      Number of link objects
    start_of_trend_obj                  Start index of trend objects
    no_of_trend_float                   Number of trend float
    no_of_trend_discrete                Number of trend discrete
    no_of_trend_bitstring               Number of trend bitstring
    no_of_mvc_objects                   Number of MCV objects
    max_number_of_mvc_elements          Maximum number of elements of a MVC object
    start_index_mvc_objects             Start index of MVC objects
    start_index_mvc_var_lists           Start index of MVC variable lists
    start_of_alert_obj                  Start index of alert objects
    index_of_action_obj                 Index of action object
    no_of_trans_blk                     Number of transducer blocks
    no_of_funct_blk                     Number of function blocks
    no_of_blocks                        Number of blocks
    no_of_ext_data_struct               Number of device-specific data-structures
    list_of_ext_data_struct             List of device-specific data-structures
    fbif_block_dir                      List of block descriptions
  ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------ */
/*    Global Variable                                                       */
/* ------------------------------------------------------------------------ */
#include "cs_fbif.h"
  extern const T_FBIF_DICTIONARY  fbif_dictionary;
#include "cs_def.h"

#endif /* __FBIF_H__ */




