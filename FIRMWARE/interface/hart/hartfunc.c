/*
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file hartfunc.c
    \brief The functions used by each HART command: the HART Functions Module

    CPU: Any

    OWNER: LS
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/hartfunc.c $
    $Date: 1/30/12 2:19p $
    $Revision: 218 $
    $Author: Arkkhasin $

    \ingroup HARTapp
*/

/*
    $History: hartfunc.c $
 *
 * *****************  Version 218  *****************
 * User: Arkkhasin    Date: 1/30/12    Time: 2:19p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/hart
 * Removed signal setpoint and loop signal data FBO TFS:8782
 * Removed control dependencies on loop signal and signal setpoint FBO
 * TFS:8782
 * Removed incompatible bumpless transfer FBO TFS:8782
 * Replaced old signal conversions (not in FFAP build) FBO TFS:8782
 *
 * *****************  Version 217  *****************
 * User: Arkkhasin    Date: 1/20/12    Time: 10:46a
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/hart
 * TFS:8748 - cmd 201
 *
 * *****************  Version 216  *****************
 * User: Arkkhasin    Date: 1/15/12    Time: 8:33p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/hart
 * Removed old command 185 FBO TFS:8680
 *
 * *****************  Version 215  *****************
 * User: Arkkhasin    Date: 1/12/12    Time: 12:39p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/hart
 * TFS:8674 - added AO interface
 *
 * *****************  Version 214  *****************
 * User: Arkkhasin    Date: 1/06/12    Time: 4:32p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/hart
 * TFS:8577 CHARACT_ prefix
 *
 * *****************  Version 213  *****************
 * User: Arkkhasin    Date: 12/05/11   Time: 4:19p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/hart
 * TFS:8347 - Restore AP build
 *
 * *****************  Version 212  *****************
 * User: Arkkhasin    Date: 11/29/11   Time: 2:12p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/hart
 * TFS:8313 Lint
 *
 * *****************  Version 211  *****************
 * User: Arkkhasin    Date: 11/21/11   Time: 12:14a
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/hart
 * TFS:8255 - A/D modularization
 *
 * *****************  Version 210  *****************
 * User: Arkkhasin    Date: 11/15/11   Time: 2:27p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/hart
 * Preliminary check-in for
 * TFS:8051 new tempcomp
 * TFS:8202 decouple I/O subsystem
 *
 * *****************  Version 209  *****************
 * User: Arkkhasin    Date: 11/04/11   Time: 4:59p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/hart
 * TFS:8072 NVMEM upgrade
 *
 * *****************  Version 207  *****************
 * User: Arkkhasin    Date: 5/19/11    Time: 2:25p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart
 * TFS:6010 Cmd 15 now says write protect none (251)
 *
 * *****************  Version 206  *****************
 * User: Arkkhasin    Date: 5/06/11    Time: 1:03a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart
 * TFS:6011 Per-master conf changed flag
 * TFS:6124 and hopefully TFS:6016 Changed reset strategy
 *
 * *****************  Version 205  *****************
 * User: Justin Shriver Date: 5/04/11    Time: 5:38p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart
 * TFS:6103
 *
 * *****************  Version 204  *****************
 * User: Anatoly Podpaly Date: 3/23/11    Time: 7:10p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart
 * TFS:5931 -- previous code did not work, corrected.
 *
 * *****************  Version 203  *****************
 * User: Anatoly Podpaly Date: 3/23/11    Time: 6:16p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart
 * TFS:5931 -- actual implementation of CMD 202 and modified 200.
 *
 * *****************  Version 202  *****************
 * User: Justin Shriver Date: 3/22/11    Time: 1:35p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart
 * TFS:5901
 *
 * *****************  Version 201  *****************
 * User: Sergey Kruss Date: 1/14/11    Time: 10:44a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart
 * TFS:5323 - OS Adj overrun. Removed non-XML HART commands definitions
 * for commands 167 - 169.
 * TFS:5225 - removed NORMAL_PRESSURE_INT=14.7 to return only guage atm
 * pressure.
 *
 * *****************  Version 200  *****************
 * User: Anatoly Podpaly Date: 12/17/10   Time: 6:28p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart
 * Bug 4957 - Added check for the factory mode to run Manufacturing Test
 * Mode.
 *
 * *****************  Version 199  *****************
 * User: Arkkhasin    Date: 12/03/10   Time: 2:28p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart
 * TFS:4951 - remove deprecate dfunction
 *
 * *****************  Version 198  *****************
 * User: Sergey Kruss Date: 12/03/10   Time: 12:03p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart
 * TFS:4352 - Cmd252 doesn't doesn't report correct atmospheric pressure
 * (reports zero).
 *
 * *****************  Version 197  *****************
 * User: Arkkhasin    Date: 12/02/10   Time: 2:23p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart
 * Extracted uiemul_led.c from hartfunc.c FOB TFS:4922
 *
 * *****************  Version 196  *****************
 * User: Arkkhasin    Date: 10/15/10   Time: 2:29p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart
 * Update set normal mode per wrapper API of bumpless transfer (TFS:4350)
 *
 * *****************  Version 195  *****************
 * User: Arkkhasin    Date: 10/12/10   Time: 3:23p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart
 * hart_Command_135_ChangeMode(): TFS:4013 invoke bumpless from setup,
 * too.
 *
 * *****************  Version 194  *****************
 * User: Arkkhasin    Date: 10/07/10   Time: 5:18p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart
 * HART switches separation FOB TFS:4036
 *
 * *****************  Version 193  *****************
 * User: Arkkhasin    Date: 10/06/10   Time: 10:07p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart
 * Partial:
 * TFS:4128 Dismantling io.{c,h}
 * TFS:4036 Fix to pressure bypass switch read
 *
 * *****************  Version 192  *****************
 * User: Sergey Kruss Date: 9/22/10    Time: 2:36p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart
 * TFS:4185
 * Modified cmd-141 to return PWMout instead of bias.
 *
 * *****************  Version 191  *****************
 * User: Sergey Kruss Date: 9/17/10    Time: 1:56p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart
 * Fixed bug TFS:3959. LINT note 960, MISRA Rule 11.5
 *
 * *****************  Version 190  *****************
 * User: Sergey Kruss Date: 9/13/10    Time: 10:41a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart
 * Added "Press5" to the comments header of command 201
 *
 * *****************  Version 189  *****************
 * User: Arkkhasin    Date: 9/03/10    Time: 3:41p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart
 * First step in TFS 4067: extracted commands 129, 130
 *
 * *****************  Version 188  *****************
 * User: Sergey Kruss Date: 9/02/10    Time: 4:33p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart
 * TFS:3704
 * TFS:3995
 * Modified command 201 to show atmospheric pressure and to show correct
 * pilot pressure.
 *
 * *****************  Version 187  *****************
 * User: Anatoly Podpaly Date: 8/31/10    Time: 4:48p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart
 * CHanged file name - uihw_test.h
 *
 * *****************  Version 186  *****************
 * User: Anatoly Podpaly Date: 8/31/10    Time: 2:56p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart
 * CHanged name of the include file - digital_pot.h.
 *
 * *****************  Version 185  *****************
 * User: Anatoly Podpaly Date: 8/27/10    Time: 6:05p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart
 * Moved commands to control WIperLock to WRITE cmd (130), originally
 * incorrectly paced them at 129.
 *
 * *****************  Version 184  *****************
 * User: Anatoly Podpaly Date: 8/27/10    Time: 5:43p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart
 * Added DEBUG functions for WiperLock control and Status Reads.
 *
 * *****************  Version 183  *****************
 * User: Anatoly Podpaly Date: 8/27/10    Time: 4:34p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart
 * Added commands to access the Digital Resistor.
 *
 * *****************  Version 182  *****************
 * User: Arkkhasin    Date: 8/19/10    Time: 12:06a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart
 * Fix for 3640, 3932
 * Standard tombstone header
 *
 * *****************  Version 181  *****************
 * User: Anatoly Podpaly Date: 7/30/10    Time: 1:50p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart
 * Bug 3674 - ADded function : Raw Read of the Serialization Chip.
 *
 * *****************  Version 180  *****************
 * User: Anatoly Podpaly Date: 7/29/10    Time: 10:18a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart
 * Bug 3627 - Updated LED patters and the LED control code. Commands
 * 129,161 and 240 were modified to reflect the mods.
 *
 * *****************  Version 179  *****************
 * User: Sergey Kruss Date: 6/25/10    Time: 1:58p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart
 * Implemented command 242 with XML and moved from hartfunc.c to glue
 * layer.
 *
 * *****************  Version 178  *****************
 * User: Arkkhasin    Date: 5/14/10    Time: 6:56p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart
 * Update for new process API
 * Removed old no-longer working command to set UI rate and LCD contrast
 *
 * *****************  Version 177  *****************
 * User: Anatoly Podpaly Date: 5/14/10    Time: 2:06p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart
 * Bug #3156: Implemented Test Mode. The command 190 was expanded to
 * accvomodate for the Test mode execution.
 *
 * *****************  Version 176  *****************
 * User: Sergey Kruss Date: 5/01/10    Time: 3:20p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart
 * AP added HART command 242 and removed internal diagnostic
 * HART cmd 129 for types:
 * TYPE_FACTORY_DEBUG_CONTROL1 -- TYPE_FACTORY_DEBUG_CONTROL5
 *
 * *****************  Version 175  *****************
 * User: Anatoly Podpaly Date: 4/05/10    Time: 3:18p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart
 * Bug #2940 - Cmd 240 used HC129_DATA - corrected, use HC240_DATA. The
 * functionality was not effected, HC***_DATA defined to 0.
 *
 * *****************  Version 174  *****************
 * User: Anatoly Podpaly Date: 4/01/10    Time: 5:46p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart
 * Bug #2930 - Enable UI emulation. Added Emulation control HART command:
 * command 240.
 *
 * *****************  Version 173  *****************
 * User: Arkkhasin    Date: 3/30/10    Time: 6:40p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart
 * Constrained the use of nvramtypes.h
 *
 * *****************  Version 172  *****************
 * User: Arkkhasin    Date: 3/30/10    Time: 5:19p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart
 * Bumpless is back but now standalone
 * Wired to setting normal mode from HART
 *
 * *****************  Version 171  *****************
 * User: Arkkhasin    Date: 3/28/10    Time: 7:32p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart
 * Removed commands 165, 210
 *
 * *****************  Version 170  *****************
 * User: Arkkhasin    Date: 3/27/10    Time: 6:47a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart
 * Signal Characterization moved from ConfigurationData_t::ConfigFlags
 * (and ComputedConfigurationData_t) to CharactData_t;
 * convert_ConvertSignalToSetpoint() no longer takes Characterization as a
 * parameter
 * Pressure units always PSI (no local display)
 * Command 172 write custom characterization moved to hart_sigsp.c
 * Command 211 read custom characterization moved to hart_sigsp.c
 * Commands 165 and 210 read/write configuration further trimmed down
 *
 * *****************  Version 169  *****************
 * User: Arkkhasin    Date: 3/24/10    Time: 2:10p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart
 * Removed command 45 (no AO and wrong number anyway, see ESD)
 * Removed command 46 (no AO and wrong number anyway, see ESD)
 * Main pressure in command 141 is always 0 (no sensor)
 * Removed non-existent or moved data from commands 165, 210
 * Removed command 207 (no AO)
 *
 * *****************  Version 168  *****************
 * User: Arkkhasin    Date: 3/12/10    Time: 3:39p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart
 * Convert 48 and 136 to xml format
 *
 * *****************  Version 167  *****************
 * User: Arkkhasin    Date: 3/09/10    Time: 12:57a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart
 * Adjusted for new fault storage from the trunk. Commands 48 and 136 and
 * 130/139 need to be extracted and need some work.
 *
 * *****************  Version 166  *****************
 * User: Sergey Kruss Date: 3/08/10    Time: 12:35p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart
 * Reverted back from the change of 3/04:
 * "Added second parameter to cnfg_SetOpenStopAdjustment() to
 * differentiate calls from  HART and UI."
 *
 * *****************  Version 165  *****************
 * User: Anatoly Podpaly Date: 3/05/10    Time: 10:04a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart
 * Command 129: Read LED state: by request added
 * - Jumpers from BIOS;
 * - PB Statuses from BIOS.
 *
 * *****************  Version 164  *****************
 * User: Sergey Kruss Date: 3/04/10    Time: 1:16p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart
 * Added second parameter to cnfg_SetOpenStopAdjustment() to differentiate
 * calls from  HART and UI.
 *
 * *****************  Version 163  *****************
 * User: Arkkhasin    Date: 2/24/10    Time: 6:45p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart
 * fTightShutoff,fLowerPositionLimit,fUpperPositionLimit moved from
 * ConfigurationData to CtlLimits
 * Unused StrokingSpeed removed from ConfigurationData.
 * Setpoint rate limits moved from PIDData to CtlLimits
 * No longer in commands 165 or 210 which should go away soon
 * BumplessSpeed set/get removed from HART commands (and going away soon
 * from ConfigurationData)
 * Commands to Read/Write PID parameters moved from hartfunc.c to
 * hart_posctl.c and acquired PID set selector
 * Autotune command re-implemented in XML and moved from hartfunc.c to
 * hart_posctl.c
 *
 * *****************  Version 162  *****************
 * User: Arkkhasin    Date: 2/09/10    Time: 2:36p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart
 * 1. Removed stubs of unimplemented functions
 * 2. Adapted mode test for XML-generated flags (now more robust)
 *
 * *****************  Version 161  *****************
 * User: Anatoly Podpaly Date: 2/02/10    Time: 10:50a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart
 * LCX - added comment re- Jumper switch in the HART command to be off by
 * 1.
 *
 * *****************  Version 160  *****************
 * User: Anatoly Podpaly Date: 2/01/10    Time: 6:22p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart
 * Moved mnwrap.h on top of the include list.
 *
 * *****************  Version 159  *****************
 * User: Anatoly Podpaly Date: 2/01/10    Time: 4:37p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart
 * LCX: initial implementation : emulation commands.

*/

