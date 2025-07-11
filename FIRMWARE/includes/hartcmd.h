/**
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file hartcmd.h
    \brief Defines used in each hart command - exported to use in the command table

    The Mesco HART module must be modified to pass pointers to all
    of the hart command functions.

    CPU: Any

    OWNER: LS
    \n RCS: $Id: $
     $DateTime: $ $Change: $ $Author: Arkkhasin $
*/

#ifndef HARTCMD_H_
#define HARTCMD_H_
#include "errcodes.h" //Define's and prototypes
#include "devicemode.h" //Define's and prototypes
#include "configure.h" //for OPTION_CONFIG_BYTES

#ifndef OLD_DEVMODE

#define HC_ALLOWED_MODE_SHIFT 4u
#define HC_MODE_BITMASK(mode) (((u8)(mode))<<HC_ALLOWED_MODE_SHIFT)

#define HC_ALLOWED_MODE_FS HC_MODE_BITMASK(MODE_FAILSAFE)
#define HC_ALLOWED_MODE_MAN HC_MODE_BITMASK(MODE_MANUAL)
#define HC_ALLOWED_MODE_OOS HC_MODE_BITMASK(MODE_SETUP)
#define HC_ALLOWED_MODE_NORMAL HC_MODE_BITMASK(MODE_OPERATE)
//Handy combos
#define HC_ALLOWED_MODE_ALL (HC_ALLOWED_MODE_MAN|HC_ALLOWED_MODE_OOS|HC_ALLOWED_MODE_NORMAL|HC_ALLOWED_MODE_FS) //note HC_ALLOWED_MODE_FS present
//#define HC_ALLOWED_MODE_MAN_OOS (HC_ALLOWED_MODE_MAN|HC_ALLOWED_MODE_OOS)
//Extractor mask
//#define HC_ALLOWED_MODE_MASK HC_ALLOWED_MODE_ALL

#endif

#define HC0_DUMMY 0u
#define HC0_MFGID 1u
#define HC0_DEVICE_TYPE 2u
#define HC0_PREAMBLES 3u
#define HC0_CMDREV 4u
#define HC0_TXREV 5u
#define HC0_SWREV 6u
#define HC0_HWREV 7u
#define HC0_FLAG 8u
#define HC0_DEVICE_ID 9u
#define HC0_LENGTH 12u
#define HC0_REQ_LENGTH 0u
#define HC0_FLAGS (HC_READ_COMMAND | HC_ALLOWED_MODE_ALL)

#define HC1_POS_UNITS 0u
#define HC1_POS 1u
#define HC1_LENGTH 5u
#define HC1_REQ_LENGTH 0u
#define HC1_FLAGS (HC_READ_COMMAND | HC_ALLOWED_MODE_ALL)

#define HC2_SIGNAL 0u
#define HC2_SIGNAL_PERCENT 4u
#define HC2_LENGTH 8u
#define HC2_REQ_LENGTH 0u
#define HC2_FLAGS (HC_READ_COMMAND | HC_ALLOWED_MODE_ALL)

#define HC3_SIGNAL 0u
#define HC3_POS_UNITS 4u
#define HC3_POS 5u
#define HC3_PRES_UNITS 9u
#define HC3_PRES 10u
#define HC3_TV_UNITS 14u
#define HC3_TV 15u
#define HC3_QV_UNITS 19u
#define HC3_QV 20u
#define HC3_LENGTH 24u
#define HC3_REQ_LENGTH 0u
#define HC3_FLAGS (HC_READ_COMMAND | HC_ALLOWED_MODE_ALL)

#define HC6_POLLING_ADDRESS 0u
#define HC6_LENGTH 1u
#define HC6_REQ_LENGTH 1u
#define HC6_FLAGS (HC_WRITE_COMMAND | HC_ALLOWED_MODE_ALL)

#define HC11_LENGTH 12u
#define HC11_REQ_LENGTH 6u
#define HC11_FLAGS (HC_READ_COMMAND | HC_ALLOWED_MODE_ALL)

#define HC12_MESSAGE 0u
#define HC12_LENGTH 24u
#define HC12_REQ_LENGTH 0u
#define HC12_FLAGS (HC_READ_COMMAND | HC_ALLOWED_MODE_ALL)

#define HC13_TAG 0u
#define HC13_DESCRIPTOR 6u
#define HC13_DATE 18u
#define HC13_LENGTH 21u
#define HC13_REQ_LENGTH 0u
#define HC13_TAG_LENGTH 6u
#define HC13_DESCRIPTOR_LENGTH 12u
#define HC13_DATE_LENGTH 3u
#define HC13_FLAGS (HC_READ_COMMAND | HC_ALLOWED_MODE_ALL)

