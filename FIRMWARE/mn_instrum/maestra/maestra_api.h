/**
    \brief maestra API for writing stubs and specific harness

NOTE: This is the first (and rather sloppy) organization: some things
are applicable to internal maestra workings and should not be exposed
to a stub/harness programmer. The problem is, for some items I am not
sure whether they should or should not be exposed, so for now almost
everything is thrown in one bin.

   Version control:
    $Date: 5/04/10 4:20p $
	$Change: 203 $
	$Id: //depot/maestra/maestra_api.h#2 $
*/
/* Copyright (C) 2007 MacroExpressions.
   Permission is granted to freely use and modify this file provided this
   comment block is retained verbatim.
*/

#ifndef MAESTRA_API_H_
#define MAESTRA_API_H_

#ifdef __cplusplus
extern "C" {
#endif

//Just a handy macro for the number of elements in an array
#ifndef maestra_NELEM
#define maestra_NELEM(array) (sizeof(array)/sizeof((array)[0]))
#endif

/** The type of test execution result */
typedef enum maestra_result_t {
    pass,
    accept,
    fail
} maestra_result_t;

extern maestra_result_t maestra_result;   //! result of the last evaluation
extern maestra_result_t maestra_result_set; //! (running) result of the whole test
extern maestra_result_t maestra_result_test; //! (running) result of the current test
extern maestra_result_t maestra_result_case; //! result of the last test case

extern int maestra_testnum; //! index into maestra_test array
extern int maestra_testcasenum; //! index into the current test case array
extern int maestra_count; //! number of test case executions so far

/** The type of test case definition */
typedef struct maestra_testcase_t {
    void *arg;
    void (*test_func)(void *arg);
    int repeat; //!< number of reperitions of the case
} maestra_testcase_t;

#define MAESTRA_TESTCASE_END {.test_func = NULL,} //! terminates a test case array

/** The type of test as a collection of test cases */
typedef struct maestra_test_t {
    const char *description; //!< what the test is about
    const char *accept_a;    //!< automatic acceptance criteria stated
    const char *accept_m;    //!< manual acceptance criteria stated
    const maestra_testcase_t *maestra_testcase; //!< an array of test cases
    void (*init_func)(void); //!< Test initialization function (optional)
    void (*cleanup_func)(void); //!< Test cleanup function (optional)
} maestra_test_t;
#define MAESTRA_TEST_END {.maestra_testcase = NULL,} //! terminates a test array

/** The type of test set as a collection of tests */
typedef struct maestra_testset_t {
    const char *description;  //!< Test set description
    void (*init_func)(void); //!< Test set initialization function
    const maestra_test_t *maestra_test;  //!< A pointer to a NULL-terminated array of tests
} maestra_testset_t; //! test set type, with a single instance in the program

//Print stuff
extern void maestra_print_description(const char *description);
extern void maestra_print_testhead(int testnum, const maestra_test_t *test);
extern void maestra_print_result(maestra_result_t result, const char *scope);

extern void maestra_printstr(const char *s);


//Data checking to use in harness or stubs
extern void maestra_intcmp(long long a, long long b, int tolerance);
extern void maestra_floatcmp(long double a, long double b, double tolerance);
#define maestra_boolcmp(exp1, exp2) maestra_boolcmp__(#exp1, #exp2, (exp1), (exp2))
extern void maestra_boolcmp__(const char *sa, const char *sb, long long a, long long b);

/** Print formatting to ensure the source of output is conspicuous */
/* I am not sure yet whether these should be string literals or constant
   strings somewhere. The implementation and the example may show
   this ambiguity.
*/
#ifdef HTML_OUT
#   define MAESTRA_SUPERSCRIPT(x) "<SUP>" #x "</SUP>"
#   define MAESTRA_HORIZONTAL_BAR "<HR color=\"#0033ff\">\n"
#   define MAESTRA_BOLD "<B>"
#   define MAESTRA_ENDBOLD "</B>"
#   define MAESTRA_CODE "<CODE><PRE>"
#   define MAESTRA_ENDCODE "</PRE></CODE>"
//Use a deprecated tag for simplicity
#   define MAESTRA_LITERAL "<XMP>"
#   define MAESTRA_ENDLITERAL "</XMP>"
#   define MAESTRA_FONT_START(a) "<FONT " a ">"
#   define MAESTRA_FONT_END "</FONT>"
#   define MAESTRA_HARD_EOL "<BR>\n"
#   define MAESTRA_PARA_START "<P>"
#   define MAESTRA_PARA_END "</P>\n"

#define MAESTRA_INSTRUM_PREFIX "<FONT style=\"BACKGROUND-COLOR: #ccffff\" face=\"Courier\"><B>I:</B>\n"
#define MAESTRA_INSTRUM_SUFFIX "</FONT>\n"
#define MAESTRA_HARNESS_PREFIX "<FONT face=\"Verdana\" size=\"-1\"><B>H:</B>\n"
#define MAESTRA_HARNESS_SUFFIX "</FONT>\n"
#define MAESTRA_STUB_PREFIX "<FONT style=\"BACKGROUND-COLOR: #ffffcc\" face=\"Courier\"><B>S:</B>\n"
#define MAESTRA_STUB_SUFFIX "</FONT>\n"

#   define MAESTRA_INIT_FORMAT \
        "<HTML><HEAD><TITLE>%s</TITLE>" \
        "<META http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\">" \
        "</HEAD><BODY>" \
        "<H1 align=\"center\"><A name=\"TOP\"></A>Unit Test Set Report</H1>" /*page title holder*/
#   define MAESTRA_RESULT_FAILED "<FONT style=\"BACKGROUND-COLOR: #dc0000\" face=\"Verdana,Arial,Helvetica\" color=\"#ffff00\"><B>&nbsp;FAILED&nbsp;</B></FONT>\n" //
#   define MAESTRA_RESULT_ACCEPTED "<FONT style=\"BACKGROUND-COLOR: yellow\" face=\"Verdana,Arial,Helvetica\" color=\"#000099\"><B>&nbsp;ACCEPTED&nbsp;</B></FONT>\n"
#   define MAESTRA_RESULT_PASSED "<FONT style=\"BACKGROUND-COLOR: #00ff00\" face=\"Verdana,Arial,Helvetica\" color=\"#000099\"><B>&nbsp;PASSED&nbsp;</B></FONT>\n"
#   define MAESTRA_FINAL_RESULT_FORMAT "<P><FONT face=\"Verdana,Arial,Helvetica\"><B>%s</B></FONT>%s</P>\n"
#   define MAESTRA_PAGE_END "</BODY></HTML>"
#   define MAESTRA_TESTMAST_FORMAT_COMMON "<H3 ALIGN=\"CENTER\">%s</H3><P>%s</P>"
#   define MAESTRA_TEST_RESULT_FORMAT "<P><FONT FACE=\"Verdana,Arial,Helvetica\">%s overall: </FONT>%s</P>\n"
extern void maestra_navig(int last_flag);
extern void maestra_casenavig(int last_flag);

#else
    //Plain text
#define MAESTRA_INSTRUM_PREFIX "I: "
#define MAESTRA_INSTRUM_SUFFIX ""
#define MAESTRA_HARNESS_PREFIX "H: "
#define MAESTRA_HARNESS_SUFFIX ""
#define MAESTRA_STUB_PREFIX "S: "
#define MAESTRA_STUB_SUFFIX ""

#   define MAESTRA_SUPERSCRIPT(x) "**" #x
#   define MAESTRA_HORIZONTAL_BAR "\n---------------------------------------------------------------\n"
#   define MAESTRA_BOLD "***"
#   define MAESTRA_ENDBOLD "***"
#   define MAESTRA_CODE "\n"
#   define MAESTRA_ENDCODE "\n"
#   define MAESTRA_LITERAL "\n   "
#   define MAESTRA_ENDLITERAL "\n"
#   define MAESTRA_FONT_START(a) ""
#   define MAESTRA_FONT_END ""
#   define MAESTRA_HARD_EOL "\n"
#   define MAESTRA_PARA_START "\n\n"
#   define MAESTRA_PARA_END "\n"
#   define MAESTRA_INIT_FORMAT "" /*page title holder: no title */
#   define MAESTRA_RESULT_FAILED " FAILED" //
#   define MAESTRA_RESULT_ACCEPTED " ACCEPTED"
#   define MAESTRA_RESULT_PASSED " PASSED"
#   define MAESTRA_FINAL_RESULT_FORMAT "%d %s%s"
#   define MAESTRA_PAGE_END ""
#   define MAESTRA_TESTMAST_FORMAT_COMMON "\n%s:\n%s"
#   define MAESTRA_TEST_RESULT_FORMAT "%s overall: %s\n"

#   define maestra_navig(lf) /*nothing*/
#   define maestra_casenavig(lf) /*nothing*/
#endif //HTML_OUT

//Output
#ifndef maestra_printf
#   define maestra_printf printf
#endif

#ifndef __cplusplus //C++ doesn't have variadic macros yet
//Handy output macros for harness/stubs/instrumentation
#define maestra_stubf(format, ...) maestra_printf(MAESTRA_STUB_PREFIX format MAESTRA_STUB_SUFFIX, __VA_ARGS__)
#define maestra_harnessf(format, ...) maestra_printf(MAESTRA_HARNESS_PREFIX format MAESTRA_HARNESS_SUFFIX, __VA_ARGS__)
#define maestra_instrumf(format, ...) maestra_printf(MAESTRA_INSTRUM_PREFIX format MAESTRA_INSTRUM_SUFFIX, __VA_ARGS__)
#define maestra_stubln(format, ...) maestra_stubf(format MAESTRA_HARD_EOL, __VA_ARGS__)
#define maestra_harnessln(format, ...) maestra_harnessf(format MAESTRA_HARD_EOL, __VA_ARGS__)
#define maestra_instrumln(format, ...) maestra_instrumf(format MAESTRA_HARD_EOL, __VA_ARGS__)
#endif

/** The instance of the test set, must be provided by specific harness */
extern const maestra_testset_t maestra_testset;

#ifdef __cplusplus
}
#endif

#endif //MAESTRA_API_H_
