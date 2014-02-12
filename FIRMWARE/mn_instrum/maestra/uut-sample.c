/**
    \brief A sample unit under test.

    Demonstrates reference implementation of maestra instrumentation
    (maestra_instrum.{c,h}

    We compile it with defining INSTRUM_HEADER="maestra_instrum.h"
    in the command line; this will bring in the instrumentation.

    There can of course be other implementations of instrumentation;
    to bring it in, define INSTRUM_HEADER in the command line
    appropriately.

   Version control:
    $Date: 9/29/09 11:53p $
	$Change: 239 $
	$Id: //depot/maestra/uut-sample.c#4 $
*/
/* Copyright (C) 2007 MacroExpressions.
   Permission is granted to use and modify this file without limitations
*/

#include "project.h" //a common header

int foo(int x)
{
    int sum = 0;
    for(int i=0, j=i+1; ISTRUE(i<x); i++) { (void)j;
        if(i==17) break;
        if(i==0) continue;
        if(i%2 == 0)
            sum += i;

        else
            sum -=i; 
    }
    return sum;
}

double bar(int x)
{
    double ret;
    switch(x) {
      default:
        return 3.29;
      case (5): case 9: //case 9 is not instrumented!
      case (8): //instrumentable case cannot be on the same line
        return 3.28;
      case (90):
        ret = 8.2007 + baz(); //need a stub
        break;
      case (30):
        ret = 0.;
    }
    return ret;
}

void w(void)
{
    do {
        int i=2;
        while(i--) printf("Hey, i=%d\n", i);
        do {
            printf("Ah, i=%d\n", i);
        } while(++i<3);
    } while(0);
}

//illustration of logical expressions instrumentation
#include <iso646.h>
int andor(int x[4])
{
    if((x[0]) and (x[1]) and ( (x[2]) or (x[3])))
    {
        return 100;
    }
    return 200;
}
