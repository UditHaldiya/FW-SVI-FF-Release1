/*****************************************************************************
*                                                                            *
*                     SOFTING Industrial Automation GmbH                     *
*                          Richard-Reitzner-Allee 6                          *
*                                D-85540 Haar                                *
*                        Phone: ++49-89-4 56 56-0                            *
*                          Fax: ++49-89-4 56 56-3 99                         *
*                                                                            *
*                            SOFTING CONFIDENTIAL                            *
*                                                                            *
*                     Copyright (C) SOFTING IA GmbH 2012                     *
*                             All Rights Reserved                            *
*                                                                            *
* NOTICE: All information contained herein is, and remains the property of   *
*   SOFTING Industrial Automation GmbH and its suppliers, if any. The intel- *
*   lectual and technical concepts contained herein are proprietary to       *
*   SOFTING Industrial Automation GmbH and its suppliers and may be covered  *
*   by German and Foreign Patents, patents in process, and are protected by  *
*   trade secret or copyright law. Dissemination of this information or      *
*   reproduction of this material is strictly forbidden unless prior         *
*   written permission is obtained from SOFTING Industrial Automation GmbH.  *
******************************************************************************
******************************************************************************
*                                                                            *
* PROJECT_NAME             Softing FF/PA FD 2.42                             *
*                                                                            *
* VERSION                  FF - 2.42                                         *
*                          PA - 2.42 (beta)                                  *
*                                                                            *
* DATE                     16. April 2012                                    *
*                                                                            *
*****************************************************************************/

/*********************************************************************
*                SEGGER MICROCONTROLLER SYSTEME GmbH                 *
*        Solutions for real time microcontroller applications        *
**********************************************************************

File        : RTOSInit.c   (for Mitsubishi M16C)
Purpose     : Initializes and handles the hardware for embOS as far
              as required by embOS.
              Feel free to modify this file acc. to your
              target system.
---------------------------END-OF-HEADER------------------------------
*/

#include "keywords.h"
#define  MODULE_ID      (COMP_HW + MOD_HWOS)

INCLUDES
#include "base.h"
#include "hw_cfg.h"
#include "hw_if.h"
#include "cpu.h"

#include "osif.h"

#include "except.h"

#include "stm32f10x_tim.h"
#include "core_cm3.h"


#if OS_USE_JLINKMEM
  // Size of the communication buffer for JLINKMEM
const OS_U32 OS_JLINKMEM_BufferSize = 32;
#else
const OS_U32 OS_JLINKMEM_BufferSize = 0;     // Communication not used
#endif


#ifndef OSIF_EMBOS /* this file is only valid if embOS is used */

#error HW_OS.C valid only for use with embOS

#else


/*
**********************************************************************
*
*                    Configuration
*
**********************************************************************
*/
                    
#ifndef OS_FSYS
  #define OS_FSYS XCLOCK
#endif

#ifndef   OS_TICK_FREQ
  #define OS_TICK_FREQ (1000)
#endif


/* Select UART for embOSView.
   If you do not want (or can not due to hardware limitations) to dedicate
   a UART to embOSView, please define it to be -1
   Currently the standard code enables UART 1 as default
   and supports UART0 and UART1
*/



#define SYS_PENDSTSET_BIT         (26)
#define NVIC_VTOR                 (*(volatile unsigned long *) (0xE000ED08))

/****** MAP UART initialization function ****************************/

#if  (OS_UART_USED || OS_USE_JLINKMEM)
  #define OS_COM_INIT() OS_COM_Init()
#else
  #define OS_COM_INIT()
#endif


#define NO_OS_UART  42    /* 42 is the answer ;-) */

#if defined(OS_LIBMODE_DT) || defined(OS_LIBMODE_DP) || defined(OS_LIBMODE_SP)
  #ifndef OS_UART
    #define OS_UART   UART_1
  #endif
  #ifndef OS_BAUDRATE
    #define OS_BAUDRATE 38400UL
  #endif
#else
  #define OS_UART   NO_OS_UART
#endif

/*
**********************************************************************
*
*                    OS_InitHW
*
**********************************************************************

Initialize the hardware required for the OS to run. This will work
on any target hardware, but may have to be tailored a little
(regarding the clock frequency). Of course the same holds true
if for some reason you choose to use an other timer.
*/

/* uses timer TIM14 currently */

