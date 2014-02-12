#ifndef MNDIAGPROCFF_H_
#define MNDIAGPROCFF_H_

#include "mnhart2ff.h"

extern fferr_t ffdiag_ReadDiagnosticCommand(T_FBIF_BLOCK_INSTANCE *p_block_instance, T_FBIF_READ_DATA *p_read, void *from, void *to);
extern fferr_t ffdiag_WriteDiagnosticCommand(T_FBIF_BLOCK_INSTANCE *p_block_instance, T_FBIF_WRITE_DATA *p_write, void *from, void *to);
extern fferr_t ffdiag_ReadDiagnosticData(T_FBIF_BLOCK_INSTANCE *p_block_instance, T_FBIF_READ_DATA *p_read, void *from, void *to);

#endif //MNDIAGPROCFF_H_
