/*
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file configure.c
    \brief Data owner for configuration data and handles get/save of data

    CPU: Any (with standard C compiler)

    OWNER: Schoonover
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/utilities/configure.c $
    $Date: 1/30/12 1:34p $
    $Revision: 114 $
    $Author: Arkkhasin $

    \ingroup configure
*/
/* $History: configure.c $
 *
 * *****************  Version 114  *****************
 * User: Arkkhasin    Date: 1/30/12    Time: 1:34p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/utilities
 * Dead code removal (no code change) FBO TFS:8782
 *
 * *****************  Version 113  *****************
 * User: Arkkhasin    Date: 1/20/12    Time: 10:58a
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/utilities
 * TFS:8748 - remove old pressure-related interfaces
 *
 * *****************  Version 112  *****************
 * User: Anatoly Podpaly Date: 12/30/11   Time: 1:40p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/utilities
 * TFS:8544 -- corrected few more instances.
 *
 * *****************  Version 111  *****************
 * User: Anatoly Podpaly Date: 12/30/11   Time: 1:34p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/utilities
 * TFS:8544 - corrected teh punction and removed unused parameter and
 * MN_ASSERT for it.
 *
 * *****************  Version 110  *****************
 * User: Arkkhasin    Date: 11/17/11   Time: 10:52a
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/utilities
 * TFS:8202 decouple I/O subsystem - removed air action conf (see
 * position.c)
 *
 * *****************  Version 109  *****************
 * User: Arkkhasin    Date: 11/15/11   Time: 4:26p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/utilities
 * Preliminary check-in for
 * TFS:8202 decouple I/O subsystem - pull things out
 *
 * *****************  Version 108  *****************
 * User: Arkkhasin    Date: 11/04/11   Time: 7:45p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/utilities
 * TFS:8072 NVMEM upgrade
 *
 * *****************  Version 106  *****************
 * User: Arkkhasin    Date: 5/06/11    Time: 12:17a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/utilities
 * TFS:6011 Per-master conf changed flag (stolen from AP HART 6)
 *
 * *****************  Version 105  *****************
 * User: Anatoly Podpaly Date: 1/20/11    Time: 10:42a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/utilities
 * TFS:5074 -- corrected Byte 1 of Options Config - removed DI option.
 *
 * *****************  Version 104  *****************
 * User: Anatoly Podpaly Date: 1/14/11    Time: 2:41p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/utilities
 * TFS:5074 - updated to disabled unused sensors.
 *
 * *****************  Version 103  *****************
 * User: Anatoly Podpaly Date: 1/14/11    Time: 11:40a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/utilities
 * TFS:5119 and TFS:5106 -- Set the default Option fo LOW COST and disable
 * POSITION RETRANSMIT option.
 *
 * *****************  Version 102  *****************
 * User: Sergey Kruss Date: 1/14/11    Time: 10:47a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/utilities
 * TFS:5323 - OS Adj overrun: added relevant entries.
 *
 * *****************  Version 101  *****************
 * User: Arkkhasin    Date: 1/04/11    Time: 6:28p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/utilities
 * TFS:5242 (Oops) and an unused header
 *
 * *****************  Version 100  *****************
 * User: Arkkhasin    Date: 12/18/10   Time: 1:26a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/utilities
 * TFS:4420 (out of bounds index) and related cleanup
 *
 * *****************  Version 99  *****************
 * User: Arkkhasin    Date: 12/17/10   Time: 2:50p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/utilities
 * TFS:5095,5096. Valve movement countermeasure corrected, due to 5097 in
 * a rather kludgy way but works. Air flag moved to position stop.
 * Configuration data became empty and is removed.
 *
 * *****************  Version 98  *****************
 * User: Anatoly Podpaly Date: 12/13/10   Time: 3:01p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/utilities
 * Bug 4955 - removed UI_Get_ATO() call as teh UI uses the standard
 * Get/Set ATO interface.
 *
 * *****************  Version 97  *****************
 * User: Sergey Kruss Date: 12/07/10   Time: 11:12a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/utilities
 * TFS:4352 rolled back per TFS:4960
 *
 * *****************  Version 96  *****************
 * User: Sergey Kruss Date: 12/03/10   Time: 11:43a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/utilities
 * TFS:4352 - Cmd252 doesn't doesn't report correct atmospheric pressure
 * (reports zero)
 *
 * *****************  Version 95  *****************
 * User: Sergey Kruss Date: 11/15/10   Time: 2:56p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/utilities
 * TFS:4713 - Clamped SP to -5% to +160%
 *
 * *****************  Version 94  *****************
 * User: Arkkhasin    Date: 10/22/10   Time: 2:51a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/utilities
 * TFS:4206 fix - Step 2: (a, also TFS:2514) checksums in rate limits
 * stuff which moved to ctllimits.c (b) removed /some/ compiled-out code
 * (c, FOB TFS:2616) rate limits can be reset to any setpoint
 *
 *
 * *****************  Version 93  *****************
 * User: Sergey Kruss Date: 6/10/10    Time: 3:11p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/utilities
 * Bug 3315:
 * Integer Open Stop Adjustment made a member of static structure of the
 * type ComputedPositionStop_t.
 * Changes made to scaling of PosComp and PIDerror in the control code.
 *
 * *****************  Version 92  *****************
 * User: Arkkhasin    Date: 3/30/10    Time: 6:40p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/utilities
 * Constrained the use of nvramtypes.h
 *
 * *****************  Version 91  *****************
 * User: Arkkhasin    Date: 3/28/10    Time: 7:28p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/utilities
 * Lint (unused macro)
 *
 * *****************  Version 90  *****************
 * User: Arkkhasin    Date: 3/27/10    Time: 7:19a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/utilities
 * Signal Characterization moved from ConfigurationData_t::ConfigFlags
 * (and ComputedConfigurationData_t) to CharactData_t;
 * Unused entries in ConfigurationData_t undefined (until we perhaps need
 * them again)
 * Unused entry in PressureScaleData_t removed
 * Pressure units always PSI (no local display)
 *
 * *****************  Version 89  *****************
 * User: Arkkhasin    Date: 3/25/10    Time: 1:15p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/utilities
 * Removed UI Language and SingleActing flag from configuration
 *
 * *****************  Version 88  *****************
 * User: Sergey Kruss Date: 3/08/10    Time: 12:28p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/utilities
 * Reverted back from the change of 3/04:
 * "Added second parameter to cnfg_SetOpenStopAdjustment() to
 * differentiate calls from  HART and UI."
 *
 * *****************  Version 87  *****************
 * User: Anatoly Podpaly Date: 3/04/10    Time: 5:41p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/utilities
 * Modified the nfg_CalculateComputedConfigurationData function: always
 * uses ATO flag from jumpers.
 *
 * *****************  Version 86  *****************
 * User: Sergey Kruss Date: 3/04/10    Time: 1:18p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/utilities
 * Added second parameter to cnfg_SetOpenStopAdjustment() to differentiate
 * calls from  HART and UI.
 *
 * *****************  Version 85  *****************
 * User: Arkkhasin    Date: 2/25/10    Time: 12:57a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/utilities
 * fTightShutoff,fLowerPositionLimit,fUpperPositionLimit and related
 * constants moved from ConfigurationData to CtlLimits
 * Similar with "Computed" type
 * Unused StrokingSpeed removed from ConfigurationData.
 *
 * *****************  Version 84  *****************
 * User: Anatoly Podpaly Date: 2/09/10    Time: 11:58a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/utilities
 * Resolved LINT issue.
 *
 * *****************  Version 83  *****************
 * User: Anatoly Podpaly Date: 2/01/10    Time: 5:06p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/utilities
 * Added new function: setbit. For LCX implementation.
 *
 * *****************  Version 81  *****************
 * User: Arkkhasin    Date: 1/28/10    Time: 7:12p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/utilities
 * File header standardized
 * Reset setpoint rate limits base is in a helper function and is placed
 * in correct places
 * Prototyped but compiled out a better version of
 * cnfg_AdjustSetpointForATOATC
*/

