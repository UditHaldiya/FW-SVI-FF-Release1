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

FILE_NAME          hw_periph_cfg.c



FUNCTIONAL_MODULE_DESCRIPTION

This modul contains the code for configuration of the ARM Cortex-M3 Peripherals.

=========================================================================== */
#include "keywords.h"   /* keyword defines */
#define  MODULE_ID      (COMP_HW + MOD_HWCONf)

INCLUDES

#include "base.h"
#include "except.h"
#include "hw_if.h"
#include "hw_cfg.h"

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_fsmc.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_tim.h"
#include "misc.h"
#include "stm32f10x.h"

GLOBAL_DEFINES

LOCAL_DEFINES

EXPORT_TYPEDEFS

LOCAL_TYPEDEFS

FUNCTION_DECLARATIONS

  FUNCTION PUBLIC VOID hw_cfg_GPIO (VOID);
  FUNCTION PUBLIC VOID hw_cfg_EXTI (FunctionalState action);
  FUNCTION PUBLIC VOID hw_cfg_FSMC (VOID);
  FUNCTION PUBLIC VOID hw_cfg_TIMx (VOID);

EXPORT_DATA

IMPORT_DATA

LOCAL_DATA




FUNCTION PUBLIC VOID hw_cfg_GPIO (VOID)
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:

  Configure the General-Purpose I/Os (GPIO).

PARAMETERS:

GLOBALS_AFFECTED:

RETURN_VALUES:

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

  GPIO_InitTypeDef GPIO_InitStructure;

FUNCTION_BODY

  GPIO_StructInit(&GPIO_InitStructure);



  /*****************************************************************************************
                                        GPIOA
  *****************************************************************************************/

#if 0
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  RCC_MCOConfig(RCC_MCO_HSE);
#endif

  GPIO_DeInit(GPIOA);

  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

#if 0
  /* Configure PAx outputs */
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;      /* PA1/WDT_FF as output to ext. Watchdog */
// the following two lines are superfluous.  They are set blow as USART TX functions which makes them output
//                                 GPIO_Pin_2  |    /* PA2/FF2APP_FF HART Tx */
//                                 GPIO_Pin_9  ;    /* PA9/ISP-UART-TX_FF */

