/*
Copyright 2010 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file pneumatics.c
    \brief A collection of pneumatics-related parameters

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/tasks/control/pneumatics.c $
    $Date: 1/23/12 3:27p $
    $Revision: 12 $
    $Author: Arkkhasin $

    \ingroup control
*/
/* $History: pneumatics.c $
 *
 * *****************  Version 12  *****************
 * User: Arkkhasin    Date: 1/23/12    Time: 3:27p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/tasks/control
 * TFS:8751 - Intentionally goofy default parameters
 *
 * *****************  Version 11  *****************
 * User: Arkkhasin    Date: 12/01/11   Time: 2:16p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/tasks/control
 * TFS:8313 Lint
 *
 * *****************  Version 10  *****************
 * User: Arkkhasin    Date: 11/04/11   Time: 7:41p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/tasks/control
 * TFS:8072 NVMEM upgrade
 *
 * *****************  Version 8  *****************
 * User: Anatoly Podpaly Date: 5/10/11    Time: 1:39p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks/control
 * TFS:5255 -- pneumatic parameters were reviewed and analyzed at depth by
 * teh F/W team and Mechanical Engineers. MAX Actuator Pressure was
 * defined as 80.0 psi -- shoould be changed to 100.0 psi.
 * The rest of the parameters -- no changes.
 *
 * *****************  Version 7  *****************
 * User: Justin Shriver Date: 3/21/11    Time: 7:12p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks/control
 * TFS:5912
 * For control also TFS:5913
 *
 * *****************  Version 6  *****************
 * User: Anatoly Podpaly Date: 11/23/10   Time: 7:47p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks/control
 * Bug 4352 - enabled the Atmpspheric pressure sensor in default sensor
 * map.
 *
 * *****************  Version 5  *****************
 * User: Arkkhasin    Date: 8/26/10    Time: 7:15p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks/control
 * Bug 3965 - added a const attribute; tested by compiling and linting
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 3/30/10    Time: 6:40p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks/control
 * Constrained the use of nvramtypes.h
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 3/12/10    Time: 5:05p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks/control
 * Oops. Default to single-acting now
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 3/12/10    Time: 11:45a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks/control
 * Corrected the order of boost coefs
 * Added an OS guard to saving in FRAM
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 3/09/10    Time: 1:11a
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/tasks/control
 * Added PneumaticParams
*/
#include "mnwrap.h"
#include "pneumatics.h"
#include "nvram.h"
#include "adtype.h" //AD_yyy
#include "faultpublic.h"
static PneumaticParams_t PneumaticParams;

/** \brief Default parameters good for nothing.
We want to make sure that something sensible was written
at device initialization, so we have goofy firmware defaults
*/
static const PneumaticParams_t PneumaticParams_Default =
{
    .boostCoeff =  //SA -1, may need to change for LCX
    {
        .boost = {0, 0},
        .BoostOffset = 0,
    },
    .presLimitsPilot =
    {
        .presLimit =
        {
            [Xlow] = 0,
            [Xhi] = 0,
        },
    },
    .SupplyLossThreshold_Pilot = 0,
    .SupplyLossThreshold_Supply = 0, //should be INT16_MIN, //this would turn off supply test
    .SingleActing = 1, //any non-zero
    .SensorMap =
    {
        [PRESSURE_ACT1_INDEX] = AD_NOT_INSTALLED,
        [PRESSURE_ACT2_INDEX] = AD_NOT_INSTALLED,
        [PRESSURE_SUPPLY_INDEX] = AD_NOT_INSTALLED,
        [PRESSURE_PILOT_INDEX] = AD_NOT_INSTALLED,
        [PRESSURE_ATM_INDEX] = AD_NOT_INSTALLED,
    },
    .CheckWord = 0 //Don't care
};

#if 0 //Uncomment to compile and inspect the default values in the listing

#include "Monitors/pressmon_pilot_hw.h"

//These are the minimum pressures
//This is based on the fact that RELAY (AP-1) and AP-2 have minimum relay pressures (zeros)
//at around 7.5psi
//TFS:8333 #define PILOT_LOW_PRESS_LIMIT_SINGLE_RELAY STD_FROM_PSI(6.0F)

