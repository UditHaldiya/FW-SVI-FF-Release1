/*
Copyright 2006 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/** \addtogroup I2cNon-volatile FRAM device
\brief Device driver for Digital Resistor MPC 4541.
*/
/**
    \file digital_pot.c
    \brief Device driver for Digital Resistor MPC 4541.

    CPU: Philips LPC21x4 (ARM)

    OWNER: Anatoly Podpaly

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/framework/bios/project_LCX/digital_pot.c $
    $Date: 11/04/11 3:55p $
    $Revision: 9 $
    $Author: Arkkhasin $

*/

/* $History: digital_pot.c $
 * 
 * *****************  Version 9  *****************
 * User: Arkkhasin    Date: 11/04/11   Time: 3:55p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/framework/bios/project_LCX
 * TFS:8072 NVMEM upgrade 
 * Headers fixed
 *
 * *****************  Version 7  *****************
 * User: Anatoly Podpaly Date: 12/16/10   Time: 10:56a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * Bug 5067 - Removed the code to write the TCON. Removed the body of the
 * function DigitalResistor_INit.
 *
 * *****************  Version 6  *****************
 * User: Anatoly Podpaly Date: 12/15/10   Time: 2:29p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * Bug 4427 - added comment re: ignoring the error code in writing of
 * TCON.
 *
 * *****************  Version 5  *****************
 * User: Anatoly Podpaly Date: 11/01/10   Time: 6:09p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * Bug 4427 - removed unused variable and assignements.
 *
 * *****************  Version 4  *****************
 * User: Anatoly Podpaly Date: 10/20/10   Time: 6:30p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * Bug 4223 - implemented results of code review.
 *
 * -- Corrected the incorrect conditional compilation statement: if the
 * number of units is 2 will not omit definitions for the Unit 1.
 * -- Implemented the checks: the number of units is  checked for the
 * range: 1 or 2 (> 0 and <= 2).
 * -- Eliminated usage of ASSERT in Read/Write functions – instead of
 * ASSERT the functions generate and propagate the error.
 * -- WriteWiper passes the Wiper position as a value.
 * -- Re-factored code to use unified errors from errorcode.h instead of
 * device-specific errors. Added two additional error codes.
 * -- Refactored the function DR_I2C_Action using switch statement.
 * -- Added functions to  to read configuration register.
 * -- Now the code writes default value to the configuration register on
 * Power UP (Init).
 * -- Refactored Access check functions: now only one function. liminated
 * retries on device busy - now it is a function of a remote HART
 * application.
 *
 * *****************  Version 3  *****************
 * User: Justin Shriver Date: 9/16/10    Time: 10:38a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * TFS:4122 Maximum value increase to 128
 *
 * *****************  Version 2  *****************
 * User: Anatoly Podpaly Date: 8/31/10    Time: 2:59p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * Changed the name of the include file: digital_pot.h
 *
 * *****************  Version 1  *****************
 * User: Anatoly Podpaly Date: 8/31/10    Time: 2:47p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * Driver for Digital Potentiometer.
 *
 *
*/

/** brief
    This file contains the Driver for the Digital Resistor: MicroChip MPC 4541 device.
    The interface is designed to:
    -- Initioalize the driver
    -- To provide Read Wiper settings from volatile control register.
    -- To provide Read Wiper settings from NON-volatile control register.
    -- To provide Write Wiper settings to volatile control register (can be locked by WiperLock).
    -- To provide Write Wiper settings to NON-volatile control register (can be locked by WiperLock).
 */

#include "mnwrap.h" //for intrinsic functions used by the OS port
#include "oswrap.h"
#include "mnassert.h"
#include "bios_def.h"
#include "errcodes.h"
#include "lpc2114io.h"
#include "i2cpriv.h"
#include "nvram.h"
#include "digital_pot.h"
#include "resource.h"

// ------------------------------------------------------------------------------------------------------------
// This MACRO is to vprovide a mechanism of controlling the mutex. There is only one mutex - FRAM mutex, that
// more corretly should be called I2C mutex. The same mutex is used (here undera different name for clarity) since
// the purpose of the mutex is to control access of FRAM driver and DP driver to a single non-shareable H/W
// resource - I2C controller and bus.

#define     MN_I2C_DR_ACQUIRE()     MN_FRAM_ACQUIRE()
#define     MN_I2C_DR_RELEASE()     MN_FRAM_RELEASE()

