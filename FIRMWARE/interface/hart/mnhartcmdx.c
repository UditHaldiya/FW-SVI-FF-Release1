/*
Copyright 2012 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/** \addtogroup mnhartcmd HART Command Module
\brief Dispatcher of HART commands
*/
/**
    \file mnhartcmdx.c
    \brief Shameless stubs for HART 6 command dispatcher, a glue between data link layer and empty app layer

    CPU: Any

    OWNER: AK
    \ingroup mnhartcmd
*/

#include "mnwrap.h"
#include "hartfunc.h"

/**
\brief Central dispatcher of HART requests

Checks the app-level correctness of the data in the request
and dispatches processing to the appropriate app-level function

\param cmd - HART command to process
\param src - request data source as a byte array
\param dst - response data destination as a byte array
\param req_data_len - length (in bytes) of the src array
\return if negative, -error, otherwise, the length of the response in dst
*/
s32 hart6_Dispatch(u8_least cmd, const u8 *src, u8 *dst, u8 req_data_len)
{
    UNUSED_OK(cmd);
    UNUSED_OK(src);
    UNUSED_OK(dst);
    UNUSED_OK(req_data_len);
    return COMMAND_NOT_IMPLEMENTED;
}

/**
\brief Central dispatcher of HART burst responses

Dispatches processing to the appropriate app-level function

\param cmd - HART command to process
\param dst - response data destination as a byte array
\return the length of the response in dst or if <0 an error code
*/
s8_least hart6_BurstDispatch(u8_least cmd, u8 *dst)
{
    UNUSED_OK(cmd);
    UNUSED_OK(dst);
    return COMMAND_NOT_IMPLEMENTED;
}

//access method
const HartDispatch_t *Hart_Cmdlookup(u8_least cmd)
{
    UNUSED_OK(cmd);
    return NULL;
}
/* This line marks the end of the source */

