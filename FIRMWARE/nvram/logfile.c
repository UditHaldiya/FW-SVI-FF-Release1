/*
Copyright 2006-2007 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/** \addtogroup logfile Log File module
\brief Dispatcher of HART commands

\htmlonly
<a href="../../../Doc/DesignDocs/Physical Design_Logfile Module.doc"> Design document </a><br>
<a href="../../inhouse/unit_test/Dbg/project_ESD/logfileHTML.html"> Unit Test Report </a><br>
\endhtmlonly
*/
/**
    \file logfile.c
    \brief Implementation of the engine of the Log File module.

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/FIRMWARE/nvram/logfile.c $
    $Date: 4/15/09 11:17p $
    $Revision: 29 $
    $Author: Arkkhasin $

    \ingroup logfile
*/
/* (Optional) $History: logfile.c $
 *
 * *****************  Version 29  *****************
 * User: Arkkhasin    Date: 4/15/09    Time: 11:17p
 * Updated in $/MNCB/Dev/FIRMWARE/nvram
 * Log file private definitions extracted from nvmempriv.h
 *
 * *****************  Version 28  *****************
 * User: Ernieprice   Date: 5/12/08    Time: 9:53a
 * Updated in $/MNCB/Dev/FIRMWARE/nvram
 * Added include of resource.h for new location of FRAM mutex
 *
 * *****************  Version 27  *****************
 * User: Arkkhasin    Date: 4/12/08    Time: 1:43a
 * Updated in $/MNCB/Dev/FIRMWARE/nvram
 * Limited refactoring required by Lint: some types changed, errors either
 * propagate up or explicitly ignored; a new HART error code 43 added to
 * indicate failed NVMEM write
 *
 * *****************  Version 26  *****************
 * User: Arkkhasin    Date: 4/07/08    Time: 1:34p
 * Updated in $/MNCB/Dev/FIRMWARE/nvram
 * New NVMEM strategy:
 * Standard-pattern type-unsafe accessors
 * Old-style accessors removed
 * Module initilaization table types - ModuleDataItem_t and ModuleData_t
 * removed
 * ram2nvram[Atomic] do not take a pointer to the buffer anymore, but now
 * return an error code
 *
 * *****************  Version 25  *****************
 * User: Arkkhasin    Date: 1/25/08    Time: 9:35a
 * Updated in $/MNCB/Dev/FIRMWARE/nvram
 * Lint
 *
 * *****************  Version 24  *****************
 * User: Arkkhasin    Date: 11/16/07   Time: 9:34a
 * Updated in $/MNCB/Dev/FIRMWARE/nvram
 * Introduced ram2nvramAtomic() as a wrapper of ram2nvram() with mutex
 * lock/unlock around it
 *
 * *****************  Version 23  *****************
 * User: Arkkhasin    Date: 5/21/07    Time: 11:51a
 * Updated in $/MNCB/Dev/FIRMWARE/nvram
 * New tombstone header
*/
#include "mnwrap.h"
#include "errcodes.h"
#include "fram.h"
#include "nvram.h"
#include "errcodes.h"
#include "crc.h"
#include "faultpublic.h"
#include "logfile.h"
#include "logfpriv.h"
#include "mnassert.h"
#include "resource.h"

//Open log info
typedef struct openlog_t
{
    u16 total_length;
    u16 log_length; //actual length of the file
    u16 addr; //!< NVMEM read/write current address
    u16 logpos; //position
    u8 extent_type; //!< extent type currently being read or written; always starting with 0
    u8 logId;
    u8 mode;
    ErrorCode_t err;
    u8 extent[MAX_EXTENT_TYPES]; //!< a sequence of extents (writes:free, reads:match)
    //AK:FUTURE: u16 CheckWord; //optional, TBD
} openlog_t;

static logdata_t logdata; //!< log "directory" info, with "normal" NVMEM image

static openlog_t openlog; //!< RAM-based info about the (only) open log


#define EXTENT_FREE 0xffU
#define EXTENT_ABSENT 0xfeU //!< there is no extent with a number indexing to this value