#define NEED_NVMEM_END //expose hidden stuff
#include "mnwrap.h"
#include MEMMAP_H_

#include "projectdef.h"
//#include "pressmon.h"
#include "oswrap.h"
#include "mnassert.h"
#include "nvram.h"
#include "utility.h"
#include "devicemode.h"
#include "faultdef.h"
#include "faultpublic.h"
#include "configure.h"
#include "diagnostics.h"
#include "bufferhandler.h"
#include "selftune.h"
#include "control.h"
#include "cycle.h"
#include "process.h"
//#include "reset.h"
#include "hartfunc.h"
#include "hartcmd.h"
#include "hart.h"
#include "tempcomp.h"
#include "smoothing.h"
#include "mncbdefs.h"
#include "sysio.h"

#include "hartdef.h"
#include "hartpriv.h"
#include "wprotect.h"

#include "poscharact.h"
#if FEATURE_BUMPLESS_XFER == FEATURE_BUMPLESS_XFER_ENABLED
#include "bumpless.h"
#endif
#include "ctllimits.h"

#if FEATURE_SIGNAL_SETPOINT == FEATURE_SIGNAL_SETPOINT_SUPPORTED
#include "signalsp.h"
#endif

#if FEATURE_LOCAL_UI == FEATURE_LOCAL_UI_SIMPLE
#include "uihw_test.h"
#endif

#include "dohwif.h" //for CMD 200 rework

#include "fpconvert.h"

#include "ipcurr.h"

#if FEATURE_LOOPSIGNAL_SENSOR == FEATURE_LOOPSIGNAL_SENSOR_SUPPORTED
#include "insignal.h"
#endif

#if FEATURE_AO == FEATURE_AO_SUPPORTED
#include "ao.h"
#endif

//#include "hartapputils.h"
#include "ramdatamap.h"

//private defines
#define MAX_POLLING_ADDRESS 15u

#if 0
//Type (0=signal, 1=position, 2=full closed, 3=full open, 4=fail position),
#if FEATURE_SIGNAL_SETPOINT == FEATURE_SIGNAL_SETPOINT_SUPPORTED
    #define TYPE_SIGNAL 0u
#endif
#define TYPE_POSITION 1u
#define TYPE_FULL_CLOSED 2u
#define TYPE_FULL_OPEN 3u
#define TYPE_FAIL_POSITION 4u
#define TYPE_IP_DIAGNOSTIC 12u
#endif //0

#define NON_LINEAR_TRANSFER 4u
#define LINEAR_TRANSFER 0

#define NOT_USED 250u

//#define HART_HCF_NONE 251u //defined by HCF
#define HART_HCF_NOTWP 0u //defined by HCF
#define HART_HCF_WP    1u //defined by HCF

//private function

bool_t hart_SetProcessCommand(ProcId_t procId)
{
    bool_t ret = (process_SetProcessCommand(procId) != ERR_OK);
    return ret;
}


//--------------- App layer functions ---------------------
/*lint -esym(613,src,dst) pointers for the app layer come from mnhartcmd.c
                          and are guaranteed non-NULL.
*/

//Configuration commands wrappers
s8_least hartcmd_ReadSettings(const u8 *src, u8 *dst)
{
    UNUSED_OK(src);
    UNUSED_OK(dst);
    return HART_OK;
}
s8_least hartcmd_WriteSettings(const u8 *src, u8 *dst)
{
    UNUSED_OK(src);
    UNUSED_OK(dst);
    return HART_OK;
}


/**
\brief Returns basic HART addressing and command information

Notes:
This command is allowed in all modes, even when write busy or process busy is set
*/
s8_least hart_Command_0_ReadUniqueIdentifier(const u8 *src, u8 *dst)
{
//"0:Ask Address,<Dummy,<MfgID,<DeviceType,<Preambles,<CmdRev,<TXRev,<SWRev,<HWRev,<Flags,<DeviceID;"

    const HartData_t * tmpptr;

    dst[HC0_DUMMY] = 254;
    dst[HC0_MFGID] = MNCB_MANUFACTURER_ID;
    dst[HC0_DEVICE_TYPE] = MNCB_MANUFACTURER_DEVICE_CODE;
    dst[HC0_PREAMBLES] = NBR_REQUEST_PREAM;
    dst[HC0_CMDREV] = UNIV_CMD_REV;
    dst[HC0_TXREV] = TXSPEC_REV;
    dst[HC0_SWREV] = SW_REV;
    dst[HC0_HWREV] = MNCB_HARDWARE_REVISION;
    dst[HC0_FLAG] = FLAGS;

    tmpptr = hart_GetHartData();
    util_PutU8Array(&dst[HC0_DEVICE_ID], 3u, tmpptr->device_id);

    UNUSED_OK(src);

    return HART_OK;
}

/**
\brief Returns position

Notes:
This command is allowed in all modes, even when write busy or process busy is set
*/
s8_least hart_Command_1_ReadPrimaryVariable(const u8 *src, u8 *dst)
{
    //"1:Read Primary Var,<PosUnits,<Pos;"
    pos_t Position;
/*
    Convert position to floating point
    Add data to send buffer (see software specifications for details of data to add).
    Hart_send()
*/
    //use smoothed position
    //Position = control_GetPosition();
    //Position = smooth_GetSmoothedData(SELECTION_POSITION);
    Position = (pos_t)vpos_GetSmoothedScaledPosition();

    u8 units = fpconvert_IntToFloatBuffer(Position, UNITSID_POSITION, &dst[HC1_POS]);

    util_PutU8(&dst[HC1_POS_UNITS], units);

    UNUSED_OK(src);

    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}


#if FEATURE_LOOPSIGNAL_SENSOR == FEATURE_LOOPSIGNAL_SENSOR_SUPPORTED
//deprecated
/**
\brief Returns the input current

Notes:
This command is allowed in all modes, even when write busy or process busy is set
*/
s8_least hart_Command_2_ReadCurrent(const u8 *src, u8 *dst)
{
    //"2:Read Current,<Sig,<SigPercent;"
    s16 RawSignal;
    percent_least_t ScaledSignal;

/*    Convert signal to floating point
    Add data to send buffer (see software specifications for details of data to add).
*/

    //use smoothed signal
    //get and convert signal
    RawSignal = sig_GetSmoothedSignal();
    (void)fpconvert_IntToFloatBuffer(RawSignal, UNITSID_SIGNAL_ENTRY, &dst[HC2_SIGNAL]);
    //note:  we scale the signal as if it were ATO always - this hart parameter
    //      doesn't depend on ATO or ATC
    ScaledSignal = sig_ConvertSignalToStd(RawSignal);
    (void)fpconvert_IntToFloatBuffer(ScaledSignal, UNITSID_PERCENT_ENTRY, &dst[HC2_SIGNAL_PERCENT]);

    UNUSED_OK(src);

    return HART_OK;
}
#endif

