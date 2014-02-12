/*
Copyright 2004-2005 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file hart.c
    \brief HART Data Link protocol - frame level processing

    CPU: Philips LPC21x4 (ARM)

    OWNER: Ernie Price

    $Archive: /MNCB/Dev/ESD_Release_3.1.2/FWH6/FIRMWARE/interface/hart/hart.c $
    $Date: 7/28/11 6:19p $
    $Revision: 61 $
    $Author: Arkkhasin $

    \ingroup HARTDLL
*/
/* $History: hart.c $
 *
 * *****************  Version 61  *****************
 * User: Arkkhasin    Date: 7/28/11    Time: 6:19p
 * Updated in $/MNCB/Dev/ESD_Release_3.1.2/FWH6/FIRMWARE/interface/hart
 * LINT unused header
 *
 * *****************  Version 60  *****************
 * User: Arkkhasin    Date: 7/26/11    Time: 4:10p
 * Updated in $/MNCB/Dev/ESD_Release_3.1.2/FWH6/FIRMWARE/interface/hart
 * Port of AP HART 6 version
 *
 * *****************  Version 12  *****************
 * User: Arkkhasin    Date: 7/14/11    Time: 5:27p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/interface/hart
 * TFS:7028 HART 6 is 4.1.1
 *
 * *****************  Version 11  *****************
 * User: Arkkhasin    Date: 6/30/11    Time: 1:59p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/interface/hart
 * TFS:6749 - HART gap timer is not activated if we *however* wake up the
 * HART task to do something with a received frame
 * TFS:6880,6883 - (internal) HART revisions enumerated from highest down.
 * TFS:6876,6402 - A mechanism to "undefine" a HART 5 command in HART 6
 *
 * *****************  Version 10  *****************
 * User: Arkkhasin    Date: 6/22/11    Time: 5:13p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/interface/hart
 * TFS:5986 compile-time test for buffer overflow for xml-based commands
 *
 * *****************  Version 9  *****************
 * User: Arkkhasin    Date: 6/08/11    Time: 6:56p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/interface/hart
 * TFS:6556 Using new oswrap API
 *
 * *****************  Version 8  *****************
 * User: Arkkhasin    Date: 5/28/11    Time: 12:38p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/interface/hart
 * TFS:6474 Addressing test hoisted to hart.c
 *
 * *****************  Version 7  *****************
 * User: Arkkhasin    Date: 5/27/11    Time: 5:08p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/interface/hart
 * TFS:6474,6450: Polling address range and broadcast addressing repair
 *
 * *****************  Version 6  *****************
 * User: Arkkhasin    Date: 4/29/11    Time: 3:39p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/interface/hart
 * TFS:6159 New HART reset strategy
 *
 * *****************  Version 5  *****************
 * User: Arkkhasin    Date: 4/28/11    Time: 12:33p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/interface/hart
 * TFS:6188 Function names and safer compiled-out code
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 4/19/11    Time: 11:15a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/interface/hart
 * Lint
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 3/18/11    Time: 9:57p
 * Updated in $/MNCB/Dev/HART6Devel/FIRMWARE/interface/hart
 * A preliminary HART 5/6 switchable processing
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 3/16/11    Time: 10:29p
 * Created in $/MNCB/Dev/HART6Devel/FIRMWARE/interface/hart
 * Adaptation from the trunk
 *
 * *****************  Version 67  *****************
 * User: Arkkhasin    Date: 3/16/11    Time: 10:26p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Added compatibility with OLD_OSPORT. No functionality change
 *
 * *****************  Version 66  *****************
 * User: Arkkhasin    Date: 11/04/09   Time: 3:40p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Removed unused header
 * Replaced unparenthesized macros and made IAR 5.40 happy
 * Added History comment block
*/
#include "mnwrap.h"
#include "oswrap.h"
#include "serial.h"

#include "hart.h"

#include "mncbdefs.h"
#include "hartfunc.h"

#include "hart.h"
//#include "errcodes.h"
#include "configure.h"
#include "reset.h"

#ifdef OLD_OSPORT
#include "mnassert.h"
static os_event_t   TskHartSemaphore;
#endif //OLD_OSPORT

#include "hartpriv.h"

//#define BURST_BIT       0x01

