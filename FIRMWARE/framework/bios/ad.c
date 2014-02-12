/*
Copyright 2006 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/** \addtogroup A_to_D  Analog multiplexer and A/D conversion
\brief Device driver for analog multiplexer and A/D conversion
\htmlonly
<a href="../../../Doc/DesignDocs/Physical Design_AtoD_Module.doc"> Design document </a><br>
<a href="../../../Doc/TestDocs/utplan_BIOS.doc"> Unit Test Report </a><br>
\endhtmlonly
*/
/**
    \file ad.c
    \brief Device driver for analog multiplexer and A/D conversion

    CPU: Philips LPC21x4 (ARM)

    OWNER: Ernie Price

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/framework/bios/ad.c $
    $Date: 12/15/11 3:46p $
    $Revision: 6 $
    $Author: Justin Shriver $

    \ingroup A_to_D
*/
/* $History: ad.c $
 *
 * *****************  Version 6  *****************
 * User: Justin Shriver Date: 12/15/11   Time: 3:46p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/framework/bios
 * -- AP -- COrrected gpioi and SPI use.
 *
 * *****************  Version 5  *****************
 * User: Justin Shriver Date: 12/09/11   Time: 6:07p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/framework/bios
 * -- AP -- Detected bug - corrections.
 *
 * *****************  Version 4  *****************
 * User: Justin Shriver Date: 11/30/11   Time: 3:52p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/framework/bios
 * -- AP -- LINT corrections.
 *
 * *****************  Version 3  *****************
 * User: Justin Shriver Date: 11/29/11   Time: 7:18p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/framework/bios
 * -- AP -- TFS:7889; 7899; 7893 - use SPI and GPIO module. Also - new A/D
 * code.
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 11/29/11   Time: 1:36p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/framework/bios
 * TFS:8255 A working version of A/D (still interim)
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 11/20/11   Time: 10:41p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/framework/bios
 * TFS:8255 (Interim) A version based on the trunk version. Awaiting
 * FF-compatible BIOS merge
 *
 * *****************  Version 61  *****************
 * User: Arkkhasin    Date: 2/24/10    Time: 1:04a
 * Updated in $/MNCB/Dev/FIRMWARE/framework/bios
 * 1. Power management remains conceptually intact (as it was done for
 * ESD)
 * 2. A/D sequencing changed to unrolled sequences similar to MESCO idea,
 * except that the unrolled tables are created in flash at compile time
 * 3. Superfluous static-duration variables eliminated
 *
 * *****************  Version 60  *****************
 * User: Arkkhasin    Date: 2/11/10    Time: 4:43p
 * Updated in $/MNCB/Dev/FIRMWARE/framework/bios
 * bios_HandleDigitalInput() now returns void.
 * MultiplexDelay() is called unconditionally and calls a supplied useful
 * function
 *
 * *****************  Version 59  *****************
 * User: Arkkhasin    Date: 1/29/10    Time: 2:16p
 * Updated in $/MNCB/Dev/FIRMWARE/framework/bios
 * Corrected the size of channel_list
 *
 * *****************  Version 58  *****************
 * User: Arkkhasin    Date: 8/31/09    Time: 5:15p
 * Updated in $/MNCB/Dev/FIRMWARE/framework/bios
 * Calling channel functions is now deferred until the end of A/D read
 * cycles. This addresses Ernie's concern about power consumption issues.
 *
 * *****************  Version 57  *****************
 * User: Arkkhasin    Date: 2/27/09    Time: 12:46a
 * Updated in $/MNCB/Dev/FIRMWARE/framework/bios
 * Removed unused KBD_OPTION_ADC_INTERLEAVE
 *
 * *****************  Version 56  *****************
 * User: Arkkhasin    Date: 5/27/08    Time: 3:53p
 * Updated in $/MNCB/Dev/FIRMWARE/framework/bios
 * Moved LCD transfer test from the caller to bios_HandleDigitalInput()  -
 * A sequel (bios_HandleDigitalInput()  returns a non-0 iff did the
 * processing)
*/
//#include "projectdef.h"
#include "mnwrap.h"
#include "mn_instrum.h"
#include "mnassert.h"
#include "ad.h"
#include "key.h"
#include "crc.h"

