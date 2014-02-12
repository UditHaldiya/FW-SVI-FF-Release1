/*******************************************************************************
* Copyright (c) 1999/2000 by Dresser. All rights reserved.
* No part of this document or its associated software may be reproduced, transcribed,
* stored in a retrieval system, translated into any language or computer language,
* or transmitted in any form whatsoever without the prior written consent.
*/
/**

    \file fram.c

    \brief Driver for the FRAM (non-volatile) Handles reads/writes using
    I2C.

    OWNER:
    CPU: LPC21x4

*/

#include "mnwrap.h" //for intrinsic functions used by the OS port
#include "bios_def.h"
#include "mnassert.h"
#include "fram.h"
#include "lpc2114io.h"
#include "errcodes.h"

#define TESTING_I2C     0
#if TESTING_I2C
# undef   MN_INLINE
# define  MN_INLINE static
# undef   PRIVATE
# define  PRIVATE static
# include "i2cpriv.h"
# include "i2c.c"
#else
# include "i2cpriv.h"
#endif

#include MEMMAP_H_

#define MAIN_BOARD_FRAM     0

// #define SENSOR_BOARD_FRAM   1

#define  FRAM_SIZE ((NVMEMEND - NVMEMSTART) + 1) /* size of FM24CL64 in bytes is 8192 but the end is taken by assert records */

#define NV_ADDRESS  (0x50u << 1)    //!< base address for main board FRAM

static u8 device;                   //!< computed i2c slave address

static ErrorCode_t FRAM_Address(u32 address, u32 length, u8_least unit);

static bool_t fram_IsOKtoAccess(void)
{
    return true;
}

//----------------------------------------------------------------------------------------------------------------------

static  void    FRAM_Write_Enable(void)
{
    GPIO1->IOCLR = GPIO1_FRAM_WP;
}

static  void    FRAM_Write_Disable(void)
{
    GPIO1->IOSET = GPIO1_FRAM_WP;
}

/** \brief Get the remapping offset
  \param[in] AreaSize - FRAM area size to generate the re-map offset
  \return Offset to be used with re-map
*/
u16     fram_GetRemapOffset(u16 AreaSize)
{
    UNUSED_OK(AreaSize);
    return 0u;
}

//---------------------------------------------------------------

/** \brief intialize the FRAM driver
*/

void bios_InitFram()
{
    fram_Initialize();
}

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
        I2C_Enable();                                   // set SDA and SCL to I2C mode
        ret = I2C_Start(device | IWRITE);
        if (ret == ERR_OK)                              // acquire bus, send slave address
        {
            (void)I2C_PutByte(((u8)(address >> 8)));    // FRAM addr MSB
            (void)I2C_PutByte((u8)(address));           //  "        LSB
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

    (void)fram_IsOKtoAccess();

    if (unit == MAIN_BOARD_FRAM)
    {
        FRAM_Write_Enable();           // Write-enable to device
    }

    ret = FRAM_Address(addr, length, unit); // transfer command and FRAM address to device
    if (ret == ERR_OK)
    {
        while (length-- != 0u)
        {
            (void)I2C_PutByte(*data++); // transfer write data
        }
    }
    I2C_Disable();                      // I2C stop

    if (unit == MAIN_BOARD_FRAM)
    {
        FRAM_Write_Disable();      // Set write protection back if needed.
    }

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
    u8 *p1 = data;
    ErrorCode_t ret;

    (void)fram_IsOKtoAccess();

    ret = FRAM_Address(addr, length, unit); // transfer command and FRAM address to device
    if (ret == ERR_OK)
    {
        I2C_Restart((u8)(device | IREAD));  // flip bus mode write to read
        I2C_SetAck();                       // acknowledge all but last byte
        while (length > 1)
        {
            *p1++ = I2C_GetByte();          // transfer read data
            length--;                       // count
        }

        I2C_ResetAck();                     // signal transfer termination
        *p1 = I2C_GetByte();                // transfer last byte

    }
    I2C_Disable();                          // I2C stop
    return ret;                             // return result
}

/** \brief Performany FRAM specific initialization - at present there is none
*/
void fram_Initialize(void)
{
    // nothing to do here
}

/* End of source */
