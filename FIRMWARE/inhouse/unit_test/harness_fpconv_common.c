#include <stdio.h>
#include <math.h>
#include <float.h>
#undef UTEST_INSTRUM_HEADER
#include "maestra_api.h"
#include "mnwrap.h"
#include "numutils.h"
#include "fpconvert.h"
#include "interface.h"
#include "bufutils.h"
#include "fptest.h"

//Replacements of static objects cum extern
UnitsConfig_t UnitsConfig;
#if NUM_UNITSIDS_CUSTOMCONV>0 //no custom units here
UnitsConv_t m_ComputedCustomUnits[NUM_UNITSIDS_CUSTOMCONV];
#endif


/* Basic test of units: a number in internal representation is
subjected to round-trip coversion to/from external (FP) representation
*/
#pragma optimize=none
#undef long
#undef double
static s32 fpconvert_RoundTrip(s32 val, UnitsId_t uid)
{
    u8 buf[sizeof(float32)], backbuf[sizeof(float32)];
    s32 uival;
    u8_least decpt = fpconvert_UIFormat(uid, val, &uival);
    fpconvert_IntToFloatBuffer(val, uid, buf);
    maestra_harnessln(" - [Info: reference value=%f UI=%de-%u]", util_GetFloat(buf), uival, decpt);

    cdouble_t fback = (uival/pow(10,decpt)); //the value from UI
    util_PutFloat(backbuf, (float32)fback);                  //the FP buffer from UI
    s32 back = fpconvert_FloatBufferToInt(backbuf, uid); //roundtrip from UI
    maestra_harnessln(" - [Info: from UI value: %f => %d]", fback, back);

    s32 roundtrip = fpconvert_FloatBufferToInt(buf, uid);

    maestra_printf("Compare float and UI-float: ");
    maestra_floatcmp((long double)(float32)fback, (long double)util_GetFloat(buf), (long double)0.5/pow(10, decpt));

    return roundtrip;
    //return back;
}

/* A test on val is
- passed if round-trip conversion result is equal to val
- accepted if the test on val didn't pass but the test on the conversion result (TBD)
*/

static void test_fpconvert_Init(void)
{
    fpconvert_InitRepair(NULL);
}

s32 max_error;
cdouble_t max_fperror; //floating point error allowance

static void test_fpconvert_RoundTripAllValues(UnitsId_t uid, s32 lo, s32 hi)
{
    s32 val;
    for(val = lo; val <= hi; val++)
    {
        maestra_printf(MAESTRA_HARD_EOL MAESTRA_HARD_EOL  "Test value %d" MAESTRA_HARD_EOL, val);
        s32 result1, result2=val;
        while((result1 = fpconvert_RoundTrip(result2, uid)) != result2)
        {
            maestra_harnessln("Walk step from %d to %d", result2, result1);
            result2 = result1;
        }
        maestra_intcmp(val, result1, max_error+1);
    }
}

#define NUMTESTPOINTS 24 //25
//static
void test_fpconvert_RoundTripUnits(UnitsId_t uid, s32 lo, s32 hi)
{
    maestra_harnessln("Testing around low point %d", lo);
    test_fpconvert_RoundTripAllValues(uid, lo-NUMTESTPOINTS, lo+NUMTESTPOINTS);
    maestra_harnessln("Testing around midpoint %d", (lo+hi)/2);
    test_fpconvert_RoundTripAllValues(uid, (lo+hi)/2-NUMTESTPOINTS, lo+(hi-lo)/2+NUMTESTPOINTS);
    maestra_harnessln("Testing around high point %d", hi);
    test_fpconvert_RoundTripAllValues(uid, hi-NUMTESTPOINTS, hi+NUMTESTPOINTS);
}

