/**
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file uijumpers.c
    \brief Implementation of MNCB LCD UI. Jumpers (Rotary Switch) section

    CPU: Any

    OWNER: AP
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/uisimple/uijumpers.c $
    $Date: 1/30/12 5:43p $
    $Revision: 13 $
    $Author: Arkkhasin $
*/

/*
    $History: uijumpers.c $
 * 
 * *****************  Version 13  *****************
 * User: Arkkhasin    Date: 1/30/12    Time: 5:43p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/uisimple
 * Restore LCX build FBO TFS:8782
 *
 * *****************  Version 12  *****************
 * User: Arkkhasin    Date: 1/05/12    Time: 3:01p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/uisimple
 * TFS:8577 - header refactoring
 *
 * *****************  Version 11  *****************
 * User: Arkkhasin    Date: 11/15/11   Time: 12:48p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/uisimple
 * TFS:8202 decouple I/O subsystem
 * (Changed position API)
 *
 * *****************  Version 10  *****************
 * User: Arkkhasin    Date: 11/04/11   Time: 7:09p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/uisimple
 * TFS:8072 NVMEM upgrade
 *
 * *****************  Version 8  *****************
 * User: Arkkhasin    Date: 12/17/10   Time: 2:54p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * TFS:5095 Air Action and Open Stop Adjustment saved atomically
 *
 * *****************  Version 7  *****************
 * User: Anatoly Podpaly Date: 12/13/10   Time: 3:03p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Bug 4955 - UI_Get_ATO() is static, used internally.
 *
 * *****************  Version 6  *****************
 * User: Anatoly Podpaly Date: 12/07/10   Time: 3:01p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Bug 4956 - added Assert when the NVRAM STore function fails.
 *
 * *****************  Version 5  *****************
 * User: Anatoly Podpaly Date: 12/06/10   Time: 5:02p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Bug 4956 - Use PID slot storage iterface (get/set) similar to ATO
 * storage.
 *
 * *****************  Version 4  *****************
 * User: Anatoly Podpaly Date: 11/29/10   Time: 6:02p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Bug 4871 - update the RS map.
 *
 * *****************  Version 3  *****************
 * User: Anatoly Podpaly Date: 11/12/10   Time: 3:36p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Bug 4652 - additional correction - cancel remotely set !normal mode
 * (and  process, if any) at the JS/RS change detectioin.
 *
 * *****************  Version 2  *****************
 * User: Anatoly Podpaly Date: 11/12/10   Time: 2:59p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Bug 4652 - fixed jumper code to accept the jumper seettings correctly.
 * SImplified the ATO/ATC handling and sync-up.
 *
 * Bug 4640 - now Jumper code will not switch to setup mode. The jumper
 * code will force Normal mode if it was in other mode - as prescribed by
 * requirements. ALso cancles the runnng process if it was launched
 * remotely.
 *
 * *****************  Version 1  *****************
 * User: Anatoly Podpaly Date: 10/20/10   Time: 10:48a
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/uisimple
 * Bug 4384 - extract jumpers code from uimain.

*/

//--------------------------------------------------------------------
//
// The code in this file contains the implementation of REQ 31-001
//
// The following LUI requirements are implemented in other modules and
// are not referenced in this file
//
// REQ 31-004 - Quadrature Encoder
// REQ 31-011 - LED blinking patterns
// REQ 31-030 - Quadrature Encoder
// REQ 31-031 - Quadrature Encoder
// REQ 31-032 - H/W Drivers
// REQ 31-033 - SSEL Pin (GPIO)
//
// Several requirements are referenced in this file and also in other files.
//--------------------------------------------------------------------

#include "config.h"
#include "mnwrap.h" //for MN_NOINIT
#include "oswrap.h"
#include "mnassert.h"
#include "mncbtypes.h" //for button lock business
#include "uipublic.h"
#include "errcodes.h"
#include "configure.h"
#include "process.h"
#include "devicemode.h"
#include "control.h"
#include "bitutils.h"
#include "bios.h"
#include "nvram.h"
#include "uiprivate.h"
#include "uisimple.h"
#include "uilock.h"
#include "selftune.h"
#include "dimensions.h"

//-------------------------------------------------------------------

CONST_ASSERT(JUMPER_PARAMETER_MASK < NUM_POSCTL_PARAMSETS);

/** REQ 31-026 */
// Jumper / Switch should be stable for this time
#define TIME_OF_JUMPER_STABILITY            (T5_000)

//-------------------------------------------------------------------
/** \brief LCX UI This variable is to support the LUI Jumper settings stored in NVRAM
*/

/** REQ 31-005 */
static LUI_Jumpers_t       m_LUI_Jumpers;

//-------------------------------------------------------------------

