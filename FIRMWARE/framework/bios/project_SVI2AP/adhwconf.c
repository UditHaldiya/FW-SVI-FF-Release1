/*
Copyright 2011 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file adhwconf.c
    \brief Project-specific A/D hardware configuration

Originally based on LCX's ad_l.c,54

    CPU: Any

    OWNER: AP

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/framework/bios/project_SVI2AP/adhwconf.c $
    $Date: 12/15/11 4:17p $
    $Revision: 5 $
    $Author: Justin Shriver $

    \ingroup A_to_D
*/
/* $History: adhwconf.c $
 *
 * *****************  Version 5  *****************
 * User: Justin Shriver Date: 12/15/11   Time: 4:17p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/framework/bios/project_SVI2AP
 * -- AP -- Corrected GPIO use.
 *
 * *****************  Version 4  *****************
 * User: Justin Shriver Date: 12/09/11   Time: 7:04p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/framework/bios/project_SVI2AP
 * -- AP -- Fixed the Poiwer control table.
 *
 * *****************  Version 3  *****************
 * User: Justin Shriver Date: 12/08/11   Time: 6:35p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/framework/bios/project_SVI2AP
 * -- AP -- LINT - removed unused header, #defines.
 *
 * *****************  Version 2  *****************
 * User: Justin Shriver Date: 12/07/11   Time: 7:38p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/framework/bios/project_SVI2AP
 * -- AP -- Corrected tables. TFS:7889
 *
 * *****************  Version 1  *****************
 * User: Justin Shriver Date: 12/06/11   Time: 6:08p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/framework/bios/project_SVI2AP
 * -- AP -- BIOS modularization
 *
 * *****************  Version 1  *****************
 * User: Justin Shriver Date: 11/29/11   Time: 5:05p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/framework/bios/project_LCX
 * -- AP -- Moved file (AD HW configuration).
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 11/20/11   Time: 11:05p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/framework/bios
 * TFS:8255 - A/D project HW configuration
*/

#include "mnwrap.h"
#include BIOS_DEF_H_
#include "lpc2114io.h"
#include "ad.h"
#include "adhwconf.h"

#include TIMEBASE_H_

#include "oswrap.h"

#define HOLD_OFF    0
#if HOLD_OFF
 #include "watchdog.h"
#endif

/*******************************************************************************
*** internal defines
*******************************************************************************/

//----------------------------------------------------------------------------------
// Disable / Enable interrupts to correct Hall sensor issue

#define MN_DisableInterrupts()                  (__set_CPSR(__get_CPSR() | CPU_NO_INTERRUPTS))
#define MN_EnableInterruptsUnconditionially()   (__set_CPSR(__get_CPSR() & ~CPU_NO_INTERRUPTS))

//----------------------------------------------------------------------------------------------------------------------
/** \brief Select External A/D Converter

    \param  - None.
    \return - Nothing
*/
void    ad_SelectExtAD(void)
{
    GPIO1->IOCLR = GPIO1_CS_AD;         // select to transfer A/D value
}

/** \brief De-Select External A/D Converter

    \param  - None.
    \return - Nothing
*/
void    ad_DeSelectExtAD(void)
{
    GPIO1->IOSET = GPIO1_CS_AD;         // deselect to convert A/D value
}

//-------------------------------------------------------------------------------------------
// Short description:
//
// The SVI II AP ADC has a chain of Analog MUXes:
// Pressure MUX - controlled by PRESS_MUH [1:0], Selects
// P1       00  -> Input 2 of Analog Mux
// Atmos    01  -> Input 2 of Analog Mux
// P3       10  -> Input 2 of Analog Mux
// P2       11  -> Input 2 of Analog Mux
//
// Reference / Temperature MUX - controlled by TEMP_REF_MUX
// Temp_Int 0   -> Input 8 of Analog MUX
// AD_DIAG  1   -> Input 8 of Analog MUX  (this is 1.25V reference)
//
// Analog Mux - connects to ADC, controlled by ADC_MUX[2..0]
// Pilot Pressure       000
//
// P1                   001 -- see above (PRESS_MUX  00)
// Atmos                001              (           01)
// P3                   001              (           10)
// P2                   001              (           11)
//
// POS INT              010
// AI_SP                011
// IP Cur               100
// AI PV                101
// POS Rem              110
//
// Temp Int             111 -- see above (TEMP_REF_MUX  0)
// AD DIAG              111              (              1)
//
// Total amount of ADC CHannels: 1 + 4 + 5 + 2 = 12
//
//-------------------------------------------------------------------------------------------
// The LCX (SVI 1000) an Analog MUX:
//
// Analog Mux - connects to ADC, controlled by ADC_MUX[2..0]
// Pilot Pressure       000
// P2                   001
// POS INT              010
// AI_SP                011
// IP Cur               100
//  -- --               101
// AD DIAG              110
// Temp Int             111
//
// Total amount of ADC CHannels: 8
//
//-------------------------------------------------------------------------------------------


