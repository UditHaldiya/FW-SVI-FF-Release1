/**
Copyright 2004-2005 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file sernumchip.c
    \brief BIOS routines for Serial Number chip (DS2401 type).

    CPU: Any (with standard C compiler)

    OWNER: AP
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/framework/bios/project_LCX/sernumchip.c $
    $Date: 11/04/11 4:00p $
    $Revision: 16 $
    $Author: Arkkhasin $
*/

/* (Optional) $History: sernumchip.c $
 * 
 * *****************  Version 16  *****************
 * User: Arkkhasin    Date: 11/04/11   Time: 4:00p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/framework/bios/project_LCX
 * TFS:8140 OS PORT upgrade
 * Headers fixed
 *
 * *****************  Version 14  *****************
 * User: Arkkhasin    Date: 1/28/11    Time: 1:41p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * Undo TFS:5505
 *
 * *****************  Version 11  *****************
 * User: Anatoly Podpaly Date: 11/01/10   Time: 5:41p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * Bug 4493 - unused header timebase.h removed.
 *
 * *****************  Version 10  *****************
 * User: Anatoly Podpaly Date: 10/25/10   Time: 7:10p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * Bug 4183 - fixed LINT.
 *
 * *****************  Version 9  *****************
 * User: Anatoly Podpaly Date: 10/25/10   Time: 6:57p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * Bug 4183 - converted SN code to use LOW Level ASM OW code
 *
 * *****************  Version 8  *****************
 * User: Anatoly Podpaly Date: 8/17/10    Time: 11:41a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * Bug 3839 - the Serial Driver saves the opriginal VPB divider, sets the
 * Normal speed divider, executes, than restores the original VPB
 * dvivider, so it can be executed anywhere in the code sequence.
 *
 * *****************  Version 7  *****************
 * User: Anatoly Podpaly Date: 7/30/10    Time: 3:28p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * Fixed LINT issue: unused #define.
 *
 * *****************  Version 6  *****************
 * User: Anatoly Podpaly Date: 7/30/10    Time: 1:50p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * Bug 3547: Return the whole data block.
 *
 * *****************  Version 5  *****************
 * User: Anatoly Podpaly Date: 6/29/10    Time: 3:45p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * 3404 - Validate CRC first, then compare the family code.
 *
 * *****************  Version 4  *****************
 * User: Anatoly Podpaly Date: 6/25/10    Time: 6:32p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * Bug 3396: improved readability via MACROs, retested the delays for safe
 * range (Delay A).
 *
 * *****************  Version 3  *****************
 * User: Anatoly Podpaly Date: 6/17/10    Time: 5:49p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * Added additional requirements.
 *
 * *****************  Version 2  *****************
 * User: Anatoly Podpaly Date: 6/17/10    Time: 3:47p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * Added req numbers.
 *
 * *****************  Version 1  *****************
 * User: Anatoly Podpaly Date: 6/16/10    Time: 3:38p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * Moved from HAL\BIOS\SRC
 *
 * *****************  Version 2  *****************
 * User: Anatoly Podpaly Date: 6/16/10    Time: 2:40p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * Moved GPIO definitions to BIOS_DEF.h
 *
 * *****************  Version 1  *****************
 * User: Anatoly Podpaly Date: 6/15/10    Time: 2:36p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * Onboard Serialization Support: header file: Driver code.
   *
   */

//--------------------------------------------------------------------------------------------
// This file contains the implementation of the following requirements:
// REQ 37-001; REQ 37-002; REQ 37-003; REQ 37-004; REQ 37-005; REQ 37-006; REQ 37-007

#include "mnwrap.h"
#include "oswrap.h"
#include "mnassert.h"
#include "bios_def.h"
#include "lpc2114io.h"
#include "inituc.h"
#include "sernumchip.h"
#include "snchippriv.h"

