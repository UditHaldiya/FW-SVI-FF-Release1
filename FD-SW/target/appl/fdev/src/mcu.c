/*
Copyright 2013 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file mcu.c. Controller specifics
    \brief Accesses to CPU registers. Here used for time measurements.

*/
/********************************************************************************
* Project    :  Rapid Prototype Test
* 
* Platform   :  STM32F101ZGT6
*
* Class/Type :  interface-portability
*
* Compiler   :  IAR C/C++ Compiler for ARM
*
* Date       :  2011/10
* Author     :  C.Heil, Mesco Engineering GmbH
*
*******************************************************************************/
/*
\verbatim
********************************************************************************
*  Changes
********************************************************************************
* Date        Sgn   Ver.  Changes
* 2011-10-10  ch    0.01  created
* 2013-03-20  ch    0.02  removed unused code
**************************************************************** \endverbatim */

/********************************************************************************
***   Import
*******************************************************************************/

/********************************************************************************
***   Export
*******************************************************************************/
#include "mcu.h"

/*******************************************************************************
*** internal defines
*******************************************************************************/

/*******************************************************************************
*** internal types
*******************************************************************************/

/*******************************************************************************
*** internal functions
*******************************************************************************/

/*******************************************************************************
*** internal data
*******************************************************************************/

/*******************************************************************************
*** Implementation
*******************************************************************************/


/******************************************************************************/
/*! \brief   initializes the mcu
*   \return  void
*
*   initializes the mcu
*
*******************************************************************************/
void init_mcu(void)
{
#ifndef _WIN32
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5  ; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init( GPIOE, &GPIO_InitStructure);

#endif
}



