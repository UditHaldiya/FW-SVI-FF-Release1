/**
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file printsize.h
    \brief A demo of a source file structure

    CPU: Any (with standard C compiler)

    OWNER: Ark
    \n RCS: $Id$ 
    $DateTime: $ $Change: $ $Author:  $
*/
                 

#ifndef FOO_H_
#define FOO_H_

/** This struct demonstrates nothing
*/
struct mystruct_t
{
    int x; ///< briefly, an int member
    double y; /**< In detail, a double member */
};

/** A brief description of the typedef to follow. */
typedef struct mystruct_t mystruct_t;

/** A sample macro */
#define STRINGIZE(x) #x
    /*!< Converts the arg into quoted string */

//see printsize.c for description
extern void foo(const char *s, size_t x);

#define PRINTSIZE(x) foo(STRINGIZE(x), sizeof(x))
    /*!< Prints nicely size of a type or an lvalue \a x. */

#endif //FOO_H_

/* This line marks the end of the source */
