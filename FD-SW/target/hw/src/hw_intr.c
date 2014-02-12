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


/* ===========================================================================

FILE_NAME          hw_intr.c



FUNCTIONAL_MODULE_DESCRIPTION

    This modul contains the interrupt routines for the hardware component.

=========================================================================== */
#include "keywords.h"   /* keyword defines */
#define  MODULE_ID      (COMP_HW + MOD_HWINTR)

INCLUDES

#include "base.h"
#include "hw_if.h"

#include "except.h"

#include "cpu.h"
#include "hw_cfg.h"
#include "dbg_out.h"

#include "stm32f10x_exti.h"


#pragma language=extended
#pragma segment="CSTACK"

GLOBAL_DEFINES

LOCAL_DEFINES

  /* Define WEAK attribute */
#if defined   ( __CC_ARM   )
    #define WEAK __attribute__ ((weak))
#elif defined ( __ICCARM__ )
    #define WEAK __weak
#elif defined (  __GNUC__  )
    #define WEAK __attribute__ ((weak))
#endif

EXPORT_TYPEDEFS

LOCAL_TYPEDEFS

//typedef PUBLIC  void (INTERRUPT IRQ_HANDLER) (void);

//typedef void( *intfunc )(void);
//typedef union { intfunc __fun; void * __ptr; } intvec_elem;

FUNCTION_DECLARATIONS

FUNCTION GLOBAL VOID fbsnv_prepare_extra_nv_block(USIGN8**,USIGN32*,USIGN16*);
FUNCTION GLOBAL VOID eep_put_extra_nv_data(USIGN32,USIGN8*,USIGN16);

//#if (defined (CPU_TYPE_M16C) && defined (COMPILER_IAR)) /* --------------- */

//extern void HardFault_Handler (void);
WEAK void HardFault_Handler (void);
//extern void MemManage_Handler (void);
WEAK void MemManage_Handler (void);
//extern void BusFault_Handler (void);
WEAK void BusFault_Handler (void);
//extern void UsageFault_Handler (void);
WEAK void UsageFault_Handler (void);
//extern void SVC_Handler (void);
WEAK void SVC_Handler (void);
//extern void DebugMon_Handler (void);
WEAK void DebugMon_Handler (void);
void __NMI_handler (void);
//extern void PendSV_Handler (void);
WEAK void PendSV_Handler (void);
void SysTick_Handler (void);

void __default_interrupt_handler (void);


FUNCTION void fbc_interrupt_handler (void);

extern void __iar_program_start (void);
extern void TIM10_handler (void);
extern void HART_timer_irq (void);
extern void I2C_IRQ_handler (void);
extern void HART_IRQ_handler (void);
extern void HART_DMAR_handler (void);
extern void HART_DMAT_handler (void);

extern void int_handler_fbc (void);     /* imported from protcol software libs  */
FUNCTION void TIM6_int_handler (void);

EXPORT_DATA

//In Assembler
extern void Cortex_Trap(void);
extern const u16 mychecksum;
extern const void *VerString;
extern const void *domainHdr;

