/*****************************************************************************
*                                                                            *
*                     SOFTING Industrial Automation GmbH                     *
*                          Richard-Reitzner-Allee 6                          *
*                                D-85540 Haar                                *
*                        Phone: ++49-89-4 56 56-0                            *
*                          Fax: ++49-89-4 56 56-3 99                         *
*                                                                            *
*                            SOFTING CONFIDENTIAL                            *
*                                                                            *
*                     Copyright (C) SOFTING IA GmbH 2012                     *
*                             All Rights Reserved                            *
*                                                                            *
* NOTICE: All information contained herein is, and remains the property of   *
*   SOFTING Industrial Automation GmbH and its suppliers, if any. The intel- *
*   lectual and technical concepts contained herein are proprietary to       *
*   SOFTING Industrial Automation GmbH and its suppliers and may be covered  *
*   by German and Foreign Patents, patents in process, and are protected by  *
*   trade secret or copyright law. Dissemination of this information or      *
*   reproduction of this material is strictly forbidden unless prior         *
*   written permission is obtained from SOFTING Industrial Automation GmbH.  *
******************************************************************************
******************************************************************************
*                                                                            *
* PROJECT_NAME             Softing FF/PA FD 2.42                             *
*                                                                            *
* VERSION                  FF - 2.42                                         *
*                          PA - 2.42 (beta)                                  *
*                                                                            *
* DATE                     16. April 2012                                    *
*                                                                            *
*****************************************************************************/


/* ===========================================================================

FILE_NAME          hw_tim.c



FUNCTIONAL_MODULE_DESCRIPTION

Timer modul.

    export:
        hw_tim_start_ms_timer
        hw_tim_stop_timer
        hw_tim_start_rti
        hw_trigger_subscr_irq

=========================================================================== */
#include "keywords.h"
#define  MODULE_ID      (COMP_HW + MOD_HWTIM)

INCLUDES
#include "base.h"
#include "hw_if.h"
#include "hw_cfg.h"
#include "osif.h"

#include "except.h"

LOCAL_DEFINES
#define TIM_10          0
#define TIM_11          1

LOCAL_TYPEDEFS

FUNCTION_DECLARATIONS
extern FUNCTION VOID hw_timer_handler (USIGN8 hw_timer);


EXPORT_DATA

IMPORT_DATA

LOCAL_DATA


/****************************************************************************/

FUNCTION    __irq void TIM10_handler (void)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

Interrupt handler for timer A0.

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY
    ENTER_IRQ ();

    hw_timer_handler (TIM_10);

    LEAVE_IRQ ();
       
    /* Set the Autoreload Register value */
    TIM10->ARR = HW_TIM_DEF_PERIOD - 1;
        
    /* re-initialize counter */
    TIM10->EGR = (USIGN16)0x0001;
    
    /* Reset the Update Disable Bit */
    TIM10->CR1 &= (USIGN16)~((USIGN16)TIM_CR1_UDIS);
    
    /* Clear the IT pending Bit */
    TIM10->SR = (USIGN16)~((USIGN16)0x0001);
    
    return;
}


/****************************************************************************/

FUNCTION GLOBAL void hw_tim_start_timer
(
    IN USIGN8   timer_id,   /* timer identifier */
    IN USIGN16  duration    /* duration */
)
/*----------------------------------------------------------------------------

FUNCTIONAL_DESCRIPTION

The timer with the id timer_id (0 .. 4) is started.

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES


FUNCTION_BODY

    switch (timer_id)
    {
    case TIM_10:
        
        /* Disable the TIM Counter */
        TIM10->CR1 &= (USIGN16)(~((USIGN16)TIM_CR1_CEN));
        
        /* Reset the Update Disable Bit */
        TIM10->CR1 &= (USIGN16)(~((USIGN16)TIM_CR1_UDIS));
        
        /* Clear the IT pending Bit */
        TIM10->SR = (USIGN16)(~((USIGN16)0x0001));
        
        /* Set the Autoreload Register value */
        TIM10->ARR = duration;
        
        /* Set the ARR Preload Bit */
        TIM10->CR1 |= TIM_CR1_ARPE;
        
        /* Set the URS Bit */
        TIM10->CR1 |= TIM_CR1_URS;
               
        /* Enable the Interrupt sources */
        TIM10->DIER |= (USIGN16)0x0001;
        
        /* re-initialize counter */
        TIM10->EGR = (USIGN16)0x0001;
        
        /* Enable the TIM Counter */
        TIM10->CR1 |= TIM_CR1_CEN;
        
        break;
    default:
        break;
    } /* end switch */

    return;
}
/****************************************************************************/

FUNCTION GLOBAL void hw_tim_stop_timer
(
    IN  USIGN8  timer_id
)
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

The timer with the timer_id (0 .. 4) is stopped.

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

    switch (timer_id)
    {
    case TIM_10:
        
        /* Disable the TIM Counter */
        TIM10->CR1 &= (USIGN16)(~((USIGN16)TIM_CR1_CEN));
        
        /* Set the Update Disable Bit */
        TIM10->CR1 |= TIM_CR1_UDIS;
        
        /* Clear the IT pending Bit */
        TIM10->SR = (USIGN16)~((USIGN16)0x0001);        

        break;
    default:
        break;
    } /* end switch */

}