// ------------------------------------------------------------------------------------------------------------
// Chip Definitions
//
// This is very specific chip specific code.
// This code is not intended to be portable and / or expandable.

// MPC4541 - contains only 1 Digital Resistor
// MPC4541 - 7-bit Resistor
#define	DIGITAL_RESISTOR_WIDTH_BITS                 (7u)
#define DIGITAL_RESISTOR_NUMBER_OF_UNITS            (1u)

// MAX Value for Wiper position
// Even though the resistor is specified as 7 bit, the 8-bit value is legitimate, so 0x80 can be written
// ANything higher - cannot. Same logic applies to 8-bit resistor: 0x100 is legitimate value.
#define	DIGITAL_RESISTOR_MAX_WIPER                  (1u << DIGITAL_RESISTOR_WIDTH_BITS)

// Note, thatthe chip has internal Pull-up on A0 pin, in the current design the pin is "floating",
// i.e. pulled up HI internally. Thus the Device address shall be adjusted accordingly:
#define DR_INTERNAL_PULLUP_ENABLED      (1u)

#ifdef  DR_INTERNAL_PULLUP_ENABLED
#define DR_A0_PULLUP_ADDRESSBIT     (1u)
#else
#define DR_A0_PULLUP_ADDRESSBIT     (0u)
#endif

// Device Address : HVC / A0 - floating, pulled down by the Weak Pull Down
#define DIGITALRESISTOR_ADDRESS     ((0x2E | DR_A0_PULLUP_ADDRESSBIT) << 1u)        //!< base address for the digirtal resistor

// ------------------------------------------------------------------------------------------------------------
// MPC 4541 Registers

#if DIGITAL_RESISTOR_NUMBER_OF_UNITS == 0u
#error - Number of resistors cannot be 0.
#endif

#if DIGITAL_RESISTOR_NUMBER_OF_UNITS > 2u
#error - Unsupported number of resistors
#endif

// We have at least one unit/resistor
#define VOLATILE_WIPER_0            ( 0u)               // Volatile Wiper 0 RAM
#define NONVOLATILE_WIPER_0         ( 2u)               // Non-Volatile Wiper 0 EEPROM

#if DIGITAL_RESISTOR_NUMBER_OF_UNITS == 2u
// 2 units
#define VOLATILE_WIPER_1            ( 1u)               // Volatile Wiper 1 RAM
#define NONVOLATILE_WIPER_1         ( 3u)               // Non-Volatile Wiper 1 EEPROM
#endif

// ------------------------------------------------------------------------------------------------------------
// Status Register Bit definitions:
//
// bits 8-4     D8:D4: Reserved. Forced to 1
//
// bit 3 EEWA:  EEPROM Write Active Status bit
//              This bit indicates if the EEPROM Write Cycle is occurring.
//              1 = An EEPROM Write cycle is currently occurring. Only serial commands to the Volatile memory
//              locations are allowed (addresses 00h, 01h, 04h, and 05h)
//              0 = An EEPROM Write cycle is NOT currently occurring
//
// bit 2 WL1:   WiperLock Status bit for Resistor Network 1 (Refer to Section 5.3 WiperLock™ Technology for further information)
//              WiperLock (WL) prevents the Volatile and Non-Volatile Wiper 1 addresses and the TCON register bits R1HW, R1A,
//              R1W, and R1B from being written to. High Voltage commands are required to enable and disable WiperLock Technology.
//              1 = Wiper and TCON register bits R1HW, R1A, R1W, and R1B of Resistor Network 1 (Pot 1) are Locked (Write Protected)
//              0 = Wiper and TCON of Resistor Network 1 (Pot 1) can be modified
//              Note: The WL1 bit always reflects the result of the last programming cycle to the non-volatile WL1 bit. After a POR or
//              BOR event, the WL1 bit is loaded with the non-volatile WL1 bit value.
//
// bit 1 WL0:   WiperLock Status bit for Resistor Network 0 (Refer to Section 5.3 WiperLock™ Technology for further information)
//              The WiperLock Technology bits (WLx) prevents the Volatile and Non-Volatile Wiper 0 addresses and the TCON register bits
//              R0HW, R0A, R0W, and R0B from being written to. High Voltage commands are required to enable and disable WiperLock Technology.
//              1 = Wiper and TCON register bits R0HW, R0A, R0W, and R0B of Resistor Network 0 (Pot 0) are Locked (Write Protected)
//              0 = Wiper and TCON of Resistor Network 0 (Pot 0) can be modified
//              Note: The WL0 bit always reflects the result of the last programming cycle to the non-volatile WL0 bit. After a POR or
//              BOR event, the WL0 bit is loaded with the non-volatile WL0 bit value.
//              Note 1: Requires a High Voltage command to modify the state of this bit (for Non-Volatile devices only). This bit is
//              Not directly written, but reflects the system state (for this feature). MCP454X/456X/464X/466X DS22107A-page 40 2008 Microchip Technology Inc.
//
// bit 0 WP:    EEPROM Write Protect Status bit (Refer to Section EEPROM Write Protect for further information)
//              This bit indicates the status of the write protection on the EEPROM memory. When Write Protect is enabled, writes to all
//              non-volatile memory are prevented. This includes the General Purpose EEPROM memory, and the non-volatile Wiper registers.
//              Write Protect does not block modification of the volatile wiper register values or the volatile TCON register value (via
//              Increment, Decrement, or Write commands). This status bit is an OR of the devices Write Protect pin (WP) and the internal
//              non-volatile WP bit. High Voltage commands are required to enable and disable the internal WP EEPROM bit.
//              1 = EEPROM memory is Write Protected
//              0 = EEPROM memory can be written
//
//  Note 1: Requires a High Voltage command to modify the state of this bit (for Non-Volatile devices only). This bit is Not directly written,
// but reflects the system state (for this feature
#define STATUS_REGISTER             ( 5u)               // Status Register RAM
#define     STATUS_BIT_EEWA         (0x8)
#define     STATUS_BIT_WL0          (0x2)
#define     STATUS_BIT_WP           (0x1)

