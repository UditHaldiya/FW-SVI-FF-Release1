#ifndef _MN_DEVIATION_ALERT_H_
#define _MN_DEVIATION_ALERT_H_

#include "mnhart2ff.h"


extern fferr_t ffda_WriteDeviationAlert
(
    T_FBIF_BLOCK_INSTANCE* p_block_instance,
    const T_FBIF_WRITE_DATA *p_write
);

#endif //_DEVIATION_ALERT_H_
