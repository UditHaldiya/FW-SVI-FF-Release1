/*
Copyright 2010 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file cryptutils.h
    \brief Interfaces of lightweight cryptographic primitives and related stuff

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes/cryptutils.h $
    $Date: 8/09/10 12:18p $
    $Revision: 2 $
    $Author: Arkkhasin $

    \ingroup numutils
*/
/* $History: cryptutils.h $
 * 
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 8/09/10    Time: 12:18p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * Made compatible with a PC build
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 7/18/10    Time: 1:40p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/includes
 * Lightweight crypto stuff to support a device activation scheme
*/
#ifndef CRYPTUTILS_H_
#define CRYPTUTILS_H_

MN_DECLARE_API_FUNC(util64_Add_mod, util64_Mult_mod, util64_Exp_mod, mnhash)
MN_DECLARE_API_FUNC(util64_VerifySignature)
MN_DECLARE_API_DATA(Ntable)
extern u64 util64_Add_mod(u64 a, u64 b, u64 modulus);
extern u64 util64_Mult_mod(u64 a, u64 b, u64 modulus, void (*waitf)(void));
extern u64 util64_Exp_mod(u64 base, u64 e, u64 modulus, void (*waitf)(void) );
extern u32 mnhash( const void *key, size_t len, u32 seed );
extern const u64 Ntable[16];
extern bool_t util64_VerifySignature(const u64 key, const u64 signature, u32 e, void (*waitf)(void));

#endif //CRYPTUTILS_H_
/* This line marks the end of the source */
