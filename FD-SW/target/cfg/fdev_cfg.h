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

FILE_NAME         fdev_cfg.h



DESCRIPTION

=========================================================================== */
#ifndef __FDEV_CFG_H__
#define __FDEV_CFG_H__

/* -------------------------------SERIAL NUMBER---------------------------- */
/*   Serial number will be saved in a special 32 byte large constant data   */
/*   segment at begin of flash => max. 32 Bytes or less. 2x16 byte segment  */
/*   needed for easier external change of serial number in hex file.        */
/*                    Only last(!) 22 used in device ID.                    */
#ifdef FC_SPC42
#define DEV_SERIAL_NR             "                  SPC42_V2_42_01"
#elif defined (FIND1_PLUS)
#define DEV_SERIAL_NR             "                  F1____V2_42_01"
#elif defined (FC_FIND1P_DMA)
#define DEV_SERIAL_NR             "                  F1DMA_V2_42_01"
#elif defined (FC_FRONTIER1_PLUS)
#define DEV_SERIAL_NR             "                  FRT___V2_42_01"
#endif
/* ------------------------------------------------------------------------ */

/* --- NV RAM and extra NV RAM cycle time -------------------------------- */
#define NV_RAM_CYCLE_TIME            0ul    /* Unit [s] */
#define EXTRA_NV_RAM_CYCLE_TIME      0ul    /* Unit [s] */



#if (RES_VAL_DEV_TYPE == 0x0008) /* DEVICE_TYPE for SVI Positioner Application */
/* ------------------------------------------------------------------------ */
/* --- SVI Positioner Application ----------------------------------------- */
/* ------------------------------------------------------------------------ */

  #define DEV_CONFIG_REVISION       28   //Changed after RC5 (Vlad's request)

  #define RES_VAL_MANUFAC_ID        0x004745/* Dresser Valve Division      */
/*#define RES_VAL_DEV_TYPE          0x0008   -> defined in makeconfig.mak   */
/*#define RES_VAL_DEV_REV           1        -> defined in makeconfig.mak   */
  #define RES_VAL_DD_REV            1

  #define RES_VAL_MIN_CYCLE_T       3200                 /* Unit: 1/32 msec */

  #define RES_VAL_HARD_TYPES        0xF000  /* Scalar Input    | Scalar Output  |
                                               Discrete Input  | Discrete Output  */


  #define RES_VAL_FEATURES        (  RES_FEAT_REPORTS_SUPP            \
                                   | RES_FEAT_FAULT_STATE_SUPP        \
                                   | RES_FEAT_SOFT_WRITE_LOCK_SUPP    \
                                   | RES_FEAT_HARD_WRITE_LOCK_SUPP    \
                                   | RES_FEAT_OUT_READBACK_SUPP       \
                                /* | RES_FEAT_RPT_MVC_SUPP         */ \
                                   | RES_FEAT_PUB_SUB_MVC_SUPP        \
                                   | RES_FEAT_MULTIBIT_ALARM_SUPP )

  #define RES_VAL_FEATURES_SEL     ( RES_VAL_FEATURES &                  \
                                    ~(  RES_FEAT_MULTIBIT_ALARM_SUPP     \
                                      | RES_FEAT_SOFT_WRITE_LOCK_SUPP    \
                                      | RES_FEAT_HARD_WRITE_LOCK_SUPP) )

  #define RES_VAL_NV_CYCLE_T        (NV_RAM_CYCLE_TIME * 32000ul)  /* Unit [1/32 ms] */
  #define RES_VAL_MAX_NOTIFY        40
  #define RES_VAL_ITK_VER           6

  #undef  DEV_SERIAL_NR
  #define DEV_SERIAL_NR             "________________XXXXXXXXXXXXXXXX"
  #define DEV_DEFAULT_PD_TAG        "SVI_____________"  /* max. 16 bytes, no blanks
                                                           as old hosts have got problems
                                                           with blanks in PD_TAG */
  #define DEV_SM_OPERATIONAL        FALSE
  #define DEV_DEFAULT_NODE_ADDRESS  31
  #define DEV_DEFAULT_DEV_TYPE      DLL_LINKMASTER

#endif /* RES_VAL_DEV_TYPE */

/* --- Field Diagnostics -------------------------------------------------- */
  #define RES_VAL_CHECK_MAP         0x80000000
  #define RES_VAL_MAINT_MAP         0x00000800
  #define RES_VAL_OFFSPEC_MAP       0x000000F0
  #define RES_VAL_FAIL_MAP          0x0000000F

  #define RES_VAL_CHECK_MASK        0x00000000
  #define RES_VAL_MAINT_MASK        0x00000000
  #define RES_VAL_OFFSPEC_MASK      0x00000000
  #define RES_VAL_FAIL_MASK         0x00000000

/* --- DLL busparameters -------------------------------------------------- */
  #define DEV_ST_MIN                8
  #define DEV_MID_MIN               6
  #define DEV_MRD_MIN               5

#endif /* __FDEV_CFG_H__ */
