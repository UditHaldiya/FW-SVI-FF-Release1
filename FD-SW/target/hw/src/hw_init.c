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

FILE_NAME          hw_init.c



FUNCTIONAL_MODULE_DESCRIPTION

This modul contains the code for processor initialization.

=========================================================================== */
#include "keywords.h"   /* keyword defines */
#define  MODULE_ID      (COMP_HW + MOD_HWINIT)

INCLUDES

#include "base.h"
#include "except.h"
#include "hw_if.h"
#include "hw_cfg.h"

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_fsmc.h"
#include "stm32f10x.h"

#include "mntrapdef.h"
#include "mnassert.h"

#if !defined (__IAR_SYSTEMS_ICC) && !defined (STM32F10X_XL)
#error Invalid compiler and / or CPU type !!
#endif

GLOBAL_DEFINES

LOCAL_DEFINES
#if (SYS_DIV == 1)                  /* CPU clock = System clock         */

#elif (SYS_DIV == 2)                /* CPU clock = System clock / 2     */

#elif (SYS_DIV == 4)                /* CPU clock = System clock / 4     */

#elif (SYS_DIV == 8)                /* CPU clock = System clock / 8     */

#elif (SYS_DIV == 16)               /* CPU clock = System clock / 16    */

#else
    #error Invalid Clock Divisor !
#endif /* SYS_DIV */


#if (ISR_SYS_DIV == 1)              /* CPU clock = System clock         */

#elif (ISR_SYS_DIV == 2)            /* CPU clock = System clock / 2     */

#elif (ISR_SYS_DIV == 4)            /* CPU clock = System clock / 4     */

#elif (ISR_SYS_DIV == 8)            /* CPU clock = System clock / 8     */

#elif (ISR_SYS_DIV == 16)           /* CPU clock = System clock / 16    */

#else
    #error Invalid Clock Divisor !
#endif /* ISR_SYS_DIV */

#define RCC_APB1Periph_ALL               ((uint32_t)0x3AFEC83F)
#define RCC_APB2Periph_ALL               ((uint32_t)0x0000FFFD)


EXPORT_TYPEDEFS

LOCAL_TYPEDEFS

FUNCTION_DECLARATIONS

  extern void hw_cfg_GPIO();
  extern void hw_cfg_TIMx();
  extern void hw_cfg_EXTI(FunctionalState);
  extern void hw_cfg_NVIC();
  extern void hw_cfg_FSMC();


EXPORT_DATA

#ifdef COMPILER_IAR /* ---------------------------------------------------- */
#if __VER__ < 300   /* -- Required only for ICCM16C 1.x and 2.x ----------- */
/*
 * The following variable definitions are used by the IAR library functions
 * malloc and free. The definitions are derived from the source etc\heap.c.
 */
#include "da_heap.h"                             /* DATA SEGMENT DEFINITION */
NO_INIT static char bulk_storage [MALLOC_BUFSIZE];
#include "da_def.h"                              /* DEFAULT DATA SEGMENT */

#include "cs_hw.h"                          /* CONSTANTS SEGMENT DEFINITION */
char *    const _heap_of_memory = bulk_storage;
char *    const _top_of_heap = bulk_storage + MALLOC_BUFSIZE - 1;
#include "cs_def.h"                         /* DEFAULT CONSTANT SEGMENT */

char *          _last_heap_object = bulk_storage;

#else

#pragma location="HEAP"
extern char * _last_heap_object;                            /* Current heap pointer */


#endif /* __VER__ */
#endif /* COMPILER_IAR ---------------------------------------------------- */


IMPORT_DATA

LOCAL_DATA


  RCC_ClocksTypeDef RCC_ClockFreqs;



#include "co_hwini.h"                            /* CODE SEGMENT DEFINITION */

FUNCTION VOID Reset_Watchdog (VOID);

//--------------------------------------------------------------------------------------

