#MP If 0 ;Tombstone header
/**
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file harttdef.inc
    \brief Master definitions of HART services - Old style

    CPU: Any

    OWNER: AK
    \n RCS: $Id: $
     $DateTime: $ $Change: $ $Author: Arkkhasin $
*/
#MP Endif ;Tombstone header
#MP     Expand DefineOb(0, #@ReadUniqueIdentifier#)
#MP     Expand DefineOb(1, #@ReadPrimaryVariable#)
#MP If FEATURE_LOOPSIGNAL_SENSOR == FEATURE_LOOPSIGNAL_SENSOR_SUPPORTED
#MP     Expand DefineOb(2, #@ReadCurrent#)
#MP Endif
#MP     Expand DefineOb(3, #@ReadDynamicVariables#)
#MP     Expand DefineOb(6, #@WritePollingAddress#)
#MP     Expand DefineOb(11, #@ReadUniqueIdentifierByTag#)
#MP     Expand DefineOb(12, #@ReadMessage#)
#MP     Expand DefineOb(13, #@ReadTagAndDescriptor#)
#MP     Expand DefineOb(14, #@ReadTransducerInfo#)
#MP     Expand DefineOb(15, #@ReadTransmitterInfo#)
#MP     Expand DefineOb(16, #@ReadFinalAssemblyNumber#)
#MP     Expand DefineOb(17, #@WriteMessage#)
#MP     Expand DefineOb(18, #@WriteTagAndDescriptor#)
#MP     Expand DefineOb(19, #@WriteFinalAssemblyNumber#)
#MP     ;;;Expand DefineOb(33, #@ReadDeviceVariables#)
#MP     ;;; Not in LCX and must have a different number Expand DefineOb(35, #@WriteRangeValues#)
#MP     Expand DefineOb(38, #@ResetConfigurationChangedFlag#)
#MP     ;;; Not in LCX and must have a different number Expand DefineOb(40, #@SetFixedOutput#)
#MP     ;;; Does not exist Expand DefineOb(41, #@PerformSelfTest#)
#MP;--in xml     Expand DefineOb(42, #@Reset#)
#MP     ;;; Not in LCX and must have a different number Expand DefineOb(45, #@TrimZero#)
#MP     ;;; Not in LCX and must have a different number Expand DefineOb(46, #@TrimGain#)
#MP     ;;; In xml Expand DefineOb(48, #@ReadAdditionalStatus#)
#MP     Expand DefineOb(108, #@WriteBurstModeCommand#)
#MP     Expand DefineOb(109, #@WriteBurstModeControl#)
#MP;--in xml     Expand DefineOb(128, #@ReadVariableAddress#)
#MP;--in xml     Expand DefineOb(129, #@ReadMemory#)
#MP;--in xml     Expand DefineOb(130, #@WriteMemory#)
#MP     ;;;Expand DefineOb(131, #@ReadDynamicInfo#)
#MP     ;;;Expand DefineOb(132, #@CaptureHighSpeedData#)
#MP;--in xml     Expand DefineOb(133, #@SetOutputSwitch2#)
#MP;--in xml     Expand DefineOb(134, #@WriteSwitchSettings#)
#MP;--in xml     Expand DefineOb(135, #@ChangeMode#)
#MP     ;;; In xml Expand DefineOb(136, #@ReadStatus#)
#MP;--in xml     Expand DefineOb(137, #@ReadSwitchSettings#)
#MP     Expand DefineOb(138, #@ClearFaults#)
#MP     Expand DefineOb(139, #@ReadContinuousDiagnostics#)
#MP     Expand DefineOb(140, #@ResetContinuousDiagnostics#)
#MP     Expand DefineOb(141, #@ReadRawData#)
#MP;--in xml     Expand DefineOb(142, #@ReadSwitches#)
#MP;--in xml     Expand DefineOb(143, #@ReadTemperatures#)
#MP;--in xml     Expand DefineOb(154, #@ReadSetpoint#)
#MP;--in xml     Expand DefineOb(155, #@WriteSetpoint#)
#MP;--in xml     Expand DefineOb(160, #@WriteIPValue#)
#MP     ;--Dismantled Expand DefineOb(165, #@WriteConfigurationData#);-- in xml
#MP;--in xml     Expand DefineOb(167, #@SetOpenStopAdjustment #)
#MP;--in xml     Expand DefineOb(168, #@SetClosedStopAdjustment #)
#MP;--in xml     Expand DefineOb(169, #@ReadOpenClosedStopAdjustment #)
#MP     ;-- in xml Expand DefineOb(172, #@WriteCustomCharacterization#)
#MP     ;-- in xml Expand DefineOb(173, #@WriteControlParameters#)
#MP     Expand DefineOb(174, #@WriteBiasParameters#)
#MP;--in xml     Expand DefineOb(175, #@WriteErrorLimits#)
#MP;--in xml     Expand DefineOb(176, "LosslesslyReadDataBuffer")
#MP     ;;;Expand DefineOb(178, #@ReplaceStops#)
#MP;--in xml     Expand DefineOb(179, #@SetManualStop#)
#MP;--in xml     Expand DefineOb(180, #@FindStops#)
#MP If FEATURE_LOOPSIGNAL_SENSOR == FEATURE_LOOPSIGNAL_SENSOR_SUPPORTED
#MP     Expand DefineOb(181, #@WriteInputCurrentRange#)
#MP Endif
#MP     Expand DefineOb(183, #@ResetToFactorCal#)
#MP     ;-- in xml Expand DefineOb(184, #@SaveStandardSignature#)
#MP     ;-- in xml Expand DefineOb(185, #@Calibrate#)
#MP     ;-- in xml Expand DefineOb(189, #@Autotune#)
#MP;--in xml     Expand DefineOb(190, #@RunDiagnostics#)
#MP     ;-- in xml will change Expand DefineOb(191, #@ReadStandardDiagnosticData#)
#MP     ;-- in xml will change Expand DefineOb(192, #@ReadStandardSignature#)
#MP     ;-- in xml will change Expand DefineOb(193, #@ReadDiagnosticData#)
#MP     Expand DefineOb(200, #@ReadOptionConfiguration#)
#MP     ;-- in xml Expand DefineOb(201, #@ReadAllPressures#)
#MP     Expand DefineOb(202, #@ActualReadOptionConfiguration#)
#MP     ;;; Not in LCX  Expand DefineOb(207, #@ReadPositionTransmitRV#)
#MP     ;--Dismantled Expand DefineOb(210, #@ReadConfigurationData#);-- in xml
#MP     ;-- in xml Expand DefineOb(211, #@ReadCustomCharacterization#)
#MP;--in xml     Expand DefineOb(212, #@ReadErrorLimits#)
#MP If FEATURE_LOOPSIGNAL_SENSOR == FEATURE_LOOPSIGNAL_SENSOR_SUPPORTED
#MP     Expand DefineOb(213, #@ReadCurrentLoopValues#)
#MP Endif
#MP     ;-- in xml Expand DefineOb(216, #@ReadControlParameters#)
#MP     Expand DefineOb(217, #@ReadBiasParameters#)
#MP     ;-- in xml NEW TEMPCOMP Expand DefineOb(220, #@ReadADCRawData#)
#MP     ;-- in xml NEW TEMPCOMP Expand DefineOb(221, #@ReadADCCompData#)
#MP     ;-- in xml NEW TEMPCOMP Expand DefineOb(233, #@WriteTemperatureCompensationTemp#)
#MP     ;-- in xml NEW TEMPCOMP Expand DefineOb(234, #@WriteChannelShift#)
#MP     ;-- in xml NEW TEMPCOMP Expand DefineOb(236, #@ReadTemperatureCompensationTemp#)
#MP     ;-- in xml NEW TEMPCOMP Expand DefineOb(237, #@ReadChannelShift#)

#MP     Expand DefineOb(248, #@WriteOptionConfig#)
#MP;--in xml     Expand DefineOb(249, #@ReadMode#)
#MP;--in xml     Expand DefineOb(251, #@CheckProcess#)
#MP     Expand DefineOb(253, #@HaltProcess#)
#MP
#MP ; This line marks the end of the source
