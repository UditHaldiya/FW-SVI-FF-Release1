/*
Copyright 2010 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file hart_posctl.c
    \brief Glue functions related to position control HART support

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/glue/hart_posctl.c $
    $Date: 1/20/12 10:48a $
    $Revision: 10 $
    $Author: Arkkhasin $

    \ingroup HARTapp
*/

#include "mnwrap.h"
#include "errcodes.h"
#include "ctllimits.h"
#include "hartfunc.h"
#include "hartdef.h"
#include "bufutils.h"
#include "fpconvert.h"
#include "devicemode.h"
#include "faultpublic.h"
#include "faultdef.h"
#include "control.h"
#include "configure.h"

// Digital SP
#include "digitalsp.h"

// IPC Definitions
#include "ipcdefs.h"

// Variable IDs
#include "ipcvarids.h"
#include "ff_resblockvars.h"
#include "ff_aoblockvars.h"
#include "ff_doblockvars.h"
#include "ff_pidblockvars.h"
#include "ff_tbblockvars.h"
#include "ff_aiblockvars.h"
#include "ff_ai2blockvars.h"
#include "ff_ai3blockvars.h"
#include "ff_do2blockvars.h"
#include "ff_isblockvars.h"
#include "ff_pid2blockvars.h"
#include "ff_devicevars.h"
#include "ff_positionConfVars.h"

// Characterization
#include "poscharact.h"

#include "cycle.h"

// Switches
#include "dohwif.h"
#include "doswitch.h"
#include "key.h"
#include "wprotect.h"

// IP Current
#include "ipcurr.h"
//--------------------------------------------------------------

typedef struct IPC_IPVariable_t
{
    u8  IPCVarStatus;
    u8  IPAction;

} IPC_IPVariable_t;

//---------------------------------------------------
// Transltaion table

typedef struct IPC_TranslationTable_t
{
    u8              VarType;
    ErrorCode_t     (* const pIPC_ReadVar)(IPC_Variable_IDs_t VarID, IPC_ReadPtrs_t const *pIPC_ReadPtrs);
    ErrorCode_t     (* const pIPC_WriteVar)(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs);

} IPC_TranslationTable_t;

//--------------------------------------------------------------
// Mode handling stub

/**
\brief This function returns the actual mode
*/
static u8      IPC_GetActualMode(void)
{
    return digsp_GetExternalMode();
}

//--------------------------------------------------------------
// Variable Translation Map

static ErrorCode_t  IPC_ReadNone(IPC_Variable_IDs_t VarID, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    util_PutFloat(pIPC_ReadPtrs->pIPC_VarBuffer, 0.0F);
    util_PutU8(pIPC_ReadPtrs->pVarStatus, (IPC_QUALITY_GOOD | IPC_VARID_NA));
    UNUSED_OK(VarID);
    return ERR_OK;
}

static ErrorCode_t  IPC_WriteNone(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    UNUSED_OK(VarID);
    UNUSED_OK(pIPC_WritePtrs);
    util_PutFloat(pIPC_ReadPtrs->pIPC_VarBuffer, 0.0F);
    util_PutU8(pIPC_ReadPtrs->pVarStatus, (IPC_QUALITY_GOOD | IPC_VARID_NA));
    return ERR_OK;
}

static ErrorCode_t  IPC_ReadDiscreteSwitch(IPC_Variable_IDs_t VarID, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    u32                     SwitchValue;
    u8                      RetStatus = IPC_QUALITY_GOOD | IPC_NO_ERROR;
    SwitchConfiguration_t   SwitchConfiguration;
    SwitchReading_t         SwitchReading;

    dosw_GetSwitchReading(&SwitchReading);

    (void)cnfg_GetFillSwitchConfiguration(&SwitchConfiguration);
    if (cnfg_GetOptionConfigFlag(DO_OPTION))
    {
        switch (VarID)
        {
            case IPC_DISCRETE_SWITCH_1 :
                SwitchValue = (u32)SwitchReading.bSwitch[DO_1];
                break;

            case IPC_DISCRETE_SWITCH_2 :
                SwitchValue = (u32)SwitchReading.bSwitch[DO_2];
                break;

            default :
                SwitchValue = 0u;
                RetStatus = IPC_QUALITY_BAD | IPC_DATA_OUT_OF_ENUM;
                break;
        }
    }
    else
    {
        SwitchValue = 0u;
        RetStatus = IPC_QUALITY_BAD | IPC_VALUE_NOTAVAIL;
    }

    util_PutU32(pIPC_ReadPtrs->pIPC_VarBuffer, SwitchValue);
    util_PutU8(pIPC_ReadPtrs->pVarStatus, RetStatus);
    return ERR_OK;
}

static ErrorCode_t  IPC_ReadDiscreteInput(IPC_Variable_IDs_t VarID, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    u32     InputValue = 0u;

    // BIOS function returns TRUE for OPEN/HI, FALSE for CLOSED/LO
    //  -- -- -- -- -- -- --  1                 0
    if (bios_ReadDiState())
    {
        InputValue |= IPC_BIT_DI_STATE;
    }

    // BIOS function returns TRUE for LOCKED, FALSE for UNLOCKED
    //     WP Jumper            INSTALLED       NOT INSTALLED
    //  -- -- -- -- -- -- -- --    1                 0
    if (bios_ReadWriteProtectInput())
    {
        InputValue |= IPC_BIT_WP_STATE;
    }

    UNUSED_OK(VarID);
    util_PutU32(pIPC_ReadPtrs->pIPC_VarBuffer, InputValue);
    util_PutU8(pIPC_ReadPtrs->pVarStatus, IPC_QUALITY_GOOD | IPC_NO_ERROR);
    return ERR_OK;
}

#define     MAX_FAULT_IPC_BUFFER_SIZE_U32           (3u)

static ErrorCode_t  IPC_ReadCompleteStatus(IPC_Variable_IDs_t VarID, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    u32     StatusBuffer[MAX_FAULT_IPC_BUFFER_SIZE_U32];        // 3 32-bit words
    u32     HistoryBuffer[MAX_FAULT_IPC_BUFFER_SIZE_U32];       // 3 32-bit words
    u32     DataToSend;
    u8      RetStatus = IPC_QUALITY_GOOD | IPC_NO_ERROR;
    const   FaultCodeData_t* pFaultCodeData;

    //get the fault code data
    pFaultCodeData = error_GetFaultCodeData(NULL);

    //we must report all of the faults
    CONST_ASSERT(FAULT_CODE_BYTES_REQUIRED <= sizeof(StatusBuffer));
    CONST_ASSERT(FAULT_CODE_BYTES_REQUIRED <= sizeof(HistoryBuffer));
    CONST_ASSERT(sizeof(StatusBuffer) >= sizeof(pFaultCodeData->fcode));
    CONST_ASSERT(sizeof(HistoryBuffer) >= sizeof(pFaultCodeData->fcodeHistory));

    mn_memset(StatusBuffer, 0, sizeof(StatusBuffer));
    mn_memset(HistoryBuffer, 0, sizeof(HistoryBuffer));

    //put the data in the buffer
    mn_memcpy(StatusBuffer, pFaultCodeData->fcode, sizeof(pFaultCodeData->fcode));
    mn_memcpy(HistoryBuffer, pFaultCodeData->fcodeHistory, sizeof(pFaultCodeData->fcodeHistory));

    switch (VarID)
    {
        case IPC_COMPLETE_STATUS_CUR_LO :
            DataToSend = StatusBuffer[0];
            break;

        case IPC_COMPLETE_STATUS_CUR_HI :
            DataToSend = StatusBuffer[1];
            break;

        case IPC_COMPLETE_STATUS_CUR_HIHI :
            DataToSend = StatusBuffer[2];
            break;

        case IPC_COMPLETE_STATUS_HIST_LO :
            DataToSend = HistoryBuffer[0];
            break;

        case IPC_COMPLETE_STATUS_HIST_HI :
            DataToSend = HistoryBuffer[1];
            break;

       case IPC_COMPLETE_STATUS_HIST_HIHI :
            DataToSend = HistoryBuffer[2];
            break;

        default :
            DataToSend = 0u;
            RetStatus = IPC_QUALITY_BAD | IPC_DATA_OUT_OF_ENUM;
            break;
    }

    util_PutU32(pIPC_ReadPtrs->pIPC_VarBuffer, DataToSend);
    util_PutU8(pIPC_ReadPtrs->pVarStatus, RetStatus);

    return ERR_OK;
}

