/**
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file ad_l.c
    \brief Basic CRC16 functions, table-based

    Origninally based on 29.11.04 version by M.Rimkus, Mesco Engineering GmbH

     CPU: Any

    OWNER: EP                                                        //
    \n RCS: $Id: $
     $DateTime: $ $Change: $ $Author: Anatoly Podpaly $
*/

/* $History: ad_l.c $
 * 
 * *****************  Version 54  *****************
 * User: Anatoly Podpaly Date: 9/08/11    Time: 4:21p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * TFS:7553 - Add code to Enable / Disable interrupts to resolve the issue
 * with Hall sensor. The issue was resolved by Jonathan and Ernie. The
 * resolution is impelmented. This is the port of teh fix similar to
 * TFS:7053.
 * 
 * *****************  Version 53  *****************
 * User: Anatoly Podpaly Date: 7/07/11    Time: 12:26p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * Reverted to the revision 50. Rolled resolution for the TFS:6817 back.
 * 
 * *****************  Version 50  *****************
 * User: Anatoly Podpaly Date: 12/20/10   Time: 5:52p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * TFS:5080 - added code to protect the AD channel table. The table is
 * build and checkummed. The operations on the table (index) are also
 * checksum protected. ADded function to periodically test tyhe table.
 * 
 * *****************  Version 49  *****************
 * User: Arkkhasin    Date: 12/05/10   Time: 10:58a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * TFS:4963 (lint consistency)
 *
 * *****************  Version 48  *****************
 * User: Anatoly Podpaly Date: 11/02/10   Time: 4:16p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * Bug 4425 - LINT: for irregularity corrected.
 *
 * *****************  Version 47  *****************
 * User: Anatoly Podpaly Date: 10/12/10   Time: 2:23p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * Bug 4359 - Added definition for the Platform Hardware. Added
 * conditional compilation based on the type of the Platform H/W.
 *
 * *****************  Version 46  *****************
 * User: Justin Shriver Date: 9/16/10    Time: 10:33a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * TFS:4124 Atmospheric Pressure
 *
 * *****************  Version 45  *****************
 * User: Anatoly Podpaly Date: 9/09/10    Time: 12:18p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * Added Power Control.
 *
 * *****************  Version 44  *****************
 * User: Anatoly Podpaly Date: 9/09/10    Time: 11:38a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * Updates for Real LCX H/W
 *
 * *****************  Version 43  *****************
 * User: Anatoly Podpaly Date: 8/27/10    Time: 1:44p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/framework/bios/project_LCX
 * Added history.
 * Added MUX controls for Actual LCX H/W.
 *
*/

#include "mnwrap.h"
#include "oswrap.h"
#include "bios_def.h"
#include "lpc2114io.h"
#include "adtype.h"
#include "ad.h"
#include "adtable.h"
#include "da.h"
#include "key.h"
//#include "bsp.h"

#include "oswrap.h"
#include "mnassert.h"
#include "bios.h"

#include "timebase.h"
#define HOLD_OFF    0
#if HOLD_OFF
 #include "timebase.h"
 #include "watchdog.h"
#endif

//----------------------------------------------------------------------------------
// Disable / Enable interrupts to correct Hall sensor issue

#define MN_DisableInterrupts()                  (__set_CPSR(__get_CPSR() | CPU_NO_INTERRUPTS))
#define MN_EnableInterruptsUnconditionially()   (__set_CPSR(__get_CPSR() & ~CPU_NO_INTERRUPTS))
//----------------------------------------------------------------------------------

/*******************************************************************************
*** internal defines
*******************************************************************************/
#define AD_TIMING_KLUDGE 0

#if     PLATFORM_HARDWARE == LCX_PROTOTYPE_HARDWARE
#define P_OFF (GPIO0_P_POS_INT)   /* Default is all power switches are off */
#else
#define P_OFF (GPIO0_P_POS_INT | GPIO0_P_PRESS)   /* Default is all power switches are off */
#endif

//--------------------------------------------------------------------------------
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
//--------------------------------------------------------------------------------
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
//--------------------------------------------------------------------------------

static u8 channel;

