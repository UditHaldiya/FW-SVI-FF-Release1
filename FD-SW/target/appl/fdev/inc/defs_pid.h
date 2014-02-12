/*
Copyright 2013 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file defs_pid.h
    \brief definitions of PID implementations

    CPU: Any

    OWNER: CH
*/
#ifndef DEFS_PID_H
#define DEFS_PID_H

/*******************************************************************************
*** Global definitions
*******************************************************************************/

/*! \def ALGO_IDEAL()
    Definition for parameter ALGORITHM, sets it to IDEAL */
	#define ALGO_IDEAL        (0)
/*! \def ALGO_SERIES()
    Definition for parameter ALGORITHM, sets it to SERIES */
	#define ALGO_SERIES     (1)
/*! \def ALGO_PARALLEL()
    Definition for parameter ALGORITHM, sets it to PARALLEL */
	#define ALGO_PARALLEL     (2)
/*! \def ALGO_I_PD()
    Definition for parameter ALGORITHM, sets it to I-PD */
	#define ALGO_I_PD         (3)
/*! \def ALGO_IP_D()
    Definition for parameter ALGORITHM, sets it to IP-D */
	#define ALGO_IP_D         (4)

/*******************************************************************************
*** Definitions for Block Error Description
*******************************************************************************/
/*! \def BLK_ERR_TIMECONSTERR
    definition for errorbits in parameter BLOCK_ERR_DESC error */
	#define BLK_ERR_TIMECONSTERR    (0x00000008)

/*! \def BLK_ERR_TIMECONSTWARN
    definition for errorbits in parameter BLOCK_ERR_DESC warning */
	#define BLK_ERR_TIMECONSTWARN   (0x00000004)

/*! \def BLK_ERRTIMEERRMIN
    definition for minimum range of time constants --> error value */
	#define BLK_ERRTIMEERRMIN		(4)

/*! \def BLK_ERRTIMEWARNMIN
    definition for minimum range of time constants --> warning value */
	#define BLK_ERRTIMEWARNMIN		(10)

/*******************************************************************************
*** Definitions for conditions for bump less transfers
*******************************************************************************/

/*! \def CONDNO
    conditions for re-calculating the integral term for bumpless transfer: no condition */
	#define CONDNO			(0x00)		/* no condition */
/*! \def CONDALGOCHG
    conditions for re-calculating the integral term for bumpless transfer: algorithm changed */
	#define CONDALGOCHG		(0x01) 		/* switching the parameter ALGORITHM */
/*! \def CONDMANAUTO
    conditions for re-calculating the integral term for bumpless transfer: block mode changed from MANUAL */
	#define CONDMANAUTO		(0x02) 		/* switching the Block Mode from MAN to AUTO */
/*! \def CONDMODECHG
    conditions for re-calculating the integral term for bumpless transfer: block mode changed between AUTO,CAS,RCAS*/
	#define CONDMODECHG		(0x04)		/* switching Block Mode between AUTO,CAS,RCAS */
/*! \def CONDROUTAUTO
    conditions for re-calculating the integral term for bumpless transfer: block mode changed from ROUT */
	#define CONDROUTAUTO	(0x08) 		/* switching the Block Mode from ROUT to AUTO */

/*******************************************************************************
*** External functions (export)
*******************************************************************************/



#else
  #error "Double include file !"
#endif /*DEFS_PID_H*/
/* This line marks the end of the source */