static u32  IPC_TestVariable = 0u;

static ErrorCode_t  IPC_ReadTest(IPC_Variable_IDs_t VarID, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    util_PutU32(pIPC_ReadPtrs->pIPC_VarBuffer, IPC_TestVariable);
    util_PutU8(pIPC_ReadPtrs->pVarStatus, (IPC_QUALITY_GOOD | IPC_NO_ERROR));
    UNUSED_OK(VarID);
    return ERR_OK;
}

static ErrorCode_t  IPC_WriteTest(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    u8          ReturnStatus = IPC_QUALITY_GOOD | IPC_NO_ERROR;

    if (IsQuality_ACCEPT(*(pIPC_WritePtrs->pVarStatus)))
    {
        IPC_TestVariable = util_GetU32(pIPC_WritePtrs->pIPC_VarBuffer);
    }
    else
    {
        ReturnStatus = *pIPC_WritePtrs->pVarStatus;
        IPC_TestVariable = 0u;
    }

    util_PutU32(pIPC_ReadPtrs->pIPC_VarBuffer, IPC_TestVariable);
    util_PutU8(pIPC_ReadPtrs->pVarStatus, ReturnStatus);
    UNUSED_OK(VarID);
    return ERR_OK;
}

//-----------------------------------------------------------------------------------------------------
// This variable cotains the Working Position -- the position BEFORE the inverse characterizartion
// This variable is periodically when the ACTUAL position is obtained.
// Note, that the ACTUAL position is ALWAYS periodically obtained, and the the actual position
// always retrieved BEFORE the working position could be read.

typedef struct Variable_WorkingPosition_t
{
    s32     Value;
    u8      Status;

} Variable_WorkingPosition_t;

static Variable_WorkingPosition_t Variable_WorkingPosition =
{
    .Value  = 0u,
    .Status = IPC_QUALITY_BAD | IPC_DATA_INCONSISTENT
};

//-----------------------------------------------------------------------------------------------------

static ErrorCode_t  IPC_ReadWorkingPos(IPC_Variable_IDs_t VarID, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    (void)fpconvert_IntToFloatBuffer(Variable_WorkingPosition.Value, UNITSID_POSITION, pIPC_ReadPtrs->pIPC_VarBuffer);
    util_PutU8(pIPC_ReadPtrs->pVarStatus, Variable_WorkingPosition.Status);
    UNUSED_OK(VarID);
    return ERR_OK;
}

static ErrorCode_t  IPC_ReadRawPosition(IPC_Variable_IDs_t VarID, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    s32     RawPos;

    RawPos = vpos_GetRawPosition();
    util_PutS32(pIPC_ReadPtrs->pIPC_VarBuffer, RawPos);

    // Reading RAW position sensor ALWAYS sends GOOD status
    util_PutU8(pIPC_ReadPtrs->pVarStatus, IPC_QUALITY_GOOD | IPC_NO_ERROR);
    UNUSED_OK(VarID);
    return ERR_OK;
}

static ErrorCode_t  IPC_ReadPosition(IPC_ReadPtrs_t const *pIPC_ReadPtrs, ErrorCode_t sperror)
{
    pos_least_t Position;
    u8          PosStatus;
    u8          ActualMode;
    bool_t      PosError;

    PosError = error_IsFault(FAULT_POSITION_SENSOR);

    if (!PosError)
    {   // No Errors!
        Position  = vpos_GetScaledPosition();
        Variable_WorkingPosition.Value = Position;          // Have the WORKING position

        Position  = poscharact_Inverse(Position);           //inverse characterization
        PosStatus = IPC_QUALITY_GOOD | IPC_NO_ERROR;

        //Check if position is limited for HI/LO CUTOFF (TFS:13885)
        if ( error_IsFault(FAULT_POS_CUTOFF_LO) )
        {
            PosStatus |= IPC_STATUS_LIMITED_LOWER;
        }
        if ( error_IsFault(FAULT_POS_CUTOFF_HI) )
        {
            PosStatus |= IPC_STATUS_LIMITED_UPPER;
        }

        // Now check the Mode
        if(sperror == ERR_UPPERLIM)
        {
            PosStatus |= IPC_STATUS_LIMITED_UPPER;
        }
        if(sperror == ERR_LOWERLIM)
        {
            PosStatus |= IPC_STATUS_LIMITED_LOWER;
        }

        ActualMode  = IPC_GetActualMode();
        // if ((ActualMode == IPC_MODE_MANUAL) || (ActualMode == IPC_MODE_LOVERRIDE))
        if (ActualMode != IPC_MODE_AUTO)
        {   // Not AUTO mode - indicate that the position is limited on BOTH ends
            PosStatus |= IPC_STATUS_LIMITED;
        }
        else
        {
            ctlmode_t cltmode; // **effective** control mode
            control_GetControlMode(&cltmode, NULL);
            if(cltmode != CONTROL_MANUAL_POS)
            {
                PosStatus |= IPC_STATUS_LIMITED;
            }
        }
    }
    else
    {
        Position  = 0;
        Variable_WorkingPosition.Value = Position;          // Have the WORKING position

        PosStatus = IPC_QUALITY_BAD | IPC_DATA_INCONSISTENT;
    }

    Variable_WorkingPosition.Status = PosStatus;

    (void)fpconvert_IntToFloatBuffer(Position, UNITSID_POSITION, pIPC_ReadPtrs->pIPC_VarBuffer);
    util_PutU8(pIPC_ReadPtrs->pVarStatus, PosStatus);

    return ERR_OK;
}

