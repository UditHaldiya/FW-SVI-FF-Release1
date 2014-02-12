/*
Copyright 2009 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file mn_trend_utility.c
    \brief utility about trend structure in TB block

    CPU: Any

    OWNER: EJ
    $Date: 11/01/12 11:19a $
    $Revision: 1 $
    $Author: Stanley Fu $

*/
/* $History: mn_trend_utility.c $
 *
 * *****************  Version 1  *****************
 * User: Stanley Fu    Date: 11/1/12    Time: 10:38p
 * Init version
*/
#include <limits.h>
#include <float.h>
#include <softing_base.h>
#include <ptb.h>
#include "numutils.h"
#include "mn_trends.h"
#include "eep_blk2.h"
#include "time_defs.h"

#define POSITION_ERROR_TREND_DEFINITION    (10)

typedef void func_GetDiff_t(const T_FBIF_PTB *  p_PTB, u32* pVal);
typedef u32 func_CountNewPeriodValue_t(Trend_Counter_t* p_trend_counter, u8 trend_counter_offset);

typedef struct
{
    u16 trend_offset;
    Trend_Counter_t* trend_counter;
    func_GetDiff_t* GetDiff;
    func_CountNewPeriodValue_t* CountNewPeriodValue;
    u8 rel_idx;
} TrendInterface_t;

/***********************************************************************************************/

static void GetTravelAccDiff(const T_FBIF_PTB *  p_PTB, u32* pVal)
{
    static s32 pre_pos = INT_MIN;

    *pVal = 0;
    if(INT_MIN == pre_pos)
    {
        pre_pos = (s32)p_PTB->working_pos.value;
    }
    else
    {
        *pVal = (u32)ABS(pre_pos - (s32)p_PTB->working_pos.value);
        if(*pVal > p_PTB->travel_accumulation_a_alert.deadband_ab)
        {
            pre_pos = (s32)p_PTB->working_pos.value;
        }
        else
        {
            *pVal = 0;
        }
    }
    return;
}

static void GetCycleCounterDiff(const T_FBIF_PTB *  p_PTB, u32* pVal)
{
    static float32 pre_working_pos = FLT_MAX;
    static bool_t is_positive_direction = true; //positive derection is true
    *pVal = 0;

    if(FLT_MAX > pre_working_pos)
    {
        float32 travel = p_PTB->working_pos.value - pre_working_pos;
        if(ABS(travel) > p_PTB->cycle_counter_a_alert.deadband_ab)
        {
            pre_working_pos = p_PTB->working_pos.value;

            if((travel < 0 && true == is_positive_direction) || (travel > 0 && true != is_positive_direction))
            {
                is_positive_direction = !is_positive_direction;

                *pVal = 1u;
            }
        }
    }
    else
    {
        pre_working_pos = p_PTB->working_pos.value;
    }
    return;
}

static void GetPositionErrDiff(const T_FBIF_PTB *  p_PTB, u32* pVal)
{
    float32 t_value = ABS(p_PTB->working_sp.value - p_PTB->working_pos.value) * POSITION_ERROR_TREND_DEFINITION;

    *pVal = (u32)t_value;

    return;
}

static u32 CountNewPeriodValueForTraveAcc(Trend_Counter_t* p_trend_counter, u8 trend_counter_offset)
{
    AccTrend_t* p_acc_trend = (AccTrend_t*)(void*)((u8*)p_trend_counter + trend_counter_offset);

    return p_acc_trend->sum;
}

static u32 CountNewPeriodValueForCycleCounter(Trend_Counter_t* p_trend_counter, u8 trend_counter_offset)
{
    AccTrend_t* p_acc_trend = (AccTrend_t*)(void*)((u8*)p_trend_counter + trend_counter_offset);

    return p_acc_trend->sum;
}

static u32 CountNewPeriodValueForPositionErr(Trend_Counter_t* p_trend_counter, u8 trend_counter_offset)
{
    AccTrend_t* p_acc_trend = (AccTrend_t*)(void*)((u8*)(p_trend_counter) + trend_counter_offset);

    float32 t = ((float32)p_acc_trend->sum/POSITION_ERROR_TREND_DEFINITION)/p_acc_trend->count;
    void* p_result = &t;
    u32* result = p_result;
    return *result;
}

static Trend_Counter_t trend_counter_travel_acc;
static Trend_Counter_t trend_counter_cycle_counter;
static Trend_Counter_t trend_counter_position_err;

void get_trend_counter_travel_acc(void** pp_value, u16* p_len)
{
    *pp_value = &trend_counter_travel_acc;
    *p_len = sizeof(trend_counter_travel_acc);
}

