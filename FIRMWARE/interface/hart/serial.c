/*
Copyright 2006 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/** \addtogroup HARTDLL HART data link layer implementation
\brief HART data link layer implementation
\htmlonly
<a href="../../../Doc/DesignDocs/Physical Design_HART_DataLinkLayer.doc"> Design document </a><br>
<a href="../../../Doc/TestDocs/utplan_BIOS.doc"> Unit Test Report </a><br>
\endhtmlonly
*/
/**
    \file serial.c
    \brief UART handler for HART communications

    CPU: Philips LPC21x4 (ARM)

    OWNER: Ernie Price

    $Archive: /MNCB/Dev/FIRMWARE/framework/bios/serial.c $
    $Date: 6/30/11 2:13p $
    $Revision: 53 $
    $Author: Arkkhasin $

    \ingroup HARTDLL
*/
#include "mnwrap.h"
#include "mnassert.h"     // temporarily not used
#include "faultpublic.h"
#include "hart.h"
#include "serial.h"

#define USE_EXPANSION_ADDR  0

/* states of the receive statemachine */
#define CHECK_FOR_PREAMBLE  0x01
#define CHECK_FOR_SECOND_PREAMBLE  0x0A //temporarily
#define CHECK_FOR_SOM       0x02
#define CAPTURE_ADDRESS     0x03
#if USE_EXPANSION_ADDR
 #define CAPTURE_EXPANSION   0x04
 /* Bits in  Delimiter */
 #define EXPANSION_SHIFT    (u8)5
 //#define ADDR_SIZE        0x05     /* Nbr of addressbytes in the longframe    */
#endif
#define EXPANSION_MASK     (u8)0x60

/** \brief receiver block self-timeout in character times;
can be large because used only for RAM error recovery.
*/
#define BLOCK_TM            15u


#define CAPTURE_CMD         0x05
#define CAPTURE_BYTECOUNT   0x06
#define CAPTURE_DATA        0x07
#define CAPTURE_CHECKSUM    0x08
#define FRAMING_ABORTED     0x09
//#define DEFAULT             0x80

//#define HSTATE_ERRABORT     0x40


// the following are counts / offsets used in receive framing
#define ADR_LEN_SHORT 1
#define ADR_LEN_LONG  5
#define EXP_INDEX     (ADR_LEN_LONG + 1)
#define CMD_INDEX     (EXP_INDEX + 3)


/*

    LPC2114 UART0 receive behavior with FIFO active and RX threshold set to one.

    OE (Overrun Error) bit accompanies top (first out) character of FIFO.  FIFO has 16
    oldest characters. One or more recent characters have been discarded.

    BI (Break Interrupt) is also accompanied by FE, [PE if ODD PARITY], and DR.  There will be
    a NUL in the receive FIFO which should be discarded.

    If IIR LSIE is set, BI will have a second interrupt immediately following.  There is no DR
    for this event. Note that this interrupt does not wait for the BREAK conditon (line at mark)
    to stop.

    The UART hardware is LEVEL (as opposed to EDGE) sensitive.  Failure to service all outstanding
    interrupts merely causes immediate reentry to the IRQ routine.

    A full-duplex test between an LPC2114 and a PC exhibits no tendency toward lost transmit
    interrupts.

    Below Added 6/5/2005.

    If LSIE is enabed the UART will interrupt twice, once with INTID_LSIE and once
    with INTID_RDA even if the LSIE handler fetches the character.  Without protection,
    i.e checking the LSR for RDR, this results in a character being processed twice.

    In order to keep the Line Status synchronized with the Receive Data, the LSIE has been
    disabled.  All receive processing is done in the Receive Data routine.  The LSR is fetched
    and processed accordingly.

*/

/*********************************************************************
* LOCAL FUNCTIONS AND VARIABLES */
/********************************************************************/


typedef struct RcvState_t
{
    u8           addr_cnt;            // counts all Adressbytes
#if USE_EXPANSION_ADDR
    u8               expans_cnt;           // size of address expansion
#endif
    u8                byte_cnt;             // Bytecount
    u8                rcv_cksum;            // actual checksum
    u8           Hart_flags;           // communication error flags, HART-compatible
    u8           rcv_buf_index;        // received buffer index
    u8           hart_state_rcv;       // Statemachine
    u16           rcv_blocked;
    u8           Hart_gap_time;      /* time between two characters         */
    u8           rcv_buffer[RX_MAXBUFLEN];  //receive buffer
} RcvState_t;

static RcvState_t RcvState[HART_NUM_CHANNELS];

u8 *serial_GetHartGapTimerPtr(u8_least hart_channel)
{
    return &RcvState[hart_channel].Hart_gap_time;
}
// Receiver variables


/** \brief  Initialize the UART for HART communications
*/
void serial_InitSerial(void)
{
    for(u8_least n = 0; n < NELEM(RcvState); n++)
    {
        RcvState[n].Hart_flags     = HRT_ZERO;              /* clear comm error flags */
        RcvState[n].hart_state_rcv = CHECK_FOR_PREAMBLE;    /* initialise the statemachine   */
        RcvState[n].rcv_buf_index  = HRT_ZERO;
        RcvState[n].rcv_blocked    = 0;
    }
}