/** \brief LCX Jumper handling variables
*/
static  Simple_UI_Jumpers_t   Simple_UI_Jumpers;

/** \brief Table for remapping Physical PID slots to Logical for further use */
static const u8 Phys_to_Logical_PIDSlot[JUMPER_PARAMETER_MASK + 1] =
{
    0,          // Slot 0 -> 0 marked as 0 / 8 on the Fron Panel of the device
    1,          // Slot 1 -> 1  -- " --  1 / 9
    2,          // Slot 2 -> 2  -- " --  2 / A
    3,          // Slot 3 -> 3  -- " --  3 / B
    4,          // Slot 4 -> 4  -- " --  4 / C
    5,          // Slot 5 -> 5  -- " --  5 / D
    6,          // Slot 6 -> 6  -- " --  6 / E
    7           // Slot 7 -> 7  -- " --  7 / F
};

//-------------------------------------------------------------------
// LCX UI State Machine handling functions

/** \brief LCX Jumpers separate the Get ATO / ATC option from the bitset
  \return bool_t bATO - true if ATO, false if ATC
*/
static bool_t   LUI_GetATO_OutOfBits(u8 BitSet)
{
    bool_t  bATO;

    if ((BitSet & JUMPER_ATO_ATC_MASK) == JUMPER_SETTING_ATO)
    {
        bATO = true;
    }
    else
    {
        bATO = false;
    }

    return bATO;
}

/** \brief LCX Jumpers separate the Get ATO / ATC option from the bitset
  \return bool_t bATO - true if ATO, false if ATC
*/
static u8   LUI_GetPID_OutOfBits(u8 BitSet)
{
    u8      Parameter;

    BitSet &= JUMPER_PARAMETER_MASK;                // Separate the PID Slot Index (Phys)

    // Remap SLot ID to logical
    Parameter = Phys_to_Logical_PIDSlot[BitSet];    // Got the Logical Number

    // Clamp parameter to the Slot Array Dimension
    Parameter = MIN(Parameter, (NUM_POSCTL_PARAMSETS - 1));

    return Parameter;
}

/** \brief LCX Jumpers Get ATO / ATC option
  \return bool_t bATO - true if ATO, false if ATC
*/
/** REQ 31-024 */
static bool_t UI_Get_ATO(void)
{
    bool_t  bATO;

    bATO = LUI_GetATO_OutOfBits(Simple_UI_Jumpers.JumperSettings);
    return bATO;
}

/** \brief LCX Jumpers Get Parameter (returns ATO state #)
    As set in the Hardware
    \return u8 - PID Slot #
*/
bool_t  UI_Get_ImmediateATO(void)
{
    u8      Jumpers;
    bool_t  bATO;

    Jumpers = bios_ReadJumpers();
    bATO    = LUI_GetATO_OutOfBits(Jumpers);
    return bATO;
}

/** \brief LCX Jumpers Get Parameter (returns LOGICAL and CLAMPED PID Slot #)
    \return u8 - PID Slot #
*/
/** REQ 31-025 */
static u8 UI_Get_PIDIndex(void)
{
    u8      Parameter;

    Parameter = LUI_GetPID_OutOfBits(Simple_UI_Jumpers.JumperSettings);
    return Parameter;
}

/** \brief LCX Jumpers Get Parameter (returns LOGICAL and CLAMPED PID Slot #)
    As set in the Hardware
    \return u8 - PID Slot #
*/
u8 UI_Get_ImmediatePIDIndex(void)
{
    u8      Jumpers;
    u8      Parameter;

    Jumpers   = bios_ReadJumpers();
    Parameter = LUI_GetPID_OutOfBits(Jumpers);      // Get the parameter
    return Parameter;
}

/** \brief LCX Jumpers Store Jumpers in NVRAM option from internal structures/jumpers
*/
/** REQ 31-024; REQ 31-025 */
static void Store_Jumpers(void)
{
    // Test the structure
    Struct_Test(LUI_Jumpers_t, &m_LUI_Jumpers);

    if (m_LUI_Jumpers.JumpersBits != Simple_UI_Jumpers.JumperSettings)
    {   // The NVRAM is different from detected settings
        MN_ENTER_CRITICAL();
            m_LUI_Jumpers.JumpersBits = Simple_UI_Jumpers.JumperSettings;
            m_LUI_Jumpers.Reserved    = 0;
            STRUCT_CLOSE(LUI_Jumpers_t, &m_LUI_Jumpers);
        MN_EXIT_CRITICAL();

#ifdef OLD_NVRAM
        LUI_Save_Jumpers();
#else
        (void)ram2nvramAtomic(NVRAMID_LUI_Jumpers); //for now
#endif
    }
}

