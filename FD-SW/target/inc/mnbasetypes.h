/*
Copyright 2012 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/** \addtogroup mncbcomp MNCB computer definitions
\brief Basic framework with native types abstraction
*/
/**
    \file mnbasetypes.h
    \brief Wrappers for compiler- and library-dependent facilities

    This header is just a hull. The actual wrapping is done in
    innerwrap.h.

    This header is a replacement of mnwrap.h because the (Softing version of)
    gcc chokes on mnwrap.h (Not standard-compliant)

    CPU: ARM family (because of armtypes.h); IAR compiler (inarm.h)

    \ingroup mncbcomp
*/

//Note: The guard's name is for mnwrap.h intentionally: innerwrap.h expects it.
#ifndef MNWRAP_H
#define MNWRAP_H

#include <intrinsics.h> //for intrinsic functions

#define MN_TYPE_ENABLE_DEFERRED //do not include typestomp.h in armtypes.h
#include "armtypes.h"

/** A handy Low/High index type */
enum
{
    Xlow,
    Xhi,
    Xends
};

#include "innerwrap.h"

#endif //MNWRAP_H
