#ifndef REMOTE_SENSOR_H_
#define REMOTE_SENSOR_H_

#include "ptb.h"
#include "mnhart2ff.h"

extern fferr_t ffrps_ReadAccessory(const T_FBIF_BLOCK_INSTANCE* p_block_instance, void* snd_buf, void* rcv_buf);
extern fferr_t ffrps_WriteAccessory
(
    const T_FBIF_BLOCK_INSTANCE* p_block_instance,
    const T_FBIF_WRITE_DATA* p_write,
    void* snd_buf,
    void* rcv_buf
);

#endif // REMOTE_SENSOR_H_