#else
  // This enables PA1 -- WD, and PA5 and PA6 -- diag pins; as OUT, Push/Pull; 2MHz
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1 | GPIO_Pin_5 | GPIO_Pin_6;      /* PA1/WDT_FF as output to ext. Watchdog */
#endif

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  Reset_Watchdog();

  /* Configure PAx inputs */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3  |    /* PA3/APP2FF_FF HART Rx */
                                 GPIO_Pin_7  |    /* PA7/SPC_CLK_2 to SPC4-2 */
                                 GPIO_Pin_8  |    /* PA7/SPC_CLK_1 to SPC4-2 */
                                 GPIO_Pin_10 ;    /* PA10/ISP-UART-RX_FF */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

  /* Configure UNUSED PAx inputs */
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0  | GPIO_Pin_4  | GPIO_Pin_11 | GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_SetBits(GPIOA, GPIO_Pin_0  | GPIO_Pin_4  | GPIO_Pin_11 | GPIO_Pin_12);

  /* Configure UNUSED PAx inputs JTAG related */
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_SetBits(GPIOA, GPIO_Pin_13 | GPIO_Pin_15);

  /* Configure UNUSED PAx inputs JTAG related */
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_ResetBits(GPIOA, GPIO_Pin_14);


  /* Configure PAx alternate function */
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2  |    /* PA2/FF2APP_FF HART Tx */
                                 GPIO_Pin_9  ;    /* PA9/ISP-UART-TX_FF */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  // This resets bunch of pins to 0 (LO).
  GPIO_ResetBits(GPIOA, GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_9); /* set high active outputs to 0 initially */

  Reset_Watchdog();

  /*****************************************************************************************
                                        GPIOB
  *****************************************************************************************/

  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

  /* Configure PBx outputs */
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_9  |    /* PB9/IDENT_SDA to Dallas Security EEPROM */
                                 //GPIO_Pin_10 |    /* PB10/SCL1 to FRAM */
                                 GPIO_Pin_12 ;    /* PB12/~WP_FRAM to FRAM */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  Reset_Watchdog();

  /* Configure PBx inputs */
  GPIO_InitStructure.GPIO_Pin  = /*GPIO_Pin_0  |*/    /* PB0/XINT_SPC to SPC4-2 */
                                 GPIO_Pin_2  |    /* PB2/BOOT1 */
                                 GPIO_Pin_6  |    /* PB6/SIM_FF Simulate Jumper */
                                 GPIO_Pin_7  |    /* PB7/WP_FF HW Write Protect Jumper */
                                 GPIO_Pin_8  |    /* PB8/IDENT_SCL to Dallas Security EEPROM */
                                 //GPIO_Pin_11 |    /* PB11/SDA1 to FRAM */
                                 GPIO_Pin_13 ;    /* PB13/NVRAM_ERASE Jumper to erase EERPOM during start-up */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  Reset_Watchdog();

  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0  ;    /* PB0/XINT_SPC to SPC4-2 */
  // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* Configure UNUSED PBx inputs */
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1  | GPIO_Pin_5  | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_SetBits(GPIOB, GPIO_Pin_1  | GPIO_Pin_5  | GPIO_Pin_14 | GPIO_Pin_15);

  /* Configure PBx alternate function */
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10 |    /* PB10/SCL1 to FRAM */
                                 GPIO_Pin_11 ;    /* PB11/SDA1 to FRAM */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  Reset_Watchdog();

  //GPIO_ResetBits(GPIOB, GPIO_Pin_9); /* set high active outputs to 0 initially */
  GPIO_SetBits(GPIOB, GPIO_Pin_9); /* set high active outputs to 0 initially */

  GPIO_SetBits(GPIOB, GPIO_Pin_12); /* set low active outputs to 1 initially */

  /*****************************************************************************************
                                        GPIOC
  *****************************************************************************************/

  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

  /* Configure PCx outputs */
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0  |    /* PC0/SPC_RES */
                                 GPIO_Pin_12 ;    /* PC12/DEBUG_TX_FF */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  Reset_Watchdog();
  /* Configure PCx inputs */
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1  ;    /* PC1/XINTC1_SPC from SPC4-2 */
  // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_SetBits(GPIOC, GPIO_Pin_1);
  Reset_Watchdog();

  /* Configure UNUSED PCx inputs */
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2  | GPIO_Pin_3  | GPIO_Pin_4  | GPIO_Pin_5  |
                                 GPIO_Pin_6  | GPIO_Pin_7  | GPIO_Pin_8  | GPIO_Pin_9  |
                                 GPIO_Pin_10 | GPIO_Pin_11 |
                                 GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_SetBits(GPIOC, GPIO_Pin_2  | GPIO_Pin_3  | GPIO_Pin_4  | GPIO_Pin_5  |
                      GPIO_Pin_6  | GPIO_Pin_7  | GPIO_Pin_8  | GPIO_Pin_9  |
                      GPIO_Pin_10 | GPIO_Pin_11 |
                      GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);

  Reset_Watchdog();

  GPIO_SetBits(GPIOC, GPIO_Pin_0 ); /* assert reset */

  GPIO_ResetBits(GPIOC, GPIO_Pin_12 ); /* set high active outputs to 0 initially */


  /*****************************************************************************************
                                        GPIOD
  *****************************************************************************************/

  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

  /* Configure PDx outputs */
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3  ;    /* PD3/TP305 Test Point */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* Configure PDx inputs */
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2  ;    /* PD2/DEBUG_RX_FF */
  // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  GPIO_SetBits(GPIOD, GPIO_Pin_2);
  Reset_Watchdog();

  /* Configure UNUSED PDx inputs */
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  GPIO_SetBits(GPIOD, GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10);

  Reset_Watchdog();

  /* Configure PDx alternate function */
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0  |    /* PD0/D2 FSMC data line */
                                 GPIO_Pin_1  |    /* PD1/D3 FSMC data line */
                                 GPIO_Pin_4  |    /* PD4/~RD FSMC-NOE */
                                 GPIO_Pin_5  |    /* PD5/~WR FSMC-NWE */
                                 /*GPIO_Pin_6  |*/    /* PD6/FSMC NWAIT */ //Not sure whether connected or not!!!!
                                 GPIO_Pin_7  |    /* PD7/~CS_SRAM FSMC-NE1 */
                                 GPIO_Pin_11 |    /* PD11/A16 FSMC address line */
                                 GPIO_Pin_12 |    /* PD12/A17 FSMC address line */
                                 GPIO_Pin_13 |    /* PD13/A18 FSMC address line */
                                 GPIO_Pin_14 |    /* PD14/D0 FSMC data line */
                                 GPIO_Pin_15 ;    /* PD15/D1 FSMC data line */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  Reset_Watchdog();
  GPIO_ResetBits(GPIOD, GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 ); /* set high active outputs to 0 initially */

  GPIO_SetBits(GPIOD, GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7 ); /* set low active outputs to 1 initially */


  /*****************************************************************************************
                                        GPIOE
  *****************************************************************************************/

  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

  /* Configure UNUSED PEx INPUTS */
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_2  | GPIO_Pin_3 |
                                 GPIO_Pin_4  | GPIO_Pin_5  | GPIO_Pin_6  | GPIO_Pin_7 |
                                 GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 | GPIO_Pin_11|
                                 GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  GPIO_SetBits(GPIOE, GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_2  | GPIO_Pin_3 |
                      GPIO_Pin_4  | GPIO_Pin_5  | GPIO_Pin_6  | GPIO_Pin_7 |
                      GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 | GPIO_Pin_11|
                      GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
  Reset_Watchdog();

  /* Configure PEx outputs */
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4  |    /* PE4/TP301 Test Point */
                                 GPIO_Pin_5  ;    /* PE5/TP302 Test Point */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  Reset_Watchdog();
  /* Configure PEx alternate function */
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_7  |    /* PE7/D4 FSMC data line */
                                 GPIO_Pin_8  |    /* PE8/D5 FSMC data line */
                                 GPIO_Pin_9  |    /* PE9/D6 FSMC data line */
                                 GPIO_Pin_10 ;    /* PE10/D7 FSMC data line */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOE, &GPIO_InitStructure);


  /*****************************************************************************************
                                        GPIOF
  *****************************************************************************************/
  /* Configure UNUSED PFx INPUTS */

  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_2  | GPIO_Pin_3 |
                                 GPIO_Pin_4  | GPIO_Pin_5  | GPIO_Pin_6  | GPIO_Pin_7 |
                                 GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 | GPIO_Pin_11|
                                 GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  GPIO_SetBits(GPIOF, GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_2  | GPIO_Pin_3 |
                      GPIO_Pin_4  | GPIO_Pin_5  | GPIO_Pin_6  | GPIO_Pin_7 |
                      GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 | GPIO_Pin_11|
                      GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
  Reset_Watchdog();

  /* Configure PFx alternate function */
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0  |    /* PF0/A0 FSMC address line */
                                 GPIO_Pin_1  |    /* PF1/A1 FSMC address line */
                                 GPIO_Pin_2  |    /* PF2/A2 FSMC address line */
                                 GPIO_Pin_3  |    /* PF3/A3 FSMC address line */
                                 GPIO_Pin_4  |    /* PF4/A4 FSMC address line */
                                 GPIO_Pin_5  |    /* PF5/A5 FSMC address line */
                                 GPIO_Pin_12 |    /* PF12/A6 FSMC address line */
                                 GPIO_Pin_13 |    /* PF13/A7 FSMC address line */
                                 GPIO_Pin_14 |    /* PF14/A8 FSMC address line */
                                 GPIO_Pin_15 ;    /* PF15/A9 FSMC address line */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  Reset_Watchdog();
  GPIO_ResetBits(GPIOF, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 |
                        GPIO_Pin_5 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15
                ); /* set high active outputs to 0 initially */


  /*****************************************************************************************
                                        GPIOG
  *****************************************************************************************/
  /* Configure UNUSED PGx INPUTS */

  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_2  | GPIO_Pin_3 |
                                 GPIO_Pin_4  | GPIO_Pin_5  | GPIO_Pin_6  | GPIO_Pin_7 |
                                 GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 | GPIO_Pin_11|
                                 GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
  GPIO_SetBits(GPIOG, GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_2  | GPIO_Pin_3 |
                      GPIO_Pin_4  | GPIO_Pin_5  | GPIO_Pin_6  | GPIO_Pin_7 |
                      GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 | GPIO_Pin_11|
                      GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
  Reset_Watchdog();

  /* Configure PGx alternate function */
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0  |    /* PG0/A10 FSMC address line */
                                 GPIO_Pin_1  |    /* PG1/A11 FSMC address line */
                                 GPIO_Pin_2  |    /* PG2/A12 FSMC address line */
                                 GPIO_Pin_3  |    /* PG3/A13 FSMC address line */
                                 GPIO_Pin_4  |    /* PG4/A14 FSMC address line */
                                 GPIO_Pin_5  |    /* PG5/A15 FSMC address line */
                                 GPIO_Pin_9  ;    /* PG9/~CS_SPC FSMC-NE2 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
  Reset_Watchdog();
  GPIO_ResetBits(GPIOG, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 |
                        GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5
                ); /* set high active outputs to 0 initially */

  GPIO_SetBits(GPIOD, GPIO_Pin_9 ); /* set low active outputs to 1 initially */


  Reset_Watchdog();

  return;

}




FUNCTION PUBLIC VOID hw_cfg_EXTI (FunctionalState action)
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:

  Configure the External Interrupt Controller.

PARAMETERS:

  action -  ENABLE the EXTI Lines
            DISABLE the EXTI Lines

GLOBALS_AFFECTED:

RETURN_VALUES:

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

  EXTI_InitTypeDef EXTI_InitStructure;

FUNCTION_BODY


  if (action == ENABLE)
  {
      /* Connect EXTI Line to GPIO Pin */
      GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);  /* ~XINT from SPC4-2 */
  }

  /* Configure EXTI line */
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = action;
  EXTI_Init(&EXTI_InitStructure);

  return;

}