//------------------------------------------------------------------------
// Type for creating the Measurement table.

#define     MEASURE_SEQ_TABLE_SIZE      (50)

// Typedef for the measurement table
typedef struct measureseq_t
{
    u8      seq[MEASURE_SEQ_TABLE_SIZE];                // !< The Measurement sequence
    u32     seqindex;                                   // !< Index to the table
    u16     CheckWord;                                  // !< Checksum

} measureseq_t;

//------------------------------------------------------------------------
// Var for the table
static measureseq_t measureseq;

//--------------------------------------------------------------------------------
#if     PLATFORM_HARDWARE == LCX_PROTOTYPE_HARDWARE

/* Multiplexer settings to select analog input channels */
/* ADC_MUX[2:0], PRESS_MUX[1:0], TEMP_REF_MUX */
#define MUX_PRESS_PILOT    (u32)(0)
#define MUX_PRESS_1        (GPIO0_ADC_MUX0 )
#define MUX_PRESS_ATMOS    (GPIO0_ADC_MUX0 )
#define MUX_PRESS_3        (GPIO0_ADC_MUX0 )
#define MUX_PRESS_2        (GPIO0_ADC_MUX0 )
#define MUX_POS_INT        (GPIO0_ADC_MUX1 )
#define MUX_AI_SP          (GPIO0_ADC_MUX1 | GPIO0_ADC_MUX0 )
#define MUX_IP_CUR         (GPIO0_ADC_MUX2 )
#define MUX_AI_PV          (GPIO0_ADC_MUX2 | GPIO0_ADC_MUX0 )
#define MUX_POS_REM        (GPIO0_ADC_MUX2 | GPIO0_ADC_MUX1 )
#define MUX_TEMP_INT       (GPIO0_ADC_MUX2 | GPIO0_ADC_MUX1 | GPIO0_ADC_MUX0 )
#define MUX_AD_DIAG        (GPIO0_ADC_MUX2 | GPIO0_ADC_MUX1 | GPIO0_ADC_MUX0 | GPIO0_TEMP_REF_MUX )

#define MUX_POWER_DOWN  (GPIO0_ADC_MUX2 | GPIO0_ADC_MUX1 | GPIO0_ADC_MUX0 |  GPIO0_TEMP_REF_MUX)

#define SCALE_SHFT  14u
#define FACTOR      (1u << SCALE_SHFT)

#if HOLD_OFF
 // The following to hold off microprocessor initialize until a nominal 3.15 milliamps.
 //
 #define MA_TO_RUN       3.15
 #define VOLTS_PER_MA    .1
 #define A_TO_D_RANGE    2.5
 #define A_TO_D_COUNT    65535.0
 #define ENOUGH_POWER (((MA_TO_RUN * VOLTS_PER_MA) / A_TO_D_RANGE) * A_TO_D_COUNT)
// Poll interval for initialize check
 #define WAIT_MS         25u
 #define WAIT_TIME ((XTALFREQ / (VPRATE * 1000u)) * WAIT_MS)
#endif

#define LAST_IN_SEQ     (u8)0x20
#define LAST_IN_LIST    (u8)0x40
//#define CH_MASK         0xf
#define CH_NONE         0xf

AdDataModule_t AdData;
static void AcquireCurrentAdChannel(void);

/* Table with multiplexer for all A/D input channels */
/* not used Power switches are turned to ON */
static const u32 MuxSelectTable[NUMBER_OF_AD_CHANNELS] =
{
    (MUX_AI_SP       | P_OFF),
    (MUX_AI_PV       | P_OFF),
    (MUX_TEMP_INT    | P_OFF),
    (MUX_PRESS_ATMOS | GPIO0_P_POS_INT),
    (MUX_POS_REM     | GPIO0_P_POS_INT ),
    (MUX_PRESS_1     | GPIO0_P_POS_INT),
    (MUX_POS_INT     ),
    (MUX_PRESS_3     | GPIO0_P_POS_INT),
    (MUX_PRESS_2     | GPIO0_P_POS_INT),
    (MUX_PRESS_PILOT | P_OFF),
    (MUX_IP_CUR      | P_OFF),
    (MUX_AD_DIAG     | P_OFF)};

