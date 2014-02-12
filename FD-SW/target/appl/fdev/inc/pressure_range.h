#ifndef PRESSURE_RANGE_H_
#define PRESSURE_RANGE_H_

#include <softing_base.h>
#include "mnhart2ff.h"

#define FF_UNITS_KPA                1133u
#define FF_UNITS_PSI                1141u
#define FF_UNITS_BAR                1137u
#define PSI_MAX                     (150.0F)
#define KPA_MAX                     ((float32)(PSI_MAX*PRESSURE_FACTOR_PSI2KPA))
#define BAR_MAX                     ((float32)(PSI_MAX*PRESSURE_FACTOR_PSI2BAR))
#define PRESSURE_RANGE_DECIMAL      (3)
#define PRESSURE_FACTOR_BAR2KPA     (100.0F)
#define PRESSURE_FACTOR_KPA2BAR     (0.01F)

extern fferr_t ReadPressureRange(const T_FBIF_BLOCK_INSTANCE* p_block_instance, void* snd_buf, void* rcv_buf);
extern fferr_t WritePressureRange(const T_FBIF_BLOCK_INSTANCE* p_block_instance, void* snd_buf, void* rcv_buf);
extern fferr_t WritePressureRangeCheck(const T_FBIF_BLOCK_INSTANCE* p_block_instance, const T_FBIF_WRITE_DATA* p_write);
extern void fftbpr_PressureUnitUpdate
(
    const T_FBIF_BLOCK_INSTANCE* p_block_instance,
    u16 uLastUnit,
    u16 uCurrentUnit
);
#endif // PRESSURE_RANGE_H_