FUNCTION PUBLIC VOID hw_cfg_NVIC (VOID)
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:

  Configure the Nested Vectored Interrupt Controller.

  The lower the NVIC_IRQChannelPreemptionPriority value, the greater the priority
  of the corresponding interrupt.

PARAMETERS:

GLOBALS_AFFECTED:

RETURN_VALUES:

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

  NVIC_InitTypeDef NVIC_InitStructure;

FUNCTION_BODY

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); /* use all 4 bits for pre-emption priority (0 bits for subpriority) */

  /* Configure/Enable the SPC interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 13; /* set to high priority --> equivalent to priority 3 of embOS? */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  /* subpriority not used for PriorityGroup 4 */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Configure/Enable the TIM10 interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 13; /* equivalent to priority 3 of embOS? */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  /* subpriority not used for PriorityGroup 4 */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

//  /* Configure/Enable the I2C2 interrupt */
//  NVIC_InitStructure.NVIC_IRQChannel = I2C2_EV_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 11; /* equivalent to priority 2 of embOS? */
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  /* subpriority not used for PriorityGroup 4 */
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);

  /* Configure/Enable the TIM11 interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_TRG_COM_TIM11_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 12; /* equivalent to priority 4 of embOS? */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  /* subpriority not used for PriorityGroup 4 */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Configure/Enable the USART2 interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 12; /* equivalent to priority 4 of embOS? */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  /* subpriority not used for PriorityGroup 4 */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Configure/Enable the DMA Channel 6 interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel6_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 12; /* equivalent to priority 4 of embOS? */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  /* subpriority not used for PriorityGroup 4 */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Configure/Enable the DMA Channel 7 interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 14; /* equivalent to priority 4 of embOS? */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  /* subpriority not used for PriorityGroup 4 */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Configure/Enable the TIM6 interrupt */
  //NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
  //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 12; /* equivalent to priority 4 of embOS? */
  //NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  /* subpriority not used for PriorityGroup 4 */
  //NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  //NVIC_Init(&NVIC_InitStructure);

  return;

}




