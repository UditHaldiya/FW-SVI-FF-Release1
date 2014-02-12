#ifndef _EEP_BLK2_H_
#define _EEP_BLK2_H_

#include "eep_if.h"
#include "mn_trends.h"
#include "histogram.h"
#include "appl_alert.h"

/*
    eep_blk2_private_t is designed to contain all internal data which should be non-volatile. These data will
    be loaded to RAM when system startups.
    If any new structures need to be saved in block2, the structures should be appended to this.
*/
typedef struct eep_blk2_private_t
{
    Trend_Counter_t trend_counter_travel_acc;
    Trend_Counter_t trend_counter_cycle_counter;
    Trend_Counter_t trend_counter_position_err;
    Histogram_PosErr_t Histogram_PosErr[SIZE_POSERR_COMP];
    AlertStoreStruc_t   AlertLog;
    u32 Position_Histogram_internal_array[NUM_OF_POSITION_HISTOGRAM_ITEM];
} eep_blk2_private_t;

/*
    eep_blk2_t describes the data structure of eep block 2 which includes private data, private data factory default value and
    parameters of factory default value for different block
*/
typedef struct eep_blk2_t
{
    eep_blk2_private_t eep_blk2_private;
} eep_blk2_t;

extern void eep_blk2_store_default(void);
extern T_EEP_RESULT eep_blk2_store(u16 offset, u16 len, const void* p_value);
extern u8 eep_blk2_apply(void);
extern u8 eep_blk2_check(void);

#endif //_EEP_BLK2_H_
