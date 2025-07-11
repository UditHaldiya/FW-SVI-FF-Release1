/**
    \brief A specific harness for unit-testing uut-sample.c

    This is an example of a specific test harness. It illutrates an arrangement
    of test cases into tests and tests into a test set.

   Version control:
    $Date: 9/29/09 11:53p $
	$Change: 233 $
	$Id: //depot/maestra/harness-sample.c#3 $
*/
/* Copyright (C) 2007 MacroExpressions.
   Permission is granted to use and modify this file without limitations
*/

//#define INSTRUM_HEADER "maestra_instrum.h"
#include <stdlib.h>
#include <stdio.h>

#include "maestra_api.h"
#include "maestra.h"
#include "mnwrap.h"
#include "bufferhandler.h"
#include "numutils.h"
#include "crc.h"
// ************* tests **********************
#define STUFF_MAX 6 //size of pre-stuffed content
u16 DATA_COUNT = 36; //128 //max number of points to collect
static dsample_t stuff[STUFF_MAX];

// ============= buffer init test cases =================
s16 n = 0;
void fill_func(diag_t data[2])
{
    data[0] = n;
    data[1] = n+1;
}
u32 num_samples = 0;

// test case harness
void inittest(void *arg)
{
    s16 y=2012;
    for(u8_least i=0; i<NELEM(stuff); i++)
    {
        stuff[i].data1 = y++;
        stuff[i].data2 = y++;
    }
    buffer_StartSampling(DIAGBUF_AUX, fill_func, DATA_COUNT, STUFF_MAX, stuff);
    const diag_t* buf = buffer_GetXDiagnosticBuffer(DIAGBUF_AUX);
    //s32 result = memcmp(stuff, buf, sizeof(stuff));
    maestra_boolcmp(0, memcmp(stuff, buf, sizeof(stuff)));
    const SamplerInfo_t *SamplerInfo = buffer_GetSamplerInfo(DIAGBUF_AUX);
    Struct_Test(SamplerInfo_t, SamplerInfo);

}
// ---- end of a test case ------------

// table of foo test cases
static const maestra_testcase_t inittestcase[] = {
    {
        .arg = NULL,
        .test_func = inittest,
        .repeat = 1,
    },
    MAESTRA_TESTCASE_END
};
// ============= end of init test cases =================

// ============= buffer fill test cases =================
// test case harness
void filltest(void *arg)
{
    const SamplerInfo_t *SamplerInfo = buffer_GetSamplerInfo(DIAGBUF_AUX);
    Struct_Test(SamplerInfo_t, SamplerInfo);
    maestra_harnessln("%s", "Add DATA_COUNT samples");
    for(u8_least i=0; i<DATA_COUNT; i++)
    {
        buffer_SampleData(DIAGBUF_AUX);
        n+=2;
        Struct_Test(SamplerInfo_t, SamplerInfo);
    }
    const diag_t* buf = buffer_GetXDiagnosticBuffer(DIAGBUF_AUX);
    maestra_harnessln("%s", "Check that the data prefix is intact");
    maestra_boolcmp(0, memcmp(stuff, buf, sizeof(stuff)));
    maestra_harnessln("%s", "Check that the data is correct");
    const dsample_t* data = (const dsample_t*)buf+STUFF_MAX;
    u32 i;
    for(i=0; i<DATA_COUNT; i++)
    {
        maestra_intcmp(2*i, data[i].data1, 0);
        maestra_intcmp(2*i+1, data[i].data2, 0);
    }
    num_samples+=i;
    Struct_Test(SamplerInfo_t, SamplerInfo);
}
// ---- end of a test case ------------

// table of fill test cases
static const maestra_testcase_t filltestcase[] = {
    {
        .arg = NULL,
        .test_func = filltest,
        .repeat = 1,
    },
    MAESTRA_TESTCASE_END
};
// ============= end of fill test cases =================

// ============= buffer prune test cases =================
// test case harness
void prunetest(void *arg)
{
    const u16 *batches = arg;
    const SamplerInfo_t *SamplerInfo = buffer_GetSamplerInfo(DIAGBUF_AUX);
    maestra_harnessln("Add %u samples", batches[maestra_count]);
    u8_least i;
    for(i=0; i<batches[maestra_count]; i++)
    {
        buffer_SampleData(DIAGBUF_AUX);
        n+=2;
    }
    num_samples+=i;
    /* How many times did the pruning occured?
    */
    u32 num_processed_samples = 1 + SamplerInfo->prune_scale * (SamplerInfo->num_points-1) + (SamplerInfo->prune_scale - SamplerInfo->skip);
    maestra_boolcmp(true, num_samples==num_processed_samples);

    buffer_StopSampling(DIAGBUF_AUX);

    const diag_t* buf = buffer_GetXDiagnosticBuffer(DIAGBUF_AUX);
    maestra_harnessln("%s", "Check that the data prefix is intact");
    maestra_boolcmp(0, memcmp(stuff, buf, sizeof(stuff)));

    maestra_harnessln("Pruning scale=%d (effective %d prunings)", SamplerInfo->prune_scale, intlog(SamplerInfo->prune_scale-1));





    maestra_harnessln("%s", "Check that the data is correct");
    const dsample_t* data = (const dsample_t*)buf+STUFF_MAX;


    for(u32 i=0; i<SamplerInfo->num_points; i++)
    {
        if(2*i*SamplerInfo->prune_scale != data[i].data1 || 2*i*SamplerInfo->prune_scale+1 != data[i].data2)
        { //just to save on report size
            maestra_intcmp(2*i*SamplerInfo->prune_scale, data[i].data1, 0);
            maestra_intcmp(2*i*SamplerInfo->prune_scale+1, data[i].data2, 0);
        }
    }

}

