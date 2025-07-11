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
#include "ad.h"
#include "adhwconf.h"
#include "gpio.h"
#include "stmcommon.h"

#include "oswrap.h"

/*******************************************************************************
*** internal defines
*******************************************************************************/

//----------------------------------------------------------------------------------
// Disable / Enable interrupts to correct Hall sensor issue

// #if __CPU_MODE__ == CPU_MODE_ARM
// #define MN_DisableInterrupts()                  (__set_CPSR(__get_CPSR() | CPU_NO_INTERRUPTS))
// #define MN_EnableInterruptsUnconditionially()   (__set_CPSR(__get_CPSR() & ~CPU_NO_INTERRUPTS))
// #endif
// #if __CPU_MODE__ == CPU_MODE_THUMB
// #define MN_DisableInterrupts()                  (mn_disable())
// #define MN_EnableInterruptsUnconditionially()   (mn_enable())
// #endif


#define MN_DisableInterrupts()                  (__disable_interrupt())
#define MN_EnableInterruptsUnconditionially()   (__enable_interrupt())

//----------------------------------------------------------------------------------------------------------------------
/** \brief Select External A/D Converter

    \param  - None.
    \return - Nothing
*/
void    ad_SelectExtAD(void)
{
    gpio_SelectExtAD();
}

/** \brief De-Select External A/D Converter

    \param  - None.
    \return - Nothing
*/
void    ad_DeSelectExtAD(void)
{
    gpio_DeSelectExtAD();
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

#define X_MUX0      (BIT(ADC_MUX0))
#define X_MUX1      (BIT(ADC_MUX1))
#define X_MUX2      (BIT(ADC_MUX2))
#define X_PMUX0     (BIT(PRESS_MUX0))
#define X_PMUX1     (BIT(PRESS_MUX1))
#define X_P_REM     (BIT(P_POS_REM))
#define X_P_INT     (BIT(P_POS_INT))
#define X_P_PRE     (BIT(P_PRESS))
#define MUX_MASK    (X_MUX0 | X_MUX1 | X_MUX2 | X_PMUX0 |X_PMUX1 | X_P_REM | X_P_INT | X_P_PRE)
#define MUX_PARK    (                                              X_P_REM | X_P_INT | X_P_PRE)
#define POWER(x)    (MUX_PARK & ~(x))

//-------------------------------------------------------------------------------------------


#define POWER_BIT_MASK  (X_P_REM | X_P_INT | X_P_PRE)

//-------------------------------------------------------------------------------------------

#define V_PRESS_PILOT (    0u |     0u |     0u |      0u |      0u | POWER(     0u))  // 000
#define V_PRESS_1     (    0u |     0u | X_MUX0 |      0u |      0u | POWER(X_P_PRE))  // 001 00
#define V_PRESS_2     (    0u |     0u | X_MUX0 |      0u | X_PMUX0 | POWER(X_P_PRE))  // 001 01
#define V_PRESS_3     (    0u |     0u | X_MUX0 | X_PMUX1 |      0u | POWER(X_P_PRE))  // 001 10
#define V_PRESS_ATMOS (    0u |     0u | X_MUX0 | X_PMUX1 | X_PMUX0 | POWER(X_P_PRE))  // 001 11
#define V_POS_INT     (    0u | X_MUX1 |     0u |      0u |      0u | POWER(X_P_INT))  // 010
#define V_TEMP_INT    (    0u | X_MUX1 | X_MUX0 |      0u |      0u | POWER(     0u))  // 011
#define V_IP_CUR      (X_MUX2 |     0u |     0u |      0u |      0u | POWER(     0u))  // 100
#define V_AI_PV       (X_MUX2 |     0u | X_MUX0 |      0u |      0u | POWER(     0u))  // 101
#define V_POS_REM     (X_MUX2 | X_MUX1 |     0u |      0u |      0u | POWER(X_P_REM))  // 110
#define V_DIAG        (X_MUX2 | X_MUX1 | X_MUX0 |      0u |      0u | POWER(     0u))  // 111

#define SET_RES(x)  ((x) | (((~x) & MUX_MASK) << 16))

// multiplexer and power settings table
static const u32 MuxPwrTable[] =
{
    [AD_PRESS_PILOT] =  SET_RES(V_PRESS_PILOT),
    [AD_PRESS_1    ] =  SET_RES(V_PRESS_1    ),
    [AD_PRESS_2    ] =  SET_RES(V_PRESS_2    ),
    [AD_PRESS_3    ] =  SET_RES(V_PRESS_3    ),
    [AD_PRESS_ATMOS] =  SET_RES(V_PRESS_ATMOS),
    [AD_POS_INT    ] =  SET_RES(V_POS_INT    ),
    [AD_TEMP_INT   ] =  SET_RES(V_TEMP_INT   ),
    [AD_TEMPR_INIT ] =  SET_RES(V_TEMP_INT   ), //must be same
    [AD_IP_CUR     ] =  SET_RES(V_IP_CUR     ),
    [AD_AI_PV      ] =  SET_RES(V_AI_PV      ),
    [AD_POS_REM    ] =  SET_RES(V_POS_REM    ),
    [AD_DIAG       ] =  SET_RES(V_DIAG       )
};

//-------------------------------------------------------------------------------------------

#define VAR_SAMPLES (1)             // enable selectable no of samples / sensor
#if !VAR_SAMPLES
#define SAMPLES     (1u)            // no of A/D samples to take
#endif

#if VAR_SAMPLES
#define SAMPLE_CT(xx)    {(xx) - 1u, FACTOR / (xx)}
#define ONE_SAMPLE (1u)
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

    // [AD_POS_INT] = SAMPLE_CT(ONE_SAMPLE),                  //
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
    //replaced with literal .PwrDownMask = MUX_POWER_DOWN,
    //replaced with literal .PwrBitsMask = POWER_BIT_MASK,
};