/* Multiplexer settings to select analog input channels */
#define MUX_PRESS_PILOT    ((u32)(0))
#define MUX_PRESS_1        (GPIO0_ADC_MUX0 )
#define MUX_PRESS_ATMOS    (GPIO0_ADC_MUX0 | GPIO0_PRESS_MUX1 | GPIO0_PRESS_MUX0 )
#define MUX_PRESS_3        (GPIO0_ADC_MUX0 | GPIO0_PRESS_MUX1 )
#define MUX_PRESS_2        (GPIO0_ADC_MUX0 | GPIO0_PRESS_MUX0 )
#define MUX_POS_INT        (GPIO0_ADC_MUX1 )
#define MUX_AI_SP          (GPIO0_ADC_MUX1 | GPIO0_ADC_MUX0 )
#define MUX_IP_CUR         (GPIO0_ADC_MUX2 )
#define MUX_AI_PV          (GPIO0_ADC_MUX2 | GPIO0_ADC_MUX0 )
#define MUX_POS_REM        (GPIO0_ADC_MUX2 | GPIO0_ADC_MUX1 )
#define MUX_TEMP_INT       (GPIO0_ADC_MUX2 | GPIO0_ADC_MUX1 | GPIO0_ADC_MUX0 )
#define MUX_AD_DIAG        (GPIO0_ADC_MUX2 | GPIO0_ADC_MUX1 | GPIO0_ADC_MUX0 | GPIO0_TEMP_REF_MUX )

//-------------------------------------------------------------------------------------------

#define MUX_POWER_DOWN      (GPIO0_ADC_MUX2 | GPIO0_ADC_MUX1 | GPIO0_ADC_MUX0 | GPIO0_PRESS_MUX0 | GPIO0_PRESS_MUX1 | GPIO0_TEMP_REF_MUX)
#define POWER_BIT_MASK      (GPIO0_P_POS_REM | GPIO0_P_POS_INT | GPIO0_P_PRESS)   /* Default is all power switches are off */

#if HOLD_OFF
 // The following to hold off microprocessor initialize until a nominal 3.15 milliamps.
 //
 #define MA_TO_RUN      (3.15)
 #define VOLTS_PER_MA   (.1)
 #define A_TO_D_RANGE   (2.5)
 #define A_TO_D_COUNT   (65535.0)
 #define ENOUGH_POWER   (((MA_TO_RUN * VOLTS_PER_MA) / A_TO_D_RANGE) * A_TO_D_COUNT)
// Poll interval for initialize check
 #define WAIT_MS        (25u)
 #define WAIT_TIME      ((XTALFREQ / (VPRATE * 1000u)) * WAIT_MS)
#endif

/* Table with multiplexer and power settings for all A/D input channels */
static const u32 MuxPwrTable[NUMBER_OF_AD_CHANNELS] =
{
    [AD_AI_SP      ] = (MUX_AI_SP       | 0U                ),
    [AD_AI_PV      ] = (MUX_AI_PV       | 0U                ),
    [AD_TEMP_INT   ] = (MUX_TEMP_INT    | 0U                ),
    [AD_TEMPR_INIT ] = (MUX_TEMP_INT    | 0U                ), //must be same
    [AD_PRESS_ATMOS] = (MUX_PRESS_ATMOS | GPIO0_P_PRESS     ),
    [AD_POS_REM    ] = (MUX_POS_REM     | GPIO0_P_POS_REM   ),
    [AD_PRESS_1    ] = (MUX_PRESS_1     | GPIO0_P_PRESS     ),
    [AD_POS_INT    ] = (MUX_POS_INT     | GPIO0_P_POS_INT   ),
    [AD_PRESS_3    ] = (MUX_PRESS_3     | GPIO0_P_PRESS     ),
    [AD_PRESS_2    ] = (MUX_PRESS_2     | GPIO0_P_PRESS     ),
    [AD_PRESS_PILOT] = (MUX_PRESS_PILOT | 0U                ),
    [AD_IP_CUR     ] = (MUX_IP_CUR      | 0U                ),
    [AD_DIAG       ] = (MUX_AD_DIAG     | 0U                )
};


#define VAR_SAMPLES 1           // enable selectable no of samples / sensor
#if !VAR_SAMPLES
#define SAMPLES     1u          // no of A/D samples to take
#endif

#if VAR_SAMPLES
#define SAMPLE_CT(xx)    {(xx) - 1u, FACTOR / (xx)}
#define ONE_SAMPLE 1u

static const ADsamples_t sampleTbl[] =
{
    //lint -save -e778  constant expression evaluates to 0
    [AD_AI_SP] = SAMPLE_CT(ONE_SAMPLE),          //
    [AD_AI_PV] = SAMPLE_CT(ONE_SAMPLE),          //
    [AD_TEMP_INT] = SAMPLE_CT(ONE_SAMPLE),          //
    [AD_TEMPR_INIT] = SAMPLE_CT(3U),          //
    [AD_PRESS_ATMOS] = SAMPLE_CT(ONE_SAMPLE),          //
    [AD_POS_REM] = SAMPLE_CT(3u),                  //
    [AD_PRESS_1] = SAMPLE_CT(ONE_SAMPLE),          //
    [AD_POS_INT] = SAMPLE_CT(3u),                  //
    [AD_PRESS_3] = SAMPLE_CT(ONE_SAMPLE),          //
    [AD_PRESS_2] = SAMPLE_CT(ONE_SAMPLE),          //
    [AD_PRESS_PILOT] = SAMPLE_CT(ONE_SAMPLE),          //
    [AD_IP_CUR] = SAMPLE_CT(ONE_SAMPLE),          //
    [AD_DIAG] = SAMPLE_CT(ONE_SAMPLE)           //
    //lint -restore                                               /
};
#endif


