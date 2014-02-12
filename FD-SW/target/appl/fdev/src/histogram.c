/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file histogram.c
    \brief "histogram parameters of transduser block" handler

    CPU: Any

    OWNER: EJ
    $Date: 09/27/12 11:19a $
    $Revision: 1 $
    $Author: Eric J $

*/
/* $History: histogram.c $
 *
 * *****************  Version 1  *****************
 * User: Eric J    Date: 09/27/12    Time: 10:38p
 * Init version
*/

#include <softing_base.h>
#include <ptb.h>
#include "numutils.h"
#include "histogram.h"
#include "eep_blk2.h"
#include "time_defs.h"

  #define  MODULE_ID      (COMP_FAPP + MOD_APPL_TRN)

// for positon error histogram,
#define INDEX_POSERR_COMP0  0
/*
#define INDEX_POSERR_COMP1  1
#define INDEX_POSERR_COMP2  2
#define INDEX_POSERR_COMP3  3
#define INDEX_POSERR_COMP4  4
#define INDEX_POSERR_COMP5  5
#define INDEX_POSERR_COMP6  6
#define INDEX_POSERR_COMP7  7
#define INDEX_POSERR_COMP8  8
#define INDEX_POSERR_COMP9  9
*/
#define INDEX_POSERR_COMP10 10
#define INTERVAL_OF_STORE   40

#define ERROR_HISTOGRAM_DEFINITION    (10)

#define HISTOGRAM_POSERR_M  0xFFFFFFA
#define HISTOGRAM_POSERR_X  (HISTOGRAM_POSERR_M * 2)

#define ABSVAL(x)       ((x) >= 0.0 ? (x) : (-(x)))

/* the cache for history positon err, the sum and counter */
static Histogram_PosErr_t Histogram_PosErr[SIZE_POSERR_COMP];
/* the cache for posistion histogram internal counter value*/
static u32 Position_Histogram_internal_array[NUM_OF_POSITION_HISTOGRAM_ITEM];

static u32 writeCounter = 0;

void get_Histogram_PosErr(void** pp_value, u16* p_len)
{
    *pp_value = Histogram_PosErr;
    *p_len = sizeof(Histogram_PosErr);
}

void get_Position_Histogram_internal(void** pp_value, u16* p_len)
{
    *pp_value = Position_Histogram_internal_array;
    *p_len = sizeof(Position_Histogram_internal_array);
}

static void histogram_UpdatePosition(const T_FBIF_PTB *  p_PTB)
{
    s32     scaledPostion;
#if 1
    if (MODE_AUTO != p_PTB->mode_blk.actual)
    {
        return;
    }

    if (SQ_GOOD != (p_PTB->working_pos.status & QUALITY_MASK) ) // SQ_GOOD_CAS?
    {
        return;
    }
#endif
    scaledPostion = (s32)(p_PTB->working_pos.value);

/*      component_0     TOTAL
        component_1     5%-CLOSED
        component_2    < 10%
        component_3    < 20%
        component_4    < 30%
        component_5    < 40%
        component_6    < 50%
        component_7    < 60%
        component_8    < 70%
        component_9    < 80%
        component_10   < 90%
        component_11   < 95%
        component_12   < 95%-OPEN   */

    // TOTAL
    //p_PTB->position_histogram.component_0 += 1;

    osif_disable_all_tasks();
    // 5% - close
    if (scaledPostion <= 4)
    {
        //p_PTB->position_histogram.component_1 += 1;
        Position_Histogram_internal_array[0]++;
    }

    // 10% - 90%
    else if (scaledPostion <= 89)
    {
        s32 index = scaledPostion / 10 + 1;
        Position_Histogram_internal_array[index]++;
    }

    // 90% - 95%
    else if (scaledPostion <= 94)
    {
        Position_Histogram_internal_array[10]++;
    }
    // 95% - open
    else
    {
        Position_Histogram_internal_array[11]++;
    }
    osif_enable_all_tasks();
}

