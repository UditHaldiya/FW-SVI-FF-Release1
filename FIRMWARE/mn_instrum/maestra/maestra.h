/**
    \brief maestra "magic header" to be included in the project-wide header

    This file exposes the implementations of C keywords instrumentation to
    the unit under test.
    A UUT is any C source file compiled with INSTRUM_HEADER appropriately
    defined in the command line, e.g. as
    INSTRUM_HEADER="maestra_instrum.h"

    If you don't like exposing some of the instrumentations,
    you can remove them here.

    Note that e.g. do and else are not instrumented; there seems
    to be very little value in instrumenting them.

   Version control:
    $Date: 9/29/09 11:53p $
	$Change: 238 $
	$Id: //depot/maestra/maestra.h#4 $
*/
/* Copyright (C) 2007 MacroExpressions.
   Permission is granted to freely use and modify this file provided this
   comment block is retained verbatim.
*/

#ifndef MAESTRA_H_
#define MAESTRA_H_

/* Here is the trick: if INSTRUM_HEADER is not defined, it is not
   included, so this header has no effect except that it defines
   the normal version of ISTRUE macro
*/
#ifdef INSTRUM_HEADER

//Define every keyword we want to abuse
#define MAESTRA_IF //Instrumentation of if requested
#define MAESTRA_SWITCH //Instrumentation of switch requested
#define MAESTRA_DEFAULT //Instrumentation of default requested
#define MAESTRA_CASE //Instrumentation of case requested
#define MAESTRA_FOR //Instrumentation of for requested
#define MAESTRA_WHILE //Instrumentation of while requested
#define MAESTRA_RETURN //Instrumentation of return requested
#define MAESTRA_BREAK //Instrumentation of break requested
#define MAESTRA_CONTINUE //Instrumentation of continue requested
#define MAESTRA_GOTO //Instrumentation of goto requested
#define MAESTRA_STATIC //Instrumentation of static requested

//Define desired instrumentation for (Recursive) MC/DC
#define MAESTRA_ANDOR //Instrumentation of and and or requested

#include INSTRUM_HEADER

#endif /*INSTRUM_HEADER*/

//Normal (non-instrumented) case of ISTRUE
#ifndef ISTRUE
#   define ISTRUE(a) (a)
#endif

#endif //MAESTRA_H_
