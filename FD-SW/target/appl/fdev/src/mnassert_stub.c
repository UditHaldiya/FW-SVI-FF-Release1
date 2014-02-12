/*
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/** \addtogroup Assert MN Assert module
\brief API and implementation of assert functionality
*/
/**
    \file mnassert.c
    \brief An STUB implementation of assert functionality for FF

    Unlike "normal" assert(), MN_ASSERT cannot be redefined, so
    this header is guarded as any other header.
    Like "normal" assert(), MN_ASSERT compiles out when NDEBUG
    is \#define'd (a.k.a. "release build") and doesn't return on error
    if NDEBUG is not \#define'd

    \ingroup Assert
*/
#include <base.h>
#include <keywords.h>
#include <hw_if.h>
//#include "mnwrap.h"
#define NEED_NVMEM_END //lint !e962 expose hidden stuff
#include "mnassert.h"
//#include "mntrapdef.h"
//#include MEMMAP_H_

#define ASSERT_SIGNATURE 0x55U
//extern MN_NORETURN void Reset_CPU(void);
//lint -esym(551, assert_msg)
volatile MN_NO_INIT assert_t assert_msg; //make sure the compiler doesn't optimize out writes to it

/* \brief Prepares the assert (trap) record (structure)
\param message - a pointer to offending place in the code (or the filename)
\param line - negative trap id or if positive, file line number
*/
static void mn_rt_assert_prep(const u8 *message, s32 line)
{
    //fill the assert message
    assert_msg.message = message;
    assert_msg.line = (s16)line; //OK, we'll mess up line numbers greater than 32767. I don't hope to live to see that day.
    assert_msg.signature = ASSERT_SIGNATURE;
    assert_msg.Reserved = ASSERT_SIGNATURE<<1; //for expediency, we'll trust 55AA.
}

/**
\brief A generic trap interface for fatal events

Fills the assert_t log structure to be analyzed on restart by mn_InitAssert().
Prints a nasty message on the LCD.

\param message - a pointer to store (e.g. filename of MN_ASSERT or location causing the exception)
\param line - if >= 0, line number; otherwise a negative trap id as defined in mntrapdef.h
*/

MN_NORETURN void mn_assert(const void *message, s32 line)
{
    mn_disable();   //game over: make it safe to call from anywhere
    mn_rt_assert_prep(message, line);
#if 1
    Reset_CPU();
#endif
    for(;;){} //in case we come here
}

/* This line marks the end of the source */
