/**
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file configure.h
    \brief Data owner for configuration data and handles get/save of data

    CPU: Any (with standard C compiler)

    OWNER: Schoonover
    \n RCS: $Id$
    Created:  9/30/04 $Change: $ Author: Schoonover
*/

#ifndef CONFIGURE_H_
#define CONFIGURE_H_

#include "errcodes.h"

#define OPTION_CONFIG_BYTES 16u

//config type (was missing)
typedef struct OptionConfig_t OptionConfig_t;
struct OptionConfig_t
{
    u8 m_OptionConfig[OPTION_CONFIG_BYTES];
    u16 CheckWord;
};

/** Public Defines  */

//lint ++flb //Options are predefined for tools use; don't have to be referenced
/** Option Config byte 0 */
#define PRESSURE_SENSOR_1 0u            /** 0x01 */
#define PRESSURE_SENSOR_2 1u            /** 0x02 */
#define PRESSURE_SUPPLY 2u              /** 0x04 */
#define DO_OPTION 3u                /** 0x08 */
#define POSITION_RETRANSMIT_OPTION 4u   /** 0x10  */

//#define DASH_VERSION (0x20 | 0x40 | 0x80)
#define DASH_VERSION_BYTE 0u
#define DASH_VERSION_SHIFT 5u
#define DASH_VERSION_MASK 7u

//dash version types
#define LOW_COST_VERSION 1u
#define STANDARD_VERSION 2u
#define ADVANCED_DIAG_VERSION 3u
#define DIAG_PLUS_VERSION 4u
#define PROCESS_CONTROLER_VERSION 5u
#define ESD_VERSION 6u

/** Option Config byte 1 */
//AK: Now a user option #define REMOTE_POSITION_SENSOR 8u       /** 0x01 */
//AK: moved to pneumatic params  #define SINGLE_ACTING 9u               /** 0x02 */
#define PV_INPUT 10u                    /** 0x04 */
#define PRESSURE_ATM 11u                /** 0x08 */
#define FACTORY_CAL_MODE 12u            /** 0x10 */
#define DI_OPTION 13u                   /** 0x20 */

//option config byte 5 - additional settings
#define STRICT_HART_CONFORMANCE 40u    /* 0x01 */

//lint --flb end options

/** Public functions
*/
extern const OptionConfig_t* cnfg_GetFillOptionConfig(OptionConfig_t *dst);
extern ErrorCode_t  cnfg_SetOptionConfig(const OptionConfig_t* pOptionConfig);
UNTYPEACC(cnfg_SetOptionConfig, cnfg_GetFillOptionConfig);
#define cnfg_GetOptionConfig() (cnfg_GetFillOptionConfig(NULL)) //deprecated

LINT_PURE(cnfg_GetOptionConfigFlag)
extern bool_t  cnfg_GetOptionConfigFlag(u8_least OptionConfigCode);
extern u8_least  cnfg_GetOptionConfigDashVersion(void);

#endif  /** CONFIGURE_H_  */

/* end of the source */