//#undef MN_INLINE
//#define MN_INLINE


#define SLAVE_RT1           61U      /* 305 ms */
#define SLAVE_RT2          14U      /*  70 ms - Normal burst interval */
#define SLAVE_RT2A         18U      /*  90 ms - Extended burst interval to accomodate Windows */
#define SLAVE_TT0           51U      /* 255 ms */


/************************************************************************
* Here Send- and Receive buffer are declared
************************************************************************/



typedef u32 bytepack_t; // four of MESCO's "diverse" timer bytes at once

//Shift quantities:
//#define HTGRANT 0
#define HTWAIT  8U
#define HTBURST 16U
//#define HTSTARTUP 24

// hart response timers
#define HTIME__AFTER_OSTX             ((bytepack_t)((HRT_ZERO  << HTWAIT) | (SLAVE_RT1  << HTBURST)))
#define HTIME__AFTER_SENT_BACK_STRICT ((bytepack_t)((HRT_ZERO  << HTWAIT) | (SLAVE_RT2  << HTBURST)))
#define HTIME__AFTER_SENT_BACK        ((bytepack_t)((HRT_ZERO  << HTWAIT) | (SLAVE_RT2A << HTBURST)))
#define HTIME__AFTER_STX              ((bytepack_t)((SLAVE_TT0 << HTWAIT) | (SLAVE_RT1  << HTBURST)))
#define HTIME__AFTER_ACK              ((bytepack_t)((HRT_ZERO  << HTWAIT) | (HRT_ONE    << HTBURST)))
#define HTIME__AFTER_START_UP         ((bytepack_t)((HRT_ZERO  << HTWAIT) | (SLAVE_RT1  << HTBURST)))

#define HTIME__AFTER_OACK           HTIME__AFTER_ACK
//#define HTIME__AFTER_FRAMING_ERROR  HTIME__AFTER_OSTX
#define HTIME__AFTER_BACK           HTIME__AFTER_OSTX


#define HRT_ERRBYTECNT          2U  /* bytecount in case of an error */
//
#define SLAVE_ADDRESS_BITS      0x3FU
#define MASTER_ADDRESS_BIT      0x80U
#define BURST_MODE_BIT          0x40U

#define HRT_BURST_SEND          1U


#define SHORT_HDR_LEN (MN_OFFSETOF(short_xmt_hdr_t, rsp_code_1) -\
                      MN_OFFSETOF(short_xmt_hdr_t, delimiter))


// CONST_ASSERT(MN_OFFSETOF(short_xmt_hdr_t, response) == MN_OFFSETOF(long_xmt_hdr_t, response));

#define MASTER_ADDR_SHIFT 7
CONST_ASSERT((1U << MASTER_ADDR_SHIFT) == MASTER_ADDRESS_BIT);

#define CONFORMANCE_KLUDGE_2 0x18U       // strip reserved delimiter bits in response
                                        // we are expected to receive them but not return them


#define LONG_HDR_LEN (MN_OFFSETOF(long_xmt_hdr_t, rsp_code_1) -\
                      MN_OFFSETOF(long_xmt_hdr_t, delimiter))
#define ADR_INDEX 1

typedef struct HartState_t
{
    bytepack_t hart_time;
    bytepack_t hart_time_next;
    const u8 *pRcvBuffer;
    u8 coldStart[2];
    u8           Hart_host;          /* toggled burst host destination      */
    u8           Hart_Rcv_proc;
    u8           Hart_burst_proc;    /* signals burst processing            */
    bool_t       burstwanted;
    u8           last_message_error;
    u8 uartNo;
    u8 xmit_buffer[TX_MAXBUFLEN];
} HartState_t;

static HartState_t HartState[HART_NUM_CHANNELS];


/** \brief A helper to test if a message has been recieved
*/
bool_t hart_IsFrameReceived(u8 hart_channel)
{
    return HartState[hart_channel].Hart_Rcv_proc != 0;
}