//--------- Mandatory Initialization and Commit ---------
ErrorCode_t TypeUnsafe_logf_InitSet(const void *src)
{
    if(src == NULL)
    {
        //default init
        mn_memset(logdata.extent_assignment, EXTENT_FREE, LOGF_EXT_COUNT_PROJ);
        mn_memset(logdata.extent_assignment+LOGF_EXT_COUNT_PROJ, EXTENT_ABSENT, sizeof(logdata.extent_assignment)-LOGF_EXT_COUNT_PROJ);
        STRUCT_CLOSE(logdata_t, &logdata);
    }
    else
    {
        Struct_Copy(logdata_t, &logdata, (const logdata_t *)src);
    }
    //AK:TODO: Add data validation!
    openlog.logId = LOGF_INVALID;
    openlog.err = ERR_OK;
    return ERR_OK;
}
const void *TypeUnsafe_logf_InitGet(void *dst)
{
    return STRUCT_GET(&logdata, dst);
}

/** \brief Resets the log file system
*/
ErrorCode_t logf_ResetFS(void)
{
    u8_least id;
    ErrorCode_t err;
    MN_FRAM_ACQUIRE(); //in mutex sandwich to ensure any file write cannot be confused
        //Clear the "new data" events first
        for(id=0; id<NUM_LOG_FILES_PROJ; id++)
        {
            error_ClearFault(logf_EventTable[id].newdata);
        }
        err = TypeUnsafe_logf_InitSet(NULL);
        if(err == ERR_OK)
        {
            err = ram2nvram(NVRAMID_logdata); //cannot do any better
        }
    MN_FRAM_RELEASE();
    return err;
}

/** \brief Returns the last latched error
\return last log file system error
*/
ErrorCode_t logf_GetLastError(void)
{
    return openlog.err;
}

/** \brief Returns the length (in bytes) of the log currently open for reads.
\return The log length. The result is indeterminate if no log is open for reads.
*/
u16_least logf_GetLength(void)
{
    return openlog.log_length;
}

//--------- Open -------------
static ErrorCode_t log_CollectExtents(u8 logId, u8 pattern)
{
    u8_least num_extent_types = NumExtents(logdef[logId]);
    u8_least i;
    u8_least start, end;
    start = 0;
    ErrorCode_t err = ERR_OK;
    for(i=0; i<num_extent_types; i++) //over extent types
    {
        end = start + extdef[i].count;
        for(; start<end; start++) //over versions of the log
        {
            if(logdata.extent_assignment[start] == pattern)
            {
                break; //remove if we want the LAST available segment instead of the FIRST
            }
        }
        if(start==end)
        {
            //we didn't find an extent of type i
            err = ERR_LOGF_INVALID;
            //break; would be a tad faster but who cares (also, MISRA woud be questionable)
        }

        openlog.extent[i] = (u8)start;
        start = end;
    }
    return err;
}

/** \brief Opens a log file; kicks out reads (if any) if open for writes is requested
\param logIdAge - ID/age(version) combo
\param mode - one of LOG_READ or LOG_WRITE
\param[in,out] plen - pointer to [in]the event clearing flag (any non-zero clears) [out] the length of the file (now good only for READ and with no error)
\return error code
*/
ErrorCode_t logf_Open(u8 logIdAge, u8 mode, u16 *plen)
{
    ErrorCode_t err;
    u16_least addr;
    u8 pattern;

    err = ERR_LOGF_INVALID;

    if(LOGF_ID(logIdAge) >= NUM_LOG_FILES_PROJ)
    {
        return err; //legit. early return: pre-requisites check
    }
    if((LOGF_AGE(logIdAge) != 0) && (mode == LOG_WRITE))
    {
        return err; //legit. early return: pre-requisites check
    }

    /* Pre-requisites checks ought to be in a critical section so that a concurrent instance
       of logf_Open() doesn't get a chance to set openlog.logId.
       However, since this function requires FRAM mutex, there is no concurrency issue.
    */
    if( (openlog.logId == LOGF_INVALID) || ((mode == LOG_WRITE) && (openlog.mode != LOG_WRITE)) )
    {
        openlog.mode = mode;
        openlog.total_length = 0;
        openlog.extent_type = 0; //access starts with the first extent of the log
        if(mode == LOG_WRITE)
        {
            pattern = EXTENT_FREE;
        }
        else
        {
            pattern = logIdAge;
        }
        err = log_CollectExtents(LOGF_ID(logIdAge), pattern);
    }

    if(err == ERR_OK)
    {
        openlog.logpos = extdef[0].length - sizeof(openlog.total_length);
        addr = LOGFILE_AREA_ADDR + extdef[0].base_addr + (extdef[0].length * openlog.extent[0]);
        openlog.addr = (u16)(addr + sizeof(openlog.log_length));

        //This is needed for reads only but doesn't hurt writes either
        err = fram_Read((void*)&openlog.log_length, addr, sizeof(openlog.log_length));

    }
    if(err == ERR_OK)
    {
        openlog.logId = logIdAge;
    }
    openlog.err = err;
    if(plen != NULL)
    {
        if((*plen != 0) && (err == ERR_OK) && (LOGF_AGE(logIdAge) == 0U))
        {
            //successfully opened the newest log and "clear event" requested
            error_ClearFault(logf_EventTable[LOGF_ID(logIdAge)].newdata);
        }
        if(mode == LOG_READ)
        {
            *plen = openlog.log_length;
        }
    }
    //TBD: CheckWord
    return err;
}