#include "mnwrap.h"
#include "mnassert.h"
#include "nvram.h"
#include "utility.h"
#include "configure.h"
#include "control.h"

/** member data */
static OptionConfig_t m_OptionConfig;

static const OptionConfig_t def_OptionConfig =
{
    //default is type 4, plus position retransmit plus switches plus pressure sensors
    //  plus single acting plus atm-pressure plus factory mode and di_option
    //-----------------------------------------------------------
    // Fixes for TFS 5106, 5119
    // Set the LOW Cost Option (Byte 0, bits 7..5 = 001 instead of 100 (0x20 vs. 0x80))
    // Set the Position Retransmit option to 0 (Byte 0 bit 4 (0x10) shall be 0)
    // .m_OptionConfig = {0x9F,0x3A,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    // TFS:5074 - removed Actuator Pressure Sensors, Pressure Supply
    // - removed DI option -- below is the default values
    //
    // Byte 0: 01 -- off; 02 -- off; 04 -- off; 08--  on (DO); 10 -- off; 20 -- on; 40 -- off; 80 -- off
    // --->28 (correct)
    // Byte 1: 01 -- off; 02 -- on; 04 -- off; 08--  on; 10 -- on as a check must be opposite of value when leaving
    //         20 -- off; 40 -- off; 80 -- off
    // -->1A (was 3A)

    // .m_OptionConfig = {0x28,0x1A,0,0,0,0,0,0,0,0,0,0,0,0,0,0},

    // 09/06/2012 -- Fix for TFS:11558 -- Correct the default option flags.
    // -- Byte 0:
    //      Added Actutator Pressures   (| 0x01 | 0x02)
    //      Added Supply Pressure       (| 0x04)
    //
    // Byte 0: 01 -- on; 02 -- on; 04 -- on; 08--  on (DO); 10 -- off; 20 -- on; 40 -- off; 80 -- off
    // --->2F (correct)
    //
    // -- Byte 1:
    //      Enable DI option (| 0x20)
    //      Atmospheric pressure ON (| 0x08) -- Was ON already. confirmed
    // Byte 1: 01 -- off; 02 -- on;  04 -- off; 08--  on; 10 -- on as a check must be opposite of value when leaving
    //         20 -- on;  40 -- off; 80 -- off
    // -->3A (was 1A)

    .m_OptionConfig = {0x2F,0x3A,0,0,0,0,0,0,0,0,0,0,0,0,0,0},

    //-----------------------------------------------------------
    CHECKFIELD_CLEAR()
};

