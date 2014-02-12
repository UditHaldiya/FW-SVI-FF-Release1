/*
Copyright 2008 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file pressures.c
    \brief Pressures-related functionality

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/sysio/pressures.c $
    $Date: 1/30/12 2:57p $
    $Revision: 2 $
    $Author: Arkkhasin $

    \ingroup sysio
*/
/* $History: pressures.c $
 * 
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 1/30/12    Time: 2:57p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/sysio
 * Dead code removal (no code change) FBO TFS:8782
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 11/14/11   Time: 11:35p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/sysio
 * Preliminary check-in for
 * TFS:8051 new tempcomp
 * TFS:8202 decouple I/O subsystem
 * Not yet ready to package as a module
 *
 * *****************  Version 9  *****************
 * User: Arkkhasin    Date: 2/10/10    Time: 7:01p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * Light linting
 *
 * *****************  Version 8  *****************
 * User: Arkkhasin    Date: 1/29/10    Time: 7:26p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * Pressure handling is split into individual channels
 * Main pressure is computed automatically
 * Pressure tests now take advantage of PRESSURE_INVALID trap
 * representation
 *
 * *****************  Version 7  *****************
 * User: Arkkhasin    Date: 12/14/09   Time: 7:29p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * Availability of PRESSURE_SUPPLY based on option config is now done at
 * the interface level
 *
 * *****************  Version 6  *****************
 * User: Arkkhasin    Date: 11/16/09   Time: 3:09p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * Fix of PRESSURE_PILOT calculation (wrong physical channel was used).
 * Partial linting
 *
 * *****************  Version 5  *****************
 * User: Arkkhasin    Date: 5/14/08    Time: 5:27p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * (Preliminarily) adapted to new tempcomp
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 4/08/08    Time: 6:44p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * Moved def_PressureCalData from configure to pressure
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 4/07/08    Time: 3:22p
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * New NVMEM strategy:
 * Standard-pattern type-unsafe accessors
 * Old-style accessors removed
 * Module initilaization table types - ModuleDataItem_t and ModuleData_t
 * removed
 * ram2nvram[Atomic] do not take a pointer to the buffer anymore, but now
 * return an error code
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 2/25/08    Time: 2:09a
 * Updated in $/MNCB/Dev/FIRMWARE/sysio
 * Removed no longer used header(s)
 * cnfg_GetPressureCalData made static pres_GetPressureCalData
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 2/15/08    Time: 10:43p
 * Created in $/MNCB/Dev/FIRMWARE/sysio
 * Pressures API implemnetation consolidated here
*/
#include "mnwrap.h"
#include "numutils.h"
#include "errcodes.h"
#include "nvram.h"
#include "faultpublic.h"
#include "tempcomp.h"
#include "pressures.h"
#include "pneumatics.h"

#define LOW_PRESSURE_LIMIT STD_FROM_PSI(4.0) //cf. includes/configure.h,91

//defaults
static const PressureCalData_t  def_PressureCalData =
{
    .nLowPressure =
        {
            [0] = LOW_PRESSURE_DEF,   //(0)
            [1] = LOW_PRESSURE_DEF,   //(0)
            [2] = LOW_PRESSURE_DEF,   //(0)
            [3] = LOW_PRESSURE_DEF,   //(0)
        },
    CHECKFIELD_CLEAR()
};


//forward

static BoardPressure_t m_PressureData;          // self-healing at long cycle rate
static BoardPressure_t m_RawPressureData;       // self-healing at long cycle rate
static PressureCalData_t       m_PressureCalData;


/** \brief Provides access to the board compensated pressure data owned by the cycle task

  \return BoardPressure_t* - pointer to the  board pressure stucture
*/
const BoardPressure_t *pres_GetPressureData(void)
{
    return &m_PressureData;
}

/** \brief Provides access to the board compensated pressure data owned by the cycle task
  no user calibrations are applied to these
  \return BoardPressure_t* - pointer to the raw board pressure stucture
*/
const BoardPressure_t *pres_GetRawPressureData(void)
{
    return &m_RawPressureData;
}