//--------- Close -------------

static void logf_age(u8 *extent_assignment, u8 logIdAge)
{
    u8 e = *extent_assignment;
    if(LOGF_ID(e^logIdAge) == 0)
    {
        e += LOGF_AGEUNIT;
        if(e>=logIdAge)
        {
            e = EXTENT_FREE;
        }
        *extent_assignment = e;
    }
}


ErrorCode_t logf_Close(u8 logId, bool_t discard)
{
    ErrorCode_t err;
    if(openlog.logId != logId)
    {
        return ERR_LOGF_INVALID;
    }
    err = ERR_OK;
    if(!discard && (openlog.mode == LOG_WRITE))
    {
        err = openlog.err;
        if(err == ERR_OK)
        {
            u32 addr = LOGFILE_AREA_ADDR + extdef[0].base_addr + (extdef[0].length * openlog.extent[0]);
            //the real thing follows

            //NOTE: It is safe to operate on logdata because all concurrency is blocked
            //by the mutex, presumably acquired by the caller.

            //1. Write the total length in the reserved space
            err = fram_Write((void*)&openlog.total_length, addr, sizeof(openlog.total_length));
        }
        if(err == ERR_OK)
        {
            u8_least i;
            //2. Age (and discard too old) extents of this log
            //logId |= (NumVersions(logdef[logId])<<LOGF_IDBITS);
            for(i=0; i<NELEM(logdata.extent_assignment); i++)
            {
                logf_age(&logdata.extent_assignment[i], logId | (NumVersions(logdef[logId])<<LOGF_IDBITS));
            }
            //3. Assign newly filled extents
            for(i=0; i<NumExtents(logdef[logId]); i++)
            {
                logdata.extent_assignment[openlog.extent[i]] = logId; //with 0 age, that is
            }
            //4. Make it valid (TODO: Change to checksum updated instead of recalculation
            STRUCT_CLOSE(logdata_t, &logdata);
            //5. Commit updated logdata
            err = ram2nvram(NVRAMID_logdata);

            //successfully created the newest log
            error_SetFault(logf_EventTable[LOGF_ID(logId)].newdata);
            error_ClearFault(logf_EventTable[LOGF_ID(logId)].dataerr);
        }
        else
        {
            error_SetFault(logf_EventTable[LOGF_ID(logId)].dataerr);
        }
    }
    openlog.logId = LOGF_INVALID;
    return err;
}


// ---------------- read and write ----------------------

typedef enum exec_t {LOGF_READ, LOGF_WRITE, LOGF_SEEK} exec_t;

