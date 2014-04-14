#define TS_SVI_POS "Time-stamp: <2013-11-12 13:20:17 GE-300010777>"
/* ===========================================================================
   
   FILE_NAME          svi_positioner.ddl



   FUNCTIONAL_MODULE_DESCRIPTION

   This file contains the device description for SVI FF application.

   =========================================================================== */


/*
**********************************************************************
** Includes
**********************************************************************
*/
/* #pragma warning(disable: 718) */

#include "std_defs.h"
#include "com_tbls.h"
#include "rev_defs.h"
#include "svi_positioner.h"
#include "blk_err.h"

#include "svi_tb.h"
#include "svi_blocks.h"

#include "fdiag_SVI_Positioner.h"


/*
**********************************************************************
********** DEVICE SECTION ********************************************
**********************************************************************
*/

MANUFACTURER      0x004745,
DEVICE_TYPE       0x0008,
DEVICE_REVISION   1,
DD_REVISION       2


/*
***********************************************************************
Import the Standard Descriptions for all Standard Parameters
***********************************************************************
*/
IMPORT MANUFACTURER     __FF,
       DEVICE_TYPE      __STD_PARM,
       DEVICE_REVISION  SFT_STD_PARM_dev_rev,
       DD_REVISION      SFT_STD_PARM_dd_rev
  {
      EVERYTHING ;
      REDEFINITIONS
      {
          REDEFINE VARIABLE    __manufac_id
          {
              LABEL           [manufac_id_label] ;
              HELP            [manufac_id_help] ;
              CLASS           CONTAINED ;
              TYPE            ENUMERATED (4)
              {
                  {0x004745, "|en|General Electric", [mfr_id_help]}
              }
              CONSTANT_UNIT   [blank] ;
              HANDLING        READ ;
          }

          REDEFINE VARIABLE __dev_type
          {
              LABEL           [dev_type_label] ;
              HELP            [dev_type_help] ;
              CLASS           CONTAINED ;
              TYPE            ENUMERATED (2)
              {
                    {0x0008, "|en|SVi2FF Positioner", [blank]}
              }
              CONSTANT_UNIT   [blank] ;
              HANDLING        READ ;
          }
          REDEFINE VARIABLE    __features
          {
              LABEL           [features_label] ;
              HELP            [features_help] ;
              CLASS           CONTAINED ;
              TYPE            BIT_ENUMERATED (2)
              {
                  FEATURES_RES2
              }
              CONSTANT_UNIT   [blank] ;
              HANDLING        READ ;
          }

          REDEFINE VARIABLE    __feature_sel
          {
              LABEL           [feature_sel_label] ;
              HELP            [feature_sel_help] ;
              CLASS           CONTAINED ;
              TYPE            BIT_ENUMERATED (2)
              {
                  FEATURES_RES2
              }
              CONSTANT_UNIT   [blank] ;
              HANDLING        READ & WRITE ;
          }

          REDEFINE VARIABLE    __restart
          {
              LABEL           [restart_label] ;
              HELP            [restart_help] ;
              CLASS           CONTAINED & DYNAMIC ;
              TYPE            ENUMERATED (1)
              {
                  _UNINITIALIZED_VALUE,
                  { 0x01,    [restart_run],           [restart_run_help] },
                  { 0x02,    [restart_resource],      [restart_resource_help] },
                  { 0x03,    [restart_defaults],      [restart_defaults_help] },
                  { 0x04,    [restart_processor],     [restart_processor_help] },
                  { 0x05,    RESTART_FACTORY_DEFAULT, RESTART_FACTORY_DEFAULT_HELP }
              }
              HANDLING        READ & WRITE ;
          /*  RESPONSE_CODES  xxx ; */
          }

          REDEFINE VARIABLE __discrete_contained_nd
          {
              LABEL           [discrete_contained] ;
              HELP            [discrete_contained_help] ;
              CLASS           CONTAINED & OPERATE;
              TYPE            UNSIGNED_INTEGER (1) /* Ref: bug#17151 for SP_D */
              {
                  MIN_VALUE 0;
                  MAX_VALUE 100;
              }
              
          }
          REDEFINE VARIABLE __discrete_output_nd
          {
              LABEL           [discrete_output_label] ;
              HELP            [discrete_output_help] ;
              CLASS           OUTPUT & OPERATE ;
              TYPE            UNSIGNED_INTEGER (1) /* Ref: bug#18272, for __out_d*/
              {
                  MIN_VALUE 0;
                  MAX_VALUE 100;
              }
          }

          VARIABLE __confirm_time
          {
              REDEFINE CONSTANT_UNIT   "|en|1/32 millisec" ;
          }
          VARIABLE    __min_cycle_t
          {
              REDEFINE CONSTANT_UNIT   "|en|1/32 millisec" ;
          }
          VARIABLE    __nv_cycle_t
          {
              REDEFINE CONSTANT_UNIT   "|en|1/32 millisec" ;
          }
          VARIABLE    __shed_rcas
          {
              REDEFINE CONSTANT_UNIT   "|en|1/32 millisec" ;
          }
          VARIABLE    __shed_rout
          {
              REDEFINE CONSTANT_UNIT   "|en|1/32 millisec" ;
          }

          VARIABLE    __cycle_sel /* for #12967 */
          {
              TYPE BIT_ENUMERATED
              {
                  DELETE 0x0004; /* block_manufacturer */
              }
          }
          VARIABLE    __cycle_type /* For #12967 */
          {
              TYPE BIT_ENUMERATED
              {
                  DELETE 0x0004; /* block_manufacturer */
              }
          }

          VARIABLE __grant /* #13923, see FF-891 1.9 4.4.3.18 */
          {
              TYPE BIT_ENUMERATED
              {
                  REDEFINE {0x10, "|en|Operate", "|en|Operations may need to manipulate the operate parameter for daily plant production."}
                  ADD { 0x20,  "|en|Service", "|en|Service" }
                  ADD { 0x40,  "|en|Diagnostic", "|en|Diagnostic" }
              }
          }

          VARIABLE __deny /* #13923, see FF-891 1.9 4.4.3.18 */
          {
              TYPE BIT_ENUMERATED
              {
                  REDEFINE {0x10, "|en|Operate Denied", "|en|The Operate permission item has been turned off."}
                  ADD { 0x20,  "|en|Service Denied", "|en|Service Denied" }
                  ADD { 0x40,  "|en|Diagnostic Denied" , "|en|Diagnostic Denied" }
              }
          }

          REDEFINE VARIABLE __ack_option /* bug 14947 */
          {
              LABEL           [ack_option_label] ;
              HELP            [ack_option_help] ;
              CLASS           CONTAINED & ALARM ;
              TYPE            BIT_ENUMERATED (2)
              {
                      _FF_ACK_OPT_0, /* For Tokenizer Ver 5.2.0, The items can't
                                      * be located in dct file */
                      _FF_ACK_OPT_1,
                      _FF_ACK_OPT_2,
                      _FF_ACK_OPT_3,
                      _FF_ACK_OPT_4,
                      _FF_ACK_OPT_5,
                      _FF_ACK_OPT_6,
                      _FF_ACK_OPT_7,
                      _FF_ACK_OPT_8,
                      _FF_ACK_OPT_9,
                      _FF_ACK_OPT_10,
                      _FF_ACK_OPT_11
                      /* { 0x0000, "|en|Auto Ack Disabled"} */
                      /* {0x0001, "|en|Auto Ack Enabled"} */
              }
              CONSTANT_UNIT   [blank] ;
              HANDLING        READ & WRITE ;
          }


          /* REDEFINE VARIABLE __xd_fstate_opt */

          /* -------------------------------------------------------------- */
          /* Redefinitions for Field Diagnostics -------------------------- */
          /* -------------------------------------------------------------- */

          /* The following redefinition are necessary as older versions of
             standard.dct do not contain the label and help strings of the
             Field Diagnostics parameters and thus hosts using an old
             standard.dct are not able to display these strings             */

          VARIABLE     __fd_ver
          {
              REDEFINE LABEL          fd_ver_label ;
              REDEFINE HELP           fd_ver_help ;
          }

          RECORD     __fd_fail_alm
          {
              REDEFINE LABEL          fd_fail_alm_label ;
              REDEFINE HELP           fd_fail_alm_help ;
          }

          RECORD     __fd_offspec_alm
          {
              REDEFINE LABEL          fd_offspec_alm_label ;
              REDEFINE HELP           fd_offspec_alm_help ;
          }

          RECORD     __fd_maint_alm
          {
              REDEFINE LABEL          fd_maint_alm_label ;
              REDEFINE HELP           fd_maint_alm_help ;
          }

          RECORD     __fd_check_alm
          {
              REDEFINE LABEL          fd_check_alm_label ;
              REDEFINE HELP           fd_check_alm_help ;
          }

          VARIABLE     __fd_fail_pri
          {
              REDEFINE LABEL          fd_fail_pri_label ;
              REDEFINE HELP           fd_fail_pri_help ;
          }

          VARIABLE     __fd_offspec_pri
          {
              REDEFINE LABEL          fd_offspec_pri_label ;
              REDEFINE HELP           fd_offspec_pri_help ;
          }

          VARIABLE     __fd_maint_pri
          {
              REDEFINE LABEL          fd_maint_pri_label ;
              REDEFINE HELP           fd_maint_pri_help ;
          }

          VARIABLE     __fd_check_pri
          {
              REDEFINE LABEL          fd_check_pri_label ;
              REDEFINE HELP           fd_check_pri_help ;
          }

          RECORD     __fd_simulate
          {
              REDEFINE LABEL          fd_simulate_label ;
              REDEFINE HELP           fd_simulate_help ;
          }


          /* The following redefinition are necessary as the following
             bit-enumerations need a device-specific definition           */

          REDEFINE VARIABLE     __fd_fail_active
          {
              LABEL           fd_fail_active_label ;
              HELP            fd_fail_active_help ;
              CLASS           CONTAINED ;
              TYPE            BIT_ENUMERATED (4)
              {
                  FD_CONDITIONS_ENUM
              }
              CONSTANT_UNIT   [blank] ;
              HANDLING        READ  ;
          }

          REDEFINE VARIABLE     __fd_offspec_active
          {
              LABEL           fd_offspec_active_label ;
              HELP            fd_offspec_active_help ;
              CLASS           CONTAINED ;
              TYPE            BIT_ENUMERATED (4)
              {
                  FD_CONDITIONS_ENUM
              }
              CONSTANT_UNIT   [blank] ;
              HANDLING        READ  ;
          }

          REDEFINE VARIABLE     __fd_maint_active
          {
              LABEL           fd_maint_active_label ;
              HELP            fd_maint_active_help ;
              CLASS           CONTAINED ;
              TYPE            BIT_ENUMERATED (4)
              {
                  FD_CONDITIONS_ENUM
              }
              CONSTANT_UNIT   [blank] ;
              HANDLING        READ  ;
          }

          REDEFINE VARIABLE     __fd_check_active
          {
              LABEL           fd_check_active_label ;
              HELP            fd_check_active_help ;
              CLASS           CONTAINED ;
              TYPE            BIT_ENUMERATED (4)
              {
                  FD_CONDITIONS_ENUM
              }
              CONSTANT_UNIT   [blank] ;
              HANDLING        READ  ;
          }

          REDEFINE VARIABLE     __fd_fail_map
          {
              LABEL           fd_fail_map_label ;
              HELP            fd_fail_map_help ;
              CLASS           CONTAINED & ALARM ;
              TYPE            BIT_ENUMERATED (4)
              {
                  FD_CONDITIONS_ENUM
              }
              CONSTANT_UNIT   [blank] ;
              HANDLING        READ & WRITE ;
          }

          REDEFINE VARIABLE     __fd_offspec_map
          {
              LABEL           fd_offspec_map_label ;
              HELP            fd_offspec_map_help ;
              CLASS           CONTAINED & ALARM ;
              TYPE            BIT_ENUMERATED (4)
              {
                  FD_CONDITIONS_ENUM
              }
              CONSTANT_UNIT   [blank] ;
              HANDLING        READ & WRITE ;
          }

          REDEFINE VARIABLE     __fd_maint_map
          {
              LABEL           fd_maint_map_label ;
              HELP            fd_maint_map_help ;
              CLASS           CONTAINED & ALARM ;
              TYPE            BIT_ENUMERATED (4)
              {
                  FD_CONDITIONS_ENUM
              }
              CONSTANT_UNIT   [blank] ;
              HANDLING        READ & WRITE ;
          }

          REDEFINE VARIABLE     __fd_check_map
          {
              LABEL           fd_check_map_label ;
              HELP            fd_check_map_help ;
              CLASS           CONTAINED & ALARM ;
              TYPE            BIT_ENUMERATED (4)
              {
                  FD_CONDITIONS_ENUM
              }
              CONSTANT_UNIT   [blank] ;
              HANDLING        READ & WRITE ;
          }

          REDEFINE VARIABLE     __fd_fail_mask
          {
              LABEL           fd_fail_mask_label ;
              HELP            fd_fail_mask_help ;
              CLASS           CONTAINED & ALARM ;
              TYPE            BIT_ENUMERATED (4)
              {
                  FD_CONDITIONS_DISABLED_ENUM
              }
              CONSTANT_UNIT   [blank] ;
              HANDLING        READ & WRITE ;
          }

          REDEFINE VARIABLE     __fd_offspec_mask
          {
              LABEL           fd_offspec_mask_label ;
              HELP            fd_offspec_mask_help ;
              CLASS           CONTAINED & ALARM ;
              TYPE            BIT_ENUMERATED (4)
              {
                  FD_CONDITIONS_DISABLED_ENUM
              }
              CONSTANT_UNIT   [blank] ;
              HANDLING        READ & WRITE ;
          }

          REDEFINE VARIABLE     __fd_maint_mask
          {
              LABEL           fd_maint_mask_label ;
              HELP            fd_maint_mask_help ;
              CLASS           CONTAINED & ALARM ;
              TYPE            BIT_ENUMERATED (4)
              {
                  FD_CONDITIONS_DISABLED_ENUM
              }
              CONSTANT_UNIT   [blank] ;
              HANDLING        READ & WRITE ;
          }

          REDEFINE VARIABLE     __fd_check_mask
          {
              LABEL           fd_check_mask_label ;
              HELP            fd_check_mask_help ;
              CLASS           CONTAINED & ALARM ;
              TYPE            BIT_ENUMERATED (4)
              {
                  FD_CONDITIONS_DISABLED_ENUM
              }
              CONSTANT_UNIT   [blank] ;
              HANDLING        READ & WRITE ;
          }

          REDEFINE VARIABLE  __fd_recommen_act
          {
              LABEL           fd_recommended_action_label ;
              HELP            fd_recommended_action_help ;
              CLASS           CONTAINED ;
              TYPE            ENUMERATED(2)
              {
                  FD_RECOMMEN_ACT_ENUM
              }
              HANDLING        READ ;
          }

          REDEFINE VARIABLE    __diagnostic_simulate_value
          {
              LABEL           diagnostic_simulate_value_label ;
              HELP            diagnostic_simulate_value_help ;
              CLASS           CONTAINED & DYNAMIC ;
              TYPE            BIT_ENUMERATED (4)
              {
                  FD_CONDITIONS_ENUM
              }
              HANDLING        READ & WRITE;
          }

          REDEFINE VARIABLE    __diagnostic_value
          {
              LABEL           diagnostic_value_label ;
              HELP            diagnostic_value_help ;
              CLASS           CONTAINED & DYNAMIC ;
              TYPE            BIT_ENUMERATED(4)
              {
                  FD_CONDITIONS_ENUM
              }
              HANDLING        READ ;
          }

          /* The following re-definition are necessary as the
             FF DD Libraries, version 3.41 are not up to date
             for the alarm summary of a resource block with
             Field Diagnostics                                   */

          REDEFINE VARIABLE    __current_summary_res2
          {
              LABEL           [current_summary_label] ;
              HELP            [current_summary_help] ;
              CLASS           CONTAINED & ALARM ;
              TYPE            BIT_ENUMERATED (2)
              {
                  FD_CURR_SUMM_ENUM
              }
              CONSTANT_UNIT   [blank] ;
              HANDLING        READ ;
          }

          REDEFINE VARIABLE    __unacked_summary_res2
          {
              LABEL           [unacked_summary_label] ;
              HELP            [unacked_summary_help] ;
              CLASS           CONTAINED & ALARM ;
              TYPE            BIT_ENUMERATED (2)
              {
                  FD_UNACK_SUMM_ENUM
              }
              CONSTANT_UNIT   [blank] ;
              HANDLING        READ ;
          }

          REDEFINE VARIABLE    __unreported_summary_res2
          {
              LABEL           [unreported_summary_label] ;
              HELP            [unreported_summary_help] ;
              CLASS           CONTAINED & ALARM ;
              TYPE            BIT_ENUMERATED (2)
              {
                  FD_UNREP_SUMM_ENUM
              }
              CONSTANT_UNIT   [blank] ;
              HANDLING        READ ;
          }

          REDEFINE VARIABLE    __disabled_summary_res2
          {
              LABEL           [disabled_summary_label] ;
              HELP            [disabled_summary_help] ;
              CLASS           CONTAINED & ALARM ;
              TYPE            BIT_ENUMERATED (2)
              {
                  FD_DISAB_SUMM_ENUM
              }
              CONSTANT_UNIT   [blank] ;
              HANDLING        READ & WRITE ;
          }

      }
  }