void OS_InitHW(void) {
#if (OS_FSYS > 10000000UL)
  #define OS_TICK_PERIOD  1     /* 1 msec per OS tick                   */
#else
  #define OS_TICK_PERIOD  5     /* 5 msec per OS tick                   */
#endif /* OS_FSYS */
 
  OS_IncDI();
  //
  // We assume, the PLL and core clock was already set by the SystemInit() function
  // which was called from the startup code
  // Therefore, we don't have to initailize any hardware here,
  // we just ensure that the system clock variable is updated and then
  // set the periodic system timer tick for embOS.
  //

  //SystemCoreClockUpdate();                             // Ensure, the SystemCoreClock is set
  if (SysTick_Config (OS_FSYS / OS_TICK_FREQ)) { // Setup SysTick Timer for 1 msec interrupts
    while (1);                                         // Handle Error
  }
  //
  // Initialize NVIC vector base address. Might be necessary for RAM targets or application not running from 0
  //
  //#ifdef __ICCARM__
  //  #define __Vectors    __vector_table
  //#else
  //  extern unsigned char __Vectors;
  //#endif
  //NVIC_VTOR = (OS_U32)&__Vectors;
  //
  // Set the interrupt priority for the system timer to 2nd lowest level to ensure the timer can preempt PendSV handler
  //
  
  NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 2);

  OS_COM_INIT();
  OS_DecRI();
}


/*
**********************************************************************
*
*                     Idle task  (OS_Idle)
*
**********************************************************************

 Please note:
 This is basically the "core" of the idle task.
 This core loop can be changed, but:
 The idle task does not have a stack of its own, therefor no
 functionality should be implemented that relies on the stack
 to be preserved. However, a simple program loop can be programmed
 (like toggeling an output or incrementing a counter)
*/

void OS_Idle(void) {          /* Idle task: No task is ready to exec    */
  for (;;);
}

/*
**************************************************************
*
*             Run-time error reaction  (OS_Error)
*
**************************************************************

 Run-time error reaction

 When this happens, a fatal error has occured and the kernel
 can not continue. In linux, the equivalent would be a
 "kernel panic"

 This routine can be modified to suit your needs ...
 E.g. a red LED could light up. When using an
 emulator, you may set a breakpoint here.
 In the release builds of the library (R), this routine is not required
 (as no checks are performed).
 In the stack check builds (S/SP), only error 120 may occur.
 In the debug builds(D/DP), all of the listed errors may occur.

  The following are the current errorcodes (V2.80) :
  (Please refer to the documentation for more info)

OS_ERR_STACK                          (120)
*** invalid or non-initialized data structures ***
OS_ERR_INV_TASK                       (128)
OS_ERR_INV_TIMER                      (129)
OS_ERR_INV_MAILBOX                    (130)
OS_ERR_INV_CSEMA                      (132)
OS_ERR_INV_RSEMA                      (133)

*** Using GetMail1 or PutMail1 or GetMailCond1 or PutMailCond1 on a non-1 byte mailbox  ***
OS_ERR_MAILBOX_NOT1                   (135)

***internal errors, please contact SEGGER Microcontrollersysteme ***
OS_ERR_MAILBOX_NOT_IN_LIST            (140)
OS_ERR_TASKLIST_CORRUPT               (142)


*** not matching routine calls or macro usage ***
OS_ERR_UNUSE_BEFORE_USE               (150)
OS_ERR_LEAVEREGION_BEFORE_ENTERREGION (151)
OS_ERR_LEAVEINT                       (152)
OS_ERR_DICNT                          (153)
OS_ERR_INTERRUPT_DISABLED             (154)

*** not a legal system call during interrupt ***
OS_ERR_ILLEGAL_IN_ISR                 (160)
*** not a legal system call during timer ***
OS_ERR_ILLEGAL_IN_TIMER               (161)

** double used data structures **
OS_ERR_2USE_TASK                      (170)
OS_ERR_2USE_TIMER                     (171)
OS_ERR_2USE_MAILBOX                   (172)
OS_ERR_2USE_BSEMA                     (173)
OS_ERR_2USE_CSEMA                     (174)
OS_ERR_2USE_RSEMA                     (175)

*/

void OS_Error(int ErrCode) {
  OS_EnterRegion();     /* Avoid further task switches */
  OS_DICnt =0;          /* Allow interrupts so we can communicate */
  OS_EI();
  _HW_FATAL(ERR_EMBOS, (USIGN32) ErrCode);
}

/*
**********************************************************************
*
*                    Get time [cycles]
*
**********************************************************************

    This routine is required for task-info via embOS Viewer.
    It returns the system time in clock cycles.
*/

OS_U32 OS_GetTime_Cycles(void) {
  unsigned int t_cnt;
  OS_U32 time;
  time  = OS_Time;

  t_cnt = (OS_FSYS/1000) - SysTick->VAL;
  if (SysTick->CTRL & (1uL << SYS_PENDSTSET_BIT)) {  /* Missed a counter interrupt? */
    t_cnt = (OS_FSYS/1000) - SysTick->VAL;     /* Adjust result               */
    time++;
  }
  return (OS_FSYS/1000) * time + t_cnt;
}