__interwork int __low_level_init(void)
{
#ifdef FC_SPC42
    volatile int i = 0;
#endif /* FC_SPC42 */

    volatile USIGN32 HSERDY_status = (USIGN32)0;
    volatile USIGN16 wait_count = (USIGN16)0;

    // Disable Interrupts.
    __set_PRIMASK(1);

    /************************ configure Clock source ************************/

    /*
    ---------------------------------------------------------------------------
    ------- Reset and Clock Control (RCC) Clock control register (CR) ---------
    ---------------------------------------------------------------------------
    Bit#31 xxxx xxxx xxxx xxxx xxxx xxxx xxxx xxxx Bit#0
           |||| |||| |||| |||| |||| |||| |||| ||||
           |||| |||| |||| |||| |||| |||| |||| |||+-- (rw) HSION: Internal high-speed clock enable (0: HSI Off; 1: HSI On)
           |||| |||| |||| |||| |||| |||| |||| ||+--- (r)  HSIRDY: Internal high-speed clock ready flag (0: HSI not ready; 1: HSI ready)
           |||| |||| |||| |||| |||| |||| |||| |+---- reserved
           |||| |||| |||| |||| |||| |||| ++++-+----- (rw) HSITRIM[4:0]: Internal high-speed clock trimming
           |||| |||| |||| |||| ++++-++++------------ (r)  HSICAL[7:0]: Internal high-speed clock calibration (automatically initialized at startup)
           |||| |||| |||| |||+---------------------- (rw) HSEON: HSE clock enable (0: HSE Off, 1: HSE On)
           |||| |||| |||| ||+----------------------- (r)  HSERDY: External high-speed clock ready flag (0: HSE not ready; 1: HSE ready)
           |||| |||| |||| |+------------------------ (rw) HSEBYP: External high-speed clock bypass (0: HSE crystal not bypassed; 1: HSE crystal bypassed with External Clock)
           |||| |||| |||| +------------------------- (rw) CSSON: Clock security system enable (0: Clock detector Off; 1: Clock detector On)
           |||| |||| ++++--------------------------- reserved
           |||| |||+-------------------------------- (rw) PLLON: PLL enable (0: PLL Off; 1: PLL On)
           |||| ||+--------------------------------- (r)  PLLRDY: PLL clock ready flag (0: PLL Unlocked; 1: PLL locked)
           ++++ ++---------------------------------- reserved
    ---------------------------------------------------------------------------
    */

    /* Set HSION bit --> High Speed Internal 8MHz Clock enabled */
    RCC->CR |= (USIGN32)0x00000001;

    /* Disable HSE Clock and Reset HSE Clock bypass --> use 4Mhz external crystal */
    RCC->CR &= (USIGN32)0xFFFAFFFF; /* Bit 16 (HSEON) = 0 (Disable); Bit 18 (HSEBYP) = 0 (Not bypassed) */


    /*
    ---------------------------------------------------------------------------
    ---- Reset and Clock Control (RCC) Clock configuration register (CFGR) ----
    ---------------------------------------------------------------------------
    Bit#31 xxxx xxxx xxxx xxxx xxxx xxxx xxxx xxxx Bit#0
           |||| |||| |||| |||| |||| |||| |||| ||||
           |||| |||| |||| |||| |||| |||| |||| ||++-- (rw) SW: System clock switch (00: HSI is SYSCLK; 01: HSE is SYSCLK; 10: PLL is SYSCLK; 11: reserved)
           |||| |||| |||| |||| |||| |||| |||| ++---- (r)  SWS: System clock switch status (00: HSI used as SYSCLK; 01: HSE used as SYSCLK; 10: PLL used as SYSCLK; 11: reserved)
           |||| |||| |||| |||| |||| |||| ++++------- (rw) HPRE: AHB prescaler - PSC for HCLK - (0xxx: SYSCLK not divided; 1000: SYSCLK divided by 2 ...multiple of 2... 1111: SYSCLK divided by 512)
           |||| |||| |||| |||| |||| |+++------------ (rw) PPRE1: APB low-speed prescaler (APB1)  - PSC for PCLK1 - (0xx: HCLK not divided; 100: HCLK divided by 2 ...multiple of 2... 111: HCLK divided by 16)
           |||| |||| |||| |||| ||++-+--------------- (rw) PPRE2: APB high-speed prescaler (APB2) - PSC for PCLK2 - (0xx: HCLK not divided; 100: HCLK divided by 2 ...multiple of 2... 111: HCLK divided by 16)
           |||| |||| |||| |||| ++------------------- (rw) ADCPRE: ADC prescaler (00: PCLK2/2; 01: PCLK2/4; 10: PCLK2/6; 11: PCLK2/8)
           |||| |||| |||| |||+---------------------- (rw) PLLSRC: PLL entry clock source (0: HSI/2 = PLL Input CLK; 1: HSE = PLL Input CLK)
           |||| |||| |||| ||+----------------------- (rw) PLLXTPRE: HSE divider for PLL entry (0: HSE Clock not divided; 1: HSE clock divided by 2)
           |||| |||| ||++-++------------------------ (rw) PLLMUL: PLL multiplication factor (0000: PLL Input CLK *2 ...increases by 1... 1110/1111: PLL Input CLK *16)
           |||| |||| |+----------------------------- (rw) USBPRE: USB prescaler (0: PLL CLK divided by 1.5; 1: PLL CLK not divided)
           |||| |||| +------------------------------ reserved
           |||| |+++-------------------------------- (rw) MCO: Microcontroller clock output (0xx: No Clock; 100: SYSCLK selected; 101: HSI CLK selected; 110: HSE CLK selected; 111: PLL clock divided by 2 selected)
           ++++ +----------------------------------- reserved
    --------------------------------------------------------------------------
    */

    /* configure low-power system startup. */
    RCC->CFGR = (USIGN32)0x000000B0;  /* AHB clock PSC division factor set to 16 */


    /*
    ---------------------------------------------------------------------------
    ------ Reset and Clock Control (RCC) Clock interrupt register (CIR) -------
    ---------------------------------------------------------------------------
    Bit#31 xxxx xxxx xxxx xxxx xxxx xxxx xxxx xxxx Bit#0
           |||| |||| |||| |||| |||| |||| |||| ||||
           |||| |||| |||| |||| |||| |||| |||| |||+-- (r)  LSIRDYF: LSI ready interrupt flag (0: No "Clock Ready" interrupt caused by int RC 40kHz oscillator; 1: "Clock Ready" interrupt caused)
           |||| |||| |||| |||| |||| |||| |||| ||+--- (r)  LSERDYF: LSE ready interrupt flag (0: No "Clock Ready" interrupt caused by ext 32kHz oscillator; 1: "Clock Ready" interrupt caused)
           |||| |||| |||| |||| |||| |||| |||| |+---- (r)  HSIRDYF: HSI ready interrupt flag (0: No "Clock Ready" interrupt caused by int 8MHz RC oscillator; 1: "Clock Ready" interrupt caused)
           |||| |||| |||| |||| |||| |||| |||| +----- (r)  HSERDYF: HSE ready interrupt flag (0: No "Clock Ready" interrupt caused by ext 4-16MHz oscillator; 1: "Clock Ready" interrupt caused)
           |||| |||| |||| |||| |||| |||| |||+------- (r)  PLLRDYF: PLL ready interrupt flag (0: No "Clock Ready" interrupt caused by PLL lock; 1: "Clock Ready" interrupt caused)
           |||| |||| |||| |||| |||| |||| |++-------- reserved
           |||| |||| |||| |||| |||| |||| +---------- (r)  CSSF: Clock security system interrupt flag (0: No "Clock Security" interrupt caused by HSE CLK failure; 1: "Clock Security" interrupt caused)
           |||| |||| |||| |||| |||| |||+------------ (rw) LSIRDYIE: LSI ready interrupt enable (0: disabled; 1: enabled)
           |||| |||| |||| |||| |||| ||+------------- (rw) LSERDYIE: LSE ready interrupt enable (0: disabled; 1: enabled)
           |||| |||| |||| |||| |||| |+-------------- (rw) HSIRDYIE: HSI ready interrupt enable (0: disabled; 1: enabled)
           |||| |||| |||| |||| |||| +--------------- (rw) HSERDYIE: HSE ready interrupt enable (0: disabled; 1: enabled)
           |||| |||| |||| |||| |||+----------------- (rw) PLLRDYIE: PLL ready interrupt enable (0: disabled; 1: enabled)
           |||| |||| |||| |||| +++------------------ reserved
           |||| |||| |||| |||+---------------------- (w)  LSIRDYC: LSI ready interrupt clear (0: No effect; 1: Clear LSIRDYF)
           |||| |||| |||| ||+----------------------- (w)  LSERDYC: LSE ready interrupt clear (0: No effect; 1: Clear LSERDYF)
           |||| |||| |||| |+------------------------ (w)  HSIRDYC: HSI ready interrupt clear (0: No effect; 1: Clear HSIRDYF)
           |||| |||| |||| +------------------------- (w)  HSERDYC: HSE ready interrupt clear (0: No effect; 1: Clear HSERDYF)
           |||| |||| |||+--------------------------- (w)  PLLRDYC: PLL ready interrupt clear (0: No effect; 1: Clear PLLRDYF)
           |||| |||| |++---------------------------- reserved
           |||| |||| +------------------------------ (w)  CSSC: Clock security system interrupt clear (0: No effect; 1: Clear CSSF)
           ++++ ++++-------------------------------- reserved
    --------------------------------------------------------------------------
    */

    /* Disable all interrupts and clear pending bits */
    RCC->CIR = 0x009F0000;


    /*----------------------- Configure HSE (High Speed External) 4MHz Clock -----------------------*/
    /* Enable HSE clock */
    RCC->CR |= (USIGN32)0x00010000;                   /* Bit 16 (HSEON) = 1 (Enable) */

    /* Wait until HSE oscillator is stable --> timeout value 0x0500 choosen regarding IAR example for stm32f103 */
    do
    {
      HSERDY_status = RCC->CR & (USIGN32)0x00020000;  /* check HSERDY bit in CR Register */
      wait_count++;
    } while ( (HSERDY_status == (USIGN32)0x00000000) && (wait_count < 0x0500) ); /* wait until HSERDY becomes 1 or wait_count becomes > 0x0500 */

    if (HSERDY_status == (USIGN32)0x00020000)         /* HSE oscillator stable */
    {
      /* set Wait States for Flash Access in dependency of CPU frequency */

      // Original ACR config. Enable Half-Cycle Flash controller feature.
      FLASH->ACR |= (USIGN32)0x00000008;              /* No Wait States */

      /* set HCLK and Peripheral clock dividers to / 16 temporarily (to reduce initial current */
      /* consumption), later the clocks are configured for increased frequencies */

#if 0

      RCC->CFGR |= RCC_CFGR_HPRE_DIV16;               /* HCLK = SYSCLK / 16 */
      RCC->CFGR |= RCC_CFGR_PPRE2_DIV16;              /* PCLK2 =  HCLK / 16 */
      RCC->CFGR |= RCC_CFGR_PPRE1_DIV16;              /* PCLK1 =  HCLK / 16 */

#else

      RCC->CFGR |= RCC_CFGR_HPRE_DIV1;                /* HCLK = SYSCLK / 1 */
      RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;               /* PCLK2 =  HCLK / 1 */
      RCC->CFGR |= RCC_CFGR_PPRE1_DIV1;               /* PCLK1 =  HCLK / 1 */

#endif

      /* Select HSE as system clock source in SW bit#0-#1*/
      RCC->CFGR |= (USIGN32)0x00000001;

      /* Wait till HSE is used as system clock source */
      while ((RCC->CFGR & (USIGN32)RCC_CFGR_SWS) != (USIGN32)0x00000004);

      /* Turn off High Speed Internal 8MHz oscillator to reduce power consumption */
      RCC->CR &= (USIGN32)0xFFFFFFFE;
    }
    else /* HSE oscillator not stable */
    {
      /* intended source of System Clock not available, no way to continue */
      Reset_CPU();
    }

    /* Disable any GPIOs Peripheral clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ALL, DISABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_ALL, DISABLE);

    /* Enable FSMC | FLITF clock | SRAM clock | CRC clock */
    RCC_AHBPeriphClockCmd((RCC_AHBPeriph_FSMC|RCC_AHBPeriph_DMA1/*|RCC_AHBPeriph_FLITF*/|RCC_AHBPeriph_SRAM|RCC_AHBPeriph_CRC), ENABLE);

    /* Enable APB2 peripheral clocks */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO  | RCC_APB2Periph_GPIOA |
                           RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |
                           RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE |
                           RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOG |
                           RCC_APB2Periph_TIM10 | RCC_APB2Periph_TIM11,
                           ENABLE);

    /* Enable APB1 peripheral clocks */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 | RCC_APB1Periph_I2C2 |
                           RCC_APB1Periph_TIM6,
                           ENABLE);

    /* configure HCLK for AHB bus */
    RCC_HCLKConfig(RCC_SYSCLK_Div1);      /* HCLK = SYSCLK/1 = 4MHz */

    /* configure high speed (APB2) peripheral clock PCLK2 */
    RCC_PCLK2Config(RCC_HCLK_Div1);       /* PCLK2 = HCKL/1 = 4MHz */

    /* configure low speed (APB1) peripheral clock PCLK1 */
    RCC_PCLK1Config(RCC_HCLK_Div1);       /* PCLK1 = HCKL/1 = 4MHz */