static void histogram_UpdateTotalTime(T_FBIF_PTB *  p_PTB)
{
#if 1
    if (MODE_AUTO != p_PTB->mode_blk.actual)
    {
        return;
    }
    if (SQ_GOOD != (p_PTB->working_pos.status & QUALITY_MASK) ) // SQ_GOOD_CAS?
    {
        return;
    }
#endif
    // TOTAL_TIME
    p_PTB->position_histogram.component_0 += 1;
}

static void histogram_CalPoserrComponent(s32 index, float32 *pData, u32 err)
{
    if(index >= SIZE_POSERR_COMP)
    {
        return;
    }

    osif_disable_all_tasks();

    if (Histogram_PosErr[index].n < HISTOGRAM_POSERR_M)
    {
        Histogram_PosErr[index].sum1 += err;
    }
    else
    {
        Histogram_PosErr[index].sumAll += err;
    }
    Histogram_PosErr[index].n += 1;

    if (Histogram_PosErr[index].n >= HISTOGRAM_POSERR_X)
    {
        Histogram_PosErr[index].n -= HISTOGRAM_POSERR_M;
        Histogram_PosErr[index].sum1 = Histogram_PosErr[index].sumAll;
        Histogram_PosErr[index].sumAll = 0;
    }

    *pData = (((float32)Histogram_PosErr[index].sum1 + Histogram_PosErr[index].sumAll) / ERROR_HISTOGRAM_DEFINITION) / Histogram_PosErr[index].n;
    osif_enable_all_tasks();
}

static void histogram_UpdatePositionErr(T_FBIF_PTB *  p_PTB)
{
    u32         err;
    s32         scaledPosition;

    if (MODE_AUTO != p_PTB->mode_blk.actual)
    {
        return;
    }

    if (SQ_GOOD != (p_PTB->working_pos.status & QUALITY_MASK) ) // SQ_GOOD_CAS?
    {
        return;
    }

    if (SQ_GOOD != (p_PTB->working_sp.status & QUALITY_MASK) ) // SQ_GOOD_CAS?
    {
        return;
    }

    // position error
    float32 fVal = p_PTB->working_sp.value - p_PTB->working_pos.value;
    fVal = ABSVAL(fVal) * ERROR_HISTOGRAM_DEFINITION;
    err = (u32)fVal;
    scaledPosition = (s32)(p_PTB->working_pos.value);

/* component_0    5%-CLOSED
   component_1  < 10%
   component_2  < 20%
   component_3  < 30%
   component_4  < 40%
   component_5  < 50%
   component_6  < 60%
   component_7  < 70%
   component_8  < 80%
   component_9  < 90%
   component_10 < 95%
   component_11   95%-OPEN  */

    // 5% - close
    if (scaledPosition <= 4)
    {
        histogram_CalPoserrComponent(INDEX_POSERR_COMP0, &p_PTB->position_error_histogram.component_0, err);
    }
    // 10% - 90%
    else if (scaledPosition <= 89)
    {
        struct buckets_t
        {
            float32 bucket[12];
        } *p = (void *)&p_PTB->position_error_histogram;

        //Assert my assumptions:
        //1. (No) padding
        CONST_ASSERT(sizeof(*p) == sizeof(p_PTB->position_error_histogram));
        //2. Alignment
        //MN_DBG_ASSERT(p->bucket[0] == p_PTB->position_error_histogram.component_0);

        s32 index = scaledPosition / 10 + 1;
        histogram_CalPoserrComponent(index, (float32*)&p->bucket[index], err);

//      histogram_CalPoserrComponent((scaledPosition/10 + 1, (&(p_PTB->position_error_histogram.component_1) + scaledPosition/10), err);
    }
    // 95%
    else if (scaledPosition <= 94)
    {
        histogram_CalPoserrComponent(INDEX_POSERR_COMP10, &p_PTB->position_error_histogram.component_10, err);
    }
    // 95% - open
    else
    {
        histogram_CalPoserrComponent(INDEX_POSERR_COMP11, &p_PTB->position_error_histogram.component_11, err);
    }
}