/**
\brief Sets the polling address of the device

Notes:
This command is allowed in all modes
*/
s8_least hart_Command_6_WritePollingAddress(const u8 *src, u8 *dst)
{
//MESCO
    const HartData_t * tmpptr;
    HartData_t tmp;
    ErrorCode_t retval;
    u8_least PollingAddress;
    s8_least ret;

    PollingAddress = src[HC6_POLLING_ADDRESS];
    if ((PollingAddress > MAX_POLLING_ADDRESS))
    {
        ret = HART_INVALID_SELECTION;
    }
    else
    {
        tmpptr = hart_GetHartData();
        tmp = *tmpptr;
        tmp.transmit_address = (u8)PollingAddress;

        retval = hart_SetHartData(&tmp);

        if (ERR_OK == retval)
        {
            //AK: pre LS, it duplicates what will be done later,PUTU8( HC6_POLLING_ADDRESS, tmpptr->transmit_address);
            ret = HART_OK;
        }
        else // FRAM write error
        {
            ret = TRANSMITTER_SPECIFIC_COMMAND_ERROR;
        }
    }

    UNUSED_OK(dst);

    return ret;
}

/**
\brief To be implemented by Mesco
*/
s8_least hart_Command_11_ReadUniqueIdentifierByTag(const u8 *src, u8 *dst)
{
    u32 i;
    s8_least ret;
    const HartData_t * tmpptr;

    tmpptr = hart_GetHartData();

    i = 0;
    while ((i < HC11_REQ_LENGTH) && (src[i] == tmpptr->tag[i]))
    {
        i++;
    }

    if (i >= HC11_REQ_LENGTH)
    {
       ret = hart_Command_0_ReadUniqueIdentifier(src, dst);
    }
    else
    {
        Hart_DropFrame();
        ret = HART_OK;
    }

    return ret;
}

/**
\brief Returns the Message field in the device

Notes:
This command is allowed in all modes, even when write busy or process busy is set
*/
s8_least hart_Command_12_ReadMessage(const u8 *src, u8 *dst)
{
//MESCO
//NOTE:  I have assumed that the ascii data is not translated from 6bit to 8 bit
//      and therefore we should just use get/put Array instead of get/put ascii

    const HartData_t * tmpptr;

    tmpptr = hart_GetHartData();

    util_PutU8Array(&dst[HC12_MESSAGE], HC12_LENGTH, (const u8 *)tmpptr->message);

    UNUSED_OK(src);
    return HART_OK;
}

/**
\brief Returns the tag, descriptor, and date fields in the device

Notes:
This command is allowed in all modes, even when write busy or process busy is set
*/
s8_least hart_Command_13_ReadTagAndDescriptor(const u8 *src, u8 *dst)
{
//MESCO
//NOTE:  I have assumed that the ascii data is not translated from 6bit to 8 bit
//      and therefore we should just use get/put Array instead of get/put ascii
    const HartData_t * tmpptr;

    tmpptr = hart_GetHartData();

    util_PutU8Array( &dst[HC13_TAG], HC13_TAG_LENGTH, (const u8 *)tmpptr->tag);
    util_PutU8Array( &dst[HC13_DESCRIPTOR], HC13_DESCRIPTOR_LENGTH, (const u8 *)tmpptr->descriptor);
    util_PutU8Array( &dst[HC13_DATE], HC13_DATE_LENGTH, (const u8 *)tmpptr->date);

    UNUSED_OK(src);
    return HART_OK;
}