void get_trend_counter_cycle_counter(void** pp_value, u16* p_len)
{
    *pp_value = &trend_counter_cycle_counter;
    *p_len = sizeof(trend_counter_cycle_counter);
}

void get_trend_counter_position_err(void** pp_value, u16* p_len)
{
    *pp_value = &trend_counter_position_err;
    *p_len = sizeof(trend_counter_position_err);
}

/*Trend parameters in this table will be constantly calculated*/
const TrendInterface_t trend_list[] =
{
    {MN_OFFSETOF(T_FBIF_PTB, travel_accumulation_trend), &trend_counter_travel_acc,     GetTravelAccDiff,    CountNewPeriodValueForTraveAcc,     REL_IDX_PTB_TRAVEL_ACCUMULATION_TREND},
    {MN_OFFSETOF(T_FBIF_PTB, cycle_counter_trend),       &trend_counter_cycle_counter,  GetCycleCounterDiff, CountNewPeriodValueForCycleCounter, REL_IDX_PTB_CYCLE_COUNTER_TREND},
    {MN_OFFSETOF(T_FBIF_PTB, position_error_trend),        &trend_counter_position_err,   GetPositionErrDiff,  CountNewPeriodValueForPositionErr,  REL_IDX_PTB_POSITION_ERROR_TREND},
};

/***********************************************************************************************/

static void UpdateTrendDay(const TrendInterface_t* p_trend_interface, _TREND* p_trend)
{
    p_trend->three_days_ago = p_trend->previous_day;
    p_trend->previous_day = p_trend->last_day;
    p_trend->last_day = p_trend->today;
    p_trend->today = p_trend->currently_collected;
    p_trend->currently_collected = 0u;

    p_trend_interface->trend_counter->day.sum = 0;
    p_trend_interface->trend_counter->day.count = 0;
}

static void UpdateTrendWeek(const TrendInterface_t* p_trend_interface, _TREND* p_trend)
{
    p_trend->three_weeks_ago = p_trend->previous_week;
    p_trend->previous_week = p_trend->last_week;
    p_trend->last_week = p_trend->current_week;
    p_trend->current_week = p_trend_interface->CountNewPeriodValue(p_trend_interface->trend_counter, MN_OFFSETOF(Trend_Counter_t, week));

    p_trend_interface->trend_counter->week.sum = 0;
    p_trend_interface->trend_counter->week.count = 0;
}

static void UpdateTrendMonth(const TrendInterface_t* p_trend_interface, _TREND* p_trend)
{
    p_trend->three_months_ago = p_trend->previous_month;
    p_trend->previous_month = p_trend->last_month;
    p_trend->last_month = p_trend->current_month;
    p_trend->current_month = p_trend_interface->CountNewPeriodValue(p_trend_interface->trend_counter, MN_OFFSETOF(Trend_Counter_t, month));

    p_trend_interface->trend_counter->month.sum = 0;
    p_trend_interface->trend_counter->month.count = 0;
}

static void UpdateTrendYear(const TrendInterface_t* p_trend_interface, _TREND* p_trend)
{
    p_trend->three_years_ago = p_trend->previous_12_months;
    p_trend->previous_12_months = p_trend->last_12_months;
    p_trend->last_12_months = p_trend->current_12_months;
    p_trend->current_12_months = p_trend_interface->CountNewPeriodValue(p_trend_interface->trend_counter, MN_OFFSETOF(Trend_Counter_t, year));

    p_trend_interface->trend_counter->year.sum = 0;
    p_trend_interface->trend_counter->year.count = 0;
}
// Update element "CURRENTLY COLLECTION" of the record
static void UpdateTrendCurrent(const TrendInterface_t* p_trend_interface, T_FBIF_PTB *  p_PTB, u32 diff)
{
    _TREND* p_trend = (_TREND*)(void*)((u8*)p_PTB + p_trend_interface->trend_offset);

    p_trend_interface->trend_counter->day.count += 1;

    p_trend_interface->trend_counter->day.sum += diff;

    p_trend->currently_collected = p_trend_interface->CountNewPeriodValue(p_trend_interface->trend_counter, MN_OFFSETOF(Trend_Counter_t, day));

    if (p_trend_interface->trend_counter->day.count >= TIME_INSECOND_ONEDAY)
    {
        p_trend_interface->trend_counter->week.sum += p_trend_interface->trend_counter->day.sum;
        p_trend_interface->trend_counter->month.sum += p_trend_interface->trend_counter->day.sum;
        p_trend_interface->trend_counter->year.sum += p_trend_interface->trend_counter->day.sum;

        p_trend_interface->trend_counter->week.count += p_trend_interface->trend_counter->day.count;
        p_trend_interface->trend_counter->month.count += p_trend_interface->trend_counter->day.count;
        p_trend_interface->trend_counter->year.count += p_trend_interface->trend_counter->day.count;

        UpdateTrendDay(p_trend_interface, p_trend);
    }
    if (p_trend_interface->trend_counter->week.count >= TIME_INSECOND_ONEWEEK)
    {
        UpdateTrendWeek(p_trend_interface, p_trend);
    }
    if (p_trend_interface->trend_counter->month.count >= TIME_INSECOND_ONEMONTH)
    {
        UpdateTrendMonth(p_trend_interface, p_trend);
    }
    if (p_trend_interface->trend_counter->year.count >= TIME_INSECOND_ONEYEAR)
    {
        UpdateTrendYear(p_trend_interface, p_trend);
    }

}

