#ifndef _FRAM_H_
/*******************************************************************************
* Copyright (c) 1999/2000 by Dresser. All rights reserved.
* No part of this document or its associated software may be reproduced, transcribed,
* stored in a retrieval system, translated into any language or computer language,
* or transmitted in any form whatsoever without the prior written consent.
********************************************************************************
*
* Filename	 : fram.h
*******************************************************************************/
#define _FRAM_H_
#include "errcodes.h"
extern void fram_Initialize(void);

extern ErrorCode_t fram_ReadExt(u8 *data, u32 addr, u32 length, u8_least unit);
extern ErrorCode_t fram_WriteExt(const u8 *data, u32 addr, u32 length, u8_least unit);

extern u16          fram_GetRemapOffset(u16 AreaSize);
MN_DECLARE_API_FUNC(fram_GetRemapOffset)

//forward compatibility
MN_INLINE ErrorCode_t fram_Read(void *data, u32 addr, u32 length)
{
    return fram_ReadExt(data, addr, length, 0); //main board FRAM
}
MN_INLINE ErrorCode_t fram_Write(const void *data, u32 addr, u32 length)
{
    return fram_WriteExt(data, addr, length, 0); //main board FRAM
}

#endif //_FRAM_H_
