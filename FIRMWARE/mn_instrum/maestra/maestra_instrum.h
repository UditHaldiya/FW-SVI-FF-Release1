/**
    \brief maestra instrumentation of C keywords

This is a header designed to be defined as INSTRUM_HEADER in the compiler
command line used for compiling a unit under test.

You can replace any of the definitions with your own if these are not
satisfactory.
The preferred way of doing so is to create your own UUT-specific instrumentation
header, say, foo_instrum.h, include this header and replace the definitions.
For instance, to define static as extern, foo_instrum.h would contain
    #include "maestra_instrum.h"
    #undef static
    #define static //nothing //never recommended

   Version control:
    $Date: 9/29/09 11:53p $
	$Change: 251 $
	$Id: //depot/maestra/maestra_instrum.h#7 $
*/
/* Copyright (C) 2007-2008 MacroExpressions.
   Permission is granted to freely use and modify this file provided this
   comment block is retained verbatim.
*/

#ifndef MAESTRA_INSTRUM_H_
#define MAESTRA_INSTRUM_H_

/* NOTE: Some compiler adaptation here.
A compiler may have an annoing warning about a missing return statement,
which happens on always-false path.
It was previously suppressed by an endless loop for(;;) but with the new
instrumentation of for the compiler no longer expected to recognize this.
*/
#define MAESTRA_SUPPRESS_NORETURN_WARNING exit(1)
#include <stdlib.h>
//Or it could be
//#define MAESTRA_SUPPRESS_NORETURN_WARNING /*nothing*/

#include "maestra_implem.h"

/* Comment this out if you don't have or do not want
variadic macros.
Instrumentation of expressions with commas will then fail
*/
#define MAESTRA_HAVE_VARIADIC_MACROS

/* Comment this out if you have variadic macros but want
instrumentation of expressions with commas fail, except the 
`for' construct. Uncomment if you hate the comma operator.
*/
//#define MAESTRA_PROHIBIT_COMMA

/* MAESTRA_CAT is a concatenation of expanded parameters;
   there is no standard name for this common thing, so
   we make our own to avoid potential name collision.
*/
#define MAESTRA_CAT1(a, b) a ## b
#define MAESTRA_CAT(a, b) MAESTRA_CAT1(a, b)

#ifdef MAESTRA_IF
//Instrumentation of if
#if defined(MAESTRA_HAVE_VARIADIC_MACROS) && !defined(MAESTRA_PROHIBIT_COMMA)
#define if(...) \
	if(maestra_condition("if", #__VA_ARGS__, (__VA_ARGS__)!=0, \
	__FILE__, __LINE__, __FUNCTION__))
#else
#define if(condition) \
	if(maestra_condition("if", #condition, (condition)!=0, \
	__FILE__, __LINE__, __FUNCTION__))
#endif //comma business
#endif //MAESTRA_IF

//ISTRUE macro
#if defined(MAESTRA_HAVE_VARIADIC_MACROS) && !defined(MAESTRA_PROHIBIT_COMMA)
#define ISTRUE(...) \
	maestra_condition("?: or for", #__VA_ARGS__, (__VA_ARGS__)!=0, \
	__FILE__, __LINE__, __FUNCTION__)
#else
#define ISTRUE(condition) \
	maestra_condition("?: or for", #condition, (condition)!=0, \
	__FILE__, __LINE__, __FUNCTION__)
#endif //comma business

#ifdef MAESTRA_WHILE
//Instrumentation of while
#if defined(MAESTRA_HAVE_VARIADIC_MACROS) && !defined(MAESTRA_PROHIBIT_COMMA)
#define while(...) \
	while(maestra_while(#__VA_ARGS__, (__VA_ARGS__)!=0, \
	__FILE__, __LINE__, __FUNCTION__))
#else
#define while(condition) \
	while(maestra_while(#condition, (condition)!=0, \
	__FILE__, __LINE__, __FUNCTION__))
#endif //comma business
#endif //MAESTRA_WHILE

#ifdef MAESTRA_FOR
//Instrumentation of for
#if defined(MAESTRA_HAVE_VARIADIC_MACROS)
#define for(...) \
    if(maestra_announce("for", 1, __FILE__, __LINE__, __FUNCTION__)) \
    for(__VA_ARGS__) \
        if(maestra_announce("loop body", 0, __FILE__, __LINE__, __FUNCTION__)) ; \
        else
#else
#define for(triplet) \
    if(maestra_announce("for", 1, __FILE__, __LINE__, __FUNCTION__)) \
    for(triplet) \
        if(maestra_announce("loop body", 0, __FILE__, __LINE__, __FUNCTION__)) ; \
        else
