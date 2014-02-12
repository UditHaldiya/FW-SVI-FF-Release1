/*
Copyright 2010 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file cryptutils.c
    \brief lightweight cryptographic primitives and related stuff

    CPU: Any with 8-bit byte

    OWNER: AK
    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/utilities/cryptutils.c $
    $Date: 11/09/10 6:52p $
    $Revision: 3 $
    $Author: Arkkhasin $

    \ingroup numutils
*/
/* $History: cryptutils.c $
 * 
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 11/09/10   Time: 6:52p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/utilities
 * TFS:4505 (Lint - MISRA 20.2)
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 8/09/10    Time: 12:19p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/utilities
 * 1. Made compatible with a PC build
 * 2. Fixed modular multiplication
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 7/18/10    Time: 1:39p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/utilities
 * Lightweight crypto stuff to support a device activation scheme
*/
#include <limits.h>
#include "mnwrap.h"
#include "cryptutils.h"

//Pre-requisites
CONST_ASSERT(CHAR_BIT==8);
CONST_ASSERT(sizeof(u32)==4);
CONST_ASSERT(sizeof(u64)==8);

//For compatibility with MS VC++
#define ULLCAST(x) ((u64)(x))

/** \brief A primitive to add mod(modulus)
Requires that a <  and b < modulus
\param a - 1st addend < modulus
\param b - 2nd addend < modulus
\param modulus - a modulus > 0
\return a+b mod modulus (which is < modulus)
*/
u64 util64_Add_mod(u64 a, u64 b, u64 modulus)
{
    a += b;
    if(a < b)
    {
        /* unsigned overflow; the result >2**64>modulus but less than 2*modulus
        Subtracting the modulus (with 1**64 MSB implied) does the trick
        */
        a -= modulus; //reduction
    }
    else
    {
        //no overflow
        if(a >= modulus)
        {
            a -= modulus; //reduction
        }
    }
    return a;
}


/** \brief A primitive to multiply mod(modulus)
Requires that a <  and b < modulus
\param a - 1st multiplicand < modulus
\param b - 2nd multiplicand < modulus
\param modulus - a modulus > 0
\param waitf - a function to sprinkle "frequently enough" through long calculations
\return a*b mod modulus (which is < modulus)
*/
u64 util64_Mult_mod(u64 a, u64 b, u64 modulus, void (*waitf)(void))
{
    u64 product = ULLCAST(0);

    while (a > 0)
    {
        if ((a & ULLCAST(1)) != ULLCAST(0))
        {
            // add in this bit's contribution while using modulus to keep result small
            product = util64_Add_mod(product, b, modulus);
        }
        // move to the next bit of a, double (and mod) the multiplicand accordingly
        a >>= 1;
        b = util64_Add_mod(b, b, modulus);
    }
    if(waitf != NULL)
    {
        waitf();
    }

    return product;
}

/** \brief A primitive to exponentiate mod(modulus)
Requires that base < modulus

Implementation is based on popular binary exponentiation by Schneier (e.g.
http://en.wikipedia.org/wiki/Modular_exponentiation) with adaptation to MNCB
computing powers

\param base - base < modulus
\param e - exponent
\param modulus - a modulus > 0
\param waitf - a function to sprinkle "frequently enough" through long calculations
\return base**e mod modulus (which is < modulus)
*/
u64 util64_Exp_mod(u64 base, u64 e, u64 modulus, void (*waitf)(void) )
{
    u64 powr = ULLCAST(1);

    while (e > 0)
    {
        if ((e & ULLCAST(1)) != ULLCAST(0))
        {
            // multiply in this bit's contribution while using modulus to keep result small
            powr = util64_Mult_mod(powr, base, modulus, waitf);
        }
        // move to the next bit of e, square (and mod) the base accordingly
        e >>= 1;
        base = util64_Mult_mod(base, base, modulus, waitf);
    }

    return powr;
}


/**
\brief A Hash Function Adapted to MNCB environment

We use hash function for randomization of short input, so the choice is
based on avalanche quality (http://sites.google.com/site/murmurhash/avalanche)

Based on MurmurHashAligned2A, by Austin Appleby; http://sites.google.com/site/murmurhash
Adapted for use in MNCB according to MIT license.

-------------- Author's comment: ----------------------
// MurmurHash2A, by Austin Appleby

// This is a variant of MurmurHash2 modified to use the Merkle-Damgard
// construction. Bulk speed should be identical to Murmur2, small-key speed
// will be 10%-20% slower due to the added overhead at the end of the hash.

// This variant fixes a minor issue where null keys were more likely to
// collide with each other than expected, and also makes the algorithm
// more amenable to incremental implementations. All other caveats from
// MurmurHash2 still apply.
-------------- End Author's comment: ----------------------
*/

