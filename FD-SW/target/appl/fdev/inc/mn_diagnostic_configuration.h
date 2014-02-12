#ifndef DIAGNOSTIC_CONFIGURATION_H_
#define DIAGNOSTIC_CONFIGURATION_H_

#include <ptb.h>
#include "mnhart2ff.h"

extern fferr_t ffdc_WriteDiagnostic_Configuration
(
    const T_FBIF_BLOCK_INSTANCE* p_block_instance,
    const T_FBIF_WRITE_DATA* p_write
);

#endif // DIAGNOSTIC_CONFIGURATION_H_