#if DIGITAL_RESISTOR_NUMBER_OF_UNITS == 1u
#define     STATUS_MASK_PROTECT     (STATUS_BIT_WL0 | STATUS_BIT_WP)
#else   // 2 units
#define     STATUS_BIT_WL1          (0x4)
#define     STATUS_MASK_PROTECT     (STATUS_BIT_WL1 | STATUS_BIT_WL0 | STATUS_BIT_WP)
#endif

#define     EEPROM_LOCKED(StatusRegister)                           ((StatusRegister & STATUS_MASK_PROTECT) != 0u)
#define     EEPROM_CYCLE_NOT_COMPLETED(StatusRegister)              ((StatusRegister & STATUS_BIT_EEWA)     != 0u)

// ------------------------------------------------------------------------------------------------------------
// TCON Register Bit definitions
//
// bit 8 GCEN:  General Call Enable bit
//              This bit specifies if I2C General Call commands are accepted
//              1 = Enable Device to Accept the General Call Address (0000h)
//              0 = The General Call Address is disabled
//
// bit 7 R1HW:  Resistor 1 Hardware Configuration Control bit
//              This bit forces Resistor 1 into the shutdown configuration of the Hardware pin
//              1 = Resistor 1 is NOT forced to the hardware pin shutdown configuration
//              0 = Resistor 1 is forced to the hardware pin shutdown configuration
//
// bit 6 R1A:   Resistor 1 Terminal A (P1A pin) Connect Control bit
//              This bit connects/disconnects the Resistor 1 Terminal A to the Resistor 1 Network
//              1 = P1A pin is connected to the Resistor 1 Network
//              0 = P1A pin is disconnected from the Resistor 1 Network
//
// bit 5 R1W:   Resistor 1 Wiper (P1W pin) Connect Control bit
//              This bit connects/disconnects the Resistor 1 Wiper to the Resistor 1 Network
//              1 = P1W pin is connected to the Resistor 1 Network
//              0 = P1W pin is disconnected from the Resistor 1 Network
//
//bit 4 R1B:    Resistor 1 Terminal B (P1B pin) Connect Control bit
//              This bit connects/disconnects the Resistor 1 Terminal B to the Resistor 1 Network
//              1 = P1B pin is connected to the Resistor 1 Network
//              0 = P1B pin is disconnected from the Resistor 1 Network
//
// bit 3 R0HW:  Resistor 0 Hardware Configuration Control bit
//              This bit forces Resistor 0 into the shutdown configuration of the Hardware pin
//              1 = Resistor 0 is NOT forced to the hardware pin shutdown configuration
//              0 = Resistor 0 is forced to the hardware pin shutdown configuration
//
// bit 2 R0A:   Resistor 0 Terminal A (P0A pin) Connect Control bit
//              This bit connects/disconnects the Resistor 0 Terminal A to the Resistor 0 Network
//              1 = P0A pin is connected to the Resistor 0 Network
//              0 = P0A pin is disconnected from the Resistor 0 Network
//
//bit 1 R0W:    Resistor 0 Wiper (P0W pin) Connect Control bit
//              This bit connects/disconnects the Resistor 0 Wiper to the Resistor 0 Network
//              1 = P0W pin is connected to the Resistor 0 Network
//              0 = P0W pin is disconnected from the Resistor 0 Network
//
// bit 0 R0B:   Resistor 0 Terminal B (P0B pin) Connect Control bit
//              This bit connects/disconnects the Resistor 0 Terminal B to the Resistor 0 Network
//              1 = P0B pin is connected to the Resistor 0 Network
//              0 = P0B pin is disconnected from the Resistor 0 Network
//
// Note 1: These bits do not affect the wiper register values.
// Default Value for this register 0x1FF.
// TCON is protected by WiperLock

