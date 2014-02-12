#ifndef XD_ERRORS_H_
#define XD_ERRORS_H_

#include "ptb.h"
#include "mnhart2ff.h"

/* XD_ERROR Enumeration */
#define XD_ERROR_NO_ERROR                       0
#define XD_ERROR_GENERAL_ERROR                  17
#define XD_ERROR_CALIBRATION_ERROR              18
//#define XD_ERROR_CONFIGURATION_ERROR            19
#define XD_ERROR_ELECTRONICS_FAILURE            20
#define XD_ERROR_MECHANICAL_FAILURE             21
#define XD_ERROR_I_O_FAILURE                    22
#define XD_ERROR_DATA_INTEGRITY_ERROR           23
#define XD_ERROR_SOFTWARE_ERROR                 24
#define XD_ERROR_ALGORITHM_ERROR                25

extern fferr_t UpdateXDErrorPos(const T_FBIF_BLOCK_INSTANCE* p_block_instance);
extern fferr_t UpdateXDErrorPressure(const T_FBIF_BLOCK_INSTANCE* p_block_instance);
extern fferr_t UpdateXDErrorTemperature(const T_FBIF_BLOCK_INSTANCE* p_block_instance);
extern fferr_t Update_FIND_STOPS_Error_bit(const T_FBIF_BLOCK_INSTANCE* p_block_instance);

#endif // XD_ERRORS_H_

/* This line marks the end of the source */
