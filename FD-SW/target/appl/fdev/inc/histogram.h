#ifndef _HISTOGRAM_H_
#define _HISTOGRAM_H_

#include "ptb.h"
#include "mnhart2ff.h"

#define INDEX_POSERR_COMP11 11
#define SIZE_POSERR_COMP    (INDEX_POSERR_COMP11+1)
typedef struct
{
    u32     sum1;
    u32     sumAll;
    u32     n;
} Histogram_PosErr_t;

#define NUM_OF_POSITION_HISTOGRAM_ITEM 12
/*
typedef struct
{
    u32 zero_to_5;
    u32 five_to_10;
    u32 ten_to_20;
    u32 twenty_to_30;
    u32 thirty_to_40;
    u32 forty_to_50;
    u32 fifty_to_60;
    u32 sixty_to_70;
    u32 seventy_to_80;
    u32 eighty_to_90;
    u32 ninety_to_95;
    u32 ninety_five_to_100;
} Position_Histogram_internal_t;
*/

/* the public interfaces for this module */
extern void UpdateHistogram(T_FBIF_BLOCK_INSTANCE * p_block_instance);
extern u16  OnWritePosErrHistogram(const T_FBIF_PTB *  p_PTB);
extern void get_Histogram_PosErr(void** pp_value, u16* p_len);
extern void get_Position_Histogram_internal(void** pp_value, u16* p_len);
extern fferr_t  OnWritePositionHistogram(const T_FBIF_WRITE_DATA* p_write);
extern fferr_t  OnReadPositionHistogram(const T_FBIF_BLOCK_INSTANCE* p_block_instance);

#endif // _HISTOGRAM_H_

