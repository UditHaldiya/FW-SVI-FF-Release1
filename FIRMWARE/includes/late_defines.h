/*
Copyright 2004-2007 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/** \addtogroup mncbcomp MNCB computer definitions
\brief Basic framework with native types abstraction

The module only has basic definitions; there is nothing to design (see compiler
manual instead) and nothing to test
*/
/**
    \file mnwrap.h
    \brief Wrappers for compiler- and library-dependent facilities

    This header is just a hull. The actual wrapping is done in
    innerwrap.h.

    CPU: ARM family (because of armtypes.h); IAR compiler (inarm.h)

    OWNER: EP
    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes/late_defines.h $
    $Date: 1/13/11 11:45a $
    $Revision: 1 $
    $Author: Justin Shriver $

    \ingroup mncbcomp
*/
/* (Optional) $History: late_defines.h $
 * 
 * *****************  Version 1  *****************
 * User: Justin Shriver Date: 1/13/11    Time: 11:45a
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * TFS:5384  global place to hold OPTIONAL_TUNE_DIAG
 *
*/

#ifndef LATE_DEFINES_H
#define LATE_DEFINES_H
#ifdef NDEBUG
//MUST BE 0
#define OPTIONAL_TUNE_DIAG 0
#else
//Define here can be either
//with 0 we have a problem with ftable
#define OPTIONAL_TUNE_DIAG 0
#endif
#endif