FUNCTION PUBLIC VOID hw_cfg_FSMC (VOID)
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:

  Configure the use of Flexible Static Memory Controller for access to SRAM and SPC4-2.

PARAMETERS:

GLOBALS_AFFECTED:

RETURN_VALUES:

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  /* configure FSMC for ext SRAM usage */
  /* FSMC Bank1_SRAM 1 is used    */
  FSMC_Bank1->BTCR[0] = (
                          FSMC_DataAddressMux_Disable|FSMC_MemoryType_SRAM|FSMC_MemoryDataWidth_8b|FSMC_BurstAccessMode_Disable|
                          FSMC_WaitSignalPolarity_Low|FSMC_WrapMode_Disable|FSMC_WaitSignalActive_BeforeWaitState|FSMC_WriteOperation_Enable|
                          FSMC_WaitSignal_Disable|FSMC_ExtendedMode_Disable|FSMC_WriteBurst_Disable
                        );
  /* DATAST phase duration = 3 × HCLK clock cycles */
  FSMC_Bank1->BTCR[1] = 0x00000200; /* chip-select timing register BTR1 */

  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);

  /* configure FSMC for SPC4-2 FIFO usage */
  /* FSMC Bank1_SRAM 2 is used    */
  FSMC_Bank1->BTCR[2] = (
                          FSMC_DataAddressMux_Disable|FSMC_MemoryType_SRAM|FSMC_MemoryDataWidth_8b|FSMC_BurstAccessMode_Disable|
                          FSMC_WaitSignalPolarity_Low|FSMC_WrapMode_Disable|FSMC_WaitSignalActive_BeforeWaitState|FSMC_WriteOperation_Enable|
                          /*FSMC_WaitSignal_Enable*/ FSMC_WaitSignal_Disable|FSMC_ExtendedMode_Disable|FSMC_WriteBurst_Disable
                        );
  /* DATAST phase duration = 3 X HCLK cycles | Data Latency of 2 CLK cycles to first burst access */
  FSMC_Bank1->BTCR[3] = 0x00000200; /* chip-select timing register BTR2 */

  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM2, ENABLE);


  return;

}