/* Table with power settings in accordance to input channels */
/* Table is used to activate (clear) power supply bits */
static const u32 PowerSelectTable[NUMBER_OF_AD_CHANNELS] =
{
    0,                      /* AI_SP no power required */
    0,                      /* AI_PV no power required */
    0,                      /* Temp_INT no power required */
    0,                      /* Press_atmos */
    0,                      /* Pos_rem */
    0,                      /* Press_1 */
    GPIO0_P_POS_INT,        /* Pos_Int */
    0,                      /* Press_3 */
    0,                      /* Press_2 */
    0,                      /* Press_Pilot no power required */
    0,                      /* Ip_cur no power required */
    0                       /* Ad_Diag no power required */
};

#define VAR_SAMPLES 1           // enable selectable no of samples / sensor
#if !VAR_SAMPLES
#define SAMPLES     1u          // no of A/D samples to take
#endif

#if VAR_SAMPLES
#define SAMPLE_CT(xx)    {(xx) - 1u, FACTOR / (xx)}
#define ONE_SAMPLE 1u
static const struct
{
    u32     count, factor;
} sampleTbl[] =
{
    //lint -save -e778  constant expression evaluates to 0
    SAMPLE_CT(ONE_SAMPLE),          // MUX_AI_SP
    SAMPLE_CT(ONE_SAMPLE),          // MUX_AI_PV
    SAMPLE_CT(ONE_SAMPLE),          // MUX_TEMP_INT
    SAMPLE_CT(ONE_SAMPLE),          // MUX_PRESS_ATMOS
    SAMPLE_CT(3u),                  // MUX_POS_REM
    SAMPLE_CT(ONE_SAMPLE),          // MUX_PRESS_1
    SAMPLE_CT(3u),                  // MUX_POS_INT
    SAMPLE_CT(ONE_SAMPLE),          // MUX_PRESS_3
    SAMPLE_CT(ONE_SAMPLE),          // MUX_PRESS_2
    SAMPLE_CT(ONE_SAMPLE),          // MUX_PRESS_PILOT
    SAMPLE_CT(ONE_SAMPLE),          // MUX_IP_CUR
    SAMPLE_CT(ONE_SAMPLE)           // MUX_AD_DIAG
    //lint -restore
};
#endif

//--------------------------------------------------------------------------------

#else

/* Multiplexer settings to select analog input channels */
/* ADC_MUX[2:0], PRESS_MUX[1:0], TEMP_REF_MUX */
#define MUX_PRESS_PILOT     ((u32)(0))
#define MUX_PRESS_ATMOS     (GPIO0_ADC_MUX0 )
#define MUX_POS_INT         (GPIO0_ADC_MUX1 )
#define MUX_AI_SP           (GPIO0_ADC_MUX1 | GPIO0_ADC_MUX0 )
#define MUX_IP_CUR          (GPIO0_ADC_MUX2 )
#define MUX_AD_DIAG         (GPIO0_ADC_MUX2 | GPIO0_ADC_MUX1 )
#define MUX_TEMP_INT        (GPIO0_ADC_MUX2 | GPIO0_ADC_MUX1 | GPIO0_ADC_MUX0 )

//--------------------------------------------------------------------
// Note: These defines are for the BUILD compatibility only
// The unsed AD channels are routed to channel 5
#define MUX_PRESS_1         (GPIO0_ADC_MUX2 | GPIO0_ADC_MUX0 )
#define MUX_PRESS_2         (GPIO0_ADC_MUX2 | GPIO0_ADC_MUX0 )
#define MUX_PRESS_3         (GPIO0_ADC_MUX2 | GPIO0_ADC_MUX0 )
#define MUX_POS_REM         (GPIO0_ADC_MUX2 | GPIO0_ADC_MUX0 )
#define MUX_AI_PV           (GPIO0_ADC_MUX2 | GPIO0_ADC_MUX0 )

//--------------------------------------------------------------------

#define MUX_POWER_DOWN  (GPIO0_ADC_MUX2 | GPIO0_ADC_MUX1 | GPIO0_ADC_MUX0 )

