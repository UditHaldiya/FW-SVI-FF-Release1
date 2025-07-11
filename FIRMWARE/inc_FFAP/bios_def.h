#ifndef _BIOS_DEF_H
/*******************************************************************************
* Copyright (c) 1999/2000 by Dresser. All rights reserved.
* No part of this document or its associated software may be reproduced, transcribed,
* stored in a retrieval system, translated into any language or computer language,
* or transmitted in any form whatsoever without the prior written consent.
********************************************************************************
* Modulheader    : constant definitions
*
* Filename   : bios_def.h
* Storage    : \dresser.181\vc_mncb1.01\fw\work\driver\inc\
*
* Project    : 18101
*
* Class/Type     : driver
*
* Date       : 29.06.04, 12:42:23
*
* Version    : 0.01
*
* Author     : M.Rimkus, Mesco Engineering GmbH
*
* Comment    : constants to set or reset microcontroller I/O pins.
*
*******************************************************************************/

 /*******************************************************************************
*   Changes
********************************************************************************
* Date                 SgnVer.  Changes
* 29.06.04    MR    0.01       Erstellt
* 29.11.04   MR 0.1     module checks
*******************************************************************************/

/*******************************************************************************
*** local definitions
*******************************************************************************/
#define _BIOS_DEF_H
/*******************************************************************************
*** data type definitions
*******************************************************************************/

/* bitmasks for I/O port access */
/* there is a seperate port register for Bitset and Bitclear */

#define SHIFT4 (4)

#define U32_NULL ((u32)0)

//--------------------------------------------------------------------
// defs are stored in NVRAM, by codes in refvoltage.c
#if 0

// This should be used for PROTO boards!
// The value of reference voltage for SVi1000 has been changed:
// V ref = 1.00 V [AD_DIAG}
// Use the original definition of VREF to genertate the correct value.
#define NOMINAL_VREF    ((32768.0 * 4.0) / 5.0)


// Note: this should be used for CONCEPT boards only!
// defines for checking the 1.25 reference voltage (AD_DIAG)
#define NOMINAL_VREF    (32768.0)


//--------------------------------------------------------------------

#define VREF_PERCENT    (5.0 / 100.0)
#define VREF_DEV        ((u32)(NOMINAL_VREF * VREF_PERCENT))
#define VREF_LOW        ((u32)NOMINAL_VREF - VREF_DEV)
//#define VREF_HIGH     ((u32)NOMINAL_VREF + VREF_DEV)
//--------------------------------------------------------------------

#else
#endif
#else
  /* no error */
#endif
