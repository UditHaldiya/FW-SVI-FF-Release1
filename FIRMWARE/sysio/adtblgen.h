/*
Copyright 2011 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/** \addtogroup ADTable  A/D sequence table module
\brief A/D sensor sequence table module
\htmlonly
<a href="../../../Doc/DesignDocs/Physical Design_ADTable Module.doc"> Design document </a><br>
<a href="../../../Doc/TestDocs/utplan_adtable_reset.doc"> Unit Test Report </a><br>
\endhtmlonly
*/
/**
    \file adtblgen.h
    \brief A/D sensor sequence table generator macros

    CPU: Any

    OWNER: AK

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/sysio/adtblgen.h $
    $Date: 12/07/11 6:49p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup ADTable
*/
/* $History: adtblgen.h $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 12/07/11   Time: 6:49p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/sysio
 * Generating macros
*/
#ifndef ADTABLGEN_H_
#define ADTABLGEN_H_
/*
    The A/D conversion tables.  Fast channels must occur first
    followed by slow channels, if any.  There must be at least
    one fast channel.  A list may contain only fast channels

    END_SEQ_ONE     Last fast channel in the list
    END_SEQ_TWO     Use for each slow channel.
    END_OF_SEQ      Use for the last channel in the list.  Note that the last
                    channel is implicitly a slow channel.

    N.B. Since table length affects Cycle task execution, care should be taken when
    constructing tables that might affect such things as sample rate.

USAGE:
static? const u8 name?[] =
{
    ADTABLE(FAST_CHANNELS(  comma_separated_list_of_fast_channels),
            NO_SLOW_CHANNELS_or_comma_separated_list_of_slow_channels
                )
};


*/
CONST_ASSERT((END_SEQ_TWO & (END_SEQ_TWO -1)) == 0);

#define ARGLIST(...) __VA_ARGS__ //A means to pass a list to a macro as a single parameter

/* We don't yet use it
#define ARGCOUNT(...) sizeof((u8[]){__VA_ARGS__})
*/

#define NO_SLOW_CHANNELS (END_SEQ_TWO+END_SEQ_ONE)
/** This macro, identical to ARGLIST, is a means of passing a list of fast channels
to the ADTABLE macro (see below)
*/
#define FAST_CHANNELS(...) __VA_ARGS__ //A means to pass a list to a macro as a single parameter

//lint -esym(750,SLOW_CHANNEL_*) OK if not referenced
/** THESE HELPER MACROS SHOULD NOT BE USED DIRECTLY
The SLOW_CHANNEL_x family of macros create an expanded list of all channels as follows:
all fast channels, first slow channel, marker, all fast channels, second slow channels, marker, etc.
*/
#define SLOW_CHANNEL_1(FAST, a) FAST, a, END_SEQ_TWO
#define SLOW_CHANNEL_2(FAST, a, ...) SLOW_CHANNEL_1(ARGLIST(FAST), a), SLOW_CHANNEL_1(ARGLIST(FAST), __VA_ARGS__)
#define SLOW_CHANNEL_3(FAST, a, ...) SLOW_CHANNEL_1(ARGLIST(FAST), a), SLOW_CHANNEL_2(ARGLIST(FAST), __VA_ARGS__)
#define SLOW_CHANNEL_4(FAST, a, ...) SLOW_CHANNEL_1(ARGLIST(FAST), a), SLOW_CHANNEL_3(ARGLIST(FAST), __VA_ARGS__)
#define SLOW_CHANNEL_5(FAST, a, ...) SLOW_CHANNEL_1(ARGLIST(FAST), a), SLOW_CHANNEL_4(ARGLIST(FAST), __VA_ARGS__)
#define SLOW_CHANNEL_6(FAST, a, ...) SLOW_CHANNEL_1(ARGLIST(FAST), a), SLOW_CHANNEL_5(ARGLIST(FAST), __VA_ARGS__)
#define SLOW_CHANNEL_7(FAST, a, ...) SLOW_CHANNEL_1(ARGLIST(FAST), a), SLOW_CHANNEL_6(ARGLIST(FAST), __VA_ARGS__)
#define SLOW_CHANNEL_8(FAST, a, ...) SLOW_CHANNEL_1(ARGLIST(FAST), a), SLOW_CHANNEL_7(ARGLIST(FAST), __VA_ARGS__)
/* Continue as needed...
#define SLOW_CHANNEL_9(FAST, a, ...) SLOW_CHANNEL_1(ARGLIST(FAST), a), SLOW_CHANNEL_8(ARGLIST(FAST), __VA_ARGS__)
#define SLOW_CHANNEL_10(FAST, a, ...) SLOW_CHANNEL_1(ARGLIST(FAST), a), SLOW_CHANNEL_9(ARGLIST(FAST), __VA_ARGS__)
*/

/**  THIS HELPER MACRO SHOULD NOT BE USED DIRECTLY
A common wrapper for SLOW_CHANNEL_x macros;
n must be a literal number that matches the actual number of slow channels, except that
if n=0, a single parameter can be anything, like IGNORE_ME.
If n is wrong, compilation error will result
*/
#define SLOW_CHANNEL(FAST, n, ...) CAT(SLOW_CHANNEL_, n) (ARGLIST(FAST), __VA_ARGS__)

/** Creates an expanded A/D channel list as a byte array with the following format:
- Number of A/D runs to complete the whole sequence
- All fast channels
- First slow channel
- Stop marker
- All fast channels
- Second slow channel
- Stop marker
...
- All fast channels
- Last slow channel
- Stop-and-Reset marker which is greater than stop marker
*/
#define ADTABLE_1(FAST, n, ...) \
        n, \
        SLOW_CHANNEL(ARGLIST(FAST), n, __VA_ARGS__) + END_SEQ_ONE,

#define COUNT(...) SELECT_POSITION_N_OF(__VA_ARGS__,LISTN())
#define SELECT_POSITION_N_OF(...) SELECT_POSITION_N(__VA_ARGS__)
#define SELECT_POSITION_N(dummy0,dummy1,dummy2,dummy3,dummy4,dummy5,dummy6,dummy7,dummy8,dummy9,x,...) x
#define LISTN() 10,9,8,7,6,5,4,3,2,1,0

#define ADTABLE(FAST, ...) ADTABLE_1(ARGLIST(FAST), COUNT(__VA_ARGS__), __VA_ARGS__)

#endif //ADTABLGEN_H_
/* This line marks the end of the source */
