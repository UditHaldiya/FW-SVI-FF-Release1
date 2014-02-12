/*
Copyright 2004-2007 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/** \addtogroup Bufferhandler Buffers handler module
\brief API and implementation of data buffer management for sampling and diagnostics
\htmlonly
<a href="../../../Doc/DesignDocs/Physical Design_BufferHandler Module.doc"> Design document </a><br>
<a href="../../inhouse/unit_test/Dbg/project_ESD/BUFFERhandler1HTML.html"> Unit Test Report [1]</a><br>
<a href="../../inhouse/unit_test/Dbg/project_ESD/bufferhandler2HTML.html"> Unit Test Report [2]</a><br>
\endhtmlonly
*/
/**
    \file bufferhandler.c
    \brief  the functions needed to control the data buffer used by
            diagnostics and data logging.

    Implements a circular buffer which can be read continuously.

            Endianness change and task (thread) safety is controlled by
            switches in config.h

    OWNER: AK
    $Archive: /MNCB/Dev/AP_Release_3.1.x/FIRMWARE/services/bufferhandler.c $
    $Date: 1/15/10 3:53p $     $Revision: 7 $     $Author: Arkkhasin $

    \ingroup Bufferhandler
*/

#include "config.h" //for compile-time switches
#include "mnwrap.h"
#include "numutils.h"
#include "bufferhandler.h"
#include "crc.h"
#include "oswrap.h" //not needed for forward port to trunk

#if DIAG_STORE_THREAD_SAFE == OPTION_ON
#   define STORE_ENTER_CRITICAL() MN_ENTER_CRITICAL()
#   define STORE_EXIT_CRITICAL() MN_EXIT_CRITICAL()
#endif
#if DIAG_STORE_THREAD_SAFE == OPTION_OFF
#   define STORE_ENTER_CRITICAL()
#   define STORE_EXIT_CRITICAL()
#endif
//No #else : other values should cause compilation error

#define PRE_STORE16(a) (a)
//No #else : other values should cause compilation error


//Defines
#define DIAG_BUFFER_INDEX_MASK (DIAGNOSTIC_BUFFER_SIZE-1u)
//not used here #define DIAG_BUFFER_MAX_ELEMENTS DIAG_BUFFER_INDEX_MASK

#define DIAG_OVFL_SIGNATURE ((diag_t)0x8000)


//Module Member Data
//static volatile u16 startpos, endpos;

typedef struct diagbuf_t
{
    SamplerInfo_t SamplerInfo;
    volatile u16 startpos, endpos;
    diag_t m_DiagnosticBuffer[DIAGNOSTIC_BUFFER_SIZE];
} diagbuf_t;

static MN_NO_INIT union db_t //lint !e960 union is used for alignment only
{
    u32 dummy; /*lint -esym(754, db_t::dummy) for alignment only*/ //!< just to ensure alignment
    diagbuf_t buf[NUM_DIAG_BUFFERS];
} db;//lint !e960 union is used for alignment only

static u16_least SizeLeft(u8_least bufnum, u16_least end)
{
    u16_least szleft = db.buf[bufnum].startpos;
    szleft -= end;
    szleft--;
    szleft &= DIAG_BUFFER_INDEX_MASK;
    return szleft;
}

static u16_least Entries(u8_least bufnum, u16_least end)
{
    u16_least e = end;
    e -= (u16_least)db.buf[bufnum].startpos;
    e &= DIAG_BUFFER_INDEX_MASK;
    return e;
}

static u16_least IncrementCounter(u16_least n)
{
    u16_least nn = n;
    nn++;
    nn &= DIAG_BUFFER_INDEX_MASK;
    return nn;
}

static u16_least DecrementCounter(u16_least n)
{
    u16_least nn = n;
    nn--;
    nn &= DIAG_BUFFER_INDEX_MASK;
    return nn;
}