/*
**********************************************************************
*
*                    OS_ConvertCycles2us
*
**********************************************************************

Purpose:
   Convert Cycles into micro seconds. (For profiling only)

   If you have a clock frequency which is not a multiple of 1 MHz,
   you may have to modify this routine in order to get proper
   diagonstics.
Please note:
   This routine is required for profiling only.
   It does not affect operation of the OS.
*/

OS_U32 OS_ConvertCycles2us(OS_U32 Cycles) {
  return Cycles/(OS_FSYS/1000000);
}

#if OS_VERSION  >= 30608
/*********************************************************************
*
*                    OS_Tick interrupt
*
**********************************************************************
*/
void SysTick_Handler (void)
{
  OS_EnterNestableInterrupt();
  OS_TICK_Handle();
  #if OS_USE_JLINKMEM
    JLINKMEM_Process();
  #endif
  OS_LeaveNestableInterrupt();
}
#endif


#if OS_UART_USED
/*********************************************************************
*
*       Communication for OSView via UART (optional)
*
**********************************************************************
*/
#if (OS_UART == 0)
  #define OS_COM_IsrHandler USART1_IRQHandler
  #define USART_BASE_ADDR   (0x40013800)
  #define OS_BRR_VALUE      (OS_PCLK_UART / OS_BAUDRATE)
#elif (OS_UART == 1)
  #define OS_COM_IsrHandler USART2_IRQHandler
  #define USART_BASE_ADDR   (0x40004400)
  #if (OS_PCLK_UART > 36000000uL)
    //
    // BUS is limited to 36MHz, assume, clock frequency is divided by 2
    //
    #define OS_BRR_VALUE     (OS_PCLK_UART / OS_BAUDRATE / 2)
  #else
    #define OS_BRR_VALUE     (OS_PCLK_UART / OS_BAUDRATE)
  #endif
#endif
#define USART_SR             (*(volatile OS_U32*)(USART_BASE_ADDR + 0x00))
#define USART_DR             (*(volatile OS_U32*)(USART_BASE_ADDR + 0x04))
#define USART_BRR            (*(volatile OS_U32*)(USART_BASE_ADDR + 0x08))
#define USART_CR1            (*(volatile OS_U32*)(USART_BASE_ADDR + 0x0C))
#define USART_CR2            (*(volatile OS_U32*)(USART_BASE_ADDR + 0x10))
#define USART_CR3            (*(volatile OS_U32*)(USART_BASE_ADDR + 0x14))
#define USART_GTPR           (*(volatile OS_U32*)(USART_BASE_ADDR + 0x18))
#define RCC_USART1EN         ((OS_U32)0x00004004)
#ifdef UART2_NOT_REMAPPED
  #define RCC_USART2EN       ((OS_U32)0x00000014)
#else  // UART 2 is on Port D
  #define RCC_USART2EN       ((OS_U32)0x00000024)
#endif

#define GPIOA_CRL            (*(volatile OS_U32*)(0x40010800))
#define GPIOA_CRH            (*(volatile OS_U32*)(0x40010804))
#define GPIOD_CRL            (*(volatile OS_U32*)(0x40011400))


#define US_RXRDY             (0x20)   // RXNE
#define USART_RX_ERROR_FLAGS (0x0F)   // ORE/NE/FE/PE
#define US_TXEMPTY           (0x80)   // TXE

/*********************************************************************
*
*       OS_COM_IsrHandler
*
* Function description
*   The communication interrupt handler for UART communication
*   to embOSView.
*
* NOTES:
*   (1) It has to be inserted in the interrupt vector table, if RAM
*       vectors are not used. Therefore is is declared public
*/
void OS_COM_IsrHandler(void) {
  volatile OS_U32 Dummy;
  int UsartStatus;
  OS_EnterNestableInterrupt();
  UsartStatus = USART_SR;                        // Examine status register
  do {
    if (UsartStatus & US_RXRDY) {                // Data received?
      if (UsartStatus & USART_RX_ERROR_FLAGS) {  // Any error ?
        Dummy = USART_DR;                        // => Discard data
      } else {
        OS_OnRx(USART_DR);                       // Process actual byte
      }
    }
    UsartStatus = USART_SR;                      // Examine current status
  } while (UsartStatus & US_RXRDY);
  if (UsartStatus & US_TXEMPTY) {
    if (OS_OnTx()) {                             // No more characters to send ?
      USART_CR1 &= ~0x40uL;                      // Disable further tx interrupts
    }
  }
  OS_LeaveNestableInterrupt();
}

/*********************************************************************
*
*       OS_COM_Send1()
*
*       Sends one character via UART. Never call this from your application
*/
void OS_COM_Send1(OS_U8 c) {
  USART_DR   = (c & (OS_U16)0x01FF);
  USART_CR1 |= 0x40;                           // Enable tx interrupt
}

