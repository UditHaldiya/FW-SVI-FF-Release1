/**
    \brief A set of maestra routines for harness output

Harness output includes output of comparisons of expected vs. actually
computed values.
In that area,
 - a simplest implementation of integer and floating point comparisons is
   provided
 - conspicously absent are implementations of string and raw memory buffers
   comparisons. They are equally straightforward to implement and may be added
   in the near future.

   Version control:
    $Date: 5/04/10 4:20p $
	$Change: 203 $
	$Id: //depot/maestra/maestra_print.c#2 $
*/
/* Copyright (C) 2007 MacroExpressions.
   Permission is granted to freely use and modify this file provided this
   comment block is retained verbatim.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "maestra_api.h"

#define OUT_STYLE_INFO \
        MAESTRA_PARA_START\
        "Report output styles:" MAESTRA_HARD_EOL\
        MAESTRA_HARNESS_PREFIX "Harness output" MAESTRA_HARNESS_SUFFIX MAESTRA_HARD_EOL\
        MAESTRA_STUB_PREFIX "Stub output" MAESTRA_STUB_SUFFIX MAESTRA_HARD_EOL\
        MAESTRA_INSTRUM_PREFIX "Instrumentation output" MAESTRA_INSTRUM_SUFFIX\
        MAESTRA_PARA_END

#define TESTSET_DESCRIPTION_FORMAT \
         MAESTRA_PARA_START\
         MAESTRA_FONT_START("size=\"+1\"") MAESTRA_BOLD "Description:"\
         MAESTRA_ENDBOLD MAESTRA_FONT_END MAESTRA_HARD_EOL "%s"\
         MAESTRA_PARA_END


/** \brief A routine to print a test set header, with completeness checks
\param maestra_testnum - the test's consecutive number
\param description - a pointer to the test set description
*/
void maestra_print_description(const char *description)
{
    maestra_printf(OUT_STYLE_INFO);
    if(description == NULL) {
        maestra_printf("Missing test set description %s" MAESTRA_HARD_EOL MAESTRA_HARD_EOL, MAESTRA_RESULT_FAILED);
        maestra_result_set = fail;
    }
    else {
        maestra_printf(TESTSET_DESCRIPTION_FORMAT , description);
    }
}

/** Test result conversion-to-string table */
static const char * const tresult[] = {
    MAESTRA_RESULT_PASSED,
    MAESTRA_RESULT_ACCEPTED,
    MAESTRA_RESULT_FAILED,
};

/** \brief A routine to print a test header, with completeness checks
\param maestra_testnum - the test's consecutive number
\param test - a pointer to the test definition
*/
void maestra_print_testhead(int maestra_testnum, const maestra_test_t *test)
{
    if(test == NULL) {
        //No more tests; wrap up instead
        maestra_printf(MAESTRA_PARA_START);
        maestra_print_result(maestra_result_set, "Test set");
        maestra_printf(MAESTRA_PARA_END MAESTRA_HORIZONTAL_BAR);
        return;
    }

#ifdef HTML_OUT
    //For HTML provide basic navigation
    printf("<H2 align=\"center\"><A name=\"TEST_%u\"></A>Test %u: &lt;<A href=\"#TOP\">Top</A>&gt;", maestra_testnum+1, maestra_testnum+1);
    if(test->maestra_testcase!=NULL) //signature that it exists
    {
        printf("&lt;<A HREF=\"#TEST_%u\">Next test</A>&gt;", maestra_testnum+2);
    }
    if(maestra_testnum != 0)
    {
        printf("&lt;<A HREF=\"#TEST_%u\">Prev.test</A>&gt;", maestra_testnum);
    }
    printf("</H2>\n");
#else //text
    printf("\n**** Test %u: ****\n", maestra_testnum+1);
#endif
    if(test->description == NULL) {
        maestra_printf("Test %d: Missing test description %s" MAESTRA_HARD_EOL, maestra_testnum, MAESTRA_RESULT_FAILED);
        maestra_result_test = fail;  //force 'em to have a test description
    }
    else {
        maestra_printf(MAESTRA_TESTMAST_FORMAT_COMMON, "Description", test->description);
    }

    if(test->accept_a == NULL && test->accept_m == NULL) {
        //no acceptance criteria
        maestra_harnessln("! Missing accptance criteria %s", MAESTRA_RESULT_FAILED);
        maestra_result_test = fail;  //force 'em to have it
    }
    else {
        maestra_printf(MAESTRA_BOLD MAESTRA_TESTMAST_FORMAT_COMMON MAESTRA_ENDBOLD, "Test acceptance criteria:", "");
        if(test->accept_a != NULL) {
            maestra_printf(MAESTRA_TESTMAST_FORMAT_COMMON, "Automatic: " MAESTRA_HARD_EOL, test->accept_a);
        }
        if(test->accept_m != NULL) {
            maestra_printf(MAESTRA_TESTMAST_FORMAT_COMMON, "Manual: " MAESTRA_HARD_EOL, test->accept_m);
        }
    }
    maestra_printf(MAESTRA_TESTMAST_FORMAT_COMMON, "Execution Trace", "");
}

/** \brief A simple wrapper to print the test result
\param result - a result to print in text form
\param scope - the scope's name - test case, test or test set
*/
void maestra_print_result(maestra_result_t result, const char *scope)
{
    maestra_printf(MAESTRA_TEST_RESULT_FORMAT, scope, tresult[result]);
}