void test_setmaxerr(UnitsId_t id, s32 irange, float32 frange)
{
    s32 dummy;
    u8_least decpt = fpconvert_UIFormat(id, 0, &dummy);
    max_fperror = 0.5 / pow(10, decpt) + DBL_EPSILON;
    max_error = (s32)ceil(max_fperror * irange / frange);

    maestra_harnessln("Max acceptable internal error = %d counts", max_error);
    maestra_harnessln("Max acceptable external error = %f counts", max_fperror);
}
const UnitsConv_t *fpconvert_GetConvertParam(UnitsId_t id);
//static
void test_fpconvert_RoundTripAllUnits(const utable_t *tab)
{

    UnitsId_t uid = tab->uid;
    s32 lo_ = tab->lo;
    s32 hi_ = tab->hi;
    u8 unit = 0;
    do
    {
        maestra_harnessln(MAESTRA_PARA_START "Testing unit %d" MAESTRA_PARA_END, unit);
#ifdef UNIT_TEST_ENVIRONMENT
        const UnitsConv_t *conv = fpconvert_GetConvertParam(uid);
        test_setmaxerr(uid, conv->irange, conv->frange);
#else
        test_setmaxerr(uid, hi_-lo_, (float32)(hi_-lo_)); //dummy
#endif
        //maestra_harnessln("---Test with unit %d\n", unit);
        test_fpconvert_RoundTripUnits(uid, lo_, hi_);
        ++unit;

    } while(fpconvert_SetUnitsByIndex(uid, unit) == ERR_OK);
    maestra_harnessln(MAESTRA_PARA_START "Done Testing %d units for units id %d" MAESTRA_PARA_END, unit, uid);
}

// test 1 case harness
void fptestf(void *arg)
{
    const utable_t *tab = arg;
    s32 i=0;
    while(tab[i].uid != UNITSID_NONE)
    {
        maestra_harnessln(MAESTRA_PARA_START MAESTRA_FONT_START("Size=+1") "Testing unit id %d=%s" MAESTRA_FONT_END MAESTRA_PARA_END, tab[i].uid, tab[i].uid_name);
        test_fpconvert_RoundTripAllUnits(&tab[i]);
        maestra_printf(MAESTRA_HORIZONTAL_BAR);
        i++;
    }
}

// table of test cases
static const maestra_testcase_t fptestcase[] =
{
    {
        .arg = (void *)utab,
        .test_func = fptestf,
        .repeat = 1,
    },
    MAESTRA_TESTCASE_END
};

#if NUM_UNITSIDS_CUSTOMCONV>0 //no custom units here
typedef const struct custlims_t
{
    float32 lrv;
    float32 urv;
} custlims_t;

// test 2 case harness
void fpcustf(void *arg)
{
    u8 lbuf[4], ubuf[4];
    const struct custlims_t *tab = arg;
    maestra_harnessln( "Setting range [%f, %f]", tab[maestra_count].lrv, tab[maestra_count].urv);
    util_PutFloat(lbuf, tab[maestra_count].lrv);
    util_PutFloat(ubuf, tab[maestra_count].urv);
    ErrorCode_t err = fpconvert_SetCustomUnits(UNITSID_POSITION, H_PERCENT, ubuf, lbuf);
    maestra_intcmp(0, err, 0);
    if(err == ERR_OK)
    {
        test_setmaxerr(UNITSID_POSITION, STANDARD_RANGE, tab[maestra_count].urv - tab[maestra_count].lrv);
        test_fpconvert_RoundTripUnits(UNITSID_POSITION, MIN_ALLOWED_POSITION, MAX_ALLOWED_POSITION);
    }
}


static const maestra_testcase_t fpcustcase[] =
{
    {
        .arg = (void *)&(custlims_t){5000.,6500.},
        .test_func = fpcustf,
        .repeat = 1,
    },
    {
        .arg = (void *)&(custlims_t){0.,100.},
        .test_func = fpcustf,
        .repeat = 1,
    },
    {
        .arg = (void *)&(custlims_t){-30.2,40.3},
        .test_func = fpcustf,
        .repeat = 1,
    },
    MAESTRA_TESTCASE_END
};
#endif //no custom units here


static const maestra_test_t fptest[] =
{
#if NUM_UNITSIDS_CUSTOMCONV>0 //no custom units here
    {
        .description = "Testing a conversion for UNITSID_POSITION (custom)",
        .accept_a = "A test on val is" MAESTRA_HARD_EOL
            "- passed if round-trip conversion result is equal to val" MAESTRA_HARD_EOL
            "- accepted if the test on val didn't pass but the test on the conversion result passed",
        .maestra_testcase = fpcustcase,
    },
#endif
    {
        .description = "Testing a conversion for unit specified below",
        .accept_a = "A test on val is" MAESTRA_HARD_EOL
            "- passed if round-trip conversion result is equal to val" MAESTRA_HARD_EOL
            "- accepted if the test on val didn't pass but the test on the conversion result passed",
        .maestra_testcase = fptestcase,
    },
    MAESTRA_TEST_END
};


//The test set with pre-defined name
const maestra_testset_t maestra_testset = {
    .description =
        "Basic test of units: a number in internal representation is\n"
        "subjected to round-trip coversion to/from external (FP) representation",
    .maestra_test = fptest,
    .init_func = test_fpconvert_Init,
};


