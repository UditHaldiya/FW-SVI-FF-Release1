#MP If 0 ;Tombstone header
/*
Copyright 2004-2012 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file nvramdef.inc
    \brief Definitions of NVRAM layout entries

    This version COULD be a hull which includes nvmassembly.inc which in turn
    may be generated automatically e.g. by NVMEM entries exported by modules

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/includes/nvramdef.inc $
    $Date: 1/30/12 12:55p $
    $Revision: 56 $
    $Author: Arkkhasin $
    $History: nvramdef.inc $
 *
 * *****************  Version 56  *****************
 * User: Arkkhasin    Date: 1/30/12    Time: 12:55p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/includes
 * Removed signal setpoint and loop signal data FBO TFS:8782
 *
 * *****************  Version 55  *****************
 * User: Arkkhasin    Date: 1/20/12    Time: 10:42a
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/includes
 * TFS:8748 - remove old pressure-related interfaces
*/
#MP Endif ;Tombstone header
#MP
#MP Include "dimensions.inc"
#MP Include "projectdef.inc"
#MP
#MP ;bitmapped flags defining the handling of data structures
#MP; Usage: nvram_Entry(typename_without_t, set_function, get_function, volume, flags_string, [num_elements]optional)
#MP; flags_string is a straight pass-through to C code
#MP
#MP NVMEM_NUM_VOLUMES = NVMEM_MAX_VOLID+1
#MP
#MP
#MP Expand nvram_LayoutStart(nvram_version)
#MP Save uJoin
#MP Setstr uJoin = " | "

#MP nvram_Entry[StartupVersionNumber, TypeUnsafe_nvmem_SetVersion, TypeUnsafe_nvmem_GetVersion, VolX, Flags_None, NVMEM_NUM_VOLUMES]
#MP nvram_Entry[ModeData, TypeUnsafe_mode_InitTargetMode, TypeUnsafe_mode_GetTargetMode, Vol0, Flags_None]

#MP If FEATURE_LOOPSIGNAL_SENSOR == FEATURE_LOOPSIGNAL_SENSOR_SUPPORTED
#MP nvram_Entry[SignalCalData, TypeUnsafe_sig_SetSignalCalData, TypeUnsafe_sig_GetSignalCalData, Vol0, Flags_ConfChanged]
#MP Endif
#MP If FEATURE_SIGNAL_SETPOINT == FEATURE_SIGNAL_SETPOINT_SUPPORTED
#MP nvram_Entry[SignalData, TypeUnsafe_cnfg_SetSignalData, TypeUnsafe_cnfg_GetSignalData, Vol0, Flags_ConfChanged]
#MP Endif

#MP If FEATURE_DIGITAL_SETPOINT == FEATURE_DIGITAL_SETPOINT_SUPPORTED
#MP nvram_Entry[DigitalSpConf, TypeUnsafe_digsp_SetConf, TypeUnsafe_digsp_GetConf, Vol0, Flags_ConfChanged]
#MP nvram_Entry[digitalsp, TypeUnsafe_digsp_SetData, TypeUnsafe_digsp_GetData, Vol0, Flags_None]
#MP Endif


#MP nvram_Entry[BoardTemperature, TypeUnsafe_tempr_SetTemperature, TypeUnsafe_tempr_GetTemperature, Vol0, Flags_None, TEMPR_NUM_SENSORS]

#MP If FEATURE_PRESSURE_INPUTS == FEATURE_PRESSURE_INPUTS_INCLUDED
#MP nvram_Entry[PressureCalData, TypeUnsafe_pres_SetPressureCalData, TypeUnsafe_pres_GetPressureCalData, Vol0, Flags_ConfChanged]
#MP Endif

#MP nvram_Entry[PositionConf, TypeUnsafe_pos_SetPositionConf, TypeUnsafe_pos_GetPositionConf, Vol0, Flags_ConfChanged]

#MP nvram_Entry[PIDData, TypeUnsafe_tune_SetPIDData, TypeUnsafe_tune_GetPIDData, Vol0, {uJoin, Flags_ConfChanged, Flags_Restored}, NUM_POSCTL_PARAMSETS]