//--------------------------------------------------------------------------------------------
/** REQ 37-004 */
//-----------------------------------------------------------------------------
// 8-bit CRC calculation for Maxim devices - adapted from MAXIM Public Software Domain kit
// This is used for calculating of the 8-bit CRC
static const u8 dscrc_table[] =
{
        0u, 94u,188u,226u, 97u, 63u,221u,131u,194u,156u,126u, 32u,163u,253u, 31u, 65u,
      157u,195u, 33u,127u,252u,162u, 64u, 30u, 95u,  1u,227u,189u, 62u, 96u,130u,220u,
       35u,125u,159u,193u, 66u, 28u,254u,160u,225u,191u, 93u,  3u,128u,222u, 60u, 98u,
      190u,224u,  2u, 92u,223u,129u, 99u, 61u,124u, 34u,192u,158u, 29u, 67u,161u,255u,
       70u, 24u,250u,164u, 39u,121u,155u,197u,132u,218u, 56u,102u,229u,187u, 89u,  7u,
      219u,133u,103u, 57u,186u,228u,  6u, 88u, 25u, 71u,165u,251u,120u, 38u,196u,154u,
      101u, 59u,217u,135u,  4u, 90u,184u,230u,167u,249u, 27u, 69u,198u,152u,122u, 36u,
      248u,166u, 68u, 26u,153u,199u, 37u,123u, 58u,100u,134u,216u, 91u,  5u,231u,185u,
      140u,210u, 48u,110u,237u,179u, 81u, 15u, 78u, 16u,242u,172u, 47u,113u,147u,205u,
       17u, 79u,173u,243u,112u, 46u,204u,146u,211u,141u,111u, 49u,178u,236u, 14u, 80u,
      175u,241u, 19u, 77u,206u,144u,114u, 44u,109u, 51u,209u,143u, 12u, 82u,176u,238u,
       50u,108u,142u,208u, 83u, 13u,239u,177u,240u,174u, 76u, 18u,145u,207u, 45u,115u,
      202u,148u,118u, 40u,171u,245u, 23u, 73u,  8u, 86u,180u,234u,105u, 55u,213u,139u,
       87u,  9u,235u,181u, 54u,104u,138u,212u,149u,203u, 41u,119u,244u,170u, 72u, 22u,
      233u,183u, 85u, 11u,136u,214u, 52u,106u, 43u,117u,151u,201u, 74u, 20u,246u,168u,
      116u, 42u,200u,150u, 21u, 75u,169u,247u,182u,232u, 10u, 84u,215u,137u,107u, 53u
};

//--------------------------------------------------------------------------
/** \brief CRC Support routine
    Update the Dallas Semiconductor One Wire CRC (utilcrc8) from the passed
    variable utilcrc8 and the byte to be added to CRC

  \param[in] u8     - CRCbyte   - Partial CRC
  \param[in] u8     - Databyte  - Byte to add to CRC
  \return u8        - Partial updated CRC
*/
/** REQ 37-004 */
static u8  OW_AddCRC8(u8 CRCbyte, u8 Databyte)
{
    CRCbyte = dscrc_table[CRCbyte ^ Databyte];
    return CRCbyte;
}

//-----------------------------------------------------------------------------
// The proto's for Low Level Functions.
// The functions are implemented in aseembly, and located in the file snchip_a.s79.

/** \brief 1-Wire protocl support routine
    Write Bit 0 over the OW
  \param[in] volatile u32     *ptrToTimer  - ptr to Timer
  \param[in] volatile u32     *ptrToData   - ptr to Base of GPIO port
  \param[in] u32               OWPinMask   - OW Pin Mask
*/
//lint -esym(526,LOWLevel_OW_WriteBit_0)  Function is in snchip_a.s79
extern void LOWLevel_OW_WriteBit_0(volatile u32     *ptrToTimer,
                                   volatile u32     *ptrToData,
                                   u32               OWPinMask);

/** \brief 1-Wire protocl support routine
    Write Bit 1 over the OW
  \param[in] volatile u32     *ptrToTimer  - ptr to Timer
  \param[in] volatile u32     *ptrToData   - ptr to Base of GPIO port
  \param[in] u32               OWPinMask   - OW Pin Mask
*/
//lint -esym(526,LOWLevel_OW_WriteBit_1)  Function is in snchip_a.s79
extern void LOWLevel_OW_WriteBit_1(volatile u32     *ptrToTimer,
                                   volatile u32     *ptrToData,
                                   u32               OWPinMask);

/** \brief 1-Wire protocl support routine
    Read 1 bit from OW interface
  \param[in] volatile u32     *ptrToTimer  - ptr to Timer
  \param[in] volatile u32     *ptrToData   - ptr to Base of GPIO port
  \param[in] u32               OWPinMask   - OW Pin Mask
  \return u32 IOPortData                   - 32-bit data read from the port (with the IOpin in the value)
*/
//lint -esym(526,LOWLevel_OW_ReadBit)  Function is in snchip_a.s79
extern u32 LOWLevel_OW_ReadBit(volatile u32         *ptrToTimer,
                               volatile u32         *ptrToData,
                               u32                   OWPinMask);

