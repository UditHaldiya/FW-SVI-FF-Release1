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

#define INSTRUM_HEADER "maestra_instrum.h"
#include "maestra.h"

/* Optional. Use if you don't want define/declare exposed versions 
of ststic-cum-global variables. You will lose an opportunity to
detect initialized statics in block scope. OTOH, you'll enjoy the
correct working of scope name resolution; that is to say, you will
not be caught if you use it. You decide whether less labor is worth
the "features"
*/
#undef static 
#include "uut-sample.c" //The unit under test included in harness - method 2

#include "maestra_uninstrum.h"

#include <stdlib.h>
#include "project.h"
#include "maestra_api.h"

// ************* tests **********************
// ============= foo test cases =================
// ---- data for repeats of a foo test case -----
static int fooarg[] = {
    0, -3, 2, 3
};
// ---- end data for repeats of a foo test case -----

// test case harness
void footest(void *arg)
{
    int *iarg = arg;
    int result = foo(iarg[maestra_count]);
    printf(MAESTRA_HARNESS_PREFIX "Computed %d" MAESTRA_HARNESS_SUFFIX "\n",
           result);
}
// ---- end of a test case ------------

// table of foo test cases
static const maestra_testcase_t footestcase[] = {
    {
        .arg = fooarg,
        .test_func = footest,
        .repeat = NELEM(fooarg),
    },
    MAESTRA_TESTCASE_END
};
// ============= end of foo test cases =================

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

static const maestra_test_t demotest[] = {
    {
        .description = "Just a demo of `for\' and `if\' instrumentation",
        .accept_m = "Inspect the trace",
        .maestra_testcase = footestcase,
    },
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
    MAESTRA_TEST_END
};
// ************* end of tests **********************

//The test set with pre-defined name
const maestra_testset_t maestra_testset = {
    .description = "maestra demo",
    .maestra_test = demotest,
};

