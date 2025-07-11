/*
Copyright 2008 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**

    \file ao.h
    \brief API of Analog Output

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/includes/ao.h $
    $Date: 1/12/12 2:05p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup sysio
*/
/* (Optional) $History: ao.h $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/12/12    Time: 2:05p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/includes
 * TFS:8674 - added AO interface 
 *
 * *****************  Version 20  *****************
 * User: Arkkhasin    Date: 6/08/09    Time: 11:08p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * A preliminary fix of bug 1069 (AO trim). Span trim still needs work
 *
 * *****************  Version 19  *****************
 * User: Arkkhasin    Date: 4/15/09    Time: 5:59p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * Added ao_GetAOValueTrimmed()
 *
 * *****************  Version 18  *****************
 * User: Arkkhasin    Date: 3/23/09    Time: 3:29p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * Corrected output range checking and added OUTPUT_SATURATED fault
 * definition
 *
 * *****************  Version 17  *****************
 * User: Arkkhasin    Date: 3/10/09    Time: 3:07p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * R/W output direction and ranges reimplemented and extracted
 *
 * *****************  Version 16  *****************
 * User: Arkkhasin    Date: 3/07/09    Time: 12:31p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * First shot at synchronized user trim of AO1 and signal readback:
 * Added sync counter (tag) to trim data
 *
 * *****************  Version 15  *****************
 * User: Arkkhasin    Date: 1/26/09    Time: 12:27a
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * An attempt to repair the DLT sensor power-up sequence
 *
 * *****************  Version 14  *****************
 * User: Sergey Kruss Date: 1/14/09    Time: 8:12p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * Moved direction setting to Cmd_170/171 and removed separate flag field
 * from NVRAM
 *
 * *****************  Version 13  *****************
 * User: Sergey Kruss Date: 1/06/09    Time: 2:30p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * Added current direction
 *
 * *****************  Version 12  *****************
 * User: Arkkhasin    Date: 11/11/08   Time: 2:01p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * Allowed direct AO control in failsafe
 *
 * *****************  Version 11  *****************
 * User: Arkkhasin    Date: 10/17/08   Time: 6:33p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * Removed AO-specific scaling. Now all AO are represented like signal.
 * Opens a way to optimize the AO table, too.
 *
 * *****************  Version 10  *****************
 * User: Arkkhasin    Date: 10/07/08   Time: 1:32a
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * AO freeze API simplified
 *
 * *****************  Version 9  *****************
 * User: Arkkhasin    Date: 9/22/08    Time: 4:52p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * Added a member pointer to read last raw output
 * Added a function to direct-control (both) ao
 *
 *
 * *****************  Version 8  *****************
 * User: Arkkhasin    Date: 7/19/08    Time: 9:16p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * Added multiple AO channels support with table-driven configuration
 * AO interfaces became ao_ (from sysio_) and take AO id as a paramter
 *
 * *****************  Version 7  *****************
 * User: Arkkhasin    Date: 5/14/08    Time: 4:54p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * Adapted to new tempcomp interface
 *
 * *****************  Version 6  *****************
 * User: Arkkhasin    Date: 4/07/08    Time: 10:04a
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * New NVMEM strategy:
 * Type-safe accessors versions use SAFE{S,G}ET[X] macros
 * Firmware defaults are now static (again)
 * AO-related functions are ao_ (not cnfg_)
 *
 * *****************  Version 5  *****************
 * User: Arkkhasin    Date: 3/24/08    Time: 5:03p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * Fifth step toward uniform support of engineering units:
 * Ranges of all variables are exposed
 * Type of AO_SCALE_MULTIPLIER corrected
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 3/17/08    Time: 5:33p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * Second step toward uniform support of engineering units:
 * Valid range limits for any parameter are owned by a corresponding
 * module and exposed as an array of 2 elements (low and high limits)
 * ALSO, moved AO-related prototypes (to ao.h)
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 2/25/08    Time: 2:56p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * Moved AO-related stuff from mncbtypes.h and configure.h and param.h to
 * ao.h
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 1/25/08    Time: 9:03a
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * Lint (added ao_Calibrate for former cnfg_CalibrateAO)
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 1/10/08    Time: 3:01p
 * Created in $/MNCB/Dev/FIRMWARE/includes
 * All AO-related things except its use for position retransmit is moved
 * to ao{.c,.h}
*/
#ifndef AO_H_
#define AO_H_

#include "errcodes.h"
#include "tempcomp.h"
#include "dimensions.h"
#include "position.h"
#include "insignal.h"

// -------------- AO - analog output (used e.g. for position retransmit) ---------------

//types
typedef sig_t ao_t;
typedef sig_least_t ao_least_t;

//factual AO paramters
#define AO_FP_LIMIT_LOW 0.0     //! low limit AO output, mA
#define AO_FP_LIMIT_HIGH (22.0) //! high limit AO output, mA
#define AO_LIMIT_LOW 0     //! low limit AO output, in internal counts
#define AO_LIMIT_HIGH 4096 //! high limit AO output, in internal counts

#define AO_SIGNAL_LOW_LIMIT 100  //! an arbitray value less than 0.2 * 4096, in internal counts

//derived AO parameters
#define AO_RANGE_MAX (AO_LIMIT_HIGH-AO_LIMIT_LOW) //!< range of AO output, in internal counts

#define AO_HIGH_SIGNAL 20.0 //! in mA

//Common default AO range values for all AO channels
#define DEFAULT_fLowAOCal (4.0F) //mA
#define DEFAULT_fHighAOCal (20.0F) //mA

#define AO_RANGE_SCALE(aorange) (s32)((cdouble_t)(aorange) * ((DEFAULT_fHighAOCal-DEFAULT_fLowAOCal)/DEFAULT_fHighAOCal))
#define AO_LOW_RANGE_SCALE (ao_t)((cdouble_t)AO_RANGE_MAX * (DEFAULT_fLowAOCal/DEFAULT_fHighAOCal))

