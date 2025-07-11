/**
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file hartfunc.h
    \brief Public interfaces of the HART Functions Module

    CPU: Any

    OWNER: LS
    $Archive: /MNCB/Dev/ESD_Release_3.1.2/FWH6/FIRMWARE/includes/hartfunc.h $
    $Date: 9/02/11 5:38p $
    $Revision: 62 $
    $Author: Arkkhasin $
*/

/*
    $History: hartfunc.h $
 *
 * *****************  Version 62  *****************
 * User: Arkkhasin    Date: 9/02/11    Time: 5:38p
 * Updated in $/MNCB/Dev/ESD_Release_3.1.2/FWH6/FIRMWARE/includes
 * TFS:7480 Post-integration Lint cleanup
 *
 * *****************  Version 61  *****************
 * User: Arkkhasin    Date: 8/10/11    Time: 1:17p
 * Updated in $/MNCB/Dev/ESD_Release_3.1.2/FWH6/FIRMWARE/includes
 * HART 6 integration from AP
 *
 * *****************  Version 60  *****************
 * User: Arkkhasin    Date: 8/08/11    Time: 5:22p
 * Updated in $/MNCB/Dev/ESD_Release_3.1.2/FWH6/FIRMWARE/includes
 * Again, per Anatoly's review
 *
 * *****************  Version 59  *****************
 * User: Arkkhasin    Date: 8/08/11    Time: 1:31p
 * Updated in $/MNCB/Dev/ESD_Release_3.1.2/FWH6/FIRMWARE/includes
 * TFS:7218 HART response codes
 *
 * *****************  Version 58  *****************
 * User: Arkkhasin    Date: 7/26/11    Time: 4:00p
 * Updated in $/MNCB/Dev/ESD_Release_3.1.2/FWH6/FIRMWARE/includes
 * Merge of AP HART 6 version with ESD
 *
 *
 * *****************  Version 71  *****************
 * User: Arkkhasin    Date: 7/14/11    Time: 4:05p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/includes
 * TFS Sharebox3 merge
 *
 * *****************  Version 70  *****************
 * User: Arkkhasin    Date: 7/11/11    Time: 6:52p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/includes
 * Response codes became disjoint between changesets 19572 and 19694
 * (Thanks, Sergey!). Pulled back together again FBO TFS:5967
 *
 * *****************  Version 69  *****************
 * User: Arkkhasin    Date: 7/08/11    Time: 2:18p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/includes
 * TFS:6957 Free up an HC_ flag and make HC_MODIFY common underlying flag
 * for HC_WRITE and HC_COMMAND
 *
 * *****************  Version 68  *****************
 * User: Arkkhasin    Date: 7/08/11    Time: 12:42p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/includes
 * To TFS Changeset 20115 FBO TFS 6957
 *
 * *****************  Version 67  *****************
 * User: Arkkhasin    Date: 6/30/11    Time: 1:54p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/includes
 * TFS:6880,6883 - (internal) HART revisions enumerated from highest down.
 * TFS:6876,6402 - A mechanism to "undefine" a HART 5 command in HART 6
 * Lint
 *
 *
 * *****************  Version 66  *****************
 * User: Sergey Kruss Date: 6/13/11    Time: 6:35p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/includes
 * Added HART Command 9 and related functions.
 *
 * *****************  Version 65  *****************
 * User: Arkkhasin    Date: 6/03/11    Time: 2:36p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/includes
 * Exclusive configuration channel support FBO TFS:6455
 *
 * *****************  Version 64  *****************
 * User: Arkkhasin    Date: 5/28/11    Time: 12:38p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/includes
 * TFS:6474 Addressing test hoisted to hart.c. The dispatchers reverted to
 * previous version
 *
 * *****************  Version 63  *****************
 * User: Arkkhasin    Date: 5/27/11    Time: 5:08p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/includes
 * TFS:6474,6450: Polling address range and broadcast addressing repair
 *
 * *****************  Version 62  *****************
 * User: Arkkhasin    Date: 4/29/11    Time: 3:37p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/includes
 * TFS:6159 New HART reset strategy
 *
 * *****************  Version 61  *****************
 * User: Arkkhasin    Date: 4/28/11    Time: 12:33p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/includes
 * TFS:6188 Function names
 *
 * *****************  Version 59  *****************
 * User: Arkkhasin    Date: 4/27/11    Time: 5:15p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/includes
 * Redefined HART flags to be compatible with other XML-friendly codebases
 * and moved from hartcmd.h to hartfunc.h
 *
 * *****************  Version 58  *****************
 * User: Arkkhasin    Date: 3/19/11    Time: 3:58p
 * Updated in $/MNCB/Dev/HART6Devel/FIRMWARE/includes
 * Fixed (back) the disp type
 *
 * *****************  Version 57  *****************
 * User: Arkkhasin    Date: 3/18/11    Time: 10:00p
 * Updated in $/MNCB/Dev/HART6Devel/FIRMWARE/includes
 * Added prototypes for HART 6 dispatcher
 *
 * *****************  Version 56  *****************
 * User: Arkkhasin    Date: 3/16/11    Time: 12:41p
 * Updated in $/MNCB/Dev/HART6Devel/FIRMWARE/includes
 * 1. New HART prototypes
 * 2. Some macros made local in hartfunc.c from hartfunc.h
 * 3. Conditionals for new NVRAM
 *
 * *****************  Version 54  *****************
 * User: Sergey Kruss Date: 11/15/10   Time: 2:50p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * TFS:4713 - moved FP setpoint limits to "configure.h"
 *
 * *****************  Version 53  *****************
 * User: Arkkhasin    Date: 9/03/10    Time: 6:13p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * First step in TFS 4067: extracted commands 129, 130
 *
 * *****************  Version 52  *****************
 * User: Anatoly Podpaly Date: 8/27/10    Time: 6:41p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * Moved Read Status function. Put "Placeholder" for the future debug
 * function.
 *
 * *****************  Version 51  *****************
 * User: Anatoly Podpaly Date: 8/27/10    Time: 5:42p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * Added DEBUG defs for WiperLock control.
 *
 * *****************  Version 50  *****************
 * User: Anatoly Podpaly Date: 8/27/10    Time: 4:20p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * Added 129 and 130 commands to access Digital Resistor.
 *
 * *****************  Version 49  *****************
 * User: Anatoly Podpaly Date: 7/30/10    Time: 1:33p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * Bug 3674 - implemented cmd 129,172 - read serialization chip.
 *
 * *****************  Version 48  *****************
 * User: Anatoly Podpaly Date: 4/01/10    Time: 5:48p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * Bug #2930 - Enable UI emulation.Removed unused definition.
 *
 * *****************  Version 47  *****************
 * User: Anatoly Podpaly Date: 3/25/10    Time: 11:07a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * See Bug #2807 - implemented similar changes to resolve potential
 * conflict with Response codes.
 *
 * *****************  Version 46  *****************
 * User: Arkkhasin    Date: 3/12/10    Time: 3:40p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * Convert 48 and 136 to xml format
 *
 * *****************  Version 45  *****************
 * User: Arkkhasin    Date: 2/24/10    Time: 7:01p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * Back-ported some HART response codes from the trunk
 *
 *
 * *****************  Version 44  *****************
 * User: Arkkhasin    Date: 2/09/10    Time: 2:45p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * Back-port and minor improvements of HART lookup table generation along
 * with C types imported from the xml framework and subcommands support
 *
 * *****************  Version 43  *****************
 * User: Anatoly Podpaly Date: 2/01/10    Time: 4:47p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * LCX initial implementation. Commands added.
 *
 * *****************  Version 41  *****************
 * User: Justin Shriver Date: 1/20/10    Time: 3:42p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/includes
 * TFS:2499
*/

