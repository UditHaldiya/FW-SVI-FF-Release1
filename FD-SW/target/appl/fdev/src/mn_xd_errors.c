/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file mn_xd_errors.c
    \brief functions about parameter XD_ERROR_POS, XD_ERROR_TEMPERATURE and XD_ERROR_PRESSURE

    CPU: Any

    OWNER: AK
    $Date: 08/27/12 11:19a $
    $Revision: 1 $
    $Author: stanley fu $

*/
#include <softing_base.h>
#include <ptb.h>

#include "mnhart2ff.h"
#include "faultdef.h"
#include "faultpublic.h"
#include "mn_xd_errors.h"
#include "mn_errorbits.h"

/* \brief update XD_ERROR_POS according to COMPLETE_STATUS
    param in:
        p_block_instance: pointer to TB function block
*/
fferr_t UpdateXDErrorPos(const T_FBIF_BLOCK_INSTANCE* p_block_instance)
{
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;

    if(
#ifdef FAULT_ACTUATOR
              mn_IsAppFault(p_PTB,FAULT_ACTUATOR)
#endif
#ifdef FAULT_IP_OUT_OF_RANGE
             || mn_IsAppFault(p_PTB,FAULT_IP_OUT_OF_RANGE)
#endif
#ifdef FAULT_KEYPAD
             || mn_IsAppFault(p_PTB,FAULT_KEYPAD)
#endif
             )
    {
        p_PTB->xd_error_pos = XD_ERROR_MECHANICAL_FAILURE;
    }
    else if(
#ifdef FAULT_SELF_CHECK
              mn_IsAppFault(p_PTB,FAULT_SELF_CHECK)
#endif
#ifdef FAULT_REF_VOLTAGE
             || mn_IsAppFault(p_PTB,FAULT_REF_VOLTAGE)
#endif
#ifdef FAULT_I2CBUS
             || mn_IsAppFault(p_PTB,FAULT_I2CBUS)
#endif
             )
    {
        p_PTB->xd_error_pos = XD_ERROR_ELECTRONICS_FAILURE;
    }
    else if (
#ifdef FAULT_RESET
               mn_IsAppFault(p_PTB,FAULT_RESET)
#endif
#ifdef FAULT_WATCHDOG_TIMEOUT
              || mn_IsAppFault(p_PTB,FAULT_WATCHDOG_TIMEOUT)
#endif
#ifdef FAULT_SOFTWARE
              || mn_IsAppFault(p_PTB,FAULT_SOFTWARE)
#endif
            )
    {
        p_PTB->xd_error_pos = XD_ERROR_SOFTWARE_ERROR;
    }
    else if(
#ifdef FAULT_NVM_TEST
              mn_IsAppFault(p_PTB,FAULT_NVM_TEST)
#endif
#ifdef FAULT_FACTORYWRITE
             || mn_IsAppFault(p_PTB,FAULT_FACTORYWRITE)
#endif
#ifdef FAULT_FW_CHECKSUM
             || mn_IsAppFault(p_PTB,FAULT_FW_CHECKSUM)
#endif
#ifdef FAULT_RAM_CHECKSUM
             || mn_IsAppFault(p_PTB,FAULT_RAM_CHECKSUM)
#endif
#ifdef FAULT_NVM_CHECKSUM0
             || mn_IsAppFault(p_PTB,FAULT_NVM_CHECKSUM0)
#endif
             )
    {
        p_PTB->xd_error_pos = XD_ERROR_DATA_INTEGRITY_ERROR;
    }
    else if(
#ifdef FAULT_POSITION_SENSOR
              mn_IsAppFault(p_PTB,FAULT_POSITION_SENSOR)
#endif
#ifdef FAULT_IPC_DISCONNECT
             || mn_IsAppFault(p_PTB,FAULT_IPC_DISCONNECT)
#endif
             )
    {
        p_PTB->xd_error_pos = XD_ERROR_I_O_FAILURE;
    }
    else if(
#ifdef FAULT_BIAS_OUT_OF_RANGE
         mn_IsAppFault(p_PTB,FAULT_BIAS_OUT_OF_RANGE)
#endif
      )
    {
        p_PTB->xd_error_pos = XD_ERROR_ALGORITHM_ERROR;
    }
    else if(
#ifdef FAULT_FIND_STOPS_FAILED
              mn_IsAppFault(p_PTB,FAULT_FIND_STOPS_FAILED)
#endif
#ifdef FAULT_CALIBRATION_FAILED
             || mn_IsAppFault(p_PTB,FAULT_CALIBRATION_FAILED)
#endif
             )
    {
        p_PTB->xd_error_pos = XD_ERROR_CALIBRATION_ERROR;
    }
    else if(
#ifdef FAULT_SETPOINT_TIMEOUT
              mn_IsAppFault(p_PTB,FAULT_SETPOINT_TIMEOUT)
#endif
#ifdef FAULT_FACTORYMODE
             || mn_IsAppFault(p_PTB,FAULT_FACTORYMODE)
#endif
#ifdef FAULT_SETPOINT_TIMEOUT_CRITICAL
             || mn_IsAppFault(p_PTB,FAULT_SETPOINT_TIMEOUT_CRITICAL)
#endif
#ifdef FAULT_POSITION_ERROR
             || mn_IsAppFault(p_PTB,FAULT_POSITION_ERROR)
#endif
             )
    {
        p_PTB->xd_error_pos = XD_ERROR_GENERAL_ERROR;
    }
    else
    {
        p_PTB->xd_error_pos = XD_ERROR_NO_ERROR;
    }

    return E_OK;
}