#ifdef HTML_OUT
void maestra_navig(int last_flag)
{
    maestra_printf("<H4><A NAME=\"CASE_%u_%d\"></A>Case %d: &lt;<A HREF=\"#TOP\">Top</A>&gt; \n", maestra_testnum+1, maestra_testcasenum+1, maestra_testcasenum+1);
    maestra_printf("&lt;<A HREF=\"#TEST_%u\">Test description</A>&gt; \n", maestra_testnum+1);
    if(!last_flag)
    {
        maestra_printf("&lt;<A HREF=\"#CASE_%u_%d\">Next case</A>&gt; ", maestra_testnum+1, maestra_testcasenum+2);
    }
    if(maestra_testcasenum > 0)
    {
        maestra_printf("&lt;<A HREF=\"#CASE_%u_%d\">Prev. case</A>&gt;", maestra_testnum+1, maestra_testcasenum);
    }
    maestra_printf("</H4>\n");
}

void maestra_casenavig(int last_flag)
{
    maestra_printf("<H5><A NAME=\"CASE_%u_%d_%d\"></A>Case %d Repeat %d: &lt;<A HREF=\"#TOP\">Top</A>&gt; \n", maestra_testnum+1, maestra_testcasenum+1, maestra_count+1,
                   maestra_testcasenum+1, maestra_count+1);
    maestra_printf("&lt;<A HREF=\"#CASE_%u_%d\">Case top</A>&gt; \n", maestra_testnum+1, maestra_testcasenum+1);
    if(!last_flag)
    {
        maestra_printf("&lt;<A HREF=\"#CASE_%u_%d_%d\">Next Repeat</A>&gt; ", maestra_testnum+1, maestra_testcasenum+1, maestra_count+2);
    }
    if(maestra_count > 0)
    {
        maestra_printf("&lt;<A HREF=\"#CASE_%u_%d_%d\">Prev. Repeat</A>&gt;", maestra_testnum+1, maestra_testcasenum+1, maestra_count);
    }
    maestra_printf("</H5>\n");
}

#endif //HTML_OUT

// *****************************************************************
/* Simple reference implementations of comparisons within test cases
long long and long double types seem mighty enough to hold smaller
promoted values.
If efficiency becomes an issue or there are types like unsigned long
long in the code, more specialized comparison functions should be
written.
*/

/** \brief An integer-types test for use in specific harness.

The function prints the result of comparison and whether it is considered
passed, accepted, or failed. It also propagates the error to the test case
level.
It is a lazy implementation with no care for efficiency: All integral-type
numbers will be promoted to long long.
\param a - value expected by the harness
\param b - value actually computed by the UUT
\param tolerance - a max error for the acceptance.
*/
void maestra_intcmp(long long a, long long b, int tolerance)
{
    maestra_result_t r;
    long long diff = llabs(a-b);
    if(diff == 0) {
        r = pass;
    }
    else if(diff < tolerance) {
        r = accept;
    }
    else {
        r = fail;
    }
    maestra_harnessln("Expected %lld vs. Actual %lld: %s", a, b, tresult[r]);

    //Propagate the worst result
    if(maestra_result_case < r) {
        maestra_result_case = r;
    }
}

void maestra_printstr(const char *s)
{
    maestra_printf("\"");
    while(*s!=0)
    {
        if(*s<32 || *s>125)
        {
            maestra_printf("\\x%02X", *s);
        }
        else
        {
            maestra_printf("%c", *s);
        }
        s++;
    }
    maestra_printf("\" ");
}


/** \brief A helper for maestra_boolcmp macro.

The function prints expressions (expected and actual), symbolic and numeric.
It also propagates the error to the test case
level.
It is a lazy implementation with no care for efficiency: All integral-type
numbers will be promoted to long long.
\param a - value expected by the harness
\param b - value actually computed by the UUT
\param tolerance - a max error for the acceptance.
*/
void maestra_boolcmp__(const char *sa, const char *sb, long long a, long long b)
{
    maestra_result_t r;
    if(!!a == !!b) {
        r = pass;
    }
    else {
        r = fail;
    }
    maestra_printf(MAESTRA_HARNESS_PREFIX "Expected {");
    maestra_printstr(sa);
    maestra_printf(" (=%lld)} == {", a);
    maestra_printstr(sb);
    maestra_printf(" (=%lld)} : %s" MAESTRA_HARNESS_SUFFIX MAESTRA_HARD_EOL, b, tresult[r]);

    //maestra_harnessln("Expected {%s (=%lld)} == {%s (==%lld)} : %s", sa, a, sb, b, tresult[r]);

    //Propagate the worst result
    if(maestra_result_case < r) {
        maestra_result_case = r;
    }
}

/** \brief A floting-point test for use in specific harness.

The function prints the result of comparison and whether it is considered
passed, accepted, or failed. It also propagates the error to the test case
level.
The implementation is relative to the magnitude of the
things we are comparing. A tolerance = 0.5 is reasonably generous.
It is also a lazy implementation with no care for efficiency: All floating
point numbers will be promoted to long double.
\param a - value expected by the harness
\param b - value actually computed by the UUT
\param tolerance - an error scaler for the acceptance.
*/
void maestra_floatcmp(long double a, long double b, double tolerance)
{
    maestra_result_t r;
    long double diff = fabs(a-b);
    if(diff == 0.) {
        //Yes, there are cases where the results must be bit-true!
        r = pass;
    }
    else if(diff < tolerance) {
        r = accept;
    }
    else {
        r = fail;
    }
    maestra_harnessln("Expected %Lf vs. Actual %Lf: %s", a, b, tresult[r]);

    //Propagate the worst result
    if(maestra_result_case < r) {
        maestra_result_case = r;
    }
}