/**
\brief The routine returns a read only
\return pointer to the diagnostic buffer
*/
diag_t *buffer_GetXDiagnosticBuffer(u8_least bufnum)
{
    if(bufnum>=NUM_DIAG_BUFFERS)
    {
        return NULL;
    }
    return db.buf[bufnum].m_DiagnosticBuffer;
}

static void dummy_sampler(diag_t data[2])
{
    UNUSED_OK(data);
}


/**
\brief resets the diagnostic buffer to empty condition

Notes:
This function should be called before a diagnostic test and should be called
at startup.
Only the hart task should get data from the buffer (which manipulates
the start pointer) and only the process task should put data into the buffer
(which manipulates the end pointer).
The places in the code that uses both must be made thread safe.
*/
void buffer_InitializeXDiagnosticBuffer(u8_least bufnum)
{
    if(bufnum>=NUM_DIAG_BUFFERS)
    {
        return;
    }
    const SamplerInfo_t init_sampler =
    {
        .fill_func = dummy_sampler,
        .baseptr = &db.buf[bufnum].m_DiagnosticBuffer[0],
        .init_points = 0,
        .prune_scale = 1,
        .context = TASKID_IDLE,
        .num_points = 0,
        .max_points = 2,
        .skip = 0,
        .prune_index = 2,
        .CheckWord = 0, //don't care

    };
    //THIS CODE MUST BE DONE AS A CRITICAL SECTION (e.g. disable interrupts)
    MN_ENTER_CRITICAL();
        db.buf[bufnum].startpos = 0U;
        db.buf[bufnum].endpos = 0U;
    MN_EXIT_CRITICAL();
    Struct_Copy(SamplerInfo_t, &db.buf[bufnum].SamplerInfo, &init_sampler);

}

/**
\brief stuffs the buffer with one or two fields
\param bufnum - a buffer to stuff the data into
\param items - must be 1 or 2
\param item1 - first item to stuff
\param item2 - second item to stuff
\return error indicator (true iff overflow)
*/
static bool_t buff_stuff(u8_least bufnum, u8_least items, diag_t item1, diag_t item2)
{
    bool_t ret;
    u16_least pos;

    if(bufnum >= NUM_DIAG_BUFFERS)
    {
        return false; //OK to stuff data into abyss
    }

    STORE_ENTER_CRITICAL();
        pos = db.buf[bufnum].endpos;
        if(SizeLeft(bufnum, pos) < items)
        {
            //not enough room
            pos = DecrementCounter(pos);
            db.buf[bufnum].m_DiagnosticBuffer[pos] = DIAG_OVFL_SIGNATURE;
            //items = 0;
            ret = true;
        }
        else
        {
            //stuff the items
            db.buf[bufnum].m_DiagnosticBuffer[pos] = PRE_STORE16(item1);
            pos = IncrementCounter(pos);
            if(items > 1u)
            {
                db.buf[bufnum].m_DiagnosticBuffer[pos] = PRE_STORE16(item2);
                pos = IncrementCounter(pos);
            }
            db.buf[bufnum].endpos = (u16)pos;
            ret = false;
        }
    STORE_EXIT_CRITICAL();
    return ret; //items;
}

/**
\brief puts one word into the diagnostic data buffer

Notes:
Only the hart task should get data from the buffer (which manipulates
the start pointer) and only the process task should put data into
the buffer (which manipulates the end pointer).  The places in the
code that uses both must be made thread safe.

\param bufnum - buffer to stuff into
\param data to stuff
\return FALSE if no error, TRUE if error
*/
bool_t buffer_AddXDiagnosticData1(u8_least bufnum, diag_t data)
{
    return buff_stuff(bufnum, 1u, data, (diag_t)0);
}
/**
\brief puts two words into the diagnostic data buffer

Notes:
Only the hart task should get data from the buffer (which manipulates the
start pointer) and only the process task should put data into the buffer
(which manipulates the end pointer).  The places in the code
that uses both must be made thread safe.
\param bufnum - buffer to stuff into
\param data1 to stuff
\param data2 to stuff
\return FALSE if no error, TRUE if error
*/
bool_t buffer_AddXDiagnosticData2(u8_least bufnum, diag_t data1, diag_t data2)
{
    return buff_stuff(bufnum, 2u, data1, data2);
}

