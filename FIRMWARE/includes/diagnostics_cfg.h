/**
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file diagnostics_cfg.h
    \brief Includes all the configuration constants that are nearly arbitrary
           That is ones that can be changed to meet user goals
    OWNER: JS
    CPU: Any

    $Revision: 2 $

*/

/*
    $History: diagnostics_cfg.h $
 *
 * *****************  Version 2  *****************
 * User: Justin Shriver Date: 7/28/10    Time: 6:55p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * TFS:3659 -- Number of diagnostic curves
 *
 * *****************  Version 1  *****************
 * User: Justin Shriver Date: 7/28/10    Time: 5:59p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * TFS:3543 -- Integrate 3.2.1 diagnostics into LCX
 *
 * *****************  Version 7  *****************
 * User: Justin Shriver Date: 1/27/10    Time: 7:23p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/tasks
 * TFS:2543
 *
 * *****************  Version 6  *****************
 * User: Justin Shriver Date: 1/20/10    Time: 7:22p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/tasks
 * TFS:2259
 *
 * *****************  Version 5  *****************
 * User: Justin Shriver Date: 1/20/10    Time: 3:55p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/tasks
 * TFS:2259
*/

#ifndef DIAGNOSTICS_CFG_H_
#define DIAGNOSTICS_CFG_H_


#include "dimensions.h" //for number of diagnostic curves

/** REQ 21-50 Allow one to specify a minimum delta bias */
#define MIN_DELTA_BIAS 200

/** REQ 21-36
    REQ 21-74
    User input speed is 1/8 %/sec for new test old test was 1/4 %/sec */
#define BIAS_HIGH_RES_SPEED_RED 2
/** REQ 21-64 User acceleration, don't allow it to go to zero rate*/
#define BIAS_SCALE_FACTOR 64

/** REQ 21-64 User acceleration, don't allow it to go to zero rate*/
#define MAX_IP_ACCELERATE_RATE (1000 * BIAS_SCALE_FACTOR)
// remove #define ACCELERATE_ADDER_SCALE 64
//Amount to increment or decrement by
#define IP_ACCELERATE_AMT 1
#define IP_DECELERATE_AMT 1
/** REQ 21-64 don't accelerate if position is in range */
#define POS_ERR_THRSH INT_PERCENT_OF_RANGE(4.5)
#define MIN_IN_RANGE_POS (INT_PERCENT_OF_RANGE(0.0) + POS_ERR_THRSH)
#define MAX_IN_RANGE_POS (INT_PERCENT_OF_RANGE(100.0) - POS_ERR_THRSH)
/** REQ 21-64 Rate to increase IP increment by
    REQ 21-48 Minimum rate of IP change */
#define MIN_IP_ACCELERATE (BIAS_SCALE_FACTOR/16)
/** REQ 21-64 REQ 21-56 IP always changes */
#define INCREMENT_EVERY_N_CYCLES 10u
//Keep IP accelerated above zero
//unused #define IP_ACCELERATE_RATE (1)

/** REQ 21-26 Keep valve in range*/
#define SCAN_ERR_THRESH INT_PERCENT_OF_RANGE(2.0)
/** REQ 21-83 Start next segement close to last open loop segement*/
#define CTRL_MULT 1
#define OPEN_LOOP_MULT 4


/** REQ 21-8 The old algorithm is run for values  below .95
    REQ 21-26*/
#define THRESHOLD_FOR_NEW_TEST INT_PERCENT_OF_RANGE(0.95)
/** REQ 21-65 user specified rate for speed >=4*/
#define MIN_USER_IP_ACCELERATE INT_PERCENT_OF_RANGE(3.95)


/** REQ 21-8 Use continious sampling */
//Wrong: should be derived from buffer size #define MAX_SAMPLES (504/2) //two-halfword entries in a 508-halfwords (actually, 510) buffer






/** REQ 21-66 Spare region for dead band, not currently needed*/
#define BIAS_TOL_BAND 0


/** REQ 21-43 */
//Samples in diagnostic buffer should be a system property based on buffer size
//back in diagnostics.c #define DIAG_MAX_SAMPLES 400u
//Note this may have to be correlated with the diagnostic buffer
#define SAMPLE_EVERY_N_CYCLES 100u
//#define TARGET_PRESSURE_POINTS 20u
//#define TARGET_POSITION_POINTS 20u
//#define MIN_PTS_TO_AQUIRE MIN(TARGET_PRESSURE_POINTS, TARGET_POSITION_POINTS)


/** REQ 21-32 produce tails above above and below 1% and 99%*/
#define GO_TO_LIMIT_BELOW ((s32)((float32)STANDARD_RANGE*0.01F))
#define GO_TO_LIMIT_ABOVE ((s32)((float32)STANDARD_RANGE*0.99F))


#endif //DIAGNOSTICS_CFG_H_
