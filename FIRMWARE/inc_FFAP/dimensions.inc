#MP If 0 ;Tombstone header
/*
Copyright 2004-2008 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file dimensions.inc
    \brief Definitions of fixed-size-like parameters shared between Unimal and C.

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/inc_FFAP/dimensions.inc $
    $Date: 1/30/12 12:26p $
    $Revision: 5 $
    $Author: Arkkhasin $

    \ingroup nvmem
*/
/* (Optional) $History: dimensions.inc $
 *
 * *****************  Version 5  *****************
 * User: Arkkhasin    Date: 1/30/12    Time: 12:26p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_FFAP
 * Adapted to the number of DO channels FBO TFS:8782
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 3/08/10    Time: 12:42p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * 8.
 * Getting $/MNCB/Dev/FIRMWARE/inc_LCX/dimensions.inc
 * Getting $/MNCB/Dev/FIRMWARE/inc_LCX/mncbdefs_proj.h
 *
 * *****************  Version 5  *****************
 * User: Arkkhasin    Date: 2/24/10    Time: 1:08a
 * Updated in $/MNCB/Dev/FIRMWARE/inc_LCX
 * Added handling of NUM_POSCTL_PARAMSETS
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 2/09/09    Time: 3:00p
 * Updated in $/MNCB/Dev/FIRMWARE/inc_SVI2AP
 * First shot at position/level range monitors (no interfaces yet)
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 7/19/08    Time: 5:18p
 * Updated in $/MNCB/Dev/FIRMWARE/inc_SVI2AP
 * Defined AO_NUM_CHANNELS and TEMPR_NUM_SENSORS for each project
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 5/26/08    Time: 9:20p
 * Created in $/MNCB/Dev/FIRMWARE/inc_SVI2AP
 * Project-wide dimensions shared between Unimal and C. Replaces
 * nvramdef.u which is deleted
*/
#MP Endif ;Tombstone header
#MP
#MP Set NUMBER_OF_CALIB_POINTS=8 ;The # of cal points as known to Unimal and C
#MP Set FAULT_CODE_BYTES_STORED=12 ;The # of bytes allocated in NVRAM to store a single copy of faults bitmap
#MP ; no analog output in the project AO_NUM_CHANNELS = 1 ;The number of analog outputs
#MP TEMPR_NUM_SENSORS = 1 ;The number of temperature sensors
#MP NVMEM_MAX_VOLID = 0 ;Valid NVMEM volumes are 0 and... that's all
#MP ;;Not used in th eproject NUM_POS_MONITORS = 4 ;Number of independent position range monitors
#MP NUM_POSCTL_PARAMSETS = 8 ;Number of independent PID parameters sets
#MP DO_NUM_CHANNELS = 2 ;The number of discrete outputs
#MP COMM_NUM_CHANNELS = 3 ;The number UART-based comm ports
#MP ;Abandoned segmented design: POSCHARACT_COARSE_SEGS = 4 ;Number of coarse characterization points - 1
#MP NUM_DIAG_BUFFERS = 1 ;number of diagnostic buffers
#MP DIAG_BUFFER_LOGSIZE = 12 ;buffer size is 1<<DIAG_BUFFER_LOGSIZE
#MP NVRAM_PARTITION_REMAP_SIZE = 8192 ;buffer size is 1<<DIAG_BUFFER_LOGSIZE
#MP
#MP ;/* This line marks the end of the source */