#define TCON_REGISTER               ( 4u)               // Volatile TCON Register RAM

#if 0

    #define     TCON_BIT_GCEN           (0x100)
    #define     TCON_BIT_R0HW           (0x8)
    #define     TCON_BIT_R0A            (0x4)
    #define     TCON_BIT_R0W            (0x2)
    #define     TCON_BIT_R0B            (0x1)

// This is the default value of TCON -- all the resistors enabled
// TCON is protected by WiperLock
#if DIGITAL_RESISTOR_NUMBER_OF_UNITS == 1u
    #define     TCON_INIT_VALUE         (TCON_BIT_GCEN | \
                                         TCON_BIT_R0HW | TCON_BIT_R0A | TCON_BIT_R0W | TCON_BIT_R0B)
#else
    #define     TCON_BIT_R1HW           (0x80)
    #define     TCON_BIT_R1A            (0x40)
    #define     TCON_BIT_R1W            (0x20)
    #define     TCON_BIT_R1B            (0x10)

    #define     TCON_INIT_VALUE         (TCON_BIT_GCEN | \
                                         TCON_BIT_R1HW | TCON_BIT_R1A | TCON_BIT_R1W | TCON_BIT_R1B | \
                                         TCON_BIT_R0HW | TCON_BIT_R0A | TCON_BIT_R0W | TCON_BIT_R0B)
#endif

#endif

// ------------------------------------------------------------------------------------------------------------

#if 0

// ERPROM Storage
#define EEPROM_LOCATION_0           ( 6u)               // Data EEPROM EEPROM
#define EEPROM_LOCATION_1           ( 7u)               // Data EEPROM EEPROM
#define EEPROM_LOCATION_2           ( 8u)               // Data EEPROM EEPROM
#define EEPROM_LOCATION_3           ( 9u)               // Data EEPROM EEPROM
#define EEPROM_LOCATION_4           (10u)               // Data EEPROM EEPROM
#define EEPROM_LOCATION_5           (11u)               // Data EEPROM EEPROM
#define EEPROM_LOCATION_6           (12u)               // Data EEPROM EEPROM
#define EEPROM_LOCATION_7           (13u)               // Data EEPROM EEPROM
#define EEPROM_LOCATION_8           (14u)               // Data EEPROM EEPROM
#define EEPROM_LOCATION_9           (15u)               // Data EEPROM EEPROM

#endif

// ------------------------------------------------------------------------------------------------------------

#define DR_WRITE_COMMAND            (0x0)
#define DR_READ_COMMAND             (0xC)
#define DR_INC_COMMAND              (0x4)
#define DR_DEC_COMMAND              (0x8)

#define BIT_8                       (1u << 8u)

// ------------------------------------------------------------------------------------------------------------
// I2C Actions for the chip
typedef enum
{
    DR_I2C_READ,                            // Read  data
    DR_I2C_WRITE,                           // Write data
    DR_I2C_DECREMENT,                       // Decrement
    DR_I2C_INCREMENT                        // Increment
} DR_I2C_Action_t;                          // Action Type

/** \brief Perform an I2C operation on the Digital Resistor
    This function actually performs the I2C operation

    \param DeviceAddr       - Device I2C ADdress
    \param DPRegister       - Index of the register to read or write
    \param Action           - Write or read data
    \param[out] Data - poiter to the data buffer for writing or reading
    \return ErrorCode_t  error code
*/
static ErrorCode_t  DR_I2C_Action(u8                DeviceAddr,
                                  u8                DPRegister,
                                  DR_I2C_Action_t   Action,
                                  u16              *Data)

