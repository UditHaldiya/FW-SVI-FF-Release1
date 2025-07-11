/**
Copyright 2004-2005 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file spi.h
    \brief header BIOS routines for support of the SPI common chip specific code.

    CPU: Any (with standard C compiler)

    OWNER: AP
    \n RCS: $Id$
    Created: 9/27/04 Author:  EP
*/

/*
    $History: spi.h $
 *
 * *****************  Version 2  *****************
 * User: Justin Shriver Date: 12/15/11   Time: 4:32p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_LCX
 * -- AP -- ADded SPI channels.
 *
 * *****************  Version 1  *****************
 * User: Justin Shriver Date: 11/29/11   Time: 3:56p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_LCX
 * BIOS modularization
 *

*/

#ifndef SPI_H_
#define SPI_H_

extern  void    MN_API_FUNC(spi_Initialize)(void);

extern  void    MN_API_FUNC(spi_StartRead_Ch1)(void);

extern  u32     MN_API_FUNC(spi_Read8bit_Ch0)(void);
extern  u32     MN_API_FUNC(spi_Read8bit_Ch1)(void);
extern  u32     MN_API_FUNC(spi_CompleteRead8bit_Ch1)(void);

extern  u32     MN_API_FUNC(spi_Read16bit_Ch0)(void);
extern  u32     MN_API_FUNC(spi_Read16bit_Ch1)(void);
extern  u32     MN_API_FUNC(spi_CompleteRead16bit_Ch1)(void);

extern  void    MN_API_FUNC(spi_Write8bit_Ch0)(u8 Data);
extern  void    MN_API_FUNC(spi_Write8bit_Ch1)(u8 Data);

extern  void    MN_API_FUNC(spi_Write16bit_Ch0)(u16 Data);
extern  void    MN_API_FUNC(spi_Write16bit_Ch1)(u16 Data);

extern  bool_t  MN_API_FUNC(spi_GetSPIStatus_Ch0)(void);
extern  bool_t  MN_API_FUNC(spi_GetSPIStatus_Ch1)(void);

extern  void    MN_API_FUNC(spi_Sio1Select)(void);
extern  void    MN_API_FUNC(spi_Sio1Deselect)(void);
extern  void    MN_API_FUNC(spi_Sio0Select)(void);
extern  void    MN_API_FUNC(spi_Sio0Deselect)(void);

#endif //_SPI_H

/* End of the source */