#ifdef FC_SPC42                           /* SPC4 needs delay here            */
    for (i=0; i<100; i++);                /* Reason __UNKNOWN__!!             */
#endif /* FC_SPC42 */

//--------------------------------------------------------------------------------------
// Initialize the GPIOs and External Memory Controller early, otherwise
// the compiler standard init code tries to access memory that is NOT enabled.
// This code moved here from the init_processor

    hw_cfg_GPIO();                      /* Configure General-Purpose I/O pins */

    hw_cfg_FSMC();                      /* Configure Flexible Static Memory Controller */

    Reset_Watchdog();                   /* Reset external Watchdog */

//--------------------------------------------------------------------------------------

    return 1;   /* activate initialisation of RAM */
}



#if defined (CUSTOMER_HW)
void init_processor (void)
{
    RCC_GetClocksFreq(&RCC_ClockFreqs); /* populate RCC_ClockFreqs with final values */

    // hw_cfg_GPIO();                      /* Configure General-Purpose I/O pins */

    hw_cfg_TIMx();                      /* Configure HW Timer */

    hw_cfg_EXTI(DISABLE);               /* Configure and Disable the External Interrupt Controller */

    hw_cfg_NVIC();                      /* Configure the Nested Vectored Interrupt Controller */

    // hw_cfg_FSMC();                      /* Configure Flexible Static Memory Controller */

    FBC_ASSERT_RESET;                   /* reset the FBC chip, before the interrupt pin is enabled */
                                        /* Assert SPC4-2 reset pin, by driving high level "1"  */

    hw_cfg_EXTI(ENABLE);                /* Configure and Enable the External Interrupt Controller */

    Reset_Watchdog();                   /* Reset external Watchdog */

}