// This is a MAX Actuator Pressure -- shall be 100 psi for SVi1000
#define PRESSURE_LIMIT_AP_LCX (100.0F)
#define GAIN_AP_LCX 6.821F
#define MAX_ZERO_AP_LCX  7.987F
#define PILOT_HIGH_PRESS_LIMIT_AP_LCX STD_FROM_PSI(MAX_ZERO_AP_LCX + (PRESSURE_LIMIT_AP_LCX/GAIN_AP_LCX))

#define BOOST_CO(x, y)   ((u16)((((float32)(x) / 19.0F) * (y)) + .5F))
#define BOOST_COEF(x) {[Xhi] = BOOST_CO(x, 1.0F), [Xlow] = BOOST_CO(x, .6F)}
//#define BOOST_COEF_SA(x) {BOOST_CO(x, 1.0F), BOOST_CO(x, .625F)}

#define SUPPLY_LOSS_THRESHOLD_PILOT  STD_FROM_PSI(.8F)          //!< detect supply loss below this point if no supply sensor or it is bad
//#define SUPPLY_LOSS_THRESHOLD_CURRENT CURR_MA_FROM_MA(1.6)

#define BOOST_OFFSET_HIGH_SA    0                         //!< beginning of SA flat spot
//#define BOOST_OFFSET_LOW_SA     (BOOST_OFFSET_HIGH_SA - 40)

/** \brief Default parameters good for LCX=SVi1000
*/
__root const PneumaticParams_t PneumaticParams_LCX =
{
    .boostCoeff =  //SA -1, may need to change for LCX
    {
        .boost = BOOST_COEF(953),
        .BoostOffset = BOOST_OFFSET_HIGH_SA,
    },
    .presLimitsPilot =
    {
        .presLimit =
        {
            [Xlow] = PILOT_LOW_PRESS_LIMIT_SINGLE_RELAY, //3.1.3 SAR 6.1 6psi lower limit for AP-1
            [Xhi] = PILOT_HIGH_PRESS_LIMIT_AP_LCX, //SAR 6.2 19.715psi upper limit for AP-1
        },
    },
    .SupplyLossThreshold_Pilot = SUPPLY_LOSS_THRESHOLD_PILOT,
//    .SupplyLossThreshold_Current = SUPPLY_LOSS_THRESHOLD_CURRENT,
    .SupplyLossThreshold_Supply = -32768, //should be INT16_MIN, //this would turn off supply test
    .SingleActing = 1, //any non-zero
    .SensorMap =
    {
        [PRESSURE_ACT1_INDEX] = AD_NOT_INSTALLED,
        [PRESSURE_ACT2_INDEX] = AD_NOT_INSTALLED,
        [PRESSURE_SUPPLY_INDEX] = AD_NOT_INSTALLED,
        [PRESSURE_PILOT_INDEX] = AD_PRESS_PILOT,
        [PRESSURE_ATM_INDEX] = AD_PRESS_ATMOS,
    },
    .CheckWord = 0 //Don't care
};

//TFS:8333 #define GAIN_AP_1  6.821F
//TFS:8333 #define MAX_ZERO_AP_1  7.987F

//This is based on the pressure that is required to generate -100psi
#define PILOT_LOW_PRESS_LIMIT_DOUBLE_ACTING STD_FROM_PSI(4.0F)
//This is based on the pressure to reach maximum pressure
//TFS:8333 #define PILOT_HIGH_PRESS_LIMIT_DOUBLE_ACTING STD_FROM_PSI(30.0F)

#define PRESSURE_LIMIT_AP_2 100.0F

#define BOOST_OFFSET_HIGH_DA    32                         //!< beginning of DA flat spot
//#define BOOST_OFFSET_LOW_DA     (BOOST_OFFSET_HIGH_DA - 40)