//-------------------------------------------------------------------------------------------
// A/D H/W related data

const adhwconf_t adhwconftab =
{
    .sampleTbl = sampleTbl,
    .MuxPwrTable = MuxPwrTable,
};

//-------------------------------------------------------------------------------------------

void ad_EndSequence(void)
{
    GPIO0->IOCLR = MUX_POWER_DOWN ;
    GPIO0->IOSET = POWER_BIT_MASK; /* switch all power supplies to OFF */
}

//-------------------------------------------------------------------------------------------
// This variable is used ONLY in the function below!
static u8 ad_PrepareChannel_PreviousChannel = 0;

void ad_PrepareChannelPower(u8 channel, u8 lookaheadhannel)
{
    if ((lookaheadhannel == AD_POS_INT) ||
        (channel         == AD_POS_INT))
    {   // We are just about to power up Hall sensor
        MN_DisableInterrupts();
    }

    GPIO0->IOCLR = (MUX_POWER_DOWN | (adhwconftab.MuxPwrTable[channel] & POWER_BIT_MASK));
    GPIO0->IOSET = (adhwconftab.MuxPwrTable[channel] ^ POWER_BIT_MASK);

    // if we just powered off the Hall sensor, enable interrupts
    if (ad_PrepareChannel_PreviousChannel == AD_POS_INT)
    {
        MN_EnableInterruptsUnconditionially();
    }

    // Store the channel
    ad_PrepareChannel_PreviousChannel = channel;
}

//-------------------------------------------------------------------------------------------
// this is A/D specific function with sole purpose to verify the "correctness"
// of the First channel
// First selected shall not be the Powered channel.

bool_t  ad_IsChannelPowered(u8 channel)
{
    bool_t  retval = true;

    if ((adhwconftab.MuxPwrTable[channel] & POWER_BIT_MASK) == POWER_BIT_MASK)
    {
        retval = false;                     // Channel is NOT powered.
    }

    return retval;
}

//-------------------------------------------------------------------------------------------
// Low Level Timer 0 code
//
// Note: timer.c contains the 5ms resolution timer and no H/W related code

/** \brief The LCD is in use so we cannot poll the keyboard.  We must, however,
    delay here for the A/D multiplexer to settle.

    This code relies on the idea that the time elapsed since we captured rawtime
       cannot exceed 5 ms, or 1 (one) timer 0 reset period.
       If bios_MeasureAd() is called from the cycle task, the control task is not
       running yet, and cycle is the highest priority task, so we are OK.
       Otherwise, it's the control task calling, which, again, is the highest
       priority.

    As of 6/9/2005 the A/D signal (input current) is the first in the A/D read sequence
    The delay of 75 us was empirically determined.  The call to bios_HandleDigitalInput()
    takes about 100 microseconds longer than that.  Therfore we do not
    bother to invoke MultiplexDelay() when we can poll the keyboard.
*/

//#define T0_RAW_TICKS_BETWEEN_RESETS (PCLKFREQ/LCL_TICKS_PER_SEC)
#define ADWAIT_TIME_uS .000075 //in seconds
#define ADWAIT_TC ((u32)((((cdouble_t)PCLKFREQ*ADWAIT_TIME_uS))+0.5)) //timer 0 ticks; -1?

/** \brief Get the 32-bit Timer 0 value

  \return Timer Value
*/
static u32     bios_GetTimer_0(void)
{
    return T0->TC;
}

/** \brief Execute delay based on Timer 0

  \return None
*/
static void    ad_Timer_0_Delay(u32 StartingCount, u32 Delay)
{
    u32 endtime;

    do
    {
        endtime = bios_GetTimer_0();
        if(endtime <= StartingCount)
        {
            //This means timer 0 was reset, adjust endtime by the reset period
            endtime += PCLKFREQ / LCL_TICKS_PER_SEC;
        }
    } while((StartingCount + Delay) > endtime);
}

/** \brief A small delay partially filled with something useful
\param useful_func - a pointer to that "something useful"
*/
void ad_MultiplexDelay(void (*useful_func)(void))
{
    u32 StartingTime = bios_GetTimer_0();

    /* do something useful. Read pushbutton interface . May be long enough to skip MuxDelay */
    useful_func();

    // Now waste some time --
    // we lapsed the start time
    // Did something (potentially) useful
    // Now we burn the rest of the delay time.
    ad_Timer_0_Delay(StartingTime, ADWAIT_TC);
}

//-------------------------------------------------------------------------------------------