#else

  #error Hardware platform not defined properly

#endif /* CUSTOMER_HW */


void rcc_enable_HSI(void)
{
    RCC->CR |= (USIGN32)0x00000001;
}

void rcc_disable_HSI(void)
{
    RCC->CR &= ~(USIGN32)0x00000001;
}

/** \brief A replacement routing app-level reset to assert, recording the
address of the requestor
*/
void Reset_CPU(void)
{
    mn_assert((void *)__get_LR(), -TRAPID_SWI); //steal TRAPID_SWI to indicate a reset
}


FUNCTION VOID HardReset_CPU (VOID)
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

Executes a software reset
------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY


  /*
  -------------------------------------------------------------------------------------------------
  ------ System Control Block (SCB) Application interrupt and reset control register (AIRCR) ------
  -------------------------------------------------------------------------------------------------
  Bit#31 xxxx xxxx xxxx xxxx xxxx xxxx xxxx xxxx Bit#0
         |||| |||| |||| |||| |||| |||| |||| ||||
         |||| |||| |||| |||| |||| |||| |||| |||+-- (w)  VECTRESET: must write 0, otherwise behavior is unpredictable
         |||| |||| |||| |||| |||| |||| |||| ||+--- (w)  VECTCLRACTIVE: must write 0, otherwise behavior is unpredictable
         |||| |||| |||| |||| |||| |||| |||| |+---- (w)  SYSRESETREQ: System reset request (0: No reset request; 1: request a reset)
         |||| |||| |||| |||| |||| |||| ++++-+----- reserved
         |||| |||| |||| |||| |||| |+++------------ (rw) PRIGROUP[2:0]: Interrupt priority grouping field
         |||| |||| |||| |||| |+++-+--------------- reserved
         |||| |||| |||| |||| +-------------------- (r)  ENDIANESS Data endianness bit (0: little endian)
         ++++-++++-++++-++++---------------------- (rw) VECTKEYSTAT[15:0]/ VECTKEY[15:0] Register key (write 0x5FA at first to enable write access)
  -------------------------------------------------------------------------------------------------
  */

               /* enable write access to SCB_AIRCR register | Priority Group should not be changed  | System reset request */
  SCB->AIRCR  = ( (0x5FA << SCB_AIRCR_VECTKEY_Pos)          | (SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk) | SCB_AIRCR_SYSRESETREQ_Msk );

  __DSB();    /* Data Synchronization Barrier - ensures that outstanding memory transactions complete before subsequent
                                                instructions execute */

  return;
}