/*
**************************************************************************
Import the Standard Descriptions of all Advanced Function Block Parameters
**************************************************************************
*/
IMPORT  MANUFACTURER        __FF,
        DEVICE_TYPE         __ADV_PARAM,
        DEVICE_REVISION     SFT_ADV_PARAM_dev_rev,
        DD_REVISION         SFT_ADV_PARAM_dd_rev
  {
      EVERYTHING ;
      REDEFINITIONS
      {
          REDEFINE VARIABLE    __select_type
          {
              LABEL           [select_type_label] ;
              HELP            [select_type_help] ;
              CLASS           CONTAINED;
              TYPE            ENUMERATED (1)
              {
                  {0 , [uninitialized],     [no_help_available]},\
                  {1 , [select_type_val_1], [no_help_available]},\
                  {2 , [select_type_val_2], [no_help_available]},\
                  {3 , [select_type_val_3], [no_help_available]},\
                  {4 , [select_type_val_4], [no_help_available]},\
                  {5 , [select_type_val_5], [no_help_available]}
              }
              HANDLING        READ & WRITE ;
          }
      }
  }


/*
**************************************************************************
Import the Standard Descriptions of all MIO Block Parameters
**************************************************************************
*/
IMPORT  MANUFACTURER        __FF,
        DEVICE_TYPE         __STD_MIO_PARM,
        DEVICE_REVISION     SFT_MIO_PARM_dev_rev,
        DD_REVISION         SFT_MIO_PARM_dd_rev
  {
      EVERYTHING;
      REDEFINITIONS
      {
      /* FSTATE_STATUS must be changed to BIT_ENUMERATED (2), and handling to read only */
          REDEFINE VARIABLE    __fstate_status
          {
              LABEL           [fs_status] ;
              HELP            [fs_status_help] ;
              CLASS           CONTAINED & DYNAMIC;
              TYPE            BIT_ENUMERATED (2)
              {
                  { 0x0001, [value_1_in_fs], [value_1_in_fs_help] },
                  { 0x0002, [value_2_in_fs], [value_2_in_fs_help] },
                  { 0x0004, [value_3_in_fs], [value_3_in_fs_help] },
                  { 0x0008, [value_4_in_fs], [value_4_in_fs_help] },
                  { 0x0010, [value_5_in_fs], [value_5_in_fs_help] },
                  { 0x0020, [value_6_in_fs], [value_6_in_fs_help] },
                  { 0x0040, [value_7_in_fs], [value_7_in_fs_help] },
                  { 0x0080, [value_8_in_fs], [value_8_in_fs_help] }
              }

              CONSTANT_UNIT   [blank] ;
              HANDLING        READ ;
          }
      }
  }