/** \brief 1-Wire protocl support routine
    Iniatilize 1-wire interface and check the device presense
  \param[in] volatile u32     *ptrToTimer  - ptr to Timer
  \param[in] volatile u32     *ptrToData   - ptr to Base of GPIO port
  \param[in] u32               OWPinMask   - OW Pin Mask
  \return u32 IOPortData                   - 32-bit data read from the port (with the IOpin in the value)
*/
//lint -esym(526,LOWLevel_OW_Init)  Function is in snchip_a.s79
extern u32 LOWLevel_OW_Init(volatile u32         *ptrToTimer,
                            volatile u32         *ptrToData,
                            u32                   OWPinMask);

//-----------------------------------------------------------------------------
/** \brief 1-Wire protocl support routine
    Iniatilize 1-wire interface and check the device presense

  \return bool_t    true - device present, false - no device
*/
/** REQ 37-004 */
static bool_t   OW_Initialize(void)
{
    u32              IOPort;

    // Timing in this routine is adjusted for very LOW execution speed
    MN_ENTER_CRITICAL();
        IOPort = LOWLevel_OW_Init(&T1->TC,
                                  &GPIO0->IOPIN,
                                  OW_PIN_MASK);
    MN_EXIT_CRITICAL();

    // If bus is 0 - device present
    return ((IOPort & OW_PIN_MASK) == 0u) ? true : false;
}

/** \brief 1-Wire protocl support routine
    Read a single bit from 1-wire interface

  \return u8        - Byte that was read from the 1-wire device
*/
static u8 OW_ReadBit(void)
{
    u32              IOPort;

    // Bit patterns and Port pointers - optimization for LOW execution speed.

    //-------------------------------------------------------
    // Read bit from 1-Wire bus
    // Timing in this routine is adjusted for very LOW execution speed
    MN_ENTER_CRITICAL();
        IOPort = LOWLevel_OW_ReadBit(&T1->TC,
                                     &GPIO0->IOPIN,
                                     OW_PIN_MASK);
    MN_EXIT_CRITICAL();

    return ((IOPort & OW_PIN_MASK) != 0u) ? 1u : 0;
}

/** \brief 1-Wire protocl support routine
    Read 1 byte from the 1-wire device

  \return u8        - Byte that has been read from 1-wire device
*/
static u8 OW_ReadByte(void)
{
    u8   retval;
    u8   Index;

    Index  = 0u;
    retval = 0u;

    while (Index < U8_BITS)
    {   // LSB first
        retval >>= 1u;

        // Place the bit into the byte
        if (OW_ReadBit() == 1u)
        {
            retval |= 0x80;
        }

        Index++;
    }

    return retval;
}

/** \brief 1-Wire protocl support routine
    Write 1 bit (1 or 0) to teh 1-wire device

  \param[in] u8     - Bit value - 0 or 1.
*/
static void OW_WriteBit(u8 Bit)
{
    //-------------------------------------------------------
    // Write a bit to 1-Wire bus
    // Timing in this routine is adjusted for very LOW execution speed
    if (Bit == 1u)
    {   // Write bit 1
        MN_ENTER_CRITICAL();
            LOWLevel_OW_WriteBit_1(&T1->TC,
                                   &GPIO0->IOPIN,
                                   OW_PIN_MASK);
        MN_EXIT_CRITICAL();
    }
    else
    {   // Write bit 0
        MN_ENTER_CRITICAL();
            LOWLevel_OW_WriteBit_0(&T1->TC,
                                   &GPIO0->IOPIN,
                                   OW_PIN_MASK);
        MN_EXIT_CRITICAL();

    }
    // Bit is written
    //-------------------------------------------------------
}

/** \brief 1-Wire protocl support routine
    Write 1 byte to 1-wire device

  \param[in] u8     - Byte to be written to the device
*/
static void OW_WriteByte(u8 Data)
{
    u8   Index;

    Index = 0u;

    while (Index < U8_BITS)
    {   // LSB First!
        OW_WriteBit(Data & 1u);

        // Next bit
        Data >>= 1u;
        Index++;
    }
}

/** \brief Onboard Serialization code
    This function reads the Serial Number from teh 1-wire device.

    The function performs the following:
    - initialiazes the 1-wire device and verifies the presense
    - If the device is present, the code sends the Read ROM command and reads the content of the devices ROM
    - The code validates the Device Family Code and computed and stored 8-bit CRCs.
    If the data are valid, the function returns true and 6 bytes of the Serial Number in 64-bit value.
    If the data are NOT valid, the function returns FALSE and 0 as a Serial Number in 64-bit value.

  \param[in] u64    *ptrSerialNumber - pointer to the 64-bit Serial Numbercontainer
  \return bool_t
*/
/** \brief Read Serialization chip
  \return bool_t    - True - Success, false - not so
*/