/** \brief A helper to clear "message has been recieved" flag
*/
void hart_ClearFrameReceived(u8 hart_channel)
{
    HartState[hart_channel].Hart_Rcv_proc = 0;
}
#if 0
/** \brief Retrieves HART request message length by pointer to data.
    It is safer to require that the pointer to data be known to the caller.
    \param rx_data - a pointer to received data in the receive buffer
    \return data length as reported by the receive buffer
*/
u8 hart_GetReqDataLength(const void *rx_data)
{
    u8 len;
    if(rx_data != NULL)
    {
        const u8 *data_m = rx_data;
        const void *hdr = data_m - MN_OFFSETOF(receive_hdr_t, data);
        const receive_hdr_t *rx_hdr = (const receive_hdr_t *)hdr;
        len = rx_hdr->bytecount;
    }
    else
    {
        len = 0;
    }
    return len;
}
#endif
/** \brief Retrieves HART request message length by pointer to data.

    \param rx_data - a pointer to received data in the receive buffer
    \return true if master bit is set (== Primary Master)
*/
bool_t hart_MsgFromSecondaryMaster(const void *rx_data)
{
    bool_t retval;

    if(rx_data != NULL)
    {
        const u8 *data_m = rx_data;
        const void *hdr = data_m - MN_OFFSETOF(receive_hdr_t, data);
        const receive_hdr_t *rx_hdr = (const receive_hdr_t *)hdr;
        retval = (MASTER_ADDRESS_BIT & rx_hdr->long_address.mfr_id_t_addr) != 0;
    }
    else
    {
        retval = false;
    }
    return retval;
}

/* * * * * *  Initialization Section  * * * * * * * */

/** \brief  Set the current timer values.  To be used on subsequent timer reloads
    until changed by protocol state

    \param[in] settime - the four timer values as bytes in a 32-bit word
*/
static void Hart_time_Set(HartState_t *hstate, bytepack_t settime)
{
    hstate->hart_time      = settime;
    hstate->hart_time_next = settime;
}


/** \brief  Initialize the HART link-layer

*/
void Hart_init(void)
{
    //modem_on();                         /* switch on hart modem */
    for(u8_least hart_channel=0; hart_channel < NELEM(HartState); hart_channel++)
    {
        HartState_t *hstate = &HartState[hart_channel];
        hstate->Hart_host           = HRT_ZERO;
        hstate->coldStart[0] = HART_COLD_START;
        hstate->coldStart[1] = HART_COLD_START;

        Hart_time_Set(hstate, HTIME__AFTER_START_UP);/* Harttimer initializing     */
    }
    serial_InitSerial();
}

/* * * * * *  End Initialization Section  * * * * * * * */


/* * * * * *  Interrupt Section  * * * * * * * */

/** \brief  Post the semaphore to wake the HART task

*/
static void hart_postSem(taskid_t taskid)
{
#if 0
    //timer_PostReq();
    hartreq = true;
#error "Newer RTOS port code is available with testing"
#else
    oswrap_IntrEnter();
    oswrap_PostTaskSemaphore(taskid);
    oswrap_IntrExit();
#endif
}


/** \brief  test the received frame to contain a broadcast address
    \param[in] addr_ptr_req - pointer to the 5 character address structure
    \return bool_t - true if broadcast address
*/
static bool_t BroadcastAddress(const long_address_t *addr_ptr_req)
{
    bool_t ret = false;

    if ( (HRT_ZERO == (addr_ptr_req->mfr_id_t_addr & ~(MASTER_ADDRESS_BIT | BURST_MODE_BIT)))
        && (HRT_ZERO == addr_ptr_req->mfr_device_code)
        && (HRT_ZERO == addr_ptr_req->dev_id_nbr[0])
        && (HRT_ZERO == addr_ptr_req->dev_id_nbr[1])
        && (HRT_ZERO == addr_ptr_req->dev_id_nbr[2]))
    {
        ret = true;
    }
    return ret;
}



/** \brief  test the received frame to contain our address
    \param[in] addr_ptr_req - pointer to the 5 character address structure
    \param[in] hdata - pointer to the  our HART address
    \return bool_t - true if broadcast address
*/
static bool_t OwnAddress(const long_address_t *addr_ptr_req, const HartData_t * hdata)
{
    bool_t ret = false;

    if ( ((addr_ptr_req->mfr_id_t_addr & ~(MASTER_ADDRESS_BIT | BURST_MODE_BIT)) == (MNCB_MANUFACTURER_ID & 0x3fU))
        && (addr_ptr_req->mfr_device_code == VerString[hdata->hart_version].ManufacturerDeviceCode)
        && (addr_ptr_req->dev_id_nbr[0] == hdata->device_id[0])
        && (addr_ptr_req->dev_id_nbr[1] == hdata->device_id[1])
        && (addr_ptr_req->dev_id_nbr[2] == hdata->device_id[2]))
    {
        ret = true;
    }
    return ret;
}


