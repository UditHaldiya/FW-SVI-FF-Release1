#include "mnwrap.h"
#include "oswrap.h"
#include "hartpriv.h"
#include "ispcomm.h"
#include "serial.h"
#include "stmcommon.h"
#include "faultpublic.h"

/** \brief Common processing of HART frames over DMA-mapped UARTs
\param uartNo - UART id
\param prochar - a pointer to character processing function
*/
static void ispcomm_Proc(taskid_t taskid, u8 uartNo, void (*prochar)(u8 c, u8 uartNo, u8_least hart_channel))
{
    hart_DoResetRequest(taskid);
    const HartData_t *hdata = hart_GetAndTestHartData(NULL);
    u8 hart_channel = uart_GetHartTaskAndChannel(uartNo, NULL, NULL);
    while(uart_ProcessUART_DMA_Chars(uartNo, prochar))
    {
        command(hart_channel, hdata);
        serial_ReceiveContinue(hart_channel);             // unlock the receiver
    }
}

/* \brief RTOS task for HART farmes over ISP
\param p_arg - a pointer to the task semaphore (unused)
*/
void isp_CommTask(void *p_arg)
{
    UNUSED_OK(p_arg);
    Hart_init();
    uart_init(UART_1);                               // setup appropriate UARTs

    for (;;)
    {
        mn_waitforsem(TASKID_ISPCOMM);

        ispcomm_Proc(TASKID_ISPCOMM, UART_1, serial_RecieveChar);
    }
}

/* \brief RTOS task for HART farmes over IPC (Inter-processor comm)
\param p_arg - a pointer to the task semaphore (unused)
*/
void ipc_CommTask(void *p_arg)
{
    UNUSED_OK(p_arg);
    Hart_init();
    uart_init(UART_2);                               // setup appropriate UARTs

    for (;;)
    {
        mn_waitforsem(TASKID_IPCCOMM);

#ifdef FAULT_IPC_DISCONNECT
        if(!error_IsFault(FAULT_IPC_DISCONNECT))
#endif
        {
            ispcomm_Proc(TASKID_IPCCOMM, UART_2, serial_RecieveChar);
        }
    }
}

/* This line marks the end of the source */