FUNCTION VOID Reset_Watchdog (VOID)
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

Reset/Clear the external Watchdog

MAXIM Datasheet for MAX6301ESA Low-Power uP Supervisory Circuits with Adjustable Reset/Watchdog

 "WDI: Watchdog Input. A rising or falling transition must occur on this input within the selected watchdog
  timeout period, or a reset pulse will occur. The capacitor value selected for SWT and the state of WDS
  determine the watchdog timeout period. The watchdog timer clears and restarts when a transition
  occurs on WDI or WDS. The watchdog timer is cleared when reset is asserted and restarted after reset
  deasserts. In the extended watchdog mode (WDS = VCC), the watchdog function can be disabled by
  driving WDI with a three-stated driver or by leaving WDI unconnected."
------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

#if 1

    if ((GPIOA->ODR & ((USIGN16)0x0002)) != 0u)
    {   // The pulse will go LHL and stay L
        GPIOA->BRR  = (USIGN16)0x0002;
        GPIOA->BSRR = (USIGN32)0x0002;
        GPIOA->BRR  = (USIGN16)0x0002;
    }
    else
    {   // The pulse will go HLH and stay H
        GPIOA->BSRR = (USIGN32)0x0002;
        GPIOA->BRR  = (USIGN16)0x0002;
        GPIOA->BSRR = (USIGN32)0x0002;
    }