//-----------------------------------------------------------------------------------------------------
static ErrorCode_t  IPC_ReadWorkingSP(IPC_Variable_IDs_t VarID, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{

    // Currently,
    // The Variable_WorkingSP.Value  contains previous limited SP
    // The Variable_WorkingSP.Status contains the status from the Write SP operation.
    // The Workig P actually should be retrieved from the Control, via the existing interface
    s32 sp;
    ctlmode_t ctlmode = mode_GetEffectiveControlMode(&sp); //same as control uses

    (void)fpconvert_IntToFloatBuffer(sp, UNITSID_POSITION, pIPC_ReadPtrs->pIPC_VarBuffer);
    u8 status;
    if(sp == SETPOINT_INVALID)
    {
        //can only be momentarily before modeguard runs but still...
        status = IPC_QUALITY_BAD | IPC_VALUE_NOTAVAIL;
    }
    else if(ctlmode == CONTROL_MANUAL_POS) //closed-loop
    {
        status = IPC_QUALITY_GOOD | IPC_NO_ERROR;
    }
    else
    {
        //open loop control mode - hmm... what about cut-off cases
        status = IPC_QUALITY_GOOD | IPC_STATUS_LIMITED;
    }



    util_PutU8(pIPC_ReadPtrs->pVarStatus, status);
    UNUSED_OK(VarID);
    return ERR_OK;
}

static ErrorCode_t  IPC_WriteSetPoint(u8 mode, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    ErrorCode_t retval = ERR_OK;
    s32         nSetpoint = 0;
    u8          ReturnStatus = IPC_QUALITY_GOOD | IPC_NO_ERROR;

    // Check the Variable quality and write ithe var ONLY if the quality is accepteable!
    if (IsQuality_ACCEPT(*(pIPC_WritePtrs->pVarStatus)))
    {   // OK, Write the data
        nSetpoint = fpconvert_FloatBufferToInt32Clamped(pIPC_WritePtrs->pIPC_VarBuffer, UNITSID_PERCENT, digitalsp_range);
        retval    = digsp_SetDigitalSetpointEx(mode, nSetpoint);
        if (retval == ERR_INVALID_PARAMETER)
        {
            ReturnStatus = IPC_QUALITY_BAD | IPC_DATA_INCONSISTENT;
        }
    }
    else
    {   // Not OK. Set the mode.
        (void)digsp_SetDigitalSetpointEx(mode, SETPOINT_INVALID);

        // Quality is not acceptable; signal back.
        ReturnStatus = *pIPC_WritePtrs->pVarStatus;
    }

    util_PutFloat(pIPC_ReadPtrs->pIPC_VarBuffer, 0.0F); //hmm... what is it?
    util_PutU8(pIPC_ReadPtrs->pVarStatus, ReturnStatus);
    return retval;
}

//-----------------------------------------------------------------------------------------------------

static ErrorCode_t  IPC_ReadIPCurrent(IPC_Variable_IDs_t VarID, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    s32     IPCurrent;

    IPCurrent = (s32)ipcurr_GetCompensatedIpCurrent();
    util_PutS32(pIPC_ReadPtrs->pIPC_VarBuffer, IPCurrent);
    util_PutU8(pIPC_ReadPtrs->pVarStatus, IPC_QUALITY_GOOD | IPC_NO_ERROR);

    UNUSED_OK(VarID);
    return ERR_OK;
}

static ErrorCode_t  IPC_ReadPressure(IPC_Variable_IDs_t VarID, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    ErrorCode_t     retval = ERR_OK;
    u8              VarStatus;

    u8              PressureIndex   = PRESSURE_MAIN_INDEX;

    bool_t          BadIndex = false;

    pres_t          Pressure = PRESSURE_INVALID;

    // Figure out what we need to do
    switch (VarID)
    {
        case IPC_PRESSURE_SUPPLY :
            PressureIndex   = PRESSURE_SUPPLY_INDEX;
            break;

        case IPC_PRESSURE_ACTUATOR_A :
            PressureIndex   = PRESSURE_ACT1_INDEX;
            break;

        case IPC_PRESSURE_ACTUATOR_B :
            PressureIndex   = PRESSURE_ACT2_INDEX;
            break;

        case IPC_PRESSURE_ATMOS :
            PressureIndex   = PRESSURE_ATM_INDEX;
            break;

        case IPC_PRESSURE_PILOT :
            PressureIndex   = PRESSURE_PILOT_INDEX;
            break;

        default :
            BadIndex  = true;
            break;
    }

    if (BadIndex)
    {   // No such varID
        VarStatus = IPC_QUALITY_BAD | IPC_BAD_INDEX;
    }
    else
    {
        Pressure = pres_GetPressureData()->Pressures[PressureIndex];
        if(Pressure == PRESSURE_INVALID)
        {
            VarStatus = IPC_QUALITY_BAD | IPC_DATA_INCONSISTENT;
        }
        else
        {
            VarStatus     = IPC_QUALITY_GOOD | IPC_NO_ERROR;
        }
    }

    (void)fpconvert_IntToFloatBuffer(Pressure, UNITSID_PRESSURE, pIPC_ReadPtrs->pIPC_VarBuffer); // convertinto the buffer
    util_PutU8(pIPC_ReadPtrs->pVarStatus, VarStatus);
    return retval;
}

static ErrorCode_t  IPC_ReadPressureUnits(IPC_Variable_IDs_t VarID, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    u32     presUnitsSupply;
    u32     TempBuffer;

    presUnitsSupply = (u32)fpconvert_IntToFloatBuffer(0, UNITSID_PRESSURE, (void *)&TempBuffer);     // Get the Pressure Units
    util_PutU32(pIPC_ReadPtrs->pIPC_VarBuffer, presUnitsSupply);
    util_PutU8(pIPC_ReadPtrs->pVarStatus, IPC_QUALITY_GOOD);
    UNUSED_OK(VarID);
    return ERR_OK;
}

static ErrorCode_t  IPC_WritePressureUnits(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    // this function block need further efforts
    u32         presUnitsSupplyToWrite;
    u32         presUnitsSupply;
    u8          ReturnStatus = IPC_QUALITY_GOOD;
    ErrorCode_t Errorcode;

    // Get TARGET  Pressure units
    presUnitsSupplyToWrite = util_GetU32(pIPC_WritePtrs->pIPC_VarBuffer);

    // Get CURRENT Pressure units
    presUnitsSupply = fpconvert_GetUnits(UNITSID_PRESSURE_ENTRY);

    // Check the Variable quality and write ithe var ONLY if the quality is accepteable!
    if (IsQuality_ACCEPT(*(pIPC_WritePtrs->pVarStatus)))
    {   // OK, Write the data

        // Note: the cast is to simplify the code --
        // the pressure units (8-bit value) will occupy 32-bit field in the buffer
        Errorcode = fpconvert_SetBuiltinUnits(UNITSID_PRESSURE, (u8)presUnitsSupplyToWrite);
        // Get the ACTUAL pressure units
        presUnitsSupply = fpconvert_GetUnits(UNITSID_PRESSURE_ENTRY);

        if ((Errorcode       != ERR_OK) ||
            (presUnitsSupply != presUnitsSupplyToWrite))
        {
            ReturnStatus = IPC_QUALITY_BAD | IPC_DATA_VALIDATION_FAILED;
        }
    }
    else
    {   // Quality is not acceptable. DO not write the data, signal back.
        ReturnStatus = *pIPC_WritePtrs->pVarStatus;
        presUnitsSupply = 0u;       // Return value 0 as specified.
    }

    util_PutU32(pIPC_ReadPtrs->pIPC_VarBuffer, presUnitsSupply);
    util_PutU8(pIPC_ReadPtrs->pVarStatus, ReturnStatus);

    UNUSED_OK(VarID);
    return ERR_OK;
}

static ErrorCode_t  IPC_ReadTemperature(IPC_Variable_IDs_t VarID, IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    // FP Temperature, similar to cmd 143.
    BoardTemperature_t TemperatureData;
    u8      TempStatus;
    bool_t  TempError;

    TempError = error_IsFault(FAULT_TEMPERATURE_SENSOR);
    if (!TempError)
    {
        (void)tempr_GetTemperature(TEMPR_MAINBOARD, &TemperatureData);
        (void)fpconvert_IntToFloatBuffer(TemperatureData.nBoardTemperature, UNITSID_TEMPR, pIPC_ReadPtrs->pIPC_VarBuffer);
        TempStatus = IPC_QUALITY_GOOD | IPC_NO_ERROR;
    }
    else
    {
        util_PutFloat(pIPC_ReadPtrs->pIPC_VarBuffer, 0.0F);
        TempStatus = IPC_QUALITY_BAD | IPC_DATA_INCONSISTENT;
    }

    util_PutU8(pIPC_ReadPtrs->pVarStatus, TempStatus);

    UNUSED_OK(VarID);
    return ERR_OK;
}

//--------------------------------------------------------------
// The translation table

#define IPC_TRANSLATION_TABLE_SIZE         (IPC_VARID_MAX)

static const IPC_TranslationTable_t IPC_TranslationTable[IPC_TRANSLATION_TABLE_SIZE] =
{
    [IPC_VARIABLE_ID_NOTUSED]       = {IPC_SHORT_VAR,   IPC_ReadNone,               IPC_WriteNone},

    //--------------------------------------------------------------
    [IPC_RB_BLOCK_TAG]              = {IPC_LONG_VAR,    NULL,                       IPC_WriteRbTag},
    [IPC_RB_BLOCK_ACTUAL_MODE]      = {IPC_SHORT_VAR,   NULL,                       IPC_WriteRbModeActual},
    [IPC_RB_BLOCK_ERROR]            = {IPC_SHORT_VAR,   NULL,                       IPC_WriteRbError},
    [IPC_RB_RESTART]                = {IPC_SHORT_VAR,   IPC_ReadRbRestart,          IPC_WriteRbRestart},
    [IPC_RB_NV_CYCLE_T]             = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_RB_BLOCK_ERR_DESC_1]       = {IPC_SHORT_VAR,   IPC_ReadRbBlockErrDesc1,    NULL},
    [IPC_RB_WRONG_CFG]              = {IPC_SHORT_VAR,   IPC_ReadRbWrongCfg,         NULL},

    [IPC_RB_FAULT_STATE]            = {IPC_SHORT_VAR,   IPC_ReadFaultState,         NULL},
    [IPC_RB_SET_FSTATE]             = {IPC_SHORT_VAR,   IPC_ReadSetFaultState,      IPC_WriteSetFaultState},
    [IPC_RB_CLR_FSTATE]             = {IPC_SHORT_VAR,   IPC_ReadClearFaultState,    IPC_WriteClearFaultState},

    //--------------------------------------------------------------
    [IPC_TB_BLOCK_TAG]              = {IPC_LONG_VAR,    NULL,                       IPC_WriteTBTag},
    [IPC_TB_BLOCK_ACTUAL_MODE]      = {IPC_SHORT_VAR,   NULL,                       IPC_WriteTBMode},
    [IPC_TB_BLOCK_TARGET_MODE]      = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_TB_FINAL_POS_VALUE]        = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_TB_FINAL_VALUE]            = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_APP_MODE]                  = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_TB_BLOCK_ERROR]            = {IPC_SHORT_VAR,   NULL,                       IPC_WriteTBError},
    [IPC_TB_ST_REV]                 = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_TB_FINAL_SP]               = {IPC_LONG_VAR,   NULL,                        IPC_WriteTBFinalSP},
    [IPC_TB_FINAL_POS]              = {IPC_LONG_VAR,   NULL,                        IPC_WriteTBFinalPOS},
    [IPC_TB_UI_CUST_CONF_0]         = {IPC_SHORT_VAR,   NULL,                       IPC_WriteTBUICustConfig0},
    [IPC_TB_UI_CUST_CONF_1]         = {IPC_SHORT_VAR,   NULL,                       IPC_WriteTBUICustConfig1},

    //--------------------------------------------------------------
    [IPC_AO_BLOCK_TAG]              = {IPC_LONG_VAR,    NULL,                       IPC_WriteAOTag},
    [IPC_AO_BLOCK_ACTUAL_MODE]      = {IPC_SHORT_VAR,   NULL,                       IPC_WriteAOMode},
    [IPC_AO_BLOCK_ERROR]            = {IPC_SHORT_VAR,   NULL,                       IPC_WriteAOError},
    [IPC_AO_SP]                     = {IPC_LONG_VAR,   NULL,                       IPC_WriteAOSP},

    //--------------------------------------------------------------
    [IPC_AI_OUT]                    = {IPC_LONG_VAR,   NULL,                       IPC_WriteAIOUT},
    [IPC_AI2_OUT]                   = {IPC_LONG_VAR,   NULL,                       IPC_WriteAI2OUT},

    //--------------------------------------------------------------
    [IPC_DO_BLOCK_TAG]              = {IPC_LONG_VAR,    NULL,                       IPC_WriteDOTag},
    [IPC_DO_BLOCK_ACTUAL_MODE]      = {IPC_SHORT_VAR,   NULL,                       IPC_WriteDOMode},
    [IPC_DO_BLOCK_ERROR]            = {IPC_SHORT_VAR,   NULL,                       IPC_WriteDOError},
    [IPC_DO_SP_D]                   = {IPC_SHORT_VAR,   NULL,                       IPC_WriteDOSPD},

    //--------------------------------------------------------------
    [IPC_PID_BLOCK_TAG]             = {IPC_LONG_VAR,    NULL,                       IPC_WritePIDTag},
    [IPC_PID_BLOCK_ACTUAL_MODE]     = {IPC_SHORT_VAR,   NULL,                       IPC_WritePIDMode},
    [IPC_PID_BLOCK_ERROR]           = {IPC_SHORT_VAR,   NULL,                       IPC_WritePIDError},
    [IPC_PID_PV]                    = {IPC_LONG_VAR,   NULL,                       IPC_WritePIDPV},
    [IPC_PID_SP]                    = {IPC_LONG_VAR,   NULL,                       IPC_WritePIDSP},
    [IPC_PID_OUT]                   = {IPC_LONG_VAR,   NULL,                       IPC_WritePIDOUT},

    //--------------------------------------------------------------
    [IPC_DEVICE_ID]                 = {IPC_LONG_VAR,    NULL,                       IPC_WriteDeviceID},
    [IPC_DEVICE_TAG]                = {IPC_LONG_VAR,    NULL,                       IPC_WriteDeviceTag},
    [IPC_DEVICE_ADDRESS]            = {IPC_SHORT_VAR,   NULL,                       IPC_WriteDeviceAddress},

    //--------------------------------------------------------------
    [IPC_XD_ERROR_POSITION]         = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_POS_CAL_RESULT]            = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_CAL_POINT_HI]              = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_CAL_POINT_LO]              = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_CAL_TYPE]                  = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_WORKING_SP]                = {IPC_SHORT_VAR,   IPC_ReadWorkingSP,          NULL},
    [IPC_WORKING_POS]               = {IPC_SHORT_VAR,   IPC_ReadWorkingPos,         NULL},

    [IPC_FINAL_CUTOFF_HI_ENABLE]    = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_FINAL_CUTOFF_HI_POINT]     = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_FINAL_CUTOFF_LO_ENABLE]    = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_FINAL_CUTOFF_LO_POINT]     = {IPC_SHORT_VAR,   NULL,                       NULL},

    //--------------------------------------------------------------
    [IPC_XD_FSTATE_OPT]             = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_FSTATE_VALUE]              = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_FSTATE_TIME]               = {IPC_SHORT_VAR,   NULL,                       NULL},

    [IPC_CHARACT_NUMPOINTS]         = {IPC_SHORT_VAR,   NULL,                       NULL},

    //--------------------------------------------------------------
    [IPC_DISCRETE_SWITCH_1]         = {IPC_SHORT_VAR,   IPC_ReadDiscreteSwitch,     NULL},
    [IPC_DISCRETE_SWITCH_2]         = {IPC_SHORT_VAR,   IPC_ReadDiscreteSwitch,     NULL},

    //--------------------------------------------------------------
    [IPC_TRVL_ACCUM_A]              = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_TRVL_ACCUM_B]              = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_TRVL_ACCUM_A_DEADBAND]     = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_TRVL_ACCUM_B_DEADBAND]     = {IPC_SHORT_VAR,   NULL,                       NULL},

    [IPC_TRVL_ACCTR_TODAY]          = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_TRVL_ACCTR_DAY1]           = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_TRVL_ACCTR_DAY2]           = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_TRVL_ACCTR_DAY3]           = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_TRVL_ACCTR_DAY4]           = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_TRVL_ACCTR_DAY5]           = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_TRVL_ACCTR_DAY6]           = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_TRVL_ACCTR_DAY7]           = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_TRVL_ACCTR_CUR15]          = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_TRVL_ACCTR_LAST15]         = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_TRVL_ACCTR_PREV15]         = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_TRVL_ACCTR_CURMONTH]       = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_TRVL_ACCTR_MONTH1]         = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_TRVL_ACCTR_MONTH2]         = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_TRVL_ACCTR_MONTH3]         = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_TRVL_ACCTR_MONTH4]         = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_TRVL_ACCTR_MONTH5]         = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_TRVL_ACCTR_MONTH6]         = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_TRVL_ACCTR_MONTH7]         = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_TRVL_ACCTR_MONTH8]         = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_TRVL_ACCTR_MONTH9]         = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_TRVL_ACCTR_MONTH10]        = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_TRVL_ACCTR_MONTH11]        = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_TRVL_ACCTR_MONTH12]        = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_TRVL_ACCTR_CUR12M]         = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_TRVL_ACCTR_YEAR1]          = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_TRVL_ACCTR_YEAR2]          = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_TRVL_ACCTR_YEAR3]          = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_TRVL_ACCTR_YEAR4]          = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_TRVL_ACCTR_YEAR5]          = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_TRVL_ACCTR_TOTAL]          = {IPC_SHORT_VAR,   NULL,                       NULL},

    //--------------------------------------------------------------
    [IPC_CYCLE_CTR_A]               = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_CYCLE_CTR_B]               = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_CYCLE_CTR_A_DEADBAND]      = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_CYCLE_CTR_B_DEADBAND]      = {IPC_SHORT_VAR,   NULL,                       NULL},

    [IPC_CYCLETR_TODAY]             = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_CYCLETR_DAY1]              = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_CYCLETR_DAY2]              = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_CYCLETR_DAY3]              = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_CYCLETR_DAY4]              = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_CYCLETR_DAY5]              = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_CYCLETR_DAY6]              = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_CYCLETR_DAY7]              = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_CYCLETR_CUR15]             = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_CYCLETR_LAST15]            = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_CYCLETR_PREV15]            = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_CYCLETR_CURMONTH]          = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_CYCLETR_MONTH1]            = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_CYCLETR_MONTH2]            = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_CYCLETR_MONTH3]            = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_CYCLETR_MONTH4]            = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_CYCLETR_MONTH5]            = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_CYCLETR_MONTH6]            = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_CYCLETR_MONTH7]            = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_CYCLETR_MONTH8]            = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_CYCLETR_MONTH9]            = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_CYCLETR_MONTH10]           = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_CYCLETR_MONTH11]           = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_CYCLETR_MONTH12]           = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_CYCLETR_CUR12M]            = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_CYCLETR_YEAR1]             = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_CYCLETR_YEAR2]             = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_CYCLETR_YEAR3]             = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_CYCLETR_YEAR4]             = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_CYCLETR_YEAR5]             = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_CYCLETR_TOTAL]             = {IPC_SHORT_VAR,   NULL,                       NULL},

    //--------------------------------------------------------------
    [IPC_XD_ERROR_PRESSURE]         = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_PRESSURE_SUPPLY]           = {IPC_SHORT_VAR,   IPC_ReadPressure,           NULL},
    [IPC_PRESSURE_ACTUATOR_A]       = {IPC_SHORT_VAR,   IPC_ReadPressure,           NULL},
    [IPC_PRESSURE_ACTUATOR_B]       = {IPC_SHORT_VAR,   IPC_ReadPressure,           NULL},
    [IPC_PRESSURE_ATMOS]            = {IPC_SHORT_VAR,   IPC_ReadPressure,           NULL},
    [IPC_PRESSURE_PILOT]            = {IPC_SHORT_VAR,   IPC_ReadPressure,           NULL},
    [IPC_PRESSURE_UNITS]            = {IPC_SHORT_VAR,   IPC_ReadPressureUnits,      IPC_WritePressureUnits},
    [IPC_XD_ERROR_TEMPERATURE]      = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_TEMPERATURE]               = {IPC_SHORT_VAR,   IPC_ReadTemperature,        NULL},
    [IPC_IP_CURRENT]                = {IPC_SHORT_VAR,   IPC_ReadIPCurrent,          NULL},

    //--------------------------------------------------------------
    // Status -- total 24 bytes, current -- 4L&4H&4HH; history -- 4L&4H&4HH
    [IPC_COMPLETE_STATUS_CUR_LO]    = {IPC_SHORT_VAR,   IPC_ReadCompleteStatus,     NULL},
    [IPC_COMPLETE_STATUS_CUR_HI]    = {IPC_SHORT_VAR,   IPC_ReadCompleteStatus,     NULL},
    [IPC_COMPLETE_STATUS_CUR_HIHI]  = {IPC_SHORT_VAR,   IPC_ReadCompleteStatus,     NULL},
    [IPC_COMPLETE_STATUS_HIST_LO]   = {IPC_SHORT_VAR,   IPC_ReadCompleteStatus,     NULL},
    [IPC_COMPLETE_STATUS_HIST_HI]   = {IPC_SHORT_VAR,   IPC_ReadCompleteStatus,     NULL},
    [IPC_COMPLETE_STATUS_HIST_HIHI] = {IPC_SHORT_VAR,   IPC_ReadCompleteStatus,     NULL},

    [IPC_SET_TO_FACTORY_CALIB]      = {IPC_SHORT_VAR,   NULL,                       NULL},

    //--------------------------------------------------------------
    [IPC_ACTUATOR_SENSOR_INCREASE]  = {IPC_SHORT_VAR,   NULL,                       NULL},

    //-------------------------------------------------------------------------
    [IPC_WORK_TIME_RESET]           = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_WORK_TIME_POWER]           = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_WORK_TIME_LIFETIME]        = {IPC_SHORT_VAR,   NULL,                       NULL},

    //-------------------------------------------------------------------------
    [IPC_WRONG_CONFIG_PARAM_ID]     = {IPC_SHORT_VAR,   NULL,                       NULL},
    [IPC_WRONG_CONFIG_DESC]         = {IPC_SHORT_VAR,   NULL,                       NULL},

    //-------------------------------------------------------------------------
    // [SERVO_WARN_COUNT]              = {IPC_SHORT_VAR,   NULL,                       NULL},

    //-------------------------------------------------------------------------
    [IPC_PID2_PV]                   = {IPC_LONG_VAR,   NULL,                       IPC_WritePID2PV},
    [IPC_PID2_SP]                   = {IPC_LONG_VAR,   NULL,                       IPC_WritePID2SP},
    [IPC_PID2_OUT]                  = {IPC_LONG_VAR,   NULL,                       IPC_WritePID2OUT},

    [IPC_AI3_OUT]                   = {IPC_LONG_VAR,   NULL,                       IPC_WriteAI3OUT},

    [IPC_DO2_SP_D]                  = {IPC_SHORT_VAR,   NULL,                       IPC_WriteDO2SPD},

    [IPC_IS_IN1]                    = {IPC_LONG_VAR,   NULL,                       IPC_WriteISIN1},
    [IPC_IS_IN2]                    = {IPC_LONG_VAR,   NULL,                       IPC_WriteISIN2},
    [IPC_IS_IN3]                    = {IPC_LONG_VAR,   NULL,                       IPC_WriteISIN3},
    [IPC_IS_IN4]                    = {IPC_LONG_VAR,   NULL,                       IPC_WriteISIN4},
    //-------------------------------------------------------------------------
    [IPC_DO2_BLOCK_TAG]              = {IPC_LONG_VAR,    NULL,                       IPC_WriteDO2Tag},
    [IPC_DO2_BLOCK_ACTUAL_MODE]      = {IPC_SHORT_VAR,   NULL,                       IPC_WriteDO2Mode},
    [IPC_DO2_BLOCK_ERROR]            = {IPC_SHORT_VAR,   NULL,                       IPC_WriteDO2Error},

    //-------------------------------------------------------------------------
    [IPC_PID2_BLOCK_TAG]             = {IPC_LONG_VAR,    NULL,                       IPC_WritePID2Tag},
    [IPC_PID2_BLOCK_ACTUAL_MODE]     = {IPC_SHORT_VAR,   NULL,                       IPC_WritePID2Mode},
    [IPC_PID2_BLOCK_ERROR]           = {IPC_SHORT_VAR,   NULL,                       IPC_WritePID2Error},

    //-------------------------------------------------------------------------
    [IPC_DISCRETE_INPUT]            = {IPC_SHORT_VAR,   IPC_ReadDiscreteInput,      NULL},

    //-------------------------------------------------------------------------
    [IPC_RAW_POSITION_SENSOR]       = {IPC_SHORT_VAR,   IPC_ReadRawPosition,        NULL},
    [IPC_STOP_HI_POS]               = {IPC_SHORT_VAR,   NULL,                       IPC_WriteStopHiPos},
    [IPC_STOP_LO_POS]               = {IPC_SHORT_VAR,   NULL,                       IPC_WriteStopLoPos},
    [IPC_LUI_SIMULATION]            = {IPC_SHORT_VAR,   IPC_ReadLuiSimulation,      NULL},

    //-------------------------------------------------------------------------
    [IPC_VARID_TEST]                = {IPC_SHORT_VAR,   IPC_ReadTest,               IPC_WriteTest},
    //-------------------------------------------------------------------------
    [IPC_APP_FWVERSION]             = {IPC_SHORT_VAR,   IPC_ReadAppFwVer,           NULL},
};

