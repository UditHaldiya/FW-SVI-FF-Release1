#ifndef CLEAR_STATUS_H_
#define CLEAR_STATUS_H_

#include "ptb.h"
#include "mnhart2ff.h"

extern fferr_t WriteClearStatus(const T_FBIF_WRITE_DATA *p_write, void* snd_buf, void* rcv_buf);

#endif // CLEAR_STATUS_H_
