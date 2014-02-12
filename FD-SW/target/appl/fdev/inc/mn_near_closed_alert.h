#ifndef _MN_NEAR_CLOSED_ALERT_H_
#define _MN_NEAR_CLOSED_ALERT_H_

#include "ptb.h"
#include "mnhart2ff.h"

/* the public interfaces for this module */
extern fferr_t WriteNearClosedAlert(T_FBIF_BLOCK_INSTANCE* p_block_instance, const T_FBIF_WRITE_DATA* p_write);

#endif // _MN_NEAR_CLOSED_ALERT_H_
