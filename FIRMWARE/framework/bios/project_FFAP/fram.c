/**

Copyright 2011 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file stm_fram.c

    \brief Driver for the FRAM (non-volatile) Handles reads/writes using
    I2C and DMA.

    OWNER:
    CPU: STM32F10xx Cortex

    $Revision:  $
*/
#include "mnwrap.h" //for intrinsic functions used by the OS port
#include "oswrap.h"
#include "mnassert.h"
#include "errcodes.h" //to map error codes returned by functions
#include "i2cpriv.h"
#include "resource.h"
#include "fram.h"
#include "gpio.h"

#include MEMMAP_H_

// Do not support the FRAM partitions
// #define FRAM_SUPPORT_PARTITION_REMAP        (1u)

#ifdef FRAM_SUPPORT_PARTITION_REMAP
#include "mn_flash.h"
#endif

#define  FRAM_SIZE ((NVMEMEND - NVMEMSTART) + 1) /* size of FM24CL64 in bytes is 8192 but the end is taken by assert records */

static u8 device;

// Re-entrancy protection
static bool_t   FRAM_Available  = true;

#define NV_ADDRESS      ((u8)0xa0)
#define IWRITE          ((u8)0)
#define IREAD           ((u8)1)
#define MAIN_BOARD_FRAM (0u)

/** \brief perform an FRAM write operation

    \param[in] address - FRAM address
    \param length = length of the transfer - for range check
    \param[in] unit - device selector 0 = mainboard, 1 = sensor board
*/
static ErrorCode_t FRAM_Address(u32 address, u32 length, u8_least unit)
{
    ErrorCode_t ret = ERR_NVMEM_RANGE;

    device = NV_ADDRESS | (u8)(unit << 1);

    address &= ((u32)FRAM_SIZE - 1u);
    if ((address + length) <= (u32)FRAM_SIZE)
    {
        ret = I2C_Start(device | IWRITE);
        if (ret == ERR_OK)                              // acquire bus, send slave address
        {
            I2C_PutByte(((u8)(address >> 8)));    // FRAM addr MSB
            I2C_PutByte((u8)(address));           //  "        LSB
        }
    }
    return ret;
}

/** \brief perform an FRAM write operation

  \param[in] *data - pointer to the write buffer
  \param[in] addr - FRAM address
  \param[in] length - length of the data block
  \param[in] unit - device selector 0 = mainboard, 1 = sensor board
  \return ERR_OK if operation successful, otherwise error code
*/
ErrorCode_t fram_WriteExt(const u8 *data, u32 addr, u32 length, u8_least unit)
{
    ErrorCode_t ret;

    MN_ENTER_CRITICAL();
        MN_ASSERT(FRAM_Available);          // Make sure it is not re-entry
        FRAM_Available = false;
    MN_EXIT_CRITICAL();

    fram_IsOKtoAccess();

#if (I2C_DIAGNOSTIC_STATE == I2C_DIAGNOSTIC_ON)
    I2C_WriteDiagnostic();
#endif

    if (unit == MAIN_BOARD_FRAM)
    {
        FRAM_Write_Enable();                    // Write-enable to device
    }
    ret = FRAM_Address(addr, length, unit);     // transfer command and FRAM address to device
    if (ret == ERR_OK)
    {
        // Note the TransferOut does not internally invoke the STop
        I2C_TransferOut(data, length);
        ret = I2C_Stop();
    }
    else
    {
        (void)I2C_Stop();
    }
    FRAM_Write_Disable();               // Write-disable device

    FRAM_Available = true;
    return ret;                         // return result
}

/** \brief perform an FRAM read operation

  \param[in] *data - pointer to the read buffer
  \param[in] addr - FRAM address
  \param[in] length - length of the data block
  \param[in] unit - device selector 0 = mainboard, 1 = sensor board
  \return ERR_OK if operation successful, otherwise error code
*/
ErrorCode_t fram_ReadExt(u8 *data, u32 addr, u32 length, u8_least unit)
{
    ErrorCode_t ret;

    MN_ENTER_CRITICAL();
        MN_ASSERT(FRAM_Available);          // Make sure it is not re-entry
        FRAM_Available = false;
    MN_EXIT_CRITICAL();

    fram_IsOKtoAccess();

#if (I2C_DIAGNOSTIC_STATE == I2C_DIAGNOSTIC_ON)
    I2C_ReadDiagnostic();
#endif

    ret = FRAM_Address(addr, length, unit); // transfer command and FRAM address to device
    if (ret == ERR_OK)
    {
        I2C_TransferIn(data, length, (u8)(device | IREAD));
    }
    else
    {
        // Note the TransferIn does internally invoke the Stop, thus assymmetry
        (void)I2C_Stop();   // Keep the previous error code, in case Stop succeeds
    }

    FRAM_Available = true;
    return ret;                             // return result
}

/** \brief Get the remapping offset
  \param[in] AreaSize - FRAM area size to generate the re-map offset
  \return Offset to be used with re-map
*/
u16     fram_GetRemapOffset(u16 AreaSize)
{
    u32 Offset;

#ifdef FRAM_SUPPORT_PARTITION_REMAP
    Offset = AreaSize * flash_GetRunningPartitionID();
#else
    UNUSED_OK(AreaSize);
    Offset = 0u;
#endif

    MN_DBG_ASSERT(Offset == ((u16)Offset));

    return (u16)Offset;
}

/** \brief Performany FRAM specific initialization - at present there is none
*/
void fram_Initialize(void)
{
    // nothing to do here
}

/* End of source */