#include "adhwconf.h"
#include "spi.h"

#ifndef INSTRUM_AD_SAMPLING
#define INSTRUM_AD_SAMPLING(channel, value) UNUSED_OK(channel)
#endif //INSTRUM_AD_SAMPLING


typedef struct ADSeqState_t
{
    const u8 *measureSeqBase;    //!< pointer to base of seq table (first fast channel)
    const u8 *pSeq;          //!< pointer to current channel
    u16 CheckWord;
} ADSeqState_t;

static ADSeqState_t ADSeqState;
static AdDataRaw_t AdData;

/** \brief Initialize the A/D conversion variables. SPI initialization is done in DA.c

*/
void bios_InitAd(void)
{
//    u8_least channel = 0u;
//    bool_t  ChPowered;

//    ChPowered = ad_IsChannelPowered(channel);
//    MN_DBG_ASSERT(!ChPowered);                              // If the channel POWERED
//    UNUSED_OK(ChPowered);

    ad_EndSequence();                                       // Initialize MUX
}

/** \brief Returns a pointer to the raw A/D data

    \return A pointer to the raw A/D data
*/
const AdDataRaw_t *bios_GetAdRawData(void)
{
    return(&AdData);
}

static void channel_convert(void)
{
    ad_DeSelectExtAD();                     // deselect to convert A/D value
    ad_SelectExtAD();                       // select to transfer A/D value
}

static void special_channel_convert(u8 channel)
{
    if (channel != AD_POS_INT)
    {
        channel_convert();
    }
    else
    {
        do
        {
            ad_SamplePWMPins();
            channel_convert();
        } while (!ad_NoPWMTransition());
    }
}

/** \brief Prepare the passed measure sequence for A/D conversions
The passed array is assumed to be expanded list in the following format:

- All fast channels
- First slow channel
- Stop marker
- All fast channels
- Second slow channel
- Stop marker
...
- All fast channels
- Last slow channel
- Stop-and-Reset marker which is greater than stop marker (greater than END_SEQ_TWO)

\param[in] m_arr - pointer to the measure sequence array
*/
void bios_WriteMeasureSequence(const measure_sequence_t *m_arr)
{
    ADSeqState_t s;
#ifndef NDEBUG
    const measure_sequence_t *chk = m_arr;

    for (;;)
    {
        bool_t  ChPowered;

        ChPowered = ad_IsChannelPowered(*chk);
        MN_ASSERT(!ChPowered);                     // If the channel POWERED
        while (*chk < END_SEQ_TWO)
        {
            chk++;
        }
        if (*chk > END_SEQ_TWO)
        {
            break;
        }
        chk++;
    }
#endif
    s.measureSeqBase = m_arr;     // base of table
    s.pSeq       = m_arr;     // next chan to convert
    Struct_Copy(ADSeqState_t, &ADSeqState, &s);
}