#define SCALE_SHFT  14u
#define FACTOR      (1u << SCALE_SHFT)


#if HOLD_OFF
 // The following to hold off microprocessor initialize until a nominal 3.15 milliamps.
 //
 #define MA_TO_RUN       3.15
 #define VOLTS_PER_MA    .1
 #define A_TO_D_RANGE    2.5
 #define A_TO_D_COUNT    65535.0
 #define ENOUGH_POWER (((MA_TO_RUN * VOLTS_PER_MA) / A_TO_D_RANGE) * A_TO_D_COUNT)
// Poll interval for initialize check
 #define WAIT_MS         25u
 #define WAIT_TIME ((XTALFREQ / (VPRATE * 1000u)) * WAIT_MS)
#endif

#define LAST_IN_SEQ     (u8)0x20
#define LAST_IN_LIST    (u8)0x40
//#define CH_MASK         0xf
#define CH_NONE         0xf

AdDataModule_t AdData;
static void AcquireCurrentAdChannel(void);

/* Table with multiplexer for all A/D input channels */
/* not used Power switches are turned to ON */
static const u32 MuxSelectTable[NUMBER_OF_AD_CHANNELS] =
{
    (MUX_AI_SP       | P_OFF),
    (MUX_AI_PV       | P_OFF),
    (MUX_TEMP_INT    | P_OFF),
    (MUX_PRESS_ATMOS | GPIO0_P_POS_INT),
    (MUX_POS_REM     | P_OFF),
    (MUX_PRESS_1     | P_OFF),
    (MUX_POS_INT     | GPIO0_P_PRESS),
    (MUX_PRESS_3     | P_OFF),
    (MUX_PRESS_2     | P_OFF),
    (MUX_PRESS_PILOT | P_OFF),
    (MUX_IP_CUR      | P_OFF),
    (MUX_AD_DIAG     | P_OFF)};

/* Table with power settings in accordance to input channels */
/* Table is used to activate (clear) power supply bits */
static const u32 PowerSelectTable[NUMBER_OF_AD_CHANNELS] =
{
    0,                      /* AI_SP no power required */
    0,                      /* AI_PV no power required */
    0,                      /* Temp_INT no power required */
    GPIO0_P_PRESS,          /* Press_atmos */
    0,                      /* Pos_rem */
    0,                      /* Press_1 */
    GPIO0_P_POS_INT,        /* Pos_Int */
    0,                      /* Press_3 */
    0,                      /* Press_2 */
    0,                      /* Press_Pilot no power required */
    0,                      /* Ip_cur no power required */
    0                       /* Ad_Diag no power required */
};

#define VAR_SAMPLES 1           // enable selectable no of samples / sensor
#if !VAR_SAMPLES
#define SAMPLES     1u          // no of A/D samples to take
#endif

#if VAR_SAMPLES
#define SAMPLE_CT(xx)    {(xx) - 1u, FACTOR / (xx)}
#define ONE_SAMPLE 1u
static const struct
{
    u32     count, factor;
} sampleTbl[] =
{
    //lint -save -e778  constant expression evaluates to 0
    SAMPLE_CT(ONE_SAMPLE),          // MUX_AI_SP
    SAMPLE_CT(ONE_SAMPLE),          // MUX_AI_PV
    SAMPLE_CT(ONE_SAMPLE),          // MUX_TEMP_INT
    SAMPLE_CT(ONE_SAMPLE),          // MUX_PRESS_ATMOS
    SAMPLE_CT(3u),                  // MUX_POS_REM
    SAMPLE_CT(ONE_SAMPLE),          // MUX_PRESS_1
    SAMPLE_CT(3u),                  // MUX_POS_INT
    SAMPLE_CT(ONE_SAMPLE),          // MUX_PRESS_3
    SAMPLE_CT(ONE_SAMPLE),          // MUX_PRESS_2
    SAMPLE_CT(ONE_SAMPLE),          // MUX_PRESS_PILOT
    SAMPLE_CT(ONE_SAMPLE),          // MUX_IP_CUR
    SAMPLE_CT(ONE_SAMPLE)           // MUX_AD_DIAG
    //lint -restore
};
#endif