static u64  LocalSerialNumber;

/** REQ 37-004 */
/** REQ 37-005 */
SerialNumberError_t bios_ReadSerialization(u64 *ptrSerialNumber)
{
    SerialNumberError_t  retval;
    u8      DataBuffer[SERIALNUM_DATA_TOTAL_SIZE];
    u8      CalculatedCRC;
    u8      TempByte;
    u8      Index;
    bool_t  VPBDivider;

    // Check and Clear the receiver
    MN_ASSERT(ptrSerialNumber != NULL);

    // static void bsp_InitTimer1()
    T1->TCR   = 0;            // Disable timer 0.
    T1->PC    = 0;            // Prescaler is set to no division.
    T1->MCR   = 0;            // Reset and interrupt on MR0 (match register 0).
    T1->CCR   = 0;            // Capture is disabled.
    T1->TCR   = 1;            // Enable timer 0.

    MN_ENTER_CRITICAL();
        // Save the current VPB divider
        VPBDivider = bios_GetVPBDividier_to2();
        // Set the Div / 2 divider
        bios_SetVPBDividier_to2(true);
    MN_EXIT_CRITICAL();

    //----------------------------------------------------------------
    // Here we read DS2401 and we get the following information:
    //
    // Init Interface
    //  -- Reset
    //  -- Read Presense Pulse
    //
    // Read ROM
    //  -- Send Read ROM Command (0x33 or 0x0F)
    //  -- Read 8 bytes (64 bits) ROM
    //
    // ROM Format ROM
    //  [0]         - Family Code   = 0x01
    //  [1] .. [6]  - SN #
    //  [7]         - CRC
    //
    // Calculate CRC...
    //
    // IF (Family code is right AND the CRC Matched) THEN
    //    Fill the return buffer with 6 bytes of SN#
    //    Return TRUE
    // ELSE
    //    Return FALSE
    //
    //  u64 SerialNumber will have the serial number in the following format
    //   b7    b6  b5   b4   b3   b2   b1   b0
    //  0x00 0x00 SN#5 SN#4 SN#3 SN#2 SN#1 SN#0
    //----------------------------------------------------------------

    LocalSerialNumber = (u64)0;                         // Clear the variable - buffer

    // Init to NO DEVICE - about to check for the device presense
    /** REQ 37-007 */
    retval = SERIALNUMBER_NODEVICE;

    // Init
    if (OW_Initialize())
    {   // Device is present
        // Prepare the CRC byte
        CalculatedCRC = 0u;

        // Read ROM
        OW_WriteByte(SERIALNUM_READ_ROM_CMD);

        for (Index = 0; Index < SERIALNUM_DATA_TOTAL_SIZE; Index++)
        {
            TempByte          = OW_ReadByte();
            DataBuffer[Index] = TempByte;

            if (Index < SERIALNUM_CRC_INDEX)
            {   // Checksum the bytes, except the CRC itself
                CalculatedCRC = OW_AddCRC8(CalculatedCRC, TempByte);
            }
        }

        // Validate the data
        /** REQ 37-007 */
        retval = SERIALNUMBER_BADCRC;

        if (DataBuffer[SERIALNUM_CRC_INDEX] == CalculatedCRC)
        {   // Data valid (CRC validation) - everything is OK

            // Check if the device is correct
            /** REQ 37-007 */
            retval = SERIALNUMBER_WRONGDEVICE;

            if (DataBuffer[SERIALNUM_FAMILY_INDEX] == SERIALNUM_FAMILY_CODE)
            {   // Correct Family code

                /** REQ 37-007 */
                retval = SERIALNUMBER_OK;

                // Copy the whole info starting from F/C (LO byte)
                // The retusn structurte will be
                // {FC SN0 SN1 SN2 SN3 SN4 SN5 CRC}
                Index = 0;
                while (Index < SERIALNUM_DATA_TOTAL_SIZE)
                {   // Copy the SN# info to the return buffer
                    LocalSerialNumber <<= 8u;
                    LocalSerialNumber  |= DataBuffer[Index];
                    Index++;
                }
            }
        }
    }

    *ptrSerialNumber = LocalSerialNumber;

    MN_ENTER_CRITICAL();
        // Restore the VPB divider
        bios_SetVPBDividier_to2(VPBDivider);
    MN_EXIT_CRITICAL();

#if !USE_INTERNAL_WATCHDOG
    bios_InitTimer1();
#endif

    return retval;
}

/* End of the source */
//--------------------------------------------------------------------------------------------
