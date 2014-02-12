/*******************************************************************************
* Copyright (c) 1999/2000 by Dresser. All rights reserved.
* No part of this document or its associated software may be reproduced, transcribed,
* stored in a retrieval system, translated into any language or computer language,
* or transmitted in any form whatsoever without the prior written consent.
********************************************************************************
* Modul      :	FRAM driver
*
* Filename   :	FRAM_H.c
* Storage    :	\dresser.181\vc_mncb1.01\fw\work\driver\src\
*
* Project    :	18101
*
* Platform   :	LPC2114
*
* Class/Type :	driver
*
* Compiler   :	IAR C-compiler for ARM 4.10B
*
* Date       :	01.10.2004, 11:46:38
*
* Version    :	0.01

*
* Author     :	M.Rimkus, Mesco Engineering GmbH
*
* Comment    :
*
*******************************************************************************/

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

/*******************************************************************************
* FUNCTION	    : u8 FRAM_Read(const FRAMControlBlock_t * pBlock)
*
* DATE	 	    : 05.08.2004
*
* PARAMETER(IN)     : struct with information what to read from FRAM
*
* RETURN(OUT)	    :  error = FRAM_NO_ERROR, FRAM_CRC_ERROR, FRAM_RANGE_ERROR
*                              FRAM_READ_ERROR
* DESCRIPTION	    :  Reads data from FRAM to buffer.
*                      Function is reentrant if use_mutex=true (mutex semaphore).
*                      Next FRAM read, write access is blocked until mutex is released
*                      A notification is send to cyclic crc checkers, that RAM/FRAM is modified.
*                      A static notification flag is set, that FRAM is under modification.
* APPLICATION       :
*
*******************************************************************************/
ErrorCode_t bios_FRAM_Read(const FRAMControlBlock_t * pBlock)
{
    ErrorCode_t     error;

    MN_ASSERT(!(pBlock == NULL));

    error = fram_ReadExt(pBlock->Pdata,
                         pBlock->FRAMAdress,
                         pBlock->NumberOfBytes,
                         MAIN_BOARD_FRAM);

    return(error);
}

/*******************************************************************************
* FUNCTION	   : u8 FRAM_Write(const FRAMControlBlock_t * pBlock)
*
* DATE		   : 05.08.2004
*
* PARAMETER(IN)    : struct with information what to write to FRAM
*
* RETURN(OUT)	   : error = FRAM_NO_ERROR, FRAM_WRITE_ERROR, FRAM_RANGE_ERROR
*                            FRAM_WRITE_DISABLED
*
* DESCRIPTION	   : Writes data from buffer to FRAM.
*                    Function is reentrant if use_mutex=true (mutex semaphore).
*                    Next FRAM read, write access is blocked until mutex is released.
*                    A notification is send to cyclic crc checkers, that RAM/FRAM is modified.
*                    A static notification flag is set, that FRAM,RAM is under modification.
*
* APPLICATION      :
*
*******************************************************************************/
ErrorCode_t bios_FRAM_Write(const FRAMControlBlock_t * pBlock)
{
    ErrorCode_t     error;

    MN_ASSERT(!(pBlock == NULL));

    error = fram_WriteExt(pBlock->Pdata,
                          pBlock->FRAMAdress,
                          pBlock->NumberOfBytes,
                          MAIN_BOARD_FRAM);

    return(error);
}

//AK: Started here

/** Counts the total number of bytes in the buffer list.
    Unfortunately, wbuflist_t and rbuflist_t are considered different enough
    to trigger diagnostics. Yet they differ only in in the const qualifier
    of the data pointer.
    I "fix" it by making the numbytes() make assumptions

    \param buflist - a buffer list (of either wbuflist_t or rbuflist_t type)
    \return the number of byhtes in all buffers
*/
static u32 numbytes(const void *buflist)
{
    u32 ret;
    const wbuflist_t *b;
    b = buflist;
    ret = 0u;
    for( ; b != NULL; b = b->next)
    {
        ret += b->len;
    }
    return ret;
}