//--------------------------------------------------------------------------------
#endif      // Real LCX

/** \brief Initialize the A/D conversion vasriables. SPI initialization is done in DA.c

  \param[in] none
  \return none
*/
void bios_InitAd(void)
{
    AdData.channel = AD_DIAG ;      /* assume channel is AD_DIAG */
    AdData.LastChannel = false ;
}

/** \brief Returns a pointer to the raw A/D data

  \param[in] none
  \return AdDataRaw_t - pointer to the raw data
*/
const AdDataRaw_t *bios_GetAdRawData(void)
{
    return(&AdData.AdRaw);
}

/** \brief initiate a conversion Transfer the most significant 8 bits of an A/D value

  \param[in] none
  \return u32 the MSB as a 32-bit value
*/
static u32 firstbyte(bool_t newch)
{
    GPIO1->IOSET = GPIO1_CS_AD;         // deselect to convert A/D value
    GPIO1->IOCLR = GPIO1_CS_AD;         // select to transfer A/D value
    SPI1->SPDR = 0;                     // Start transfer of MSB via SPI1
    if (newch)
    {
        channel  = AdData.channel;
        AcquireCurrentAdChannel(); /* select channel, set mulpiplexer and power */
    }
    while (SPIF != (SPI1->SPSR & SPIF)) /* wait for end of transmission */
    {
    }
    return (u32)SPI1->SPDR;
}

/** \brief Transfer the least significant 8 bits of an A/D value

  \param[in] none
  \return u32 - the LSB as a 32-bit value
*/
static u32 secondbyte(void)
{
    SPI1->SPDR = 0; /* Start reading MSBs of A/D via SPI1 (87us until result is available) */
    while (SPIF != (SPI1->SPSR & SPIF)) /* wait for end of transmission */
    {
    }
    return (u32)SPI1->SPDR;
}

/** \brief the microprocessor startup strategy - does not return until the loop current is sufficient
    to successfully start.

  \param[in] none
  \return none
*/
void bios_WaitPowerOK(void)
{
#if HOLD_OFF
    u32 temp2;

    for (;;)
    {
        Sio1Select();                                           /* Powerup SIO1 */
        GPIO0->IOSET = MuxSelectTable[AD_AI_SP];
        GPIO1->IOCLR = GPIO1_CS_AD;                             /* prepare A/D conversion */
        temp2        = firstbyte() << SHIFT8;
        temp2       += secondbyte();
        GPIO1->IOSET = GPIO1_CS_AD;                             /* Deselect A/D */
        GPIO0->IOCLR = MUX_POWER_DOWN ;
        Sio1Deselect();
        if (temp2 >=(u32)ENOUGH_POWER)
        {
            break;
        }
        temp2 = T1TC;
        while ((T1TC - temp2) < WAIT_TIME)
        {
        }
        wdog_WatchdogTrigger();
    }
#endif
}

/** \brief Convert the passed measure sequence into a byte stream for fast
    access to the A/D channel sequence

  \param[in] measure_sequence_t *m_arr - the measure sequence structure
  \return none
*/
void bios_WriteMeasureSequence(const measure_sequence_t *m_arr)
{
    const measure_sequence_t *p2;
    measure_sequence_t flag = 0;
    u32 x = 0;
    u32 y = 0;
    u32 z;

    MN_ASSERT(m_arr != NULL);
    p2 = m_arr;

    MN_ENTER_CRITICAL();

        // scan for frequency 1 and add to array
        while ((flag & AD_FREQ_END)==0)
        {
            if( ((flag = *m_arr) & AD_FREQ_1) != 0)
            {
                measureseq.seq[x++] = flag & CH_MASK;
            }
            m_arr++;
        }
        y = x;
        // scan for frequency 2. Add a copy of frequency 1 data and each frequency 2
        flag = 0;
        while ((flag & AD_FREQ_END) == 0)
        {
            if ( ((flag = *p2) & AD_FREQ_2) != 0)
            {
                if (y != x)
                {
                    for (z = 0; z < x; z++)
                    {
                        measureseq.seq[y++] = measureseq.seq[z];
                    }
                }
                measureseq.seq[y++] = (flag & CH_MASK) | LAST_IN_SEQ;
            }
            p2++;
        }

        MN_ASSERT(y <= MEASURE_SEQ_TABLE_SIZE);
        measureseq.seq[y - 1u] |= (LAST_IN_LIST | LAST_IN_SEQ);

        // Fill the rest of the table with 0-s
        while (y < MEASURE_SEQ_TABLE_SIZE)
        {
            measureseq.seq[y] = 0;
            y++;
        }

        measureseq.seqindex = 0;

        STRUCT_CLOSE(measureseq_t, &measureseq);

    MN_EXIT_CRITICAL();
}

