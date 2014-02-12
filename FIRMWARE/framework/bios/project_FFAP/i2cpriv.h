/**
Copyright 2012 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file i2cpriv.h

    \brief Header for FF version of I2C

    OWNER:
    CPU: STM32F10xx Cortex

    $Revision:  $
*/
#ifndef I2C_PRIV_H_
#define I2C_PRIV_H_

#define I2C_DIAGNOSTIC_OFF          (0u)
#define I2C_DIAGNOSTIC_ON           (1u)

#define I2C_DIAGNOSTIC_STATE        (I2C_DIAGNOSTIC_ON)

extern ErrorCode_t  I2C_Stop (void);
extern void         I2C_PutByte(u8 byte);
extern void         I2C_TransferIn(u8 *data, u32 length, u8 addr);
extern ErrorCode_t  I2C_Start(u8 slaveaddr);
extern void         I2C_TransferOut(const u8 *data, u32 length);

CONST_ASSERT((I2C_DIAGNOSTIC_STATE == I2C_DIAGNOSTIC_OFF) || (I2C_DIAGNOSTIC_STATE == I2C_DIAGNOSTIC_ON));

#if (I2C_DIAGNOSTIC_STATE == I2C_DIAGNOSTIC_ON)

extern void         I2C_WriteDiagnostic(void);
extern void         I2C_ReadDiagnostic(void);

#endif

#endif

// end of source
