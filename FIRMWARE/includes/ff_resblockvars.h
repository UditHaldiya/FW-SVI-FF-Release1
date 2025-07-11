#ifndef _FF_RESBLOCKVARS_H_
#define _FF_RESBLOCKVARS_H_

#include "ipcdefs.h"
#include "ffdefs.h"
#include "ipcvarids.h"

#define FF_RB_TAG_MAX_LEN                       FF_TAG_MAX_LEN
//#define FF_RB_TAG_RD_MAX_BLOCK_NUM              (0u)
#define FF_RB_TAG_WR_MAX_BLOCK_NUM              FF_TAG_WR_MAX_BLOCK_NUM

// #define SPACE   0x20u
#define FF_RB_TAG_DEF   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0

// FF RB variables. May be move to standalone file if necessary
typedef struct IPC_FFResourceParam_t
{
    u8          tag[FF_RB_TAG_MAX_LEN];
    u8          ModeActual;
    u8          SetFstate;
    u8          ClrFstate;
    u16         BlockErr;
    u16 CheckWord;
} IPC_FFResourceParams_t;

// for lint comment these 2 lines
//extern const void *GetResBlockTag(void *dst);
//extern ErrorCode_t SetResBlockTag(const void *src);

extern ErrorCode_t  IPC_WriteRbTag(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs);
//extern ErrorCode_t  IPC_ReadRbTag(IPC_Variable_IDs_t VarID, IPC_ReadPtrs_t const *pIPC_ReadPtrs);
extern ErrorCode_t  IPC_ReadRbRestart(IPC_Variable_IDs_t VarID, IPC_ReadPtrs_t const *pIPC_ReadPtrs);
extern ErrorCode_t  IPC_WriteRbRestart(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs);
extern ErrorCode_t  IPC_ReadFaultState(IPC_Variable_IDs_t VarID, IPC_ReadPtrs_t const *pIPC_ReadPtrs);
extern ErrorCode_t  IPC_ReadRbWrongCfg(IPC_Variable_IDs_t VarID, IPC_ReadPtrs_t const *pIPC_ReadPtrs);
extern ErrorCode_t  IPC_WriteClearFaultState(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs);
extern ErrorCode_t  IPC_ReadClearFaultState(IPC_Variable_IDs_t VarID, IPC_ReadPtrs_t const *pIPC_ReadPtrs);
extern ErrorCode_t  IPC_WriteSetFaultState(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs);
extern ErrorCode_t  IPC_ReadSetFaultState(IPC_Variable_IDs_t VarID, IPC_ReadPtrs_t const *pIPC_ReadPtrs);
extern ErrorCode_t  IPC_ReadRbBlockErrDesc1(IPC_Variable_IDs_t VarID, IPC_ReadPtrs_t const *pIPC_ReadPtrs);
//extern ErrorCode_t  IPC_ReadRbModeActual(IPC_Variable_IDs_t VarID, IPC_ReadPtrs_t const *pIPC_ReadPtrs);
extern ErrorCode_t  IPC_WriteRbModeActual(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs);
extern ErrorCode_t  IPC_WriteRbError(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs);

extern const IPC_FFResourceParams_t* GetResBlockVar(void);

#endif
/* This line marks the end of the source */
