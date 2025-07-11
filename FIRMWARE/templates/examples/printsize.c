/**
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
    
    \file foo.c
    \brief A brief description of the content

    CPU: Any (with standard C compiler)

    OWNER: Ark
    \n RCS: $Id$ 
    $DateTime: $ $Change: $ $Author: $
*/
/* This is optional and is pruned at sole descretion of the maintainer
   HISTORY:
   WHAT: a sample source file
   WHY: file template demo
   WHO: AK 06-AUG-2004
*/
#include <stdio.h>
#include "printsize.h"


/** \brief A short description.

  Detailed description goes here and
  it can be as long as you see fit.
  Document what the function does, not
  how it does it.
  \param[in] s pointer to const string
  \param[in] x size
  \return guess what void type returns?
*/
void foo(const char *s, size_t x)
{
    printf("sizeof(%s) = %u (%04xu hex)\n", s, x, x);
}



/** \brief A demo main ()
*/
int main()
{
    mystruct_t x;

    PRINTSIZE(mystruct_t);
    PRINTSIZE(x);
    PRINTSIZE(x.y);
}

/* This line marks the end of the source */