/* Here are some defaults related to AP */
__root const PneumaticParams_t PneumaticParams_Default_LowCost =
{
    .boostCoeff =  //SA -1
    {
        .boost = BOOST_COEF(953),
        .BoostOffset = BOOST_OFFSET_HIGH_SA,
    },
    .presLimitsPilot =
    {
        .presLimit =
        {
            [Xlow] = PILOT_LOW_PRESS_LIMIT_SINGLE_RELAY, //3.1.3 SAR 6.1 6psi lower limit for AP-1
            [Xhi] = PILOT_HIGH_PRESS_LIMIT_AP_1, //SAR 6.2 19.715psi upper limit for AP-1
        },
    },
    .SupplyLossThreshold_Pilot = SUPPLY_LOSS_THRESHOLD_PILOT,
    .SupplyLossThreshold_Supply = -32768, //should be INT16_MIN, //this would turn off supply test
    .SingleActing = 1, //any non-zero
    .SensorMap =
    {
        [PRESSURE_ACT1_INDEX] = AD_NOT_INSTALLED,
        [PRESSURE_ACT2_INDEX] = AD_NOT_INSTALLED,
        [PRESSURE_SUPPLY_INDEX] = AD_NOT_INSTALLED,
        [PRESSURE_PILOT_INDEX] = AD_PRESS_PILOT,
        [PRESSURE_ATM_INDEX] = AD_NOT_INSTALLED,
    },
    .CheckWord = 0 //Don't care
};

__root const PneumaticParams_t PneumaticParams_Default_AP_SA = //single acting
{
    .boostCoeff =  // SA -2 through -5
    {
        .boost = BOOST_COEF(815),
        .BoostOffset = BOOST_OFFSET_HIGH_SA,
    },
    .presLimitsPilot =
    {
        .presLimit =
        {
            [Xlow] = PILOT_LOW_PRESS_LIMIT_SINGLE_RELAY, //AP 3.1.3 SAR 4.3 6psi lower limit for AP-2
            [Xhi] = PILOT_HIGH_PRESS_LIMIT_AP_2, //AP 3.1.3 SAR 4.4 22.28psi upper limit for AP-2
        },
    },
    .SupplyLossThreshold_Pilot = SUPPLY_LOSS_THRESHOLD_PILOT,
    .SupplyLossThreshold_Supply = STD_FROM_PSI(10.0F),
    .SingleActing = 1, //any non-zero
    .SensorMap =
    {
        [PRESSURE_ACT1_INDEX] = AD_PRESS_1,
        [PRESSURE_ACT2_INDEX] = AD_NOT_INSTALLED,
        [PRESSURE_SUPPLY_INDEX] = AD_PRESS_2,
        [PRESSURE_PILOT_INDEX] = AD_PRESS_PILOT,
        [PRESSURE_ATM_INDEX] = AD_PRESS_ATMOS,
    },
    .CheckWord = 0 //Don't care
};

__root const PneumaticParams_t PneumaticParams_Default_AP_DA12 = //double acting 12:1 relay
{
    .boostCoeff =
    {
        .boost = BOOST_COEF(950),
        .BoostOffset = BOOST_OFFSET_HIGH_DA,
    },
    .presLimitsPilot =
    {
        .presLimit =
        {
            [Xlow] = PILOT_LOW_PRESS_LIMIT_DOUBLE_ACTING, //AP 3.1.3 SAR 5.3 lower limit 4psi for double acting
            [Xhi] = PILOT_HIGH_PRESS_LIMIT_DOUBLE_ACTING, //AP 3.1.3 SAR 5.4 upper limit 30psi for double acting
        },
    },
    .SupplyLossThreshold_Pilot = SUPPLY_LOSS_THRESHOLD_PILOT,
    .SupplyLossThreshold_Supply = STD_FROM_PSI(15.0F), //AP 3.1.3 SAR 3.5 12:1 relay
    .SingleActing = 0,
    .SensorMap =
    {
        [PRESSURE_ACT1_INDEX] = AD_PRESS_2,
        [PRESSURE_ACT2_INDEX] = AD_PRESS_1,
        [PRESSURE_SUPPLY_INDEX] = AD_PRESS_3,
        [PRESSURE_PILOT_INDEX] = AD_PRESS_PILOT,
        [PRESSURE_ATM_INDEX] = AD_PRESS_ATMOS,
    },
    .CheckWord = 0 //Don't care
};

