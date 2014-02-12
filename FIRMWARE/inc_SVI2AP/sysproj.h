/*
Copyright 2013 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file sysproj.h
    \brief header Processor-dependent routines for initializing the microprocessor

    CPU: STMicro STM32F10xxE

    OWNER: AK
*/
#ifndef SYSPROJ_H_
#define SYSPROJ_H_
extern void bios_SetVPBDividier_to2(bool_t divval);
//lint -esym(526,bsp_ServiceTimer) is an assembly language routine
/** \brief Assembly language Timer 0 interrupt hanfdler
*/
extern MN_IRQ void bsp_ServiceTimer(void);         // in assembly language
extern void bsp_Init(void);


#endif //SYSPROJ_H_
/* This line marks the end of the source */
