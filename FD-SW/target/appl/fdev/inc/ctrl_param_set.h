#ifndef CTRL_PARAM_SET_H_
#define CTRL_PARAM_SET_H_

#include "mnhart2ff.h"
#include <fbif.h>

#define ACTIVATE_CUSTOM_CONTROL_SET             0
#define ACTIVATE_CONTROL_SET_1                  1
#define ACTIVATE_CONTROL_SET_2                  2
#define ACTIVATE_CONTROL_SET_3                  3
#define ACTIVATE_CONTROL_SET_4                  4
#define ACTIVATE_CONTROL_SET_5                  5
#define ACTIVATE_CONTROL_SET_6                  6
#define ACTIVATE_CONTROL_SET_7                  7
#define RESTORE_CONTROL_SET                     10
#define MAKE_ACTIVE_CONTROL_AS_CUSTOM_CONTROL   11
#define DO_NOTHING                              (255u)

#define  AUTOTUNE_CANCEL        99
#define  AUTOTUNE_START         1
#define  AUTOTUNE_NOTHING       0

extern fferr_t ctrl_param_ReadActiveCtrlSet(const T_FBIF_BLOCK_INSTANCE* p_block_instance, T_FBIF_READ_DATA * p_read, void* snd_buf, void* rcv_buf);
extern fferr_t ctrl_param_ReadCtrlSetParam(const T_FBIF_BLOCK_INSTANCE* p_block_instance, USIGN16 rel_idx, void* snd_buf, void* rcv_buf);
extern fferr_t ctrl_param_ReadAutotuneParam(const T_FBIF_BLOCK_INSTANCE* p_block_instance, void* snd_buf, void* rcv_buf);
extern fferr_t ctrl_param_WriteActivate(T_FBIF_BLOCK_INSTANCE* p_block_instance, const T_FBIF_WRITE_DATA *p_write, void* snd_buf, void* rcv_buf);
extern fferr_t ctrl_param_Autotune(const T_FBIF_BLOCK_INSTANCE* p_block_instance, void* snd_buf, void* rcv_buf);
extern fferr_t ctrl_param_WriteCustom(const T_FBIF_WRITE_DATA *p_write);

#endif // CTRL_PARAM_SET_H_