/*
***********************************************************************
Import the Standard Descriptions for all Transducer Block Parameters
***********************************************************************
*/
IMPORT MANUFACTURER       __FF,
       DEVICE_TYPE        __STD_XDCR_PARM,
       DEVICE_REVISION    SFT_STD_XDCR_PARM_dev_rev,
       DD_REVISION        SFT_STD_XDCR_PARM_dd_rev
  {
      EVERYTHING ;
      REDEFINITIONS
      {
          RECORD __primary_value
          {
              REDEFINE MEMBERS
              {
                  STATUS,     __status_contained_r ;
                  VALUE,      __float_contained_r ;
              }
          }
          RECORD __secondary_value
          {
              REDEFINE MEMBERS
              {
                  STATUS,     __status_contained_r ;
                  VALUE,      __float_contained_r ;
              }
          }
          ARRAY  __transducer_directory
          {
              REDEFINE NUMBER_OF_ELEMENTS 4;
          }
      }
  }


/*
**********************************************************************
** Resource block 2
**********************************************************************
*/
IMPORT MANUFACTURER     __FF,
DEVICE_TYPE      __RES_BLOCK_2,
DEVICE_REVISION  SFT_RES_BLOCK_2_dev_rev,
DD_REVISION      SFT_RES_BLOCK_2_dd_rev

{
    EVERYTHING ;
    REDEFINITIONS
    {
        
        /* -------------------------------------------------------------- */
        /* Field Diagnostics parameters are added to the Resouce Block -- */
        /* -------------------------------------------------------------- */
        
        /* (The following _FD_xxx macros are defined in com_tbls.h)       */
        
        BLOCK  __resource_block_2
        {
            PARAMETERS
            {
                REDEFINE MODE_BLK, svi2_rb_mode_blk ;
                __FD_PARAMS
                ADD_SVI_ADDITION_PARA_RB
            }
            
#ifndef NOMENUS
            REDEFINE MENU_ITEMS
            {
                RB_MENU_LIST
            }
#endif
            REDEFINE METHOD_ITEMS
            {
                RB_METHOD_LIST
            }
            
        }
        
        VARIABLE_LIST   __res_2_view_1
        {
            MEMBERS
            {
                __FD_PARAMS_VIEW1
                /* ADD_SVI_ADDITION_VIEW3_ITEMS_RB */
            }
        }
        
        VARIABLE_LIST   __res_2_view_3
        {
            MEMBERS
            {
                __FD_PARAMS_VIEW3
                /* ADD_SVI_ADDITION_VIEW3_ITEMS_RB */ /* use the view defines from softing */
                
            }
        }
        
        VARIABLE_LIST   __res_2_view_4
        {
            MEMBERS
            {
                __FD_PARAMS_VIEW4
                /* ADD_SVI_ADDITION_VIEW4_ITEMS_RB     */
            }
        }
        
    }
}

/*
**********************************************************************
** Function Blocks
**********************************************************************
*/

/* variable for BLOCK_ERR_DESC_1 */

VARIABLE    block_err_desc_1
{
    LABEL           LBL_BLOCK_ERR_DESC_1 ;
    HELP            HLP_BLOCK_ERR_DESC_1 ;
    CLASS           CONTAINED & DYNAMIC ;
    TYPE            BIT_ENUMERATED (4)
    {
        BLK_ERR_DESCR_1_ENUM
    }
    CONSTANT_UNIT   [blank] ;
    HANDLING        READ ;
}




/*
**********************************************************************
** Input Selector Block
**********************************************************************
*/
VARIABLE    isel_disable_value
{
    LABEL           LBL_ISEL_DISABLE_VALUE ;
    HELP            HLP_ISEL_DISABLE_VALUE ;
    CLASS           INPUT & DYNAMIC ;
    TYPE            ENUMERATED (1)
    {
       ENUM_ISEL_DISABLE_USE,
       ENUM_ISEL_DISABLE_DISABLE
    }
    HANDLING        READ & WRITE ;
}

