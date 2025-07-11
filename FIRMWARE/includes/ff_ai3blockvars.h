#ifndef _FF_AI3BLOCKVARS_H_
#define _FF_AI3BLOCKVARS_H_

#include "ipcdefs.h"
#include "ffdefs.h"

// FF AI3 variables. May be move to standalone file if necessary
typedef struct IPC_FFAI3Param_t
{
    ffDataFloat_t     out;
    u16 CheckWord;
} IPC_FFAI3Params_t;

extern IPC_FFAI3Params_t* GetAi3BlockVar(void);
extern ErrorCode_t  IPC_WriteAI3OUT(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs);

#endif //_FF_AI2BLOCKVARS_H_