/* \brief Since histogram parameters changed by itself, not by user
            so the storage is not handled by Write_handler_PTB()
            and we need to do it manually
    param in:
        p_PTB: pointer to the objects of transducer block
    return:
        void
*/
static void histogram_WriteNvram(T_FBIF_BLOCK_INSTANCE * p_block_instance)
{
    T_FBS_WRITE_PARAM_LOCAL write_loc;
    T_FBIF_PTB *  p_PTB;

    p_PTB = p_block_instance->p_block_desc->p_block;

    write_loc.rel_idx = REL_IDX_PTB_POSITION_ERROR_HISTOGRAM;
    /* Store out parameter in non-volatile RAM ---------------------- */
    write_loc.subindex      = 0;
    write_loc.length        = MN_OFFSETOF(_POS_ERR_HISTOGRAM, component_11) + sizeof(p_PTB->position_error_histogram.component_11);
    write_loc.source        = (USIGN8 *)&p_PTB->position_error_histogram;
    if (appl_check_hm_state() == HM_RUN )
    {
      write_loc.startup_sync = FALSE;
    }
    else
    {
      write_loc.startup_sync = TRUE;
    }
    (void)fbs_write_param_loc (p_block_instance,&write_loc);

    write_loc.rel_idx = REL_IDX_PTB_POSITION_HISTOGRAM;
    /* Store out parameter in non-volatile RAM ---------------------- */
    write_loc.subindex      = 0;
    write_loc.length        = sizeof(p_PTB->position_histogram);
    write_loc.source        = (USIGN8 *)&p_PTB->position_histogram;
    if (appl_check_hm_state() == HM_RUN )
    {
      write_loc.startup_sync = FALSE;
    }
    else
    {
      write_loc.startup_sync = TRUE;
    }
    (void)fbs_write_param_loc (p_block_instance,&write_loc);
}

/* \brief update the histogram of positon, position err
            when transducer block excute once
    param in:
        p_PTB: pointer to the objects of transducer block
    return:
        void
*/
void UpdateHistogram(T_FBIF_BLOCK_INSTANCE * p_block_instance)
{
    u32         TimeDiff;
    u32         CurrentTime;
    static u32      LastTime = 0;

    CurrentTime = osif_get_time_in_ms();
    TimeDiff = TIME_DIFF(LastTime,CurrentTime);

    // less than 1 second, do nothing
    if (TimeDiff < NUMBER_OF_MS_IN_SECOND)
    {
    }
    // when time changes too much, give up the conllection, only happen at power up
    else if (TimeDiff > (3u * NUMBER_OF_MS_IN_SECOND))
    {
        LastTime = CurrentTime;
    }
    else
    {
        LastTime += NUMBER_OF_MS_IN_SECOND;
        histogram_UpdateTotalTime(p_block_instance->p_block_desc->p_block);
        histogram_UpdatePosition(p_block_instance->p_block_desc->p_block);
        histogram_UpdatePositionErr(p_block_instance->p_block_desc->p_block);

        writeCounter ++;
        if (writeCounter >= INTERVAL_OF_STORE)
        {
            writeCounter = 0;
            histogram_WriteNvram(p_block_instance);
            //lint -esym(458,Histogram_PosErr) The variable will be read in critical section in the lower level
            (void)eep_blk2_store(MN_OFFSETOF(eep_blk2_t, eep_blk2_private)+MN_OFFSETOF(eep_blk2_private_t, Histogram_PosErr), sizeof(Histogram_PosErr), (void*)Histogram_PosErr);
            //lint -esym(458,Position_Histogram_internal_array) The variable will be read in critical section in the lower level
            (void)eep_blk2_store(MN_OFFSETOF(eep_blk2_t, eep_blk2_private)+MN_OFFSETOF(eep_blk2_private_t, Position_Histogram_internal_array), sizeof(Position_Histogram_internal_array), (void*)Position_Histogram_internal_array);
        }
    }
}

