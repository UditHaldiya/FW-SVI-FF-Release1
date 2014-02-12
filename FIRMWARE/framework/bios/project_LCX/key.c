/**
Copyright 2004-2005 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file key.c
    \brief BIOS routines for support of the keyboard buttons
    keyboard/display reconnect is also supported

    CPU: Any (with standard C compiler)

    OWNER: EP
    \n RCS: $Id$
    Created: 9/27/04 Author:  EP
*/

/*
    $History: key.c $
 * 
 * *****************  Version 37  *****************
 * User: Arkkhasin    Date: 11/04/11   Time: 3:57p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/framework/bios/project_LCX
 * TFS:8072 NVMEM upgrade 
 * Headers fixed
 *
 * *****************  Version 35  *****************
 * User: Arkkhasin    Date: 1/28/11    Time: 1:43p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * Undo TFS:5505
 *
 * *****************  Version 33  *****************
 * User: Anatoly Podpaly Date: 1/11/11    Time: 11:15a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * TFS 5324 - removed obsoleted EMulation-related definitions, variables
 * and the code. Converted the newly introduced file scope global var to
 * standard form (m_name).
 *
 * *****************  Version 32  *****************
 * User: Anatoly Podpaly Date: 12/03/10   Time: 12:08p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * Bug 4754 - The debouncing code was corrected to eliminate the "release"
 * situation on bouncing. Added critical section protections for the PB
 * and RS Power control code. Headers corrected.
 *
 *
 * *****************  Version 31  *****************
 * User: Anatoly Podpaly Date: 11/08/10   Time: 11:49a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * Bug 4637 - removed unused code from teh file.
 *
 * *****************  Version 30  *****************
 * User: Anatoly Podpaly Date: 10/12/10   Time: 2:23p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * Bug 4359 - Added definition for the Platform Hardware. Added
 * conditional compilation based on the type of the Platform H/W.
 *
 * *****************  Version 29  *****************
 * User: Anatoly Podpaly Date: 9/09/10    Time: 11:56a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * Updates for real H/W.
 *
 * *****************  Version 28  *****************
 * User: Anatoly Podpaly Date: 8/25/10    Time: 11:53a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * Enable / Disable power on PB / RS.
 *
 * *****************  Version 27  *****************
 * User: Anatoly Podpaly Date: 6/18/10    Time: 11:45a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * Added mods for actual H/W w/cond compilation.
 *
 * *****************  Version 26  *****************
 * User: Anatoly Podpaly Date: 5/28/10    Time: 3:03p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * Updated Req numbers.
 *
 * *****************  Version 25  *****************
 * User: Anatoly Podpaly Date: 5/24/10    Time: 2:54p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * Placed comments referencing requirements.
 *
 * *****************  Version 24  *****************
 * User: Anatoly Podpaly Date: 4/28/10    Time: 5:36p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * Development in progress: added function to read interface card ID.
 * (stub)
 *
 * *****************  Version 23  *****************
 * User: Anatoly Podpaly Date: 4/01/10    Time: 5:45p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * Bug #2930 - Enable UI emulation.
 *
 * *****************  Version 22  *****************
 * User: Arkkhasin    Date: 3/09/10    Time: 12:37a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * Lint (mostly unused header)
 *
 * *****************  Version 21  *****************
 * User: Anatoly Podpaly Date: 2/23/10    Time: 5:35p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * Added check for interference between PB and jumper readings.
 *
 * *****************  Version 20  *****************
 * User: Anatoly Podpaly Date: 2/23/10    Time: 4:06p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * Fix for the Buttons - renmove the Power Cycling on Pullups, correct the
 * Shift register operations.
 *
 * *****************  Version 19  *****************
 * User: Anatoly Podpaly Date: 2/09/10    Time: 4:20p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * Corrected the Header shift value.
 *
 * *****************  Version 18  *****************
 * User: Anatoly Podpaly Date: 2/09/10    Time: 11:51a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * LCX development - different pushbutton handling.
 *
 * *****************  Version 17  *****************
 * User: Anatoly Podpaly Date: 2/01/10    Time: 4:29p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * LCX initial implementation.

*/

//-----------------------------------------------------
// This file contains code implementding requirements
/** REQ 31-001; REQ 31-002; REQ 31-003; REQ 31-005 */
//-----------------------------------------------------

#include "mnwrap.h"
#include "oswrap.h"
#include "mnassert.h"
#include "lpc2114io.h"
#include "bios_def.h"
#include "faultpublic.h"
#include "key.h"