/** \brief  Set receive state to abort_frame so that the rest of the frame, if any,
    will be discarded.
*/
static void framing_abort(RcvState_t *rstate)
{
    rstate->hart_state_rcv   = FRAMING_ABORTED;
}



/** \brief  Handle the first non-preamble charater after two or more preambles received when
    in frame hunt mode.  This char is treated as a delimeter.  Interpret it here.

    \param[in] c - the non-preamble charactger received.
*/
static void ProcessDelimiter(u8 c, RcvState_t *rstate)
{
    //we are guaranteed to have at least two GOOD preambles, and now got a GOOD delimiter
    //delimiter = c & (FRM_MASK | LONG_FRAME);
    /*
      For conformance, we must treat delimiter candidates with bits 3 and/or 4 set the same
      as if these bits were 0.  Thus 0x1a, 0x12 and 0xa are all treated the same as 0x2.
      However these reserved bits are not copied into the response frame. While they could
      be masked here the actual received delimitier is place in the received frame.  The
      response masks them off with a kludge in hart.c
    */
    switch (c & FRM_MASK)  /* what kind of Frame was received - 3 legal values ? */
    {
        case STX_FRAME:     /* fall thru */
        case ACK_FRAME:     /* fall thru */
        case BRS_FRAME:
#if !USE_EXPANSION_ADDR
            if ((c & EXPANSION_MASK) != 0)   // expansion not allowed, fails conformance
            {
                framing_abort(rstate);        // HART conformance
            }
            else
#endif
            {
                rstate->Hart_flags  = HRT_ZERO; //in case there were bad preambles  before
                rstate->rcv_cksum   = c;        //prime the checksum
                *rstate->rcv_buffer = c;
#if USE_EXPANSION_ADDR
                rstate->expans_cnt  = (c & EXPANSION_MASK) >> EXPANSION_SHIFT;
#endif
                rstate->rcv_buf_index  = HRT_ONE;
                rstate->hart_state_rcv = CAPTURE_ADDRESS;
                rstate->addr_cnt       = ((c & LONG_FRAME) != 0)
                    ? ADR_LEN_LONG
                    : ADR_LEN_SHORT;
            }
            break;

        default:
            rstate->hart_state_rcv = CHECK_FOR_PREAMBLE;  // HART conformance requires this
            break;
    }
}

/** \brief  Unlock the receiver after we have processed a request frame. Called
        from hart.c after decode/execution od the request.

*/
void serial_ReceiveContinue(u8_least hart_channel)
{
    MN_ENTER_CRITICAL();
        RcvState[hart_channel].rcv_blocked = 0;
        RcvState[hart_channel].hart_state_rcv = CHECK_FOR_PREAMBLE;
    MN_EXIT_CRITICAL();
}

/** \brief  Handle a receive exception from the UART
    \param[in] hart_err - HART comm std error code
*/
void rcv_exception(u8 hart_err, u8_least hart_channel)
{
    RcvState[hart_channel].Hart_flags |= hart_err;
    if (
        ((hart_err & OVERRUN) != 0)
        || ((hart_err != 0) && ((RcvState[hart_channel].hart_state_rcv == CHECK_FOR_SOM) || (RcvState[hart_channel].hart_state_rcv == CAPTURE_BYTECOUNT)))
            )
    {
        framing_abort(&RcvState[hart_channel]);
    }
}

void serial_RearmGapTimer(u8_least hart_channel)
{
    RcvState_t *rstate = &RcvState[hart_channel];
	// Trigger the gap timer only if carrier is present
    if (serial_CarrierDetect(hart_channel))
    {
        //exponential representation and decrement saves us the 0-condition check on decrement
        rstate->Hart_gap_time = 1U << GAP_TIME;           /* Gap timer restart       */
    }
}

void serial_ExpireGapTimer(u8_least hart_channel)
{
    RcvState_t *rstate = &RcvState[hart_channel];
    rstate->Hart_gap_time >>= 1U;
    if (rstate->Hart_gap_time == HRT_ZERO)         /* Gaptimeout ?             */
    {
        rstate->rcv_blocked >>= 1;                      // in case stuck
        // Can't receive if HART task hasn't released the frame
        if (rstate->rcv_blocked == 0)
        {
            rstate->hart_state_rcv = CHECK_FOR_PREAMBLE;/* Statemachine restart  */
        }
    }
}