#MP nvram_Entry[ErrorLimits, TypeUnsafe_pos_SetErrorLimits, TypeUnsafe_pos_GetErrorLimits, Vol0, Flags_ConfChanged]
#MP nvram_Entry[CtlLimits, TypeUnsafe_control_SetLimits, TypeUnsafe_control_GetLimits, Vol0, {uJoin, Flags_ConfChanged, Flags_Restored}]

#MP If FEATURE_LOGFILES == FEATURE_LOGFILES_USED
#MP nvram_Entry[logdata, TypeUnsafe_logf_InitSet, TypeUnsafe_logf_InitGet, Vol0, Flags_None]
#MP Endif

#MP nvram_Entry[ContinuousDiagnostics, TypeUnsafe_posmon_SetContinuousDiagnostics, TypeUnsafe_posmon_GetContinuousDiagnostics, Vol0, Flags_None]
#MP nvram_Entry[SwitchConfiguration, TypeUnsafe_cnfg_SetSwitchConfiguration, TypeUnsafe_cnfg_GetFillSwitchConfiguration, Vol0, {uJoin, Flags_ConfChanged, Flags_Restored}]
#MP nvram_Entry[OptionConfig, TypeUnsafe_cnfg_SetOptionConfig, TypeUnsafe_cnfg_GetFillOptionConfig, Vol0, Flags_ConfChanged]
#MP nvram_Entry[BiasData, TypeUnsafe_control_SetNVMEMBiasData, TypeUnsafe_control_GetNVMEMBiasData, Vol0, Flags_None]
#MP nvram_Entry[BiasExt, TypeUnsafe_control_SetBiasExt, TypeUnsafe_control_GetFillBiasExt, Vol0, Flags_None]
#MP nvram_Entry[TuneData, TypeUnsafe_tune_SetTuneData, TypeUnsafe_tune_GetTuneData, Vol0, {uJoin, Flags_ConfChanged, Flags_Restored}]
#MP nvram_Entry[ConfigurationChanged, nvram_SetConfigurationChanged, nvram_GetConfigurationChanged, Vol0, Flags_None] ;absolutely cannot have Flags_ConfChanged

#MP If FEATURE_LOOPSIGNAL_SENSOR == FEATURE_LOOPSIGNAL_SENSOR_SUPPORTED
#MP nvram_Entry[sig_TCalRow, TypeUnsafe_sig_SetTempCorrRow, TypeUnsafe_sig_GetTempCorrRow, Vol0, Flags_Tempcomp]
#MP Endif

#MP If FEATURE_PVINPUT_SENSOR == FEATURE_PVINPUT_SENSOR_AVAILABLE
#MP nvram_Entry[inpv_TCalRow, TypeUnsafe_inpv_SetTempCorrRow, TypeUnsafe_inpv_GetTempCorrRow, Vol0, Flags_Tempcomp]
#MP Endif

#MP If FEATURE_PRESSURE_SENSOR_1 == FEATURE_PRESSURE_SENSOR_1_SUPPORTED
#MP nvram_Entry[pres1_TCalRow, TypeUnsafe_pres1_SetTempCorrRow, TypeUnsafe_pres1_GetTempCorrRow, Vol0, Flags_Tempcomp]
#MP Endif

#MP nvram_Entry[TemperatureConf, TypeUnsafe_tempr_SetTemperatureData, TypeUnsafe_tempr_GetTemperatureData, Vol0, Flags_None]
#MP nvram_Entry[tempr_TCalRow, TypeUnsafe_tempr_SetTempCorrRow, TypeUnsafe_tempr_GetTempCorrRow, Vol0, Flags_Tempcomp]

#MP If FEATURE_REMOTE_POSITION_SENSOR == FEATURE_REMOTE_POSITION_SENSOR_AVAILABLE
#MP nvram_Entry[SensorType, TypeUnsafe_ad_SetSensorType, TypeUnsafe_ad_GetSensorType, Vol0, {uJoin, Flags_ConfChanged, Flags_Restored}]
#MP nvram_Entry[posext_TCalRow, TypeUnsafe_posext_SetTempCorrRow, TypeUnsafe_posext_GetTempCorrRow, Vol0, Flags_Tempcomp]
#MP Endif