#if     PLATFORM_HARDWARE == LCX_PROTOTYPE_HARDWARE

#define PB_MASK             (GPIO0_AUTO | GPIO0_OPEN | GPIO0_CLOSE | GPIO0_CANCEL)

// This valid for the PROTO H/W!!
#define HDR_COUNT           8u              // 8 header positions

CONST_ASSERT((1u << HDR_GPIO_PIN) == GPIO1_SI_HDR);

#define HDR_ALIGN_SFT       ((HDR_GPIO_PIN - HDR_COUNT) + 1u)

//While at it: we want to be able to shift all 8 bits comfortably; hence the shift direction
CONST_ASSERT(HDR_GPIO_PIN > 8u);
#define HDR_SHIFT_IN(keymask) (((keymask) >> 1) | (GPIO1->IOPIN &  GPIO1_SI_HDR))

#else

#define PB_MASK             (GPIO1_PB_AUTO | GPIO1_PB_OPEN | GPIO1_PB_FIND | GPIO1_PB_CANCEL)
#define IO_JUMPER_MASK      (GPIO0_RS_B0   | GPIO0_RS_B1   | GPIO0_RS_B2   | GPIO0_RS_B3)

#endif

//------------------------------------------------------------------------------------------------------

// Digital Inputs - for the Jumpers etc.
static u8  m_DigitalInputsBitMap = 0;

// #define LCX_EMULATION_ON    (1)

/** REQ 31-005 */
#define JUMPER_VALID_MASK                   (JUMPER_PARAMETER_MASK | JUMPER_ATO_ATC_MASK)

//------------------------------------------------------------------------------------------------------

#define PB_DEBOUNCE_COUNT 2

typedef u8 key_val_t;

/* pushutton related variables */
typedef struct
{
    key_val_t   debounced;
    key_val_t   lastVal;
    key_val_t   downEdge;
    u8          latchCount;
}   PBData_t;

static PBData_t KeyData;

//------------------------------------------------------------

void bios_InitPushbuttons(void)
{

}

/** \brief read and debounce the current state of the pushbuttins
*/
void bios_HandleDigitalInput(void)
{
    u8          keyVal = 0;
    u32         ioval;

#if     PLATFORM_HARDWARE == LCX_PROTOTYPE_HARDWARE

    if ((GPIO1->IOPIN & GPIO1_HDR_LOAD) == GPIO1_HDR_LOAD)
    {
        return;                             // someone reading the headers - exit
    }

    ioval = GPIO0->IOPIN ^ PB_MASK;

    if ((ioval & GPIO0_AUTO) != 0u)
    {
        keyVal |= PUSHBUTTON_AUTO;
    }

    if ((ioval & GPIO0_OPEN) != 0u)
    {
        keyVal |= PUSHBUTTON_OPEN;
    }

    if ((ioval & GPIO0_CLOSE) != 0u)
    {
        keyVal |= PUSHBUTTON_CLOSED;
    }

    if ((ioval & GPIO0_CANCEL) != 0u)
    {
        keyVal |= PUSHBUTTON_CANCEL;
    }

#else
    // Flip Enable Switch ON
    MN_ENTER_CRITICAL();
        GPIO0->IOCLR = GPIO0_EN_SWITCH;             // Set PB/RS Enable Switch ON (LO)
        ioval = GPIO1->IOPIN ^ PB_MASK;             // Read PBs
        GPIO0->IOSET = GPIO0_EN_SWITCH;             // Set PB/RS Enable Switch OFF (HI)
    MN_EXIT_CRITICAL();

    if ((ioval & GPIO1_PB_AUTO) != 0u)
    {
        keyVal |= PUSHBUTTON_AUTO;
    }

    if ((ioval & GPIO1_PB_OPEN) != 0u)
    {
        keyVal |= PUSHBUTTON_OPEN;
    }

    if ((ioval & GPIO1_PB_FIND) != 0u)
    {
        keyVal |= PUSHBUTTON_CLOSED;
    }

    if ((ioval & GPIO1_PB_CANCEL) != 0u)
    {
        keyVal |= PUSHBUTTON_CANCEL;
    }

#endif

    MN_ENTER_CRITICAL();
        // Debouncing code
        if (keyVal == KeyData.lastVal)
        {   // We are not bouncing
            if (KeyData.latchCount == 0)
            {   // We are not bouncing for a number of cycles - truly debounced
                u8 edge = keyVal ^ KeyData.debounced;

                KeyData.downEdge |= keyVal & edge;
                KeyData.debounced = keyVal;
            }
            else
            {   // Decrement the debouncing counter - need to be stable for a number of cycles
                --KeyData.latchCount;
            }
        }
        else
        {   // Here we are bouncing
            // We do not need to clear the Debounced value - it will be cleared automatically
            // by the debouncing code above. Just let it do the work.

            KeyData.lastVal    = keyVal;                // Store the last key value for debouncer
            KeyData.latchCount = PB_DEBOUNCE_COUNT;     // Restart the debouncing counter
        }
    MN_EXIT_CRITICAL();
}

