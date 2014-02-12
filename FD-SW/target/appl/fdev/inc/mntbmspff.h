#ifndef MNTBMSPFF_H_
#define MNTBMSPFF_H_

#include <ffbl_res.h>
#include <fbap.h>
#include <ptb.h>

#include "mnhart2ff.h"

//---------------------------------------------------------------------------------------------
// Readback select enums
// This is switched by the readback_select parameter
//    0: Actual Position Value
//    1: Working Position Value
#define   READBACK_SELECT_ACTUALPOS             (0)
#define   READBACK_SELECT_WORKINGPOS            (1)

// For conversion of D (bool) to FLOAT and back
#define ZERO_PERCENT_FLOAT                        (0.0F)
#define ONE_HUNDRED_PERCENT_FLOAT                 (100.0F)
#define ZERO_PERCENT_INT                          (0)
#define ONE_HUNDRED_PERCENT_INT                   (100)


#define SETPOINT_DOBOOLFB_MIN                   (ZERO_PERCENT_FLOAT)
#define SETPOINT_DOBOOLFB_MAX                   (ONE_HUNDRED_PERCENT_FLOAT)
#define SETPOINT_DOBOOLFB_LO_AS_MIN             (1.0F)

//---------------------------------------------------------------------------------------------

extern void mnipc_SendSpAndMode(u8 *send_buff, const T_RESOURCE *p_resource_, const T_FBIF_BLOCK_INSTANCE *p_block_instance);
extern void mnipc_ReceiveSpAndMode(const u8 *receive_buff, T_RESOURCE *p_resource_, T_FBIF_PTB *p_PTB);
extern void mnipc_InitSpAndMode(T_FBIF_PTB *p_PTB);

extern bool_t Handle_MinMax(const FLOAT_S  *pfsCurrent, FLOAT *ppfMaxValue, FLOAT *ppfMinValue);


extern fferr_t ffmansp_WriteManualSetpoint(T_FBIF_BLOCK_INSTANCE *p_block_instance, T_FBIF_WRITE_DATA *p_write, void *from, void *to);
extern fferr_t ffmansp_WriteManualSetpointD(T_FBIF_BLOCK_INSTANCE *p_block_instance, T_FBIF_WRITE_DATA *p_write, void *from, void *to);
extern fferr_t ffmansp_WriteManualSetpointDINT(T_FBIF_BLOCK_INSTANCE *p_block_instance, T_FBIF_WRITE_DATA *p_write, void *from, void *to);

#endif //MNTBMSPFF_H_