#MP nvram_Entry[posint_TCalRow, TypeUnsafe_posint_SetTempCorrRow, TypeUnsafe_posint_GetTempCorrRow, Vol0, Flags_Tempcomp]

#MP If FEATURE_PRESSURE_SENSOR_ATM == FEATURE_PRESSURE_SENSOR_ATM_SUPPORTED
#MP nvram_Entry[presatm_TCalRow, TypeUnsafe_presatm_SetTempCorrRow, TypeUnsafe_presatm_GetTempCorrRow, Vol0, Flags_Tempcomp]
#MP Endif

#MP If FEATURE_PRESSURE_SENSOR_3 == FEATURE_PRESSURE_SENSOR_3_SUPPORTED
#MP nvram_Entry[pres3_TCalRow, TypeUnsafe_pres3_SetTempCorrRow, TypeUnsafe_pres3_GetTempCorrRow, Vol0, Flags_Tempcomp]
#MP Endif

#MP If FEATURE_PRESSURE_SENSOR_2 == FEATURE_PRESSURE_SENSOR_2_SUPPORTED
#MP nvram_Entry[pres2_TCalRow, TypeUnsafe_pres2_SetTempCorrRow, TypeUnsafe_pres2_GetTempCorrRow, Vol0, Flags_Tempcomp]
#MP Endif

#MP If FEATURE_PRESSURE_SENSOR_PILOT == FEATURE_PRESSURE_SENSOR_PILOT_SUPPORTED
#MP nvram_Entry[prespilot_TCalRow, TypeUnsafe_prespilot_SetTempCorrRow, TypeUnsafe_prespilot_GetTempCorrRow, Vol0, Flags_Tempcomp]
#MP Endif

#MP nvram_Entry[ipcurr_TCalRow, TypeUnsafe_ipcurr_SetTempCorrRow, TypeUnsafe_ipcurr_GetTempCorrRow, Vol0, Flags_Tempcomp]


#MP nvram_Entry[pwmcomp_TCalRow, TypeUnsafe_pwmcomp_SetTempCorrRow, TypeUnsafe_pwmcomp_GetTempCorrRow, Vol0, Flags_Tempcomp]
#MP nvram_Entry[TemperatureCalibrationPoints, TypeUnsafe_tempr_SetTemprCalPoints, TypeUnsafe_tempr_GetTemprCalPoints, VolX, Flags_Tempcomp, TEMPR_NUM_SENSORS]

#MP nvram_Entry[HartData, TypeUnsafe_hart_SetHartData, TypeUnsafe_hart_GetAndTestHartData, Vol0, Flags_ConfChanged]

#MP If FEATURE_AO_RETRANSMIT == FEATURE_AO_RETRANSMIT_AVAILABLE
#MP nvram_Entry[ao0_TCalRow, TypeUnsafe_ao0_SetTempCorrRow, TypeUnsafe_ao0_GetTempCorrRow, Vol0, Flags_Tempcomp]
#MP nvram_Entry[AORangeValues, TypeUnsafe_ao_SetAORangeValues, TypeUnsafe_ao_GetAORangeValues, Vol0, Flags_ConfChanged]
#MP nvram_Entry[AOCalibration, TypeUnsafe_ao_SetAOCalibration, TypeUnsafe_ao_GetAOCalibration, Vol0, Flags_ConfChanged, AO_NUM_CHANNELS]
#MP Endif ;FEATURE_AO_RETRANSMIT

#MP nvram_Entry[PosCharactSel, TypeUnsafe_poscharact_SetCharacterizationSel, TypeUnsafe_poscharact_GetCharacterizationSel, Vol0, {uJoin, Flags_ConfChanged, Flags_Restored}]
#MP nvram_Entry[PosCharact, TypeUnsafe_poscharact_SetCustomTable, TypeUnsafe_poscharact_GetCustomTable, Vol0, {uJoin, Flags_ConfChanged, Flags_Restored}]

#MP nvram_Entry[FaultCodeData, error_FCodeInit, TypeUnsafe_error_GetFaultCodeData, Vol0, Flags_None]
#MP nvram_Entry[FaultCodeMasks, TypeUnsafe_error_SetFaultCodeMasks, TypeUnsafe_error_GetFaultCodeMasks, Vol0, Flags_None]