//-------------------------------------------------------------------------

/** \brief A simple helper to extract delimiter from the receive buffer
\return message delimiter character
*/
MN_INLINE u8 get_received_delimiter(const HartState_t *hstate)
{
    const receive_hdr_t *rxHdr  = (const receive_hdr_t*)(const void*)hstate->pRcvBuffer;
    return rxHdr->delimiter;
}

/** \brief  Restart all HART timers
    The value in hart_time_next is based on the most recent timer selection.
    E.g. AFTER_ACK, AFTER_SENT_BACK ... Called at the time of receive or send checksum

*/
static void Hart_time_Reinit(HartState_t *hstate)
{
    hstate->hart_time = hstate->hart_time_next;
}

static void SetTimersOnReceive(HartState_t *hstate)
{
    u8 delimiter = get_received_delimiter(hstate);
    switch (delimiter & FRM_MASK)            /* check frametype */
    {
        case STX_FRAME:
            hstate->hart_time_next = HTIME__AFTER_STX;
            /* EXPLANATION: If we decide later not to process the frame, we'll call
            hart_DropFrame() which will reset WAIT timer. That would be equivalent to
            the old HTIME__AFTER_OSTX if we knew immediately we won't process the frame
            */
            break;

        case ACK_FRAME:
            hstate->hart_time_next = HTIME__AFTER_OACK; //This must take care of triggering the burst response
            break;

        case BRS_FRAME:
            hstate->hart_time_next = HTIME__AFTER_BACK;
            break;

        default:    /* this should not be */
            hstate->hart_time_next = HTIME__AFTER_START_UP;
            break;
    }
    Hart_time_Reinit(hstate);                             /* start Harttimer again */
}

MN_INLINE hart_address_t STXframe_LongAddrType(const HartData_t * hdata, const receive_hdr_t *rxHdr)
{
    const long_address_t *addr_ptr_req;

    addr_ptr_req = &rxHdr->long_address;
    hart_address_t ret;
    if  (OwnAddress(addr_ptr_req, hdata) )
    {
        ret = hart_address_long;
    }
    else if(BroadcastAddress(addr_ptr_req))
    {
        ret = hart_address_broadcast;
    }
    else
    {
        ret = hart_address_not4us;
    }
    return ret;
}

MN_INLINE hart_address_t STXframe_ShortAddrType(const HartData_t * hdata, const receive_hdr_t *rxHdr)
{
    const long_address_t *addr_ptr_req;

    addr_ptr_req = &rxHdr->long_address;
    hart_address_t ret;
    if ((addr_ptr_req->mfr_id_t_addr & SLAVE_ADDRESS_BITS) == hdata->transmit_address) /* the own device short address ? */
    {
        ret = hart_address_short;
    }
    else
    {
        ret = hart_address_not4us;
    }

    return ret;
}

//-------------------------------------------------------------------------


/** \brief  Process a received frame. Called from [within] receive interrupt.
*/
void hart_rcv_ok(u8 uartNo, const u8 *buf, u8 flags)
{
    u8 hart_channel;
    bool_t isBlockDevice;
    taskid_t taskid;
    hart_channel = uart_GetHartTaskAndChannel(uartNo, &taskid, &isBlockDevice);
    HartState_t *hstate = &HartState[hart_channel];
    hstate->burstwanted    = false;            // do not burst at gap timeout
    hstate->pRcvBuffer     = buf;

    hstate->xmit_buffer[0] = hart_channel; //A kludge to use old Hart_DropFrame();

    //What UART receives the response?
    hstate->uartNo = uartNo;

    hstate->Hart_Rcv_proc      = HRT_ONE;
    hstate->last_message_error = flags;                 //at the moment of receipt; not waiting for another byte
    if(!isBlockDevice) //true HART or as good as
    {
        hart_postSem(taskid);  //NOTE: No task switch here because we are in the ISR (which is not obvious)
    }

    SetTimersOnReceive(hstate);
}