/** \brief LCX Jumpers
 *  Store ATO configurations in config data
*/
/** REQ 31-024; REQ 31-025 */
static void Store_ATO_Configuration(void)
{
    bool_t      LUI_bAto  = UI_Get_ATO();
    //const PositionStop_t  *pPositionStop = cnfg_GetPositionStop();
    PositionConf_t posconf;
    (void)pos_GetPositionConf(&posconf);

    //if (pPositionStop->bATO != LUI_bAto)
    if (posconf.bATO != LUI_bAto)
    {   // ATC / ATO Store required!

        ErrorCode_t                 err;

        /** TFS:4224 */ //check if air action ATO/ATC changed and reset OS Adjustment
        //function always returns ERR_OK, therefore OK to (void) the return

        // Set the change
        posconf.bATO = LUI_bAto;
        posconf.PositionRange[Xhi] = OPEN_STOP_ADJ_MAX_INT;

        // Store new config data
        err = pos_SetPositionConf(&posconf);
        MN_ASSERT(err == ERR_OK);

#if 0
        if (err != ERR_OK)
        {
            // error_SetFault(...); ???
        }
#endif

        // Now - cancel ALL processes (if running) and switch back to Normal mode.
        if (process_CheckProcess())
        {   // Something is running. Kill it.
            process_CancelProcess();
        }

        (void)LUI_SetNormalDeviceMode();
    }
}

/** \brief LCX Jumpers
 *  Store PID configurations in config data
*/
/** REQ 31-024; REQ 31-025 */
static void Store_PID_Configuration(void)
{
    const CtlSetSlot_t *ptrCtlSetSlot;
    CtlSetSlot_t        CtlSetSlot;
    ErrorCode_t         err;
    u8                  LUI_PIDSlot;

    ptrCtlSetSlot = posctl_GetCtlset(&CtlSetSlot);
    LUI_PIDSlot   = UI_Get_PIDIndex();

    if (ptrCtlSetSlot->slot != LUI_PIDSlot)
    {   // PID slot store required!

        // Set the slot
        CtlSetSlot.slot = LUI_PIDSlot;

        err = posctl_SetCtlset(&CtlSetSlot);
        MN_ASSERT(err == ERR_OK);

#if 0
        if (err != ERR_OK)
        {
           // error_SetFault(...); ???
        }
#endif

        // Now - cancel ALL processes (if running) and switch back to Normal mode.
        if (process_CheckProcess())
        {   // Something is running. Kill it.
            process_CancelProcess();
        }

        (void)LUI_SetNormalDeviceMode();
    }
}

/** \brief LCX UI Handle Jumper processing
*/
/** REQ 31-005 */
void Simple_UI_JumperHandler(void)
{
    Simple_UI_Jumpers_t   LocalJumpers;
    u8          JumpersFromBIOS;
    tick_t      CurrentTimerTicks;

    // Test the structure
    Struct_Test(Simple_UI_Jumpers_t, &Simple_UI_Jumpers);

    MN_ENTER_CRITICAL();
        // Copy "Global structure locally
        LocalJumpers = Simple_UI_Jumpers;
    MN_EXIT_CRITICAL();

    CurrentTimerTicks = bios_GetTimer0Ticker();

    if (!UI_IsLUILocked())
    {   // LUI Not Locked
        /** REQ 31-027 */
        JumpersFromBIOS = bios_ReadJumpers();
        if (LocalJumpers.NewJumperSettings != JumpersFromBIOS)
        {   // Change detected. Record the change and setup the T/O system
            LocalJumpers.NewJumperSettings = JumpersFromBIOS;
            LocalJumpers.ElapsedTime       = 0;                 // Reset the T/O
            LocalJumpers.TimeStart         = CurrentTimerTicks;

            if (!LocalJumpers.ChangeDetected)
            {   // First time change detected...
                LocalJumpers.ChangeDetected = true;

                if (Is_UI_StateMachineIdle())
                {   // State macine is idle - cancel ALL processes (if running)
                    // and switch back to Normal mode.
                    if (process_CheckProcess())
                    {   // Something is running. Kill it.
                        process_CancelProcess();
                    }
                    (void)LUI_SetNormalDeviceMode();
                }

                // Set request to set and hold idle the UI state machine
                Set_UI_StateMachineIdle();
            }
        }
    }
    else
    {   // LUI is locked
        /** REQ 31-045 */
        LocalJumpers.ChangeDetected = false;
    }

    if (LocalJumpers.ChangeDetected)
    {   // Change detected, check the state machine
        if (Is_UI_StateMachineIdle())
        {   // Now, if the change detected and the state machine is idle - then we check the T/O

            /** REQ 31-026 */
            LocalJumpers.ElapsedTime += CurrentTimerTicks - LocalJumpers.TimeStart;
            LocalJumpers.TimeStart    = CurrentTimerTicks;

            if (LocalJumpers.ElapsedTime > (tick_t)TIME_OF_JUMPER_STABILITY)
            {   // Jumpers are stable for given time!
                // Copy new settings into Active
                LocalJumpers.JumperSettings = LocalJumpers.NewJumperSettings;
                // Reset the change flag
                LocalJumpers.ChangeDetected = false;
                // Let the UI state machine go
                /** REQ 31-026; REQ 31-027 */
                Let_UI_StateMachineGo();
            }
        }
    }   // Change detected

    MN_ENTER_CRITICAL();
        // Copy "Global" structure back
        Simple_UI_Jumpers = LocalJumpers;

        // Checksum the structure
        STRUCT_CLOSE(Simple_UI_Jumpers_t, &Simple_UI_Jumpers);
    MN_EXIT_CRITICAL();

    // Periodic sync-up
    Store_Jumpers();
    Store_ATO_Configuration();
    Store_PID_Configuration();
}

