#ifndef UI_ACCESS_CONTROL_H_
#define UI_ACCESS_CONTROL_H_

#include "ptb.h"
#include "mnhart2ff.h"

extern fferr_t ReadUIAccessControl(const T_FBIF_BLOCK_INSTANCE* p_block_instance, void* snd_buf, void* rcv_buf);
extern fferr_t WriteUIAccessControl(const T_FBIF_BLOCK_INSTANCE* p_block_instance, const T_FBIF_WRITE_DATA *p_write, void* snd_buf, void* rcv_buf);

#endif // UI_ACCESS_CONTROL_H_
