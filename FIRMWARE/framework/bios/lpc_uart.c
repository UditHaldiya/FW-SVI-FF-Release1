#include "mnwrap.h"
#include "lpc2114io.h"
#include "bios_def.h"
#include "serial.h"
#include "timebase.h"
#include "hart.h"

#define BPS_RATE           1200
#define RATE_DIVISOR (PCLKFREQ / (BPS_RATE * 16u))

/* states of the send statemachine */
#define SEND_PREAMBLE       0x10
#define PREAMBLES_SENT      0x11
#define DATA_SENT           0x12
#define LASTBYTE_SENT       0x16
#define TRANSMIT_IDLE       0x17


const u8 uart2hart[] =           // UART to HART receive error translation table
{
    [0]                        = 0U,
    [LSR_FE]                   = FRAMING,
    [LSR_PE]                   = PARITY_ERR,
    [LSR_OE]                   = OVERRUN,
    [LSR_FE | LSR_PE]          = FRAMING | PARITY_ERR,
    [LSR_FE | LSR_OE]          = FRAMING | OVERRUN,
    [LSR_PE | LSR_OE]          = PARITY_ERR | OVERRUN,
    [LSR_FE | LSR_PE | LSR_OE] = FRAMING | PARITY_ERR | OVERRUN
};

// Transmitter variables
static const u8     *ser_send_ptr;        // pointer to the sendbuffer
#if 0
static u8           pre_cnt,              // counts all transmit Preambles
                    xmt_cksum;
static u8           hart_state_xmt;       // Statemachine
#endif
static u8           chr_cnt;              // number of bytes to send



void uart_setup(void)
{
#if 0
    hart_state_xmt = TRANSMIT_IDLE;
#endif
    chr_cnt        = 0;

    uart_t *uart = UART0;
    uart->FCR      = FCRFIFOENA | FCRRSTRX | FCRRSTTX;   /* enable Fifo, every char -> interrupt */
                                                    /* Clear Rx and Tx FIFOs */

    /* Baudrate 1843200 Hz / 96 / 2 = 19200 --> 1200Baud */
    uart->LCR      = LCRDLAB;    /* enable divisor access bit */
    uart->DLL      = RATE_DIVISOR;
    uart->DLM      = 0;
     /* Line control register, data format  */
    uart->LCR      = (LCRLENSEL | LCRPARENA); /* enable odd parity,
                                        select wordlen 8bits,
                                        1 stop bit,
                                        disable divisor access */
    /* Interrupts enable*/
    uart->IER      = (RBRIE | THRIE);        /* Receive- Send interrupt */
}

static void HART_Transmit(u8 ch)
{
    GPIO1->IOCLR   = GPIO1_HART_RTS;                /* switch modem to send; won't hurt if already on     */
    UART0->THR      = ch;                      /* first Preamble to the sendbuffer */
}


/** \brief  Starts sending of a frame
    Called from task context to initiate frame transmit, the rest of the frame
    will be sent in interrupt context.
*/
void serial_SendFrame(u8 uartNo, u8_least len, const u8 *bufptr)
{
    UNUSED_OK(uartNo); //single channel
        MN_ENTER_CRITICAL();
            ser_send_ptr   = bufptr+1;                        /* Initialize the send buffer */
            chr_cnt        = len - 1;

            HART_Transmit(*bufptr);
        MN_EXIT_CRITICAL();
}

/** \brief  Handle a receive interrupt from the UART - error bits, if any,
        are fetched here and rcv_exception is called for processing.

    Oddly enough, per HCF_SPEC_81, the device must respond if an error
    occurs in:
    - command
    - data
    - check byte (checksun)
    In fact, the spec talks about parity error.
    Unfortunately, the twisted nature of the receive interface makes it unreasonable
    to implement a block interface between UART and HART layers; the character
    interface is made instead, and this function belongs therefore to HART layer.

    \param[in] uart - a pointer to UART0
*/
static void rcv_complete(uart_t *uart)
{
    u8  lsrval = uart->LSR;

    u8 com_err = lsrval & (LSR_OE | LSR_PE | LSR_FE);
    // if there is an error condition process it now
    if (com_err != 0)
    {
        if ((com_err & LSR_OE) != 0)
        {
            uart->FCR = FCRFIFOENA | FCRRSTRX;         /* discard Rx FIFO content */
        }
        //And notify the state machine
        rcv_exception(uart2hart[com_err], 0); //0 is the only channel number
    }

    // now process the character - rcv_exception() will change the receive
    // state if required
    if ((lsrval & LSR_RDR) != 0)
    {
        rcv_char(uart->RBR, 0, 0); //0 is the only channel number
    }
}

/** \brief  Process a transmitter empty interrupt
    \param[in] uart - a pointer to the uart 0 hardware
*/
static void xmt_complete(uart_t *uart)
{
#if 1
    if(chr_cnt == 0)
    {
        //nothing to send; disable Tx interrupt
        //uart->CR1 &= ~TXEIE;            // if message complete, disable TX interrupts
        GPIO1->IOSET   = GPIO1_HART_RTS;      /* switch modem to receive state  */
        //notify end of transmission
        hart_xmit_ok(TASKID_HART);
    }
    else
    {
        uart->THR  = *ser_send_ptr++;    /* Character to the sendbuffer */
        --chr_cnt;
    }
#else
    switch (hart_state_xmt)
    {
        case SEND_PREAMBLE:
            uart->THR = PREAMBLE;                   /* Preamble to the sendbuffer   */
            if (0  == --pre_cnt)          /* are there any Preambles to send ?  */
            {
                hart_state_xmt = PREAMBLES_SENT;
            }
            break;
        case LASTBYTE_SENT:
            //wrap up the transmission business
            hart_state_xmt = DATA_SENT;
            uart->THR      = PREAMBLE-1U; //"dribble" byte to account for RTS decay time
            break;

        case DATA_SENT:
            hart_state_xmt = TRANSMIT_IDLE;
            GPIO1->IOSET   = GPIO1_HART_RTS;      /* switch modem to receive state  */
            //notify end of transmission
            hart_xmit_ok(TASKID_HART);
            break;

        case PREAMBLES_SENT:
            if (0 == --chr_cnt)          /* all chars sent ?  */
            {
                hart_state_xmt = LASTBYTE_SENT;
                uart->THR      = xmt_cksum;
            	Hart_time_Reinit(); 		/* burst timer retriggers here */
            }
            else
            {
                xmt_cksum ^= *ser_send_ptr;      /* checksum compute */
                uart->THR  = *ser_send_ptr++;    /* Character to the sendbuffer */
            }
            break;

        default:
            break;
    }
#endif
}




MN_IRQ void Hart_ISR(void)
{
    uart_t *uart = UART0;

    switch (uart->IIR & IIP_INTID)
    {
        case INTID_RDA:
            rcv_complete(uart);             /* Receivestatemachine  */
            break;

        case INTID_THRE:                /* Sendinterrupt ?    */
            xmt_complete(uart);         /* Sendstatemachine     */
            break;

        default:
            break;
    }
    VICVectAddr = NULL;             /* update priority hardware (see datasheet) */
}

bool_t serial_CarrierDetect(u8_least hart_channel)
{
    UNUSED_OK(hart_channel);
    bool_t rslt = false;

    if ((GPIO1->IOPIN & GPIO1_HART_CD) != 0u)   // pin is high when CD present
    {
        rslt = true;
    }
    return rslt;
}


