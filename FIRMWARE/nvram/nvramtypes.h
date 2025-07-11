/*
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file nvramtypes.h
    \brief Collection of all types for NVRAM layout and interface.

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/nvram/nvramtypes.h $
    $Date: 1/30/12 2:55p $
    $Revision: 27 $
    $Author: Arkkhasin $

    \ingroup nvmem
*/
/* $History: nvramtypes.h $
 *
 * *****************  Version 27  *****************
 * User: Arkkhasin    Date: 1/30/12    Time: 2:55p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/nvram
 * Removed signal setpoint and loop signal data FBO TFS:8782
 *
 * *****************  Version 26  *****************
 * User: Arkkhasin    Date: 1/12/12    Time: 12:42p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/nvram
 * TFS:8674 - added AO trim interface
 *
 * *****************  Version 25  *****************
 * User: Arkkhasin    Date: 1/06/12    Time: 4:27p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/nvram
 * Still TFS:8577
 *
 * *****************  Version 24  *****************
 * User: Arkkhasin    Date: 1/05/12    Time: 3:21p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/nvram
 * TFS:8577 - MVMEM for UI
 *
 * *****************  Version 23  *****************
 * User: Arkkhasin    Date: 12/09/11   Time: 3:46p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/nvram
 * TFS:8370 Module drop-in -- engunits_1-0
 *
 * *****************  Version 22  *****************
 * User: Arkkhasin    Date: 12/07/11   Time: 12:59p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/nvram
 * TFS:8204 - features for I/O channels
 *
 * *****************  Version 21  *****************
 * User: Arkkhasin    Date: 11/17/11   Time: 10:50a
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/nvram
 * TFS:8202 decouple I/O subsystem - removed AO-related things
 *
 * *****************  Version 20  *****************
 * User: Arkkhasin    Date: 11/15/11   Time: 12:56p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/nvram
 * TFS:8051 new tempcomp
 * TFS:8202 decouple I/O subsystem
*/

#ifndef NVRAMTYPES_H_
#define NVRAMTYPES_H_

/* Need to #include all NVRAM types.
*/
#include "projectdef.h"
#include "nvram.h"
#include "poscharact.h"
#include "tempcomp.h"
#include "faultpublic.h"
#include "smoothing.h"
#include "devicemode.h"
#include "configure.h"
#include "tempr.h"
#include "selftune.h"
#include "control.h"
#include "errlimits.h"
#include "tempcomp.h"
#include "diagnostics.h"
#include "hart.h"
#include "ctllimits.h"
#if FEATURE_LOCAL_UI == FEATURE_LOCAL_UI_SIMPLE
#include "uisimple.h"
#include "uilock.h"
#include "osadjust.h"
#endif
#if FEATURE_LOCAL_UI == FEATURE_LOCAL_UI_LCD
#include "uipublic.h"
#include "uimenued.h"
#endif
#if FEATURE_BUMPLESS_XFER == FEATURE_BUMPLESS_XFER_ENABLED
#include "bumpless.h"
#endif
#if FEATURE_ACTIVATION == FEATURE_ACTIVATION_ID_SN
#   include "activate_id_sn.h"
#endif

#if FEATURE_SIGNAL_SETPOINT == FEATURE_SIGNAL_SETPOINT_SUPPORTED
#include "signalsp.h"
#endif

#include "digitalsp.h"

#include "nvmempriv.h"
// - needed for I/P disconnect - #if FEATURE_LOOPSIGNAL_SENSOR == FEATURE_LOOPSIGNAL_SENSOR_SUPPORTED
#include "sysiolopwr.h"
//#endif
#include "mncbdefs_proj.h"
#include "interfacenvm.h"

//Need this for I/P disconnect detection -
#include "iplimit.h"

#if FEATURE_LOOPSIGNAL_SENSOR == FEATURE_LOOPSIGNAL_SENSOR_SUPPORTED
#   include "insignal.h"
#endif
#include "pwmcomp.h"
#if FEATURE_PVINPUT_SENSOR == FEATURE_PVINPUT_SENSOR_AVAILABLE
#   include "inpv.h"
#endif
#include "tempr_mncb.h"
#include "posint.h"
#if FEATURE_REMOTE_POSITION_SENSOR == FEATURE_REMOTE_POSITION_SENSOR_AVAILABLE
#   include "adtbl.h"
#   include "posext.h"
#endif
#include "ipcurr.h"
#if FEATURE_PRESSURE_INPUTS == FEATURE_PRESSURE_INPUTS_INCLUDED
#   include "pneumatics.h"
#   include "prestab.h"
#endif
#if FEATURE_AO == FEATURE_AO_SUPPORTED
#include "ao.h"
#include "osubrange.h"
#endif

#include "doswitch.h"

#include "ifman.h"

#if FEATURE_CPU == FEATURE_CPU_STM32F10xxE
#include "refvoltage.h"
#include "ff_devicevars.h"
#endif

#if FEATURE_LOGFILES == FEATURE_LOGFILES_USED
#include "logfile.h"
#endif

#ifdef OLD_NVRAM
#define NV_HEADERSIZE 1u //size of the header of a NVRAM record
#define NV_NUMCOPIES 2u //number of copies in the record
#else
//These are coming from nvmempreiv.h - don't belong here
#endif //OLD_NVRAM

#endif //NVRAMTYPES_H_
/* This line marks the end of the source */