typedef struct hart_reset_flag_t
{
    u16 signature;
    taskid_t context;
} hart_reset_flag_t;

static hart_reset_flag_t hart_reset_flag =
{
    .signature = 0U,
    .context = TASKID_IDLE,
};

/** \brief THE API to request reset via HART
*/
void hart_ResetDeviceOnSend(void)
{
    taskid_t context;
    if(oswrap_IsContext(TASKID_HART))
    {
        context = TASKID_HART;
    }
    else if(oswrap_IsContext(TASKID_IPCCOMM))
    {
        context = TASKID_IPCCOMM;
    }
    else //(oswrap_IsContext(TASKID_ISPCOMM))
    {
        context = TASKID_ISPCOMM;
    }
    MN_ENTER_CRITICAL();
        hart_reset_flag.context = context;
        hart_reset_flag.signature=0xAAAA; //Indicator of reset
    MN_EXIT_CRITICAL();
}

/** \brief End-of-transmission handler for character-based interface
    \param taskid - a task to notify
*/
void hart_xmit_ok(taskid_t taskid)
{
    if(hart_reset_flag.signature==0xAAAA)
    {
        hart_postSem(taskid);                             //NOTE: No task switch here because we are in the ISR (which is not obvious)
        /* EXPLANATION: we cannot reset right here because we may upset FRAM
        write in progress.
        NOTE: reset_HardwareReset() acquires the mutex for that purpose.
        We trigger the HART task instead, and let it call reset_HardwareReset().
        */
    }
}


/** \brief  Wake the HART task setting the signal indicating burst timer timeout.
    may be delayed by incoming characters in which case the delay becomes burst timer
    plus gap timeout.

*/
static void signalburst(void)
{
    HartState_t *hstate = &HartState[HART_MAIN_CHANNEL];

    hstate->burstwanted     = false;
    hstate->Hart_burst_proc = HRT_BURST_SEND;  //AK: Just set a flag
    hart_postSem(TASKID_HART);                    // wake the HART task to complete the frame
}

/** \brief  Entry from 5 ms timer interrupt to process running timers

*/
void maintain_timers(void)
{
    bytepack_t t;

    for(u8_least hart_channel=0; hart_channel<NELEM(HartState); hart_channel++)
    {
        HartState_t *hstate = &HartState[hart_channel];
        t          = hstate->hart_time;
        t         |= t >> 4;
        t         |= t >> 2;
        t         |= t >> 1;
        t         &= ((1u << HTBURST) | (1u << HTWAIT));
        hstate->hart_time -= t;                                     // decrement all non-zero counters

        if((hstate->hart_time & (0xffu << HTBURST)) == 0u)          // is burst timer zero ?
        {
            if ((t & (0xffu << HTBURST)) != 0u)             // it just became 0
            {
                hstate->burstwanted = true;                    // wait for gap timer
            }
        }
        //exponential decrement saves us the condition check
        u8 timehandler_result = HART_timehandler(hart_channel);
        if ((timehandler_result == 0) && hstate->burstwanted)
        {
            if(hart_GetHartData()->burst_mode_ctrl != 0)
            {
                if (!serial_CarrierDetect(hart_channel))
                {
                    signalburst();
                }
            }
            else
            {
                hstate->burstwanted = false;
            }
        }
    }
}

/* * * * *  End Interrupt Section * * * * */

/** \brief Compute the checksum (LRC) on the specified string
    Who cares if we process the string backwards - the result is the same

    \param buffer - pointer to the string
    \parm len - length (in bytes) of the string
*/
static u8 CSCompute(const u8 *buffer, u32 len)
{
    u8 sum = 0;

    while(len-- > 0u)
    {
        sum ^= buffer[len];
    }
    return sum;
}


static void hart_SendFrame(const HartState_t *hstate, u8 preambles, u8_least len, u8 *bufptr)
{
    //checksum
    u8 c = CSCompute(bufptr, len-1);
    bufptr[len-1] = c;
    //for now, add dribble character unconditionally
    bufptr[len++] = PREAMBLE-1;
    len += preambles;

    while(preambles!= 0)
    {
        *(--bufptr) = PREAMBLE;
        --preambles;
    }

    serial_SendFrame(hstate->uartNo, len, bufptr);
}