VARIABLE    op_selected_value
{
    LABEL           LBL_OP_SELECT_VALUE ;
    HELP            HLP_OP_SELECT_VALUE ;
    CLASS           INPUT & DYNAMIC ;
    TYPE            ENUMERATED (1)
    {
        ENUM_OP_SELECT_0,
        ENUM_OP_SELECT_1,
        ENUM_OP_SELECT_2,
        ENUM_OP_SELECT_3,
        ENUM_OP_SELECT_4
    }
    HANDLING        READ & WRITE ;
}

VARIABLE  selected_value_redefined
{
    LABEL           LBL_SELECTED_VALUE ;
    HELP            HLP_SELECTED_VALUE ;
    CLASS           OUTPUT & DYNAMIC ;
    TYPE            UNSIGNED_INTEGER (1) ;
    CONSTANT_UNIT   [blank] ;
    HANDLING        READ ;
}


IMPORT  MANUFACTURER        __FF,
        DEVICE_TYPE         __INPUT_SELECTOR,
        DEVICE_REVISION     SFT_INPUT_SELECTOR_dev_rev,
        DD_REVISION         SFT_INPUT_SELECTOR_dd_rev
  {
    EVERYTHING ;
    REDEFINITIONS
    {
      BLOCK __input_selector
      {
        REDEFINE  LABEL LBL_ISFB ;
        REDEFINE  HELP  HLP_ISFB ;

        PARAMETERS
        {
            ADD   BLOCK_ERR_DESC_1,   block_err_desc_1;
        }
      }

      RECORD __disable_1
      {
          REDEFINE MEMBERS
          {
              STATUS,     __status_input     ;
              VALUE,      isel_disable_value ;
          }
      }

      RECORD __disable_2
      {
          REDEFINE MEMBERS
          {
              STATUS,     __status_input     ;
              VALUE,      isel_disable_value ;
          }
      }

      RECORD __disable_3
      {
          REDEFINE MEMBERS
          {
              STATUS,     __status_input     ;
              VALUE,      isel_disable_value ;
          }
      }

      RECORD __disable_4
      {
          REDEFINE MEMBERS
          {
              STATUS,     __status_input     ;
              VALUE,      isel_disable_value ;
          }
      }

      RECORD __op_select
      {
          REDEFINE MEMBERS
          {
              STATUS,  __status_input    ;
              VALUE,   op_selected_value ;
          }
      }

      RECORD      __selected
      {
          REDEFINE MEMBERS
          {
              STATUS,  __status_output_r ;
              VALUE,   selected_value_redefined ;
          }
      }

      REDEFINE VARIABLE __select_type
      {
        LABEL     [select_type_label];
        HELP      [select_type_help];
        CLASS     CONTAINED;
        HANDLING  READ & WRITE;
        TYPE      ENUMERATED (1)
        {
          ENUM_SELECT_TYPE_UNITIALIZED,
          ENUM_SELECT_TYPE_FIRST_GOOD,
          ENUM_SELECT_TYPE_MINIMUM,
          ENUM_SELECT_TYPE_MAXIMUM,
          ENUM_SELECT_TYPE_MIDDLE,
          ENUM_SELECT_TYPE_AVERAGE
        }
      }
    }
  }




/*
**********************************************************************
** Multiple Analog Input block (MAI)
**********************************************************************
*/
VARIABLE    mai_channel
{
    LABEL           [channel_label] ;
    HELP            [channel_help] ;
    CLASS           CONTAINED ;
    TYPE            ENUMERATED (2)
    {
        CHANNELS_MAI
    }
    CONSTANT_UNIT   [blank] ;
    HANDLING        READ & WRITE ;
}


IMPORT  MANUFACTURER      __FF,
        DEVICE_TYPE       __MAI,
        DEVICE_REVISION   SFT_MAI_dev_rev,
        DD_REVISION       SFT_MAI_dd_rev
  {
      EVERYTHING;

      REDEFINITIONS
      {
          BLOCK __mai_block
          {
             REDEFINE LABEL  LBL_MAI ;

              PARAMETERS
              {
                  REDEFINE   CHANNEL,            mai_channel;
                  ADD        BLOCK_ERR_DESC_1,   block_err_desc_1;
              }
          }
      }
  }





/*
**********************************************************************
** Analog Input block (AI)
**********************************************************************
*/

VARIABLE    ai_channel
{
    LABEL           [channel_label] ;
    HELP            [channel_help] ;
    CLASS           CONTAINED ;
    TYPE            ENUMERATED (2)
    {
        CHANNELS_AI
    }
    CONSTANT_UNIT   [blank] ;
    HANDLING        READ & WRITE ;
}

IMPORT MANUFACTURER     __FF,
       DEVICE_TYPE      __AI_BLOCK,
       DEVICE_REVISION  SFT_AI_BLOCK_dev_rev,
       DD_REVISION      SFT_AI_BLOCK_dd_rev
  {
      EVERYTHING;

      REDEFINITIONS
      {


          REDEFINE VARIABLE    __ack_option_ai
          {
              LABEL           [ack_option_label] ;
              HELP            [ack_option_help] ;
              CLASS           CONTAINED & ALARM ;
              TYPE            BIT_ENUMERATED (2)
              {
                  ACK_OPTIONS_AI
              }
              CONSTANT_UNIT   [blank] ;
              HANDLING        READ & WRITE ;
          }

          BLOCK __analog_input_block
          {
             PARAMETERS
             {
               REDEFINE   CHANNEL,               ai_channel;
               ADD        BLOCK_ERR_DESC_1,      block_err_desc_1;
             }

          }

      }
  }

/*
**********************************************************************
** Digital Input block (DI)
**********************************************************************
*/

VARIABLE    di_channel
{
    LABEL           [channel_label] ;
    HELP            [channel_help] ;
    CLASS           CONTAINED ;
    TYPE            ENUMERATED (2)
    {
        CHANNELS_DI
    }
    CONSTANT_UNIT   [blank] ;
    HANDLING        READ & WRITE ;
}

IMPORT MANUFACTURER     __FF,
       DEVICE_TYPE      __DI_BLOCK,
       DEVICE_REVISION  SFT_DI_BLOCK_dev_rev,
       DD_REVISION      SFT_DI_BLOCK_dd_rev
  {
      EVERYTHING;

      REDEFINITIONS
      {
          REDEFINE VARIABLE    __ack_option_di
          {
              LABEL           [ack_option_label] ;
              HELP            [ack_option_help] ;
              CLASS           CONTAINED & ALARM ;
              TYPE            BIT_ENUMERATED (2)
              {
                  ACK_OPTIONS_DI
              }
              CONSTANT_UNIT   [blank] ;
              HANDLING        READ & WRITE ;
          }

          BLOCK __discrete_input_block
          {
              PARAMETERS
              {
                REDEFINE   CHANNEL,               di_channel;
                ADD        BLOCK_ERR_DESC_1,      block_err_desc_1;
              }
          }
      }
  }

/*
**********************************************************************
** Analog Output block (AO)
**********************************************************************
*/

VARIABLE    ao_channel
{
    LABEL           [channel_label] ;
    HELP            [channel_help] ;
    CLASS           CONTAINED ;
    TYPE            ENUMERATED (2)
    {
        CHANNELS_AO
    }
    CONSTANT_UNIT   [blank] ;
    HANDLING        READ & WRITE ;
}

