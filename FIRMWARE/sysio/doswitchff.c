/*
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**

    \file doswitchff.c
    \brief DO switch support (ff flavor). Based on configure1.c and cycle.c

    CPU: Any (with standard C compiler)

    OWNER: AK
*/

#include "mnwrap.h"
#include "mnassert.h"
#include "nvram.h"
#include "utility.h"
#include "configure.h"
#include "dohwif.h"
#include "doswitch.h"

#include "fullthrottle.h"

#include "devicemode.h"
#include "faultpublic.h"

// defines for output switch handling
#define DONT_ASSERT     0u
#define DO_ASSERT       1u
#define DO_NOTHING      2u
//#define SW_DEADBAND     ((u16)INT_PERCENT_OF_RANGE(.5))


/** member data */
static DO_Passthrough_t DO_Passthrough;

static SwitchConfiguration_t   m_SwitchConfiguration;

static const SwitchConfiguration_t def_SwitchConfiguration =
{
    .swconf =
    {
        [DO_1] =
        {
            .SwitchDirection = SWITCH_NORMAL_OPEN, //default power-off state
            .SwitchType = SWITCH_DISABLE,
        },
#if DO_NUM_CHANNELS>1
        [DO_2] =
        {
            .SwitchDirection = SWITCH_NORMAL_OPEN, //default power-off state
            .SwitchType = SWITCH_DISABLE,
        },
#endif //DO_NUM_CHANNELS
    },
    CHECKFIELD_CLEAR()
};

void dosw_SetPassthroughValues(const DO_Passthrough_t *src)
{
    Struct_Copy(DO_Passthrough_t, &DO_Passthrough, src);
}

/** \brief returns the configuration of the output switches

  returns the configuration of the output switches
  \param[in] none
  \return const SwitchConfiguration_t*
*/
const void *TypeUnsafe_cnfg_GetFillSwitchConfiguration(void *dst)
{
    return STRUCT_TESTGET(&m_SwitchConfiguration, dst);
}


/** \brief Saves the configuration of the output switches

  Saves the configuration of the output switches
  \param[in] const SwitchConfiguration_t* pSwitchConfiguration - the data to be saved
  \return ErrorCode_t - 0 if no error, PARAM_OUT_OF_RANGE if invalid data
*/
ErrorCode_t     TypeUnsafe_cnfg_SetSwitchConfiguration(const void *src)
{
    const SwitchConfiguration_t* pSwitchConfiguration = src;

    if(pSwitchConfiguration == NULL)
    {
        pSwitchConfiguration = &def_SwitchConfiguration;
    }

    /** validate data */
    for(u8_least i=0; i<DO_NUM_CHANNELS; i++)
    {
        if( (pSwitchConfiguration->swconf[i].SwitchDirection > SWITCH_DIRECTION_MAX) ||
            (pSwitchConfiguration->swconf[i].SwitchType > SWITCH_TYPE_MAX) )
        {
            /* Legitimate early return: Pre-requisites check */
            return ERR_INVALID_PARAMETER;
        }
    }

#if 0
    s16_least val;
    //check limits of switch values
    val = f2i_Scaled(pSwitchConfiguration->fSwitch1Value, MIN_ALLOWED_POSITION-1, MAX_ALLOWED_POSITION+1, STD_SCALE); //allow out-of range detection
    if ((val <  SWITCH_VALUE_MIN_INT) ||
        (val >  SWITCH_VALUE_MAX_INT) ||
        (val >= MAX_ALLOWED_POSITION))
    {   // MAX to accomodate for clamping
        /* Legitimate early return: Pre-requisites check */
        return ERR_INVALID_PARAMETER;
    }

    val = f2i_Scaled(pSwitchConfiguration->fSwitch2Value, MIN_ALLOWED_POSITION-1, MAX_ALLOWED_POSITION+1, STD_SCALE); //allow out-of range detection
    if ((val <  SWITCH_VALUE_MIN_INT) ||
        (val >  SWITCH_VALUE_MAX_INT) ||
        (val >= MAX_ALLOWED_POSITION))
    {   // MAX to accomodate for clamping
        /* Legitimate early return: Pre-requisites check */
        return ERR_INVALID_PARAMETER;
    }
#endif

    MN_ENTER_CRITICAL();
        Struct_Copy(SwitchConfiguration_t, &m_SwitchConfiguration, pSwitchConfiguration);
        //reset of data on change of configuration never hurts
        for(u8_least i=0; i<DO_NUM_CHANNELS; i++)
        {
            DO_Passthrough.state[i] = DONT_ASSERT;
        }
        STRUCT_CLOSE(DO_Passthrough_t, &DO_Passthrough);
    MN_EXIT_CRITICAL();
    return ram2nvramAtomic(NVRAMID_SwitchConfiguration);
}

/** \brief returns DO switch readinf

  returns a pointer to the switch settings.  The output setings are
  set with SetSwitchX.  The input switch is not yet defined.
  \param[out] dst a pointer to the data read
*/
void dosw_GetSwitchReading(SwitchReading_t *dst)
{
    MN_ENTER_CRITICAL();
        for(u8 i=0; i<DO_NUM_CHANNELS; i++)
        {
            dst->bSwitch[i] = (u8)(!bios_GetDigitalOutput(i));
        }
    MN_EXIT_CRITICAL();
}