__root const PneumaticParams_t PneumaticParams_Default_AP_DA6 = //double acting 6:1 relay
{
    .boostCoeff =
    {
        .boost = BOOST_COEF(1624),
        .BoostOffset = BOOST_OFFSET_HIGH_DA,
    },
    .presLimitsPilot =
    {
        .presLimit =
        {
            [Xlow] = PILOT_LOW_PRESS_LIMIT_DOUBLE_ACTING, //AP 3.1.3 SAR 5.3 lower limit 4psi for double acting
            [Xhi] = PILOT_HIGH_PRESS_LIMIT_DOUBLE_ACTING, //AP 3.1.3 SAR 5.4 upper limit 30psi for double acting
        },
    },
    .SupplyLossThreshold_Pilot = SUPPLY_LOSS_THRESHOLD_PILOT,
    .SupplyLossThreshold_Supply = STD_FROM_PSI(20.0F), //AP 3.1.3 SAR 3.5 6:1 relay
    .SingleActing = 0,
    .SensorMap =
    {
        [PRESSURE_ACT1_INDEX] = AD_PRESS_2,
        [PRESSURE_ACT2_INDEX] = AD_PRESS_1,
        [PRESSURE_SUPPLY_INDEX] = AD_PRESS_3,
        [PRESSURE_PILOT_INDEX] = AD_PRESS_PILOT,
        [PRESSURE_ATM_INDEX] = AD_PRESS_ATMOS,
    },
    .CheckWord = 0 //Don't care
};

#endif //0 - Uncomment for other relays

/** \brief Standard accessor
*/
const void *TypeUnsafe_pneu_GetParams(void *dst)
{
    return STRUCT_TESTGET(&PneumaticParams, dst);
}

/** \brief Allowed values for pressures A/D channels
*/
static const s8 allowed_index[] =
{
    AD_PRESS_1,
    AD_PRESS_2,
    AD_PRESS_3,
    AD_PRESS_ATMOS,
    AD_PRESS_PILOT,
    AD_NOT_INSTALLED
};

/** \brief A standard "Set" accessor
*/
ErrorCode_t TypeUnsafe_pneu_SetParams(const void *src)
{
    const PneumaticParams_t *p = src;
    if(p == NULL)
    {
        p = &PneumaticParams_Default;
    }

    //Test some data here and return an error if wrong
    ErrorCode_t err;
    for(u8_least n = 0; n<NUMBER_OF_PRESSURE_SENSORS; n++)
    {
        err = ERR_INVALID_PARAMETER;
        for(u8_least i=0; i<NELEM(allowed_index); i++)
        {
            if(p->SensorMap[n] == allowed_index[i])
            {
                err = ERR_OK;
                break;
            }
        }
        if(err != ERR_OK)
        {
            break;
        }
    }
    if(p->boostCoeff.boost[Xlow] > p->boostCoeff.boost[Xhi])
    {
        err = ERR_INVALID_PARAMETER;
    }

    if(err == ERR_OK)
    {
        Struct_Copy(PneumaticParams_t, &PneumaticParams, p);

#ifdef OLD_NVRAM
        if(oswrap_IsOSRunning())
        {
            MN_FRAM_ACQUIRE();
                ram2nvram(&PneumaticParams, NVRAMID_PneumaticParams);
            MN_FRAM_RELEASE();
        }
#else
        err = ram2nvramAtomic(NVRAMID_PneumaticParams);
#endif //OLD_NVRAM
    }
    return err;
}

#ifdef OLD_NVRAM
void  pneu_InitData(InitType_t Type)
{
    PneumaticParams_t param;
    const PneumaticParams_t *p;
    if(Type == INIT_FROM_NVRAM)
    {
        nvram2ram(&param, NVRAMID_PneumaticParams);
        p = &param;
    }
    else //init from defaults
    {
        p = NULL;
    }
    ErrorCode_t err = pneu_SetParams(p);
    if(err != ERR_OK)
    {
        error_SetFault(FAULT_NVM_CHECKSUM0);
    }
}

void pneu_SaveData(void)
{
    MN_FRAM_ACQUIRE();
        ram2nvram(&PneumaticParams, NVRAMID_PneumaticParams);
    MN_FRAM_RELEASE();
}
#endif //OLD_NVRAM

/* This line marks the end of the source */
