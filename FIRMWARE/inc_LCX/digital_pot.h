/*
Copyright 2008 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file digital_pot.h
    \brief Header for digital_pot i2c Digital Resistor driver

    CPU: Philips LPC21x4 (ARM)

    OWNER: Anatoly Podpaly

    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX/digital_pot.h $
    $Date: 10/22/10 1:01p $
    $Revision: 3 $
    $Author: Anatoly Podpaly $

    \ingroup digital_pot
*/

/* $History: digital_pot.h $
 * 
 * *****************  Version 3  *****************
 * User: Anatoly Podpaly Date: 10/22/10   Time: 1:01p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * Fixed protection wrapper.
 * 
 * *****************  Version 2  *****************
 * User: Anatoly Podpaly Date: 10/20/10   Time: 6:20p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * Bug 4223 - implemented results of code review:
 * -- Eliminated proprietary error codes.
 * -- Added Read Config Register function.
 * -- Write Wiper passes parameter by value.
 * 
 * *****************  Version 1  *****************
 * User: Anatoly Podpaly Date: 8/31/10    Time: 2:46p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * Header file for Digirtal Potentiometer. 
 *                
 * 
*/

#ifndef DIGITALRESISTOR_H_
#define DIGITALRESISTOR_H_

// ------------------------------------------------------------------------------------------------------------
// The Digital resistor, based on particular implementation, may contain 1 or 2 actual resistors.
// They are named Resistor 1 and Resistor 2

typedef enum {
    DR_WIPER_VOLATILE_STORAGE,      //!< RAM storage
    DR_WIPER_NONVOLATILE_STORAGE,   //!< ROM storage
    DR_MAX_WIPER_STORAGE            // For testing - Invalid value
} DR_WiperStorage_t;

typedef u16     DR_WiperPosition_t;
typedef u16     DR_Register_t;

// ------------------------------------------------------------------------------------------------------------
// Added commands for manufacturing

/** \brief Read Status
    \param[out]   Status Register   - Status Register value
    \return ErrorCode_t     Error code
*/
ErrorCode_t DigitalResistor_ReadStatus(DR_Register_t *StatusReg);

/** \brief Read Config
    \param[out]   Config Register   - Config Register value
    \return ErrorCode_t    Error code
*/
ErrorCode_t DigitalResistor_ReadConfig(DR_Register_t *ConfigRegister);

/** \brief Enable the WiperLock protection
    \return  ErrorCode_t    Error code
*/
ErrorCode_t DigitalResistor_EnableWiperLock(void);

/** \brief Disable the WiperLock protection
    \return  ErrorCode_t    Error code
*/
ErrorCode_t DigitalResistor_DisableWiperLock(void);

// ------------------------------------------------------------------------------------------------------------

/** \brief Set the Digital Resistor configuration
*/
void            DigitalResistor_Initialize(void);

/** \brief Read the Wiper Position of the Digital Resitor
    \param[in]   DR_WiperStorage_t      DR_WiperStorage: volatile or non-volitile
    \param[out]  DR_WiperPosition_t    *DR_WiperPosition
    \return ErrorCode_t                 Error code
*/
ErrorCode_t     DigitalResistor_ReadWiper(DR_WiperStorage_t   DR_WiperStorage,
                                          DR_WiperPosition_t *DR_WiperPosition);

/** \brief Write the Wiper Position of the Digital Resitor
    \param[in]   DR_WiperStorage_t      DR_WiperStorage: volatile or non-volitile
    \param[in]   DR_WiperPosition_t     DR_WiperPosition
    \return ErrorCode_t                 Error code
*/
ErrorCode_t     DigitalResistor_WriteWiper(DR_WiperStorage_t        DR_WiperStorage,
                                           DR_WiperPosition_t       DR_WiperPosition);

#endif // DIGITALRESISTOR_H_

/* This line marks the end of the source */