/** \brief Get the next A/D channel to be converted

  \param[in] none
  \return none
*/
static void AcquireCurrentAdChannel(void)
{
    static u8 chan = 0;
    u32     temp_seq_index;

    if ((chan & (LAST_IN_SEQ | LAST_IN_LIST)) != 0)
    {
        if ((chan & LAST_IN_LIST) != 0)
        {
            MN_ENTER_CRITICAL();
                storeMemberInt(&measureseq, seqindex, 0);
            MN_EXIT_CRITICAL();
        }
        AdData.LastChannel = true;
        chan = CH_NONE;
        GPIO0->IOCLR   = MUX_POWER_DOWN ;
        GPIO0->IOSET   = P_OFF;         /* switch all power supplies to OFF */
    }
    else
    {
        u8  oldChan = AdData.channel;

        MN_ENTER_CRITICAL();
            temp_seq_index = measureseq.seqindex;
            AdData.channel = (chan = measureseq.seq[temp_seq_index]) & CH_MASK;
            temp_seq_index++;

            storeMemberInt(&measureseq, seqindex, temp_seq_index);
        MN_EXIT_CRITICAL();

        // if we are just about to power the Hall sensor, disable interrupts
        if (AdData.channel == AD_POS_INT)          
        {
            MN_DisableInterrupts();
        }

        GPIO0->IOCLR = (MUX_POWER_DOWN | PowerSelectTable[AdData.channel]);
        GPIO0->IOSET = MuxSelectTable[AdData.channel] ;

        // if we just powered off the Hall sensor, enable interrupts
        if (oldChan == AD_POS_INT) 
        {
            MN_EnableInterruptsUnconditionially();
        }
    }
}

