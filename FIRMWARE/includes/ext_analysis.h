/*
Copyright 2010 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file ext_analysis.h
    \brief Types and prototypes of extended analysis HART functions

    CPU: Any

    OWNER: SK
    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes/ext_analysis.h $
    $Date: 5/18/11 10:19a $
    $Revision: 11 $
    $Author: Justin Shriver $

    \ingroup control
*/
/* $History: ext_analysis.h $
 *
 * *****************  Version 11  *****************
 * User: Justin Shriver Date: 5/18/11    Time: 10:19a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * TFS:6337
 *
 * *****************  Version 10  *****************
 * User: Justin Shriver Date: 5/13/11    Time: 5:36p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * TFS:5384 Instrument Autotune Temporarily
 *
 * *****************  Version 9  *****************
 * User: Sergey Kruss Date: 3/02/11    Time: 12:19p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * TFS:5746 - CalcFreq is an erroneous term: it must be period of adding
 * integral.
 *
 * *****************  Version 8  *****************
 * User: Justin Shriver Date: 1/13/11    Time: 12:06p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * TFS:5384 add debug capabilities
 *
 * *****************  Version 7  *****************
 * User: Sergey Kruss Date: 11/19/10   Time: 11:59a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * TFS:4761--Final checkin for command 242,2
 *
 * *****************  Version 6  *****************
 * User: Sergey Kruss Date: 11/17/10   Time: 4:19p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * TFS:4761--Added command 242,2 for observing position propagation
 * through all transformations from A/D converter.
 *
 * *****************  Version 5  *****************
 * User: Sergey Kruss Date: 10/01/10   Time: 11:53a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * TFS:4226. Changed Cotnrol Output related variables to less misleading.
 *
 * *****************  Version 4  *****************
 * User: Sergey Kruss Date: 9/24/10    Time: 10:06a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * TFS:4180. Fixed cmd-242 parameter types.
 *
 * *****************  Version 3  *****************
 * User: Sergey Kruss Date: 8/31/10    Time: 1:35p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * -Added control calculated PWM value to cmd 242
 * -Changed version of cmd 242 to Alpha 9
 *
 * *****************  Version 2  *****************
 * User: Sergey Kruss Date: 8/07/10    Time: 3:16p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * TFS:3520, TFS:3596 -- Separate terms P and D and add Boost term
 *
 * *****************  Version 1  *****************
 * User: Sergey Kruss Date: 6/25/10    Time: 1:55p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * Modified HART command 242, implemented it with XML and moved from
 * hartfunc.c
 *

*/

#ifndef EXT_ANALYSIS_H_
#define EXT_ANALYSIS_H_

typedef struct ExtAnalysParams_t
{
    /** TFS:4016 */  /** TFS:4226 */
    u16 CtlOutput;
    s16 AvgErr;
    s16 MinErr;
    s16 MaxErr;
    /** TFS:4180 */
    u16 PosComp;
    s16 P_term;
    s16 D_term;
    s16 Boost_term;
    s16 Fast_terms;
    s32 Integral;
    u16 Bias;
    u16 Overshoots;
    u16 JigglesPassed;
    u16 CalcPeriod;  // TFS:5746
    u8  IntegalCount;
    u8  CtrlByte;
    u16  BitPack;
} ExtAnalysParams_t;


extern void control_FillExtAnalysParams(ExtAnalysParams_t *extAnalysPar);

#endif //EXT_ANALYSIS_H_

/* This line marks the end of the source */