IMPORT MANUFACTURER     __FF,
       DEVICE_TYPE      __AO_BLOCK,
       DEVICE_REVISION  SFT_AO_BLOCK_dev_rev,
       DD_REVISION      SFT_AO_BLOCK_dd_rev
  {
    EVERYTHING;

      REDEFINITIONS
      {
          REDEFINE VARIABLE   __io_opts
          {
              LABEL           [io_opts_label] ;
              HELP            [io_opts_help] ;
              CLASS           CONTAINED ;
              TYPE            BIT_ENUMERATED (2)
              {
                   { 0x0001,   [io_invert],              [io_invert_help] },
                   { 0x0002,   [io_sp_pv_track_man],     [io_sp_pv_track_man_help] },
                   { 0x0008,   [io_sp_pv_track_lo],      [io_sp_pv_track_lo_help] },
                   { 0x0010,   [io_sp_track_target],     [io_sp_track_target_help] },
                   { 0x0020,   [io_inc_to_close],        [io_inc_to_close_help] },
                   { 0x0040,   [io_faultstate],          [io_faultstate_help] },
                   { 0x0080,   [io_faultstate_restart],  [io_faultstate_restart_help] },
                   { 0x0100,   [io_target_to_man],       [io_target_to_man_help] },
                   { 0x0200,   [io_pv_for_bkcal_out],    [io_pv_for_bkcal_out_help] },
                   { 0x0400,   [io_low_cutoff],          [io_low_cutoff_help] },
                   { 0x0800,   [io_latch_fstate],        [io_latch_fstate_help] },
                   { 0x1000,   "|en|Units Conversion",   [io_units_conversion_help] }
              }
              CONSTANT_UNIT   [blank] ;
              HANDLING        READ & WRITE ;
          }

          BLOCK __analog_output_block
          {
              PARAMETERS
              {
                REDEFINE   CHANNEL,               ao_channel;
                ADD        BLOCK_ERR_DESC_1,      block_err_desc_1;
              }
          }
      }
  }

/*
**********************************************************************
** Digital Output block (DO)
**********************************************************************
*/

VARIABLE    do_channel
{
    LABEL           [channel_label] ;
    HELP            [channel_help] ;
    CLASS           CONTAINED ;
    TYPE            ENUMERATED (2)
    {
        CHANNELS_DO
    }
    CONSTANT_UNIT   [blank] ;
    HANDLING        READ & WRITE ;
}

IMPORT MANUFACTURER     __FF,
       DEVICE_TYPE      __DO_BLOCK,
       DEVICE_REVISION  SFT_DO_BLOCK_dev_rev,
       DD_REVISION      SFT_DO_BLOCK_dd_rev
  {
      EVERYTHING;
      
      REDEFINITIONS
      {
          BLOCK __discrete_output_block
          {
              PARAMETERS
              {
                REDEFINE   CHANNEL,               do_channel;
                ADD        BLOCK_ERR_DESC_1,      block_err_desc_1;
              }
          }
      }

  }

/*
**********************************************************************
** Proportional Integral Derivative block (PID)
**********************************************************************
*/

VARIABLE    algorithm
{
    LABEL           LBL_ALGORITHM ;
    HELP            HLP_ALGORITHM ;
    CLASS           CONTAINED ;
    TYPE            ENUMERATED (1)
    {
        ENUM_ALGORITHM
    }
    HANDLING        READ & WRITE ;
}

VARIABLE    tfpd
{
    LABEL           LBL_TFPD ;
    HELP            HLP5(HLP_TFPD) ;
    CLASS           CONTAINED ;
    TYPE            FLOAT;
    HANDLING        READ & WRITE ;
}

VARIABLE    tfpid
{
    LABEL           LBL_TFPID ;
    HELP            HLP_TFPID ;
    CLASS           CONTAINED ;
    TYPE            FLOAT;
    HANDLING        READ & WRITE ;
}


IMPORT MANUFACTURER     __FF,
       DEVICE_TYPE      __PID_BLOCK,
       DEVICE_REVISION  SFT_PID_BLOCK_dev_rev,
       DD_REVISION      SFT_PID_BLOCK_dd_rev
  {
      EVERYTHING;

      REDEFINITIONS
      {

          REDEFINE VARIABLE    __ack_option_pid
          {
              LABEL           [ack_option_label] ;
              HELP            [ack_option_help] ;
              CLASS           CONTAINED & ALARM ;
              TYPE            BIT_ENUMERATED (2)
              {
                  ACK_OPTIONS_PID
              }
              CONSTANT_UNIT   [blank] ;
              HANDLING        READ & WRITE ;
          }

          REDEFINE VARIABLE    __status_opts_pid
          {
              LABEL           [status_opts_label] ;
              HELP            [status_opts_help] ;
              CLASS           CONTAINED ;
              TYPE            BIT_ENUMERATED (2)
              {
                STATUS_OPTIONS_PID
              }
              CONSTANT_UNIT   [blank] ;
              HANDLING        READ & WRITE ;
          }

          BLOCK __proportional_integral_derivative_block
          {
              PARAMETERS
              {
                ADD        BLOCK_ERR_DESC_1,      block_err_desc_1;
                ADD        ALGORITHM,   algorithm;
                ADD        TFPD,        tfpd;
                ADD        TFPID,       tfpid;
              }
          }
      }
  }


/*
**********************************************************************
** Output Splitter Block
**********************************************************************
*/
IMPORT  MANUFACTURER        __FF,
        DEVICE_TYPE         __OUTPUT_SPLITTER,
        DEVICE_REVISION     SFT_OUTPUT_SPLITTER_dev_rev,
        DD_REVISION         SFT_OUTPUT_SPLITTER_dd_rev
  {
    EVERYTHING ;
    REDEFINITIONS
    {
      BLOCK __output_splitter
      {
        REDEFINE  LABEL LBL_OSFB ;
        REDEFINE  HELP  HLP_OSFB ;

        PARAMETERS
        {
            ADD   BLOCK_ERR_DESC_1,   block_err_desc_1;
        }
      }

      REDEFINE VARIABLE    __status_opts_os
      {
        LABEL           [status_opts_label] ;
        HELP            [status_opts_help] ;
        CLASS           CONTAINED ;
        TYPE            BIT_ENUMERATED (2)
        {
          STATUS_OPTIONS_OS
        }
        CONSTANT_UNIT   [blank] ;
        HANDLING        READ & WRITE ;
      }
    }
  }

/*
**********************************************************************
** Arithmetic block
**********************************************************************
*/
IMPORT  MANUFACTURER    __FF,
        DEVICE_TYPE     __ARITHMETIC,
        DEVICE_REVISION __ARITHMETIC_rel_dev_rev,
        DD_REVISION     __ARITHMETIC_rel_dd_rev
{
    EVERYTHING ;
    REDEFINITIONS
    {
      BLOCK __arithmetic
      {
        REDEFINE  LABEL LBL_ARFB ;
        REDEFINE  HELP  HLP_ARFB ;

        PARAMETERS
        {
            ADD   BLOCK_ERR_DESC_1,   block_err_desc_1;
        }

      }

      REDEFINE VARIABLE __input_opts
      {
        LABEL     [input_opts_label];
        HELP      [input_opts_help];
        CLASS     CONTAINED;
        HANDLING  READ & WRITE;
        TYPE      BIT_ENUMERATED (2)
        {
          ENUM_AR_INPUT_OPT_IN_USE_UNCERT,
          ENUM_AR_INPUT_OPT_IN_LO_USE_UNCERT,
          ENUM_AR_INPUT_OPT_IN_1_USE_UNCERT,
          ENUM_AR_INPUT_OPT_IN_1_USE_BAD,
          ENUM_AR_INPUT_OPT_IN_2_USE_UNCERT,
          ENUM_AR_INPUT_OPT_IN_2_USE_BAD,
          ENUM_AR_INPUT_OPT_IN_3_USE_UNCERT,
          ENUM_AR_INPUT_OPT_IN_3_USE_BAD
        }
      }

      REDEFINE VARIABLE __arith_type
      {
        LABEL     [arith_type_label];
        HELP      [arith_type_help];
        CLASS     CONTAINED;
        HANDLING  READ & WRITE;
        TYPE      ENUMERATED (1)
        {
          ENUM_ARITH_TYPE_UNITIALIZED,
          ENUM_ARITH_TYPE_FLOW_COMP_LIN,
          ENUM_ARITH_TYPE_FLOW_COMP_SQR,
          ENUM_ARITH_TYPE_FLOW_COMP_APPROX,
          ENUM_ARITH_TYPE_BTU_FLOW,
          ENUM_ARITH_TYPE_MULT_DIV,
          ENUM_ARITH_TYPE_AVERAGE,
          ENUM_ARITH_TYPE_TRADIT_SUM,
          ENUM_ARITH_TYPE_POLYNOM,
          ENUM_ARITH_TYPE_HTG_COMP,
          ENUM_ARITH_TYPE_POLYNOM_2
        }
      }

      RECORD __pre_out
      {
        REDEFINE MEMBERS
        {
          STATUS,  __status_contained_r ;
          VALUE,   __float_contained_r ;
        }
      }
    }

}
/*
**********************************************************************
** Control Selector block
**********************************************************************
*/