/** \brief  Fill in the static part of the long form header address

    \param[in] hdr - pointer to the header in xmit_buffer
    \param[in] hdata - pointer to our address
*/
static void HeaderAddressCommon(long_xmt_hdr_t *hdr, const HartData_t *hdata)
{
    /* put the own address into the frame */

    hdr->long_address.mfr_device_code = VerString[hdata->hart_version].ManufacturerDeviceCode;
    hdr->long_address.dev_id_nbr[0]   = hdata->device_id[0];
    hdr->long_address.dev_id_nbr[1]   = hdata->device_id[1];
    hdr->long_address.dev_id_nbr[2]   = hdata->device_id[2];
}

/** \brief  Build the burst frame and send it

    \param[in] hdata - pointer to the local HART information
*/
static void Hart_burst_process(HartState_t *hstate, const HartData_t *hdata)
{
    s8_least    ret;
    u8          *dst;
    u8_least    rsp_len;

    long_xmt_hdr_t *hdr = (long_xmt_hdr_t *)hstate->xmit_buffer;

    hdr->delimiter = LONG_FRAME | BRS_FRAME;

    /* The Masteraddressbit is toggled. So both masters are able to receive the burstframes */
    hstate->Hart_host         ^= MASTER_ADDRESS_BIT;
    hdr->long_address.mfr_id_t_addr = (MNCB_MANUFACTURER_ID | BURST_MODE_BIT | hstate->Hart_host);

    HeaderAddressCommon(hdr, hdata);

    /* store default into the responsecode */
    hdr->rsp_code_1  = HART_NO_COMMAND_SPECIFIC_ERRORS;

    dst              = &hdr->response;
    //process the command
    u8 cmd = hdata->burst_mode_cmd;

    const hartburstdispfunc_t * const *p = &hartburstdisptab[hdata->hart_version];
    do
    {
        ret = (*p)(cmd, dst);
        if(ret != -COMMAND_NOT_IMPLEMENTED)
        {
            break;
        }
        p++;
    } while(*p != NULL);

    if(ret > 0) //then it's a length; otherwise an error occurred building the response
    {

        hdr->bytecount   = (u8)ret;
        rsp_len          = LONG_HDR_LEN + (u8_least)ret + 1u; // the 1 is for the checksum

        hdr->rsp_code_2  = hart_makeResponseFlags(hstate->Hart_host != 0);
        hdr->command     = cmd;

        hstate->hart_time_next   = cnfg_GetOptionConfigFlag(STRICT_HART_CONFORMANCE)
            ? HTIME__AFTER_SENT_BACK_STRICT
            : HTIME__AFTER_SENT_BACK;

        /* send it */
        hart_SendFrame(hstate, hdata->nbr_resp_pream, rsp_len, &hdr->delimiter);
    }
}


/** \brief  Force a Hart timeout of the WAIT timer - called by hartfunc.c
    to prevent response to a frame.

    Leave the burst timer running as it is properly based at the checksum of the
    request frame.

*/
static void Hart_timeout(HartState_t *hstate)
{
    bytepack_t t;
    MN_ENTER_CRITICAL();
        t  = hstate->hart_time;
        t &= ~(0xffu << HTWAIT);
        hstate->hart_time = t;
    MN_EXIT_CRITICAL();
}

/** \brief  HART request processing doesn't want us to respond

*/
static void Hart_DropFrameEx(u8_least hart_channel)
{
    Hart_timeout(&HartState[hart_channel]);
    serial_ReceiveContinue(hart_channel);
}

/** \brief Drops the frame from the app (response) part of transmit buffer
*/
void Hart_DropFrameEx1(const u8 *dst)
{
    const u8 *txbuf = dst - MN_OFFSETOF(long_xmt_hdr_t, response);
    u8 hart_channel = txbuf[0];
    Hart_DropFrameEx(hart_channel);
}

