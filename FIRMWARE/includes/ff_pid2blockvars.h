#ifndef _FF_PID2BLOCKVARS_H_
#define _FF_PID2BLOCKVARS_H_

#include "ipcdefs.h"
#include "ffdefs.h"

#define FF_PID2_TAG_MAX_LEN                       FF_TAG_MAX_LEN
//#define FF_PID2_TAG_RD_MAX_BLOCK_NUM              (0u)
#define FF_PID2_TAG_WR_MAX_BLOCK_NUM              FF_TAG_WR_MAX_BLOCK_NUM

#define FF_PID2_TAG_DEF   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0

// FF PID2 variables. May be move to standalone file if necessary
typedef struct IPC_FFPID2Param_t
{
    u8              tag[FF_PID2_TAG_MAX_LEN];
    u8              ModeActual;
    u16             BlockErr;
    ffDataFloat_t			pv;
    ffDataFloat_t     	sp;
    ffDataFloat_t     	out;
    u16             CheckWord;
} IPC_FFPID2Params_t;


extern IPC_FFPID2Params_t* GetPid2BlockVar(void);
extern ErrorCode_t  IPC_WritePID2Tag(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs);
extern ErrorCode_t  IPC_WritePID2Mode(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs);
extern ErrorCode_t  IPC_WritePID2Error(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs);
extern ErrorCode_t  IPC_WritePID2PV(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs);
extern ErrorCode_t  IPC_WritePID2SP(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs);
extern ErrorCode_t  IPC_WritePID2OUT(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs);

#endif
