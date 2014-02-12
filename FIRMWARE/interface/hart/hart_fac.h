/*
Copyright 2004-2007 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file hart_fac.h
    \brief Internal interfaces of the HART Functions Module related to factory support

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/hart/hart_fac.h $
    $Date: 11/15/11 3:36p $
    $Revision: 1 $
    $Author: Arkkhasin $

    \ingroup HARTapp
*/
/* $History: hart_fac.h $
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 11/15/11   Time: 3:36p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/interface/hart
 * Preliminary check-in for
 * TFS:8051 new tempcomp
 * 
 * *****************  Version 5  *****************
 * User: Arkkhasin    Date: 3/30/11    Time: 1:39p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Reverted to old API for old fixed-length commands
 * 
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 5/18/09    Time: 3:30p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Lint-inspired tempcomp layout CONST_ASSERTs
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 3/29/09    Time: 4:12p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * A few items moved from configure.h to pressures.h and errlimits.h
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 10/01/08   Time: 5:51p
 * Created in $/MNCB/Dev/FIRMWARE/interface/hart
 * Project-specific factory operations extracted into separate source
 * files.
*/
#ifndef HART_FAC_H_
#define HART_FAC_H_

//Temperature compensation read/write support (fashioned after tempcomp.h)

//lint -esym(526, bogus16) Size calc only - no instantiation
extern TemperatureCalibrationRow_t bogus16; //! for size checks only
//typedef TemperatureCalibrationRow_t TemperatureCalibrationRow16_t;
//lint -esym(526, bogus32) Size calc only - no instantiation
extern TemperatureCalibrationRow32_t bogus32; //! for size checks only
typedef struct TCRow32_t
{
    u8 ChannelShift[1][sizeof(H_UCHAR)]; //!< for the 2**-shift scaler
    u8 TCOffsetSpan32[NELEM(bogus32.TemperatureCalData)][sizeof(TCOffsetSpan32_t)]; //!< offset/span in HART buffer format
    u8 endmark; //required marker
} TCRow32_t; //! Generic layout of 32-bit tempcomp data in HART command layout format
typedef struct TCRow16_t
{
    u8 ChannelShift[1][sizeof(H_UCHAR)]; //!< for the 2**-shift scaler
    u8 TCOffsetSpan16[NELEM(bogus16.TemperatureCalData)][sizeof(TCOffsetSpan16_t)]; //!< offset/span in HART buffer format
    u8 endmark; //required marker
} TCRow16_t; //! Generic layout of 16-bit tempcomp data in HART command layout format

/** Generic template macro for tempcomp write implementations
*/
#define WRITE_TEMPCOMP_TEMPLATE(name, prefix, sizeId) \
s8_least CAT(hartcmd_, name)(const u8 *src, u8 *dst) \
{ \
    UNUSED_OK(dst); \
    const CAT2(TCRow, sizeId, _t) *s = (const void *)src; \
    CONST_ASSERT(NELEM(CAT(bogus, sizeId).TemperatureCalData) == NELEM(s->CAT(TCOffsetSpan, sizeId))); /*check number of elements*/ \
    CONST_ASSERT(sizeof(CAT(bogus, sizeId).TemperatureCalData) == sizeof(s->CAT(TCOffsetSpan, sizeId))); /*check size of elements*/ \
    CONST_ASSERT(MN_OFFSETOF(CAT2(Req_, name, _t), ChannelShift) == MN_OFFSETOF(CAT2(TCRow, sizeId, _t), ChannelShift)); /*check number of elements*/ \
    CONST_ASSERT(MN_OFFSETOF(CAT2(Req_, name, _t), CAT(TCOffsetSpan, sizeId)) == MN_OFFSETOF(CAT2(TCRow, sizeId, _t), CAT(TCOffsetSpan, sizeId))); /*check number of elements*/ \
    CONST_ASSERT(MN_OFFSETOF(CAT2(Req_, name, _t), endmark) == MN_OFFSETOF(CAT2(TCRow, sizeId, _t), endmark)); /*check number of elements*/ \
    return CAT(SetTempComp, sizeId)(s, CAT(prefix, _SetTempCorrRow)); \
}
/** Generic template macro for tempcomp read implementations
*/
#define READ_TEMPCOMP_TEMPLATE(name, prefix, sizeId, TCtype) \
s8_least CAT(hartcmd_, name)(const u8 *src, u8 *dst) \
{ \
    UNUSED_OK(src); \
    TCtype trow; \
    CAT2(TCRow, sizeId, _t) *s = (void *)dst; \
    CONST_ASSERT(NELEM(CAT(bogus, sizeId).TemperatureCalData) == NELEM(s->CAT(TCOffsetSpan, sizeId))); /*check number of elements*/ \
    CONST_ASSERT(sizeof(CAT(bogus, sizeId).TemperatureCalData) == sizeof(s->CAT(TCOffsetSpan, sizeId))); /*check size of elements*/ \
    CONST_ASSERT(MN_OFFSETOF(CAT2(Rsp_, name, _t), ChannelShift) == MN_OFFSETOF(CAT2(TCRow, sizeId, _t), ChannelShift)); /*check number of elements*/ \
    CONST_ASSERT(MN_OFFSETOF(CAT2(Rsp_, name, _t), CAT(TCOffsetSpan, sizeId)) == MN_OFFSETOF(CAT2(TCRow, sizeId, _t), CAT(TCOffsetSpan, sizeId))); /*check number of elements*/ \
    CONST_ASSERT(MN_OFFSETOF(CAT2(Rsp_, name, _t), endmark) == MN_OFFSETOF(CAT2(TCRow, sizeId, _t), endmark)); /*check number of elements*/ \
    if(CAT(prefix, _GetTempCorrRow)(&trow) == NULL) \
    { \
        return TRANSMITTER_SPECIFIC_COMMAND_ERROR; \
    } \
    return CAT(GetTempComp, sizeId)(&trow, s); \
}

//Generic tempcomp Read/write macros
#define WRITE_TEMPCOMP_16(name, prefix) WRITE_TEMPCOMP_TEMPLATE(name, prefix, 16)
#define WRITE_TEMPCOMP_32(name, prefix) WRITE_TEMPCOMP_TEMPLATE(name, prefix, 32)
#define READ_TEMPCOMP_16(name, prefix) READ_TEMPCOMP_TEMPLATE(name, prefix, 16, TemperatureCalibrationRow_t)
#define READ_TEMPCOMP_32(name, prefix) READ_TEMPCOMP_TEMPLATE(name, prefix, 32, TemperatureCalibrationRow32_t)

//I/O buffers parsers
extern s8_least GetTempComp16(const TemperatureCalibrationRow_t *trow, TCRow16_t *d);
extern s8_least SetTempComp16(const TCRow16_t *s, ErrorCode_t (*setf)(const TemperatureCalibrationRow_t *arg));
extern s8_least GetTempComp32(const TemperatureCalibrationRow32_t *trow, TCRow32_t *d);
extern s8_least SetTempComp32(const TCRow32_t *s, ErrorCode_t (*setf)(const TemperatureCalibrationRow32_t *arg));

#endif //HART_FAC_H_
/* This line marks the end of the source */
