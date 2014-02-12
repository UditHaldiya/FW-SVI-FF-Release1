#ifndef _MN_ACTUATOR_3_H_
#define _MN_ACTUATOR_3_H_

#include "ptb.h"
#include "mnhart2ff.h"

#define SINGLE_ACTING       1u
#define DOUBLE_ACTING       0u

//lint ++flb
#define MN_AIR_TO_CLOSE         0
#define MN_AIR_TO_OPEN          1
#define MN_DDVALUE_AIR_TO_CLOSE 2
//lint --flb

extern fferr_t ReadActuator3(const T_FBIF_BLOCK_INSTANCE* p_block_instance, void* snd_buf, void* rcv_buf);
extern fferr_t WriteActuator3(T_FBIF_BLOCK_INSTANCE* p_block_instance, const T_FBIF_WRITE_DATA *p_write, void* snd_buf, void* rcv_buf);

#endif // _MN_ACTUATOR_3_H_