#if HART_NUM_CHANNELS == 1
static
#endif
void hart_DoResetRequest(taskid_t taskid)
{
    bool_t do_reset;
    MN_ENTER_CRITICAL();
        do_reset = (hart_reset_flag.context == taskid) && (hart_reset_flag.signature == 0xAAAA);
    MN_EXIT_CRITICAL();

    if(do_reset)
    {
        reset_HardwareReset();
    }
}

/**
  \brief Process a HART command

  \param[in]  hdata - pointer to that hart data
*/
#if HART_NUM_CHANNELS == 1
static
#endif
void command(u8 hart_channel, const HartData_t *hdata)
{
    s32    ret;
    u8_least    rsp_len;
    u8          cmd,
                delimiter,
                *buff;
    hart_address_t address_type;

    HartState_t *hstate = &HartState[hart_channel];

    const receive_hdr_t *rxHdr     = (const receive_hdr_t*)(const void*)(hstate->pRcvBuffer);
         long_xmt_hdr_t *longTxHdr = (long_xmt_hdr_t*)(hstate->xmit_buffer);
        short_xmt_hdr_t *shrtTxHdr = (short_xmt_hdr_t*)(hstate->xmit_buffer);

    cmd       = rxHdr->command;                /* get the actual command nbr */
    delimiter = rxHdr->delimiter;

    if (STX_FRAME == (delimiter & FRM_MASK))            /* STX_FRAME received ? */
    {
        delimiter = (delimiter & ~(FRM_MASK | BRS_FRAME | CONFORMANCE_KLUDGE_2)) | ACK_FRAME;
        if (LONG_FRAME ==(delimiter & LONG_FRAME))
        {
            address_type = STXframe_LongAddrType(hdata, rxHdr);
            // form the long response header
            longTxHdr->delimiter      = delimiter;
            longTxHdr->long_address.mfr_id_t_addr  = MASTER_ADDRESS_BIT & rxHdr->long_address.mfr_id_t_addr;
            longTxHdr->long_address.mfr_id_t_addr |= (MNCB_MANUFACTURER_ID & ~(MASTER_ADDRESS_BIT | BURST_MODE_BIT));
            HeaderAddressCommon(longTxHdr, hdata);
            buff                      = hstate->xmit_buffer + MN_OFFSETOF(long_xmt_hdr_t, delimiter);
            rsp_len                   = LONG_HDR_LEN + 1u;  // = 9 The 1 is for the sumcheck
        }
        else
        {
            address_type = STXframe_ShortAddrType(hdata, rxHdr);
            // form the short response header
            shrtTxHdr->delimiter = delimiter;
            shrtTxHdr->address   = MASTER_ADDRESS_BIT & rxHdr->long_address.mfr_id_t_addr;
            shrtTxHdr->address  |= hdata->transmit_address;
            buff                 = hstate->xmit_buffer + MN_OFFSETOF(short_xmt_hdr_t, delimiter);
            rsp_len              = SHORT_HDR_LEN + 1u; // = 5 The 1 is for the sumcheck
        }

        if(!hart_TestStxAddressing(cmd, address_type))
        {
            //this addressing is not allowed; bail out
            Hart_DropFrameEx(hart_channel);
        }
        else
        {
            //process the request message

            // the following are at the same xmit_buffer offset for long and short headers
            longTxHdr->command    = cmd;

            ret      = -(s32)hstate->last_message_error;
            if (ret == -HART_OK)
            {
#if 1 //Equivalent for the near future and packaging in a module
                const hartdipfunc_t * const *p = &hartdisptab[hdata->hart_version];
                do
                {
                    ret = (*p)(cmd, &rxHdr->data, &longTxHdr->response, rxHdr->bytecount);
                    if(ret != -COMMAND_NOT_IMPLEMENTED)
                    {
                        break;
                    }
                    p++;
                } while(*p != NULL);
                if(ret == -HART_COMMAND_UNDEFINED)
                {
                    ret = -COMMAND_NOT_IMPLEMENTED;
                }
#else
                if(hdata->hart_version == HART_REV_5)
                {
                    ret = hart_Dispatch(cmd, &rxHdr->data, &longTxHdr->response, rxHdr->bytecount);
                }
                else
                {
                    //assume 6 - or 6+?
                    ret = hart6_Dispatch(cmd, &rxHdr->data, &longTxHdr->response, rxHdr->bytecount);
                    if(ret == -COMMAND_NOT_IMPLEMENTED)
                    {
                        //see if legacy implementation is provided
                        ret = hart_Dispatch((u8_least)cmd, &rxHdr->data, &longTxHdr->response, rxHdr->bytecount);
                    }
                }
#endif
            }

            if(ret < 0)
            {
                //By convention, -ret is an error
                longTxHdr->rsp_code_1 = (u8)(-ret);
                longTxHdr->bytecount  = HRT_ERRBYTECNT;
            }
            else
            {
                //By convention, ret is length + (warning << HART_WARNING_SCALER)
                longTxHdr->bytecount = (u8)ret;              //ret is the response length
                longTxHdr->rsp_code_1 = (u8)(((u32)ret) >> HART_WARNING_SCALER);
            }

            if (hdata->burst_mode_ctrl != 0)
            {
                buff[ADR_INDEX] |= BURST_MODE_BIT;
            }

            u8_least ix = (buff[ADR_INDEX] & MASTER_ADDRESS_BIT) >> MASTER_ADDR_SHIFT;
            longTxHdr->rsp_code_2  = hart_makeResponseFlags((MASTER_ADDRESS_BIT & rxHdr->long_address.mfr_id_t_addr) != 0) | hstate->coldStart[ix];
            hstate->coldStart[ix]          = 0;

            // Now that the response is ready, we swallow it if we didn't make it on time
            if( (hstate->hart_time & (0xffu << HTWAIT)) == (u32)HRT_ZERO )
            {
                //Nothing
            }
            else
            {
                //everything checks OK; prime the timers and send the response
                hstate->hart_time      = HRT_ZERO;          // stop the burst timer until after the ack
                hstate->hart_time_next = HTIME__AFTER_ACK;
                hart_SendFrame(hstate, hdata->nbr_resp_pream, (u8)(rsp_len + (u8_least)longTxHdr->bytecount), buff);
            }
        }
    }
    else
    {
        //Not an STX frame - perhaps, listening in the future. For now -
        Hart_DropFrameEx(hart_channel);
    }
}