//--------------------------------------------------------------
static bool_t   SearchTranslationTable(IPC_Variable_IDs_t  VarID,
                                       u16  *TableIndex)
{
    bool_t  EntryFound = false;

    if (VarID >= IPC_TRANSLATION_TABLE_SIZE)
    {   // Looks like there is no such ID.
        *TableIndex = (u16)IPC_VARIABLE_ID_NOTUSED;
    }
    else
    {
        *TableIndex = (u16)VarID;
        EntryFound = true;
    }

    return EntryFound;
}

//--------------------------------------------------------------
// Variable handling stub

static ErrorCode_t  ReadShortVariable(IPC_Variable_IDs_t   VarID,
                                     IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    ErrorCode_t     retval  = ERR_OK;
    bool_t          EntryFound;
    u16             TableEntry;

    EntryFound = SearchTranslationTable(VarID, &TableEntry);
    if (EntryFound)
    {
        if (IPC_TranslationTable[TableEntry].VarType != IPC_SHORT_VAR)
        {   // Just the wrong type
            util_PutU8(pIPC_ReadPtrs->pVarStatus, (IPC_QUALITY_BAD | IPC_TYPE_CONFLICT));
            util_PutFloat(pIPC_ReadPtrs->pIPC_VarBuffer, 0.0F);
        }
        else if (IPC_TranslationTable[TableEntry].pIPC_ReadVar == NULL)
        {   // No function available
            util_PutU8(pIPC_ReadPtrs->pVarStatus, (IPC_QUALITY_BAD | IPC_VALUE_NOTAVAIL));
            util_PutFloat(pIPC_ReadPtrs->pIPC_VarBuffer, 0.0F);
        }
        else
        {   // Everything is OK
            retval = IPC_TranslationTable[TableEntry].pIPC_ReadVar(VarID, pIPC_ReadPtrs);
        }
    }
    else
    {
        util_PutU8(pIPC_ReadPtrs->pVarStatus, (IPC_QUALITY_BAD | IPC_BAD_INDEX));
        util_PutFloat(pIPC_ReadPtrs->pIPC_VarBuffer, 0.0F);
    }
    return retval;
}