/** private function prototypes */

static bool_t  cnfg_GetFlagFromOptionConfig(u8_least OptionConfigCode, const OptionConfig_t* pOptionConfig);
static u8_least cnfg_GetPointerOptionConfigDashVersion(const OptionConfig_t* pOptionConfig);

//see cnfghook.c

/** \brief returns the option config flags

  returns the option config flags
  \param[in] none
  \return u8* - address of the option config flag array
*/
const OptionConfig_t* cnfg_GetFillOptionConfig(OptionConfig_t *dst)
{
    return STRUCT_TESTGET(&m_OptionConfig, dst);
}

/** \brief sets the option config flags in FRAM

  \param[in] const OptionConfig_t* pOptionConfig - pointer to the option config structure
  \return none
*/
ErrorCode_t  cnfg_SetOptionConfig(const OptionConfig_t* pOptionConfig)
{

    if(pOptionConfig == NULL)
    {
        pOptionConfig = &def_OptionConfig;
    }

    Struct_Copy(OptionConfig_t, &m_OptionConfig, pOptionConfig);

#ifdef OLD_NVRAM
    /** get sole use of FRAM */
    MN_FRAM_ACQUIRE();

        /** write the data to fram and release fram */
        ram2nvram(&m_OptionConfig, NVRAMID_OptionConfig);
        MN_ENTER_CRITICAL();
        MN_EXIT_CRITICAL();

        cnfg_SetFRAMConfigurationChangedFlag(true);

    MN_FRAM_RELEASE();

    return ERR_OK;
#else
    return ram2nvramAtomic(NVRAMID_OptionConfig);
#endif //OLD_NVRAM
}


/** \brief Returns the value of a specific bit from the option config flags

  Returns the value of a specific bit from the option config flags
  \param[in] u8 OptionConfigCode - the bit number of the requested item
  \return bool_t - true if the bit is set
*/
bool_t  cnfg_GetOptionConfigFlag(u8_least OptionConfigCode)
{
    Struct_Test(OptionConfig_t, &m_OptionConfig);
    return cnfg_GetFlagFromOptionConfig(OptionConfigCode, &m_OptionConfig);
}

LINT_PURE(cnfg_GetFlagFromOptionConfig)
/** \brief Returns the value of a specific bit from the option config flags

  Returns the value of a specific bit from the option config flags
  \param[in] u8 OptionConfigCode - the bit number of the requested item
  \return bool_t - true if the bit is set
*/
static bool_t  cnfg_GetFlagFromOptionConfig(u8_least OptionConfigCode, const OptionConfig_t* pOptionConfig)
{
    MN_ASSERT(pOptionConfig != NULL);
    //AK: assume whoever got the pointer caused the test
    return util_GetBit(OptionConfigCode, pOptionConfig->m_OptionConfig, OPTION_CONFIG_BYTES);
}

/** \brief Returns the version type of the MNCB

  Returns the version type of the MNCB - 1 is low cost, 2 is without pressure, 3 is with
            pressure, 4 is advanced, 6 is ESD
  \return - the dash version number
*/
u8_least  cnfg_GetOptionConfigDashVersion(void)
{
    Struct_Test(OptionConfig_t, &m_OptionConfig);
    return cnfg_GetPointerOptionConfigDashVersion(&m_OptionConfig);
}

static u8_least cnfg_GetPointerOptionConfigDashVersion(const OptionConfig_t* pOptionConfig)
{
    u8_least Version;

    MN_ASSERT(pOptionConfig != NULL);

    //AK: assume whoever got the pointer caused the test
    Version = (u8_least)(pOptionConfig->m_OptionConfig[DASH_VERSION_BYTE]>>DASH_VERSION_SHIFT) & DASH_VERSION_MASK;

    //to help be backward compatible, if this is 0 (which is not valid) then set it to type 3
    //******** this should be removed in later versions
    if(Version == 0u)
    {
        Version = 3u;
    }

    return Version;
}






