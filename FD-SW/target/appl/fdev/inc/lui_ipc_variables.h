#ifndef LUI_IPC_VARIABLES_H_
#define LUI_IPC_VARIABLES_H_

#include "ptb.h"
#include "mnhart2ff.h"

/* Definitions for command #186 */
#define CMD_186_WRITE_SHORT_VARIABLES                     186
/* Definitions for command #187 */
#define CMD_187_WRITE_ARRAY                               187
/* Definitions for command #170,176*/
#define CMD_170_176_READ_UNIT                             170

extern void cmd_186_handler(u8 * toHart, u8 *send_length);
extern void cmd_187_handler(u8 * toHart, u8 *send_length);
extern void enable_cust_conf(void);
extern fferr_t WriteUiCustomConfiguration(const T_FBIF_WRITE_DATA* p_write);
extern void cmd_170_handler
(
    T_FBIF_BLOCK_INSTANCE * p_block_instance,
    USIGN8 *from_hart,
    USIGN8 *toHart,
    USIGN8 *send_length,
    USIGN8 access
);

#endif // LUI_IPC_VARIABLES_H_
