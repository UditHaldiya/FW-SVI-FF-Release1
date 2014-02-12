#ifndef APP_MODE_H_
#define APP_MODE_H_

#include "ptb.h"
#include "mnhart2ff.h"

extern fferr_t ReadAPPMode(const T_FBIF_BLOCK_INSTANCE* p_block_instance, void* snd_buf, void* rcv_buf);
extern fferr_t WriteAPPMode(const T_FBIF_BLOCK_INSTANCE* p_block_instance, void* snd_buf, void* rcv_buf);

#endif // APP_MODE_H_
