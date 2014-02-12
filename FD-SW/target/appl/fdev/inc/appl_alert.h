/*
Copyright 2012 by GE Energy / Dresser, Inc., as an unpublished trade secret. All rights reserved.

This document and all information herein are the property of GE Energy.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon demand.
*/

/**
    \file appl_alert.h
    \brief Position and other types of Alerts (Level monitors) headers

    CPU: Any

    OWNER: AP
*/

#ifndef APPL_ALERT_H__
#define APPL_ALERT_H__

#include <ptb.h>

//------------------------------------------------------------------
// Function API

u32         Execute_PTB_Alerts(const T_FBIF_BLOCK_INSTANCE * p_block_instance);

//------------------------------------------------------------------
// Get the state of the specific Alerta

bool_t      Alert_AirSupply(const T_FBIF_PTB *p_PTB);
bool_t      Alert_TravelDeviation(const T_FBIF_PTB *p_PTB);
bool_t      Alert_PositionHI(const T_FBIF_PTB *p_PTB);
bool_t      Alert_PositionLO(const T_FBIF_PTB *p_PTB);

//------------------------------------------------------------------
#include    "mnhart2ff.h"

#define     ALERT_LOG_NUM_ENTRIES   (32)

//----------------------------------------------------------------------------------------------
// Storage data structure
// The data in this structure are stored in NVRAM. The structure shall be checksum protected
// Organization -- circular buffer
//      -- NEW Record index that points to the OLDEST written entry
//      -- OLD Record index that points to the NEWEST written entry
typedef struct AlertStoreStruc_t
{
    u16             StoreBufferNewIndex;
    u16             StoreBufferOldIndex;
    u8              StoreDataSize;

    // Storage Array
    _ALERT_LOG      Buffer[ALERT_LOG_NUM_ENTRIES];

} AlertStoreStruc_t;

//----------------------------------------------------------------------------------------------

typedef enum AlertBufferControl_t
{
    ALERT_AUTO_INC,
    ALERT_NO_ACTION
} AlertBufferControl_t;

fferr_t     Alert_ReadAlertLogEntry(const T_FBIF_BLOCK_INSTANCE * p_block_instance, AlertBufferControl_t Action);
fferr_t     Alert_WriteAlertLogEntry(const T_FBIF_BLOCK_INSTANCE * p_block_instance);
void        Alert_InitAlertLog(const T_FBIF_BLOCK_INSTANCE * p_block_instance);

void        Alert_IPCStateMachineExecuted(void);

extern void get_AlertLogPtr(void** pp_value, u16* p_len);

extern void ff_AlertSPTimeout_Execute_fromFB(const T_FBIF_BLOCK_INSTANCE *p_block_instance, u16 Channel);

#endif /* APPL_ALERT_H__ */