/*********************************************************************
*
*       OS_COM_Init()
*
*       Initialize the selected UART
*/
#if (OS_UART == 0)
void OS_COM_INIT(void) {
  OS_U8 Priority;

  OS_IncDI();
  RCC_APB2ENR |= RCC_USART1EN;  // Enable GPIO port used for USART and USART clock
  GPIOA_CRH    = (GPIOA_CRH & 0xFFFFF00F) | 0x000004B0;
  USART_CR2    = 0x200;
  USART_CR1    = 0x2C;
  USART_CR3    = 0x00;
  USART_BRR    = OS_BRR_VALUE;
  USART_CR1   |= 0x2000;         // Enable uart
  //
  // Install USART Handler with preemtion level one above lowest level to ensure communication during PendSV
  //
  OS_ARM_InstallISRHandler(ISR_ID_USART1, (OS_ISR_HANDLER*)OS_COM_IsrHandler);
  OS_ARM_ISRSetPrio(ISR_ID_USART1, 0xFF);               // Set lowest Priority, ALL BITS set
  Priority  = OS_ARM_ISRSetPrio(ISR_ID_USART1, 0xFF);   // Read back priority to examine preemption level bits
  Priority -= 1;                                        // Set priority one level above lowest priority
  OS_ARM_ISRSetPrio(ISR_ID_USART1, Priority);
  OS_ARM_EnableISR(ISR_ID_USART1);
  OS_DecRI();
}

#elif (OS_UART == 1)
void OS_COM_INIT(void) {
  OS_U8 Priority;

  OS_IncDI();
  RCC_APB1ENR |= (1uL << 17);               // Enable USART2 clock
  RCC_APB2ENR |= RCC_USART2EN  | (1uL << 0);// Enable GPIO port needed for USART, enable alternate function
#ifndef UART2_NOT_REMAPPED
  AFIO_MAPR   |= (1uL << 3);                // Remap USART to Port D
  GPIOD_CRL    = (GPIOD_CRL & 0xF00FFFFF) | 0x04B00000;
#else
  GPIOA_CRL    = (GPIOA_CRL & 0xFFFF00FF) | 0x00004B00;
#endif
  USART_CR2    = 0x200;
  USART_CR1    = 0x2C;
  USART_CR3    = 0x00;
  USART_BRR    = OS_BRR_VALUE;
  USART_CR1   |= 0x2000;         // Enable uart
  //
  // Install USART2 Handler with preemtion level one above lowest level to ensure communication
  //
  OS_ARM_InstallISRHandler(ISR_ID_USART2, (OS_ISR_HANDLER*)OS_COM_IsrHandler);
  OS_ARM_ISRSetPrio(ISR_ID_USART2, 0xFF);               // Set lowest Priority, ALL BITS set
  Priority  = OS_ARM_ISRSetPrio(ISR_ID_USART2, 0xFF);   // Read back priority to examine preemption level bits
  Priority -= 1;                                        // Set priority one level above lowest priority
  OS_ARM_ISRSetPrio(ISR_ID_USART2, Priority);
  OS_ARM_EnableISR(ISR_ID_USART2);
  OS_DecRI();
}

#else
void OS_COM_INIT(void) {

return;

}

#endif

#elif OS_USE_JLINKMEM

/*********************************************************************
*
*       _JLINKMEM_OnRx()
*/
static void _JLINKMEM_OnRx(OS_U8 Data) {
  OS_OnRx(Data);
}

/*********************************************************************
*
*       _JLINKMEM_OnTx()
*/
static void _JLINKMEM_OnTx(void) {
  OS_OnTx();
}

/*********************************************************************
*
*       _JLINKMEM_GetNextChar()
*/
static OS_INT _JLINKMEM_GetNextChar(void) {
  return OS_COM_GetNextChar();
}

/*********************************************************************
*
*       OS_COM_Init()
*       Initialize memory access for OSView
*/
void OS_COM_Init(void) {
  JLINKMEM_SetpfOnRx(_JLINKMEM_OnRx);
  JLINKMEM_SetpfOnTx(_JLINKMEM_OnTx);
  JLINKMEM_SetpfGetNextChar(_JLINKMEM_GetNextChar);
}

/*********************************************************************
*
*       OS_COM_Send1()
*       Send 1 character via memory
*/
void OS_COM_Send1(OS_U8 c) {
  JLINKMEM_SendChar(c);
}

#else // No UART communication for OS enabled, define dummy functions
/*********************************************************************
*
*       Communication for embOSView not selected
*
**********************************************************************
*/
void OS_COM_Init(void) {}
void OS_COM_Send1(OS_U8 c) {
  OS_USEPARA(c);
  OS_COM_ClearTxActive();  /* Free OS transmitter buffer */
}
#endif /*  OS_UART_USED  */


#endif /* OSIF_EMBOS */