/** \brief Common logfile code for read and write
\param data - pointer to data buffer to read to or write from
\param[in,out] plen - requested length / actual length
\param logId - file id/age
\param method - LOGF_READ or LOGF_WRITE or LOGF_SEEK
\return error code
*/
static ErrorCode_t logf_Exec(u8 *data, u16 *plen, u8 logId, exec_t method)
{
    ErrorCode_t err = ERR_OK;
    u8 extent_type;
    u16_least len;
    u16_least prev_total_length = openlog.total_length;
    //const u8 *extent =

    //Do the pre-requisites checks here...
    if(openlog.logId != logId)
    {
        return ERR_LOGF_INVALID;
    }
    if(plen == NULL)
    {
        return ERR_INVALID_PARAMETER;
    }

    if(method != LOGF_SEEK)
    {
        if(data == NULL)
        {
            return ERR_INVALID_PARAMETER;
        }
        if( ((method==LOGF_READ) && (openlog.mode != LOG_READ))
           || ((method==LOGF_WRITE) && (openlog.mode != LOG_WRITE)) )
        {
            return ERR_LOGF_OPENMODE;
        }
    }

    //also, check for a sticky error - it is safer than waiting until close!
    if(openlog.err != ERR_OK)
    {
        return ERR_LOGF_PREVIOUS_ERROR;
    }

    len = *plen;

    if(method!=LOGF_WRITE)
    {
        //Do not allow to go beyond EOF
        u16_least remains = openlog.log_length - openlog.total_length;
        if(remains < len)
        {
            len = remains;
        }
    }


    while((len > 0) && (err==ERR_OK))
    {
        if(openlog.logpos == 0)
        {
            extent_type = openlog.extent_type+1;
            if(extent_type == NumExtents(logdef[openlog.logId]))
            {
                err = ERR_LOGF_LENGTH;
            }
            else
            {
                openlog.extent_type = extent_type;
                openlog.logpos = extdef[extent_type].length;
                openlog.addr = LOGFILE_AREA_ADDR +
                    (u16)(extdef[extent_type].base_addr +
                          (extdef[extent_type].length *
                           (openlog.extent[extent_type]-extdef[extent_type-1].count)));
            }
        }
        else
        {
            u16_least ext_length = MIN(openlog.logpos, len);

            if(method==LOGF_READ)
            {
                MN_DBG_ASSERT(data != NULL);
                err = fram_Read(data, openlog.addr, ext_length);
            }
            else if(method==LOGF_WRITE)
            {
                MN_DBG_ASSERT(data != NULL);
                err = fram_Write(data, openlog.addr, ext_length);
            }
            else
            {
                err = ERR_OK;
            }

            if(err == ERR_OK)
            {
                openlog.logpos -= ext_length; //lint !e734 loss of precision: 16-bit arithmetic needed
                len -= ext_length;
                openlog.addr += ext_length; //lint !e734 loss of precision: 16-bit arithmetic needed
                openlog.total_length += ext_length; //lint !e734 loss of precision: 16-bit arithmetic needed
                if(data != NULL) //for lint; we don't dereference data on seek
                {
                    data += ext_length;
                }
            }
        }
    }

    openlog.err = err;
    *plen = (u16)(openlog.total_length - prev_total_length);
    return err;
}

/** \brief Log write API
\param[in] data - pointer to data buffer to read to or write from
\param[in,out] plen - requested length / actual length
\param logId - file id/age
\return error code
*/
ErrorCode_t logf_Write(const void *data, u16 *plen, u8 logId)
{
    ErrorCode_t err;
    //we drop the const qualifier in favor of common interface. But, honestly, we don't write to 'data'
    err = logf_Exec((void *)data, plen, logId, LOGF_WRITE); //lint !e960 (11.5 const cast)
    if(err != ERR_OK)
    {
        error_SetFault(logf_EventTable[LOGF_ID(logId)].dataerr);
    }
    return err;
}

/** \brief Log read API
    Allows to read beyond the actual length (but not beyond the max alloted length)
\param[out] data - pointer to data buffer to read to or write from
\param[in,out] plen - requested length / actual length
\param logId - file id/age
\return error code
*/
ErrorCode_t logf_Read(void *data, u16 *plen, u8 logId)
{
    return logf_Exec(data, plen, logId, LOGF_READ);
}

/** \brief Log seek API
    Seeks a POSITIVE offset of the CURRENT position
\param[in,out] plen - requested / actual file position
\param logId - file id/age
\return error code
*/
ErrorCode_t logf_Seek(u16 *plen, u8 logId)
{
    return logf_Exec(NULL, plen, logId, LOGF_SEEK);
}