/**
\brief Returns the transducer information in the device.

  This data is not used by the SVI but the command is provided here
  for compatibility with some DCS systems.

Notes:
  This command is allowed in all modes, even when write busy or process busy is set

To be implemented by Mesco
*/
s8_least hart_Command_14_ReadTransducerInfo(const u8 *src, u8 *dst)
{
    //"14:Read Transducer Info,<TSerial,<TUnits,<TULim,<TLLim,<TSpan;"
    //This command reports constants and is only implemented to statisfy some DCS systems

    util_PutU24(&dst[HC14_SERIAL], 0u);
    util_PutU8(&dst[HC14_UNITS], fpconvert_GetUnits(UNITSID_POSITION));
    util_PutFloat(&dst[HC14_ULIM], 180.0F); //AK: Need symbolic names already used internally!
    util_PutFloat(&dst[HC14_LLIM], -5.0F);
    util_PutFloat(&dst[HC14_SPAN], 100.0F);

    UNUSED_OK(src);
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

/**
\brief Returns the transmitter information in the device.

  This data is not used by the SVI but the command is provided here for compatibility with some DCS systems.
To be implemented by Mesco

Notes:
This command is allowed in all modes, even when write busy or process busy is set
*/
s8_least hart_Command_15_ReadTransmitterInfo(const u8 *src, u8 *dst)
{
    //"15:Read PV Output Info, <AlarmCode, <TransferFunc, <Units, <URV, <LRV, <Damping15, <WriteProtect, <PrivLabel;"
    //This command reports constants and is only implemented to statisfy some DCS systems
    util_PutU8(&dst[HC15_ALARM], (u8)NOT_USED);

    if(poscharact_GetCharacterizationSel(NULL)->CharactSel == CHARACT_LINEAR)
    {
        util_PutU8(&dst[HC15_TXFR_FCN], (u8)LINEAR_TRANSFER);
    }
    else
    {
        util_PutU8(&dst[HC15_TXFR_FCN], (u8)NON_LINEAR_TRANSFER);
    }

    util_PutU8(&dst[HC15_UNITS], fpconvert_GetUnits(UNITSID_POSITION));
    util_PutFloat(&dst[HC15_URV], 100.0F); //AK: Symbolics?
    util_PutFloat(&dst[HC15_LRV], 0.0F);
    util_PutFloat(&dst[HC15_DAMPING], 0.0F);

    if (bios_ReadWriteProtectInput())
    {
        util_PutU8(&dst[HC15_WRITE_PROT], HART_HCF_WP);
    }
    else
    {
        util_PutU8(&dst[HC15_WRITE_PROT], HART_HCF_NOTWP);
    }

    util_PutU8(&dst[HC15_PRIVATE_LABEL], MNCB_MANUFACTURER_ID);

    UNUSED_OK(src);
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

/**
\brief Returns the final assembly number

Notes:
This command is allowed in all modes, even when write busy or process busy is set
*/
s8_least hart_Command_16_ReadFinalAssemblyNumber(const u8 *src, u8 *dst)
{
//MESCO
//NOTE:  Mesco seems to have defined final assy number as a byte array - I leave it like that
    const HartData_t * tmpptr;

    tmpptr = hart_GetHartData();

    util_PutU8Array(&dst[HC16_FINAL_ASSY], HC16_LENGTH, (const u8 *)tmpptr->finalass_nbr);

    UNUSED_OK(src);
    return HART_OK;
}


//AK: A common wrapper for HART data stuff
static s8_least hart_WriteHartData(const HartData_t *hdata)
{
    s8_least ret;
    ErrorCode_t retval;
    retval = hart_SetHartData(hdata);
    if (ERR_OK == retval)
    {
        ret = HART_OK;
    }
    else
    {
        ret = TRANSMITTER_SPECIFIC_COMMAND_ERROR;
    }

    return ret;
}


/**
\brief Sets the Message field of the device

Notes:
This command is allowed in all modes
*/
s8_least hart_Command_17_WriteMessage(const u8 *src, u8 *dst)
{
//MESCO

    const HartData_t * tmpptr;
    HartData_t tmp;

    tmpptr = hart_GetHartData();
    tmp = *tmpptr;

    util_GetU8Array(&src[HC17_MESSAGE], HC17_LENGTH, tmp.message);
    UNUSED_OK(dst);
    return hart_WriteHartData(&tmp);
}

/**
\brief Sets the tag, descriptor, and date fields of the device

Notes:
This command is allowed in all modes
*/
s8_least hart_Command_18_WriteTagAndDescriptor(const u8 *src, u8 *dst)
{
//MESCO
    const HartData_t * tmpptr;
    HartData_t tmp;

    tmpptr = hart_GetHartData();
    tmp = *tmpptr;

    util_GetU8Array( &src[HC13_TAG], HC13_TAG_LENGTH, tmp.tag);
    util_GetU8Array( &src[HC13_DESCRIPTOR], HC13_DESCRIPTOR_LENGTH, tmp.descriptor);
    util_GetU8Array( &src[HC13_DATE], HC13_DATE_LENGTH, tmp.date);

    UNUSED_OK(dst);
    return hart_WriteHartData(&tmp);
}

/**
\brief Sets the final assembly number field of the device

Notes:
This command is allowed in all modes
*/
s8_least hart_Command_19_WriteFinalAssemblyNumber(const u8 *src, u8 *dst)
{
//MESCO
    const HartData_t * tmpptr;
    HartData_t tmp;

    tmpptr = hart_GetHartData();
    tmp = *tmpptr;

    util_GetU8Array( &src[HC19_FINAL_ASSY], HC19_LENGTH, tmp.finalass_nbr);

    UNUSED_OK(dst);
    return hart_WriteHartData(&tmp);
}


/**
\brief Clears the configuration changed bit

Notes:
This command is allowed in all modes
*/
s8_least hart_Command_38_ResetConfigurationChangedFlag(const u8 *src, u8 *dst)
{
    //handled by hart_makeResponseFlags() akt_response_code_2 &= ~HART_CONFIG_CHANGED; //AK:TODO
    (void)nvram_ResetConfigurationChangedFlag(hart_MsgFromSecondaryMaster(src)); //can't do anything useful with return code yet
    UNUSED_OK(dst);
    UNUSED_OK(src);
    return HART_OK;
}


/**
\brief Performs a master reset

Notes:
This command is allowed in all modes
*/
s8_least hartcmd_Reset(const u8 *src, u8 *dst)
{
    //call reset with a delay that will let the hart response happen
    hart_ResetDeviceOnSend();
    UNUSED_OK(src);
#if 0 && __CORE__ == __ARM7M__ //Hack: Meaning: no HART modem, normally no dribble character
    u8 *start = dst - (MN_OFFSETOF(long_xmt_hdr_t, response) - MN_OFFSETOF(long_xmt_hdr_t, delimiter));
    u8 ck = 0;
    do
    {
        ck ^= *start++;
    } while(start!=dst);
    *dst = ck;
    dst[1] = 0; //add dribble character
    return -1; //One extra byte
#else
    UNUSED_OK(dst);
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
#endif
}

#ifdef OLD_NVRAM
static const writeGap_t wrgap[] =
{
    /* The gaps must be in ascending order */
/* Lint, quite unfortunately, screws up thoroughly on C99 designated
   initializers being array elements. Suppressing 940 and 26 then
   produces 155. So, we hide the whole construct from it. Thanks, Gimpel.
*/
# ifndef _lint
#   if NVRAMID_TemperatureCalibrationShifts < NVRAMID_TemperatureCalibrationCol
#       define CALSHIFTS_POS 0
#   else
#       define CALSHIFTS_POS 1
#   endif
# endif //_lint
# ifndef _lint
    [CALSHIFTS_POS] =
# endif //_lint
    {
        NVRAMID_TemperatureCalibrationShifts,
        NVRAMID_TemperatureCalibrationShifts
    },
# ifndef _lint
    [1-CALSHIFTS_POS] =
# endif //_lint
    {
        NVRAMID_TemperatureCalibrationCol,
        NVRAMID_TemperatureCalibrationCol+(NUMBER_OF_CALIB_POINTS-1U)
    }
};

/** \brief Resets MNCB after ensuring the FRAM is initialized
    and that the HART response went out
    IMPORTANT: This can only be called from the context of the
    process task!!!
    \param flag - true iff cal data are to be reinitialized
*/
void hart_procRamCommit(bool_t flag)
{
    u32 numgaps;
    if(flag)
    {
        //Save all
        numgaps = 0U;
    }
    else
    {
        //Save all except calibrations
        numgaps = NELEM(wrgap);
    }
    ram_Commit(numgaps, wrgap);
    reset_HardwareReset();
}
#endif //OLD_NVRAM

/**
\brief Sets the command to be used by burst mode
To be implemented by Mesco.

Notes:
This command is allowed in all modes
*/
s8_least hart_Command_108_WriteBurstModeCommand(const u8 *src, u8 *dst)
{
    const HartData_t * tmpptr;
    HartData_t tmp;
    u8 burstcmd;
    s8_least ret;

    burstcmd = util_GetU8(&src[HC108_CMD]);

    if ((0 == burstcmd) || (burstcmd > 3))
    {
        ret = HART_INVALID_SELECTION;
    }
    else
    {
        tmpptr = hart_GetHartData();
        tmp = *tmpptr;

        tmp.burst_mode_cmd = burstcmd;

        ret = hart_WriteHartData(&tmp);
    }
    UNUSED_OK(dst);
    return ret;
}

/**
\brief Turns burst mode on or off
To be implemented by Mesco.

Notes:
This command is allowed in all modes
*/
s8_least hart_Command_109_WriteBurstModeControl(const u8 *src, u8 *dst)
{
    const HartData_t * tmpptr;
    HartData_t tmp;
    s8_least ret;

    u8 burst_sel;

    burst_sel = util_GetU8(&src[HC109_CTL]);

    if ((ON != burst_sel) && (OFF != burst_sel))
    {
        //HART_ERROR_RETURN__(INVALID_SELECTION);
        ret = HART_INVALID_SELECTION;
    }
    else
    {
        tmpptr = hart_GetHartData();
        tmp = *tmpptr;
        tmp.burst_mode_ctrl = burst_sel;

        ret = hart_WriteHartData(&tmp);
    }

    UNUSED_OK(dst);
    return ret;
}

//Special indices
#define INDEX_TRAP_AREA 10000

/**
\brief Returns the address of the variable from a table of allowed variables

Notes:
This command is allowed in all modes, even when write busy or process busy is set
*/
s8_least hartcmd_ReadVariableAddress(const u8 *src, u8 *dst)
{
    const Req_ReadVariableAddress_t *s = (const void *)src;
    Rsp_ReadVariableAddress_t *d = (void *)dst;
    s8_least ret = HART_NO_COMMAND_SPECIFIC_ERRORS;
    u16 Index;
    u16 offset=0, length=0;

    //get the requested variable number
    Index = util_GetU16(s->VariableNumber[0]);

    if(Index < NVRAM_ENTRIES)
    {
        //this is an FRAM variable
        util_PutU8(d->VariableType[0], HC_VarTypeNVMEM);

        (void)nvmem_GetMapEntry(Index, &offset, &length, NULL); //assuming vol 0 for now
        util_PutU16(d->VariableAddress[0], offset);
        util_PutU16(d->VariableSize[0], length);
    }
    else if(Index >= RAMID_FLAG) //base addr
    {
        bool_t result = ram_GetMapEntry(Index, &offset, &length);
        if(result)
        {
            //this is a RAM variable
            util_PutU8(d->VariableType[0], HC_VarTypeRAM);
        }
        else
        {
            ret = HART_INVALID_SELECTION;
        }
    }
    else
    {
        //Special cases
        switch(Index)
        {
            case INDEX_TRAP_AREA:
                //this is an FRAM variable
                util_PutU8(d->VariableType[0], HC_VarTypeNVMEM);
                offset = ASSERT_DATA;
                length = ASSERT_DATA_STORE_SIZE+ASSERT_POSITION_SIZE;
                break;
            default:
                //nothing else yet
                ret = HART_INVALID_SELECTION;
                break;
        }
    }
    util_PutU16(d->VariableAddress[0], offset);
    util_PutU16(d->VariableSize[0], length);

    return ret;
}

//--------------------------------------------------------------------
static const DeviceModeEnum_t devmode2hart_tab[MODEBIT_TOTAL] =
{
    [MODEBIT_FAILSAFE] = HFailsafe,
    [MODEBIT_SETUP] = HSetup,
    [MODEBIT_MANUAL] = HManual,
    [MODEBIT_OPERATE] = HNormal,
};

static const devmodebit_t hart2devmode_tab[MODEBIT_TOTAL] =
{
    [HFailsafe] = MODEBIT_FAILSAFE,
    [HSetup] = MODEBIT_SETUP,
    [HManual] = MODEBIT_MANUAL,
    [HNormal] = MODEBIT_OPERATE,
};

/** \brief Convert internal device mode representation to HART device mode representation
*/
static DeviceModeEnum_t hart_FormatDevModeToHART(devmode_t mode)
{
    return devmode2hart_tab[intlog(mode)-1];
}

/** \brief Convert HART device mode representation to internal device mode representation
*/
static devmode_t hart_FormatDevModeToInternal(u8 hmode)
{
    if((u32)hmode >= NELEM(hart2devmode_tab))
    {
        return MODE_INVALID;
    }
    return MODEMAP(hart2devmode_tab[hmode]);
}




/**
\brief Changes the mode of the device
*/
s8_least hartcmd_ChangeDeviceMode(const u8 *src, u8 *dst)
{
    //"135:Set Mode,>Mode;"
    u8 Mode;
    s8_least Error=HART_NO_COMMAND_SPECIFIC_ERRORS;

    const Req_ChangeDeviceMode_t *s = (const void *)src;
    Rsp_ChangeDeviceMode_t *d = (void *)dst;
    UNUSED_OK(d->DeviceMode); //rely on HART framework to fill data

    UNUSED_OK(dst);
    Mode = util_GetU8(s->DeviceMode[0]);
#ifdef OLD_DEVMODE
    const ModeData_t* pMode;
    pMode = mode_GetMode();

    if( (pMode->mode == MODE_OOS) && (pMode->submode == SUBMODE_OOS_FAILSAFE) )
    {
        //try clearing failsafe before changing to other mode
        if(!mode_ClearFailsafeMode())
        {
            //could not exit failsafe
            Error = HART_CANT_CHANGE_MODE;
        }
    }
    if(Error == HART_NO_COMMAND_SPECIFIC_ERRORS)
#else
    Mode = hart_FormatDevModeToInternal(Mode);
#endif
    {
        switch(Mode)
        {
#ifdef OLD_DEVMODE
            case MODE_OOS:
                if(mode_SetOOSMode(SUBMODE_OOS_NORMAL) != ERR_OK)
#else
            case MODE_SETUP:
                if(mode_SetMode(MODE_SETUP) != ERR_OK)
#endif
                {
                    Error = HART_CANT_CHANGE_MODE;
                }
                break;
            case MODE_MANUAL:
#ifdef OLD_DEVMODE
                if(mode_SetManualMode() != ERR_OK)
#else
                if(mode_SetMode(MODE_MANUAL) != ERR_OK)
#endif
                {
                    Error = HART_CANT_CHANGE_MODE;
                }
                break;
            case MODE_OPERATE:
#if FEATURE_BUMPLESS_XFER == FEATURE_BUMPLESS_XFER_ENABLED
                if(mode_SetNormalModeEx() != ERR_OK)
#endif
#if FEATURE_BUMPLESS_XFER == FEATURE_BUMPLESS_XFER_DISABLED
#ifdef OLD_DEVMODE
                if(mode_SetOperateMode() != ERR_OK)
#else
                if(mode_SetNormalMode() != ERR_OK)
#endif //OLD_DEVMODE
#endif //FEATURE_BUMPLESS_XFER
                {
                    Error = HART_CANT_CHANGE_MODE;
                }
                break;
            default:
                Error = HART_INVALID_DATA;
        }
    }

    return Error;
}


/**
\brief Clears the fault codes

Notes:
This command is allowed in all modes, even when write busy or process busy is set
*/
s8_least hart_Command_138_ClearFaults(const u8 *src, u8 *dst)
{
    bool_t flag;
    u8 Input;

    UNUSED_OK(dst);
    //handle this way so that ANYTHING not 1 is false
    Input = util_GetU8(&src[HC138_FLAG]);
    if(Input == 1)
    {
        flag = true;
    }
    else
    {
        flag = false;
    }

    //"138:Clear Faults;"
    error_ClearFaults(flag);
    return HART_OK;
}

/**
\brief Returns the strokes, cycles, time open/closed/near

Notes:
This command is allowed in all modes, even when write busy or process busy is set
*/
s8_least hart_Command_139_ReadContinuousDiagnostics(const u8 *src, u8 *dst)
{
    //"139:Read Continuous Diag Data,<Travel,<TimeClosed,<TimeOpen,<TimeNear,<Cycles;"
    const ContinuousDiagnostics_t* pContinuousDiagnostics;
    UNUSED_OK(src);

    //not allowed in low cost mode
    if(cnfg_GetOptionConfigDashVersion()==LOW_COST_VERSION)
    {
        /* Legitimate early return: Pre-requisites check */
        return COMMAND_NOT_IMPLEMENTED;
    }

    pContinuousDiagnostics = control_GetContinuousDiagnostics();

    //put the data into the hart buffer
    util_PutU32(&dst[HC139_TRAVEL], pContinuousDiagnostics->TotalTravelCntr);
    util_PutU32(&dst[HC139_TIME_CLOSED], pContinuousDiagnostics->TimeClosedCntr);
    util_PutU32(&dst[HC139_TIME_OPEN], pContinuousDiagnostics->TimeOpenCntr);
    util_PutU32(&dst[HC139_TIME_NEAR], pContinuousDiagnostics->TimeNearCntr);
    util_PutU32(&dst[HC139_CYCLES], pContinuousDiagnostics->CyclesCntr);

    return HART_OK;
}

/**
\brief Clears the continuous diagnostic data.
*/
s8_least hart_Command_140_ResetContinuousDiagnostics(const u8 *src, u8 *dst)
{
    //not allowed in low cost mode
    if(cnfg_GetOptionConfigDashVersion()==LOW_COST_VERSION)
    {
        /* Legitimate early return: Pre-requisites check */
        return COMMAND_NOT_IMPLEMENTED;
    }

    //"140:Reset Continuous Diag;"
    UNUSED_OK(src);
    UNUSED_OK(dst);
    control_ClearContinuousDiagnostics();
    return HART_OK;
}

/**
\brief Returns the raw position, raw signal, etc.

Notes:
This command is allowed in all modes, even when write busy or process busy is set
*/
s8_least hart_Command_141_ReadRawData(const u8 *src, u8 *dst)
{
    //"141:Read Raw Data,<RawSignal,<RawPosition,<RawPressure,<PWMout,<LowPosStop,"
    //        "<HighPosStop,<Temp,<LowTemp,<HighTemp,<IPOutput;"

    /** TFS:4185 */
    u16 nPWMout;
    s16 nPosition, nSignal, IPCurrent;
    const PositionConf_t*   pPositionStop;
    const BoardTemperature_t* pTemperatureData;

    UNUSED_OK(src);
    //get the data
#if FEATURE_LOOPSIGNAL_SENSOR == FEATURE_LOOPSIGNAL_SENSOR_SUPPORTED
    nSignal = sig_GetSignal();
#else
    nSignal = 0; //RED_FLAG fixme
#endif

    nPosition = (s16)vpos_GetRawPosition();

    nPWMout = sysio_GetRealPWM();      /** TFS:4185 */
    pPositionStop = pos_GetPositionConf(NULL);
    pTemperatureData = cycle_GetTemperatureData();

    IPCurrent = (s16)ipcurr_GetCompensatedIpCurrent();

    //put the data into the hart buffer
    util_PutU16(&dst[HC141_RAW_SIGNAL], (u16)nSignal);
    util_PutU16(&dst[HC141_RAW_POSITION], (u16)nPosition);

    //LS - removed code that set this to 0 - we need to see pressure for testing
    util_PutS16(&dst[HC141_RAW_PRESSURE], pres_GetMainPressure());

    util_PutU16(&dst[HC141_IPOUT], nPWMout);    /** TFS:4185 */
#if 0
    util_PutU16(&dst[HC141_LOW_POS_STOP], (u16)pPositionStop->LowPositionStop);
    util_PutU16(&dst[HC141_HIGH_POS_STOP], (u16)pPositionStop->HighPositionStop);
#else
    util_PutU16(&dst[HC141_LOW_POS_STOP], (u16)pPositionStop->rangeval[Xlow]);
    util_PutU16(&dst[HC141_HIGH_POS_STOP], (u16)pPositionStop->rangeval[Xhi]);
#endif
    util_PutU16(&dst[HC141_TEMP], (u16)(pTemperatureData->nBoardTemperature));
    util_PutU16(&dst[HC141_LOW_TEMP], (u16)(pTemperatureData->nLowestTemperature));
    util_PutU16(&dst[HC141_HIGH_TEMP], (u16)(pTemperatureData->nHighestTemperature));
    util_PutU16(&dst[HC141_IPCURRENT], (u16)IPCurrent);
    return HART_OK;
}

/**
\brief Returns the temperature data

Notes:
This command is allowed in all modes, even when write busy or process busy is set
*/
s8_least hartcmd_ReadTemperatures(const u8 *src, u8 *dst)
{
    BoardTemperature_t TemperatureData;
    UNUSED_OK(src);
    Rsp_ReadTemperatures_t *d = (void *)dst;
    Temperatures_t *t = (void *)d->Temperatures[0];
    (void)tempr_GetTemperature(TEMPR_MAINBOARD, &TemperatureData);

    u8 units = fpconvert_IntToFloatBuffer(TemperatureData.nBoardTemperature, UNITSID_TEMPR, t->CurrentTemperature[0]);
    util_PutU8(d->TemprUnits[0], units);
    (void)fpconvert_IntToFloatBuffer(TemperatureData.nHighestTemperature, UNITSID_TEMPR, t->MaxTemperature[0]);
    (void)fpconvert_IntToFloatBuffer(TemperatureData.nLowestTemperature, UNITSID_TEMPR, t->MinTemperature[0]);

    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

#if 0
/**
\brief Returns the current manual setpoint, signal setpoint, and the control mode.

Notes:
This command is allowed in all modes, even when write busy or process busy is set
*/
s8_least hart_Command_154_ReadSetpoint(const u8 *src, u8 *dst)
{
    //"154:Read Setpoint:<SetpointScaled,<SigSetpoint,<ControlMode;"
    s16 ManualSetpoint, SignalSetpoint;
    s32 Setpoint;
    ctlmode_t ControlMode;
    float32 FloatNumber;

    UNUSED_OK(src);

    control_GetControlMode(&ControlMode, &Setpoint);

    //get the setpoint
    ManualSetpoint = (s16)Setpoint;

#if FEATURE_SIGNAL_SETPOINT == FEATURE_SIGNAL_SETPOINT_SUPPORTED
    const ModeData_t *pMode = mode_GetMode();
    s32 Position;
    s16 RawSignal;
    control_GetSetpointPosition(&Setpoint , &Position);
    //if we are in operate mode, show the manual setpoint as the signal setpoint
    if(pMode->mode == MODE_OPERATE)
    {
        //and set the signal setpoint to the returned setpoin
        SignalSetpoint = (s16)Setpoint;
    }
    //if we are in manual or OOS mode, the manual setpoint was returned
    //  - compute the signal setpoint from the signal
    else
    {
        RawSignal = sig_GetSignal();
        SignalSetpoint = (s16)sigsp_ConvertSignalToSetpoint(RawSignal);
    }
#else
    SignalSetpoint = 0; //fixme
#endif //FEATURE_SIGNAL_SETPOINT_SUPPORTED
    FloatNumber = convert_StdPositionToFPosition(ManualSetpoint);
    util_PutFloat(&dst[HC154_MANUAL_SETPOINT], FloatNumber);
    FloatNumber = convert_StdPositionToFPosition(SignalSetpoint);
    util_PutFloat(&dst[HC154_SIGNAL_SETPOINT], FloatNumber);
    util_PutU8(&dst[HC154_CONTROL_MODE], ControlMode); //lint !e633 Sending out a strong type in a byte

    return HART_OK;
}

/**
\brief Writes the setpoint to use in manual mode.

Notes:
Device must be in manual mode or out of service
*/
s8_least hart_Command_155_WriteSetpoint(const u8 *src, u8 *dst)
{
    //"155:Set Setpoint,>Type,>Setpoint;"
    u32 Type;
    ctlmode_t ControlMode=CONTROL_MANUAL_POS; //to make lint happy
    s32 nSetpoint = 0;
    s8_least error;
    const ComputedConfigurationData_t*  pComputedConfigurationData;
    //const ComputedSignalData_t* pSignalData;
    UNUSED_OK(dst);

    //get the parameters from the buffer
    Type = util_GetU8(&src[HC155_TYPE]);
    //special IP diagnostic mode only in OOS mode
    if(Type == TYPE_IP_DIAGNOSTIC)
    {
        error = hart_CheckMode((u8)HC_ALLOWED_MODE_OOS);
        ControlMode = CONTROL_IP_DIAGNOSTIC;
    }
    else
    {
        error = HART_OK;

        //process input
        pComputedConfigurationData =  cnfg_GetComputedConfigurationData();

        //Type (0=signal, 1=position, 2=full closed, 3=full open, 4=fail position),
        switch(Type)
        {
#if FEATURE_SIGNAL_SETPOINT == FEATURE_SIGNAL_SETPOINT_SUPPORTED
            case TYPE_SIGNAL:
            {
                sig_least_t nSignal;
                nSignal = fpconvert_FloatBufferToInt(&src[HC155_SETPOINT], UNITSID_SIGNAL_ENTRY);
                /* The actual limits happen to be 3.7994998 to 20.2004995 */
                if( (nSignal < LOW_SIGNAL_LOW_LIMIT_INT) ||
                   (nSignal > HIGH_SIGNAL_HIGH_LIMIT_INT) )
                {
                    error = HART_INVALID_DATA;
                }
                else
                {
                    //pSignalData = cnfg_GetComputedSignalData();
                    nSetpoint = sigsp_ConvertSignalToSetpoint(nSignal);
                    ControlMode = CONTROL_MANUAL_POS;
                }
                break;
            }
#endif //FEATURE_SIGNAL_SETPOINT_SUPPORTED
            case TYPE_POSITION:
                /* The actual limits happen to be -5.001830816 to 160.003654479899*/
                nSetpoint = fpconvert_FloatBufferToInt(&src[HC155_SETPOINT], UNITSID_POSITION_ENTRY);
                if( (nSetpoint < POS_SETPOINT_LOW_INT) ||
                    (nSetpoint > POS_SETPOINT_HIGH_INT) )
                {
                    error = HART_INVALID_DATA;
                }
                else
                {
                    ControlMode = CONTROL_MANUAL_POS;
                }
                break;
            case TYPE_FULL_CLOSED:
                if(pComputedConfigurationData->bATO)
                {
                    ControlMode = CONTROL_IPOUT_LOW;
                }
                else
                {
                    ControlMode = CONTROL_IPOUT_HIGH;
                }
                break;
            case TYPE_FULL_OPEN:
                if(pComputedConfigurationData->bATO)
                {
                    ControlMode = CONTROL_IPOUT_HIGH;
                }
                else
                {
                    ControlMode = CONTROL_IPOUT_LOW;
                }
                break;
            case TYPE_FAIL_POSITION:
                ControlMode = CONTROL_IPOUT_LOW;
                break;
            default:
                ///invalid type
                error = HART_INVALID_DATA;
                break;
        }
    }

    if(error == HART_OK)
    {
        mode_SetControlMode(ControlMode, nSetpoint);

        ///for write commands, move the input data to the output
    }

    return error;
}
#endif


#if 0
/**
\brief Writes the configuration data to RAM and FRAM and performs the necessary computation.

Notes:
Only allowed in OOS mode
*/
s8_least hart_Command_165_WriteConfigurationData(const u8 *src, u8 *dst)
{
/* data structure
typedef struct
{
    u8 ConfigFlags[CONFIGURATION_BYTES];
    float32 fTightShutoff;       // Tight shutoff limit in % open
    float32 fLowerPositionLimit;     //Lower position limit in % open
    float32 fUpperPositionLimit;    // Upper position limit in % open
    u8 BumplessSpeed;       //Bumpless transfert
    u8 StrokingSpeed;
    u8 PresUnits;       // Units code for pressure
    u8 Language;        // language for text
    CHECKFIELD;
} ConfigurationData_t;
*/
/* Command parameters
1.  Config Flags, UARRAY4
2.  TightShutoff, Float
3.  Lower Position Limit, Float
4.  Upper Position Limit, Float
5.  Bumpless Speed, Unsigned-8
6.  Stroking Speed Limit, Unsigned-8
7.  Pressure Units, Unsigned-8
8.  Language, Unsigned-8
*/
/*
    Processing
    If process is busy then
        Hart_send(HART_ERROR_BUSY)
        Return
    Get varaiables from receive buffer (see software specifications for details of data)
    Save the configuration (see ConfigData module).
    If error then
        HartSend(HART_ERROR_INVALID_DATA) and return
    Add data to send buffer (see software specifications for details of data to add).
*/
    ConfigurationData_t ConfigurationData;
    //const ConfigurationData_t* pConfigurationData;
    s8_least ret;
    //buttonlock_t bl, bl_new;
    UNUSED_OK(dst);

    ///get the data from the hart buffer
    util_GetU8Array(&src[HC165_CONFIG_FLAGS], CONFIGURATION_BYTES, ConfigurationData.ConfigFlags);
#if 0 //FIX THE HART COMMAND!
    ConfigurationData.fTightShutoff =
        util_GetFloat(&src[HC165_TIGHTSHUTOFF]);
    ConfigurationData.fLowerPositionLimit =
        util_GetFloat(&src[HC165_LOW_LIMIT]);
    ConfigurationData.fUpperPositionLimit =
        util_GetFloat(&src[HC165_HIGH_LIMIT]);
    ConfigurationData.BumplessSpeed =
        util_GetU8(&src[HC165_BUMPLESS_SPEED]);
    ConfigurationData.StrokingSpeed =
        util_GetU8(&src[HC165_STROKE_SPEED]);
    ConfigurationData.PresUnits =
        util_GetU8(&src[HC165_PRES_UNITS]);
    ConfigurationData.Language =
        util_GetU8(&src[HC165_LANGUAGE]);
#endif

    ///if button lock has changed then change the ui
    //pConfigurationData = cnfg_GetConfigurationData();
    //bl = cnfg_GetButtonLock(pConfigurationData); //old

    ///save the data
    if(cnfg_SetConfigurationData(&ConfigurationData) != ERR_OK)
    {
        ///must be in OOS mode for this command
        //HART_ERROR_RETURN__(HART_INVALID_DATA);
        ret = HART_INVALID_DATA;
    }
    else
    {
        ///for write commands, move the input data to the output
#if 0 //UI language is not in LCX and will have its own configuration in future AP
        ///if button lock has changed then change the ui
        bl_new = cnfg_GetButtonLock(pConfigurationData); //new
        if(bl != bl_new)
        {
            ui_FollowLock(bl_new);
        }
#endif
        ret = HART_OK;
    }

    return ret;
}
#endif


/**
\brief Save some startup bias parameters

Notes:
    Only valid in OOS mode - used primarily in the factory
*/
s8_least hart_Command_174_WriteBiasParameters(const u8 *src, u8 *dst)
{
    s8_least ret;
    BiasExt_t BiasExt;
    UNUSED_OK(dst);

    BiasExt.uiBiasShift = util_GetU16(&src[HC174_BIAS_SHIFT]);
    BiasExt.nBiasAddAirLoss = (s16)util_GetU16(&src[HC174_BIAS_ADD_AIRLOSS]);
    BiasExt.nBiasAdd = (s16)util_GetU16(&src[HC174_BAIS_ADD]);
    BiasExt.nBiasTempCoef = (s16)util_GetU16(&src[HC174_BIAS_TEMP_COEF]);
    BiasExt.uiMaxHysteresis = util_GetU16(&src[HC174_HYSTERESIS]);

    if(control_SetBiasExt(&BiasExt) != ERR_OK)
    {
        //HART_ERROR_RETURN__(HART_INVALID_DATA);
        ret = HART_INVALID_DATA;
    }
    else
    {
        ///for write commands, move the input data to the output
        ret = HART_OK;
    }
    return ret;
}



/**
\brief Replaces stop and recalibrates the position

Notes:
The control mode must be in full open or full closed so that the valve is at the stop position
*/
s8_least hartcmd_ManualFindStops(const u8 *src, u8 *dst)
{
    const Req_ManualFindStops_t *s = (const void *)src;
    Rsp_ManualFindStops_t *d = (void *)dst;
    UNUSED_OK(d);

    u8 Step;
    ErrorCode_t err;

    //get the data
    Step = util_GetU8(s->ManualFindStopsStep[0]);

    switch(Step)
    {
        case HFindStopsStart:
        case HFindStopsCancel:
            err = poscal_CommitStops(false);
            break;
        case HFindStopsAccept:
            err = poscal_AcceptStop();
            break;
        case HFindStopsCommit:
            err = poscal_CommitStops(true);
            break;
        default:
            err = ERR_INVALID_PARAMETER;
            break;
    }

    return err2hart(err);
}

/**
\brief This command runs the find stops process
*/
s8_least hartcmd_FindStops(const u8 *src, u8 *dst)
{
    s8_least ret;
    UNUSED_OK(src);
    UNUSED_OK(dst);
    if(hart_SetProcessCommand(PROC_FIND_STOPS))
    {
        ret = HART_BUSY;
    }
    else
    {
        ret = HART_NO_COMMAND_SPECIFIC_ERRORS;
    }
    return ret;
}

#if FEATURE_SIGNAL_SETPOINT == FEATURE_SIGNAL_SETPOINT_SUPPORTED
//deprecated
/**
\brief Saves the low and high signal range

Notes:
    Only valid in OOS mode
*/
s8_least hart_Command_181_WriteInputCurrentRange(const u8 *src, u8 *dst)
{
    //"181:Write Signal Values,>LowSignal,>HighSignal;"
    s8_least ret;
    UNUSED_OK(dst);

    //get the data from the hart buffer
    SignalData_t sd;
    (void)cnfg_GetSignalData(&sd);

    sd.LowSignal = fpconvert_FloatBufferToInt16Clamped(&src[HC181_LOW_SIGNAL], UNITSID_SIGNAL_ENTRY, SignalLimits_range[Xlow]);
    sd.HighSignal = fpconvert_FloatBufferToInt16Clamped(&src[HC181_HIGH_SIGNAL], UNITSID_SIGNAL_ENTRY, SignalLimits_range[Xhi]);

    //save the data
    if(cnfg_SetSignalData(&sd) != ERR_OK)
    {
        //HART_ERROR_RETURN__(HART_INVALID_DATA);
        ret = HART_INVALID_DATA;
    }
    else
    {
        ///for write commands, move the input data to the output
        ret = HART_OK;
    }
    return ret;
}
#endif

/**
\brief Resets the user calibrations back to the factory calibrations

Notes:
    Only valid in OOS mode

*/
s8_least hart_Command_183_ResetToFactorCal(const u8 *src, u8 *dst)
{
/* Set SignalCalData to factory default (4000 = 4ma, 20000=20ma) and save using SetSignalCalData() */
/* set pressure cal zero data to 0 */
    UNUSED_OK(src);
    UNUSED_OK(dst);

    ErrorCode_t ReturnValue;

    //set to defaults

//save the data
#if FEATURE_LOOPSIGNAL_SENSOR == FEATURE_LOOPSIGNAL_SENSOR_SUPPORTED
    ReturnValue = sig_SetSignalCalData(NULL);
    if(ReturnValue != ERR_OK)
    {
        //should never have a problem with default data
        MN_ASSERT(ReturnValue == ERR_OK);
    }
#endif

    ReturnValue = pres_SetPressureCalData(NULL);
    if(ReturnValue != ERR_OK)
    {
        //should never have a problem with default data
        MN_ASSERT(ReturnValue == ERR_OK);
    }
#if FEATURE_AO == FEATURE_AO_SUPPORTED
    ReturnValue = ao_SetAOCalibration(AO_CHANNEL_POS_RETRANSMIT, NULL);
    if(ReturnValue != ERR_OK)
    {
        //should never have a problem with default data
        MN_ASSERT(ReturnValue == ERR_OK);
    }
#endif
    return HART_OK;
}


/**
\brief A shell for a diagnostic process
*/
s8_least hartcmd_RunaProcess(const u8 *src, u8 *dst)
{
    UNUSED_OK(src);
    UNUSED_OK(dst);
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

#if FEATURE_LOCAL_UI == FEATURE_LOCAL_UI_SIMPLE
//Complete this factory subcommand for LCX
s8_least ...
{   // This command shall be valid ONLY when the device is in FACTORY mode
    // Now - validate the parameters
    // In this mode:
    // Type         - is the Type.
    // nSpeed       - TimeOut in Seconds, 0 means no T/O, command invalid
    // Parameter 1  - Device Test Mode #
    // Parameter 2  - Optional - for specific mode, LED # to lit

    ErrorCode_t     err;
    test_TestMode_t TestMode;
    LEDNumber_t     TestLed;
    u16_least       TestTime;

    TestMode = (test_TestMode_t)Parameter1;
    TestTime = (u16)nSpeed;
    TestLed  = (LEDNumber_t)Parameter2;

    err = test_SetTestParameters(TestMode, TestTime, TestLed);
    if (err != ERR_OK)
    {   // Parameters are not accepted
        ret = HART_INVALID_SELECTION;
    }
    else
    {   // Parameters are OK
        if(hart_SetProcessCommand((u8)PROC_TEST_PROCESS))
        {   //could not start process
            //HART_ERROR_RETURN__(HART_BUSY);
            ret = HART_BUSY;
        }
        else
        {
            ret = HART_NO_COMMAND_SPECIFIC_ERRORS;
        }
    }
}
#endif //FEATURE_LOCAL_UI == FEATURE_LOCAL_UI_SIMPLE


/**
\brief Reads data from the diagnostic data buffer
*/
s8_least hartcmd_ReadDataBufferRaw(const u8 *src, u8 *dst)
{
    ErrorCode_t err;

    const Req_ReadDataBufferRaw_t *s = (const void *)src;
    Rsp_ReadDataBufferRaw_t *d = (void *)dst;

    u8 BufferId = util_GetU8(s->BufferId[0]);
    u16_least DataOffset = util_GetU16(s->DataOffset[0]);
    u8_least DataSampleSize = util_GetU8(s->DataSampleSize[0]);
    u8 DataSampleSkipCount = util_GetU8(s->DataSampleSkipCount[0]);

    const u8 *p = (const u8 *)buffer_GetXDiagnosticBuffer(BufferId);
    s16_least n = buffer_GetEntriesInXDiagnosticBuffer(BufferId); //in diag_t samples
    u16_least bytes_in_buf = sizeof(diag_t) * (u16_least)n; //in bytes
    u16_least datasz = bytes_in_buf;

    if(p == NULL)
    {
        err = ERR_INVALID_PARAMETER;
    }
    else if((DataOffset > datasz) || (DataSampleSize == 0) || (DataSampleSize > sizeof(d->RawDataSamples)))
    {
        err = ERR_INVALID_PARAMETER;
    }
    else
    {
        datasz -= DataOffset;
        datasz = MIN(datasz, sizeof(d->RawDataSamples)); //number of bytes to populate
        u8_least num_samples = 0;
        u8 *rsp = d->RawDataSamples[0];
        while(datasz >= DataSampleSize)
        {
            mn_memcpy(rsp, p + DataOffset, DataSampleSize);
            rsp += DataSampleSize;
            DataOffset += DataSampleSize * (1U + DataSampleSkipCount);
            if((datasz >= DataSampleSize) && (DataOffset < bytes_in_buf))
            {
                datasz -= DataSampleSize;
            }
            else
            {
                datasz = 0;
            }
            num_samples++;
        }
        mn_memset(rsp, 0x80, sizeof(d->RawDataSamples) - (num_samples*DataSampleSize));

        util_PutU16(d->DataOffset[0], (u16)DataOffset);
        util_PutU8(d->DataSampleCount[0], (u8)num_samples);

        //Rely on HART framework to fill the members
        UNUSED_OK(d->BufferId);
        UNUSED_OK(d->DataSampleSize);
        UNUSED_OK(d->DataSampleSkipCount);
        err = ERR_OK;
    }

    return err2hart(err);
}


/**
\brief Returns the option configuration data.

Notes:
This command is allowed in all modes, even when write busy or process busy is set
*/

#define CMD_200_BYTE_FOR_DO     BYTENUM(DO_OPTION)
#define CMD_200_BIT_FOR_DO      BYTEBITMASK(DO_OPTION)

s8_least hart_Command_200_ReadOptionConfiguration(const u8 *src, u8 *dst)
{
    //"200:Read Option Config,<OptionConfig;"
    //{"OptionConfig", (void*)NULL, H_ARRAY+16},

    const   OptionConfig_t* pOptionConfig;

    UNUSED_OK(src);

    // get a pointer to the configuration
    pOptionConfig = cnfg_GetOptionConfig();

    // and fill the response
    util_PutU8Array(&dst[HC200_OPTION_CONFIG], OPTION_CONFIG_BYTES, pOptionConfig->m_OptionConfig);

    // Extract the DO_OPTION
    u8  thebyte = pOptionConfig->m_OptionConfig[CMD_200_BYTE_FOR_DO];
    if (!bios_DOCardPresent())
    {   // Terminal Card is NOT present -- clear the bit
        thebyte &= ~CMD_200_BIT_FOR_DO;
    }

    util_PutU8(&dst[CMD_200_BYTE_FOR_DO], thebyte);
    return HART_OK;
}

/**
\brief Returns the actual option configuration data.

Notes:
This command is allowed in all modes, even when write busy or process busy is set
*/

s8_least hart_Command_202_ActualReadOptionConfiguration(const u8 *src, u8 *dst)
{
    //"202:Actual Read Option Config,<OptionConfig;"
    //{"OptionConfig", (void*)NULL, H_ARRAY+16},

    const   OptionConfig_t* pOptionConfig;

    UNUSED_OK(src);

    // get a pointer to the configuration
    pOptionConfig = cnfg_GetOptionConfig();

    // and fill the response
    util_PutU8Array(&dst[HC202_OPTION_CONFIG], OPTION_CONFIG_BYTES, pOptionConfig->m_OptionConfig);
    return HART_OK;
}


#if FEATURE_PRESSURE_INPUTS == FEATURE_PRESSURE_INPUTS_INCLUDED
#define NUMBER_PRESSURES_REPORTED 5
/**
\brief Returns pressure data - returns 0 for pressures that are not enabled

Notes:
This command is allowed in all modes, even when write busy or process busy is set
*/
s8_least hartcmd_ReadAllPressures(const u8 *src, u8 *dst)
{
    Rsp_ReadAllPressures_t *d = (void *)dst;
    CONST_ASSERT(NUMBER_PRESSURES_REPORTED == NELEM(d->PressureArray));

    const BoardPressure_t* BoardPressure;
    pres_t Pressure;
    u32 i;
    bool_t bPressure[NUMBER_PRESSURES_REPORTED];
    static const u8 PressureIndex[NUMBER_PRESSURES_REPORTED] =
    {
        PRESSURE_ACT1_INDEX,
        PRESSURE_ACT2_INDEX,
        PRESSURE_SUPPLY_INDEX,
        PRESSURE_PILOT_INDEX,
        PRESSURE_ATM_INDEX
    };
    //static const u8 PressureDataStart[NUMBER_PRESSURES_REPORTED] = {HC201_PRESSURE, HC201_PRESSURE2, HC201_PRESSURE3, HC201_PRESSURE4};
    UNUSED_OK(src);

    //see which pressures are there
    bPressure[PRESSURE_ACT1_INDEX] = cnfg_GetOptionConfigFlag(PRESSURE_SENSOR_1);
    bPressure[PRESSURE_ACT2_INDEX] = cnfg_GetOptionConfigFlag(PRESSURE_SENSOR_2);
    bPressure[PRESSURE_SUPPLY_INDEX] = cnfg_GetOptionConfigFlag(PRESSURE_SUPPLY);
    bPressure[PRESSURE_PILOT_INDEX] = true; //TFS:3995 -- show Pilot Pressure in Cmd-201 for LCX
    bPressure[PRESSURE_ATM_INDEX] = true;
    CONST_ASSERT(NELEM(bPressure)==NUMBER_PRESSURES_REPORTED);

    BoardPressure = pres_GetPressureData();
    u8 PresUnits;

    for(i=0; i<NUMBER_PRESSURES_REPORTED; ++i)
    {
        Pressure = BoardPressure->Pressures[PressureIndex[i]];
        if(!bPressure[i] || Pressure==PRESSURE_INVALID)
        {
            //Replace with 0; should turn into a NaN soon
            Pressure = 0;
        }
        PresUnits = fpconvert_IntToFloatBuffer(Pressure, UNITSID_PRESSURE, d->PressureArray[i]);
    }

    util_PutU8(d->PresUnits[0], PresUnits);

    return HART_OK;
}
#endif //FEATURE_PRESSURE_INPUTS_INCLUDED



#if FEATURE_LOOPSIGNAL_SENSOR == FEATURE_LOOPSIGNAL_SENSOR_SUPPORTED
//deprected
/**
\brief Returns the signal values.

Notes:
This command is allowed in all modes, even when write busy or process busy is set
*/
s8_least hart_Command_213_ReadCurrentLoopValues(const u8 *src, u8 *dst)
{
    //see command 181 for defines
    //"213:Read Signal Values,<LowSignal,<HighSignal;"}
    const SignalData_t*     pSignalData;
    UNUSED_OK(src);
    pSignalData = cnfg_GetSignalData(NULL);

    (void)fpconvert_IntToFloatBuffer(pSignalData->LowSignal, UNITSID_SIGNAL_ENTRY, &dst[HC181_LOW_SIGNAL]);
    (void)fpconvert_IntToFloatBuffer(pSignalData->HighSignal, UNITSID_SIGNAL_ENTRY, &dst[HC181_HIGH_SIGNAL]);
    return HART_OK;
}
#endif //FEATURE_LOOPSIGNAL_SENSOR_SUPPORTED

/**
\brief Reads the bias startup parameters

Notes:
This command is allowed in all modes, even when write busy or process busy is set
*/
s8_least hart_Command_217_ReadBiasParameters(const u8 *src, u8 *dst)
{
    const BiasExt_t* pBiasExt;
    pBiasExt = control_GetBiasExt();

    UNUSED_OK(src);

    util_PutU16(&dst[HC174_BIAS_SHIFT], pBiasExt->uiBiasShift);
    util_PutU16(&dst[HC174_BIAS_ADD_AIRLOSS], (u16)(pBiasExt->nBiasAddAirLoss));
    util_PutU16(&dst[HC174_BAIS_ADD], (u16)(pBiasExt->nBiasAdd));
    util_PutU16(&dst[HC174_BIAS_TEMP_COEF], (u16)(pBiasExt->nBiasTempCoef));
    util_PutU16(&dst[HC174_HYSTERESIS], pBiasExt->uiMaxHysteresis);
    return HART_OK;
}


/**
\brief Writes the option configuation - factory only

Notes:
This command is allowed in only in oos mode
*/
s8_least hart_Command_248_WriteOptionConfig(const u8 *src, u8 *dst)
{
    u16 Key;
    OptionConfig_t OptionConfig;
    s8_least retCode;

    //make sure it's the factory sending this
    Key = util_GetU16(&src[HC248_KEY]);
    if(Key != HC248_KEY_VALUE)
    {
        /* Legitimate early return: Pre-requisites check */
        return COMMAND_NOT_IMPLEMENTED;
    }

    ///for write commands, move the input data to the output
    UNUSED_OK(dst);

    //save the option config data
    util_GetU8Array(&src[HC248_OPTION_CONFIG], OPTION_CONFIG_BYTES, OptionConfig.m_OptionConfig);
    if(cnfg_SetOptionConfig(&OptionConfig) != ERR_OK)
    {
        retCode = HART_INVALID_DATA;
    }
    else
    {
        retCode = HART_OK;
    }

    return retCode;
}


/**
\brief Returns the mode

Notes:
This command is allowed in all modes, even when write busy or process busy is set
*/
s8_least hartcmd_ReadDeviceMode(const u8 *src, u8 *dst)
{
    //"249:Read Mode,<Mode,<SubMode;"
#ifdef OLD_DEVMODE
    const ModeData_t* pMode;
    u8 submode;

    // get a pointer to the current mode information
    pMode = mode_GetMode();

     // write the mode in to the buffer
    util_PutU8(&dst[HC249_MODE], pMode->mode);
    submode = pMode->submode;
    if(error_IsFault(FAULT_MARGINAL_POWER))
    {
        submode |= SUBMODE_MARGINAL_POWER; //an artificial flag for ValVue
    }
    util_PutU8(&dst[HC249_SUB_MODE], submode);
#else
    Rsp_ReadDeviceMode_t *d1 = (void *)dst;
    FullDeviceMode_t *d = (void *)d1->FullDeviceMode[0];
    devsubmode_t submode = mode_GetSubmode();
    devmode_t mode = mode_GetMode();
    DeviceModeEnum_t dmode = hart_FormatDevModeToHART(mode);
#if 0 //temporary patch
    if(dmode == HFailsafe)
    {
        dmode = HSetup;
        submode |= 0x02U; //old failsafe
    }
#endif
    util_PutU8(d->DeviceMode[0], dmode);
    util_PutU8(d->DeviceSubmode[0], submode);
#endif
    UNUSED_OK(src);
    return HART_OK;
}



/**
\brief Returns the status of long processes.

Notes:
This command is allowed in all modes, even when write busy or process busy is set
*/
s8_least hartcmd_CheckProcess(const u8 *src, u8 *dst)
{
    Rsp_CheckProcess_t *d = (void *)dst;
    UNUSED_OK(src);

    util_PutU8(d->ProcessId[0], process_GetProcId());
    util_PutU8(d->PercentComplete[0], process_CheckProcessProgress());

    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

/**
\brief Stops any process running by setting a flag.

Notes:
This command is allowed in all modes, even when write busy or process busy is set
*/
s8_least hart_Command_253_HaltProcess(const u8 *src, u8 *dst)
{
    // cancel the process, no checking required
    process_CancelProcess();
    UNUSED_OK(src);
    UNUSED_OK(dst);
    return HART_OK;
}

#ifdef OLD_DEVMODE
/** \brief Returns true iff device mode is acceptable for the command
*/
static bool_t hart_TestMode(u8 ModeFlag)
{
    u8 allowed;
    ModeData_t mode = *mode_GetMode();
    if(mode.mode == MODE_OPERATE)
    {
        allowed = HC_ALLOWED_MODE_NORMAL;
    }
    else if(mode.mode == MODE_MANUAL)
    {
        allowed = HC_ALLOWED_MODE_MAN;
    }
    else
    {
        MN_DBG_ASSERT(mode.mode == MODE_OOS);
        if(mode.submode == SUBMODE_OOS_FAILSAFE)
        {
            allowed = HC_ALLOWED_MODE_FS;
        }
        else
        {
            allowed = HC_ALLOWED_MODE_OOS;
        }
    }
    return (ModeFlag & allowed) != 0;
}
#endif

/**
\brief sets an error if wrong mode

ModeAllowed=0 means all modes, ModeAllowed=1 means manual or OOS
*/
//static bool_t hart_CheckMode(devmode_t ModeAllowed)
//{
//    const ModeData_t* pMode;
//
//    pMode = mode_GetMode();
//    if(pMode->mode > ModeAllowed)
//    {
//        ///must be in OOS mode for this command
//        HART_ERROR_RETURN__(HART_WRONG_MODE);
//        return true;
//    }
//    return false;
//}
s8_least hart_CheckMode(u8 ModeFlag)
{
    u8 HartError = HART_NO_COMMAND_SPECIFIC_ERRORS;

#ifdef OLD_DEVMODE
    if(!hart_TestMode(ModeFlag))
#else
    devmode_t mode;
    mode = mode_GetMode();
    mode = mode_GetEffectiveMode(mode);

    if( (HC_MODE_BITMASK(mode) & ModeFlag) == 0U)
#endif
    {
        ///not allowed in this mode
        HartError = HART_WRONG_MODE;
    }

    //if factory command, only allow if enabled
    if( ((ModeFlag&HC_FACTORY_COMMAND) != 0)
        && !error_IsFault(FAULT_FACTORYMODE))
    {
        ///not allowed in this mode
        HartError = COMMAND_NOT_IMPLEMENTED;
    }

    return HartError;
}

//================vv Top-level envelopes for subcommands vv===============
s8_least hartcmd_ResetToFactoryCalibrations(const u8 *src, u8 *dst)
{
    UNUSED_OK(src);
    UNUSED_OK(dst);
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}
s8_least hartcmd_ReadUserTrim(const u8 *src, u8 *dst)
{
    UNUSED_OK(src);
    UNUSED_OK(dst);
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}
//================^^ Top-level envelopes for subcommands ^^===============


//----------------------- Stolen from the trunk -------------------------
//A not-instantiated object to feed the NELEM
DUMMMY_OBJECT(Rsp_LosslesslyReadDataBuffer_t, dummy_object);

typedef struct BufferCache_t
{
    u8 BufferId;
    u8 Tag;
    diag_t cache[NELEM(dummy_object.BufferDataLong)];
    u8 length;
    u16 CheckWord;
} BufferCache_t;

static BufferCache_t BufferCache =
{
    .BufferId = 0x80,
    .CheckWord = (u16)(CRC_SEED + 0x80),
};

/**
\brief Reads data from the circular diagnostic data buffer with losless retries
*/
s8_least hartcmd_LosslesslyReadDataBuffer(const u8 *src, u8 *dst)
{
    s8_least ret;
    const Req_LosslesslyReadDataBuffer_t *s = (const void *)src;
    Rsp_LosslesslyReadDataBuffer_t *d = (void *)dst;

    u8 bufid_cmd = util_GetU8(s->BufferId[0]);
    u8 bufid = bufid_cmd & 0x7fU;
    u8 tag = util_GetU8(s->BufferReadTag[0]);

    if(bufid != bufid_cmd)
    {
        //Reset the read session by convention
        storeMemberInt(&BufferCache, BufferId, bufid);
        storeMemberInt(&BufferCache, Tag, tag);
        ret = TRANSMITTER_SPECIFIC_COMMAND_ERROR; //by convention
    }
    else
    {
        Struct_Test(BufferCache_t, &BufferCache);

        if(bufid == BufferCache.BufferId)
        {
            //buffer is correct; new or retry?
            if(tag == (BufferCache.Tag + 1))
            {
                //new request; fill the cache
                BufferCache.Tag = tag;
                BufferCache.length = (u8)buffer_GetDataFromXDiagnosticBuffer(bufid, NELEM(BufferCache.cache), BufferCache.cache);
                STRUCT_CLOSE(BufferCache_t, &BufferCache);
            }
            //Send the data from cache
            if(tag == BufferCache.Tag)
            {
                util_PutU8(d->BufferReadTag[0], tag);
                util_PutU8(d->BufferReadLength[0], BufferCache.length);
                for(u8_least ix=0; ix < NELEM(BufferCache.cache); ix++)
                {
                    util_PutU16(d->BufferDataLong[ix], (u16)BufferCache.cache[ix]);
                }
                ret = HART_NO_COMMAND_SPECIFIC_ERRORS;
            }
            else
            {
                ret = HART_INVALID_DATA;
            }
        }
        else
        {
            //wrong buffer
            ret = HART_INVALID_DATA;
        }
    }

    return ret;
}

//----------------------- End Stolen from the trunk -------------------------

/*lint +esym(613,src,dst) restore pointers checking
*/

/* This line marks the end of the source */
