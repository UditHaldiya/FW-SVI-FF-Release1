/*****************************************************************************
*                                                                            *
*                     SOFTING Industrial Automation GmbH                     *
*                          Richard-Reitzner-Allee 6                          *
*                                D-85540 Haar                                *
*                        Phone: ++49-89-4 56 56-0                            *
*                          Fax: ++49-89-4 56 56-3 99                         *
*                                                                            *
*                            SOFTING CONFIDENTIAL                            *
*                                                                            *
*                     Copyright (C) SOFTING IA GmbH 2012                     *
*                             All Rights Reserved                            *
*                                                                            *
* NOTICE: All information contained herein is, and remains the property of   *
*   SOFTING Industrial Automation GmbH and its suppliers, if any. The intel- *
*   lectual and technical concepts contained herein are proprietary to       *
*   SOFTING Industrial Automation GmbH and its suppliers and may be covered  *
*   by German and Foreign Patents, patents in process, and are protected by  *
*   trade secret or copyright law. Dissemination of this information or      *
*   reproduction of this material is strictly forbidden unless prior         *
*   written permission is obtained from SOFTING Industrial Automation GmbH.  *
******************************************************************************
******************************************************************************
*                                                                            *
* PROJECT_NAME             Softing FF/PA FD 2.42                             *
*                                                                            *
* VERSION                  FF - 2.42                                         *
*                          PA - 2.42 (beta)                                  *
*                                                                            *
* DATE                     16. April 2012                                    *
*                                                                            *
*****************************************************************************/

/*
** libgcc support for software floating point.
** Copyright (C) 1991 by Pipeline Associates, Inc.  All rights reserved.
** Permission is granted to do *anything* you want with this file,
** commercial or otherwise, provided this message remains intact.  So there!
** I would appreciate receiving any updates/patches/changes that anyone
** makes, and am willing to be the repository for said changes (am I
** making a big mistake?).

Warning! Only single-precision is actually implemented.  This file
won't really be much use until double-precision is supported.

However, once that is done, this file might eventually become a
replacement for libgcc1.c.  It might also make possible
cross-compilation for an IEEE target machine from a non-IEEE
host such as a VAX.

If you'd like to work on completing this, please talk to rms@gnu.ai.mit.edu.

--> Double precision floating support added by James Carlson on 20 April 1998.

**
** Pat Wood
** Pipeline Associates, Inc.
** pipeline!phw@motown.com or
** sun!pipeline!phw or
** uunet!motown!pipeline!phw
**
** 05/01/91 -- V1.0 -- first release to gcc mailing lists
** 05/04/91 -- V1.1 -- added float and double prototypes and return values
**                  -- fixed problems with adding and subtracting zero
**                  -- fixed rounding in truncdfsf2
**                  -- fixed SWAP define and tested on 386
*/

/*
** The following are routines that replace the libgcc soft floating point
** routines that are called automatically when -msoft-float is selected.
** The support single and double precision IEEE format, with provisions
** for byte-swapped machines (tested on 386).  Some of the double-precision
** routines work at full precision, but most of the hard ones simply punt
** and call the single precision routines, producing a loss of accuracy.
** long long support is not assumed or included.
** Overall accuracy is close to IEEE (actually 68882) for single-precision
** arithmetic.  I think there may still be a 1 in 1000 chance of a bit
** being rounded the wrong way during a multiply.  I'm not fussy enough to
** bother with it, but if anyone is, knock yourself out.
**
** Efficiency has only been addressed where it was obvious that something
** would make a big difference.  Anyone who wants to do this right for
** best speed should go in and rewrite in assembler.
**
** I have tested this only on a 68030 workstation and 386/ix integrated
** in with -msoft-float.
*/

#include <stdlib.h>
#include "osif_dbl.h"

/* the following deal with IEEE single-precision numbers */
#define EXCESS      126
#define SIGNBIT     0x80000000UL
#define HIDDEN      (1UL << 23)
#define SIGN(fp)    ((fp) & SIGNBIT)
#define EXP(fp)     (((fp) >> 23) & 0xFF)
#define MANT(fp)    (((fp) & 0x7FFFFF) | HIDDEN)
#define PACK(s,e,m) ((s) | ((e) << 23) | ((m) & 0x7FFFFF))

/* the following deal with IEEE double-precision numbers */
#define EXCESSD     1022
#define HIDDEND     (1UL << 20)
#define EXPD(fp)    ((((fp).upper) >> 20) & 0x7FF)
#define SIGND(fp)   (((fp).upper) & SIGNBIT)
#define MANTD(fp)   ((((((fp).upper) & 0xFFFFF) | HIDDEND) << 10) | ((fp).lower >> 22))
#define MANTD_LL(val,fp) do { (val).upper = MANTD(fp); (val).lower = (fp).lower << 10; } while(0)
#define PACKD_LL(fp,s,e,m) do { (fp).upper = (s) | ((e)<<20) | (((m).upper >> 10) & 0xFFFFF); \
                                (fp).lower = ((m).upper << 22) | ((m).lower >> 10); } while(0)

#define NEG_LL(val) do { (val).lower = ~(val).lower; (val).upper = ~(val).upper; \
                         if (!++(val).lower) ++(val).upper; } while(0)

#define SHR_LL(val,exp) do { if ((exp)<32) { (val).lower >>= (exp); \
                                             (val).lower |= (val).upper<<(32-(exp)); \
                                             (val).upper >>= (exp); } \
                             else { (val).lower = (unsigned long)(val).upper >> abs(exp-32); (val).upper >>= 31; } \
                        } while (0)

