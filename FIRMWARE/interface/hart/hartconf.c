/*
Copyright 2004-2011 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file hartconf.c
    \brief HART Data Link protocol - configuration parameters and device ID

    CPU: Philips LPC21x4 (ARM)

    OWNER: Ernie Price

    $Archive: /MNCB/Dev/ESD_Release_3.1.2/FWH6/FIRMWARE/interface/hart/hartconf.c $
    $Date: 9/15/11 4:42p $
    $Revision: 27 $
    $Author: Arkkhasin $

    \ingroup HARTDLL
*/
/* $History: hartconf.c $
 *
 * *****************  Version 27  *****************
 * User: Arkkhasin    Date: 9/15/11    Time: 4:42p
 * Updated in $/MNCB/Dev/ESD_Release_3.1.2/FWH6/FIRMWARE/interface/hart
 * TFS:7588,7512 Special strict HART compliance factory option config to
 * pass compliance tests on commands 6, 7
 *
 * *****************  Version 26  *****************
 * User: Arkkhasin    Date: 7/26/11    Time: 4:12p
 * Updated in $/MNCB/Dev/ESD_Release_3.1.2/FWH6/FIRMWARE/interface/hart
 * Port of AP HART 6 version
*/
#include "mnwrap.h"
#include "nvram.h"
#include "oswrap.h"
#include "mnassert.h"
#include "hart.h"
#include "hartfunc.h"
#include "faultpublic.h"
#include "configure.h"

/*******************************************************************************
*** internal defines
*******************************************************************************/

/* values for first time HART initialization */

#define INIT_BURST_MODE_COMMAND_VALUE 3         /* use command 1 */
#define INIT_BURST_MODE_CTRL_VALUE 0            /* no burst mode */
#define INIT_PREAMBLE_VALUE 5                  /* number of Hart praeamble bytes */
#define INIT_HART_ADDRESS 0                      /* default adress of Hart device */

/*******************************************************************************
*** internal data
*******************************************************************************/
/*
 \brief the command list that when burst
 */
static const u8 burst_commands[] =
{
    1, 2, 3, 9, 33
};

static HartData_t HartData;

/**
\brief The accessor of HART configuration data.

    This functions is called a few times in a row, also from
    the interrupt. So it doesn't do the data checking. It is
    deferred for Hart_proc() which calls a different accessor
    (below).

\return a pointer to HartData_t holding the configuration
*/
const HartData_t *hart_GetHartData(void)
{
    return &HartData;
}

/**
\brief The accessor of HART configuration data.
\return a pointer to HartData_t holding the configuration
*/
const void *TypeUnsafe_hart_GetAndTestHartData(void *dst)
{
    return STRUCT_TESTGET(&HartData, dst);
}

#define MAX_POLLING_ADDRESS_HART5 15u
#define MAX_POLLING_ADDRESS_HART6 63u

#define FPS     0x82, 0x08, 0x20,                              // four packed ASCII spaces
#define TAGV    0x38, 0x55, 0xe0,                              // Packed ASCII "NEW "
#define MON      6
#define DAY      19
#define YEAR    (2007 - 1900)
static const HartData_t HartData_Default =
{
    .hart_version = HART_REV_5,
    .device_id =
    {
        0, 0, 0 /* device ID number */
    },
    .message =
    {
        FPS FPS FPS FPS FPS FPS FPS FPS //message
    },
    .tag =
    {
        TAGV FPS //tag
    },
    .descriptor =
    {
        FPS FPS FPS FPS  //descriptor
    },
    .date =
    {
        DAY, MON, YEAR //date
    },
    .finalass_nbr =
    {
        0, 0, 0 //final assy #
    },
    .transmit_address = INIT_HART_ADDRESS, //default transmit addr
    .nbr_resp_pream = INIT_PREAMBLE_VALUE, //default number of preambles
    .burst_mode_cmd = INIT_BURST_MODE_COMMAND_VALUE, //default burst mode command
    .burst_mode_ctrl = INIT_BURST_MODE_CTRL_VALUE, // /* burst mode off */
    .LoopCurrentMode = LOOP_MODE_ENABLED,
    CHECKFIELD_CLEAR() //placeholder for CheckWord
};