IMPORT MANUFACTURER    __FF,
       DEVICE_TYPE     __CS_BLOCK,
       DEVICE_REVISION __CS_BLOCK_rel_dev_rev,
       DD_REVISION     __CS_BLOCK_rel_dd_rev
{
    EVERYTHING ;
    REDEFINITIONS
    {
        BLOCK __control_selector_block
        {
            PARAMETERS
            {
                ADD     BLOCK_ERR_DESC_1,   block_err_desc_1;

            }
        }
        
        REDEFINE VARIABLE_LIST   __cs_view_4
        {
            MEMBERS
            {
                VL_ST_REV,      PARAM.ST_REV ;
                VL_STRATEGY,    PARAM.STRATEGY ;
                VL_ALERT_KEY,   PARAM.ALERT_KEY ;
                VL_STATUS_OPTS, PARAM.STATUS_OPTS;
                VL_SEL_TYPE,    PARAM.SEL_TYPE ;
            }
        }
        
    }

}


/*
**********************************************************************
**********************************************************************
** AI transducer block
**********************************************************************
**********************************************************************
*/


/*
**********************************************************************
** Variables
**********************************************************************
*/


/*
**********************************************************************
pv_unit
**********************************************************************
*/
VARIABLE    pv_unit
{
    LABEL          LBL_PV_UNIT ;
    HELP           HLP_PV_UNIT ;
    CLASS           CONTAINED ;
    TYPE            ENUMERATED (2)
    {
        __UNITS_CODES1
    __UNITS_CODES2
    }
    CONSTANT_UNIT   [blank] ;
    HANDLING       READ ;
}

/*
**********************************************************************
sv_unit
**********************************************************************
*/
VARIABLE    sv_unit
{
    LABEL          LBL_SV_UNIT ;
    HELP           HLP_SV_UNIT ;
    CLASS           CONTAINED ;
    TYPE            ENUMERATED (2)
    {
       __UNITS_CODES1
       __UNITS_CODES2
    }
    CONSTANT_UNIT   [blank] ;
    HANDLING       READ ;
}
#if 0
/*
**********************************************************************
hart_man_id
**********************************************************************
*/
VARIABLE    hart_man_id
{
    LABEL          LBL_HART_MAN_ID ;
    HELP           HLP_HART_MAN_ID ;
    CLASS          CONTAINED ;
    TYPE           UNSIGNED_INTEGER (1) ;
    HANDLING       READ ;
}

/*
**********************************************************************
hart_dev_type
**********************************************************************
*/
VARIABLE    hart_dev_type
{
    LABEL          LBL_HART_DEV_TYPE ;
    HELP           HLP_HART_DEV_TYPE ;
    CLASS          CONTAINED ;
    TYPE           UNSIGNED_INTEGER (1) ;
    HANDLING       READ ;
}

/*
**********************************************************************
hart_dev_id
**********************************************************************
*/
VARIABLE    hart_dev_id
{
    LABEL          LBL_HART_DEV_ID ;
    HELP           HLP_HART_DEV_ID ;
    CLASS          CONTAINED ;
    TYPE           UNSIGNED_INTEGER (4) ;
    HANDLING       READ ;
}

/*
**********************************************************************
hart_dev_rev
**********************************************************************
*/
VARIABLE    hart_dev_rev
{
    LABEL          LBL_HART_DEV_REV ;
    HELP           HLP_HART_DEV_REV ;
    CLASS          CONTAINED ;
    TYPE           UNSIGNED_INTEGER (1) ;
    HANDLING       READ ;
}

/*
**********************************************************************
hart_sw_rev
**********************************************************************
*/
VARIABLE    hart_sw_rev
{
    LABEL          LBL_HART_SW_REV ;
    HELP           HLP_HART_SW_REV ;
    CLASS          CONTAINED ;
    TYPE           UNSIGNED_INTEGER (1) ;
    HANDLING       READ ;
}

/*
**********************************************************************
hart_hw_rev
**********************************************************************
*/
VARIABLE    hart_hw_rev
{
    LABEL          LBL_HART_HW_REV ;
    HELP           HLP_HART_HW_REV ;
    CLASS          CONTAINED ;
    TYPE           UNSIGNED_INTEGER (1) ;
    HANDLING       READ ;
}

/*
**********************************************************************
hart_tag
**********************************************************************
*/
VARIABLE    hart_tag
{
    LABEL           LBL_HART_TAG ;
    HELP            HLP_HART_TAG ;
    CLASS           CONTAINED ;
    TYPE            OCTET (8) ;
    HANDLING        READ & WRITE ;
}

/*
**********************************************************************
hart_descr
**********************************************************************
*/
VARIABLE    hart_descr
{
    LABEL           LBL_HART_DESCR ;
    HELP            HLP_HART_DESCR ;
    CLASS           CONTAINED ;
    TYPE            OCTET (16) ;
    HANDLING        READ & WRITE ;
}

/*
**********************************************************************
hart_date
**********************************************************************
*/
VARIABLE    day
{
    LABEL          LBL_DAY ;
    HELP           HLP_DAY ;
    CLASS          CONTAINED ;
    TYPE           UNSIGNED_INTEGER (1) ;
    HANDLING       READ ;
}


VARIABLE    month
{
    LABEL          LBL_MONTH ;
    HELP           HLP_MONTH ;
    CLASS          CONTAINED ;
    TYPE           UNSIGNED_INTEGER (1) ;
    HANDLING       READ ;
}


VARIABLE    year
{
    LABEL          LBL_YEAR ;
    HELP           HLP_YEAR ;
    CLASS          CONTAINED ;
    TYPE           UNSIGNED_INTEGER (1) ;
    HANDLING       READ ;
}


RECORD    hart_date
{
    LABEL          LBL_HART_DATE ;
    HELP           HLP_HART_DATE ;
    MEMBERS
    {
        DAY,     day ;
        MONTH,   month ;
        YEAR,    year ;
    }
}

/*
**********************************************************************
hart_cmd_major_rev
**********************************************************************
*/
VARIABLE    hart_cmd_major_rev
{
    LABEL          LBL_HART_CMD_MAJOR_REV ;
    HELP           HLP_HART_CMD_MAJOR_REV ;
    CLASS          CONTAINED ;
    TYPE           UNSIGNED_INTEGER (1) ;
    HANDLING       READ ;
}

/*
**********************************************************************
ramp_value
**********************************************************************
*/
ramp_value LIKE RECORD __secondary_value
{
    REDEFINE LABEL           LBL_RAMP_VALUE ;
    REDEFINE HELP            HLP_RAMP_VALUE ;
}

/*
**********************************************************************
ramp_rate
**********************************************************************
*/
VARIABLE    ramp_rate
{
    LABEL          LBL_RAMP_RATE ;
    HELP           HLP_RAMP_RATE ;
    CLASS          CONTAINED ;
    TYPE           FLOAT ;
    HANDLING       READ & WRITE ;
}

/*
**********************************************************************
ramp_min
**********************************************************************
*/
VARIABLE    ramp_min
{
    LABEL          LBL_RAMP_MIN ;
    HELP           HLP_RAMP_MIN ;
    CLASS          CONTAINED ;
    TYPE           FLOAT ;
    HANDLING       READ & WRITE ;
}

/*
**********************************************************************
ramp_max
**********************************************************************
*/
VARIABLE    ramp_max
{
    LABEL          LBL_RAMP_MAX ;
    HELP           HLP_RAMP_MAX ;
    CLASS          CONTAINED ;
    TYPE           FLOAT ;
    HANDLING       READ & WRITE ;
}

/*
**********************************************************************
rect_value
**********************************************************************
*/
rect_value LIKE RECORD __secondary_value
{
    REDEFINE LABEL           LBL_RECT_VALUE ;
    REDEFINE HELP            HLP_RECT_VALUE ;
}