void prunetest1(void *arg)
{
    num_samples = 0;
    n = 0;
    buffer_StartSampling(DIAGBUF_AUX, fill_func, DATA_COUNT, STUFF_MAX, stuff);
    prunetest(arg);
}
// ---- end of a test case ------------

u16 batches[] = {100, 200, 300, 150, 250, 350};
u16 b[] = {100};
// table of fill test cases
static const maestra_testcase_t prunetestcase[] = {
    {
        .arg = b,
        .test_func = prunetest,
        .repeat = 1,
    },
    {
        .arg = batches,
        .test_func = prunetest1,
        .repeat = NELEM(batches),
    },
    MAESTRA_TESTCASE_END
};
// ============= end of fill test cases =================

#if 0
// ============= bar test cases =================
// ---- data for repeats of a foo test case -----
static struct bararg_t {
    int in;
    double out;
} bararg[] = {
    {0, 3.29},
    {10, 3.2900001},
    {5, 3.28},
    {8, 3.28},
    {9, 3.28},
    {90, 8.2007},
    {30, 0.},
};
// ---- end data for repeats of a foo test case -----

// test case harness
void bartest(void *arg)
{
    struct bararg_t *iarg = arg;
    double result = bar(iarg[maestra_count].in);
    printf(MAESTRA_HARNESS_PREFIX "Testing bar(%d)" MAESTRA_HARNESS_SUFFIX "\n",
           iarg[maestra_count].in);
    maestra_floatcmp(iarg[maestra_count].out, result, 0.5);
}
// ---- end of a test case ------------

// table of bar test cases
static const maestra_testcase_t bartestcase[] = {
    {
        .arg = bararg,
        .test_func = bartest,
        .repeat = NELEM(bararg),
    },
    MAESTRA_TESTCASE_END
};
// ============= end of bar test cases =================

// ============= w test cases =================
// ---- no data for repeats of a foo test case -----

// test case harness
void wtest(void *arg)
{
    (void)arg; //unused
    w(); //just watch what happens
}
// ---- end of a test case ------------

// table of w test cases
static const maestra_testcase_t wtestcase[] = {
    {
        .test_func = wtest,
        .repeat = 1,
    },
    MAESTRA_TESTCASE_END
};
// ============= end of w test cases =================

// ============= andor test cases =================
// test case harness
#include <iso646.h>
void andortest(void *arg)
{
    (void)arg; //unused
    int x[4], y, result;
    y = rand();
    for(int i=0; i<4; i++) {
        x[i] = y & (1<<i);
    }
    if((x[0]) and (x[1]) and ( (x[2]) or (x[3])))
    {
        result = 100;
    }
    else {
        result = 200;
    }
    y = andor(x); //just watch what happens
    maestra_intcmp(y, result, 1);
}
// ---- end of a test case ------------

// table of andor test cases
static const maestra_testcase_t andortestcase[] = {
    {
        .test_func = andortest,
        .repeat = 16,
    },
    MAESTRA_TESTCASE_END
};
// ============= end of andor test cases =================
#endif

static const maestra_test_t buftest[] = {
    {
        .description = "Initialize a buffer",
        .accept_a = "Ensure that prefix data are filled correctly",
        .maestra_testcase = inittestcase,
    },
    {
        .description = "Fill the buffer with max number of points",
        .accept_a = "Ensure that no pruning occurred and the buffer contains all data",
        .maestra_testcase = filltestcase,
    },
    {
        .description = "Fill the buffer with more points",
        .accept_a = "Ensure that pruning occurred correctly",
        .maestra_testcase = prunetestcase,
    },
#if 0
    {
        .description = "Just a demo of `switch\' and `case\' instrumentation",
        .accept_a = "Check results against pre-computed values",
        .maestra_testcase = bartestcase,
    },
    {
        .description = "Just a demo of `while\' instrumentation",
        .accept_m = "Inspect the trace",
        .maestra_testcase = wtestcase,
    },
    {
        .description = "Just a demo of instrumentation for RMC/DC coverage",
        .accept_m = "MArvel at trace",
        .maestra_testcase = andortestcase,
    },
#endif
    MAESTRA_TEST_END
};
// ************* end of tests **********************

//The test set with pre-defined name
const maestra_testset_t maestra_testset =
{
    .description = "Linear buffer sampling and pruning tests",
    .maestra_test = buftest,
};

