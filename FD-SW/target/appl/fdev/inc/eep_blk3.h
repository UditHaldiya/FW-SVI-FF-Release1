#ifndef _EEP_BLK3_H_
#define _EEP_BLK3_H_

#include <ptb.h>
#include "eep_if.h"
#include "eep_blk2.h"

/*
    eep_blk3_t describes the data structure of eep block 3 which includes
    parameters of factory default value for TB block
*/
typedef struct eep_blk3_t
{
    eep_blk2_private_t eep_blk3_private_factory_default;
    T_FBIF_PTB tb_factory_default;
} eep_blk3_t;

extern T_EEP_RESULT eep_blk3_store(u16 offset, u16 len, const void* p_value);
extern T_EEP_RESULT eep_blk3_get(u16 offset, u16 len, void* p_value);
extern T_EEP_RESULT eep_blk23_transfer(u16 offsetdest, u16 offsetsrc, u16 len);
extern T_EEP_RESULT eep_blk32_transfer(u16 offsetdest, u16 offsetsrc, u16 len);
extern void eep_blk3_store_default(void);
extern u8 eep_blk3_apply(void);
extern u8 eep_blk3_check(void);

#endif //_EEP_BLK3_H_