/*
**********************************************************************
rect_period
**********************************************************************
*/
VARIABLE    rect_period
{
    LABEL          LBL_RECT_PERIOD ;
    HELP           HLP_RECT_PERIOD ;
    CLASS          CONTAINED ;
    TYPE           UNSIGNED_INTEGER (2) ;
    HANDLING       READ & WRITE ;
}

/*
**********************************************************************
rect_min
**********************************************************************
*/
VARIABLE    rect_min
{
    LABEL          LBL_RECT_MIN ;
    HELP           HLP_RECT_MIN ;
    CLASS          CONTAINED ;
    TYPE           FLOAT ;
    HANDLING       READ & WRITE ;
}

/*
**********************************************************************
rect_max
**********************************************************************
*/
VARIABLE    rect_max
{
    LABEL          LBL_RECT_MAX ;
    HELP           HLP_RECT_MAX ;
    CLASS          CONTAINED ;
    TYPE           FLOAT ;
    HANDLING       READ & WRITE ;
}
#endif

/*
**********************************************************************
const_value
**********************************************************************
*/
RECORD    const_value
{
    LABEL          LBL_CONST_VALUE ;
    HELP           HLP_CONST_VALUE ;
    MEMBERS
    {
        STATUS,     __status_contained ;
        VALUE,      __float_contained ;
    }
}


/*
**********************************************************************
comm_state
**********************************************************************
*/
VARIABLE    comm_state
{
    LABEL          LBL_COMM_STATE ;
    HELP           HLP_COMM_STATE ;
    CLASS          CONTAINED ;
    TYPE           ENUMERATED (1)
    {
      COMM_STATE_ENUMS
    }
    HANDLING       READ ;
}


/*
**********************************************************************
ptb_character
**********************************************************************
*/

RECORD ptb_character
{
    LABEL           LBL_PTB_CHARACTER ;
    HELP            HLP_PTB_CHARACTER ;
    MEMBERS
    {
        BLOCK_TAG,          __block_tag ;
        DD_MEMBER,          __dd_member ;
        DD_ITEM,            __dd_item ;
        DD_REVIS,           __dd_revis ;
        PROFILE,            __profile ;
        PROFILE_REVISION,   __profile_revision ;
        EXECUTION_TIME,     __execution_time ;
        EXECUTION_PERIOD,   __execution_period ;
        NUM_OF_PARAMS,      __num_of_params ;
        NEXT_FB_TO_EXECUTE, __next_fb_to_execute ;
        VIEWS_INDEX,        __views_index ;
        NUMBER_VIEW_3,      __number_view_3 ;
        NUMBER_VIEW_4,      __number_view_4 ;
    }
}


BLOCK PTB
{
    CHARACTERISTICS   ptb_character ;
    LABEL             LBL_POSITIONER_TRANSDUCER_BLOCK ;
    HELP              HLP_POSITIONER_TRANSDUCER_BLOCK ;
    
    PARAMETERS
    {
        TB_BLOCK_MEMBERS_STD;
        TB_BLOCK_MEMBERS_SVI;
    }
    
    PARAMETER_LISTS
    {
        VIEW_1, ptb_view_1 ;
        VIEW_2, ptb_view_2 ;
        VIEW_3, ptb_view_3 ;
        VIEW_4, ptb_view_4 ;
    }
    
    UNIT_ITEMS
    {
        /* pv_unit_relation, */ /* The sviFF use the % as pv unit */
        pressure_unit_relation,
        working_sp_unit_relation,
        travel_unit_relation
    }
#ifndef NOMENUS    
    MENU_ITEMS
    {
        TB_MENU_LIST
    }
#endif /* NOMENUS */    

    METHOD_ITEMS
    {
        TB_METHOD_LIST
    }
    
    COLLECTION_ITEMS
    {
        no_download_ptb,
        upload_wanted_ptb
    }
    
    /* LOCAL_PARAMETERS */
    /* { */
    /*     LOCAL_PARAMETER_LIST */
    /* } */
}


COLLECTION no_download_ptb
{
    LABEL [no_download_label] ;
    HELP  [no_download_help] ;
    MEMBERS {}
}

COLLECTION upload_wanted_ptb
{
    LABEL [upload_wanted_label] ;
    HELP  [upload_wanted_help] ;
    MEMBERS {}
}


/*
**********************************************************************
Positioner TB VIEW
**********************************************************************
*/

VARIABLE_LIST ptb_view_1
{
    LABEL           LBL_PTB_VIEW_1 ;
    HELP            HLP_PTB_VIEW_1 ;
    MEMBERS
    {
        /* PTB_VIEW_1_LIST */
        VL_ST_REV,                       PARAM.ST_REV;
        VL_FINAL_VALUE,                  PARAM.FINAL_VALUE;
        VL_FINAL_POSITION_VALUE,         PARAM.FINAL_POSITION_VALUE;
        VL_WORKING_SP,                   PARAM.WORKING_SP;
        VL_WORKING_POS,                  PARAM.WORKING_POS;
        VL_SUPPLY_PRESSURE,              PARAM.SUPPLY_PRESSURE;
        VL_ACTUATOR_A_PRESSURE,          PARAM.ACTUATOR_A_PRESSURE;
        VL_ACTUATOR_B_PRESSURE,          PARAM.ACTUATOR_B_PRESSURE;
        VL_ATMOSPHERIC_PRESSURE,         PARAM.ATMOSPHERIC_PRESSURE;
        VL_PILOT_PRESSURE,               PARAM.PILOT_PRESSURE;
        VL_TEMPERATURE,                  PARAM.TEMPERATURE;
        VL_IP_DRIVE_CURRENT,             PARAM.IP_DRIVE_CURRENT;
        VL_SETPOINT,                     PARAM.SETPOINT;
        /* VL_MANUAL_SETPOINT,              PARAM.MANUAL_SETPOINT; */
        VL_ACTUAL_POSITION,              PARAM.ACTUAL_POSITION;
        VL_RAW_POSITION,                 PARAM.RAW_POSITION;
    }
}

VARIABLE_LIST ptb_view_2
{
    LABEL           LBL_PTB_VIEW_2 ;
    HELP            HLP_PTB_VIEW_2 ;
    MEMBERS
    {
        /* PTB_VIEW_2_LIST*/
        VL_ST_REV,                       PARAM.ST_REV;
        VL_TAG_DESC,                     PARAM.TAG_DESC;
        VL_STRATEGY,                     PARAM.STRATEGY;
        VL_ALERT_KEY,                    PARAM.ALERT_KEY;
        VL_UI_CUSTOM_CONFIGURATION,      PARAM.UI_CUSTOM_CONFIGURATION;
        VL_UI_ACCESS_CONTROL,            PARAM.UI_ACCESS_CONTROL;
        VL_UI_LANGUAGE,                  PARAM.UI_LANGUAGE;
        VL_ALERT_ACTION,                 PARAM.ALERT_ACTION;
    }
}