static ErrorCode_t WriteArray(IPC_Variable_IDs_t       VarID,
                              IPC_WritePtrs_t   const *pIPC_WritePtrs,
                              IPC_ReadPtrs_t    const *pIPC_ReadPtrs)
{
    ErrorCode_t     retval  = ERR_OK;
    bool_t          EntryFound;
    u16             TableEntry;

    EntryFound = SearchTranslationTable(VarID, &TableEntry);
    if (EntryFound)
    {
        if (IPC_TranslationTable[TableEntry].VarType != IPC_LONG_VAR)
        {   // Just the wrong type
            util_PutU8(pIPC_ReadPtrs->pVarStatus, (IPC_QUALITY_BAD | IPC_TYPE_CONFLICT));

        }
        else if (IPC_TranslationTable[TableEntry].pIPC_WriteVar == NULL)
        {   // No function available
            util_PutU8(pIPC_ReadPtrs->pVarStatus, (IPC_QUALITY_BAD | IPC_DATA_NOTWRITABLE));
        }
        else
        {   // Everything is OK
            retval = IPC_TranslationTable[TableEntry].pIPC_WriteVar(VarID, pIPC_WritePtrs, pIPC_ReadPtrs);
        }
    }
    else
    {
        util_PutU8(pIPC_ReadPtrs->pVarStatus, (IPC_QUALITY_BAD | IPC_BAD_INDEX));
    }
    return retval;
}


