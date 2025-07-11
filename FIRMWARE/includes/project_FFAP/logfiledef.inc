#MP If 0 ;Tombstone header
/*
Copyright 2006-2007 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file logfiledef.inc
    \brief Definitions of entries for the project-dependent log file configuration table

    This file is included in logfdata.u to yield logfdata.h and logfdata.c with
    C definitions of log file system configuration

Instructions for arguments to LogfDefine:
Param 1: Max size of the file
Param 2: Number of versions of the file to keep
Param 3: Fault to set if a new file version is written
Param 4: Fault to set if writing a new file version failed.
IMPORTANT: All parameters must be symbolic; no plain numbers allowed!

    CPU: Any

    OWNER: AK

    \ingroup logfile
*/
#MP Endif ;Tombstone header
#MP Expand LogfBegin()
#MP Expand LogfDefine(DIAGSIGN_LOGFILE_MAXSIZE, NUMBER_DIAGNOSTIC_BASELINES, DIAGSIGN_BASELINE_LOGFILE_ID, FAULT_NEWDIAGSIGN_BASELINE, FAULT_NONE)
#MP Expand LogfDefine(DIAGSIGN_LOGFILE_MAXSIZE, NUMBER_DIAGNOSTIC_CURRENT_CURVES, DIAGSIGN_CURRENT_LOGFILE_ID, FAULT_NEWDIAGSIGN_CURRENT, FAULT_NONE)
#MP Expand LogfDefine(DIAGSIGN_LOGFILE_MAXSIZE, NUMBER_DIAGNOSTIC_USER_CURVES, DIAGSIGN_USER_LOGFILE_ID, FAULT_NEWDIAGSIGN_USER, FAULT_NONE)
#MP Expand LogfDefine(NVM_TOTAL_DATA_LENGTH, NUMBER_NVM_BACKUPS, NVMBACKUP_LOGFILE_ID, FAULT_NONE, FAULT_NONE)
#MP Expand LogfEnd()