/** \brief Save the HART configuration data to FRAM
    \return ErrorCode;
*/
ErrorCode_t TypeUnsafe_hart_SetHartData(const void *src)
{
    u8 i, items;
    const HartData_t * pHartData = src;
    ErrorCode_t errcode = ERR_OK;
    bool_t hd5, hd6;

    if(pHartData == NULL)
    {
        pHartData = &HartData_Default;
    }

    if((pHartData->LoopCurrentMode == LOOP_MODE_ENABLED) ||
        ((pHartData->LoopCurrentMode == LOOP_MODE_DISABLED) && cnfg_GetOptionConfigFlag(STRICT_HART_CONFORMANCE)))
    {
        //OK
    }
    else
    {
        return ERR_NOT_SUPPORTED; //must translate to rsp code 6 "device-specific error"
    }

    /* Check range of HART parameters */
    items = sizeof(burst_commands)/sizeof(burst_commands[0]);
    for (i = 0; i < items; i += 1)
    {
        if (pHartData->burst_mode_cmd == burst_commands[i])
        {
            break;
        }
    }
    if (i == items)
    {
        errcode = ERR_INVALID_PARAMETER;
    }

    if((pHartData->nbr_resp_pream < HRT_MIN_RESPPREAM) || (pHartData->nbr_resp_pream > HRT_MAX_RESPPREAM))
    {
        errcode = ERR_INVALID_PARAMETER;
    }

    switch (pHartData->hart_version)
    {
    case HART_REV_5:
        if(pHartData->transmit_address > MAX_POLLING_ADDRESS_HART5)
        {
            errcode = ERR_INVALID_PARAMETER;
        }
        if (NULL == HART_cmdlookup(pHartData->burst_mode_cmd) )
        {
            errcode = ERR_INVALID_PARAMETER;
        }
        break;

    case HART_REV_6:
        if(pHartData->transmit_address > MAX_POLLING_ADDRESS_HART6)
        {
            errcode = ERR_INVALID_PARAMETER;
        }
        hd6 = (NULL == Hart_Cmdlookup(pHartData->burst_mode_cmd) );
        hd5 = (NULL == HART_cmdlookup(pHartData->burst_mode_cmd) );
        if ( hd5 && hd6  )
        {
            errcode = ERR_INVALID_PARAMETER;
        }
        break;

    default:
        errcode = ERR_INVALID_PARAMETER;
        break;
    }

    if (ERR_OK == errcode)
    {
        Struct_Copy(HartData_t, &HartData, pHartData);
#ifdef OLD_NVRAM
        hart_SaveHartData();
#else
        errcode = ram2nvramAtomic(NVRAMID_HartData);
#endif
    }
    return errcode; //AK: for now
}

#ifdef OLD_NVRAM
/**
\brief save HART configuration data on commit
  NOTE:   ***** ram2nvram can be called here passing the member data directly
          because the data may be modified only when HART interface requires it.
          The HART request will not be granted if a write command or a process
          is being executed. So, the data is guaranteed not to change
          while being written.
*/
void hart_SaveHartData(void)
{
    MN_FRAM_ACQUIRE();
        ram2nvram(&HartData, NVRAMID_HartData);
    MN_FRAM_RELEASE();
}

/** \brief Initialize the HART data struct - from FRAM or default as requested
    \param[in] itype = flag to indicate load from FRAM versus default
*/
void hart_InitHartData(InitType_t itype)
{
    MN_ASSERT(!oswrap_IsOSRunning()); //and no FRAM mutex
    if(itype == INIT_FROM_NVRAM)
    {
        nvram2ram(&HartData, NVRAMID_HartData);
    }

    if( error_IsFault(FAULT_NVM_CHECKSUM) || (!(itype == INIT_FROM_NVRAM)) )  //on error use the defaults
    {
        /* no basic initialisation of BIOS configuration */
        HartData = HartData_Default;
        STRUCT_CLOSE(HartData_t, &HartData);
    }
    if ((HRT_MAX_RESPPREAM < HartData.nbr_resp_pream ) || (HRT_MIN_RESPPREAM > HartData.nbr_resp_pream))  // insert a useful startvalue
    {
        HartData.nbr_resp_pream = HRT_MIN_RESPPREAM;
        STRUCT_CLOSE(HartData_t, &HartData);
    }
    Hart_init();
}
#endif

/* This line marks the end of the source */