#ifndef HARTFUNC_H_
#define HARTFUNC_H_
#include "config.h"
#include "ifman.h"
#include "hart.h" //needed to fix the response codes

//defines for the flags
//TFS:6957 -  hurray, flag 0x40  is freed up. But for now, WRITE and COMMAND are almost the same
#define HC_WRITE_PROTECT 0x01U
#define HC_MODIFY 0x02U
#define HC_WRITE_COMMAND (HC_MODIFY | HC_WRITE_PROTECT)
#define HC_READ_COMMAND 0x00U //Not used for anything
#define HC_UNPUBLISHED_COMMAND 0x00U //Not used for anything
#define HC_COMMAND_COMMAND HC_MODIFY
#define HC_PROCESS_COMMAND HC_COMMAND_COMMAND
#define HC_FACTORY_COMMAND 0x08U

#ifdef OLD_DEVMODE

#define HC_ALLOWED_MODE_ALL HC_ALLOWED_MODE_MASK
#define HC_ALLOWED_MODE_NORMAL 0x40U
#define HC_ALLOWED_MODE_MAN 0x20U
#define HC_ALLOWED_MODE_OOS 0x10U
#define HC_ALLOWED_MODE_FS 0x80U
#define HC_ALLOWED_MODE_MAN_OOS (HC_ALLOWED_MODE_MAN|HC_ALLOWED_MODE_OOS)
#define HC_ALLOWED_MODE_MASK 0xf0U

