/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file quad.c
    \brief Driver for Low-Cost Quadrature Encoder

    CPU: NXP LPC21x4 (ARM)

    OWNER: Ernie Price

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/hal/bios/src/quad.c $
    $Date: 11/04/11 4:07p $
    $Revision: 18 $
    $Author: Arkkhasin $

    \ingroup test
*/

/* (Optional) $History: quad.c $
 * 
 * *****************  Version 18  *****************
 * User: Arkkhasin    Date: 11/04/11   Time: 4:07p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/hal/bios/src
 * TFS:8140 OS PORT upgrade
 * Headers fixed
 *
 * *****************  Version 16  *****************
 * User: Arkkhasin    Date: 1/28/11    Time: 1:47p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * Undo TFS:5505
 *
 * *****************  Version 14  *****************
 * User: Anatoly Podpaly Date: 11/09/10   Time: 12:48p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * Bug 4651 - moved EXT INT mapp of control registers to lpc2114io.h.
 * Removed unused code.
 *
 * *****************  Version 13  *****************
 * User: Anatoly Podpaly Date: 10/28/10   Time: 6:35p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * Bug 4475 - eliminated LINT warning.
 *
 * *****************  Version 12  *****************
 * User: Anatoly Podpaly Date: 9/03/10    Time: 2:24p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * Bug 3956: corrected the code to eliminate the attempted casting of
 * volatile away.
 *
 * *****************  Version 11  *****************
 * User: Anatoly Podpaly Date: 8/27/10    Time: 4:38p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * LINT warnings - removed unneccessary var inits.
 *
 * *****************  Version 10  *****************
 * User: Anatoly Podpaly Date: 5/26/10    Time: 5:59p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * Req ##s corrected.
 *
 * *****************  Version 9  *****************
 * User: Anatoly Podpaly Date: 5/21/10    Time: 5:54p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * Added comments to indicate and reference the LCX LUI requirements. The
 * requirements are listed in an appropriate document.
 *
 * *****************  Version 8  *****************
 * User: Anatoly Podpaly Date: 5/21/10    Time: 11:38a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * 3263 - also removed clearing of an interrupt - because of possibility
 * to clear other interrupt.
 *
 * *****************  Version 7  *****************
 * User: Anatoly Podpaly Date: 4/28/10    Time: 5:32p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * Properly removed obsoleted code.
 *
 * *****************  Version 6  *****************
 * User: Anatoly Podpaly Date: 4/28/10    Time: 5:27p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * Bug 3088: Enable Rotary Encoder interrupts only when they are needed.
 * Disable them otherwise.
 *
 * *****************  Version 5  *****************
 * User: Anatoly Podpaly Date: 4/01/10    Time: 10:05a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * Update: enable emulation.
 *
 * *****************  Version 4  *****************
 * User: Anatoly Podpaly Date: 3/25/10    Time: 5:58p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * Bug #2888: - Changed teh STEP_LIMIT - to prevent potential overflow;
 * Function GetValue is pure (no side effects); function ClearValue
 * eliminated; new function ReadAndClearValue; eliminated static vars
 * inside functions.
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 3/08/10    Time: 5:40p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 *         - remove duplicate equiv() - it's in numutils.h
 *
 * *****************  Version 2  *****************
 * User: Anatoly Podpaly Date: 2/09/10    Time: 11:55a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * LCX development - placed ENcoder driver, corrected the direction.
 *
 * *****************  Version 1  *****************
 * User: Anatoly Podpaly Date: 2/01/10    Time: 4:28p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hal/bios/src
 * LCX implementation : Rotary Encoder handler.
 *
*/

//-------------------------------------------------------------
//
// The code in this file implements a sum of requirements:
/** REQ 31-004; 31-030; 31-031; 31-032 */

#include "mnwrap.h"
#include "oswrap.h"
#include "param.h"
#include "lpc2114io.h"
#include "bsp.h"
#include "quad.h"
#include "bios.h"
#include "bios_def.h"
#include "numutils.h"

#define MASK_GEN(x) (1u << (x))
#define QPA_PIN_MSK  MASK_GEN(QPA_PIN)
#define QPB_PIN_MSK  MASK_GEN(QPB_PIN)
#define QPA_FCT     3u                          // phase A extint == fct 3
#define QPB_FCT     3u                          // phase B extint == fct 3
#define GPIO_FCT    0u                          // GPIO fct for both A & B