/** \brief Do one 15 ms (short) A/D measure sequence

Expects expanded channel sequence in the same fashion as bios_WriteMeasureSequence.
  \return true if end of long A/D sequence
*/
bool_t bios_MeasureAd(void)
{
    u32 temp2;
    u32 kt;
    u32 sample;

    /* This could and should move to some benign place when testing is done */
    Struct_Test(ADSeqState_t, &ADSeqState);

    /* measure selected A/D channels + pushbuttons */
    /* select channel and set multiplexer and power of pressure sensor (first action) */

    const u8 *seq = ADSeqState.pSeq;
    u8  channel = *seq; //seq is incremented on selection of next channel inside the do/while loop
    u8  lookaheadhannel = *(seq + 1u);

#if 1

    bios_HandleDigitalInput();

    ad_PrepareChannelPower(channel, lookaheadhannel);   //channel is guaranteed valid in the beginning

    spi_Sio1Select();    /* Powerup SIO1 but don't select A/D chip */

    ad_MultiplexDelay(NULL);

#else

    ad_PrepareChannelPower(channel, lookaheadhannel);   //channel is guaranteed valid in the beginning

    spi_Sio1Select();    /* Powerup SIO1 but don't select A/D chip */

    ad_MultiplexDelay(bios_HandleDigitalInput);

#endif

    /* Wait until power and inputs are ready for conversion (100us) */
    ad_SelectExtAD();                       // select to transfer A/D value

    /* now we start with measure loop *******************************************************/
    /* with call of AcquireCurrentAdChannel() the LastChannel marker is calculated */
    do
    {
        temp2 = 0;                                  // Clear the accumulator
        kt    = adhwconftab.sampleTbl[channel].count;   // Get the channel counter

        //------------------------------------------------------------------
        // Tricky part of the A/D reading operation
        // This is done to minimize the Power consumption AND to reduce the CPU load.

        // channel_convert();                          // Launch next conversion
        special_channel_convert(channel);           // Launch next conversion
        spi_StartRead_Ch1();                        // Start SPI read operation

        // If kt > 0 we do some reads here AND one more read after the loop
        // Otherwise we will do just one read.
        while (kt > 0U)
        {
            sample = spi_CompleteRead16bit_Ch1();   // Complete SPI 16-bit read operation
            INSTRUM_AD_SAMPLING(channel, sample);
            temp2 += sample;

            // channel_convert();                      // ... and start NEXT conversion
            special_channel_convert(channel);       // ... and start NEXT conversion
            spi_StartRead_Ch1();                    // ... and start another SPI read operation

            --kt;                                   // Decrement counter
        }

        u8  nextchannel = *++seq;                   // kt is 0 (or was 0) - next channel
        if (nextchannel < END_SEQ_TWO)               // Change the MUX and Power settings
        {
            lookaheadhannel = *(seq + 1u);          // Look at the next after next
            ad_PrepareChannelPower(nextchannel, lookaheadhannel);    // ...
        }
        sample = spi_CompleteRead16bit_Ch1();       // ... and complete the SPI read operation
        INSTRUM_AD_SAMPLING(channel, sample);
        temp2 += sample;

        //------------------------------------------------------------------

        /* EXPLANATION:
        The deal here is to interleave power settling time with SPI transfer of 2 bytes.
        The assumption here is that it is sufficient.
        */

        // now divide by the number of samples
        // NOTE: calculations are performed with 32-bit values but the result shall be u16 !
        AdData.AdValue[channel] = (u16)((temp2 * adhwconftab.sampleTbl[channel].factor) >> SCALE_SHFT);

        //process the value just read
        //This has a good potential of CPU time saving by interleaving with (next) SPI transfer.
        // - But code organization will be more complex
        channel = nextchannel;

    } while(channel < END_SEQ_TWO);

    /* all channels are measured. prepare for next call */
    ad_EndSequence();

    ad_DeSelectExtAD();         // De-select A/D

    spi_Sio1Deselect();

    /* Call channel functions. It is deferred to address
     Ernie's note of excessive time of powered sensors
    */
    for(const u8 *p = ADSeqState.pSeq; p != seq; p++)
    {
        u8 channel_num;
        channel_num = *p;
        if(ad_UpdateFunc[channel_num] != NULL)
        {
            ad_UpdateFunc[channel_num](AdData.AdValue[channel_num]);
        }
    }

    /* seq points to an end-of-sequence marker. If the marker is > END_SEQ_TWO
    then we are done with the whole list; otherwise just step over the stop marker
    */
    if(*seq++ > END_SEQ_TWO)
    {
        seq = ADSeqState.measureSeqBase; //wrap the sequience around
    }
    storeMemberPtr(&ADSeqState, pSeq, seq);

    return ADSeqState.measureSeqBase == seq;
}

#if 0 && (FEATURE_TERMINAL_BOARD_TEST == FEATURE_TERMINAL_BOARD_TEST_SUPPORTED)

#define MAX_LOOP_COUNT      55

#define DEBUG_AD            0
#define DO_SLOPE            0
#if DEBUG_AD
 #define MONITOR_VALS(x) (pVal[loopCounter] = (x))
