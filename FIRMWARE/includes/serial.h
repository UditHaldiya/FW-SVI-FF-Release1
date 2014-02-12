/*
Copyright 2006 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file serial.h
    \brief Header for UART device driver for HART communications

    CPU: Philips LPC21x4 (ARM)

    OWNER: Ernie Price

    $Archive: /MNCB/Dev/FIRMWARE/includes/serial.h $
    $Date: 6/14/07 11:23a $
    $Revision: 11 $
    $Author: Ernieprice $

    \ingroup HARTDLL
*/
#ifndef SERIAL_H_
#define SERIAL_H_

/* some flags for communication errors, in u8 Hart_flags */
#define PARITY_ERR      0xC0U            /* parity error   */
#define FRAMING         0x90U            /* framing error  */
#define OVERRUN         0xA0U            /* overrun error  */
#define CHECKSUM        0x88U            /* checksum error */
#define RX_BUFOVL       0x82U            /* rx buffer overflow */

/* HART-specific */
#define PREAMBLE            0xFF


extern  void            serial_SendFrame(u8 uartNo, u8_least len, const u8 *bufptr);

extern  void            serial_InitSerial(void);
extern  void            serial_ReceiveContinue(u8_least hart_channel);
extern  u8 *serial_GetHartGapTimerPtr(u8_least hart_channel);

//required interfaces
/** \brief A mandatory initializer
*/
extern void uart_setup(void);

/** \brief  Handle a serial interrupt from the UART (HART port)
*/
extern MN_IRQ void serial_int(void);
#define Hart_ISR serial_int

/** \brief  Handle a receive character from the UART - error bits, if any,
        are handled in rcv_exception before we get here.
    \param[in] c - the char to be processed
*/
extern void rcv_char(u8 c, u8 uartNo, u8_least hart_channel);

/** \brief  Return the state of the carrier detect line
    \return  bool_t  - true if carrier is present
*/
extern bool_t serial_CarrierDetect(u8_least hart_channel);

extern void rcv_exception(u8 hart_err, u8_least hart_channel);

extern void serial_ExpireGapTimer(u8_least hart_channel);
extern void serial_RearmGapTimer(u8_least hart_channel);
extern void serial_RecieveChar(u8 c, u8 uartNo, u8_least hart_channel);

#endif // SERIAL_H_

/* This line marks the end of the source */