/** \brief returns the proper pressure for both single and double acting

  returns the proper pressure for both single (actuator 1) and double acting
    (actuator1 - actuator2)
  \return pressure
*/
pres_t pres_GetMainPressure(void)
{
    return m_PressureData.Pressures[PRESSURE_MAIN_INDEX];
}


/** \brief Computes pressures usable by the application layer
*/
void pres_PressureComp(void)
{
    u32 i;
    const AdDataRaw_t *raw  = bios_GetAdRawData();

    //NOTE:  Channels changed around to match actual
    //      This will be fixed by the multiplexer settings
    //      in the future and will have to be changed back

    /* It is safe to cast atm to pres_t because of the range of atm
    (see TEMPCOMP_C_ENTRY(presatm)
    Moreover, it is safe to cast any pressure - atm to pres_t because of
    allowed ranges.
    See corresponding CONST_ASSERT around the TEMPCOMP_C_ENTRY place on
    top of the file.
    */

    BoardPressure_t pr;
    const PneumaticParams_t *p = pneu_GetParams(NULL);

    for(i=0; i<NUMBER_OF_PRESSURE_SENSORS; i++)
    {
        s8 adchannel = p->SensorMap[i];
        if(adchannel < 0)
        {
            //Sensor is not there
            pr.Pressures[i] = PRESSURE_INVALID;
        }
        else
        {
            pr.Pressures[i] =
                (pres_t)pres_CorrectForTemperature(adchannel, raw->AdValue[adchannel]);
            //Cast OK because of tempcomp limits; see prestab.c
        }
    }

    pres_t atm = pr.Pressures[PRESSURE_ATM_INDEX];
    if(atm == PRESSURE_INVALID)
    {
        atm = 0; //no correction
    }

    for(i=0; i<NUMBER_OF_PRESSURE_SENSORS; i++)
    {
        if((pr.Pressures[i] != PRESSURE_INVALID) && (i != PRESSURE_ATM_INDEX))
        {
            //Sensor is there and not atmospheric
            pr.Pressures[i] -= atm;
        }
    }

    pres_t corr = 0; //cal correction value
    pr.Pressures[PRESSURE_MAIN_INDEX] = pr.Pressures[PRESSURE_ACT1_INDEX];
    //main pressure (courtesy service)
    if(pr.Pressures[PRESSURE_ACT1_INDEX] == PRESSURE_INVALID)
    {
        //Nothing more to do
    }
    else if(p->SingleActing != 0)
    {
        corr = m_PressureCalData.nLowPressure[PRESSURE_ACT1_INDEX];
        //and Nothing more to do
    }
    else
    {
        //double-acting: careful with 2 sensors!
        if(pr.Pressures[PRESSURE_ACT2_INDEX] == PRESSURE_INVALID)
        {
            pr.Pressures[PRESSURE_MAIN_INDEX] = PRESSURE_INVALID;
            //and Nothing more to do
        }
        else
        {
            pr.Pressures[PRESSURE_MAIN_INDEX] = pr.Pressures[PRESSURE_ACT1_INDEX] - pr.Pressures[PRESSURE_ACT2_INDEX];
            corr = m_PressureCalData.nLowPressure[PRESSURE_ACT1_INDEX] - m_PressureCalData.nLowPressure[PRESSURE_ACT2_INDEX];
        }
    }

    MN_ENTER_CRITICAL();
        m_RawPressureData = pr;
        //make user corrections
        for (i = PRESSURE_ACT1_INDEX; i < NUMBER_OF_PRESSURE_SENSORS; ++i)
        {
            if((i==PRESSURE_ATM_INDEX) || (pr.Pressures[i] == PRESSURE_INVALID))
            {
                m_PressureData.Pressures[i] = pr.Pressures[i];
            }
            else
            {
                m_PressureData.Pressures[i] = pr.Pressures[i] - m_PressureCalData.nLowPressure[i];
            }
        }
        m_PressureData.Pressures[PRESSURE_MAIN_INDEX] = pr.Pressures[PRESSURE_MAIN_INDEX] - corr;
    MN_EXIT_CRITICAL();
    Struct_Test(PressureCalData_t, &m_PressureCalData); //as good a place as any
}


