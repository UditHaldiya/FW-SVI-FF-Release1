#ifndef _FF_POSITIONCONFVARS_H_
#define _FF_POSITIONCONFVARS_H_

#include "ipcdefs.h"
#include "ffdefs.h"

extern ErrorCode_t  IPC_WriteStopHiPos(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs);
extern ErrorCode_t  IPC_WriteStopLoPos(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs);

#endif //_FF_POSITIONCONFVARS_H_

