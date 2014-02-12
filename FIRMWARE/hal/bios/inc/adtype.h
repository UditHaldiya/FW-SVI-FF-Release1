#ifndef ADTYPE_H_
#define ADTYPE_H_
#include "config.h"

/*      Pressure sensors

        Sensor  Funct   Single  Double
                        Acting  Acting
        IC310   Pilot   Pilot   Pilot
        IC702   Press1  Act     Act2
        IC703   Press2  Supply  Act1
        IC704   Press3  N/A     Supply
        IC705   Atmos   Atmos   Atmos

*/
/* List with valid A/D channels for initialisation of measure sequence */
/* #define list shall start with 0 and increments of 1. */
#define AD_NOT_INSTALLED (-1)
#define AD_AI_SP        (u8)0           /* Input current from 4..20ma input */
#define AD_AI_PV        (u8)1           /* Input voltage */
#define AD_TEMP_INT     (u8)2           /* Board temperature */
#define AD_PRESS_ATMOS  (u8)3           /* onboard pressure sensor. Atmosphere pressure */
#define AD_POS_REM      (u8)4           /* remote position */
#define AD_PRESS_1      (u8)5           /* pressure sensor 1 */
#define AD_POS_INT      (u8)6           /* Position sensor (Hall) */
#define AD_PRESS_3      (u8)7           /* pressure sensor 3 */
#define AD_PRESS_2      (u8)8           /* pressure sensor 2 */
#define AD_PRESS_PILOT  (u8)9           /* pressure sensor Pilot pressure */
#define AD_IP_CUR       (u8)10          /* output of D/A 2 or of the pressure controller */
#define AD_DIAG         (u8)11   	    /* analog reference voltage */
#define AD_TEMPR_INIT     12U           /* Board temperature - init only */

#define NUMBER_OF_AD_CHANNELS 13U  /* BIOS internal */

#define END_SEQ_TWO  0x20
#define END_SEQ_ONE  0x10


/* Type with all A/D input values. used for storage of raw data */
typedef struct
{
    u16 AdValue[NUMBER_OF_AD_CHANNELS];
} AdDataRaw_t;

typedef u8 measure_sequence_t;

#endif //ADTYPE_H_
