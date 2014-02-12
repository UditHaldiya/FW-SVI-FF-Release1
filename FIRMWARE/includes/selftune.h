/*
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file selftune.h
    \brief Data owner for PID and its self-tuner data and handles get/save of data

    CPU: Any (with standard C compiler)

    OWNER: Zhou
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/includes/selftune.h $
    $Date: 1/20/12 10:44a $
    $Revision: 31 $
    $Author: Arkkhasin $
*/

/*
    $History: selftune.h $
 *
 * *****************  Version 31  *****************
 * User: Arkkhasin    Date: 1/20/12    Time: 10:44a
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/includes
 * TFS:8748 - control types moved to selftune.h
 *
 * *****************  Version 30  *****************
 * User: Arkkhasin    Date: 11/15/11   Time: 3:57p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/includes
 * Preliminary check-in for
 * TFS:8202 decouple I/O subsystem
 *
 * *****************  Version 29  *****************
 * User: Arkkhasin    Date: 11/04/11   Time: 4:46p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/includes
 * TFS:8072 NVMEM upgrade
 *
 * *****************  Version 27  *****************
 * User: Arkkhasin    Date: 1/05/11    Time: 3:03p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * TFS:4607 (Removed T0_050t) and standard tombstone header
*/

#ifndef SELFTUNE_H_
#define SELFTUNE_H_
#include "pressuredef.h"
#include "errcodes.h"
#include "crc.h"
#include "process.h"

typedef struct TuneData_t
{
    pres_t SupplyPres_User;
    u8  n1TunePara1;
    s8  n1Level;
    CHECKFIELD;
} TuneData_t;

typedef struct PIDData_t
{
    u16 P;
    u16 I;
    u16 D;
    s16 PAdjust;
    s16 Beta;
    //removed s16 Damping;
    s16 PosComp;
    u16 DeadZone;
    u16 Band;
    //s16 NonLin;
    //removed s16 Reserved1;
    //removed s16 Reason;
    CHECKFIELD;
} PIDData_t;


extern const pres_t SupplyPressure_range[Xends]; //for user supply pressure

/** Public Defines  */
// copied from legacy svi2defs.h
/* miscellaneous states or flags */
#define POS 1
/* Masoneilan Faults */
#define PERCENT_TO_SCALED_RANGE 163.84F     /* 16384/100 */

/** Additive part of position noise (guess) */
#define NOISE 40 //Should be 0.244% ?

#define POS_STAY_LOW 2458
#define POS_STAY_HIGH 13107

#define PRES_SUPPLY_HI  STD_FROM_PSI(150.0F) //was 25000   // 100.0F ///AK:NOTE: Changed to STD_FROM_PSI(100.0)
#define PRES_SUPPLY_LO  STD_FROM_PSI(20.0F)  //was 3750  // 15.0F    ///AK:NOTE: Changed to STD_FROM_PSI(15.0)
#define BAND_SUPPLY_RATIO  STD_FROM_PSI(6.6F) //was 1650 before change of pressure scaling - now 1320

#define P_HIGH_LIMIT  5000
#define I_HIGH_LIMIT  1000
#define D_HIGH_LIMIT  200
#define PADJ_LOW_LIMIT  -3000
#define PADJ_HIGH_LIMIT  3000
#define BETA_LOW_LIMIT  -9
#define BETA_HIGH_LIMIT  9
#define PCOMP_LOW_LIMIT  2
#define PCOMP_HIGH_LIMIT  20
//#define BOOST_HIGH_LIMIT  20
#define BOOST_HIGH_LIMIT ((PRES_SUPPLY_HI/BAND_SUPPLY_RATIO)+1)  //compute from pres limits and round up
#define DEADZONE_HIGH_LIMIT  821

// #define SUPPLY_LOW  1.0F
// #define SUPPLY_HIGH  1500.0F
#define TUNE_LEVEL_LOW  -9
#define TUNE_LEVEL_HIGH  9

/** constants for scaling */



/** Public functions
*/
#ifdef OLD_NVRAM
extern void  tune_InitTuneData(InitType_t Type);
#endif

SAFEGETX(tune_GetPIDData, PIDData_t);
SAFESETX(tune_SetPIDData, PIDData_t);
extern const PIDData_t *tune_GetCurrentPIDData(PIDData_t *dst);

#define CTLPARAMSET_AUTOTUNE 0 //!< Index of tunable set of PID parameters
#define CTLPARAMSET_5CAMFLEX 1
#define CTLPARAMSET_6CAMFLEX 2
#define CTLPARAMSET_10_20    3
#define CTLPARAMSET_10_24    4
#define CTLPARAMSET_23_12    5
#define CTLPARAMSET_9CAMFLEX 6
#define CTLPARAMSET_23_24    7

#define MANUALTUNE_MASK      0x07u

//required interface
extern u8 posctl_GetCtlSetIndex(void);

typedef struct CtlSetSlot_t
{
    u8 slot;
    u16 CheckWord;
} CtlSetSlot_t;

SAFEGET(posctl_GetCtlset, CtlSetSlot_t);
SAFESET(posctl_SetCtlset, CtlSetSlot_t);

#ifdef OLD_NVRAM
//NVRAM Shims
extern void posctl_Init(InitType_t itype);
extern void posctl_Save(void);
#endif //OLD_NVRAM

/** \brief Indicates whether the currently effective control param set
is tunable.
\return true iff tunable
*/
MN_INLINE bool_t posctl_IsTunableCtlset(void)
{
    return posctl_GetCtlSetIndex()==CTLPARAMSET_AUTOTUNE;
}


extern procresult_t tune_Run_Selftune(s16 *procdetails);

SAFEGET(tune_GetTuneData, TuneData_t);
SAFESET(tune_SetTuneData, TuneData_t);

extern s16 tune_GetCompletionCode(void);

#endif  /** SELFTUNE_H_  */

/* end of the source */
