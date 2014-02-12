/**
    \brief maestra common harness driver

   Version control:
    $Date: 5/04/10 4:20p $
	$Change: 203 $
	$Id: //depot/maestra/maestra.c#2 $
*/
/* Copyright (C) 2007 MacroExpressions.
   Permission is granted to freely use and modify this file provided this
   comment block is retained verbatim.
*/

#include <stdlib.h>
#include <stddef.h>
#include "maestra_api.h"

//see descriptions in maestra_api.h
maestra_result_t maestra_result_set;
maestra_result_t maestra_result_test;
maestra_result_t maestra_result_case;
maestra_result_t maestra_result;

int maestra_testnum; //! index into maestra_test array
int maestra_testcasenum; //! index into the current test case array
int maestra_count; //! number of test case executions so far

int main(void)
{
    const maestra_test_t *p;
    const maestra_testcase_t *q;

    maestra_result_set = pass;

    //1. print test set description
    maestra_print_description(maestra_testset.description);

    //2. do initialization
    if(maestra_testset.init_func!=NULL) {
        maestra_testset.init_func();
    }
    maestra_testnum = 0;

    //3. Execute the set
    for(p=maestra_testset.maestra_test; p != NULL && (q = p->maestra_testcase) != NULL; p++, maestra_testnum++) {
        //3.1. Initialize the current test
        maestra_result_test = pass;
        maestra_print_testhead(maestra_testnum, p);
        if(p->init_func != NULL) {
            p->init_func();
        }

        //3.2. Execute all test cases
        for(maestra_testcasenum = 0; q->test_func != NULL; q++, maestra_testcasenum++) {
            //execute the test case
            maestra_navig((q+1)->test_func == NULL); //pass the flag of last testcase
            maestra_result_case = pass;
            for(maestra_count=0; maestra_count<q->repeat; maestra_count++) {
                if(q->repeat>1) { //otherwise no point in navigating repeats
                    maestra_casenavig(maestra_count+1==q->repeat);
                }
                q->test_func(q->arg);
            }
            maestra_print_result(maestra_result_case, "test case");
            //propagate the worst result from test case to test
            if(maestra_result_test < maestra_result_case) {
                maestra_result_test = maestra_result_case;
            }
        }
        maestra_print_result(maestra_result_test, "test");

        //propagate the worst result from test to test set
        if(maestra_result_set < maestra_result_test) {
            maestra_result_set = maestra_result_test;
        }

        //cleanup
        if(p->cleanup_func != NULL) {
            p->cleanup_func();
        }
    }

    maestra_print_result(maestra_result_set, "test set");
    exit( maestra_result_set == fail ); //return non-zero iff failed
}


