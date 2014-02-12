#ifndef FACTORY_DEFAULTS_H_
#define FACTORY_DEFAULTS_H_

#include <softing_base.h>
#include "mnhart2ff.h"

#if 0
extern fferr_t ffres_recovery_defaults(void* snd_buf, void* rcv_buf);
#endif
extern fferr_t ffres_restart_APP(void* snd_buf, void* rcv_buf);
extern fferr_t ffres_restart_hidden(void* snd_buf, void* rcv_buf);
extern fferr_t ffres_restart_factory_defaults(void* snd_buf, void* rcv_buf);
extern fferr_t ffres_restart_restore_advanced(void);

#endif //FACTORY_DEFAULTS_H_