{
    ErrorCode_t             retval;
    u16                     tempvalue;
    u8                      TempBit8;
    u8                      CmdByte;

    I2C_Enable();                                                                   // Start I2C
    retval = I2C_Start((u8)(DeviceAddr | IWRITE));                                  // Slave address
    if (retval == ERR_OK)
    {   // Bus is OK

        switch (Action)
        {
            case DR_I2C_READ :                                              // Read the device
                CmdByte = ((u8)(DPRegister << 4u)) | ((u8)DR_READ_COMMAND);         // Build RD cmd
                (void)I2C_PutByte(CmdByte);                                         // Send CMD

                I2C_Restart((u8)(DeviceAddr | IREAD));                              // Restart to Read
                I2C_SetAck();                                                       // Acknowledge first byte
                tempvalue  = I2C_GetByte() << 8;                                    // Get one
                I2C_ResetAck();                                                     // Do not acknowledge last byte
                tempvalue |= I2C_GetByte();                                         // Get the last

                *Data = tempvalue;                                                  // Get the data back to the caller
                break;

            case DR_I2C_WRITE :                                             // Write to the device
                tempvalue = *Data;                                                  // Local copy
                TempBit8  = ((tempvalue & ((u16)BIT_8)) != 0u) ? 1u : 0u;           // Separate bit 8
                CmdByte = ((u8)(DPRegister << 4u)) | ((u8)DR_WRITE_COMMAND) | TempBit8; // Build WR command
                (void)I2C_PutByte(CmdByte);                                         // Send Cmd
                (void)I2C_PutByte((u8)tempvalue);                                   // second byte
                break;

            case DR_I2C_INCREMENT :                                         // Write to the device (used as a commnad)
                CmdByte = ((u8)(DPRegister << 4u)) | ((u8)DR_INC_COMMAND);          // Build INC command
                (void)I2C_PutByte(CmdByte);                                         // Send Cmd
                break;

            case DR_I2C_DECREMENT :                                         // Write to the device
                CmdByte = ((u8)(DPRegister << 4u)) | ((u8)DR_DEC_COMMAND);          // Build DEC command
                (void)I2C_PutByte(CmdByte);                                         // Send Cmd
                break;

            default :
                break;
        }
    }

    I2C_Disable();                                                                  // Stop I2C

    return retval;
}

/** \brief DigitalResistor_ReadStatus - read Status Register
    \param[out]   Status Register   - Status Register value
    \return ErrorCode_t               Error code
*/
static ErrorCode_t  DR_ReadStatus(DR_Register_t *DR_StatusRegister)
{
    ErrorCode_t         ReturnStatus;
    DR_Register_t       RegisterValue;

    ReturnStatus = DR_I2C_Action(DIGITALRESISTOR_ADDRESS,
                                 STATUS_REGISTER,
                                 DR_I2C_READ,
                                 &RegisterValue);

    *DR_StatusRegister = RegisterValue;

    return ReturnStatus;
}

