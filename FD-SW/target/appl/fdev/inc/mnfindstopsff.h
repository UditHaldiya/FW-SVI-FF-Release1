#ifndef MNFINDSTOPSFF_H_
#define MNFINDSTOPSFF_H_

#include "mnhart2ff.h"

extern fferr_t ffposcal_ReadFindStops(T_FBIF_BLOCK_INSTANCE *p_block_instance, T_FBIF_READ_DATA *p_read, void *from, void *to);
extern fferr_t ffposcal_WriteFindStops(T_FBIF_BLOCK_INSTANCE *p_block_instance, T_FBIF_WRITE_DATA *p_write, void *from, void *to);
extern fferr_t ffposcal_ReadFindStopsInfo(T_FBIF_BLOCK_INSTANCE *p_block_instance, T_FBIF_READ_DATA *p_read, void *from, void *to);
extern fferr_t ffposcal_WriteStopPosition(T_FBIF_BLOCK_INSTANCE *p_block_instance, const T_FBIF_WRITE_DATA *p_write, void *snd_buf, void *rcv_buf);

extern fferr_t ffposcal_ReadOpenStopAdjustment(T_FBIF_BLOCK_INSTANCE *p_block_instance, T_FBIF_READ_DATA *p_read, void *from, void *to);
extern fferr_t ffposcal_WriteOpenStopAdjustment(T_FBIF_BLOCK_INSTANCE *p_block_instance, T_FBIF_WRITE_DATA *p_write, void *from, void *to);

#endif //MNFINDSTOPSFF_H_