//-------------------------------------------------------------------------------------------

void ad_EndSequence(void)
{
    u32 BitMask;

    BitMask = SET_RES(MUX_PARK);
    gpio_SelectMUXPWRbits(BitMask);
}

//-------------------------------------------------------------------------------------------
// This variable is used ONLY in the function below!
static u8 ad_PrepareChannel_PreviousChannel = 0u;

void ad_PrepareChannelPower(u8 channel, u8 lookaheadhannel)
{
    u32 BitMask;

    if ((lookaheadhannel == AD_POS_INT) ||
        (channel         == AD_POS_INT))
    {   // Anticipate that the NEXT channel after this one will be the Hall sensor
        // and take preventive action -- disable interrupts
        // We are just about to power up Hall sensor
        MN_DisableInterrupts();
    }

    MN_ASSERT(channel < NELEM(MuxPwrTable));
    BitMask = MuxPwrTable[channel];

    gpio_SelectMUXPWRbits(BitMask);

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

#define ADWAIT_TIME_uS      (.000075) //in seconds
#define ADWAIT_TC           ((u32)(((cdouble_t)CORE_FREQ * ADWAIT_TIME_uS) + 0.5))

/** \brief Get the 32-bit Timer 0 value
  \return Timer Value
*/
static u32     bios_GetTimer_0(void)
{
    return SysTick->VAL;
}

/** \brief Execute delay based on Timer 0

  \return None
*/
static void    ad_Timer_0_Delay(u32 StartingCount, u32 Delay)
{
#if 0

    u32 endtime;

    do
    {
        endtime = bios_GetTimer_0();
        if(endtime <= StartingCount)
        {
            //This means timer 0 was reset, adjust endtime by the reset period
            endtime += (CORE_FREQ / TICKS_PER_SEC);
        }
    } while((StartingCount + Delay) > endtime);

#else

    systickdly_with_start(StartingCount, Delay);

#endif
}


/** \brief A small delay partially filled with something useful
\param useful_func - a pointer to that "something useful"
*/
void ad_MultiplexDelay(void (*useful_func)(void))
{
    u32 StartingTime;

    StartingTime = bios_GetTimer_0();

    /* do something useful. Read pushbutton interface . May be long enough to skip MuxDelay */
    if (useful_func != NULL)
    {
        useful_func();
    }

    // Now waste some time --
    // we lapsed the start time
    // Did something (potentially) useful
    // Now we burn the rest of the delay time.
    ad_Timer_0_Delay(StartingTime, ADWAIT_TC);
}

//-------------------------------------------------------------------------------------------
// Note that due to the speed of our execution (2MHz) the delay is LONGER than the one
// specified by the constant, approximately by 5us.

// #define     HALL_SENSOR_READING_DELAY_uS        (.000010)      // 10us in seconds
#define     HALL_SENSOR_READING_DELAY_uS        (.000001)      //  1us in seconds -- to get approx 1us delay
#define     HALL_SENSOR_READING_DELAY_TC        ((u32)(((cdouble_t)CORE_FREQ * HALL_SENSOR_READING_DELAY_uS) + 0.5))

static void ad_HallSensorDelay(void)
{
    u32 StartingTime;

    StartingTime = bios_GetTimer_0();

    systickdly_with_start(StartingTime, HALL_SENSOR_READING_DELAY_TC);
}

//-------------------------------------------------------------------------------------------
static  u32     ad_SamplePWMPinsState;

#define AD_PWM_PINS_1_0_MASK        (0x3u)

void    ad_SamplePWMPins(void)
{
    ad_SamplePWMPinsState = gpio_ReadGPIOB() & AD_PWM_PINS_1_0_MASK;
    ad_HallSensorDelay();
}

bool_t  ad_NoPWMTransition(void)
{
    u32     TempPort;

    TempPort = gpio_ReadGPIOB() & AD_PWM_PINS_1_0_MASK;
    return (TempPort == ad_SamplePWMPinsState);
}

//-------------------------------------------------------------------------------------------