#MP ;;; Need this for I/P disconnect detection - If FEATURE_LOOPSIGNAL_SENSOR == FEATURE_LOOPSIGNAL_SENSOR_SUPPORTED
#MP nvram_Entry[LowPowerData, TypeUnsafe_sysio_SetLowPowerData, TypeUnsafe_sysio_GetLowPwrData, Vol0, Flags_None]
#MP ;;;Endif

#MP If FEATURE_LOCAL_UI == FEATURE_LOCAL_UI_SIMPLE
#MP nvram_Entry[OSAdjConf, TypeUnsafe_osadj_SetConf, TypeUnsafe_osadj_GetConf, Vol0, Flags_ConfChanged]
#MP nvram_Entry[LUI_Jumpers, TypeUnsafe_LUI_SetJumpers, TypeUnsafe_LUI_GetJumpers, Vol0, Flags_ConfChanged]
#MP Endif ;FEATURE_LOCAL_UI

#MP If FEATURE_LOCAL_UI == FEATURE_LOCAL_UI_LCD
    #MP nvram_Entry(UIData, TypeUnsafe_ui_RepairSetUIData, TypeUnsafe_ui_GetLocalUIData, Vol0, {uJoin, Flags_ConfChanged, Flags_Restored})
#MP Endif ;FEATURE_LOCAL_UI

#MP nvram_Entry[PneumaticParams, TypeUnsafe_pneu_SetParams, TypeUnsafe_pneu_GetParams, Vol0, {uJoin, Flags_None, Flags_Restored}]

#MP If FEATURE_LOOPSIGNAL_SENSOR == FEATURE_LOOPSIGNAL_SENSOR_SUPPORTED
#MP nvram_Entry[SignalConf, TypeUnsafe_sig_SetSignalConf, TypeUnsafe_sig_GetSignalConf, Vol0, Flags_ConfChanged]
#MP Endif

#MP If FEATURE_BUMPLESS_XFER == FEATURE_BUMPLESS_XFER_ENABLED
#MP nvram_Entry[BumplessConf, TypeUnsafe_bumpless_SetConf, TypeUnsafe_bumpless_GetConf, Vol0, Flags_ConfChanged]
#MP Endif ;FEATURE_BUMPLESS_XFER

#MP If FEATURE_ACTIVATION == FEATURE_ACTIVATION_ID_SN
#MP nvram_Entry[ActivateVerifyData, TypeUnsafe_actver_SetData, TypeUnsafe_actver_GetData, Vol0, Flags_ConfChanged]
#MP Endif ;FEATURE_ACTIVATION

#MP nvram_Entry[CtlSetSlot, TypeUnsafe_posctl_SetCtlset, TypeUnsafe_posctl_GetCtlset, Vol0, {uJoin, Flags_ConfChanged, Flags_Restored}]

#MP nvram_Entry(UnitsOptions, TypeUnsafe_unitsconv_SetUnitsOptions, TypeUnsafe_unitsconv_GetUnitsOptions, Vol0, Flags_None)
#MP nvram_Entry(UnitsConfig, fpconvert_InitRepair, fpconvert_GetAllUnits, Vol0, {uJoin, Flags_ConfChanged, Flags_Restored});
#MP nvram_Entry(InterfaceAccess, TypeUnsafe_ifman_SetLock, TypeUnsafe_ifman_GetLock, Vol0, Flags_ConfChanged)

#MP If FEATURE_CPU == FEATURE_CPU_STM32F10xxE
#MP nvram_Entry[RefVoltage, TypeUnsafe_refvoltage_SetData, TypeUnsafe_refvoltage_GetData, Vol0, {uJoin, Flags_None, Flags_Restored}]
#MP nvram_Entry(FFData, TypeUnsafe_ff_RepairSetFFData, TypeUnsafe_ff_GetLocalFFData, Vol0,  {uJoin, Flags_None, Flags_Restored})
#MP Endif ;FEATURE_CPU

#MP Restore uJoin

#MP Expand nvram_LayoutEnd()
#MP ;/* This line marks the end of the source */