/** \brief DR_IsOKToAccessEEPROM - checks if the resistor
    EEPROM Storage location is accessible
    \param[in]   DR_WiperStorage_t      DR_WiperStorage: volatile or non-volitile
    \param[in]   DR_I2C_Action_t        Action: Read or Write
    \return ErrorCode_t                 Error code
*/
static ErrorCode_t  DR_IsOKToAccessEEPROM(DR_WiperStorage_t DR_WiperStorage,
                                          DR_I2C_Action_t   Action)
{
    ErrorCode_t             ReturnStatus;
    DR_Register_t           StatusRegister;

    //  --------------------------------------------------------
    // There are following combinations that has to be checked
    // -- Read  from RAM
    // -- Read  from ROM
    // -- Write to   RAM (includes Inc/Dec)
    // -- Write to   ROM (includes Inc/Dec)
    //
    // The code does not attempt to execute re-tries in the case
    // if the device did not finish WRite cycle. It is up to the
    // HART application to do so.
    //  --------------------------------------------------------

    if ((DR_WiperStorage == DR_WIPER_VOLATILE_STORAGE) && (Action == DR_I2C_READ))
    {   // Read RAM -- always OK
        ReturnStatus = ERR_OK;
    }
    else
    {   // The rest of Read / Write operations

        // Read Status Register
        ReturnStatus = DR_ReadStatus(&StatusRegister);
        if (ReturnStatus == ERR_OK)
        {   // I2C Operation was OK, check status register

            // Now we have the status register, and we can check
            // if it is OK to do the Action with the Storage unit

            if ((Action == DR_I2C_READ) && (DR_WiperStorage == DR_WIPER_NONVOLATILE_STORAGE))
            {   // Read ROM
                if (EEPROM_CYCLE_NOT_COMPLETED(StatusRegister))
                {   // Cannot Read ROM - write cycle did not complete
                    ReturnStatus = ERR_DP_DEVICE_BUSY;
                }
            }
            else if ((Action != DR_I2C_READ) && (DR_WiperStorage == DR_WIPER_VOLATILE_STORAGE))
            {   // WRite RAM
                if (EEPROM_LOCKED(StatusRegister))
                {   // Cannot write RAM - device Locked...
                    ReturnStatus = ERR_DP_DEVICE_LOCKED;
                }
            }
            else if ((Action != DR_I2C_READ) && (DR_WiperStorage == DR_WIPER_NONVOLATILE_STORAGE))
            {   // Write ROM
                if (EEPROM_LOCKED(StatusRegister))
                {   // Cannot write RAM - device Locked...
                    ReturnStatus = ERR_DP_DEVICE_LOCKED;
                }
                else if (EEPROM_CYCLE_NOT_COMPLETED(StatusRegister))
                {   // Cannot Write ROM - write cycle did not complete
                    ReturnStatus = ERR_DP_DEVICE_BUSY;
                }
                else
                {
                    //Nothing
                }
            }
            else
            {
                // Nothing
            }
        }
    }

    return ReturnStatus;
}

/** \brief DR_ReadConfig - read Config Register
    \param[out]   Config Register   - Config Register value
    \return ErrorCode_t               Error code
*/
static ErrorCode_t  DR_ReadConfig(DR_Register_t *DR_ConfigRegister)
{
    ErrorCode_t         ReturnStatus;
    DR_Register_t       RegisterValue;

    ReturnStatus = DR_I2C_Action(DIGITALRESISTOR_ADDRESS,
                                 TCON_REGISTER,
                                 DR_I2C_READ,
                                 &RegisterValue);

    *DR_ConfigRegister = RegisterValue;

    return ReturnStatus;
}

/** \brief Read Status
    \param[out]   Status Register   - Status Register value
    \return ErrorCode_t    Error code
*/
ErrorCode_t DigitalResistor_ReadStatus(DR_Register_t *StatusRegister)
{
    ErrorCode_t             ReturnStatus;
    DR_Register_t           RegVal;

    MN_ASSERT(oswrap_IsOSRunning());

    MN_I2C_DR_ACQUIRE();
        ReturnStatus = DR_ReadStatus(&RegVal);
    MN_I2C_DR_RELEASE();

    *StatusRegister = RegVal;

    return ReturnStatus;
}

/** \brief Read Config
    \param[out]   Config Register   - Config Register value
    \return ErrorCode_t    Error code
*/
ErrorCode_t DigitalResistor_ReadConfig(DR_Register_t *ConfigRegister)
{
    ErrorCode_t             ReturnStatus;
    DR_Register_t           RegVal;

    MN_ASSERT(oswrap_IsOSRunning());

    MN_I2C_DR_ACQUIRE();
        ReturnStatus = DR_ReadConfig(&RegVal);
    MN_I2C_DR_RELEASE();

    *ConfigRegister = RegVal;

    return ReturnStatus;
}

/** \brief Enable the WiperLock protection
    \return ErrorCode_t   Error code
*/
ErrorCode_t DigitalResistor_EnableWiperLock(void)
{
    ErrorCode_t             ReturnStatus;
    DR_Register_t           RegVal = 0;

    MN_ASSERT(oswrap_IsOSRunning());

    MN_I2C_DR_ACQUIRE();
        // The HI VOLTAGE HAS TO BE APPLIED EXTERNALLY
        ReturnStatus = DR_I2C_Action(DIGITALRESISTOR_ADDRESS,
                                     NONVOLATILE_WIPER_0,
                                     DR_I2C_DECREMENT,
                                     &RegVal);

    MN_I2C_DR_RELEASE();

    return ReturnStatus;
}

