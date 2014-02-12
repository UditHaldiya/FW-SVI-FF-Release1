#include "mnwrap.h"
#include "fpconvert.h"
#include "param.h"
#include "interface.h"
#include "customunits.h"
#include "pressures.h"
#include "timedef.h"
#include "signaldef.h"
#include "tempr.h"
#include "errlimits.h"
#include "fptest.h"

#define NAMENUM(a) a, #a

#define MIN_TIME_TO_ACTION_FP           (1.0)
#define MAX_TIME_TO_ACTION_FP           (600.0)

const utable_t utab[] =
{
    {
        NAMENUM(UNITSID_SIGNAL_ENTRY),
        STD_FROM_MA(-0.1),
        STD_FROM_MA(23.0),
    },
    {
        NAMENUM(UNITSID_SIGNAL),
        STD_FROM_MA(-0.1),
        STD_FROM_MA(23.0),
    },
    {
        NAMENUM(UNITSID_PERCENT),
        MIN_ALLOWED_POSITION,
        MAX_ALLOWED_POSITION,
    },
    {
        NAMENUM(UNITSID_TEMPR),
        STD_FROM_DEGREES_C(-55.0),
        STD_FROM_DEGREES_C(125.0),
    },
	//For MNCB
    {
        NAMENUM(UNITSID_MNTIME),
        MNTIME_TO_CTLCYCLES(POSTIME1_LOW),
        MNTIME_TO_CTLCYCLES(POSTIME1_HIGH),
    },
    {
        NAMENUM(UNITSID_PRESSURE),
        STD_FROM_PSI(PRESSURE_MIN_PSI),
        STD_FROM_PSI(PRESSURE_MAX_PSI),
    },
    {
        NAMENUM(UNITSID_PRESSURE_ENTRY),
        STD_FROM_PSI(PRESSURE_MIN_PSI),
        STD_FROM_PSI(PRESSURE_MAX_PSI),
    },
    {
        NAMENUM(UNITSID_POSITION),
        MIN_ALLOWED_POSITION,
        MAX_ALLOWED_POSITION,
    },
#if 0
    {
        NAMENUM(UNITSID_SHEDTIME),
        TICKS_FROM_SHEDTIME_SEC(0.1),
        0xffffff, //we promise only 24 bits TICKS_FROM_SHEDTIME_SEC(2.0E6),
    },
#endif
	//termination
    {
        NAMENUM(UNITSID_NONE),
        1,
        0,
    }
};