/** \brief The Digital Output processing logic.  Called once for each of 2 outputs.

  \param[in] swType - The operating mode for the switch
  \param[in] direction - normally open/ normally closed
  \param[in] sw - the switch specifier (D0 or D1)
  \return none
*/
static void doSwitch(u8 swType, u8 direction, u8 sw)
{
    //s32                 pn4Position;
    u32                 action = DONT_ASSERT;

    switch (swType)
    {
        case SWITCH_DISABLE:
        case SWITCH_ALWAYSOFF:
            //Disable:  set as don't assert
            break;

        case SWITCH_ALWAYSON:
            action = DO_ASSERT;
            break;

        case SWITCH_PASSTHROUGH:
            Struct_Test(DO_Passthrough_t, &DO_Passthrough);
            action = DO_Passthrough.state[sw];
            break;

        case SWITCH_FAIL:
            //Fail:  if mode=OOS and submode > 0 then set as assert
            //Else set as don't assert
            if(mode_GetMode() == MODE_FAILSAFE) //what about FF OOS?
            {
                action = DO_ASSERT;
            }
            break;      //            return DONT_ASSERT;

        case SWITCH_RESET:
            //Reset:  if reset fault bit set then set as assert
            //Else set as don't assert
            if (error_IsFault(FAULT_RESET) == true)
            {
                action = DO_ASSERT;
            }
            break;                  //return DONT_ASSERT;

        case SWITCH_POS_ERROR:
            //Position Error:  if position error bit is set then set as assert
            //Else set as don't assert
            if (error_IsFault(FAULT_POSITION_ERROR) == true)
            {
                action = DO_ASSERT;
            }
            break;          //            return DONT_ASSERT;

        case SWITCH_TIGHT:
            //Tight Shutoff:  if control in tight shutoff (see Control task documentation) then set as assert
            //Else set as don't assert
            if ( full_GetFullThrottleEnabled() ) //TFS:9578 - Take Full Open/Close out of control
            {
                action = DO_ASSERT;
            }
            break;

#if 0 //in FFP
        case SWITCH_POS_LOW:                   //todoEP - float conversion below?
            //pn4Position = smooth_GetSmoothedData(SELECTION_POSITION);
            pn4Position = vpos_GetSmoothedScaledPosition();

            if (pn4Position < SwitchValue)
            {
                action = DO_ASSERT;
            }
            else if (pn4Position < (SwitchValue + SW_DEADBAND))
            {
                action = DO_NOTHING;
            }
            else
            {
            }
            //Position Low:  if position < nSwitch1Value then set as assert
            //Else if position > nSwitch1Value + (0.5% of std range) then set as don't assert
            //Else set as don't do nothing
            break;

        case SWITCH_POS_HIGH:                   //todoEP - float conversion below?
            //pn4Position = smooth_GetSmoothedData(SELECTION_POSITION);
            pn4Position = vpos_GetSmoothedScaledPosition();

            if (pn4Position > SwitchValue)
            {
                action = DO_ASSERT;
            }
            else if (pn4Position > (SwitchValue - SW_DEADBAND))
            {
                action = DO_NOTHING;
            }
            else
            {
            }
            //Position High:  if position > nSwitch1Value then set as assert
            //Else if position < nSwitch1Value - (0.5% of std range) then set as don't assert
            //Else set as don't do nothing
            break;
#endif
        case SWITCH_MANUAL:
            //Manual:  if mode = MANUAL then set as assert
            //Else set as don't assert
            if(mode_GetMode() == MODE_MANUAL)
            {
                action = DO_ASSERT;
            }
            break;

        case SWITCH_CUTOFF_HI:
        case SWITCH_CUTOFF_LO:
        case SWITCH_POS_LOW:
        case SWITCH_POS_HIGH:
        default:
            action = DO_NOTHING;
    }

    //If it is set as do nothing then
    //Do nothing
    if (action != DO_NOTHING)
    {
        bool_t out;

        //Else if it is set as assert then
        //  If switch1direction = normally closed (assert open) then
        //    Set as don't assert
        //  Else (it is set as don't assert)
        //If switch1direction = normally closed (assert open) then
        //Set as assert
        if (direction == SWITCH_NORMAL_CLOSED)
        {
            action ^= DO_ASSERT;
        }
        out = (action == DO_ASSERT) ? true : false;
        bios_SetDigitalOutput(sw, out);
    }
}

/** \brief The Digital Output processing logic.  Calls the switch logic for each switch

  \param[in] none
  \return none
*/
void dosw_OutputSwitches(void)
{
    SwitchConfiguration_t Sw;

    (void)cnfg_GetFillSwitchConfiguration(&Sw);
    //only do the switches if switches are enabled
    if(cnfg_GetOptionConfigFlag(DO_OPTION))
    {
        for(u8 i=0; i<DO_NUM_CHANNELS; i++)
        {
            doSwitch(Sw.swconf[i].SwitchType, Sw.swconf[i].SwitchDirection, i);
        }
    }
    else
    {
        //make sure the switches are off
        for(u8 i=0; i<DO_NUM_CHANNELS; i++)
        {
            doSwitch(SWITCH_DISABLE, SWITCH_NORMAL_OPEN, i);
        }
    }
}

