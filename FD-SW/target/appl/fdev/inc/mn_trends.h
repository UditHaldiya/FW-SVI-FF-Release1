#ifndef _MN_TREND_UTILITY_H_
#define _MN_TREND_UTILITY_H_

#include "ptb.h"

typedef struct
{
    u32 count;
    u32 sum;
} AccTrend_t;

typedef struct
{
    AccTrend_t day;
    AccTrend_t week;
    AccTrend_t month;
    AccTrend_t year;
} Trend_Counter_t;

extern void get_trend_counter_travel_acc(void** pp_value, u16* p_len);
extern void get_trend_counter_cycle_counter(void** pp_value, u16* p_len);
extern void get_trend_counter_position_err(void** pp_value,  u16* p_len);
extern void UpdateTrends(T_FBIF_BLOCK_INSTANCE * p_block_instance);

#endif //_MN_TREND_UTILITY_H_