#pragma location = ".intvec"
const intvec_elem __int_vector_table[] =
{
  { .__ptr = __sfe( "CSTACK" ) },
  /* Reset Handler  */                          __iar_program_start,
  /* NMI Handler    */                          Cortex_Trap,//__NMI_handler,
  /* Hard Fault Handler */                      Cortex_Trap,//HardFault_Handler,
  /* Mem Manage Handler */                      Cortex_Trap,//MemManage_Handler,
  /* Bus Fault Handler */                       Cortex_Trap,//BusFault_Handler,
  /* Usage Fault Handler */                     Cortex_Trap,//UsageFault_Handler,
  /* Reserved*/                                 Cortex_Trap,//0,
  /* Reserved*/                                 Cortex_Trap,//0,
  /* Reserved*/                                 Cortex_Trap,//0,
  /* Reserved*/                                 Cortex_Trap,//0,
  /* SVCall Handler */                          Cortex_Trap,//SVC_Handler,
  /* Debug Monitor Handler */                   Cortex_Trap,//DebugMon_Handler,
  /* Reserved*/                                 Cortex_Trap,//0,
  /* PendSV Handler */                          PendSV_Handler,
  /* SysTick Handler*/                          SysTick_Handler,

  /************************ External Interrupts ********************************/
  /* Window Watchdog */                         __default_interrupt_handler,
  /* PVD through EXTI Line detect */            __default_interrupt_handler,
  /* Tamper */                                  __default_interrupt_handler,
  /* RTC */                                     __default_interrupt_handler,
  /* Flash */                                   __default_interrupt_handler,
  /* RCC */                                     __default_interrupt_handler,
  /* EXTI Line 0 */                             fbc_interrupt_handler,
  /* EXTI Line 1 */                             __default_interrupt_handler,
  /* EXTI Line 2 */                             __default_interrupt_handler,
  /* EXTI Line 3 */                             __default_interrupt_handler,
  /* EXTI Line 4 */                             __default_interrupt_handler,
  /* DMA1 Channel 1 */                          __default_interrupt_handler,
  /* DMA1 Channel 2 */                          __default_interrupt_handler,
  /* DMA1 Channel 3 */                          __default_interrupt_handler,
  /* DMA1 Channel 4 */                          __default_interrupt_handler,
  /* DMA1 Channel 5 */                          __default_interrupt_handler,
  /* DMA1 Channel 6 */                          HART_DMAR_handler,//__default_interrupt_handler,  //
  /* DMA1 Channel 7 */                          HART_DMAT_handler,//__default_interrupt_handler,  //
  /* ADC1 & ADC2 */                             __default_interrupt_handler,
  /* USB High Priority or CAN1 TX */            __default_interrupt_handler,
  /* USB Low  Priority or CAN1 RX0 */           __default_interrupt_handler,
  /* CAN1 RX1 */                                __default_interrupt_handler,
  /* CAN1 SCE */                                __default_interrupt_handler,
  /* EXTI Line 9..5 */                          __default_interrupt_handler,
  /* TIM1 Break and TIM9 */                     __default_interrupt_handler,
  /* TIM1 Update and TIM10 */                   TIM10_handler,
  /* TIM1 Trigger and Commutation and TIM11 */  HART_timer_irq,
  /* TIM1 Capture Compare */                    __default_interrupt_handler,
  /* TIM2 */                                    __default_interrupt_handler,
  /* TIM3 */                                    __default_interrupt_handler,
  /* TIM4 */                                    __default_interrupt_handler,
  /* I2C1 Event */                              __default_interrupt_handler,
  /* I2C1 Error */                              __default_interrupt_handler,
  /* I2C2 Event */                              __default_interrupt_handler,
  /* I2C2 Error */                              __default_interrupt_handler,
  /* SPI1 */                                    __default_interrupt_handler,
  /* SPI2 */                                    __default_interrupt_handler,
  /* USART1 */                                  __default_interrupt_handler,
  /* USART2 */                                  __default_interrupt_handler, //HART_IRQ_handler,
  /* USART3 */                                  __default_interrupt_handler,
  /* EXTI Line 15..10 */                        __default_interrupt_handler,
  /* RTC Alarm through EXTI Line */             __default_interrupt_handler,
  /* USB Wakeup from suspend */                 __default_interrupt_handler,
  /* TIM8 Break and TIM12 */                    __default_interrupt_handler,
  /* TIM8 Update and TIM13 */                   __default_interrupt_handler,
  /* TIM8 Trigger and Commutation and TIM14 */  __default_interrupt_handler,
  /* TIM8 Capture Compare */                    __default_interrupt_handler,
  /* ADC3 */                                    __default_interrupt_handler,
  /* FSMC */                                    __default_interrupt_handler,
  /* SDIO */                                    __default_interrupt_handler,
  /* TIM5 */                                    __default_interrupt_handler,
  /* SPI3 */                                    __default_interrupt_handler,
  /* UART4 */                                   __default_interrupt_handler,
  /* UART5 */                                   __default_interrupt_handler,
  /* TIM6 */                                    TIM6_int_handler,
  /* TIM7 */                                    __default_interrupt_handler,
  /* DMA2 Channel1 */                           __default_interrupt_handler,
  /* DMA2 Channel2 */                           __default_interrupt_handler,
  /* DMA2 Channel3 */                           __default_interrupt_handler,
  /* DMA2 Channel4 & Channel5 */                __default_interrupt_handler
    ,{ .__ptr = (void*)&mychecksum}
    ,{ .__ptr = (void*)&domainHdr}
};