#define QPA_INTNO   2u                          // VIC interrupt number phase A
#define QPB_INTNO   3u                          //  "                    "    B
#define QPA_INT_MSK MASK_GEN(QPA_INTNO)
#define QPB_INT_MSK MASK_GEN(QPB_INTNO)
#define PIN_FCT_COUNT (16u)
#define PIN_FCT_MASK  (PIN_FCT_COUNT - 1u)
#define FCT_RST_MSK  (3u)

#define PERCENY_PT25 .00025
#define MOVE_BASE ((u32)((cdouble_t)STANDARD_100 * PERCENY_PT25))

#define ACCELERATION_FACTOR        (10u)       // max acceleration factor
#define ACCELERATION_MAX_INTERVAL  (100u)       // pulses >= 100 ms apart are AF 1
#define ACCELERATION_DIVISOR (ACCELERATION_MAX_INTERVAL / ACCELERATION_FACTOR)

// ---------------------------------------------------------
// Reduce step limit to make the increments more realistic and protect the
// step counter from potential overflow.
//
// #define STEP_LIMIT (1 << 17)
#define STEP_LIMIT (1 << 12u)

static s32 step;    // accumulator for QE pulses

//  While this function can be used with constants or variables for 'pin' and 'fct',
//  it is most efficient when used with constants.

MN_INLINE void SetPinFunction(u32 pin, u32 fct)
{
    u32     val;

    val = (pin < PIN_FCT_COUNT) ? PINSEL0 : PINSEL1;

    if (fct != FCT_RST_MSK)
    {
        val &= ~(FCT_RST_MSK << ((pin & PIN_FCT_MASK) << 1u));
    }
    if (fct != 0u)
    {
        val |=  (fct << ((pin & PIN_FCT_MASK) << 1u));
    }

    if (pin < PIN_FCT_COUNT)
    {
        PINSEL0 = val;
    }
    else
    {
        PINSEL1 = val;
    }
}

/** Routine to:
        1. setup specified EXTINT (intMask1) to interrupt on the oppsite state
           of the specified pin (pinMask)
        2. compute the direction based on the current state of pin (pinMask)
           and the pre-interrupt state of the complementary EXTINT (intMask2)

    \param pinMask - a single bit mask selecting the GPIO pin assigned to EXTINT intMask1
    \param intMask1 - the EXTINT associated with GPIO pin pinMask
    \param intMask2 - the EXTINT that is complementary to intMask1
    \return a flag derived from the state of GPIO(pinmask) and EXTINT(intMask2)
            indicating rotational direction. Has opposite meaning depending upon
            caller - but caller knows what to do
*/

static bool_t SetPolarityGetDirection(u32 pinMask, u32 intMask1, u32 intMask2)
{
    u32 pinpol;
    u32 pol = EXT->EXTPOLAR & ~intMask1;    // read current state of POLARITY

    pinpol = IO0PIN & pinMask;              // read current state of pin
    if (pinpol == 0u)
    {
        pol |= intMask1;
    }
    EXT->EXTPOLAR = pol;                    // set new polarity = opposite of pin state

    return equiv((pol & intMask2) == 0u, pinpol == 0u);
}

/** \brief routine to record directional steps as reported by the interrupt routines

    \param direction - flag indicating step direction: true=clockwise; false = ccw
*/
static void RecordStep(bool_t direction, u32 *timeLast)
{
    u32 timeNow = bios_GetTimer0Ticker(), ticks;

    ticks     = timeNow - *timeLast;    // ticks since last pulse on this phase
    *timeLast = timeNow;

    // compute accelerated pulses based on time interval
    ticks     = MOVE_BASE * (ACCELERATION_FACTOR - MIN(ACCELERATION_FACTOR - 1, ticks / ACCELERATION_DIVISOR));

    if (direction)
    {
        s32 temp = step + (s32)ticks;
        step = MIN(temp, STEP_LIMIT);      // accumulate CW pulses

    }
    else
    {
        s32 temp = step - (s32)ticks;
        step = MAX(temp, -STEP_LIMIT);      // accumulate CCW pulses
    }
}

void    HART_EmulationSetSteps(s16 EmulatedSteps)
{
    step = (s32)EmulatedSteps;
}

/** \brief Return the current encoder count value
*/
s16 quad_GetValue(void)
{
    s16 ThisStep;

    ThisStep = (s16)step;
    return ThisStep;
}

