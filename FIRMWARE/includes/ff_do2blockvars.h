#ifndef _FF_DO2BLOCKVARS_H_
#define _FF_DO2BLOCKVARS_H_

#include "ipcdefs.h"
#include "ffdefs.h"

#define FF_DO2_TAG_MAX_LEN                       FF_TAG_MAX_LEN
//#define FF_DO2_TAG_RD_MAX_BLOCK_NUM              (0u)
#define FF_DO2_TAG_WR_MAX_BLOCK_NUM              FF_TAG_WR_MAX_BLOCK_NUM

#define FF_DO2_TAG_DEF   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0

// FF DO2 variables. May be move to standalone file if necessary
typedef struct IPC_FFDO2Param_t
{
    u8                  tag[FF_DO2_TAG_MAX_LEN];
    u8                  ModeActual;
    u16                 BlockErr;
    ffDataDiscrete_t         	sp_d;
    u16 CheckWord;
} IPC_FFDO2Params_t;

extern IPC_FFDO2Params_t* GetDo2BlockVar(void);
extern ErrorCode_t  IPC_WriteDO2Tag(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs);
extern ErrorCode_t  IPC_WriteDO2Mode(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs);
extern ErrorCode_t  IPC_WriteDO2Error(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs);
extern ErrorCode_t  IPC_WriteDO2SPD(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs);

#endif