static ErrorCode_t  WriteShortVariable(IPC_Variable_IDs_t      VarID,
                                      IPC_WritePtrs_t   const *pIPC_WritePtrs,
                                      IPC_ReadPtrs_t    const *pIPC_ReadPtrs)
{
    ErrorCode_t     retval  = ERR_OK;
    bool_t          EntryFound;
    u16             TableEntry;

    EntryFound = SearchTranslationTable(VarID, &TableEntry);
    if (EntryFound)
    {
        if (IPC_TranslationTable[TableEntry].VarType != IPC_SHORT_VAR)
        {   // Just the wrong type
            util_PutU8(pIPC_ReadPtrs->pVarStatus, (IPC_QUALITY_BAD | IPC_TYPE_CONFLICT));
            util_PutU32(pIPC_ReadPtrs->pIPC_VarBuffer, 0u);
        }
        else if (IPC_TranslationTable[TableEntry].pIPC_WriteVar == NULL)
        {   // Read only, not writable
            util_PutU8(pIPC_ReadPtrs->pVarStatus, (IPC_QUALITY_BAD | IPC_DATA_NOTWRITABLE));
            util_PutU32(pIPC_ReadPtrs->pIPC_VarBuffer, 0u);
        }
        else
        {   // Everything is OK
            retval = IPC_TranslationTable[TableEntry].pIPC_WriteVar(VarID, pIPC_WritePtrs, pIPC_ReadPtrs);
        }
    }
    else
    {
        util_PutU8(pIPC_ReadPtrs->pVarStatus, (IPC_QUALITY_BAD | IPC_BAD_INDEX));
        util_PutU32(pIPC_ReadPtrs->pIPC_VarBuffer, 0u);
    }
    return retval;
}

