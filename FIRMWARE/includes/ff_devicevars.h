#ifndef _FF_DEVICEVARS_H_
#define _FF_DEVICEVARS_H_

#include "ipcdefs.h"
#include "ffdefs.h"
#include "ipcvarids.h"

#define FF_DEVICE_TAG_DEF   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
#define FF_DEVICE_ID_DEF    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0

#define SIMULATION_ACTIVE   1
#define SIMULATION_DEACTIVE 0

typedef struct IPC_FFDeviceParam_t
{
    u8              dev_tag[FF_DEVICE_TAG_LEN];
    u8              dev_id[FF_DEVICE_ID_LEN];
    u8              dev_addr;
    u16 CheckWord;
} IPC_FFDeviceParams_t;

extern IPC_FFDeviceParams_t* GetDeviceVar(void);
//extern IPC_FFDeviceParams_t* SetDeviceVar(const IPC_FFDeviceParams_t* p);

extern ErrorCode_t  IPC_WriteDeviceAddress(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs);
extern ErrorCode_t  IPC_WriteDeviceTag(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs);
extern ErrorCode_t  IPC_WriteDeviceID(IPC_Variable_IDs_t VarID, IPC_WritePtrs_t const *pIPC_WritePtrs, IPC_ReadPtrs_t const *pIPC_ReadPtrs);
extern ErrorCode_t  IPC_ReadAppFwVer(IPC_Variable_IDs_t VarID, IPC_ReadPtrs_t const *pIPC_ReadPtrs);
extern ErrorCode_t  IPC_ReadLuiSimulation(IPC_Variable_IDs_t VarID, IPC_ReadPtrs_t const *pIPC_ReadPtrs);

typedef struct FFData_t
{
    bool_t SimulationActive; //!< Indicator of whether UI password is enabled
    u16 CheckWord;
} FFData_t;

//Application get/set interface
SAFEGET(ff_GetLocalFFData, FFData_t);
SAFESET(ff_RepairSetFFData, FFData_t);

#endif

