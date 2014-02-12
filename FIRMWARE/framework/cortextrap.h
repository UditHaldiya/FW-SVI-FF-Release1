/*
Copyright 2013 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file cortextrap.h
    \brief Low-level traps handover API

    CPU: Any

    OWNER: AK

    \ingroup Framework
*/
#ifndef CORTEXTRAP_H_
#define CORTEXTRAP_H_
/*
    See ARM architecture manual. This is the structure of the stack frame created
    by the hardware when taking a trap or interrupt.  LR contains an indicator of
    the interrupted context and tells the hardware how to return.

    Table B1-8 EXC_RETURN definition of exception return behavior, no FP extension
        EXC_RETURN      Return to       Return stack
        0xFFFFFFF1      Handler mode    Main
        0xFFFFFFF9      Thread mode     Main
        0xFFFFFFFD      Thread mode     Process
*/
typedef struct stk_t
{
    //lint ++flb Hardware layout is predefined
    u32 r0, r1, r2, r3, r12, lr;
    void *ret;                      // PC address of trap or interrupt
    u32 xpsr;
    //lint --flb
} stk_t;

//lint -esym(765,Cortex_TrapC,NVIC_TrapC) called by Assembler routines below
//lint -esym(714,Cortex_TrapC,NVIC_TrapC) called by Assembler routines below
//lint -esym(759,Cortex_TrapC,NVIC_TrapC) called by Assembler routines below
extern void Cortex_TrapC(const volatile stk_t *spVal);
extern void NVIC_TrapC(const volatile stk_t *spVal);

//lint -esym(526, Cortex_Trap, NVIC_Trap) In Assembler
extern irqf_t Cortex_Trap;
extern irqf_t NVIC_Trap;
#endif //CORTEXTRAP_H_
/* This line marks the end of the source */