#else
//see hartcmd.h
#endif //OLD_DEVMODE

//Module Member Data
struct hartDispatch_t
{
    u8 cmd; //command id
    u8 rsp_len; //response length, 2-based (+2, that is)
    u8 req_len; //request/command length; 0 means no length checking
    u8 flags;
    s8_least (*proc)(const u8 *src, u8 *dst); //processing function; if NULL, nothing to do
};
typedef struct hartDispatch_t hartDispatch_t;
extern const hartDispatch_t *HART_cmdlookup(u8_least key); //key is command number

#if 0
typedef struct HartCmdParams_t
{
    u8 req_data_len;
    u8 rsp_data_len;
    u8 rsp_warning;
    bool_t master; //true iff primary
} HartCmdParams_t;

typedef struct HartDispatch_t
{
    u8 cmd; //command id
    u8 rsp_len; //response length, 2-based (+2, that is)
    u8 req_len; //request/command length; 0 means no length checking
    u8 flags;
    s8_least (*proc)(const void *src, void *dst, HartCmdParams_t *params); //processing function; if NULL, nothing to do
} HartDispatch_t;
#else
//#define  HartCmdParams_t
typedef void HartDispatch_t;
#endif
extern const HartDispatch_t *Hart_Cmdlookup(u8_least cmd);

typedef enum hart_address_t
{
    //These are addressing from host (STX frame)
    hart_address_ok =0,
    hart_address_long = hart_address_ok,
    hart_address_broadcast =1,
    hart_address_not4us =2,
    hart_address_short = 3
        //no need in this: , hart_address_short_not4us = 4
    /*In the future, these may be expanded to addressing
      for listening to others ACK or even BACK frames
    */
} hart_address_t;

/** \brief Checks if the addressing is appropriate for a request (STX frame) command
\param cmd - received request command number
\param hart_address_type - addressing in the received command
\return true iff command COULD be accepted
*/
extern bool_t hart_TestStxAddressing(u8_least cmd, hart_address_t hart_address_type);

//Type of automatic subcommands support
typedef struct SubCommandInfo_t
{
    u8 type_offset; //!< where the subcommand id is located in the input data frame
    u8 length_offset;    //!< 1-based offset of the subcommand request length in the input data frame (0=absent)
    u8 data_offset; //!< offset of the subcommand request data  in the input data frame
    const hartDispatch_t *(*lookup_func)(u8_least key); //A lookup function to rettrieve disp info for the subcommand
} SubCommandInfo_t;

const SubCommandInfo_t *hart_GetSubCommandInfo(u8_least cmd);

typedef s8_least hartdipfunc_t(u8_least cmd, const u8 *src, u8 *dst, u8 req_data_len);
extern const hartdipfunc_t * const hartdisptab[];
typedef s8_least hartburstdispfunc_t(u8_least cmd, u8 *dst);
extern const hartburstdispfunc_t * const hartburstdisptab[];


