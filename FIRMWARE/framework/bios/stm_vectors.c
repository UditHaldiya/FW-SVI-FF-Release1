/**
Copyright 2011 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file stm_vectors.c

    \brief The ARM Cortex and NVIC vector table for the STM32F10xx CPU

    OWNER:
    CPU: STM32F10xx Cortex

    $Revision:  $
*/
#include "mnwrap.h"
#include "serial.h"
#include "stmcommon.h"
#include "gpio.h"
#include "cortextrap.h"
#include "bsp.h"

//lint -esym(754,vectors_t::*) No explicit member access OK for a memory-mapped peripheral
typedef struct vectors_t
{
    void           *stackPtr;
    irqf_t *Fnct[75];
    const u16      *crcPtr;
    const u16      *dummyPtr;
} vectors_t;

#pragma language=extended
#pragma segment="CSTACK"

#define TRAP1   Cortex_Trap
#define TRAP2   NVIC_Trap

#pragma section=".intvec"
#pragma location=".intvec"

const  vectors_t __vector_table[] =
{
    // extra level of braces is needed by Lint
    {
        .stackPtr = __sfe("CSTACK"),                                   //  0, Initial Stack Pointer
        .Fnct =
        {
            __iar_program_start,                            //  1, PC start value.
            TRAP1,                                          //  2, NMI.
            TRAP1,                                          //  3, Hard Fault.
            TRAP1,                                          //  4, Memory Management.
            TRAP1,                                          //  5, Bus Fault.
            TRAP1,                                          //  6, Usage Fault.
            TRAP1,                                          //  7, Reserved.
            TRAP1,                                          //  8, Reserved.
            TRAP1,                                          //  9, Reserved.
            TRAP1,                                          // 10, Reserved.
            TRAP1,                                          // 11, SVCall.
            TRAP1,                                          // 12, Debug Monitor.
            TRAP1,                                          // 13, Reserved.
            PendSV_Handler,                                 /* 14, PendSV Handler.                                  */
            SysTick_Handler,                                // 15, uC/OS-II Tick ISR Handler.

            // Peripheral (NVIC) interrupts
            TRAP2,                                          // 16, INTISR[  0]  Window Watchdog.
            TRAP2,                                          // 17, INTISR[  1]  PVD through EXTI Line Detection.
            TRAP2,                                          // 18, INTISR[  2]  Tamper Interrupt.
            TRAP2,                                          // 19, INTISR[  3]  RTC Global Interrupt.
            TRAP2,                                          // 20, INTISR[  4]  FLASH Global Interrupt.
            TRAP2,                                          // 21, INTISR[  5]  RCC Global Interrupt.
            TRAP2,                                          // 22, INTISR[  6]  EXTI Line0 Interrupt.
            TRAP2,                                          // 23, INTISR[  7]  EXTI Line1 Interrupt.
            TRAP2,                                          // 24, INTISR[  8]  EXTI Line2 Interrupt.
            TRAP2,                                          // 25, INTISR[  9]  EXTI Line3 Interrupt.
            TRAP2,                                          // 26, INTISR[ 10]  EXTI Line4 Interrupt.
            TRAP2,                                          // 27, INTISR[ 11]  DMA Channel1 Global Interrupt.
            TRAP2,                                          // 28, INTISR[ 12]  DMA Channel2 Global Interrupt.
            TRAP2,                                          // 29, INTISR[ 13]  DMA Channel3 Global Interrupt.
            TRAP2,                                          // 30, INTISR[ 14]  DMA Channel4 Global Interrupt.
            TRAP2,                                          // 31, INTISR[ 15]  DMA Channel5 Global Interrupt.
            TRAP2,                                          // 32, INTISR[ 16]  DMA Channel6 Global Interrupt.
            TRAP2,                                          // 33, INTISR[ 17]  DMA Channel7 Global Interrupt.
            TRAP2,                                          // 34, INTISR[ 18]  ADC1 & ADC2 Global Interrupt.
            TRAP2,                                          // 35, INTISR[ 19]  USB High Prio / CAN TX  Interrupts.
            TRAP2,                                          // 36, INTISR[ 20]  USB Low  Prio / CAN RX0 Interrupts.
            TRAP2,                                          // 37, INTISR[ 21]  CAN RX1 Interrupt.
            TRAP2,                                          // 38, INTISR[ 22]  CAN SCE Interrupt.
            TRAP2,                                          // 39, INTISR[ 23]  EXTI Line[9:5] Interrupt.
            TRAP2,                                          // 40, INTISR[ 24]  TIM1 Break  Interrupt.
            TRAP2,                                          // 41, INTISR[ 25]  TIM1 Update Interrupt.
            TRAP2,                                          // 42, INTISR[ 26]  TIM1 Trig & Commutation Interrupts.
            TRAP2,                                          // 43, INTISR[ 27]  TIM1 Capture Compare Interrupt.
            TRAP2,                                          // 44, INTISR[ 28]  TIM2 Global Interrupt.
            TRAP2,                                          // 45, INTISR[ 29]  TIM3 Global Interrupt.
            TRAP2,                                          // 46, INTISR[ 30]  TIM4 Global Interrupt.
            TRAP2,                                          // 47, INTISR[ 31]  I2C1 Event  Interrupt.
            TRAP2,                                          // 48, INTISR[ 32]  I2C1 Error  Interrupt.
            TRAP2,                                          // 49, INTISR[ 33]  I2C2 Event  Interrupt.
            TRAP2,                                          // 50, INTISR[ 34]  I2C2 Error  Interrupt.
            TRAP2,                                          // 51, INTISR[ 35]  SPI1 Global Interrupt.
            TRAP2,                                          // 52, INTISR[ 36]  SPI2 Global Interrupt.
            UART1_IRQHandler,                               // 53, INTISR[ 37]  USART1 Global Interrupt.
            // Temporalily Set the UART 2 IRQ Handler
            UART2_IRQHandler,                               // 54, INTISR[ 38]  USART2 Global Interrupt.
            TRAP2,                                          // 55, INTISR[ 39]  USART3 Global Interrupt.

            EXTI_IRQHandler,                                // 56, INTISR[ 40]  EXTI Line [15:10] Interrupts.

            TRAP2,                                          // 57, INTISR[ 41]  RTC Alarm EXT Line Interrupt.
            TRAP2,                                          // 58, INTISR[ 42]  USB Wakeup from Suspend EXTI Int.
            TRAP2,                                          // 59, INTISR[ 43]  TIM8 Break Interrupt.
            TRAP2,                                          // 60, INTISR[ 44]  TIM8 Update Interrupt.
            TRAP2,                                          // 61, INTISR[ 45]  TIM8 Trigg/Commutation Interrupts.
            TRAP2,                                          // 62, INTISR[ 46]  TIM8 Capture Compare Interrupt.
            TRAP2,                                          // 63, INTISR[ 47]  ADC3 Global Interrupt.
            TRAP2,                                          // 64, INTISR[ 48]  FSMC Global Interrupt.
            TRAP2,                                          // 65, INTISR[ 49]  SDIO Global Interrupt.
            TRAP2,                                          // 66, INTISR[ 50]  TIM5 Global Interrupt.
            TRAP2,                                          // 67, INTISR[ 51]  SPI3 Global Interrupt.
            TRAP2,                                          // 68, INTISR[ 52]  UART4 Global Interrupt.
            Hart_ISR,                               // 69, INTISR[ 53]  UART5 Global Interrupt.
            TRAP2,                                          // 70, INTISR[ 54]  TIM6 Global Interrupt.
            TRAP2,                                          // 71, INTISR[ 55]  TIM7 Global Interrupt.
            TRAP2,                                          // 72, INTISR[ 56]  DMA2 Channel1 Global Interrupt.
            TRAP2,                                          // 73, INTISR[ 57]  DMA2 Channel2 Global Interrupt.
            TRAP2,                                          // 74, INTISR[ 58]  DMA2 Channel3 Global Interrupt.
            TRAP2                                           // 75, INTISR[ 59]  DMA2 Channel4/5 Global Interrups.
        },
        .crcPtr = &mychecksum,
        .dummyPtr = NULL
    }
};

// end of source
