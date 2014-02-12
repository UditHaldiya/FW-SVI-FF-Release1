#ifndef FFDEFS_H_
#define FFDEFS_H_

#define FF_TAG_MAX_LEN                       (32u)
#define FF_TAG_WR_MAX_BLOCK_NUM              (2u)
#define FF_DEVICE_TAG_LEN                    (32u)
#define FF_DEVICE_TAG_WR_MAX_BLOCK_NUM       (2u)
#define FF_DEVICE_ID_LEN                     (32u)
#define FF_DEVICE_ID_WR_MAX_BLOCK_NUM        (2u)

//---------------------------------------------------
// Fault State
// FAULT_STATE, SET_FSTATE, CLR_FSTATE
#define FF_FAULT_STATE_CLEAR                    (1u)
#define FF_FAULT_STATE_ACTIVE                   (2u)

#define FF_SET_FSTATE_OFF                       (1u)
#define FF_SET_FSTATE_SET                       (2u)
#define isSetFstate_DEFINED(x)                  ((x == FF_SET_FSTATE_OFF) || (x == FF_SET_FSTATE_SET))

#define FF_CLR_FSTATE_OFF                       (1u)
#define FF_CLR_FSTATE_CLR                       (2u)
#define isClrFstate_DEFINED(x)                  ((x == FF_CLR_FSTATE_OFF) || (x == FF_CLR_FSTATE_CLR))

#define FF_BLK_ERR_NOERR                        0

#define FLOAT_STRING_LEN        9
#define FLOAT_STRING_INIT       0,0,0,0,0,0,0,0

// Definition for Resource Block RESTART value
#define IPC_RESTART_PROCESSOR       (4u)

#define IPC_RESTART_VALUE_MIN   1
#define IPC_RESTART_VALUE_MAX   12


typedef struct
{
  u8  value[8];
  u8       status;
} ffDataFloat_t;

typedef struct
{
  s16       value;
  u8       status;
} ffDataDiscrete_t;

#endif //FFDEFS_H_

