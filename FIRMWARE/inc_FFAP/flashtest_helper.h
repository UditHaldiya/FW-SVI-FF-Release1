#ifndef _FLASHTEST_HELPER_H
/*******************************************************************************
* Copyright (c) 2013 by GE Oil and Gas (Masoneilan). All rights reserved.
* No part of this document or its associated software may be reproduced, transcribed,
* stored in a retrieval system, translated into any language or computer language,
* or transmitted in any form whatsoever without the prior written consent.
********************************************************************************
* Modulheader    : constant definitions
*
* Filename   : flashtest_helper.h
* Date       : 08/14/2013
*
* Author     : AP
* Comment    : Contains the definition of the uC FLASH "unit"
*
*******************************************************************************/

#define _FLASHTEST_HELPER_H

// Definition of the ROM start for teh STM project
#include "stmcommon.h"
#define FLASHTEST_START HARDWARE(u32, __vector_table)

// For the LCX / AP or any other LPC - based project:
// #include MEMMAP_H_
// #define FLASHTEST_START ROMSTART

#else       // Not _FLASHTEST_HELPER_H


#endif      // _FLASHTEST_HELPER_H
