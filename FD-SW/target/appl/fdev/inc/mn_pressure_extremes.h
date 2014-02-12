#ifndef _MN_PRESSURE_EXTREMES_H_
#define _MN_PRESSURE_EXTREMES_H_

#include <softing_base.h>
#include "mnhart2ff.h"
#include "pressuredef.h"

#define PSI_PRESSURE_MIN                 (-25.0F)
#define KPA_PRESSURE_MIN                 ((float32)(PSI_PRESSURE_MIN*PRESSURE_FACTOR_PSI2KPA))
#define BAR_PRESSURE_MIN                 ((float32)(PSI_PRESSURE_MIN*PRESSURE_FACTOR_PSI2BAR))

extern fferr_t fftbpe_WritePressureExtremes
(
    const T_FBIF_BLOCK_INSTANCE* p_block_instance,
    const T_FBIF_WRITE_DATA *p_write
);

extern void fftbpe_UpdateStRev(T_FBIF_BLOCK_INSTANCE * p_block_instance);\

#endif //_MN_PRESSURE_EXTREMES_H_

