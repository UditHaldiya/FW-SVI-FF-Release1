#ifndef MN_WORKING_TIMES_H_
#define MN_WORKING_TIMES_H_

#include "ptb.h"

extern void UpdateWorkingTimes(T_FBIF_BLOCK_INSTANCE* p_block_instance);
extern void InitialWorkingTimes(const T_FBIF_BLOCK_INSTANCE * p_block_instance);
extern void SetResetFlag(void);

#endif // MN_WORKING_TIMES_H_

