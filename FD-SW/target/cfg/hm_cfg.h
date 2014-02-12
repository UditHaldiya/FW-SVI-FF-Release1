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

FILE_NAME         hm_cfg.h



DESCRIPTION

=========================================================================== */

#ifndef __HM_CFG_H__
#define __HM_CFG_H__

/* FBK HW settings */

#define HART_INTERRUPT_LEVEL        4       /* Must not be higher than 4 with   */
                                            /* embOS V3.28 or higher            */

#ifndef HM_UART

    #error No UART for HART-Interface defined!

#else

    #define HART_SRIC    RIC_REG(HM_UART)
    #define HART_STIC    TIC_REG(HM_UART)
    #define HART_SMODE   MR_REG(HM_UART)
    #define HART_SBRG    BRG_REG(HM_UART)
    #define HART_SCL     C0_REG(HM_UART)
    #define HART_SCH     C1_REG(HM_UART)
    #define HART_STB     TB_REG(HM_UART)
    #define HART_STBL    TBL_REG(HM_UART)
    #define HART_STBH    TBH_REG(HM_UART)
    #define HART_SRB     RB_REG(HM_UART)
    #define HART_SRBL    RBL_REG(HM_UART)
    #define HART_SRBH    RBH_REG(HM_UART)

#endif


#ifndef HM_TIMER

    #error No Timer for HART-Interface defined!

#else
/* @Dresser-Masoneilan project: TIM11 is used for HART timer */
    #define HART_TIC     TIM_IC(HM_TIMER)
    #define HART_TMR     TIM11->PSC
    #define HART_TVAL    TIM11->ARR

#endif

/* @Dresser-Masoneilan project: USART2 is used for HART UART communication */
#define HM_GET_U16_DATA()               (USIGN16)(USART2->DR & (USIGN16)0x01FF)
#define HM_SEND_U8_DATA(x)              USART2->DR = (x & (USIGN16)0x00FF)

/* @Dresser-Masoneilan project: TIM11 is used for HART timer */
//#define HM_STOP_TIMER()                 TIM11->CR1 &= (USIGN16)(~TIM_CR1_CEN)
//#define HM_START_TIMER()                TIM11->CR1 |= TIM_CR1_CEN

#define HM_STOP_TIMER()               {/* Disable the TIM Counter */                        \
                                       TIM11->CR1 &= (USIGN16)(~((USIGN16)TIM_CR1_CEN));    \
                                                                                            \
                                       /* Set the Update Disable Bit */                     \
                                       TIM11->CR1 |= TIM_CR1_UDIS;                          \
                                                                                            \
                                       /* Clear the IT pending Bit */                       \
                                       TIM11->SR = (USIGN16)~((USIGN16)0x0001);             \
                                      }

#define HM_START_TIMER()              { /* Disable the TIM Counter */                       \
                                        TIM11->CR1 &= (USIGN16)(~((USIGN16)TIM_CR1_CEN));   \
                                                                                            \
                                        /* Reset the Update Disable Bit */                  \
                                        TIM11->CR1 &= (USIGN16)(~((USIGN16)TIM_CR1_UDIS));  \
                                                                                            \
                                        /* Clear the IT pending Bit */                      \
                                        TIM11->SR = (USIGN16)(~((USIGN16)0x0001));          \
                                                                                            \
                                        /* Set the Autoreload Register value */             \
                                        /*TIM11->ARR = x;*/                                     \
                                                                                            \
                                        /* Set the ARR Preload Bit */                       \
                                        TIM11->CR1 |= TIM_CR1_ARPE;                         \
                                                                                            \
                                        /* Set the URS Bit */                               \
                                        TIM11->CR1 |= TIM_CR1_URS;                          \
                                                                                            \
                                        /* Enable the Interrupt sources */                  \
                                        TIM11->DIER |= (USIGN16)0x0001;                     \
                                                                                            \
                                        /* re-initialize counter */                         \
                                        TIM11->EGR = (USIGN16)0x0001;                       \
                                                                                            \
                                        /* Enable the TIM Counter */                        \
                                        TIM11->CR1 |= TIM_CR1_CEN;                          \
                                      }

#define MAX_HART_TELEGRAMM_LEN          100 //54 // max. length of a hart telegram (in bytes)

#define HM_RESET_OVERRUN_ERROR()        do {                                        \
                                        USART2->CR1 &= (USIGN16)(~((USIGN16)0x0004));   /* disable and Enable Receiver again to reset the overrun situation */ \
                                        USART2->CR1 |= ((USIGN16)0x0004);               \
                                        } while (0)


/* @Dresser-Masoneilan project: Interrupt of TIM11 and USART2 Rx/Tx must be disabled */
#define HM_DISABLE_INT()                NVIC_SetPriority(TIM1_TRG_COM_TIM11_IRQn, 255); \
                                        NVIC_SetPriority(USART2_IRQn, 255)

//#define ENABLE_OCD
#ifdef ENABLE_OCD
#define HM_ASSERT_OCD()                 do {                    \
                                            _BIT(P6,0) = 1;     \
                                        } while (0)

#define HM_RELEASE_OCD()                do {                    \
                                            _BIT(P6,0) = 0;     \
                                        } while (0)
#else
#define HM_ASSERT_OCD()
#define HM_RELEASE_OCD()
#endif

/* HART communication settings */
#define HART_BPS                        38400
#define TIMER_BPS                       1200
//#define TIMER_BPS                       2400
//#define TIMER_BPS                       4800
//#define TIMER_BPS                       9600
//#define TIMER_BPS                       19200
//#define TIMER_BPS                       28800
//#define TIMER_BPS                       38400

#define PRIMARY                        /* This is a primary HART master */
#define HART_RETRY_LIMIT 4             /* HART master will retry n times */

#if TIMER_BPS>1200
#define TIMEOUT(ms)                      (((long)ms*TIMER_BPS+10999)/11000)
#define STO                              (TIMEOUT(132))
#define HOLD                             (TIMEOUT(13))
#define RT2                              (TIMEOUT(13)+6)
#define RT1_PRIMARY                      (TIMEOUT(132)+5)
#define RT1_SECONDARY                    (TIMEOUT(135)+11)
#else
#define STO                              28
#define HOLD                             2
#define RT2                              8
#define RT1_PRIMARY                      33
#define RT1_SECONDARY                    41
#endif

#ifdef PRIMARY
#define RT1                              RT1_PRIMARY
#define MASTER_ADDR                      0x80
#else
#define RT1                              RT1_SECONDARY
#define MASTER_ADDR                      0x00
#endif

#define HM_HART_DEVICE_ADDRESS           0
#define HM_MAX_INIT_FAIL                 10

#endif /* __HM_CFG_H__ */
