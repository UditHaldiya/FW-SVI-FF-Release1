/**
    \file uiemul_led.c
    \brief

    CPU: Philips LPC21x4 (ARM)

    OWNER: AP

    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue/uiemul_led.c $
    $Date: 12/02/10 2:22p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup Hart
*/
/* $History: uiemul_led.c $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 12/02/10   Time: 2:22p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart/glue
 * Extracted from hartfunc.c FOB TFS:4922
*/
#include "mnwrap.h"
#include "hartcmd.h"
#include "hartdef.h"
#include "hart.h"
#include "hartfunc.h"
#include "bufutils.h"
#include "key.h"
#include "quad.h"
#include "uipublic.h"

// Flags to control emulation
#define PARAMETER_SET_EMULATION_ON          (1u)
#define PARAMETER_SET_EMULATION_OFF         (0u)



#define HART_EMULATION_CONTROL_SIGNATURE    (0xAA55)
#define HART_EMULATION_CONTROL_CMD          (0U)
#define HART_EMULATION_PARAMETERS_CMD       (1U)

// Byte / Nibble manipulation definitions
#define NIBBLE_SHIFT                        (4U)
#define NIBBLE_MASK_LOW                     (0x0F)

// Get LO Nibble from the byte
// Returns the byte with LO Nibbles packed into it
static u8   GetNibbleLO(u8 ByteParam)
{
    return (ByteParam & NIBBLE_MASK_LOW);
}

// Get HI Nibble from the byte
// Returns the byte with HI Nibbles packed into it
static u8   GetNibbleHI(u8 ByteParam)
{
    return ((ByteParam >> NIBBLE_SHIFT) & NIBBLE_MASK_LOW);
}


/**
    \brief Emulation support command:
    Subcommand 0 - control Emulation On/Off
    Subcommand 1 - sends emulated PushButton, Jumpers, Rotary Encoder
    All subcommands return the LED states to the HART Master
*/
s8_least hart_Command_240_EmulationCommand(const u8 *src, u8 *dst)
{
    s8_least    ret = HART_OK;      // Assume OK
    u8          ControlByte;        // Subcommand # / control byte
    u8          FirstParameter;     // First parameter

    u16         Signature;          // Vars for parsing the command
    u16         PushButtons;        //
    u8          Jumpers;            //
    s16         Encoder;            //

    ControlByte    = util_GetU8(&src[HC240_DATA]);                  // Extract subcommand
    FirstParameter = util_GetU8(&src[HC240_DATA + 1]);              // Extract First parameter

    switch (ControlByte)
    {
        case HART_EMULATION_CONTROL_CMD :                           // Emulation Control Function
            // Verify the control signature
            Signature = util_GetU16(&src[HC240_DATA + 2]);
            if (Signature == HART_EMULATION_CONTROL_SIGNATURE)
            {   // Signature correct  - Set emulation control
                if (FirstParameter == PARAMETER_SET_EMULATION_ON)
                {   // Turn emulation ON
                    HART_EmulationControl(true);
                }
                else if (FirstParameter == PARAMETER_SET_EMULATION_OFF)
                {   // Turn Emulation OFF
                    HART_EmulationControl(false);
                }
                else
                {
                    ret = HART_INVALID_DATA;
                }
            }
            else
            {   // Signature did not match - incorrect parameter
                ret = HART_INVALID_DATA;
            }
            break;

        case HART_EMULATION_PARAMETERS_CMD :                        // Emulation Function
            // First Parameter contains the PB and jumpers - packed values
            PushButtons = GetNibbleLO(FirstParameter);              // Get LO nibble (PB)
            Jumpers     = GetNibbleHI(FirstParameter);              // Get HI nibble (Jumpers)
            Encoder     = (s16)util_GetU16(&src[HC240_DATA + 2]);   // Get RE clicks

            // Send the emulated parameters for processing
            HART_EmulationSetKeys(PushButtons);
            HART_EmulationSetJumpers(Jumpers);
            HART_EmulationSetSteps(Encoder);
            break;

        default :                                                   // Something else - wrong parameter
            ret = HART_INVALID_SELECTION;
            break;
    }

    // Now reply with LED info...
    util_PutU8(&dst[HC240_DATA + 0u], (u8)LED_GetDisplayState());
    util_PutU8(&dst[HC240_DATA + 1u], 0u);

    return ret;
}