IMPORT_DATA

  extern BOOL wait;

LOCAL_DATA

USIGN32 count_fbc_irq = 0;



#pragma location="CODE"

/****************************************************************************/


void init_interrupts (void)
/*----------------------------------------------------------------------------

FUNCTIONAL_DESCRIPTION

    This function does nothing, but its call from main makes possible that
    this module will be linked.

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES


FUNCTION_BODY



  return;
}

#ifdef USE_POWERFAIL_IRQ
void extra_nv_data_block_handler (void)
/*----------------------------------------------------------------------------

FUNCTIONAL_DESCRIPTION

  This function is called by the NMI handler when a power
  fail interrupt occurs.

  It may be necessary to write your own low level write to NV data
  function when the default function eep_put_extra_nv_data() is not
  fast enough.

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

  USIGN8  * p_ram_addr;
  USIGN16   eep_len;
  USIGN32   eep_addr;

FUNCTION_BODY

  /* Prepare the extra NV blocks for writing */
  fbsnv_prepare_extra_nv_block(&p_ram_addr, &eep_addr, &eep_len);

  /* Copy the extra nv data into the NV memory.                  */
  /* The next function is the default function to copy the extra */
  /* NV data into the NV memory.                                 */
  eep_put_extra_nv_data(eep_addr, p_ram_addr, eep_len);

  return;
}
#endif /* USE_POWERFAIL_IRQ */

/****************************************************************************/
u32 default_interrupt_count; //for debug and testing. It better stay at C-init 0

FUNCTION void __default_interrupt_handler (void)
/*----------------------------------------------------------------------------

FUNCTIONAL_DESCRIPTION

    Interrupt handler for undefined interrupts.

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

    ENTER_IRQ();
    default_interrupt_count++;
    _HW_EXCEPTION(ERR_INV_INT, 0);
    LEAVE_IRQ();
}


/****************************************************************************/


FUNCTION void __NMI_handler (void)
/*----------------------------------------------------------------------------

FUNCTIONAL_DESCRIPTION

    Interrupt handler for NMI. This interrupt is executed if a
    power fail occurs.

    This function makes the NMI behave like a RESET.

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

#ifdef USE_POWERFAIL_IRQ
  #if !defined (FBK_HW) && !defined(BFD_HW)

    /* If it is needed to save critical data into the non volatile */
    /* area, enable this section and modify the function           */
    /* extra_nv_data_block_handler().                              */

    extra_nv_data_block_handler();

  #endif /* !FBK_HW && !BFD_HW */
#endif /* USE_POWERFAIL_IRQ */

#if defined (PD30) /* || defined (KD30) */
    //break_instruction ();
#endif


#ifdef FC_SPC42
    FBC_ASSERT_RESET;          /* Assert SPC4-2 reset pin  */
#endif /* FC_SPC42 */

#if defined (FIND1_PLUS)  || defined (FC_FIND1P_DMA) || defined (FC_FRONTIER1_PLUS)
    FBC_RESET = 0;          /* Assert F1 reset pin      */
#endif /* FIND1_PLUS, FC_FIND1P_DMA, FC_FRONTIER1_PLUS */

    /* now wait for actual power fail while NMI is L */
    //while (!_BIT(P8,5)) /* P8.5 is the NMI input pin */
        ;

    /* now NMI has gone away, but there was no power fail? */
    Reset_CPU ();
}


/****************************************************************************/




FUNCTION void fbc_interrupt_handler (void)
/*----------------------------------------------------------------------------

FUNCTIONAL_DESCRIPTION

    Interrupt handler for Fieldbus Controller interrupt.

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

    EXTI_ClearITPendingBit(EXTI_Line0);
    ENTER_FBC_IRQ();
    int_handler_fbc ();
    LEAVE_FBC_IRQ();
}



FUNCTION void TIM6_int_handler (void)
/*----------------------------------------------------------------------------

FUNCTIONAL_DESCRIPTION

 Timer TIM6 is used for Single Wire Interface to access the Dongle Security EEPROM.
 Therefore this Dummy ISR is defined to avoid accidental re-use of TIM6.
----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  return;
}

//#endif /* CPU_TYPE_M16C, COMPILER_IAR ------------------------------------ */