#else
    // Accordingly to the MAX EWD spec this consumes less power
    // than the code above. This is more difficult to observe
    // on the scope - short pulse.

    GPIOA->BSRR = (USIGN32)0x0002;
    GPIOA->BRR  = (USIGN16)0x0002;

#endif

    return;
}

//-------------------------------------------------------------------------------
// Diagnostic functions to manipulate PA 5 and PA6
// Note, that these functions require proper configuration of the GPIO regs.

FUNCTION VOID SetPA_5(VOID)
{
    GPIOA->BSRR = (USIGN32)0x0020;
}

FUNCTION VOID SetPA_6(VOID)
{
    GPIOA->BSRR = (USIGN32)0x0040;
}

FUNCTION VOID ResetPA_5(VOID)
{
    GPIOA->BRR  = (USIGN16)0x0020;
}

FUNCTION VOID ResetPA_6(VOID)
{
    GPIOA->BRR  = (USIGN16)0x0040;
}
//-------------------------------------------------------------------------------


/******************************************************************************/


FUNCTION GLOBAL VOID hw_init_set_fbc_isr_clk (VOID)
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:

PARAMETERS:

GLOBALS_AFFECTED:

RETURN_VALUES:

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

    return;
} /* FUNCTION hw_init_set_cpu_clk */


/******************************************************************************/


FUNCTION GLOBAL VOID hw_init_set_default_clk (VOID)
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:

PARAMETERS:

GLOBALS_AFFECTED:

RETURN_VALUES:

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY


    return;
} /* FUNCTION hw_init_set_cpu_clk */




