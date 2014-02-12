#ifndef _FF_AOBLOCKVARS_H_
#define _FF_AOBLOCKVARS_H_

#include "ipcdefs.h"
#include "ffdefs.h"

#define FF_AO_TAG_MAX_LEN                       FF_TAG_MAX_LEN
//#define FF_AO_TAG_RD_MAX_BLOCK_NUM              (0u)
#define FF_AO_TAG_WR_MAX_BLOCK_NUM              FF_TAG_WR_MAX_BLOCK_NUM

#define FF_AO_TAG_DEF   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0

// FF AO variables. May be move to standalone file if necessary
typedef struct IPC_FFAOParam_t
{
    u8              tag[FF_AO_TAG_MAX_LEN];
    u8              ModeActual;
    u16             BlockErr;
    ffDataFloat_t   		sp;
    u16 CheckWord;
} IPC_FFAOParams_t;

extern IPC_FFAOParams_t* GetAoBlockVar(void);
extern ErrorCode_t  IPC_WriteAOTag(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs);
extern ErrorCode_t  IPC_WriteAOMode(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs);
extern ErrorCode_t  IPC_WriteAOError(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs);
extern ErrorCode_t  IPC_WriteAOSP(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs);

#endif
