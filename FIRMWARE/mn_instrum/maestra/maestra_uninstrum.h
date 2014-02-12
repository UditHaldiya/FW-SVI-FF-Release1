/**
    \brief maestra un-instrumentation of C keywords

This is a header designed to be #include'd AFTER the UUT in the harness file
(method 2 of instrumentation)

   Version control:
    $Date: 9/29/09 11:53p $
	$Change: 233 $
	$Id: //depot/maestra/maestra_instrum.h#3 $
*/
/* Copyright (C) 2007-2008 MacroExpressions.
   Permission is granted to freely use and modify this file provided this
   comment block is retained verbatim.
*/

#ifndef MAESTRA_UNINSTRUM_H_
#define MAESTRA_UNINSTRUM_H_

//Un-Instrumentation of if
#undef if

//Default ISTRUE macro
#undef ISTRUE
#define ISTRUE(condition) (condition)

//Un-Instrumentation of while
#undef while

//Un=Instrumentation of for
#undef for

//Un-Instrumentation of switch, case and default
#undef switch
#undef default
#undef case

//Un-Instrumentation of return
#undef return

//Un-Instrumentation of break
#undef break

//Un-Instrumentation of continue
#undef continue

//Un-Instrumentation of goto
#undef goto

//Un-Instrumentation of static
#undef static

//Un-Instrumentation of RMC/DC
#undef and
#undef or
#ifndef __cplusplus
#   define and &&
#   define or ||
#endif //__cplusplus

#endif //MAESTRA_UNINSTRUM_H_
