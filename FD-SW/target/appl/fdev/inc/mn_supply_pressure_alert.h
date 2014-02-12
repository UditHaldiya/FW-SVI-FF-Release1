#ifndef _MN_SUPPLY_PRESSURE_ALERT_H_
#define _MN_SUPPLY_PRESSURE_ALERT_H_

#include "ptb.h"
#include "mnhart2ff.h"
#include "pressuredef.h"

#define PSI_PRESSURE_MAX                  (150.0F)
#define KPA_PRESSURE_MAX                  ((float32)(PSI_PRESSURE_MAX*PRESSURE_FACTOR_PSI2KPA))
#define BAR_PRESSURE_MAX                  ((float32)(PSI_PRESSURE_MAX*PRESSURE_FACTOR_PSI2BAR))

#define PSI_PRESSURE_DEADBAND_MAX         (2.0F)
#define KPA_PRESSURE_DEADBAND_MAX         ((float32)(PSI_PRESSURE_DEADBAND_MAX*PRESSURE_FACTOR_PSI2KPA))
#define BAR_PRESSURE_DEADBAND_MAX         ((float32)(PSI_PRESSURE_DEADBAND_MAX*PRESSURE_FACTOR_PSI2BAR))

#define PRESSURE_MIN                       (0.0F)

extern fferr_t fftbsp_WriteSupplyPressureAlert
(
    T_FBIF_BLOCK_INSTANCE* p_block_instance,
    const T_FBIF_WRITE_DATA *p_write
);

#endif //_MN_SUPPLY_PRESSURE_ALERT_H_
