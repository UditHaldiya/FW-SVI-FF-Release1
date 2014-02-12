#ifndef _MCU_H
/*******************************************************************************
* Copyright (c) 2013 by Dresser Masoneilan . All rights reserved. No part of this
* document or its associated software may be reproduced, transcribed, stored in
* a retrieval system, translated into any language or computer language,
* or transmitted in any form whatsoever without the prior written consent.
********************************************************************************
* Project    :  Rapid Prototype Test
*
* Platform   :  STM32F101ZGT6
*
* Package    :  -
*
* Compiler   :  IAR C/C++ Compiler for ARM
*
* Date       :  2011-10-26
* Author     :  C.Heil , MESCO Engineering GmbH
*
*******************************************************************************/
/*! \brief   Register and port mapping
*   \file    mcu.h
*
\verbatim
********************************************************************************
*  Changes
********************************************************************************
* Date        Sgn   Ver.  Changes
* 2011-10-26  ch    0.1   Created
**************************************************************** \endverbatim */

/*******************************************************************************
*** global definitions
*******************************************************************************/
#define _MCU_H

// Define the desired target CPU..

#ifndef _WIN32

// Include corresponding header files..
  #include <intrinsics.h>

  #include "..\..\STM32F10x_StdPeriph_Driver\inc\stm32f10x_gpio.h"

#endif


/*******************************************************************************
*** external functions (export)
*******************************************************************************/
void init_mcu (void);

/*******************************************************************************
*** global definitions
*******************************************************************************/
#ifdef _WIN32
	#define disable_interrupt()	
	#define enable_interrupt()	
#else
/* Defines for intrinsic functions */
	#define disable_interrupt()	__disable_interrupt()// Disable all interrupts
	#define enable_interrupt()	__enable_interrupt() // Enable all interrupts
#endif

#define ISASCII(c)  ( ((c >= '0') && (c <= '9')) ||\
                      ((c >= 'a') && (c <= 'z')) ||\
                      ((c >= 'A') && (c <= 'Z')) ||\
                        (c==0x0d))?(TRUE):(FALSE)
//..........................................................


/*** I/O-PORTS ***/

#ifdef _WIN32

  #define FRAM_WR_ACTIVE    
  #define FRAM_WR_INACTIVE  
  #define READ_WP_FF        TRUE      
  #define READ_SIM_FF       TRUE
  #define READ_NVRAM_ERASE  TRUE
  #define ENABLE_FRAM_WRP() 
  #define DISABLE_FRAM_WRP()

  #define IDENT_SCLHI()     
  #define IDENT_SCLLO()     
  #define IDENT_SDAHI()     
  #define IDENT_SDALO()     
  #define USART2_HI()      

  #define WATCHDOG_REFRESH 

  #define SPC4_RESET_HIGH()
  #define SPC4_RESET_LOW() 

  /* debug pins */
  #define TP301_HIGH()     
  #define TP302_HIGH()     
  #define TP301_LOW()      
  #define TP302_LOW()      

  #define SEND_INT_BYTE(a) 
  #define SEND_DBG_BYTE(a) 
  #define RECEIVE_INT_BYTE()          0
  #define RECEIVE_DBG_BYTE()          0
  #define USART0_RX_IS_PARITY_ERROR() TRUE
  #define USART0_RX_IS_OVERRUN_ERROR()TRUE
  #define USART0_RX_IS_FRAME_ERROR()  TRUE
  #define ENABLE_DBGUART_TXINT()      
  #define DISABLE_DBGUART_TXINT()     
  #define ENABLE_INTUART_TXINT()      
  #define DISABLE_INTUART_TXINT()     

  #define INTUART_RECEIVEPENDING()    TRUE
  #define INTUART_SENDINT()           TRUE
  #define DBGUART_RECEIVEPENDING()    TRUE
  #define DBGUART_SENDINT()           TRUE
  #define DBGUART_SENDREGNOTEMPTY()      TRUE
  #define INTUART_SENDREGNOTEMPTY()      TRUE


#define I2C_EVENT_MASTER_MODE_SELECT TRUE
#define I2C_Direction_Transmitter 0x00
#define I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED 0x00
#define I2C_EVENT_MASTER_BYTE_TRANSMITTED 0x00
#define SET TRUE
#define I2C_FLAG_AF 0x00

#define ENABLE 1
#define DISABLE 0

#define SUCCESS 1

