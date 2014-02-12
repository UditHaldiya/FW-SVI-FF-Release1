/*
Copyright 2013 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file controller_pid.h
    \brief API of PID implementations

    CPU: Any

    OWNER: CH
*/
#ifndef CONTROLLER_PID_H
#define CONTROLLER_PID_H

/*******************************************************************************
*** Global definitions
*******************************************************************************/


/*******************************************************************************
*** External functions (export)
*******************************************************************************/

FLOAT pid_algo(T_PIDFB_DATA * p_pidfb_data, const  T_PIDFB * p_pidfb, T_PIDFB_CONTROL_DATA * p_control_data);
void  pid_prm(T_PIDFB_DATA * p_pidfb_data, T_PIDFB * p_pidfb);




#else
  #error "Double include file !"
#endif /*CONTROLLER_PID_H*/ 
/* This line marks the end of the source */