//lint -emacro(717, mmix)
#define mmix(h,k) do { k *= m; k ^= k >> r; k *= m; h *= m; h ^= k; } while(false)

u32 mnhash( const void *key, size_t len, u32 seed )
{
	const u32 m = 0x5bd1e995;
	const u8_least r = 24;
	u32 l = len;

	const u8 *data = (const u8 *)key;

	u32 h = seed;
	u32 t = 0;

	while(len >= 4)
	{
		u32 k;
        k = *data++;
        k = (k<<8) | *data++;
        k = (k<<8) | *data++;
        k = (k<<8) | *data++;

		mmix(h,k);

		len -= 4;
	}


	switch(len)
	{
	    case 3: t ^= data[2] << 16; /*lint -fallthrough*/
	    case 2: t ^= data[1] << 8; /*lint -fallthrough*/
	    case 1: t ^= data[0]; /*lint -fallthrough*/
        default://nothing
            ;
	}

	mmix(h,t);
	mmix(h,l);

	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
}



/** \brief 16 products of two "large" primes to choose from.

They are chosen "randomly" (not quite) so that the protuct is
large (0xFFCAnnnnnnnnnnnn) with room for some growth. Other than that,
there is no magic in their selection.
Source: http://www.bigprimes.net/archive/prime
Look for prime #279996801 etc. (larger) and #147506601 (smaller).
*/
const u64 Ntable[16] =
{
#if MN_NO_C99 == 0
    6010170281ULL*3066742429ULL,
    6010170289ULL*3066742417ULL,
    6010170349ULL*3066742403ULL,
    6010170367ULL*3066742391ULL,

    6010170371ULL*3066742379ULL,
    6010170391ULL*3066742351ULL,
    6010170493ULL*3066742321ULL,
    6010170499ULL*3066742309ULL,

    6010170527ULL*3066742307ULL,
    6010170553ULL*3066742277ULL,
    6010170563ULL*3066742267ULL,
    6010170577ULL*3066742229ULL,

    6010170581ULL*3066742207ULL,
    6010170619ULL*3066742169ULL,
    6010170679ULL*3066742043ULL,
    6010170697ULL*3066742037ULL,
#else
    ULLCAST(6010170281)*ULLCAST(3066742429),
    ULLCAST(6010170289)*ULLCAST(3066742417),
    ULLCAST(6010170349)*ULLCAST(3066742403),
    ULLCAST(6010170367)*ULLCAST(3066742391),

    ULLCAST(6010170371)*ULLCAST(3066742379),
    ULLCAST(6010170391)*ULLCAST(3066742351),
    ULLCAST(6010170493)*ULLCAST(3066742321),
    ULLCAST(6010170499)*ULLCAST(3066742309),

    ULLCAST(6010170527)*ULLCAST(3066742307),
    ULLCAST(6010170553)*ULLCAST(3066742277),
    ULLCAST(6010170563)*ULLCAST(3066742267),
    ULLCAST(6010170577)*ULLCAST(3066742229),

    ULLCAST(6010170581)*ULLCAST(3066742207),
    ULLCAST(6010170619)*ULLCAST(3066742169),
    ULLCAST(6010170679)*ULLCAST(3066742043),
    ULLCAST(6010170697)*ULLCAST(3066742037),
#endif
};

/** \brief Verifies a 64-bit RSA signature of a 64-bit key.

Algorithm: key ==? signature**e mod N
where N=from table with the randomized index hash(key)

\param key - a key whose signature to verify
\param signature - presumed signature of key
\param e - the exponent (cf. the algorithm)
\param waitf - a function to execute sporadically during lengthy operations
\return true iff the signature is valid
*/
bool_t util64_VerifySignature(const u64 key, const u64 signature, u32 e, void (*waitf)(void))
{
    bool_t ret;
    u32 hash = mnhash(&key, sizeof(u64), 0);
    u64 N = Ntable[hash & (NELEM(Ntable)-1)];
    if(waitf != NULL)
    {
        waitf(); //Not sure we need it here, but to be on a safe side
    }
    if((key >= N) || (signature >= N))
    {
        ret = false;
    }
    else
    {
        N = util64_Exp_mod(signature, (u64)e, N, waitf);
        ret = (N==key);
    }
    return ret;
}


/* This line marks the end of the source */