static ErrorCode_t  ReadArray(IPC_Variable_IDs_t    VarID,
                              IPC_ReadPtrs_t const *pIPC_ReadPtrs)
{
    ErrorCode_t     retval  = ERR_OK;
    bool_t          EntryFound;
    u16             TableEntry;

    EntryFound = SearchTranslationTable(VarID, &TableEntry);
    if (EntryFound)
    {
        if (IPC_TranslationTable[TableEntry].VarType != IPC_LONG_VAR)
        {   // Just the wrong type
            util_PutU8(pIPC_ReadPtrs->pVarStatus, (IPC_QUALITY_BAD | IPC_TYPE_CONFLICT));

        }
        else if (IPC_TranslationTable[TableEntry].pIPC_ReadVar == NULL)
        {   // No function available
            util_PutU8(pIPC_ReadPtrs->pVarStatus, (IPC_QUALITY_BAD | IPC_VALUE_NOTAVAIL));
        }
        else
        {   // Everything is OK
            retval = IPC_TranslationTable[TableEntry].pIPC_ReadVar(VarID, pIPC_ReadPtrs);
        }
    }
    else
    {
        util_PutU8(pIPC_ReadPtrs->pVarStatus, (IPC_QUALITY_BAD | IPC_BAD_INDEX));
    }
    return retval;
}

//--------------------------------------------------------------
// IP Action handling stub

static ErrorCode_t      IPC_SetIPAction(const IPC_IPVariable_t    *pIPCIPStruc)
{
    ErrorCode_t retval = ERR_OK;

    UNUSED_OK(*pIPCIPStruc);
    return retval;
}

//--------------------------------------------------------------
// Process DO Switches

static void IPC_Process_DOSwitches(u16 U16BufferValue)
{
    // Process as Digital Switches :
    // Bit 15 = DO2 state
    // Bit 14 = DO1 state

    if (!cnfg_GetOptionConfigFlag(DO_OPTION))
    {   // DO Parameters are not configured
        return;
    }

    DO_Passthrough_t Passthrough;

    Passthrough.state[0] = ((U16BufferValue & DO_1_IPC_STATE) != 0u) ? 1u : 0u;
    Passthrough.state[1] = ((U16BufferValue & DO_2_IPC_STATE) != 0u) ? 1u : 0u;

    dosw_SetPassthroughValues(&Passthrough);
}

//--------------------------------------------------------------

/**
\brief This command IS an IPC Dynamic Parameter Exchange command
*/

s8_least hartcmd_IPCDynamicParametersExchange(const u8 *src, u8 *dst)
{
    ErrorCode_t         retval;
    ErrorCode_t         ret;

    IPC_WritePtrs_t     WritePtrs;
    IPC_ReadPtrs_t      ReadPtrs;

    // To work with Parameters
    const Req_IPCDynamicParametersExchange_t *s1 = (const void *)src;
    Rsp_IPCDynamicParametersExchange_t *d1 = (void *)dst;

    u8                              IPCTargetMode;
    u8                              IPCActualMode;
    IPC_IPVariable_t                IPCIPStructure;

    u16                             U16TempID;          // Used for the Var ID processing
    IPC_Variable_IDs_t              IPCVariableID1;
    IPC_Variable_IDs_t              IPCVariableID2;

    u8                              ResStatus;
    u32                             RetBuffer;

    u16                             U16BufferValue;

    //-----------------------------------------------------------------------------------------
    // Extract IN parameters
    IPCTargetMode  = util_GetU8(s1->IPCTargetMode[0]);

    // Process Var ID1
    U16TempID = util_GetU16(s1->IPCVariableID1[0]);     // Extract Var ID
    util_PutU16(d1->IPCVariableID1[0], U16TempID);      // Send it back
    if (U16TempID >= (u16)IPC_VARID_MAX)
    {   // Check validity
        U16TempID = (u16)IPC_VARID_MAX;                 // If invalid -- force it to be known invalid
    }
    IPCVariableID1 = (IPC_Variable_IDs_t)U16TempID;     // Use it

    // Process Var ID2
    U16TempID = util_GetU16(s1->IPCVariableID2[0]);     // Extract Var ID
    util_PutU16(d1->IPCVariableID2[0], U16TempID);      // Send it back
    if (U16TempID >= (u16)IPC_VARID_MAX)
    {   // Check validity
        U16TempID = (u16)IPC_VARID_MAX;                 // If invalid -- force it to be known invalid
    }
    IPCVariableID2 = (IPC_Variable_IDs_t)U16TempID;     // Use it

    IPCIPStructure.IPCVarStatus = util_GetU8(s1->IPCIPActionStatus[0]);
    IPCIPStructure.IPAction     = util_GetU8(s1->IPCIPAction[0]);

    //-----------------------------------------------------------------------------------------
    // Process IN parameters
    //-----------------------------------------------------------------------------------------
    // Position and set-point and Mode
    WritePtrs.pVarStatus     = s1->IPCSPStatus[0];
    WritePtrs.pIPC_VarBuffer = s1->IPCSPFloat[0];
    ReadPtrs.pVarStatus      = &ResStatus;
    ReadPtrs.pIPC_VarBuffer  = &RetBuffer;
    retval = IPC_WriteSetPoint(IPCTargetMode, &WritePtrs, &ReadPtrs);

    ReadPtrs.pVarStatus      = d1->IPCPOSStatus[0];
    ReadPtrs.pIPC_VarBuffer  = d1->IPCPOSFloat[0];
    retval = IPC_ReadPosition(&ReadPtrs, retval); //this function needs Actual Mode already set

    //-----------------------------------------------------------------------------------------
    // Handle the Digital Switches -- from the 2 byte unused buffer
    U16BufferValue = util_GetU16(s1->IPCUnused2byteBuffer[0]);          // Get the buffer
    IPC_Process_DOSwitches(U16BufferValue);

    //-----------------------------------------------------------------------------------------
    // Variables
    ReadPtrs.pVarStatus      = d1->IPCVar1Status[0];
    ReadPtrs.pIPC_VarBuffer  = d1->IPCVar1Buffer[0];
    ret = ReadShortVariable(IPCVariableID1, &ReadPtrs);
    if (retval == ERR_OK)
    {
        retval = ret;
    }

    ReadPtrs.pVarStatus      = d1->IPCVar2Status[0];
    ReadPtrs.pIPC_VarBuffer  = d1->IPCVar2Buffer[0];
    ret = ReadShortVariable(IPCVariableID2, &ReadPtrs);
    if (retval == ERR_OK)
    {
        retval = ret;
    }

    ret = IPC_SetIPAction(&IPCIPStructure);
    if (retval == ERR_OK)
    {
        retval = ret;
    }

    //-----------------------------------------------------------------------------------------
    // Read Mode

    IPCActualMode = IPC_GetActualMode();
    util_PutU8(d1->IPCActualMode[0], IPCActualMode);

    //-----------------------------------------------------------------------------------------

    MN_ASSERT(retval == ERR_OK);
    // Return with correct status

    return HART_NO_COMMAND_SPECIFIC_ERRORS; //required for IPC on FF side
}