FUNCTION PUBLIC VOID hw_cfg_TIMx (VOID)
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:

  Update_event = TIM_CLK/((PSC + 1)*(ARR + 1)*(RCR + 1))

PARAMETERS:

GLOBALS_AFFECTED:

RETURN_VALUES:

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

FUNCTION_BODY

  /******************* Configure Timer TIM10 for FB Task Execution *******************/
  /* HCLK = 4MHz, APB2 PSC = 2 --> TIMxCLK = PCLK2 * 2 --> TIMxCLK = HCLK/2 * 2 = 4MHz */
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

  TIM_TimeBaseStructure.TIM_Prescaler = 15; /* used PSC = TIM_Prescaler + 1 = 16 */

  TIM_TimeBaseInit(TIM10, &TIM_TimeBaseStructure);

  TIM_UpdateDisableConfig(TIM10, ENABLE);

  TIM_ARRPreloadConfig(TIM10, ENABLE);

  TIM_UpdateRequestConfig(TIM10, TIM_UpdateSource_Regular);

  TIM_ClearITPendingBit(TIM10, 0x0001);

  TIM_Cmd(TIM10, DISABLE);

  TIM_ITConfig(TIM10, 0x0001, ENABLE);


  /******************* Configure Timer TIM6 for Single Wire EEPROM *******************/

  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

  TIM_TimeBaseStructure.TIM_Prescaler = 15; /* used PSC = TIM_Prescaler + 1 = 16 */

  TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

  //TIM_SelectOnePulseMode(TIM6, TIM_OPMode_Single);

  TIM_UpdateDisableConfig(TIM6, ENABLE);

  TIM_ARRPreloadConfig(TIM6, DISABLE);

  TIM_SetAutoreload(TIM6, 0xFFFF);

  TIM_UpdateRequestConfig(TIM6, TIM_UpdateSource_Regular);

  TIM_ClearITPendingBit(TIM6, 0x0001);

  //TIM_Cmd(TIM6, DISABLE);
  TIM_Cmd(TIM6, ENABLE);

  TIM_ITConfig(TIM6, 0x0001, DISABLE);


  /******************* Configure Timer TIM11 for HART communication *******************/

  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

  TIM_TimeBaseStructure.TIM_Prescaler = 15; /* used PSC = TIM_Prescaler + 1 = 16 */

  TIM_TimeBaseInit(TIM11, &TIM_TimeBaseStructure);

  TIM_UpdateDisableConfig(TIM11, ENABLE);

  TIM_ARRPreloadConfig(TIM11, ENABLE);

  TIM_UpdateRequestConfig(TIM11, TIM_UpdateSource_Regular);

  TIM_ClearITPendingBit(TIM11, 0x0001);

  TIM_Cmd(TIM11, DISABLE);

  TIM_ITConfig(TIM11, 0x0001, ENABLE);


  /******************* Configure Timer TIM14 for Segger embOS usage *******************/

  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

  TIM_TimeBaseStructure.TIM_Prescaler = 15; /* used PSC = TIM_Prescaler + 1 = 16 */

  TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);

  TIM_UpdateDisableConfig(TIM14, ENABLE);

  TIM_ARRPreloadConfig(TIM14, ENABLE);

  TIM_UpdateRequestConfig(TIM14, TIM_UpdateSource_Regular);

  TIM_ClearITPendingBit(TIM14, 0x0001);

  TIM_Cmd(TIM14, DISABLE);

  TIM_ITConfig(TIM14, 0x0001, ENABLE);


  return;

}



#if 0
FUNCTION PUBLIC VOID hw_cfg_EXTI (VOID)
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

}




FUNCTION PUBLIC VOID hw_cfg_EXTI (VOID)
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

}




FUNCTION PUBLIC VOID hw_cfg_EXTI (VOID)
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

}
#endif