/** \brief Disable the WiperLock protection
    \return ErrorCode_t    Error code
*/
ErrorCode_t DigitalResistor_DisableWiperLock(void)
{
    ErrorCode_t             ReturnStatus;
    DR_Register_t           RegVal = 0;

    MN_ASSERT(oswrap_IsOSRunning());

    MN_I2C_DR_ACQUIRE();
        // The HI VOLTAGE HAS TO BE APPLIED EXTERNALLY
        ReturnStatus = DR_I2C_Action(DIGITALRESISTOR_ADDRESS,
                                     NONVOLATILE_WIPER_0,
                                     DR_I2C_INCREMENT,
                                     &RegVal);

    MN_I2C_DR_RELEASE();

    return ReturnStatus;
}

/** \brief Write the Wiper Position of the Digital Resitor
    \param[in]   DR_WiperStorage_t      DR_WiperStorage: volatile or non-volitile
    \param[in]   DR_WiperPosition_t     DR_WiperPosition
    \return ErrorCode_t                 Error code
*/
ErrorCode_t DigitalResistor_WriteWiper(DR_WiperStorage_t        DR_WiperStorage,
                                       DR_WiperPosition_t       DR_WiperPosition)
{
    ErrorCode_t             ReturnStatus;
    DR_WiperPosition_t      WriteValue;
    u8                      RegisterAddress;

    MN_ASSERT(oswrap_IsOSRunning());

    if (DR_WiperStorage >= DR_MAX_WIPER_STORAGE)
    {   // Storage Location is invalid
        return ERR_BIOS_PARAM;
    }

    if (DR_WiperPosition > DIGITAL_RESISTOR_MAX_WIPER)
    {   // Wiper Position is out of range
        return ERR_BIOS_PARAM;
    }

    // Copy the Write value locally
    WriteValue = DR_WiperPosition;

    // Address of the Wiper Position
    RegisterAddress = (DR_WiperStorage == DR_WIPER_VOLATILE_STORAGE) ?
                            VOLATILE_WIPER_0 : NONVOLATILE_WIPER_0;

    MN_I2C_DR_ACQUIRE();
        // Check if we can write the data
        ReturnStatus = DR_IsOKToAccessEEPROM(DR_WiperStorage, DR_I2C_WRITE);

        if (ReturnStatus == ERR_OK)
        {   // Yes, Resistopr Storage is writeable
            ReturnStatus = DR_I2C_Action(DIGITALRESISTOR_ADDRESS,
                                         RegisterAddress,
                                         DR_I2C_WRITE,
                                         &WriteValue);
        }
    MN_I2C_DR_RELEASE();

    return ReturnStatus;
}

/** \brief Read the Wiper Position of the Digital Resitor
    \param[in]   DR_WiperStorage_t      DR_WiperStorage: volatile or non-volitile
    \param[out]  DR_WiperPosition_t    *DR_WiperPosition
    \return ErrorCode_t                 Error code
*/
ErrorCode_t DigitalResistor_ReadWiper(DR_WiperStorage_t   DR_WiperStorage,
                                      DR_WiperPosition_t *DR_WiperPosition)
{
    ErrorCode_t             ReturnStatus;
    u8                      RegisterAddress;

    MN_ASSERT(DR_WiperPosition != NULL);
    MN_ASSERT(oswrap_IsOSRunning());

    if (DR_WiperStorage >= DR_MAX_WIPER_STORAGE)
    {   // Storage Location is invalid
        return ERR_BIOS_PARAM;
    }

    RegisterAddress = (DR_WiperStorage == DR_WIPER_VOLATILE_STORAGE) ?
                            VOLATILE_WIPER_0 : NONVOLATILE_WIPER_0;

    MN_I2C_DR_ACQUIRE();
    // Check if we can read the data
        ReturnStatus = DR_IsOKToAccessEEPROM(DR_WiperStorage, DR_I2C_READ);

        if (ReturnStatus == ERR_OK)
        {   // Read DR Wiper Register
            ReturnStatus = DR_I2C_Action(DIGITALRESISTOR_ADDRESS,
                                         RegisterAddress,
                                         DR_I2C_READ,
                                         DR_WiperPosition);
        }
    MN_I2C_DR_RELEASE();

    return ReturnStatus;
}

/** \brief Set the Digital Resistor configuration
    Init TCON register
*/
void DigitalResistor_Initialize(void)
{
    // Nothing
}

/* This line marks the end of the source */