#if 1
/* \brief Since trend set is changed by itself, not by user
            so the storage is not handled by Write_handler_PTB()
            and we need to do it manually
    param in:
        p_PTB: pointer to the objects of transducer block
    return:
        void
*/
static u16 OnWriteTrend(T_FBIF_BLOCK_INSTANCE* p_block_instance, u16 trend_offset, u8 rel_idx)
{
    T_FBS_WRITE_PARAM_LOCAL write_loc;
    T_FBIF_PTB *  p_PTB = p_block_instance->p_block_desc->p_block;

    write_loc.rel_idx = rel_idx;
    /* Store out parameter in non-volatile RAM ---------------------- */
    write_loc.subindex      = 0;
    write_loc.length        = MN_OFFSETOF(_TREND, three_years_ago) + sizeof(u32);
    write_loc.source        =  ((u8*)p_PTB + trend_offset);

    if (appl_check_hm_state() == 0x01 )
    {
      write_loc.startup_sync = FALSE;
    }
    else
    {
      write_loc.startup_sync = TRUE;
    }

    return fbs_write_param_loc (p_block_instance, &write_loc);
}
#endif

/* \brief update the trend of positon error about every 1 second
            when transducer block excute once
    param in:
        p_block_instance: pointer to the instance of transducer block
    return:
        void
*/
void UpdateTrends(T_FBIF_BLOCK_INSTANCE * p_block_instance)
{
    u32         TimeDiff;
    u32         CurrentTime;
    static u32      LastTime = 0;
    T_FBIF_PTB *    p_PTB = p_block_instance->p_block_desc->p_block;
    u32         diff;

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

        if (   (MODE_AUTO == p_PTB->mode_blk.actual)
            || (MODE_LO == p_PTB->mode_blk.actual)
            || (MODE_MAN == p_PTB->mode_blk.actual) )
        {
            if (  (SQ_GOOD == (p_PTB->working_pos.status & QUALITY_MASK) ) // SQ_GOOD_CAS?
                &&(SQ_GOOD == (p_PTB->working_sp.status & QUALITY_MASK)) )
            {
                for(u16 i = 0; i < sizeof(trend_list)/sizeof(TrendInterface_t); i++)
                {
                    trend_list[i].GetDiff(p_PTB, &diff);

                    UpdateTrendCurrent(&trend_list[i], p_PTB, diff);
                }

                if (0 == (CurrentTime % 100) )
                {
                    for(u16 i = 0; i < sizeof(trend_list)/sizeof(TrendInterface_t); i++)
                    {
                        if(E_OK != OnWriteTrend(p_block_instance, trend_list[i].trend_offset, trend_list[i].rel_idx))
                        {
                            return;
                        }
                    }
                    (void)eep_blk2_store(MN_OFFSETOF(eep_blk2_t, eep_blk2_private)+MN_OFFSETOF(eep_blk2_private_t, trend_counter_travel_acc), sizeof(Trend_Counter_t), (void*)&trend_counter_travel_acc);
                    (void)eep_blk2_store(MN_OFFSETOF(eep_blk2_t, eep_blk2_private)+MN_OFFSETOF(eep_blk2_private_t, trend_counter_cycle_counter), sizeof(Trend_Counter_t), (void*)&trend_counter_cycle_counter);
                    (void)eep_blk2_store(MN_OFFSETOF(eep_blk2_t, eep_blk2_private)+MN_OFFSETOF(eep_blk2_private_t, trend_counter_position_err), sizeof(Trend_Counter_t), (void*)&trend_counter_position_err);
                }
            } //SQ_GOOD
        } //p_PTB->mode_blk.actual
    }//TimeDiff
}