VARIABLE_LIST ptb_view_3
{
    LABEL           LBL_PTB_VIEW_3 ;
    HELP            HLP_PTB_VIEW_3 ;
    MEMBERS
    {
        /* PTB_VIEW_3_LIST*/
        VL_ST_REV,                       PARAM.ST_REV;
        VL_FINAL_VALUE,                  PARAM.FINAL_VALUE;
        VL_FINAL_POSITION_VALUE,         PARAM.FINAL_POSITION_VALUE;
        VL_WORKING_SP,                   PARAM.WORKING_SP;
        VL_WORKING_POS,                  PARAM.WORKING_POS;
        VL_SUPPLY_PRESSURE,              PARAM.SUPPLY_PRESSURE;
        VL_ACTUATOR_A_PRESSURE,          PARAM.ACTUATOR_A_PRESSURE;
        VL_ACTUATOR_B_PRESSURE,          PARAM.ACTUATOR_B_PRESSURE;
        VL_ATMOSPHERIC_PRESSURE,         PARAM.ATMOSPHERIC_PRESSURE;
        VL_PILOT_PRESSURE,               PARAM.PILOT_PRESSURE;
        VL_TEMPERATURE,                  PARAM.TEMPERATURE;
        VL_IP_DRIVE_CURRENT,             PARAM.IP_DRIVE_CURRENT;
        VL_SETPOINT,                     PARAM.SETPOINT;
        /* VL_MANUAL_SETPOINT,              PARAM.MANUAL_SETPOINT; */
        VL_ACTUAL_POSITION,              PARAM.ACTUAL_POSITION;
        VL_RAW_POSITION,                 PARAM.RAW_POSITION;

        VL_ST_REV_01,                    PARAM.ST_REV;
        VL_FAILED_STATE,                 PARAM.FAILED_STATE;
        VL_FINAL_VALUE_D,                PARAM.FINAL_VALUE_D;
        VL_FINAL_POSITION_VALUE_D,       PARAM.FINAL_POSITION_VALUE_D;
        VL_FINAL_VALUE_DINT,             PARAM.FINAL_VALUE_DINT;
        VL_FINAL_POSITION_VALUE_DINT,    PARAM.FINAL_POSITION_VALUE_DINT;
        /* VL_FSC_SET_D,                    PARAM.FSC_SET_D; */
        /* VL_FSC_ACTUAL_D,                 PARAM.FSC_ACTUAL_D; */
        VL_DISCRETE_OUTPUT_1_STATE,      PARAM.DISCRETE_OUTPUT_1_STATE;
        VL_DISCRETE_OUTPUT_2_STATE,      PARAM.DISCRETE_OUTPUT_2_STATE;
        VL_DISCRETE_INPUT,               PARAM.DISCRETE_INPUT;

        VL_ST_REV_02,                    PARAM.ST_REV;
        VL_MODE_BLK,                     PARAM.MODE_BLK;
        VL_BLOCK_ERR,                    PARAM.BLOCK_ERR;
        VL_XD_ERROR_POS,                 PARAM.XD_ERROR_POS;
        VL_XD_ERROR_PRESSURE,            PARAM.XD_ERROR_PRESSURE;
        VL_XD_ERROR_TEMPERATURE,         PARAM.XD_ERROR_TEMPERATURE;
        VL_APP_MODE,                     PARAM.APP_MODE;
        VL_COMPLETE_STATUS,              PARAM.COMPLETE_STATUS;
        VL_ALERT_STATE,                  PARAM.ALERT_STATE;
        VL_ALERT_COUNTERS,               PARAM.ALERT_COUNTERS;

        VL_ST_REV_03,                    PARAM.ST_REV;
        VL_POSITION_HISTOGRAM,           PARAM.POSITION_HISTOGRAM;
        VL_POSITION_ERROR_HISTOGRAM,     PARAM.POSITION_ERROR_HISTOGRAM;

        VL_ST_REV_04,                    PARAM.ST_REV;
        VL_POSITION_EXTREMES,            PARAM.POSITION_EXTREMES;
        VL_PRESSURE_EXTREMES,            PARAM.PRESSURE_EXTREMES;
        VL_TEMPERATURE_EXTREMES,         PARAM.TEMPERATURE_EXTREMES;
        VL_IP_CURRENT_EXTREMES,          PARAM.IP_CURRENT_EXTREMES;

        VL_ST_REV_05,                    PARAM.ST_REV;
        VL_UPDATE_EVT,                   PARAM.UPDATE_EVT;
        VL_BLOCK_ALM,                    PARAM.BLOCK_ALM;
        /* VL_WRONG_CONFIGURATION,          PARAM.WRONG_CONFIGURATION; */
        VL_WORKING_TIMES,                PARAM.WORKING_TIMES;
        VL_BLOCK_ERR_DESC_1,             PARAM.BLOCK_ERR_DESC_1;
    }
}

VARIABLE_LIST ptb_view_4
{
    LABEL           LBL_PTB_VIEW_4 ;
    HELP            HLP_PTB_VIEW_4 ;
    MEMBERS
    {
        /* PTB_VIEW_4_LIST             */
        VL_ST_REV,                       PARAM.ST_REV;
        VL_FINAL_VALUE_RANGE,            PARAM.FINAL_VALUE_RANGE;
        VL_POSITION_LIMITS,              PARAM.POSITION_LIMITS;
        VL_FINAL_VALUE_CUTOFF_HI,        PARAM.FINAL_VALUE_CUTOFF_HI;
        VL_FINAL_VALUE_CUTOFF_LO,        PARAM.FINAL_VALUE_CUTOFF_LO;
        VL_CUSTOM_CONTROL_SET,           PARAM.CUSTOM_CONTROL_SET;
        VL_TRAVEL,                       PARAM.TRAVEL;
        VL_XD_FSTATE,                    PARAM.XD_FSTATE;
        VL_READBACK_SELECT,              PARAM.READBACK_SELECT;
        VL_OPEN_STOP_ADJUSTMENT,         PARAM.OPEN_STOP_ADJUSTMENT;
        VL_SETPOINT_SOURCE,              PARAM.SETPOINT_SOURCE;

        VL_ST_REV_01,                    PARAM.ST_REV;
        VL_PRESSURE_RANGE,               PARAM.PRESSURE_RANGE;
        /* VL_PRESSURE_CALIBRATION,         PARAM.PRESSURE_CALIBRATION; */

        VL_ST_REV_02,                    PARAM.ST_REV;
        VL_DEVIATION_ALERT,              PARAM.DEVIATION_ALERT;
        VL_POSITION_HIHI_ALERT,          PARAM.POSITION_HIHI_ALERT;
        VL_POSITION_HI_ALERT,            PARAM.POSITION_HI_ALERT;
        VL_POSITION_LO_ALERT,            PARAM.POSITION_LO_ALERT;
        VL_POSITION_LOLO_ALERT,          PARAM.POSITION_LOLO_ALERT;
        VL_TRAVEL_ACCUMULATION_A_ALERT,  PARAM.TRAVEL_ACCUMULATION_A_ALERT;
        VL_TRAVEL_ACCUMULATION_B_ALERT,  PARAM.TRAVEL_ACCUMULATION_B_ALERT;

        VL_ST_REV_03,                    PARAM.ST_REV;
        VL_CYCLE_COUNTER_A_ALERT,        PARAM.CYCLE_COUNTER_A_ALERT;
        VL_CYCLE_COUNTER_B_ALERT,        PARAM.CYCLE_COUNTER_B_ALERT;
        VL_NEAR_CLOSED_ALERT,            PARAM.NEAR_CLOSED_ALERT;
        VL_SETPOINT_TIMEOUT_ALERT,       PARAM.SETPOINT_TIMEOUT_ALERT;
        VL_WORKING_TIME_ALERT,           PARAM.WORKING_TIME_ALERT;

        VL_ST_REV_04,                    PARAM.ST_REV;
        VL_SUPPLY_PRESSURE_HI_ALERT,     PARAM.SUPPLY_PRESSURE_HI_ALERT;
        VL_SUPPLY_PRESSURE_LO_ALERT,     PARAM.SUPPLY_PRESSURE_LO_ALERT;
        VL_SUPPLY_PRESSURE_LOLO_ALERT,   PARAM.SUPPLY_PRESSURE_LOLO_ALERT;
        VL_TEMPERATURE_HI_ALERT,         PARAM.TEMPERATURE_HI_ALERT;
        VL_TEMPERATURE_LO_ALERT,         PARAM.TEMPERATURE_LO_ALERT;
        VL_IP_DRIVE_CURRENT_HI_ALERT,    PARAM.IP_DRIVE_CURRENT_HI_ALERT;
        VL_IP_DRIVE_CURRENT_LO_ALERT,    PARAM.IP_DRIVE_CURRENT_LO_ALERT;

    }
}

#include "svi_tb.ddl"
#include "svi_blocks.ddl"

/****************************************************************************
 <2012-10-11>         Update for FF_Parameter_Mapping_TB_121005.xlsx
 <2012-09-03>       : add the entries for menus and methods for TB & RB
 <2012-08-31>#25143: adding views for TB, ready for eddl menus design.
*****************************************************************************/