/**   \brief HART Task Process routine.  Entered from HART semaphore post.
      Process a command and/or issue a burst

*/
static void hart_Proc(void)
{
    hart_DoResetRequest(TASKID_HART);

    const HartData_t *hdata;
    hdata = hart_GetAndTestHartData(NULL);
    u8 do_it;
    HartState_t *hstate;
    u8 hart_channel = HART_MAIN_CHANNEL;

    hstate = &HartState[hart_channel];
    MN_ENTER_CRITICAL(); //FUTURE: Do it as for burst: interrupt can only set it!
        do_it         = hstate->Hart_Rcv_proc;
        hstate->Hart_Rcv_proc = 0;
    MN_EXIT_CRITICAL();
    if (do_it != 0)
    {
        command(hart_channel, hdata);
        serial_ReceiveContinue(hart_channel);             // unlock the receiver
    }
    if (hstate->Hart_burst_proc != 0)
    {
        hstate->Hart_burst_proc = 0;
        if (hdata->burst_mode_ctrl != 0)
        {
            hstate->uartNo = hart2uartMap(hart_channel); //know where to send
            Hart_burst_process(hstate, hdata); /* process burst command */
        }
    }
}

/** \brief uC-OS/II frame for HART task
    \param[in] p_arg - a pointer to the task's wake-up semaphore
*/
#ifdef OLD_OSPORT
void hart_TskHart(void *p_arg)
{
    u8 myErr;
    TskHartSemaphore = (os_event_t)p_arg;
    for (;;)
    {  /* Wait for signalization from UART Interrupt -- or burst timer */
        mn_waitforsem(p_arg, (u16)0, &myErr);

        hart_Proc();
    }
}
#else
void hart_TskHart(void *p_arg)
{
    UNUSED_OK(p_arg);
    Hart_init();
    uart_setup();                               // setup appropriate UARTs

    for (;;)
    {  /* Wait for signalization from UART Interrupt -- or burst timer */
        mn_waitforsem(TASKID_HART);

        hart_Proc();
    }
}
#endif //OLD_OSPORT

/* This line marks the end of the source */