/**
\brief puts a floating point value into the diagnostic data buffer

Notes:
Only the hart task should get data from the buffer (which manipulates
the start pointer) and only the process task should put data into the
buffer (which manipulates the end pointer).  The places in
the code that uses both must be made thread safe.

\param bufnum - buffer to sample into
\param fData - a 4-byte floating-point number to sample
\return bError = false if no error, true if error
*/
bool_t buffer_AddXDiagnosticDataFloat(u8_least bufnum, float32 fData)
{
    diag_t data1, data2;
    u32 uData = (u32)float2ieee(fData);
    data1 = (diag_t)uData; //lower halfword
    uData >>= 16; //upper halfword
    data2 = (diag_t)uData;
    return buff_stuff(bufnum, 2u, data1, data2);
}

/**
\brief fills data from the circular buffer

Notes:
Only the hart task should get data from the buffer (which manipulates
the start pointer) and only the process task should put data into
the buffer (which manipulates the end pointer).
  The places in the code that uses both must be made thread safe.

This function is reentrant all right but if preempted by itself will
skip the values filled into another buffer (with at most one entry
getting into both buffers in the beginning and the end.
If there is ever a need to fail on second entry, write a small
wrapper to check if the routine is already running (by checking
a static volatile flag).

\param bufnum - buffer to sample into
\param count - the number of diag_t points to get
\param[out] dst - the recipent of the data array
\return the number of entries filled
*/
s16 buffer_GetDataFromXDiagnosticBuffer(u8_least bufnum, s16 count, diag_t* dst)
{
    s16 n = 0; //count the number of entries actually filled
    if((dst == NULL) || (count <= 0) || (bufnum >= NUM_DIAG_BUFFERS))
    {
        return 0; /* legitimate early return: pre-requisite check */
    }
    if(db.buf[bufnum].SamplerInfo.skip != 0)
    {
        //The buffer is sampling in linear self-pruning mode: can't give up data
        return 0; /* legitimate early return: pre-requisite check */
    }
    do
    {
        u16 pos = db.buf[bufnum].startpos;
        if(pos == db.buf[bufnum].endpos)
        {
            break; //buffer empty
        }
        else
        {
            *dst++ = db.buf[bufnum].m_DiagnosticBuffer[pos];
            db.buf[bufnum].startpos = (u16)IncrementCounter(pos); //11-bit value
            n++;
        }
    } while(n < count);
    return n;
}
/**
\brief returns the current number of entries in the diagnostic buffer
\param bufnum - buffer to sample into
\return the current number of entries in the diagnostic buffer
*/
s16 buffer_GetEntriesInXDiagnosticBuffer(u8_least bufnum)
{
    if(bufnum >= NUM_DIAG_BUFFERS)
    {
        return 0;
    }
    u16_least end = db.buf[bufnum].endpos;
    return (s16)Entries(bufnum, end);
}

/** \brief Amortized pruning: X[i] <= X[2*i] recursively from the end.
Has worst-case logarithmic complexity. Implementation should be optimized for time.
\param p - base pointer
\param point - index of place (pointed to by p) which must be relinquished
*/
static void buffer_PrunePoint32(SamplerInfo_t *SamplerInfo, u16 point)
{
    u16 npoint = point+2U;
    storeMemberInt(SamplerInfo, prune_index, npoint); //prepare next point
    u32 *p = SamplerInfo->baseptr;
    u16 prev_point;
    u32 val = p[point];
    while(/*(point != 0) &&*/ ((point & 1U) == 0)) //i.e. while the index is even
    {
        prev_point = point/2U;
        u32 prev_val = p[prev_point];
        p[prev_point] = val;
        val = prev_val;
        point = prev_point;
    }
}