#define I2C_FLAG_BUSY 1
#define I2C_Direction_Receiver 1
#define I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED 0
#define __IO 
#define SR1_Tmp test
#define I2C_Register_SR1 1
#define I2C_Register_SR1 1
#define I2C_EVENT_MASTER_BYTE_RECEIVED 1


#else

  #define FRAM_WR_ACTIVE    GPIO_SetBits(GPIOB,GPIO_Pin_12)
  #define FRAM_WR_INACTIVE  GPIO_ResetBits(GPIOB,GPIO_Pin_12)
  #define READ_WP_FF        GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7)
  #define READ_SIM_FF       GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)
  #define READ_NVRAM_ERASE  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)  
  #define ENABLE_FRAM_WRP()  GPIO_SetBits(GPIOB, GPIO_Pin_12);
  #define DISABLE_FRAM_WRP() GPIO_ResetBits(GPIOB, GPIO_Pin_12);

  #define IDENT_SCLHI()       GPIO_SetBits(GPIOB, GPIO_Pin_8);
  #define IDENT_SCLLO()       GPIO_ResetBits(GPIOB, GPIO_Pin_8)
  #define IDENT_SDAHI()       GPIO_SetBits(GPIOB, GPIO_Pin_9);
  #define IDENT_SDALO()       GPIO_ResetBits(GPIOB, GPIO_Pin_9)
  #define USART2_HI()         GPIO_SetBits(GPIOA, GPIO_Pin_2);

  #define WATCHDOG_REFRESH {GPIO_SetBits(GPIOA,GPIO_Pin_1);GPIO_ResetBits(GPIOA,GPIO_Pin_1);}

  #define SPC4_RESET_HIGH() {GPIO_SetBits(GPIOC,GPIO_Pin_0);}
  #define SPC4_RESET_LOW()  {GPIO_ResetBits(GPIOC,GPIO_Pin_0);}

  /* debug pins */
  #define TP301_HIGH()      {GPIO_SetBits(GPIOE,GPIO_Pin_4);}
  #define TP302_HIGH()      {GPIO_SetBits(GPIOE,GPIO_Pin_5);}
  #define TP301_LOW()       {GPIO_ResetBits(GPIOE,GPIO_Pin_4);}
  #define TP302_LOW()       {GPIO_ResetBits(GPIOE,GPIO_Pin_5);}

  #define SEND_INT_BYTE(a)      USART_SendData(USART2, a)
  #define SEND_DBG_BYTE(a)      USART_SendData(UART5, a)
  #define RECEIVE_INT_BYTE()    USART_ReceiveData(USART2)
  #define RECEIVE_DBG_BYTE()    USART_ReceiveData(UART5)
  #define USART0_RX_IS_PARITY_ERROR() (USART_GetITStatus(USART2, USART_IT_PE) != RESET )
  #define USART0_RX_IS_OVERRUN_ERROR()(USART_GetITStatus(USART2, USART_IT_ORE) != RESET )
  #define USART0_RX_IS_FRAME_ERROR()  (USART_GetITStatus(USART2, USART_IT_FE) != RESET )
  #define ENABLE_DBGUART_TXINT()      USART_ITConfig(UART5, USART_IT_TXE, ENABLE)
  #define DISABLE_DBGUART_TXINT()     USART_ITConfig(UART5, USART_IT_TXE, DISABLE)
  #define ENABLE_INTUART_TXINT()      USART_ITConfig(USART2, USART_IT_TXE, ENABLE)
  #define DISABLE_INTUART_TXINT()      USART_ITConfig(USART2, USART_IT_TXE, DISABLE)

  #define INTUART_RECEIVEPENDING()    (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
  #define INTUART_SENDINT()           (USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
  #define DBGUART_RECEIVEPENDING()    (USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)
  #define DBGUART_SENDINT()           (USART_GetITStatus(UART5, USART_IT_TXE) != RESET)
  #define DBGUART_SENDREGNOTEMPTY()   (USART_GetITStatus(UART5, USART_IT_TC) != RESET)
  #define INTUART_SENDREGNOTEMPTY()   (USART_GetITStatus(USART2, USART_IT_TC) != RESET)

#endif

#define DIROUTPUT 1
#define DIRINPUT  2

#define INT_DISABLE_ALL()
#define INT_ENABLE_ALL()




/*******************************************************************************
*** data type definitions
*******************************************************************************/

/*******************************************************************************
*** external data (export)
*******************************************************************************/

#else
  #error "Double include file !"
#endif 