/*******************************************************************************
* FUNCTION	    : u8 FRAM_Read(const FRAMControlBlock_t * pBlock)
*
* DATE	 	    : 05.08.2004
*
* PARAMETER(IN)     : struct with information what to read from FRAM
*
* RETURN(OUT)	    :  error = FRAM_NO_ERROR, FRAM_CRC_ERROR, FRAM_RANGE_ERROR
*                              FRAM_READ_ERROR
* DESCRIPTION	    :  Reads data from FRAM to buffer.
*                      Function is reentrant if use_mutex=true (mutex semaphore).
*                      Next FRAM read, write access is blocked until mutex is released
*                      A notification is send to cyclic crc checkers, that RAM/FRAM is modified.
*                      A static notification flag is set, that FRAM is under modification.
* APPLICATION       :
*
*******************************************************************************/
ErrorCode_t bios_FRAM_ReadExt(const rbuflist_t *buflist, u16_least faddr)
{
    ErrorCode_t         ret;
    u8                 *PtrData;
    const rbuflist_t   *LocalBuflist;
    u16_least           BufferLength;
    u16_least           TotaldLength;

    /* Read from FRAM, if we have no error ***/
    /* Transmit FRAM slave adress and start adress in FRAM */

    TotaldLength = numbytes(buflist);

    ret = FRAM_Address(faddr, TotaldLength, MAIN_BOARD_FRAM);    // transfer command and FRAM address to device
    if (ret == ERR_OK)
    {   // Within the range

        I2C_Restart((u8)(device | IREAD));  // flip bus mode write to read
        I2C_SetAck();                       // acknowledge all but last byte

        LocalBuflist = buflist;
        while (LocalBuflist != NULL)
        {
            BufferLength = LocalBuflist->len;
            PtrData      = LocalBuflist->data;
            while (BufferLength != 0u)
            {
                if (TotaldLength == 1u)
                {   // Last Byte? - yes!
                    I2C_ResetAck();                 // signal transfer termination
                }

                *PtrData++ = I2C_GetByte();          // transfer read data
                BufferLength--;
                TotaldLength--;
            }

            // Next buffer in the list
            LocalBuflist = LocalBuflist->next;
        }

        I2C_Disable();                          // I2C stop
    }

    return ret;                             // return result
}

/*******************************************************************************
* FUNCTION	   : u8 FRAM_Write(const FRAMControlBlock_t * pBlock)
*
* DATE		   : 05.08.2004
*
* PARAMETER(IN)    : struct with information what to write to FRAM
*
* RETURN(OUT)	   : error = FRAM_NO_ERROR, FRAM_WRITE_ERROR, FRAM_RANGE_ERROR
*                            FRAM_WRITE_DISABLED
*
* DESCRIPTION	   : Writes data from buffer to FRAM.
*                    Function is reentrant if use_mutex=true (mutex semaphore).
*                    Next FRAM read, write access is blocked until mutex is released.
*                    A notification is send to cyclic crc checkers, that RAM/FRAM is modified.
*                    A static notification flag is set, that FRAM,RAM is under modification.
*
* APPLICATION      :
*
*******************************************************************************/
ErrorCode_t bios_FRAM_WriteExt(const wbuflist_t *buflist, u16_least faddr)
{
    ErrorCode_t         ret;
    const u8           *PtrData;
    const wbuflist_t   *LocalBuflist;
    u16_least           BufferLength;
    u16_least           TotalLength;

    /* Read from FRAM, if we have no error ***/
    /* Transmit FRAM slave adress and start adress in FRAM */

    TotalLength = numbytes(buflist);

    FRAM_Write_Enable();                   // Write-enable to device

    ret = FRAM_Address(faddr, TotalLength, MAIN_BOARD_FRAM);    // transfer command and FRAM address to device
    if (ret == ERR_OK)
    {   // Within the range

        LocalBuflist = buflist;
        while (LocalBuflist != NULL)
        {
            BufferLength = LocalBuflist->len;
            PtrData      = LocalBuflist->data;
            while (BufferLength != 0u)
            {
                (void)I2C_PutByte(*PtrData++); // transfer write data
                BufferLength--;
            }

            LocalBuflist = LocalBuflist->next;
        }
    }

    I2C_Disable();                          // I2C stop
    FRAM_Write_Disable();              // Write-disable device

    return ret;                             // return result
}

//---------------------------------------------------------------

/** \brief intialize the FRAM driver
*/

void fram_Initialize(void)
{
}

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

    address &= FRAM_SIZE - 1;
    if ((address + length) <= FRAM_SIZE)
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

/* End of source */
