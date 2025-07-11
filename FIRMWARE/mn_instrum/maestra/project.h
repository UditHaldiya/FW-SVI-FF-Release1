/**
    \brief A sample common project header.

    Demonstrates implementation of maestra instrumentation

   Version control:
    $Date: 9/29/09 11:53p $
	$Change: 233 $
	$Id: //depot/maestra/project.h#3 $
*/
/* Copyright (C) 2007 MacroExpressions.
   Permission is granted to use and modify this file without limitations
*/

#ifndef PROJECT_H
#define PROJECT_H

//Include common system files as needed
#include <stdio.h>

//Include common project files as needed
// ------ from the UUT 
extern int foo(int); //we'll test it
extern double bar(int x); //we'll test it
extern void w(void); //we'll test it
extern int andor(int x[4]); //we'll test it

//from elsewhere - we'll stub it out since we don't have it in a library
extern double baz(void);

//This is the code instrumentation plugin
#include "maestra.h"

#endif //PROJECT_H