//------------------------------------------------------------------
// NVRAM supporting functionality - for the Jumper settings

const void *TypeUnsafe_LUI_GetJumpers(void *dst)
{
    return STRUCT_TESTGET(&m_LUI_Jumpers, dst);
}

ErrorCode_t TypeUnsafe_LUI_SetJumpers(const void *src)
{
    LUI_Jumpers_t local;
    const LUI_Jumpers_t *p = src;
    if(p == NULL)
    {
        local.JumpersBits = bios_ReadJumpers();
        local.Reserved    = 0;
        p = &local;
    }
    MN_ENTER_CRITICAL();
        Struct_Copy(LUI_Jumpers_t, &m_LUI_Jumpers, p);
        if(!oswrap_IsOSRunning())
        {
            Simple_UI_Jumpers.JumperSettings    = m_LUI_Jumpers.JumpersBits;
            Simple_UI_Jumpers.NewJumperSettings = m_LUI_Jumpers.JumpersBits;
            Simple_UI_Jumpers.ChangeDetected    = false;
            Simple_UI_Jumpers.TimeStart         = 0;
            Simple_UI_Jumpers.ElapsedTime       = 0;
            STRUCT_CLOSE(Simple_UI_Jumpers_t, &Simple_UI_Jumpers);
        }
    MN_EXIT_CRITICAL();

    return ram2nvramAtomic(NVRAMID_LUI_Jumpers);
}



#ifdef OLD_NVRAM
/** \brief LUI Function - initializes the LUI parameters from NVRAM or to default
  \parameter[in] InitType_t Init type : from NVRAM or to default
*/

void LUI_Init_Jumpers(InitType_t Type)
{
	// This function is externally called to initialize tune data
	// If input Type== INIT_FROM_NVRAM, it copies the tune data from NVRAM to RAM
	// Otherwise it sets the data to default values
	// parameters description
	// InitType_t Type: input value to determine to copy or initialized tune data.
    MN_ASSERT(!oswrap_IsOSRunning()); //and no FRAM mutex

    if (Type == INIT_FROM_NVRAM)
    {   /** read fram to get the data */
        nvram2ram(&m_LUI_Jumpers, NVRAMID_LUI_Jumpers);
    }
    else
    {   //Type == INIT_TO_DEFAULT
        m_LUI_Jumpers.JumpersBits = bios_ReadJumpers();
        m_LUI_Jumpers.Reserved    = 0;
        STRUCT_CLOSE(LUI_Jumpers_t, &m_LUI_Jumpers);
    }

    // Init LCX Jumpers Structure from NVRAM
    Simple_UI_Jumpers.JumperSettings    = m_LUI_Jumpers.JumpersBits;
    Simple_UI_Jumpers.NewJumperSettings = m_LUI_Jumpers.JumpersBits;
    Simple_UI_Jumpers.ChangeDetected    = false;
    Simple_UI_Jumpers.TimeStart         = 0;
    Simple_UI_Jumpers.ElapsedTime       = 0;
    STRUCT_CLOSE(Simple_UI_Jumpers_t, &Simple_UI_Jumpers);
}

/** \brief LUI Function - saves the LUI parameters in NVRAM
*/

void LUI_Save_Jumpers(void)
{
    MN_FRAM_ACQUIRE();

        // write the data to fram and release fram
        ram2nvram(&m_LUI_Jumpers, NVRAMID_LUI_Jumpers);
        cnfg_SetFRAMConfigurationChangedFlag(true);

    MN_FRAM_RELEASE();
}
#endif //OLD_NVRAM
//------------------------------------------------------------------
/* This line marks the end of the source */