/* \brief reset the internal sum and counter for position error histogram
    param in:
        p_PTB: pointer to the objects of transducer block
    return:
        void
*/
u16 OnWritePosErrHistogram(const T_FBIF_PTB *  p_PTB)
{
    u32 i;
    struct buckets_t
    {
        float32 bucket[12];
    } const *p = (const void *)&p_PTB->position_error_histogram;

    //Assert my assumptions:
    //1. (No) padding
    CONST_ASSERT(sizeof(*p) == sizeof(p_PTB->position_error_histogram));
    //2. Alignment
    //MN_DBG_ASSERT(p->bucket[0] == p_PTB->position_error_histogram.component_0);

    osif_disable_all_tasks();
    for (i=0; i< SIZE_POSERR_COMP; i += 1)
    {
        Histogram_PosErr[i].n = 1;
        float32 t = p->bucket[i] * ERROR_HISTOGRAM_DEFINITION;
        Histogram_PosErr[i].sum1 = (u32)t;
//      Histogram_PosErr[i].sum1 = *( &(p_PTB->position_error_histogram.component_0) + i );
        Histogram_PosErr[i].sumAll = 0;
    }
    osif_enable_all_tasks();
    return E_OK;
}

/* \brief when user write POSITION_HISTOGRAM, this function will be invoked to check the values
    param in:
        T_FBIF_BLOCK_INSTANCE: function block pointer
        T_FBIF_WRITE_DATA: write structure
    return:
        fferr
*/
fferr_t  OnWritePositionHistogram(const T_FBIF_WRITE_DATA* p_write)
{
    fferr_t fferr = E_OK;

    switch (p_write->subindex)
    {
        case 0:
        {
            _POS_HISTOGRAM* p_pos_histogram= (_POS_HISTOGRAM*)(void*)p_write->source;

            if(0 != p_pos_histogram->component_0)
            {
                fferr=E_ERROR;
            }
            break;
        }
        case 1:
        {
            u32* p_total_time = (u32*)(void*)p_write->source;

            if(0 != *p_total_time)
            {
                fferr=E_ERROR;
            }
            break;
        }
        default:
        {
            fferr=E_ERROR;
        }
    }

    if(E_OK == fferr)
    {
        osif_disable_all_tasks();
        for(u16 i=0; i<NUM_OF_POSITION_HISTOGRAM_ITEM; i++)
        {
            Position_Histogram_internal_array[i] = 0;
        }
        osif_enable_all_tasks();
        //lint -esym(458,Position_Histogram_internal_array) The variable will be read in critical section in the lower level
        (void)eep_blk2_store(MN_OFFSETOF(eep_blk2_t, eep_blk2_private)+MN_OFFSETOF(eep_blk2_private_t, Position_Histogram_internal_array), sizeof(Position_Histogram_internal_array), (void*)Position_Histogram_internal_array);

    }

    return fferr;
}

/* \brief when user read POSITION_HISTOGRAM, this function will be invoked to convert the values
    param in:
        T_FBIF_BLOCK_INSTANCE: function block pointer
    return:
        fferr
*/
fferr_t  OnReadPositionHistogram(const T_FBIF_BLOCK_INSTANCE* p_block_instance)
{
    T_FBIF_PTB* p_PTB = p_block_instance->p_block_desc->p_block;

    u32 sum = 0;

    osif_disable_all_tasks();
    for(u16 i=0; i<NUM_OF_POSITION_HISTOGRAM_ITEM; i++)
    {
        sum += Position_Histogram_internal_array[i];
    }
    osif_enable_all_tasks();

    //float p_pos_histogram_item_start = (void *)&(p_PTB->position_histogram.component_1);
    struct buckets_t
    {
        u32 total_time;
        float32 bucket[NUM_OF_POSITION_HISTOGRAM_ITEM];
    } *p_external = (void *)&(p_PTB->position_histogram);

    UNUSED_OK(p_external->total_time);
    osif_disable_all_tasks();
    if(0 != sum)
    {
        for(u16 i=0; i<NUM_OF_POSITION_HISTOGRAM_ITEM; i++)
        {
            p_external->bucket[i] = (float32)Position_Histogram_internal_array[i] / sum;
        }
    }
    else
    {
        for(u16 i=0; i<NUM_OF_POSITION_HISTOGRAM_ITEM; i++)
        {
            p_external->bucket[i] = 0.0f;
        }
    }
    osif_enable_all_tasks();
    return E_OK;
}




