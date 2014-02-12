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

FILE_NAME          hw_hm.c



FUNCTIONAL_MODULE_DESCRIPTION

This modul contains the code for HART master access on hardware.

=========================================================================== */
#include "keywords.h"   /* keyword defines */
#define  MODULE_ID      (COMP_HW + MOD_HWHM)

INCLUDES

#include "base.h"
#include "except.h"
#include "hw_if.h"
#include "hw_cfg.h"
#include "hm_cfg.h"

#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"

#if !defined (__IAR_SYSTEMS_ICC__) && !defined (STM32F10X_XL)
#error Invalid compiler and / or CPU type !!
#endif

GLOBAL_DEFINES

LOCAL_DEFINES

    /* Define external CLOCK value for HART UART */
    /*-------------------------------------------*/
#ifdef EXTERNAL_HART_CLOCK_USED
  #define HART_CLOCK                   xxxxxxxUL /* Define external clock here! */
#endif /* EXTERNAL_HART_CLOCK_USED */

  /* USART OverSampling-8 Mask */
  #define CR1_OVER8_Set             ((USIGN16)0x8000)  /* USART OVER8 mode Enable Mask */

EXPORT_TYPEDEFS

LOCAL_TYPEDEFS

FUNCTION_DECLARATIONS

EXPORT_DATA

IMPORT_DATA

  extern RCC_ClocksTypeDef RCC_ClockFreqs;

LOCAL_DATA




FUNCTION GLOBAL VOID hw_hm_init(VOID)
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION


------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

  USART_InitTypeDef USART_InitStructure;

FUNCTION_BODY

  /* condifure USART2 peripheral */
  USART_InitStructure.USART_BaudRate = 1200;                                      /* set as initial baudrate */
  USART_InitStructure.USART_WordLength = USART_WordLength_9b;                     /* 8 data bits + 1 parity bit */
  USART_InitStructure.USART_StopBits = USART_StopBits_1;                          /* HART defines 1 stop bit */
  USART_InitStructure.USART_Parity = USART_Parity_Odd;                            /* HART parity is fixed defined as ODD */
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  /* Configure USART2 peripheral */
  USART_Init(USART2, &USART_InitStructure);

  /* Enable USART2 Receive and Transmit interrupts */
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

  /* Enable the USART2 peripheral */
  USART_Cmd(USART2, ENABLE);

  HM_STOP_TIMER();


#if 11*TIM11CLK/(TIMER_BPS*32)<=65536 && 11*TIM11CLK/(TIMER_BPS*8)>65536

  HART_TMR  = 15; /* Timer mode, Clock = f/16 */
  HART_TVAL = (USIGN16)(11*TIM11CLK/(TIMER_BPS*16)-1); /* one character time = 11 bit times */

#elif 11*TIM11CLK/(TIMER_BPS*8)<=65536 && 11*TIM11CLK/(TIMER_BPS*1)>65536

  HART_TMR  = 7; /* Timer mode, Clock = f/8 */
  HART_TVAL = (USIGN16)(11*TIM11CLK/(TIMER_BPS*8)-1); /* one character time = 11 bit times */

#elif 11*TIM11CLK/(TIMER_BPS*1)<=65536

  HART_TMR  = 0; /* Timer mode, Clock = f/1 */
  HART_TVAL = (USIGN16)(11*TIM11CLK/(TIMER_BPS*1)-1); /* one character time = 11 bit times */

#else
  #pragma error unsupported bitrate!
#endif

  HM_START_TIMER();

  return;

} /* FUNCTION hw_hm_init */



/******************************************************************************/

FUNCTION GLOBAL USIGN8 hw_hm_set_baudrate
  (
    IN USIGN16  baudrate
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
    Sets the communication speed of the HART interface. Valid baudrates are:
    1200, 2400, 4800, 9600, 19200, 38400

PARAMETERS:
    baudrate:       HART baudrate to be set

GLOBALS_AFFECTED:
    none

RETURN_VALUES:
    0 =>            new baudrate set successfully
    1 =>            invalid baudrate value

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
    USIGN32 br_val = 0x00;

    USIGN32 apbclock = 0x00;
    USIGN32 integerdivider = 0x00;
    USIGN32 fractionaldivider = 0x00;

FUNCTION_BODY

#ifdef EXTERNAL_HART_CLOCK_USED
  #undef XCLOCK
  #define XCLOCK      HART_CLOCK
#endif /* EXTERNAL_HART_CLOCK_USED */

    switch (baudrate) {
    case 1200:
    case 2400:
    case 4800:
    case 9600:
    case 14400:
    case 19200:
    case 28800:
    case 38400:


        /* Configure the USART Baud Rate -------------------------------------------*/
        /* from standard peripheral driver stm32f10x_usart.c                        */

        apbclock = RCC_ClockFreqs.PCLK1_Frequency;

        /* Determine the integer part */
        if ((USART2->CR1 & CR1_OVER8_Set) != 0)
        {
          /* Integer part computing in case Oversampling mode is 8 Samples */
          integerdivider = ((25 * apbclock) / (2 * (baudrate)));
        }
        else /* if ((USARTx->CR1 & CR1_OVER8_Set) == 0) */
        {
          /* Integer part computing in case Oversampling mode is 16 Samples */
          integerdivider = ((25 * apbclock) / (4 * (baudrate)));
        }
        br_val = (integerdivider / 100) << 4;

        /* Determine the fractional part */
        fractionaldivider = integerdivider - (100 * (br_val >> 4));

        /* Implement the fractional part in the register */
        if ((USART2->CR1 & CR1_OVER8_Set) != 0)
        {
          br_val |= ((((fractionaldivider * 8) + 50) / 100)) & ((USIGN8)0x07);
        }
        else /* if ((USARTx->CR1 & CR1_OVER8_Set) == 0) */
        {
          br_val |= ((((fractionaldivider * 16) + 50) / 100)) & ((USIGN8)0x0F);
        }

        /* Write to USART BRR */
        USART2->BRR = (USIGN16)br_val;

        break;

    default:
        return (1);
    } /* end switch */

    return (0);
} /* FUNCTION hw_hm_set_baudrate */



