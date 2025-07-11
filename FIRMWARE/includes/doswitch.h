/**
    \file doswitch.h
    \brief Public API header for project-independent DO switches handling

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/FIRMWARE/includes/doswitch.h $
    $Date: 3/07/09 12:33p $
    $Revision: 4 $
    $Author: Arkkhasin $

    \ingroup sysio
*/
#ifndef DOSWITCH_H_
#define DOSWITCH_H_

#include "dimensions.h"
#include "errcodes.h"

#define SWITCH_DISABLE  0u
#define SWITCH_FAIL  1u
#define SWITCH_RESET  2u
#define SWITCH_POS_ERROR  3u
#define SWITCH_TIGHT  4u
#define SWITCH_POS_LOW  5u
#define SWITCH_POS_HIGH  6u
#define SWITCH_MANUAL  7u
#define SWITCH_PASSTHROUGH  8u
#define SWITCH_ALWAYSON  9u
#define SWITCH_ALWAYSOFF  10u
#define SWITCH_CUTOFF_HI  11u
#define SWITCH_CUTOFF_LO  12u

#define SWITCH_TYPE_MAX 12U

#define SWITCH_NORMAL_OPEN ((u8)1)
#define SWITCH_NORMAL_CLOSED ((u8)0)
#define SWITCH_DIRECTION_MAX ((u8)1)

typedef struct DOSWCOnf1_t
{
    u8  SwitchDirection;
    u8  SwitchType;
} DOSWCOnf1_t;

typedef struct SwitchConfiguration_t
{
    DOSWCOnf1_t swconf[DO_NUM_CHANNELS];
    u16 CheckWord;
} SwitchConfiguration_t;


typedef struct
{
    u8  bSwitch[DO_NUM_CHANNELS];
} SwitchReading_t;

typedef struct DO_Passthrough_t
{
    u8 state[DO_NUM_CHANNELS];
    u16 CheckWord;
} DO_Passthrough_t;

//--------------------------------------------------------------
// Bits in the 16-bit IPC buffer

#define DO_2_IPC_STATE                  (0x8000u)
#define DO_1_IPC_STATE                  (0x4000u)

//--------------------------------------------------------------

extern void dosw_SetPassthroughValues(const DO_Passthrough_t *src);

SAFEGET(cnfg_GetFillSwitchConfiguration, SwitchConfiguration_t);

SAFESET(cnfg_SetSwitchConfiguration, SwitchConfiguration_t);
//extern ErrorCode_t     cnfg_SetSwitchConfiguration(SwitchConfiguration_t* pSwitchConfiguration);

extern void dosw_GetSwitchReading(SwitchReading_t *dst);

extern void dosw_OutputSwitches(void);

#endif //DOSWITCH_H_
/* This line marks the end of the source */