/** \brief The LCD is in use so we cannot poll the keyboard.  We must, however,
    delay here for the A/D multiplexer to settle.

  \param[in] none
  \return none

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

static void MultiplexDelay(void)
{
    u32 endtime, rawtime = T0->TC;

    do
    {
        endtime = T0->TC;
        if(endtime <= rawtime)
        {
            //This means timer 0 was reset, adjust endtime by the reset period
            endtime += PCLKFREQ / LCL_TICKS_PER_SEC;
        }
    } while((rawtime + ADWAIT_TC) > endtime);
}

#ifndef NDEBUG  //start "moving window" smoothing
#if 0
        static u16 avgPilotI;
        static u16 avgIParr[10];
#endif //if 0
#endif //NDEBUG   end "moving window" smoothing

/** \brief Do one 15 ms A/D measure sequence

  \param[in] none
  \return none
*/
void bios_MeasureAd(void)
{
    u32 spi_temp, temp2;
#if VAR_SAMPLES
    u32 kt;
#endif
    /* measure selected A/D channels + pushbuttons */
    /* select channel and set multiplexer and power of pressure sensor (first action) */
    AcquireCurrentAdChannel();

    Sio1Select();                   /* Powerup SIO1 */

    bios_HandleDigitalInput();      /* do something useful. Read pushbutton interface */

    MultiplexDelay();               /* settling time */

    /* Wait until power and inputs are ready for conversion (100us) */
    GPIO1->IOCLR = GPIO1_CS_AD ;    /* prepare A/D conversion */
    /* now we start with measure loop *******************************************************/
    /* with call of AcquireCurrentAdChannel() the LastChannel marker is calculated */
    while (false == AdData.LastChannel)
    {
        temp2    = 0;
#if VAR_SAMPLES
        kt = sampleTbl[AdData.channel].count;
        spi_temp = firstbyte((kt == 0u) ? true : false) << SHIFT8;
        for (; kt-- > 0u;)
        {
            temp2   += secondbyte() | spi_temp;
            spi_temp = firstbyte((kt == 0u) ? true : false) << SHIFT8;
        }

        temp2   += secondbyte() | spi_temp;
        // now divide by the number of samples
        //lint -save -e734  loss of precision is OK
        AdData.AdRaw.AdValue[channel] = (temp2 * sampleTbl[channel].factor) >> SCALE_SHFT;
        //lint -restore
#ifndef NDEBUG  //start "moving window" smoothing
#if 0
        u16 thisIPval;
        s8  i;

        if ( channel == AD_IP_CUR )
        {
            thisIPval = AdData.AdRaw.AdValue[channel];

            for ( i = 0; i < 9; i++ )
            {
                avgIParr[i] = avgIParr[i+1];
            }
            avgIParr[9] = thisIPval;
            avgPilotI = 0;
            for (i = 0; i < 10; i++ )
            {
                avgPilotI += avgIParr[i];
            }
            avgPilotI /= 10;
            AdData.AdRaw.AdValue[channel] = avgPilotI;
        }
#endif //if 0
#endif //NDEBUG   end "moving window" smoothing

#else //VAR_SAMPLES
        spi_temp = firstbyte(SAMPLES == 1u) << SHIFT8;
 #if SAMPLES > 1u
        temp2   += secondbyte() | spi_temp;
        spi_temp = firstbyte(SAMPLES == 2u) << SHIFT8;
 #endif
 #if SAMPLES > 2u
        temp2   += secondbyte() | spi_temp;
        spi_temp = firstbyte(SAMPLES == 3u) << SHIFT8;
 #endif
 #if SAMPLES > 3u
        temp2   += secondbyte() | spi_temp;
        spi_temp = firstbyte(SAMPLES == 4u) << SHIFT8;
 #endif
 #if SAMPLES > 4u
        temp2   += secondbyte() | spi_temp;
        spi_temp = firstbyte(SAMPLES == 5u) << SHIFT8;
 #endif
        temp2   += secondbyte() | spi_temp;
        // now divide by the number of samples
        //lint -save -e734  loss of precision is OK
        AdData.AdRaw.AdValue[channel] = (temp2 * (FACTOR / SAMPLES)) >> SCALE_SHFT;
        //lint -restore
#endif //VAR_SAMPLES

#if AD_TIMING_KLUDGE
        // The following is a kludge to allow extra time for the pressure and remote sensor
        // power supplies to stabilize.  The added delay is approximately 150 micro-seconds.
        // While it is possible to turn the supplies on during the prior cycle, possible noise
        // issues as well as increased complexity are avoided by juyst adding a delay.
        // This delay amounts to about .6% for the pressure sensors and about 1.6% when
        // the remote position sensor is active.
        if ((GPIO0->IOPIN & (GPIO0_P_PRESS | GPIO0_P_POS_REM))!= (GPIO0_P_PRESS | GPIO0_P_POS_REM))
        {
            u32 kt;
            (void)secondbyte();
            for (kt = 0u; kt < 4u; kt++)
            {
                (void)GPIO0->IOPIN;
            }
        }
        // end kludge
#endif
    }
    GPIO1->IOSET = GPIO1_CS_AD ; /* Deselect A/D */

    /* all channels are measured. prepare for next call */
    AdData.LastChannel = false ;
    Sio1Deselect();
#if 0
    if (AdData.AdRaw.AdValue[AD_AI_SP] >= (u16)ENOUGH_POWER)
    {
        bios_ADPowerIsOK();
    }
#endif
}

/** \brief Periodic Measurement Table Test

  \param[in] none
  \return none
*/
void Periodic_Measurement_Table_Test(void)
{
    MN_ENTER_CRITICAL();
        Struct_Test(measureseq_t, &measureseq);
    MN_EXIT_CRITICAL();
}
//--------------------------------------------------------------