#define HART_WARNING_SCALER 12 //12 bits of range separation

//Defines
#define HART_NOT_USED 250u //Common value across HART common enum tables


//----------------- HART commands response codes ---------------------
#define COMMAND_NOT_IMPLEMENTED                     64

#define HART_NO_COMMAND_SPECIFIC_ERRORS                   0
#define HART_TYPE_MISMATCH                                1 //obsolte not to be used
#define HART_COMMAND_UNDEFINED HART_TYPE_MISMATCH //use it to prevent fallback to previous HART version
#define HART_INVALID_SELECTION                            2
#define HART_PASSED_PARAMETER_TOO_LARGE                   3
#define HART_PASSED_PARAMETER_TOO_SMALL                   4
#define HART_TOO_FEW_DATA_BYTES_RECEIVED                  5
#define TRANSMITTER_SPECIFIC_COMMAND_ERROR           6
#define HART_WRITE_PROTECT_MODE                        7
#define HART_INVALID_LOCK_CODE                      10

#define HART_ERROR_BYTECOUNT 2u
#define HART_BUSY 32

#define HART_NOT_PROPER_OUTPUT_MODE 9 //9 has multiple definitions; see chpater 3.1 in spec-307
#define HART_RV_LOW_TOO_HIGH 9
#define HART_RV_LOW_TOO_LOW 10
#define HART_RV_HIGH_TOO_HIGH 11
#define HART_RV_HIGH_TOO_LOW 12
#define HART_RV_BOTH_OOR 13
#define HART_RV_SPAN_INVALID 29

#define HART_INVALID_DATA 2

// Definition of HART Response codes (not compatible with 3.1.x AP and ESD 3.1.2 but correct-er)

#if 0 //That fix would go to ESD builds
/*NOTE: The kludgy fix below is necessary for ESD and then DLT when time comes.
AP and LCX got compliant response codes before HCF froze HART 5
*/
#define HART_WRONG_MODE         ((hart_GetHartData()->hart_version != HART_REV_5)?HART_ACCESS_RESTRICTED:33)
#define HART_CANT_CHANGE_MODE   ((hart_GetHartData()->hart_version != HART_REV_5)?95:35)
#define HART_FIND_STOPS_FAILED  ((hart_GetHartData()->hart_version != HART_REV_5)?94:36)
#define HART_CAL_FAILED         ((hart_GetHartData()->hart_version != HART_REV_5)?93:37)
#define HART_DIAG_NOT_ALLOWED   ((hart_GetHartData()->hart_version != HART_REV_5)?HART_ACCESS_RESTRICTED:38)
#define HART_TUNE_NOT_ALLOWED   ((hart_GetHartData()->hart_version != HART_REV_5)?HART_ACCESS_RESTRICTED:39)
#define HART_READ_FILE_FAILED   ((hart_GetHartData()->hart_version != HART_REV_5)?92:40)
#define HART_CANT_START_DIAG    ((hart_GetHartData()->hart_version != HART_REV_5)?91:41)
#define HART_PST_NOT_ALLOWED    ((hart_GetHartData()->hart_version != HART_REV_5)?90:42)
#endif //0

// Definition of HART Response codes (not compatible with 3.1.x AP and ESD but correct-er)
#define HART_WRONG_MODE         HART_ACCESS_RESTRICTED
#define HART_ACCESS_RESTRICTED  16
#define HART_CANT_CHANGE_MODE   95
#define HART_FIND_STOPS_FAILED  94
#define HART_CAL_FAILED         93
#define HART_DIAG_NOT_ALLOWED   HART_ACCESS_RESTRICTED
#define HART_TUNE_NOT_ALLOWED   HART_ACCESS_RESTRICTED


//Synonyms - TBD which to keep
#define HART_LOWERRANGE_TOOHIGH    HART_RV_LOW_TOO_HIGH //9
#define HART_LOWERRANGE_TOOLOW     HART_RV_LOW_TOO_LOW //10
#define HART_UPPERRANGE_TOOHIGH    HART_RV_HIGH_TOO_HIGH //11
#define HART_UPPERRANGE_TOOLOW     HART_RV_HIGH_TOO_LOW //12
#define HART_INVALID_SPAN          29
#define HART_INVALID_UNITCODE      18