//--------------- Automatic sampling --------------------------
void buffer_StopSampling(u8_least bufnum)
{
    if(bufnum >= NUM_DIAG_BUFFERS)
    {
        return; //OK to stop non-existent sampling
    }
    SamplerInfo_t *SamplerInfo = &db.buf[bufnum].SamplerInfo;
    MN_ENTER_CRITICAL();
        storeMemberInt(SamplerInfo, skip, 0);
    MN_EXIT_CRITICAL();
    //Now complete any remaining pruning
    for(u16 prune_index = SamplerInfo->prune_index; prune_index < SamplerInfo->max_points; prune_index+=2)
    {
        buffer_PrunePoint32(SamplerInfo, prune_index);
    }
    Struct_Test(SamplerInfo_t, SamplerInfo);
    //Allow circular buffer interface to read the buffer
    db.buf[bufnum].endpos = (u16)(2U*(SamplerInfo->init_points+SamplerInfo->num_points)); //valid non-losing cast: we already checked pre-requisites
}

/** \brief Reports the state of the data sampler
\param bufnum - buffer to sample into
\return a pointer to const sampler info
*/
const SamplerInfo_t *buffer_GetSamplerInfo(u8_least bufnum)
{
    if(bufnum >= NUM_DIAG_BUFFERS)
    {
        return NULL;
    }
    //do not test SamplerInfo: it is tested once in the end at StopSampling
    return &db.buf[bufnum].SamplerInfo;
}


ErrorCode_t buffer_StartSampling(u8_least bufnum,
                          taskid_t context,
                          void (*fill_func)(diag_t data[2]),
                          u16 max_points,
                          u8 init_points,
                          const dsample_t init_stuff[])
{
  //call with the default value of 1 for prune scale
  return buffer_StartSamplingExt(bufnum,
                          context,
                          fill_func,
                          max_points,
                          init_points,
                          init_stuff,
                          1u);
}

MN_DECLARE_API_FUNC(buffer_StartSamplingExt)
/** \brief Starts data sampling, always with the rate of the cycle task where dsampler_SampleData call is
\param bufnum - buffer to sample into
\param sinfo - a pointer to caller-prepared sampler configuration
\param init_stuff - the points to stuff initially (ignored if init_points=0)
*/
//void buffer_StartSampling(u8_least bufnum, u16_least max_points, u8_least init_points, const dsample_t init_stuff[], u8_least scale)
ErrorCode_t buffer_StartSamplingExt(u8_least bufnum,
                          taskid_t context,
                          void (*fill_func)(diag_t data[2]),
                          u16 max_points,
                          u8 init_points,
                          const dsample_t init_stuff[],
                          u16 InitialPruneScale)
{
    s8_least i;
    if(
       (bufnum >= NUM_DIAG_BUFFERS)
       || (fill_func == NULL)
       || ((max_points + init_points) >= DIAGNOSTIC_BUFFER_SIZE)
       || ((max_points & 0x0001U) != 0) //better yet, if agreed, ((num_points & 0x0003U) != 0x0002U)
       || (max_points < 4) //e.g. 4
       )
    {
        return ERR_INVALID_PARAMETER;
    }
    //Future; need to change initialization: Struct_Test(SamplerInfo_t, &db.buf[bufnum].SamplerInfo);
    if(db.buf[bufnum].SamplerInfo.skip != 0)
    {
        return ERR_MODE_ILLEGAL_SUBMODE; //A hijack of an error code no longer used
    }
    const SamplerInfo_t sampler =
    {
        .fill_func = fill_func,
        .baseptr = &db.buf[bufnum].m_DiagnosticBuffer[2U*init_points],
        .init_points = init_points,
        .prune_scale = InitialPruneScale,  //LS:  changed to allow initialization other than 1u
        .context = context,
        .num_points = 0,
        .max_points = max_points,
        .skip = 1,
        .prune_index = max_points,
        .CheckWord = 0, //don't care

    };
    const SamplerInfo_t *sinfo = &sampler;
    buffer_InitializeXDiagnosticBuffer(bufnum);
    MN_ENTER_CRITICAL();
        Struct_Copy(SamplerInfo_t, &db.buf[bufnum].SamplerInfo, sinfo);

        //This has to be in a critical section so as to not be interrupted by the first sample(s)
        //If need be, this can be improved (?)
        if(init_stuff != NULL)
        {
            for(i=0; i<sinfo->init_points; i++)
            {
                (void)buffer_AddXDiagnosticData2(bufnum, init_stuff[i].data1, init_stuff[i].data2);
                //Who pre-fills the buffer with too much stuff will discover it on read; no error reporting here.
            }
        }
        else
        {
            //just reserve the space
            for(i=0; i<sinfo->init_points; i++)
            {
                (void)buffer_AddXDiagnosticData2(bufnum, 0, 0);
                //Who pre-fills the buffer with too much stuff will discover it on read; no error reporting here.
            }
        }
    MN_EXIT_CRITICAL();
    return ERR_OK;
}

