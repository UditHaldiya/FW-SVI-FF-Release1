#ifndef UI_LANGUAGE_H_
#define UI_LANGUAGE_H_

#include "ptb.h"
#include "mnhart2ff.h"

extern fferr_t ReadUILanguage(const T_FBIF_BLOCK_INSTANCE* p_block_instance, void* snd_buf, void* rcv_buf);
extern fferr_t WriteUILanguage(const T_FBIF_BLOCK_INSTANCE* p_block_instance, void* snd_buf, void* rcv_buf);

#endif // UI_LANGUAGE_H_