#define HART_INVALID_MODE_SELECTION 12 //Spec 127 command 6

//For cmd 45, 46
#define HART_INVALID_LOOPCURRENT_MODE_OR_VALUE    9
#define HART_LOOPCURRENT_NOT_ACTIVATE    11

//Non-compliant
#if 0
#define HART_WRONG_MODE 33u
#define HART_INVALID_DATA 2u
#define HART_CANT_CHANGE_MODE 35u
#define HART_FIND_STOPS_FAILED 36u
#define HART_CAL_FAILED 37u
#define HART_DIAG_NOT_ALLOWED 38u
#define HART_TUNE_NOT_ALLOWED 39u
#define HART_READ_FILE_FAILED 40u
#define HART_CANT_START_DIAG  41u
#define HART_PST_NOT_ALLOWED 42u
#endif

//--------------------------------------------------

#define HART_DEVICE_FAILED 0x80U
#define HART_CONFIG_CHANGED 0x40U
#define HART_COLD_START 0x20U
#define HART_ADDL_STATUS 0x10U
#define HART_LOOPCURRENT_FIXED 0x08U
#define HART_PRIMARYVAR_OUT_OF_LIMITS 0x01U

#if 0 //Units Codes must be with their interface conversion definitions, H_...
//--------------------------------------------------
// Device Variable Units Codes (HART6-Spec 183, $5.2)

#define HART_PERCENT_CODE       57u
#define HART_CELSIUS_CODE       32u
//pressure units
#define HART_PSI_CODE            6u
#define HART_BAR_CODE            7u
#define HART_MBAR_CODE           8u
#define HART_PASCAL_CODE        11u //Pa
#define HART_KPASCAL_CODE       12u //kPa
//electric units
#define HART_VOLT_CODE          58u
#define HART_MVOLT_CODE         36u  //(mV)
#define HART_MAMP_CODE          39u  //(mA)
#define HART_TIME_CODE          51u  //(second)
//
#define HART_UNITSNONE_CODE     251u //counts, no units

//standard hart units for % and mA
#define HART_POS_UNITS_CODE 57u
#define HART_SIG_UNITS_CODE 39u

#endif //0

//----- Bad legacy stuff ------
#if 0 //Unused and too suspicious
#define NO_VALUE_FLOAT 0.0F
#define NO_VALUE_INT 0
#endif
//----- End Bad legacy stuff ------

//Public Functions

extern u8 hart_makeResponseFlags(bool_t masterFlag);

//Old
extern s8_least hart_BurstDispatch(u8_least cmd, u8 *dst);
extern s8_least hart_Dispatch(u8_least cmd, const u8 *src, u8 *dst, u8 req_data_len);

//New
extern s8_least hart6_BurstDispatch(u8_least cmd, u8 *dst);
extern s8_least hart6_Dispatch(u8_least cmd, const u8 *src, u8 *dst, u8 req_data_len);

//HART-spawned processes
//for ESD+, see hartpriv.h extern bool_t hart_SetProcessCommand(u8 procId);

//Helper functions

/** \brief Project-specific plugin to set device-specific status bits
\param partially assembled HART response byte 2
\return fully assembled HART response byte 2
*/
extern u8 hart_StatusPlugin(u8 rsp2);

//-------------- interfaces to common command handler ----------------
extern s8_least hart_CheckMode(u8 ModeFlag);



#ifdef OLD_NVRAM
//AP style extern void hart_procRamCommit(bool_t flag);  //flag is true iff cal data are initialized, too
//or, ESD style
void hart_procRamCommit_WithCal(void);
void hart_procRamCommit_NoCal(void);
#else
//extern enum procresult_t hart_procRamCommit(s16 *procdetails); //factory stuff, not really HART
#endif

#endif //HARTFUNC_H_
/* This line marks the end of the source */
