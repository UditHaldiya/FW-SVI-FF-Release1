/*
Copyright 2012 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file mnhart2ff.c
    \brief Misc. conversions betweein HART and FF

    CPU: Any

    OWNER: AK
*/

//Softing headers first
#include <softing_base.h>

//MN FIRMWARE headers second
#include "hartfunc.h"
#include "mncbdefs.h" //to force linking of VerString (and mychecksum)

//Glue headers last
#include "mnhart2ff.h"

/** \brief Conversion of HART error to FF error
\param harterr - HART error code
\return FF error code
*/
fferr_t err_Hart2FF(u8 harterr)
{
    fferr_t fferr;
    switch(harterr)
    {
        case HART_NO_COMMAND_SPECIFIC_ERRORS:
            fferr = E_OK;
            break;

        case HART_INVALID_SELECTION:
        case HART_RV_SPAN_INVALID:
        case HART_INVALID_UNITCODE:
        //case HART_INVALID_LOCK_CODE: collides with HART_RV_LOW_TOO_LOW
            fferr = E_FB_PARA_CHECK;
            break;

        case HART_PASSED_PARAMETER_TOO_LARGE:
        case HART_PASSED_PARAMETER_TOO_SMALL:
        case HART_RV_LOW_TOO_HIGH:
        case HART_RV_LOW_TOO_LOW:
        case HART_RV_HIGH_TOO_HIGH:
        case HART_RV_BOTH_OOR:
            fferr = E_FB_PARA_LIMIT;
            break;

        case TRANSMITTER_SPECIFIC_COMMAND_ERROR:
        case HART_WRITE_PROTECT_MODE:
            fferr = E_FB_WRITE_LOCK;
            break;


        case HART_TOO_FEW_DATA_BYTES_RECEIVED: //TODO!!!
        case HART_BUSY:
            fferr = E_FB_TEMP_ERROR;
            break;

        case HART_INVALID_MODE_SELECTION: //also HART_RV_HIGH_TOO_LOW:
        case HART_CANT_CHANGE_MODE:
        case HART_ACCESS_RESTRICTED: //WRONG MODE
            fferr = E_FB_WRONG_MODE;
            break;

        default:
            fferr=E_FB_NO_SUBIDX_ACCESS; //TODO!!!
            break;
    }
    return fferr;
}

/** A wrapper around HART_acyc_cmd returning a coherent FF response code
\param cmd - command number
\param send_buff - a pointer to a xmit buffer
\param send_length - xmit length of data payload
\param receive_buff - a pointer to a receive buffer
\return FF error code
*/
fferr_t mn_HART_acyc_cmd(u8 cmd, u8 *send_buff, u8 send_length, u8 *receive_buff)
{
    fferr_t fferr;

    //A countermeasure (not a solution) for bug 11609
    receive_buff[0] = HART_TYPE_MISMATCH; //forbidden response which must be overwritten on success

    u16 result = HART_acyc_cmd(cmd, send_buff, send_length, receive_buff);
    if(result == 0)
    {
        u8 rsp = receive_buff[0];
        fferr = err_Hart2FF(rsp);
    }
    else
    {
        fferr = E_FB_TEMP_ERROR; //Comm error; TODO
    }
    return fferr;
}

//brutal hack to have checksum linked in from the library
__root const void * const pVerString = VerString;
extern const u16 mychecksum;
__root const u16 * const pcrc = &mychecksum;


/* This line marks the end of the source */
