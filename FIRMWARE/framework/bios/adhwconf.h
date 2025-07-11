/*
Copyright 2011 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file adhwconf.h
    \brief Private interface to A/D hardware configuration

    CPU: Any

    OWNER: AP

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/framework/bios/adhwconf.h $
    $Date: 12/15/11 4:44p $
    $Revision: 3 $
    $Author: Justin Shriver $

    \ingroup A_to_D
*/
/* $History: adhwconf.h $
 *
 * *****************  Version 3  *****************
 * User: Justin Shriver Date: 12/15/11   Time: 4:44p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/framework/bios
 * -- AP -- Corrected GPIO use.
 *
 * *****************  Version 2  *****************
 * User: Justin Shriver Date: 11/29/11   Time: 7:00p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/framework/bios
 * -- AP -- TFS 7889 - moved the .C file, added project specific function
 * proto's.
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 11/20/11   Time: 10:53p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/framework/bios
 * TFS:8255 - interface to project HW configuration
*/
#ifndef ADHWCONF_H_
#define ADHWCONF_H_

typedef struct ADsamples_t
{
    u32     count, factor; //!< Averging coefficients (Ernie's original way)
} ADsamples_t;

#define SCALE_SHFT  14u
#define FACTOR      (1u << SCALE_SHFT)

typedef struct adhwconf_t
{
    const ADsamples_t *sampleTbl; //!< table of number of samples per channel
    const u32 *MuxPwrTable; //!< multiplexor and power configuration table
    //replaced with literal u32 PwrDownMask;
    //replaced with literal u32 PwrBitsMask;
} adhwconf_t;

extern const adhwconf_t adhwconftab;

extern  void    ad_EndSequence(void);
extern  void    ad_PrepareChannelPower(u8 channel, u8 lookaheadhannel);
extern  void    ad_SelectExtAD(void);
extern  void    ad_DeSelectExtAD(void);
extern  bool_t  ad_IsChannelPowered(u8 channel);

extern  void    ad_MultiplexDelay(void (*useful_func)(void));

extern  void    ad_SamplePWMPins(void);
extern  bool_t  ad_NoPWMTransition(void);

#endif //ADHWCONF_H_
/* This line marks the end of the source */