/** \brief Combined atomic "Read at offset" API
    Opens, seeks, reads and closes a log file
\param[out] data - recipent of the data
\param[in,out] plen - requested / actual number of bytes to read
\param logIdAge - file id/age
\param offset - offset from the beginning of the log file
\return error code
*/
ErrorCode_t logf_Retrieve(void *data, u16 *plen, u8 logIdAge, u16 offset)
{
    ErrorCode_t err;
    u16 len;  //file length

    len = 0; //do not clear the "new data" fault
    err = logf_Open(logIdAge, LOG_READ, &len);
    if(err == ERR_OK)
    {
        if(len <= offset)  //
        {
            //we are going to be at EOF or beyond
            err = ERR_LOGF_LENGTH;
            if(plen != NULL)
            {
                *plen = 0; //for consistency
            }
        }
        else
        {
            err = logf_Seek(&offset, logIdAge);
        }

        if(err == ERR_OK)
        {
            err = logf_Read(data, plen, logIdAge);
        }

        (void)logf_Close(logIdAge, true); //cannot fail with discard=true
    }

    return err;
}


/** \brief Closes ANY file open for read WHATSOEVER
*/
void logf_CloseAnyRead(void)
{
    if(openlog.mode == LOG_READ)
    {
        openlog.logId = LOGF_INVALID;
    }
}

// -------------- additions --------------------------
MN_DECLARE_API_FUNC(logf_Move)

/** \brief Pushes a temporary file as a latest version to keep.
What is a temporary file is application-defined, but it must have
the same extents layout as the destination file. There are rudimentary
checks for it.
On completion, the temporary file is gone.
Currently, the temporary file may only have one version.
\param temp - a temporary file/age; age must be 0
\param dst - a destination file/age; age must be 0
\return an error code
*/
ErrorCode_t logf_Move(u8 temp, u8 dst)
{
    ErrorCode_t err;
    u16 len;
    openlog_t local;
    MN_FRAM_ACQUIRE();
        err = logf_Open(temp, LOG_READ, &len);
        //Uhm... we didn't mean it; just for error checking... but let's cache extents assignments
        local = openlog;
        if(err == ERR_OK)
        {
            (void)logf_Close(temp, true); //cannot fail with discard
        }
    MN_FRAM_RELEASE();
    if(err != ERR_OK)
    {
        return err;
    }

    if(LOGF_AGE(temp) != 0)
    {
        return ERR_INVALID_PARAMETER; //constraint violation
    }
    if(LOGF_AGE(dst) != 0)
    {
        return ERR_INVALID_PARAMETER; //constraint violation
    }

    u8 temp_id = LOGF_ID(temp);
    u8 temp_numversions = NumVersions(logdef[temp_id]);
    if(temp_numversions != 1)
    {
        return ERR_INVALID_PARAMETER; //constraint violation
    }

    MN_FRAM_ACQUIRE();
        err = logf_Open(dst, LOG_WRITE, NULL);

        if(err == ERR_OK)
        {
            u8_least i;
            //Age (and discard too old) extents of the dst log
            for(i=0; i<NELEM(logdata.extent_assignment); i++)
            {
                logf_age(&logdata.extent_assignment[i], dst | (NumVersions(logdef[dst])<<LOGF_IDBITS));
            }
            //Assign extents of the
            for(i=0; i<NumExtents(logdef[dst]); i++)
            {
                logdata.extent_assignment[local.extent[i]] = dst; //with 0 age, that is
            }
            //Make it valid (TODO: Change to checksum updated instead of recalculation
            STRUCT_CLOSE(logdata_t, &logdata);
            //Commit updated logdata
            err = ram2nvram(NVRAMID_logdata);
            //Discard open log
            (void)logf_Close(dst, true); //can't fail
        }

    MN_FRAM_RELEASE();

    return err;
}

/** \brief tests existence of a log file
\param logIdAge - full file id with age
\return true iff file exists
*/
bool_t logf_Exist(u8 logIdAge)
{
    Struct_Test(logdata_t, &logdata);
    for(size_t i=0; i<NELEM(logdata.extent_assignment); i++)
    {
        if(logdata.extent_assignment[i] == logIdAge)
        {
            return true;
        }
    }
    return false;
}
/* This line marks the end of the source */