#define SHL_LL(val,exp) do { if ((exp)<32) { (val).upper <<= (exp); \
                                             (val).upper |= (val).lower>>(32-(exp)); \
                                             (val).lower <<= (exp); } \
                             else { (val).upper = (val).lower << abs(exp-32); (val).lower = 0; } \
                        } while (0)

#define ADD_LL(val1,val2) do { (val1).lower += (val2).lower; (val1).upper += (val2).upper; \
                            if ((val1).lower<(val2).lower) ++(val1).upper; \
                        } while (0)

union float_long
  {
    float f;
    long l;
  };

/* negate a double */
void osif_negate_double (const FB_DOUBLE *a1, FB_DOUBLE *res)
{
  FB_DOUBLE dbl1;

  dbl1 = *a1;

  if (!dbl1.upper && !dbl1.lower)
    {
      *res = dbl1;
      return;
    }

  dbl1.upper ^= SIGNBIT;
  *res = dbl1;
  return;
}

/* convert float to double */
void osif_float_to_double (float a1, FB_DOUBLE *res)
{
  long exp;
  union float_long fl1;
  FB_DOUBLE dbl;

  fl1.f = a1;

  if (!fl1.l)
    {
      dbl.upper = dbl.lower = 0;
      *res = dbl;
      return;
    }

  dbl.upper = SIGN (fl1.l);
  exp = EXP (fl1.l) - EXCESS + EXCESSD;
  dbl.upper |= exp << 20;
  dbl.upper |= (MANT (fl1.l) & ~HIDDEN) >> 3;
  dbl.lower  = MANT (fl1.l) << 29;

  *res = dbl;
  return;
}

/* convert double to float */
float osif_double_to_float (const FB_DOUBLE *a1)
{
  register long exp;
  register long mant;
  union float_long fl;
  FB_DOUBLE dbl;

  dbl = *a1;

  if (!dbl.upper && !dbl.lower)
    return (float)(0);

  exp = EXPD (dbl) - EXCESSD + EXCESS;

  /* shift double mantissa 6 bits so we can round */
  mant = MANTD (dbl) >> 6;

  /* now round and shift down */
  mant += 1;
  mant >>= 1;

  /* did the round overflow? */
  if (mant & 0xFF000000)
    {
      mant >>= 1;
      exp++;
    }

  /* pack up and go home */
  fl.l = PACK (SIGND (dbl), exp, mant);
  return (fl.f);
}

/* add two doubles */
void osif_add_double (const FB_DOUBLE *a1, const FB_DOUBLE *a2, FB_DOUBLE *res)
{
  FB_DOUBLE mant1, mant2;
  FB_DOUBLE fl1, fl2;
  register long exp1, exp2;
  register long sign = 0;

  fl1 = *a1;
  fl2 = *a2;

  /* check for zero args */
  if (!fl2.upper && !fl2.lower)
    goto test_done;
  if (!fl1.upper && !fl2.lower) {
    fl1 = fl2;
    goto test_done;
  }

  exp1 = EXPD(fl1);
  exp2 = EXPD(fl2);

  if (exp1 > exp2 + 54)
    goto test_done;
  if (exp2 > exp1 + 54) {
    fl1 = fl2;
    goto test_done;
  }

  /* do everything in excess precision so's we can round later */
  MANTD_LL(mant1,fl1);
  MANTD_LL(mant2,fl2);

  if (SIGND(fl1))
    NEG_LL(mant1);
  if (SIGND(fl2))
    NEG_LL(mant2);

  if (exp1 > exp2) {
    SHR_LL(mant1,1);
    SHR_LL(mant2,1+(exp1 - exp2));
  } else if (exp1 < exp2) {
    SHR_LL(mant1,1+(exp2 - exp1));
    SHR_LL(mant2,1);
    exp1 = exp2;
  } else {
    SHR_LL(mant1,1);
    SHR_LL(mant2,1);
  }

  ADD_LL(mant1,mant2);

  if (mant1.upper < 0) {
    NEG_LL(mant1);
    sign = SIGNBIT;
  } else if (!mant1.upper && !mant1.lower) {
    fl1.upper = fl1.lower = 0;
    goto test_done;
  }

  /* normalize up */
  while (!(mant1.upper & 0xE0000000)) {
    SHL_LL(mant1,1);
    exp1--;
  }

  /* normalize down? */
  if (mant1.upper & 0xC0000000) {
    SHR_LL(mant1,1);
    exp1++;
  }

  /* round to even */
  mant2.lower = (mant1.lower & (1<<9)) ? (1<<8) : ((1<<8)-1);
  mant2.upper = 0;
  ADD_LL(mant1,mant2);

  /* normalize down? */
  if (mant1.upper & 0xC0000000) {
    SHR_LL(mant1,1);
    exp1++;
  }

  SHL_LL(mant1,1);

  /* pack up and go home */
  PACKD_LL(fl1,sign,exp1,mant1);

test_done:
  *res = fl1;
  return;
}

/* subtract two doubles */
void osif_sub_double (const FB_DOUBLE *a1, const FB_DOUBLE *a2, FB_DOUBLE *res)
{
  FB_DOUBLE fl1, fl2;

  fl1 = *a1;
  fl2 = *a2;

  osif_negate_double (&fl2, &fl2);
  osif_add_double (&fl1, &fl2, res);
  return;
}