/* \brief update XD_ERROR_PRESSURE according to COOMPLETE_STATUS
    param in:
        p_block_instance: pointer to TB function block
*/
fferr_t UpdateXDErrorPressure(const T_FBIF_BLOCK_INSTANCE* p_block_instance)
{
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;

    if(
#ifdef FAULT_PRESSURE1
       mn_IsAppFault(p_PTB, FAULT_PRESSURE1)
#endif
#ifdef FAULT_PRESSURE2
       || mn_IsAppFault(p_PTB,FAULT_PRESSURE2)
#endif
#ifdef FAULT_PRESSURE3
       || mn_IsAppFault(p_PTB,FAULT_PRESSURE3)
#endif
#ifdef FAULT_PRESSURE4
       || mn_IsAppFault(p_PTB,FAULT_PRESSURE4)
#endif
#ifdef FAULT_PRESSURE5
       || mn_IsAppFault(p_PTB,FAULT_PRESSURE5)
#endif
#ifdef FAULT_AIR_SUPPLY_LOW
       || mn_IsAppFault(p_PTB,FAULT_AIR_SUPPLY_LOW)
#endif
       )
    {
        p_PTB->xd_error_pressure = XD_ERROR_I_O_FAILURE;
    }
    else
    {
        p_PTB->xd_error_pressure = XD_ERROR_NO_ERROR;
    }

    return E_OK;
}

/* \brief update XD_ERROR_TEMPERATURE according to COMPLETE_STATUS
    param in:
        p_block_instance: pointer to TB function block
*/
fferr_t UpdateXDErrorTemperature(const T_FBIF_BLOCK_INSTANCE* p_block_instance)
{
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;

    if(
#ifdef FAULT_TEMPR_OUT_OF_RANGE
        mn_IsAppFault(p_PTB,FAULT_TEMPR_OUT_OF_RANGE)
#endif
#ifdef FAULT_TEMPERATURE_SENSOR
       || mn_IsAppFault(p_PTB,FAULT_TEMPERATURE_SENSOR)
#endif
       )
    {
        p_PTB->xd_error_temperature = XD_ERROR_I_O_FAILURE;
    }
    else
    {
        p_PTB->xd_error_temperature = XD_ERROR_NO_ERROR;
    }

    return E_OK;
}


/* \brief When the FIND_STOPS fault exists
          update BLOCK_ERR_DESC_1 parameter with the ERRB_ERROR_FIND_STOPS bit
    param in:
        p_block_instance: pointer to TB function block
*/
fferr_t Update_FIND_STOPS_Error_bit(const T_FBIF_BLOCK_INSTANCE* p_block_instance)
{
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;

    if ( mn_IsAppFault(p_PTB,FAULT_FIND_STOPS_FAILED) )
    {
        mn_ErrorBits_Set(ERRB_ERROR_FIND_STOPS, true);
    }
    else
    {
        mn_ErrorBits_Set(ERRB_ERROR_FIND_STOPS, false);
    }

    return E_OK;
}

/* This line marks the end of the source */