/**
    \brief return the stae of the pushbuttons
    After call of function, the "pressed" flags are reset.

    \param[out] bool_t - state of digital input - true == open; false closed
     : bitcoded pushbuttons PUSHBUTTON_STAR, PUSHBUTTON_MINUS, PUSHBUTTON_PLUS
                  return value = pressed (down and released) state of pushbuttons	
                  If bit = 1 (down and released)
                  pUpDown = state of pushbuttons
                  0 = pushbutton is up
                  1 = pushbutton is down
*/
u8 bios_ReadPushButtonStatus(u8 *pUpdown)
{
    u8  retval;

    MN_ASSERT(NULL != pUpdown);

    MN_ENTER_CRITICAL();
        *pUpdown = KeyData.debounced;
        retval = KeyData.downEdge;
        KeyData.downEdge = 0u;
    MN_EXIT_CRITICAL();

    return retval;
}

/**
    \brief Read the Low-Cost front panel headers.  Thius is done by
            strobing in the 8 header positionsin serial fashion.

    \return u8_least - the header value - 0's represent shunts installed
*/
/** REQ 31-005 */
u8 bios_ReadJumpers(void)
{
    u32         hdr_temp;

#if     PLATFORM_HARDWARE == LCX_PROTOTYPE_HARDWARE
    u8_least    shiftCnt;

    GPIO1->IOCLR = GPIO1_HDR_LOAD;                      // enable pull-ups and parallel load mode
    GPIO1->IOCLR = GPIO1_HDR_CLK;                       // load or shift bit
    hdr_temp = 0u;

    for (shiftCnt = HDR_COUNT; shiftCnt > 0U; shiftCnt--)
    {
        GPIO1->IOCLR = GPIO1_HDR_CLK;                   // load or shift bit
        GPIO1->IOSET = GPIO1_HDR_CLK | GPIO1_HDR_LOAD;  // disable parallel load and complement RS
        hdr_temp     = HDR_SHIFT_IN(hdr_temp);
    }

    GPIO1->IOCLR = GPIO1_HDR_LOAD;                      // enable pull-ups and parallel load mode

    m_DigitalInputsBitMap  = (u8)(hdr_temp >> HDR_ALIGN_SFT);

#else
    MN_ENTER_CRITICAL();
        GPIO0->IOCLR = GPIO0_EN_SWITCH;                     // Set PB/RS Enable Switch ON (LO)
        m_DigitalInputsBitMap = 0u;
        hdr_temp = GPIO0->IOPIN & IO_JUMPER_MASK;           // Read jumpers
        GPIO0->IOSET = GPIO0_EN_SWITCH;                     // Set PB/RS Enable Switch OFF (HI)
    MN_EXIT_CRITICAL();

    if ((hdr_temp & GPIO0_RS_B0) != 0u)
    {
        m_DigitalInputsBitMap |= RS_JUMPERS_BIT_0;
    }

    if ((hdr_temp & GPIO0_RS_B1) != 0u)
    {
        m_DigitalInputsBitMap |= RS_JUMPERS_BIT_1;
    }

    if ((hdr_temp & GPIO0_RS_B2) != 0u)
    {
        m_DigitalInputsBitMap |= RS_JUMPERS_BIT_2;
    }

    if ((hdr_temp & GPIO0_RS_B3) != 0u)
    {
        m_DigitalInputsBitMap |= RS_JUMPERS_BIT_3;
    }

#endif
    m_DigitalInputsBitMap  = ((~m_DigitalInputsBitMap) & (u8)JUMPER_VALID_MASK);

    return m_DigitalInputsBitMap;
}

//-----------------------------------------------------------------------
// Below are hacks for the LCX UI.
//
// AP : Need to revisit and remove them!!
//
// Non-stub functions:

// End of non-stub functions

bool_t bios_ReadDiState(void)
{
    return true;
}

/* End of the source */
//-----------------------------------------------------------