/** \brief Return the current encoder count value and clear the value
*/
s16 quad_ReadAndClearValue(void)
{
    s16 ThisStep;

    MN_ENTER_CRITICAL();
        ThisStep = (s16)step;
        step     = 0;
    MN_EXIT_CRITICAL();

    return ThisStep;
}

/** \brief routine to handle quadrature encoder phase A interrupts
*/

static u32 timeLast_A;

static MN_IRQ void QuadraturePhaseA(void)
{
    bool_t direction;

    SetPinFunction(QPB_PIN, GPIO_FCT);       // disable the other EXTINT
    direction =                              // get direction while setimeNowing up
        SetPolarityGetDirection(QPB_PIN_MSK, //   the other EXTINT
                                QPB_INT_MSK,
                                QPA_INT_MSK);
    SetPinFunction(QPB_PIN, QPB_FCT);        // enable the other EXTINT
    EXT->EXTINT = QPB_INT_MSK;               // clear any pending interrupts on it

    RecordStep(!direction, &timeLast_A);

    SetPinFunction(QPA_PIN, GPIO_FCT);       // disable interrupt on this EXTINT
    EXT->EXTINT = QPA_INT_MSK;               //  and clear its pending interrupt

    VICVectAddr = 0;                         // clear VIC interrupt
}

/** \brief routine to handle quadrature encoder phase B interrupts
*/

static u32 timeLast_B;

static MN_IRQ void QuadraturePhaseB(void)
{
    bool_t direction;

    SetPinFunction(QPA_PIN, GPIO_FCT);       // disable the other EXTINT
    direction =                              // get direction while setimeNowing up
        SetPolarityGetDirection(QPA_PIN_MSK, //   the other EXTINT
                                QPA_INT_MSK,
                                QPB_INT_MSK);
    SetPinFunction(QPA_PIN, QPA_FCT);        // enable the other EXTINT
    EXT->EXTINT = QPA_INT_MSK;               // clear any pending interrupts on it

    RecordStep(direction, &timeLast_B);

    SetPinFunction(QPB_PIN, GPIO_FCT);       // disable interrupt on this EXTINT
    EXT->EXTINT = QPB_INT_MSK;               //  and clear its pending interrupt

    VICVectAddr = 0;                         // clear VIC interrupt
}

/*******************************************************************************
*/

/** \brief Enable RE intrs on Phase A
    The driver switches QPA and QPB intrs during run-time ops
*/
/** REQ 31-32 - Enable Intrs */
void quad_EnableInt(void)
{
    MN_ENTER_CRITICAL();
        SetPinFunction(QPA_PIN, QPA_FCT);             //   and put it in EXTINT mode
        EXT->EXTINT = QPA_INT_MSK;                    // clear any pending PHA interrupts
        // VICVectAddr = 0;                         // clear VIC interrupt
    MN_EXIT_CRITICAL();
}

/** \brief Disable the RE interrupts
*/
/** REQ 31-32 - Disable Intrs */
void quad_DisableInt(void)
{
    MN_ENTER_CRITICAL();
        SetPinFunction(QPA_PIN, GPIO_FCT);       // disable the EXTINT
        EXT->EXTINT = QPA_INT_MSK;               //  and clear its pending interrupt
        SetPinFunction(QPB_PIN, GPIO_FCT);       // disable the EXTINT
        EXT->EXTINT = QPB_INT_MSK;               //  and clear its pending interrupt
    MN_EXIT_CRITICAL();
}

/** \brief Initialize uC H/W for the Rotary Encoder operation
*/
void quad_Initialize(void)
{
    step = 0;

    bsp_InitInterrupt(VIC_IRQ_PRIO_5, VIC_EINT2, (void(*)())QuadraturePhaseA);
    bsp_InitInterrupt(VIC_IRQ_PRIO_6, VIC_EINT3, (void(*)())QuadraturePhaseB);

    EXT->EXTMODE = QPA_INT_MSK | QPB_INT_MSK;     // make both EXTINTs edge sensitive

    (void)SetPolarityGetDirection(QPA_PIN_MSK,    // setup phase A ...
                                  QPA_INT_MSK,
                                  QPB_INT_MSK);

    /** REQ 31-32 - Disable Intrs */
    quad_DisableInt();                              // Disable intrs for now

    VICVectAddr = 0;                              // clear VIC interrupt, if any
}

// end of source