#define HC14_SERIAL 0u
#define HC14_UNITS 3u
#define HC14_ULIM 4u
#define HC14_LLIM 8u
#define HC14_SPAN 12u
#define HC14_LENGTH 16u
#define HC14_REQ_LENGTH 0u
#define HC14_FLAGS (HC_READ_COMMAND | HC_ALLOWED_MODE_ALL)

#define HC15_ALARM 0u
#define HC15_TXFR_FCN 1u
#define HC15_UNITS 2u
#define HC15_URV 3u
#define HC15_LRV 7u
#define HC15_DAMPING 11u
#define HC15_WRITE_PROT 15u
#define HC15_PRIVATE_LABEL 16u
#define HC15_LENGTH 17u
#define HC15_REQ_LENGTH 0u
#define HC15_FLAGS (HC_READ_COMMAND | HC_ALLOWED_MODE_ALL)

#define HC16_FINAL_ASSY 0u
#define HC16_LENGTH 3u
#define HC16_REQ_LENGTH 0u
#define HC16_FLAGS (HC_READ_COMMAND | HC_ALLOWED_MODE_ALL)

#define HC17_MESSAGE 0u
#define HC17_LENGTH 24u
#define HC17_REQ_LENGTH 24u
#define HC17_FLAGS (HC_WRITE_COMMAND | HC_ALLOWED_MODE_ALL)

#define HC18_LENGTH 21u
#define HC18_REQ_LENGTH 21u
#define HC18_FLAGS (HC_WRITE_COMMAND | HC_ALLOWED_MODE_ALL)

#define HC19_FINAL_ASSY 0u
#define HC19_LENGTH 3u
#define HC19_REQ_LENGTH 3u
#define HC19_FLAGS (HC_WRITE_COMMAND | HC_ALLOWED_MODE_ALL)

#if 0
#define HC35_UNITS_CODE 0u
#define HC35_URV 1u
#define HC35_LRV 5u
#define HC35_LENGTH 9u
#define HC35_REQ_LENGTH HC35_LENGTH
#define HC35_FLAGS (HC_WRITE_COMMAND | HC_ALLOWED_MODE_ALL)
#endif

#define HC38_LENGTH 0u
#define HC38_REQ_LENGTH 0u
#define HC38_FLAGS (HC_WRITE_COMMAND | HC_ALLOWED_MODE_ALL)

#if 0
#define HC40_FIXED_VALUE 0u
#define HC40_LENGTH 4u
#define HC40_REQ_LENGTH HC40_LENGTH
#define HC40_FLAGS (HC_PROCESS_COMMAND | HC_ALLOWED_MODE_ALL)

#define HC45_MEASURED_VALUE 0u
#define HC45_LENGTH 4u
#define HC45_REQ_LENGTH HC45_LENGTH
#define HC45_FLAGS (HC_WRITE_COMMAND | HC_ALLOWED_MODE_ALL)

#define HC46_MEASURED_VALUE 0u
#define HC46_LENGTH 4u
#define HC46_REQ_LENGTH HC46_LENGTH
#define HC46_FLAGS (HC_WRITE_COMMAND | HC_ALLOWED_MODE_ALL)
#endif

#define HC108_CMD 0u
#define HC108_LENGTH 1u
#define HC108_REQ_LENGTH 1u
#define HC108_FLAGS (HC_WRITE_COMMAND | HC_ALLOWED_MODE_ALL)

#define HC109_CTL 0u
#define HC109_LENGTH 1u
#define HC109_REQ_LENGTH 1u
#define HC109_FLAGS (HC_WRITE_COMMAND | HC_ALLOWED_MODE_ALL)

#if 0
#define HC128_VAR_NUMBER 0u
#define HC128_REQ_LENGTH 1u
#define HC128_ADDRESS 0u
#define HC128_TYPE 2u
#define HC128_SIZE 3u
#define HC128_LENGTH 5u
#define HC128_FLAGS (HC_READ_COMMAND | HC_ALLOWED_MODE_ALL | HC_FACTORY_COMMAND)
#endif


#define HC138_FLAG 0u
#define HC138_LENGTH 0u
#define HC138_REQ_LENGTH 1u
#define HC138_FLAGS (HC_READ_COMMAND | HC_ALLOWED_MODE_ALL)

#define HC139_TRAVEL 0u
#define HC139_TIME_CLOSED 4u
#define HC139_TIME_OPEN 8u
#define HC139_TIME_NEAR 12u
#define HC139_CYCLES 16u
#define HC139_LENGTH 20u
#define HC139_REQ_LENGTH 0u
#define HC139_FLAGS (HC_READ_COMMAND | HC_ALLOWED_MODE_ALL)

