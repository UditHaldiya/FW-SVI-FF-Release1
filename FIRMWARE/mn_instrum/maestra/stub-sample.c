/**
    \brief A specific stub for unit-testing uut-sample.c

    This is an example of a specific test stub.

   Version control:
    $Date: 9/29/09 11:53p $
	$Change: 203 $
	$Id: //depot/maestra/stub-sample.c#2 $
*/
/* Copyright (C) 2007 MacroExpressions.
   Permission is granted to use and modify this file without limitations
*/
#include <stdlib.h>
#include <float.h>
#include "project.h"
#include "maestra_api.h"

/** \brief a sample stub function for bar() in uut-sample.c
\return a small floating point number (noise)
*/
double baz(void)
{
    double val = rand()*DBL_EPSILON;
    printf(MAESTRA_STUB_PREFIX "%s returns %f" MAESTRA_STUB_SUFFIX "\n", __FUNCTION__, val);
    return val;
}
