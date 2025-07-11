/**
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file config.h
    \brief Contains compilation control flags

    CPU: Any (with standard C compiler)

    OWNER: Price
    \n RCS: $Id$
    Created: 9/27/04 Author:  Price
*/

#ifndef CONFIG_H_
#define CONFIG_H_

#if (__CORE__ == 7)
#define CTX_SW_FLAG				0		// 1 = indirect context switch
#else
#define CTX_SW_FLAG				1		// 1 = indirect context switch
#endif



#   define GREEN_BOARD_BUTTONS     0       // Ernie's button poll
#   define DISPLAY_OPTION DISPLAY_OPTION_ST7036 //Sitronix ST7036

//#define NEW_TASK_SEQUENCE        1

//Options values: DO NOT CHANGE!
#define OPTION_ON      1
#define OPTION_OFF     2

//Diagnostic buffer options
#define DIAG_STORE_THREAD_SAFE OPTION_ON

//UI Display character capabilities set
#define DISPLAY_OPTION_ASCII 1
#define DISPLAY_OPTION_ST7036 2 //Sitronix ST7036

// Internal watchdog options
#define USE_INTERNAL_WATCHDOG OPTION_ON     // Internal WD only active when NDEBUG is defined

//Keyboard read policy
#define KBD_OPTION_EDGE_UP OPTION_OFF //ON=react to release, OFF=react to press

// reinit LCD periodically
#define PERIODIC_LCD_INIT OPTION_ON

//Cost-related options
#define SWOPTION_UI_LOWCOST_ENABLE_CUSTOM_CHARACT OPTION_ON
#define SWOPTION_UI_LOWCOST_FAULTS_SHOW OPTION_OFF
#define SWOPTION_UI_LOWCOST_FAULTS_CLEAR OPTION_OFF
#define SWOPTION_UI_LOWCOST_ENABLE_SPLIT_RANGE OPTION_ON

//Power-related options
#define PWR_MARGINAL_CANCELS_OOS_PROCESS OPTION_ON

//#define OPTION_INCLUDE_HANDWHEELS OPTION_OFF
//#define OPTION_INTEGRAL_TRIMMING  OPTION_OFF
//#define OPTION_LINEAR_POSCOMP OPTION_ON

//Transitional from 3.1.3/3.2.1 to lcx
//#define OLD_NVRAM
//#define OLD_OSPORT
//#define OLD_DEVMODE
//#define OLD_CYCLETASK
//#define OLD_NUMUTILS_API

//ON means TEMPR_OUT_OF_RANGE is set/cleared when we are outside/inside the cal range
//OFF means it is set/cleared when we are outside/inside dogmatic limits
#define TEMPR_CAL_RANGE_DEFINES_FAULT OPTION_ON

#endif /* CONFIG_H_ */

/* end of the source */

