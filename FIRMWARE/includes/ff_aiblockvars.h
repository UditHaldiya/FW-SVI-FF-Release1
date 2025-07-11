#ifndef _FF_AIBLOCKVARS_H_
#define _FF_AIBLOCKVARS_H_

#include "ipcdefs.h"
#include "ffdefs.h"

// FF AI variables. May be move to standalone file if necessary
typedef struct IPC_FFAIParam_t
{
    ffDataFloat_t     out;
    u16 CheckWord;
} IPC_FFAIParams_t;

extern IPC_FFAIParams_t* GetAiBlockVar(void);
extern ErrorCode_t  IPC_WriteAIOUT(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs);

#endif //_FF_AIBLOCKVARS_H_
