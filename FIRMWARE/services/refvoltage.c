/*
Copyright 2004 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file refvoltage.c
    \brief Implementation of the reference voltage

     CPU: Any

    $Revision: 1 $
    OWNER: EJ
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/services/refvoltage.c $
    $Date: 7/20/12 12:43p $
    $Revision: 1 $
    $Author: EricJ $

    \ingroup services
*/

/* $History: refvoltage.c $
 *
 * *****************  Version 1  *****************
 * User: Ericj Date: 7/20/12    Time: 5:15p
 * Created in $/MNCB/Dev/FIRMWARE/services
 * Reference Voltage handling

*/

#include "mnwrap.h"
#include "oswrap.h"
#include "crc.h"
#include "errcodes.h"
#include "nvram.h"
#include "refvoltage.h"

//-----------------------------------------------------------------------------------
// reference voltage Handling variables

static RefVoltage_t m_RefVoltage;

static const RefVoltage_t  def_RefVoltage =
{
    .HiLimit = 34406,
    .LoLimit = 31130,
    CHECKFIELD_CLEAR()
};

//-----------------------------------------------------------------------------------

/**
    \brief Conforming accessor to reference voltage data.
    \return RefVoltage_t* - pointer to reference voltage
*/
const RefVoltage_t* refvoltage_GetData(RefVoltage_t *dst)
{
    return STRUCT_TESTGET(&m_RefVoltage, dst);
}

/**
    \brief This function is externally called to set reference voltage and write it to NVRAM.

    \param[in] pReVoltage - the pointer to reference voltage structure
*/
ErrorCode_t refvoltage_SetData(const RefVoltage_t* pRefVoltage)
{
    if(pRefVoltage == NULL)
    {
        pRefVoltage = &def_RefVoltage;
    }

    // Copy the structure with the checksum
    Struct_Copy(RefVoltage_t, &m_RefVoltage, pRefVoltage);

#ifdef OLD_NVRAM
    /** get sole use of FRAM */
    MN_FRAM_ACQUIRE();

        // write the data to fram and release fram
        sysio_saveLowPowerDataInNVRAM();

    MN_FRAM_RELEASE();
    return ERR_OK;
#else
    return ram2nvramAtomic(NVRAMID_RefVoltage);
#endif //OLD_NVRAM
}

/* This line marks the end of the source */