/** \brief Performs periodic data sampling. Stops automatically if prune scale wraps to 0 and is used.
*/
void buffer_SampleData(u8_least bufnum)
{
    SamplerInfo_t *SamplerInfo = &db.buf[bufnum].SamplerInfo;

    u16_least skip = SamplerInfo->skip;
    if(skip == 0)
    {
        return; //we are not sampling
    }
    if(!oswrap_IsContext(SamplerInfo->context))
    {
        return; //wrong context
    }

    skip--;
    if(skip == 0)
    {
        u16 num_points = SamplerInfo->num_points;
        if(SamplerInfo->num_points == SamplerInfo->max_points)
        {
            //we need to prune and delay the samples
            num_points/=2U; //restart with pruning
            storeMemberInt(SamplerInfo, prune_scale, SamplerInfo->prune_scale * 2U);
            if((num_points & 1U)==0)
            {
                //start amortized pruning with points that need to get out of the way first
                buffer_PrunePoint32(SamplerInfo, num_points);
            }
            else
            {
                storeMemberInt(SamplerInfo, prune_index, num_points+1);
            }
        }
        //else
        {
            void *baseptr = (u32 *)(SamplerInfo->baseptr) + num_points;
            num_points += 1;
            SamplerInfo->fill_func(baseptr);
        }
        storeMemberInt(SamplerInfo, num_points, num_points);
        skip = SamplerInfo->prune_scale; //set wait period until next writable sample
    }
    else
    {
        //Do amortized pruning
        u16 prune_index = SamplerInfo->prune_index; //no reason to trace odd indices;
        if(prune_index < SamplerInfo->max_points)
        {
            buffer_PrunePoint32(SamplerInfo, prune_index);
        }
    }
    storeMemberInt(SamplerInfo, skip,  skip);
}

/** \brief A wrapper for sampling all buffers
*/
void buffer_SampleAllData(void)
{
    for(u8_least bufnum=0; bufnum<NUM_DIAG_BUFFERS; bufnum++)
    {
        buffer_SampleData(bufnum);
    }
}

/** \brief suspend/resume sampling
\param bufnum - buffer id
\param new_skip - number skips to next sample
*/
static void buffer_HackSampling(u8_least bufnum, u16 new_skip)
{
    if(bufnum >= NUM_DIAG_BUFFERS)
    {
        return; //OK to suspend non-existent sampling
    }
    SamplerInfo_t *SamplerInfo = &db.buf[bufnum].SamplerInfo;
    MN_ENTER_CRITICAL();
        storeMemberInt(SamplerInfo, skip, new_skip);
    MN_EXIT_CRITICAL();
}

/** \brief suspend linear sampling
\param bufnum - buffer id
*/
void buffer_SuspendSampling(u8_least bufnum)
{
    //This may be broken by reading the buffer as circular
    buffer_HackSampling(bufnum, 0);
}

/** \brief resume linear sampling
\param bufnum - buffer id
*/
void buffer_ResumeSampling(u8_least bufnum)
{
    //Make the next samle go in the buffer
    buffer_HackSampling(bufnum, 1);
}


/* This line marks the end of the source */