/** \brief  Handle a receive character from the UART - error bits, if any,
        are handled in rcv_exception before we get here.
DOES NOT handle Gap Timer
    \param[in] c - the char to be processed
*/
void serial_RecieveChar(u8 c, u8 uartNo, u8_least hart_channel)
{
    RcvState_t *rstate = &RcvState[hart_channel];
    switch (rstate->hart_state_rcv)
    {
        case CHECK_FOR_PREAMBLE: // abort_frame on error
            if (PREAMBLE == c)
            {
                rstate->hart_state_rcv = CHECK_FOR_SECOND_PREAMBLE;
            }
            break;

        case CHECK_FOR_SECOND_PREAMBLE: // abort_frame on error
            rstate->hart_state_rcv = (PREAMBLE == c)
                ? CHECK_FOR_SOM
                : CHECK_FOR_PREAMBLE;
            break;

        case CHECK_FOR_SOM:     // abort_frame on error
            if (PREAMBLE == c)
            {
                //Nothing to do: just an extra preamble
            }
            else
            {
                if (rstate->rcv_blocked == 0)
                {
                    ProcessDelimiter(c, rstate);
                }
                else
                {
                    /* Can't receive if HART task hasn't released the frame
                    This should never ever happen unless RAM glitch occurred
                    or HART task took ~3 (27.6 ms) character times on a message
                    that must have no response
                    */
                    /*resolution of TFS:9219: don't assert a rogue host
                    - MN_DBG_ASSERT(rstate->rcv_blocked == 0); //should not happen
                    (It is harmless in true HART half-duplex but may show up in HART over serial)
                    */
                    rstate->hart_state_rcv = FRAMING_ABORTED; /* State machine restart after gap timeout */
                }
            }
            break;

        case CAPTURE_ADDRESS:
            *(rstate->rcv_buffer + rstate->rcv_buf_index) = c;
            rstate->rcv_buf_index++;
            rstate->rcv_cksum ^= c;
            if (--(rstate->addr_cnt) == 0)
            {
#if !USE_EXPANSION_ADDR
                // HART conformance test (at least level 5) does not allow expansion
                rstate->rcv_buf_index = CMD_INDEX;
                rstate->hart_state_rcv = CAPTURE_CMD;
#else
                if (rstate->expans_cnt > 0)
                {
                    rstate->rcv_buf_index = EXP_INDEX;
                    rstate->hart_state_rcv = CAPTURE_EXPANSION;
                }
                else
                {
                    rstate->rcv_buf_index = CMD_INDEX;
                    rstate->hart_state_rcv = CAPTURE_CMD;
                }
            }
            break;

        case CAPTURE_EXPANSION:
            *(rstate->rcv_buffer + rstate->rcv_buf_index) = c;
            rstate->rcv_buf_index++;
            rstate->rcv_cksum ^= c;
            if (--(rstate->expans_cnt) == 0)/* all Expansionbytes received ? */
            {
                rstate->rcv_buf_index = CMD_INDEX;
                rstate->hart_state_rcv = CAPTURE_CMD;
#endif
            }
            break;

        case CAPTURE_CMD:
            rstate->hart_state_rcv                  = CAPTURE_BYTECOUNT;
            *(rstate->rcv_buffer + rstate->rcv_buf_index) = c;
            rstate->rcv_buf_index++;
            rstate->rcv_cksum                      ^= c;
            break;

        case CAPTURE_BYTECOUNT:     // abort_frame on error
            rstate->byte_cnt       = c;
            rstate->hart_state_rcv = (HRT_ZERO < rstate->byte_cnt)
                ? CAPTURE_DATA
                : CAPTURE_CHECKSUM;
            *(rstate->rcv_buffer + rstate->rcv_buf_index) = c;
            rstate->rcv_buf_index++;
            rstate->rcv_cksum                    ^= c;
            break;

        case CAPTURE_DATA:
            if (--(rstate->byte_cnt) == 0)
            {
                rstate->hart_state_rcv = CAPTURE_CHECKSUM;
            }
            if (rstate->rcv_buf_index >= (RX_MAXBUFLEN - 1))   // must be space for this char & checksum
            {
                rstate->Hart_flags |= RX_BUFOVL;
            }
            else
            {
                *(rstate->rcv_buffer + rstate->rcv_buf_index) = c;
                rstate->rcv_buf_index++;
            }
            rstate->rcv_cksum ^= c;
            break;

        case CAPTURE_CHECKSUM:
            *(rstate->rcv_buffer + rstate->rcv_buf_index) = c;
            rstate->rcv_buf_index++;
            if (rstate->rcv_cksum != c)
            {
                rstate->Hart_flags |= CHECKSUM;
            }

            //whether there were comm errors or not, process the frame
            hart_rcv_ok(uartNo, rstate->rcv_buffer, rstate->Hart_flags);
            rstate->hart_state_rcv = CHECK_FOR_PREAMBLE;    // receive the next frame to preserve
            rstate->rcv_blocked    = 1u << BLOCK_TM;        // until command processing is done
            break;

        case FRAMING_ABORTED:
        default:
            //Just swallow the character
            break;
    }
}

/** \brief  Handle a receive character from the UART - error bits, if any,
        are handled in rcv_exception before we get here.
    \param[in] c - the char to be processed
*/
void rcv_char(u8 c, u8 uartNo, u8_least hart_channel)
{
    serial_ExpireGapTimer(hart_channel);

    serial_RearmGapTimer(hart_channel);

    serial_RecieveChar(c, uartNo, hart_channel);
}


/* This line marks the end of the source */