#define HC140_LENGTH 0u
#define HC140_REQ_LENGTH 0u
#define HC140_FLAGS (HC_PROCESS_COMMAND | HC_ALLOWED_MODE_OOS)

#define HC141_RAW_SIGNAL 0u
#define HC141_RAW_POSITION 2u
#define HC141_RAW_PRESSURE 4u
#define HC141_IPOUT 6u
#define HC141_LOW_POS_STOP 8u
#define HC141_HIGH_POS_STOP 10u
#define HC141_TEMP 12u
#define HC141_LOW_TEMP 14u
#define HC141_HIGH_TEMP 16u
#define HC141_IPCURRENT 18u
#define HC141_LENGTH 20u
#define HC141_REQ_LENGTH 0u
#define HC141_FLAGS (HC_READ_COMMAND | HC_ALLOWED_MODE_ALL)

#if 0
#define HC160_IPOut 0u
#define HC160_LENGTH 2u
#define HC160_REQ_LENGTH HC160_LENGTH
#define HC160_FLAGS (HC_PROCESS_COMMAND | HC_ALLOWED_MODE_OOS)
#endif

#define HC174_BIAS_SHIFT 0u
#define HC174_BIAS_ADD_AIRLOSS 2u
#define HC174_BAIS_ADD 4u
#define HC174_BIAS_TEMP_COEF 6u
#define HC174_HYSTERESIS 8u
#define HC174_LENGTH 10u
#define HC174_REQ_LENGTH HC174_LENGTH
#define HC174_FLAGS (HC_WRITE_COMMAND | HC_ALLOWED_MODE_OOS | HC_ALLOWED_MODE_FS)

#define HC181_LOW_SIGNAL 0u
#define HC181_HIGH_SIGNAL 4u
#define HC181_LENGTH 8u
#define HC181_REQ_LENGTH HC181_LENGTH
#define HC181_FLAGS (HC_WRITE_COMMAND | HC_ALLOWED_MODE_OOS | HC_ALLOWED_MODE_FS)

#define HC183_LENGTH 0u
#define HC183_REQ_LENGTH 0u
#define HC183_FLAGS (HC_WRITE_COMMAND | HC_ALLOWED_MODE_OOS | HC_ALLOWED_MODE_FS)


#define HC200_OPTION_CONFIG 0u
#define HC200_LENGTH  OPTION_CONFIG_BYTES
#define HC200_REQ_LENGTH 0u
#define HC200_FLAGS (HC_READ_COMMAND | HC_ALLOWED_MODE_ALL)

#define HC202_OPTION_CONFIG 0u
#define HC202_LENGTH  OPTION_CONFIG_BYTES
#define HC202_REQ_LENGTH 0u
#define HC202_FLAGS (HC_READ_COMMAND | HC_ALLOWED_MODE_ALL)

#define HC213_LENGTH HC181_LENGTH
#define HC213_REQ_LENGTH    0u
#define HC213_FLAGS (HC_READ_COMMAND | HC_ALLOWED_MODE_ALL)

#define HC217_LENGTH HC174_LENGTH
#define HC217_REQ_LENGTH    0u
#define HC217_FLAGS (HC_READ_COMMAND | HC_ALLOWED_MODE_ALL)

#define HC248_KEY 0u
#define HC248_OPTION_CONFIG 2u
#define HC248_LENGTH  (OPTION_CONFIG_BYTES + HC248_OPTION_CONFIG)
#define HC248_REQ_LENGTH HC248_LENGTH
#define HC248_FLAGS (HC_WRITE_COMMAND | HC_ALLOWED_MODE_ALL | HC_FACTORY_COMMAND)
#define HC248_KEY_VALUE 0xaf0e

#define HC253_LENGTH 0u
#define HC253_REQ_LENGTH    0u
#define HC253_FLAGS (HC_READ_COMMAND | HC_ALLOWED_MODE_ALL)

///prototypes are in hartdef.h (generated by hartdef.u)
//Generic prototype: extern s8_least hart_Command_<number>_<HumanReadableName>(void);

//-------------- interfaces to common command handler ----------------

//------------------------- Stolen from the trunk -------------------------
//Types known to automatic HART commands import
#define H_UCHAR u8
#define H_CHAR u8
#define H_ASCII u8
#define H_FLOAT float32
#define H_UINT u16
#define H_INT s16
#define H_UINT4 u32
#define H_INT4 s32
typedef u8 H_UINT3[3];
typedef u8 H_INT3[3];

extern s8_least err2hart(ErrorCode_t err);
//----------------------- End Stolen from the trunk -------------------------

#endif //HARTCMD_H_
/* This line marks the end of the source */
