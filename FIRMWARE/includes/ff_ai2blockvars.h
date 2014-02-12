#ifndef _FF_AI2BLOCKVARS_H_
#define _FF_AI2BLOCKVARS_H_

#include "ipcdefs.h"
#include "ffdefs.h"

// FF AI2 variables. May be move to standalone file if necessary
typedef struct IPC_FFAI2Param_t
{
    ffDataFloat_t     out;
    u16 CheckWord;
} IPC_FFAI2Params_t;

extern IPC_FFAI2Params_t* GetAi2BlockVar(void);
extern ErrorCode_t  IPC_WriteAI2OUT(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs);

#endif //_FF_AI2BLOCKVARS_H_
