#ifndef _MN_ADVANCED_H
#define _MN_ADVANCED_H
#include <ptb.h>
#include "mnhart2ff.h"

typedef enum {a_false, a_true} advanced_state;
extern fferr_t ffcheck_WriteAdvanced(const T_FBIF_BLOCK_INSTANCE * p_block_instance, T_FBIF_WRITE_DATA *p_write);
extern fferr_t ffcheck_WriteFilter(const T_FBIF_BLOCK_INSTANCE * p_block_instance, const T_FBIF_WRITE_DATA *p_write);

extern void ffcheck_AdvancedOnStartUp(void);
extern advanced_state ffcheck_GetAdvancedFlag(void);

#endif //_MN_ADVANCED_H