s8_least hartcmd_IPCReadShortVariables(const u8 *src, u8 *dst)
{
    ErrorCode_t         retval;
    ErrorCode_t         ret;

    IPC_ReadPtrs_t      ReadPtrs;

    // To work with Parameters
    const Req_IPCReadShortVariables_t *s1 = (const void *)src;
    Rsp_IPCReadShortVariables_t *d1 = (void *)dst;

    IPC_Variable_IDs_t              IPCVariableID1;
    IPC_Variable_IDs_t              IPCVariableID2;

    //-----------------------------------------------------------------------------------------
    // Extract IN parameters
    IPCVariableID1 = (IPC_Variable_IDs_t)util_GetU16(s1->IPCVariableID1[0]);
    IPCVariableID2 = (IPC_Variable_IDs_t)util_GetU16(s1->IPCVariableID2[0]);

    // Copy the Vraiable IDs to the response buffer
    util_PutU16(d1->IPCVariableID1[0], (u16)IPCVariableID1);
    util_PutU16(d1->IPCVariableID2[0], (u16)IPCVariableID2);

    //-----------------------------------------------------------------------------------------
    // Process READ Variables
    ReadPtrs.pVarStatus      = d1->IPCVar1Status[0];
    ReadPtrs.pIPC_VarBuffer  = d1->IPCVar1Buffer[0];
    retval = ReadShortVariable(IPCVariableID1, &ReadPtrs);  // 1st use!

    ReadPtrs.pVarStatus      = d1->IPCVar2Status[0];
    ReadPtrs.pIPC_VarBuffer  = d1->IPCVar2Buffer[0];
    ret = ReadShortVariable(IPCVariableID2, &ReadPtrs);
    if (retval == ERR_OK)
    {
        retval = ret;
    }

    //-----------------------------------------------------------------------------------------
    UNUSED_OK(s1->IPCUnused2byteBuffer[0]);
    UNUSED_OK(s1->IPCUnused4byteBufferA[0]);
    UNUSED_OK(s1->IPCUnused4byteBufferB[0]);

    //-----------------------------------------------------------------------------------------
    // Return with correct status
    return err2hart(retval);
}

/**
\brief This command IS an IPC Dynamic Parameter Exchange command
*/
s8_least hartcmd_IPCWriteShortVariables(const u8 *src, u8 *dst)
{
    ErrorCode_t retval;
    ErrorCode_t ret;

    // To work with Parameters
    const Req_IPCWriteShortVariables_t *s1 = (const void *)src;
    Rsp_IPCWriteShortVariables_t *d1 = (void *)dst;

    IPC_Variable_IDs_t  IPCVariableID1;
    IPC_Variable_IDs_t  IPCVariableID2;

    IPC_WritePtrs_t     WritePtrs;
    IPC_ReadPtrs_t      ReadPtrs;

    //-----------------------------------------------------------------------------------------
    // Extract IN parameters

    IPCVariableID1 = (IPC_Variable_IDs_t)util_GetU16(s1->IPCVariableID1[0]);
    IPCVariableID2 = (IPC_Variable_IDs_t)util_GetU16(s1->IPCVariableID2[0]);

    // Copy some stuff back
    util_PutU16(d1->IPCVariableID1[0], (u16)IPCVariableID1);
    util_PutU16(d1->IPCVariableID2[0], (u16)IPCVariableID2);

    //-----------------------------------------------------------------------------------------
    // Process IN parameters

    WritePtrs.pVarStatus     = s1->IPCVar1Status[0];
    WritePtrs.pIPC_VarBuffer = s1->IPCVar1Buffer[0];
    ReadPtrs.pVarStatus      = d1->IPCVar1Status[0];
    ReadPtrs.pIPC_VarBuffer  = d1->IPCVar1Buffer[0];
    retval = WriteShortVariable(IPCVariableID1, &WritePtrs, &ReadPtrs);

    WritePtrs.pVarStatus     = s1->IPCVar2Status[0];
    WritePtrs.pIPC_VarBuffer = s1->IPCVar2Buffer[0];
    ReadPtrs.pVarStatus      = d1->IPCVar2Status[0];
    ReadPtrs.pIPC_VarBuffer  = d1->IPCVar2Buffer[0];
    ret = WriteShortVariable(IPCVariableID2, &WritePtrs, &ReadPtrs);
    if (retval == ERR_OK)
    {
        retval = ret;
    }

    //-----------------------------------------------------------------------------------------
    // Return with correct status
    return err2hart(retval);
}

/**
\brief This command IS an IPC Read Array command
*/

s8_least hartcmd_IPCReadArray(const u8 *src, u8 *dst)
{
    ErrorCode_t retval;

    // To work with Parameters
    const Req_IPCReadArray_t *s1 = (const void *)src;
    Rsp_IPCReadArray_t *d1 = (void *)dst;

    IPC_Variable_IDs_t              IPCVariableID1;
    u8                              IPCDataBlockNum;

    IPC_ReadPtrs_t      ReadPtrs;

    //-----------------------------------------------------------------------------------------
    // Extract IN parameters

    IPCVariableID1 = (IPC_Variable_IDs_t)util_GetU16(s1->IPCVariableID1[0]);
    IPCDataBlockNum = util_GetU8(s1->IPCDataBlockNum[0]);

    // Copy some stuff back
    util_PutU16(d1->IPCVariableID1[0], (u16)IPCVariableID1);
    util_PutU8(d1->IPCDataBlockNum[0], IPCDataBlockNum);

    //-----------------------------------------------------------------------------------------
    // Process IN parameters

    ReadPtrs.IPC_DataBlockNum = IPCDataBlockNum;
    ReadPtrs.pVarStatus      = d1->IPCArrayStatus[0];
    ReadPtrs.pIPC_VarBuffer  = d1->IPCArrayData[0];
    retval = ReadArray(IPCVariableID1, &ReadPtrs);

    //-----------------------------------------------------------------------------------------
    UNUSED_OK(s1->IPCDataBlock[0]);

    //-----------------------------------------------------------------------------------------
    // Return with correct status
    return err2hart(retval);
}

/**
\brief This command IS an IPC write array command
*/
s8_least hartcmd_IPCWriteArray(const u8 *src, u8 *dst)
{
    ErrorCode_t retval;

    // To work with Parameters
    const Req_IPCWriteArray_t *s = (const void *)src;
    Rsp_IPCWriteArray_t *d = (void*)dst;
    IPC_Variable_IDs_t  IPCVarId;
    IPC_WritePtrs_t     WritePtrs;
    IPC_ReadPtrs_t      ReadPtrs;

    //-----------------------------------------------------------------------------------------
    // Extract IN parameters
    IPCVarId = (IPC_Variable_IDs_t)util_GetU16(s->IPCVariableID1[0]);
    WritePtrs.IPC_DataBlockNum = util_GetU8(s->IPCDataBlockNum[0]);

    // Copy some stuff back
    util_PutU16(d->IPCVariableID1[0], (u16)IPCVarId);
    util_PutU8(d->IPCDataBlockNum[0], WritePtrs.IPC_DataBlockNum);

    WritePtrs.pIPC_VarBuffer = s->IPCDataBlock[0];
    ReadPtrs.pVarStatus      = d->IPCArrayStatus[0];
    retval = WriteArray(IPCVarId, &WritePtrs, &ReadPtrs);

    return err2hart(retval);
}

/* This line marks the end of the source */
