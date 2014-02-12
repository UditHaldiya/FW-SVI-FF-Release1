/**
Copyright 2010 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file diagnosticsUniversal_sub.h
    \brief Interface to diagnosticsUniversal

    OWNER: JS
    CPU: Any

    $Revision: 9 $

*/

/** \addtogroup ValveAndActuatorDiagnostics
\htmlonly
<a href="http://mymasoneilan/en/Engineering/innovations/Team%20Documents1/Projects/SVI2AP/3.1.5/Requirements/Extended%20Signature%20--%202194/Req_ExtendedDiagnosticSignature.doc"> Requirements document </a><br>
<a href="N/A"> Design document </a><br>
<a href="TBD"> Unit Test Report [1]</a><br>
<a href="TBD"> Unit Test Report [2]</a><br>
\endhtmlonly
*/

#ifndef DIAGNOSTICSUNIVERSAL_SUB_H_
#define DIAGNOSTICSUNIVERSAL_SUB_H_


extern procresult_t diag_Perform_ExtActuatorSignatureOpen(void);



void setPrepareVars(u32 bias_low, u32 bias_high, u16 cur_bias);


#endif //DIAGNOSTICSUNIVERSAL_SUB_H_