#endif //comma business
#endif //MAESTRA_FOR

#ifdef MAESTRA_SWITCH
//Instrumentation of switch
/* In a hope that long long is enough to promote any practical
   switch control expression to...
*/
#if defined(MAESTRA_HAVE_VARIADIC_MACROS) && !defined(MAESTRA_PROHIBIT_COMMA)
#define switch(...) \
	switch(maestra_announce_ll("switch", #__VA_ARGS__, (__VA_ARGS__), \
	__FILE__, __LINE__, __FUNCTION__))
#else
#define switch(x) \
	switch(maestra_announce_ll("switch", #x, (x), \
	__FILE__, __LINE__, __FUNCTION__))
#endif //comma business
#endif //MAESTRA_SWITCH

#ifdef MAESTRA_DEFAULT
//Instrumentation of default
#define default \
    default: if(maestra_announce("default", 0, __FILE__, __LINE__, __FUNCTION__)) \
        goto MAESTRA_CAT(maestra_label, __LINE__); else MAESTRA_CAT(maestra_label, __LINE__)
#endif //MAESTRA_DEFAULT

#ifdef MAESTRA_CASE
//Instrumentation of case
//This implementation doesn't allow two cases in a single line
#define case(x) \
	case(x): if((maestra_announce_ll("case", #x, (x), \
	__FILE__, __LINE__, __FUNCTION__), maestra_false)) \
        goto MAESTRA_CAT(maestra_label, __LINE__); else MAESTRA_CAT(maestra_label, __LINE__)
#endif //MAESTRA_CASE

#ifdef MAESTRA_RETURN
#define return \
    if(maestra_announce("return", 0, __FILE__, __LINE__, __FUNCTION__)) \
        {MAESTRA_SUPPRESS_NORETURN_WARNING;} else return
#endif //MAESTRA_RETURN

#ifdef MAESTRA_BREAK
//Instrumentation of break
#define break \
    if(maestra_announce("break", 0, __FILE__, __LINE__, __FUNCTION__)); else break
#endif //MAESTRA_BREAK

#ifdef MAESTRA_CONTINUE
//Instrumentation of continue
#define continue \
    if(maestra_announce("continue", 0, __FILE__, __LINE__, __FUNCTION__)); else continue
#endif //MAESTRA_CONTINUE

#ifdef MAESTRA_GOTO
//Instrumentation of goto
#define goto \
    if(maestra_fail("goto", 0, __FILE__, __LINE__, __FUNCTION__)); else goto
#endif //MAESTRA_GOTO

#ifdef MAESTRA_STATIC
//Instrumentation of static
#define static extern //always better than /*nothing*/
#endif //MAESTRA_STATIC

#ifdef MAESTRA_ANDOR
//Instrumentation of and and or for RMC/DC
#ifdef __cplusplus
#if defined(MAESTRA_HAVE_VARIADIC_MACROS) && !defined(MAESTRA_PROHIBIT_COMMA)
#define or(...) \
    or maestra_logical_orand(".OR.", #__VA_ARGS__, (__VA_ARGS__)!=0, __FILE__, __LINE__, __FUNCTION__)

#define and(...) \
    and maestra_logical_orand(".AND.", #__VA_ARGS__, (__VA_ARGS__)!=0, __FILE__, __LINE__, __FUNCTION__)
#else
#define or(e) \
    or maestra_logical_orand(".OR.", #e, (e)!=0, __FILE__, __LINE__, __FUNCTION__)

#define and(e) \
    and maestra_logical_orand(".AND.", #e, (e)!=0, __FILE__, __LINE__, __FUNCTION__)
#endif //comma business
#else //no __cplusplus
#include <iso646.h>
#undef and
#undef or
#if defined(MAESTRA_HAVE_VARIADIC_MACROS) && !defined(MAESTRA_PROHIBIT_COMMA)
#define or(...) \
    || maestra_logical_orand(".OR.", #__VA_ARGS__, (__VA_ARGS__)!=0, __FILE__, __LINE__, __FUNCTION__)

#define and(...) \
    && maestra_logical_orand(".AND.", #__VA_ARGS__, (__VA_ARGS__)!=0, __FILE__, __LINE__, __FUNCTION__)
#else
#define or(e) \
    || maestra_logical_orand(".OR.", #e, (e)!=0, __FILE__, __LINE__, __FUNCTION__)

#define and(e) \
    && maestra_logical_orand(".AND.", #e, (e)!=0, __FILE__, __LINE__, __FUNCTION__)
#endif //comma business
#endif //__cplusplus
#endif //MAESTRA_ANDOR

#endif //MAESTRA_INSTRUM_H_