//AO (position retransmit) calibration limits
#define AO_CAL_DIF (5.0) //min difference between AO calib limits, mA
#define AO_CAL_LOW_LOW (3.5) //! low limit of low AO calib limit, mA
#define AO_CAL_LOW_HIGH (8.5) //! high limit of low AO calib limit, mA
#define AO_CAL_HIGH_LOW (15.5) //! low limit of high AO calib limit, mA
#define AO_CAL_HIGH_HIGH AO_FP_LIMIT_HIGH //! high limit of high AO calib limit, mA

//--------------- AO cal limits (integer counts) --------------------

//lint -emacro(747, AO_CAL_LOW_LOW_INT, AO_CAL_LOW_HIGH_INT, AO_CAL_HIGH_LOW_INT, AO_CAL_HIGH_HIGH_INT)
#define AO_CAL_LOW_LOW_INT   STD_FROM_MA(AO_CAL_LOW_LOW)
#define AO_CAL_LOW_HIGH_INT  STD_FROM_MA(AO_CAL_LOW_HIGH)
#define AO_CAL_HIGH_LOW_INT  STD_FROM_MA(AO_CAL_HIGH_LOW)
#define AO_CAL_HIGH_HIGH_INT STD_FROM_MA(AO_CAL_HIGH_HIGH)
CONST_ASSERT(AO_CAL_HIGH_LOW_INT>AO_CAL_LOW_HIGH_INT); //play it safe since AO_FP_LIMIT_HIGH is not immediately visible

/* Like in scale calculation in position.c, we don't want to handcraft it for (accidental)
choice of internal representation, nor do we want arithmetic errors depend on it.
So we resort to 64-bit arithmetic in calculating the scale(s) in order to do multiplication
before the division, with automatic compile-time
derivation of the corresponding constants such that the result fits in 32 bits.
Since the 64-bit calculations occur rarely - on calibration change - I don't think it's
a big deal to worry about.
*/
#define POS_TO_AO_SCALE_SHIFT MIN(30, ((30 - INTLOG(AO_STD_RANGE)) + INTLOG(RV_MIN_SPAN_AO)))

#define AO_STD_RANGE (AO_MAP_URV-AO_MAP_LRV)
#define AO_MAP_URV STD_FROM_MA(20.0) //! upper pos RV AO_1 output, in internal counts ~ 20 mA fixed
#define AO_MAP_LRV STD_FROM_MA(4.0) //! lower pos RV AO_1 output, in internal counts ~ 4 mA fixed

typedef struct AOCalibration_t
{
    ao_t AOCal[Xends]; //!< low/high entered (user-measured) AO cal values
    ao_t nAOCalAD[Xends]; //!< low/high device-measured AO cal values (after = without any user cals)
    u32 SeqNumber; //!< the number of performed calibrations (exceeding 4 billion not expected)
    u16 CheckWord;
}AOCalibration_t;

typedef struct AOComputedCalibration_t
{
    s32 Span;
    ao_t PreOffset;
    ao_t PostOffset;
    u16 CheckWord;
}AOComputedCalibration_t;

SAFESETX(ao_SetAOCalibration, AOCalibration_t);
SAFEGETX(ao_GetAOCalibration, AOCalibration_t);

extern ErrorCode_t ao_SetWorkingAOCalibration(u8_least index, const AOCalibration_t* pAOCalibration);

extern ErrorCode_t  ao_Calibrate(u8_least index, u8 CalType, ao_t CalValue);

MN_DECLARE_API_FUNC(ao_IsFixedAO) //optional use for a given project
extern bool_t ao_IsFixedAO(u8_least index);
extern ao_t ao_GetAOValue(u8_least index);
extern ao_t ao_GetAOValueTrimmed(u8_least index);

extern void ao_Freeze(u8_least index, bool_t freeze);

extern ErrorCode_t ao_SetAOValueFixed(u8_least index, ao_least_t AOValue);

extern void ao_SetPositionRetransmit(u8_least index, pos_least_t PositionStd);
extern void ao_SetAOValue(u8_least index, ao_least_t AOValue, bool_t usrcal);

//AO channel ids
enum
{
    AO_CHANNEL_POS_RETRANSMIT
#if AO_NUM_CHANNELS > 1
    , AO_CHANNEL_LOOP_CURRENT
#endif
};

//Tempcomp entries cannot be arrays yet:
TEMPCOMP_H_ENTRY(ao0);
TEMPCOMP_H_ENTRY(ao1);
//etc. as needed
//typedef ao_TCalRow_t ao0_TCalRow;

typedef struct aotab_t
{
    void (*writef)(u16 value);
    u16 (*readf)(void); //!< Read last value
    tcorr_t (*tcorr)(s32 adval);
    u16 output_hw_range[Xends];
    ao_t aoNormal_range[Xends];
    ao_t aoFixed_range[Xends];
    ao_t maxCalDifference; //! How far cal value can be from actual read
    ao_t CalMinRange_Raw; //!< How close the output counts may be
    u8 aoUsercal_scaleShift; //scale shift to compute user calibration mapping of ao_t on itself
    ao_t ao_CalLimits[Xends][Xends]; //boundaries of low/high calibration values
} aotab_t;
extern const aotab_t aotab[AO_NUM_CHANNELS];

extern const AOCalibration_t def_AOCalibration; //doesn't need to be per channel

extern ErrorCode_t ao_SetDirectControl(u8_least index, bool_t enable, u16 DirectControlVal);
extern bool_t ao_IsDirectControl(u8_least index, u16 *value);

#endif //AO_H_
/* This line marks the end of the source */
