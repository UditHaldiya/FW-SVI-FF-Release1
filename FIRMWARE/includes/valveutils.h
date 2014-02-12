/**
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file valveutils.h
    \brief Contains functions used by several other modules

    CPU: Any (with standard C compiler)

    OWNER: EP
    \n RCS: $Id$
    Created: 9/27/04 Author:  LS
*/

#ifndef VALVEUITILS_H_
#define VALVEUITILS_H_

#define BIAS_ERR 1u
//#define WAIT_FOR_SETPOINT true
//#define WAIT_FOR_STABLE false


extern u16 	   util_GetStableBias(s16 nStdPos, u16 nTime, u16 nPosDB, u16 nPresDB);
extern bool_t  util_WaitStablePosPres(u16 nTime, u16 nPosDB, u16 nPresDB);
extern bool_t  util_WaitForPos(u16 nTime,  u16 nNoiseDB , bool_t cFlag);

#endif /* VALVEUITILS_H_ */

/* end of the source */