/** \brief Performs a user zero calibration of the pressure sensors
   Notes:
    The 4 primary pressure sensors (does not include the atmosphere pressure) are
    all rezeroed at the same time.
  \return bool_t - false if no error, true if error
*/
//#define MAX_PRESSURE_SENSORS 4
bool_t cal_CalibratePressure(void)
{
    u32     x;
    PressureCalData_t mypressuredata;
    bool_t retVal = false;

    //If any of the raw pressure values are greater than MAX_PRESSURE_OFFSET then
    for (x = 0u; x < MAXNUM_PRESSURE_SENSORS; x++)
    {
        if (m_RawPressureData.Pressures[x] > MAX_PRESSURE_OFFSET)
        {
            retVal = true;
        }
    }
    if (!retVal)
    {
        //Get the pressure calibration data by calling GetPressureCalData() and move the data to a local copy of the structure
        //mn_memcpy((void*)&mypressuredata, (void*)pres_GetPressureCalData(), sizeof(PressureCalData_t));
         (void)pres_GetPressureCalData(&mypressuredata);

        /* This loop could well be in a critical section - but we expect stable
         conditions during calibration, in which case it doesn't matter
        */
        for (x = 0u; x < MAXNUM_PRESSURE_SENSORS; x++)
        {
            pres_t pres = m_RawPressureData.Pressures[x];
            if(pres != PRESSURE_INVALID) //Don't calibrate missing or failed sensors
            {
                mypressuredata.nLowPressure[x] = pres;
            }
        }

        if (pres_SetPressureCalData(&mypressuredata) != ERR_OK)
        {
            retVal = true;
        }
    }
    if (retVal)
    {
        error_SetFault(FAULT_CALIBRATION_FAILED);
    }
    return retVal;
}

/** \brief returns the pressure cal data
  \return const PressureCalData_t*
*/
const void *TypeUnsafe_pres_GetPressureCalData(void *dst)
{
    return STRUCT_TESTGET(&m_PressureCalData, dst);
}


/** \brief stores the pressure zero calibrations

  stores the pressure zero calibrations
  \param[in] pPressureCalData - local copy of the cal data
  \return ERR_OK if no error, ERR_CNFG_PRESSURE_RANGE if zero readings are
        out of range
*/
ErrorCode_t TypeUnsafe_pres_SetPressureCalData(const void *src)
{
    const PressureCalData_t* pPressureCalData = src;
    u32 i;

    /** validate the input */
    if(pPressureCalData == NULL)
    {
        pPressureCalData = &def_PressureCalData;
    }

    for(i=0u; i<(u32)MAXNUM_PRESSURE_SENSORS; ++i)
    {
        if( (pPressureCalData->nLowPressure[i] > LOW_PRESSURE_LIMIT) ||
            (pPressureCalData->nLowPressure[i] < -LOW_PRESSURE_LIMIT) )
        {
            /* Legitimate early return: Pre-requisites check */
            return ERR_CNFG_PRESSURE_RANGE;
        }
    }

    Struct_Copy(PressureCalData_t, &m_PressureCalData, pPressureCalData);
    /** write the data to fram  */
#ifdef OLD_NVRAM
    /** get sole use of FRAM */
    MN_FRAM_ACQUIRE();

        /** write the data to fram and release fram */
        ram2nvram((void*)pPressureCalData, NVRAMID_PressureCalData);

    MN_FRAM_RELEASE();

    return ERR_OK;
#else
    return ram2nvramAtomic(NVRAMID_PressureCalData);
#endif //OLD_NVRAM
}
/* This line marks the end of the source */