u16 pVal[MAX_LOOP_COUNT];
#else
 #define MONITOR_VALS(x)
#endif

static ad_diag_t diagResult;

/** \brief ESD - Get the pointer to the terminal board diagnostic result.
    Also presets the test result to failed in case the test does not run.

  \return -  A pointer to the structure.
*/
const ad_diag_t *ad_GetTerminalDiagResult(void)
{
    // preset to failed in case the test doesn't run
    diagResult.diagSampleCount = -1;
    return &diagResult;
}

static void waste(void)
{
    /*nothing*/
}

/** \brief ESD - Perform the low-level Terminal Board Diagnostic
*/
void ad_PerformTerminalBoardDiag(void)
{
    s8_least    loopCounter = 0;
    u32         temp2;
    s32         i2p        = 0;
    s32         count      = 500;
    s32         iterations = MAX_LOOP_COUNT;

    MN_ENTER_CRITICAL();

        Sio1Select(0, SIO1_CONTROL_INIT_AD);     // Enable SIO1 as master butdon;t select A/D chip

#if DO_SLOPE
        GPIO0->IOSET = (MuxPwrTable[AD_POS_REM]  & ~POWER_BIT_MASK);
        GPIO0->IOCLR = (MuxPwrTable[AD_POS_REM]  &  POWER_BIT_MASK);
        MultiplexDelay();               // wait for mux to settle + power

        diagResult.diagCumSlope = 0;

        // GPIO1->IOCLR = GPIO1_CS_AD ;     // wake up A/D chip
        ad_SelectExtAD();                   // select to transfer A/D value

        // record slope for approx half of the ramp time
        for (loopCounter = 0; loopCounter < SLOPE_COUNT; loopCounter++)
        {
            channel_convert();
            temp2 = spi_Read16bit_Ch1();

            MONITOR_VALS((u16)(i2p - (s32)temp2));      // debug
            if (loopCounter >= SLOPE_START)
            {
                diagResult.diagCumSlope +=
                    i2p - (s32)temp2;                   // accumulate diff per step
            }
            i2p = (s32)temp2;
        }
        i2p = 0;
        GPIO0->IOCLR = GPIO0_ADC_MUX1;          // set mux to I/P current
#else
        GPIO0->IOSET = (MuxPwrTable[AD_IP_CUR]   & ~POWER_BIT_MASK);
        GPIO0->IOCLR = (MuxPwrTable[AD_POS_REM]  &  POWER_BIT_MASK);

        // GPIO1->IOCLR = GPIO1_CS_AD ;     // wake up A/D chip
        ad_SelectExtAD();                   // select to transfer A/D value

#endif
        MultiplexDelay(waste);                       // wait for mux to settle
        for (; loopCounter < iterations; loopCounter++)
        {
            // read the I/P current
            channel_convert();
            temp2 = spi_Read16bit_Ch1();
            MONITOR_VALS((u16)temp2);     // debug
            if ((i2p - (s32)temp2) >= count)
            {
                diagResult.diagSampleCount   = loopCounter;
                iterations = loopCounter + 6;
                do
                {
                    channel_convert();
                    temp2 = spi_Read16bit_Ch1();
                    MONITOR_VALS((u16)temp2);     // debug
                    loopCounter++;
                } while ((loopCounter < iterations) && ((s32)temp2 > count));
                break;
            }
            i2p = (s32)temp2;
        }

        // park the mux and turn off the diag (remote pos) power
        // GPIO1->IOSET = GPIO1_CS_AD;             // put A/D to sleep
        // ad_DeSelectExtAD();                     // select to transfer A/D value

        GPIO0->IOCLR = MUX_POWER_DOWN;          // park MUX
        GPIO0->IOSET = POWER_BIT_MASK;          // all power down

        // idle the SPI channel

#if 0 //INTEGRATE!
        Sio1Deselect(GPIO1_CS_AD);
#else
        Sio1Deselect();
#endif


    MN_EXIT_CRITICAL();
    // if did not trip, report fail
    if (loopCounter >= iterations)
    {
        diagResult.diagSampleCount = -1;
    }
}
#endif

/* This line marks the end of the source */
