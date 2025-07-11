#ifndef _FF_TBBLOCKVARS_H_
#define _FF_TBBLOCKVARS_H_

#include "ipcdefs.h"
#include "ipcvarids.h"
#include "ffdefs.h"

#define FF_TB_TAG_MAX_LEN                       FF_TAG_MAX_LEN
//#define FF_TB_TAG_RD_MAX_BLOCK_NUM              (0u)
#define FF_TB_TAG_WR_MAX_BLOCK_NUM              FF_TAG_WR_MAX_BLOCK_NUM

#define FF_TB_TAG_DEF   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0

// the index of variables for LUI custom 1 and custom 2
#define CYCLEMENUFFVAR_MASK         0xFFu
#define CYCLEMENUFFVAR_WORKINGSP    1
#define CYCLEMENUFFVAR_WORKINGPOS   2
#define CYCLEMENUFFVAR_PIDSP        3
#define CYCLEMENUFFVAR_PIDPV        4
#define CYCLEMENUFFVAR_PIDOUT       5
#define CYCLEMENUFFVAR_PID2SP       6
#define CYCLEMENUFFVAR_PID2PV       7
#define CYCLEMENUFFVAR_PID2OUT      8
#define CYCLEMENUFFVAR_AIOUT        9
#define CYCLEMENUFFVAR_AI2OUT       10
#define CYCLEMENUFFVAR_AI3OUT       11
#define CYCLEMENUFFVAR_AOSP         12
#define CYCLEMENUFFVAR_DOSP         13
#define CYCLEMENUFFVAR_DO2SP        14
#define CYCLEMENUFFVAR_ISIN1        15
#define CYCLEMENUFFVAR_ISIN2        16
#define CYCLEMENUFFVAR_ISIN3        17
#define CYCLEMENUFFVAR_ISIN4        18

// FF AO variables. May be move to standalone file if necessary
typedef struct IPC_FFTBParam_t
{
    u8              tag[FF_TB_TAG_MAX_LEN];
    u8              ModeActual;
    u16             BlockErr;
    ffDataFloat_t   working_sp;
    ffDataFloat_t   working_pos;
    u8              cust_config_0;
    u8              cust_config_1;
    u16             CheckWord;
} IPC_FFTBParams_t;

extern IPC_FFTBParams_t* GetTbBlockVar(void);
extern ErrorCode_t  IPC_WriteTBTag(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs);
extern ErrorCode_t  IPC_WriteTBMode(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs);
extern ErrorCode_t  IPC_WriteTBError(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs);
extern ErrorCode_t  IPC_WriteTBFinalSP(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs);
extern ErrorCode_t  IPC_WriteTBFinalPOS(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs);
extern ErrorCode_t  IPC_WriteTBUICustConfig0(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs);
extern ErrorCode_t  IPC_WriteTBUICustConfig1(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs);

#endif
