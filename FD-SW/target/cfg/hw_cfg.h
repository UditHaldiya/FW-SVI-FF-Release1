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

FILE_NAME          hw_cfg.h



FUNCTIONAL_MODULE_DESCRIPTION

    This file contains the configuration settings for the target hardware and
    for the hardware and software interrupts.

=========================================================================== */
#ifndef __HW_CFG_H__
#define __HW_CFG_H__


#if defined (CUSTOMER_HW)

  #define XCLOCK              4000000UL   /* external clock frequency       */

#endif /* BFD_HW, FBK_HW, CUSTOMER_HW */


/* --- CPU Frequency ------------------------------------------------------ */

//#define CPU_FREQ            (XCLOCK / SYS_DIV)
/*@Dresser-Masoneilan project modified */
#define CPU_FREQ            XCLOCK


/* --- EEPROM Interface --------------------------------------------------- */

#if defined (FBK_HW) || defined (FBK2_HW)

  #define SERIAL_FRAM                   /* serial FRAM device is used       */
  #define FRAM_FM32256                  /* Ramtron FM32256 FRAM             */

#if defined (PROCESSOR_M16C_62P) && defined (ENHANCED_IIC)
  #define UART2_RX_interrupt        i2c_ack_handler
  #define UART2_TX_interrupt        i2c_nack_handler
  #define BUS_COLLISION_interrupt   i2c_stsp_handler
#else
  #define UART2_RX_interrupt        I2C_interrupt
  #define UART2_TX_interrupt        __default_interrupt_handler
  #define BUS_COLLISION_interrupt   __default_interrupt_handler
#endif /* PROCESSOR_M16C_62P */

  #define EE_WR_ENABLE
  #define EE_WR_DISABLE

#elif defined (BFD_HW)

  #define SERIAL_EEPROM                 /* serial EEPROM is used            */
  #define EEPROM_AT24C32                /* Atmel 24C32 EEPROM               */

  #define UART2_RX_interrupt        I2C_interrupt
  #define UART2_TX_interrupt        __default_interrupt_handler
  #define BUS_COLLISION_interrupt   __default_interrupt_handler
  #define EEWC                      _BIT(P8,7)   /* EEPROM write control pin */
  #define EEWC_DIR                  _BIT(P8D,7)

  #define EE_WR_ENABLE              EEWC=0;
  #define EE_WR_DISABLE             EEWC=1;

#elif defined (CUSTOMER_HW)

  #define SERIAL_FRAM                   /* serial FRAM device is used       */
  //#define FRAM_FM2464                   /* Ramtron FM24CL64B FRAM             */
  #define FRAM_FM24256                  /* Ramtron FM24W256-G FRAM             */

  #define EE_WR_ENABLE
  #define EE_WR_DISABLE

#else /* not BFD_HW, FBK_HW, FBK2_HW, CUSTOMER_HW */
  #error Hardware platform not defined properly
#endif /* not BFD_HW, FBK_HW, FBK2_HW */

#define USE_CRC_HARDWARE


/* --- Firmware Identification for Software Download ---------------------- */

#define FOOTPRINT_STRING      "Don't Panic" /* "magic" string indicating the existence of a
                                               functioning field device firmware            */
                                            /* has to be identical with the same macro at
                                               bootloader software!                         */


/* --- Fieldbus Controller Configuration ---------------------------------- */

/* FBC Reset Pin (SPC_RES) of SPC4-2 connected to GPIO C Pin 0 */
#define FBC_ASSERT_RESET          (GPIOC->BSRR = GPIO_BSRR_BS0) /* set Port#0 of GPIO C */
#define FBC_RELEASE_RESET         (GPIOC->BRR  = GPIO_BRR_BR0)  /* reset Port#0 of GPIO C */



/* --- Timer Configuration ------------------------------------------------ */

#define MODE_TIMER                  0x00
#define MODE_COUNTER                0x01
#define MODE_ONE_SHOT               0x02
#define MODE_PWM                    0x03

#define NO_GATE_INPUT               0x00
#define GATE_TAIN_LOW               0x10
#define GATE_TAIN_HIGH              0x18

#define NO_PULSE_OUTPUT             0x00
#define PULSE_OUTPUT                0x04

#define CLOCK_F1                    0x00
#define CLOCK_F8                    0x40
#define CLOCK_F32                   0x80
#define CLOCK_FC32                  0xC0


/* --- Single Wire Timer Configuration------------------------------------- */

/* @Dresser-Masoneilan project: Basic Timer TIM6 used as Single Wire timer */

#define SINGLE_WIRE_SET_TIMER(x)    TIM6->ARR = x
//#define SINGLE_WIRE_START_TIMER     TIM6->CR1 |= TIM_CR1_CEN
#define SINGLE_WIRE_START_TIMER     { /* Disable the TIM Counter */                       \
                                        TIM6->CR1 &= (USIGN16)(~((USIGN16)TIM_CR1_CEN));   \
                                                                                            \
                                        /* Reset the Update Disable Bit */                  \
                                        TIM6->CR1 &= (USIGN16)(~((USIGN16)TIM_CR1_UDIS));  \
                                                                                            \
                                        /* Clear the IT pending Bit */                      \
                                        TIM6->SR = (USIGN16)(~((USIGN16)0x0001));          \
                                                                                            \
                                        /* Set the ARR Preload Bit */                       \
                                        TIM6->CR1 |= TIM_CR1_ARPE;                         \
                                                                                            \
                                        /* Set the URS Bit */                               \
                                        TIM6->CR1 |= TIM_CR1_URS;                          \
                                                                                            \
                                        /* Enable the Interrupt sources */                  \
                                        TIM6->DIER &= (USIGN16)~0x0001;                     \
                                                                                            \
                                        /* re-initialize counter */                         \
                                        TIM6->EGR = (USIGN16)0x0001;                       \
                                                                                            \
                                        /* Enable the TIM Counter */                        \
                                        TIM6->CR1 |= TIM_CR1_CEN;                          \
                                      }

//#define SINGLE_WIRE_STOP_TIMER      TIM6->CR1 &= (USIGN16)(~((USIGN16)TIM_CR1_CEN))
#define SINGLE_WIRE_STOP_TIMER      {/* Disable the TIM Counter */                        \
                                       TIM6->CR1 &= (USIGN16)(~((USIGN16)TIM_CR1_CEN));    \
                                                                                            \
                                       /* Set the Update Disable Bit */                     \
                                       TIM6->CR1 |= TIM_CR1_UDIS;                          \
                                                                                            \
                                       /* Clear the IT pending Bit */                       \
                                       TIM6->SR = (USIGN16)~((USIGN16)0x0001);             \
                                      }
#define SINGLE_WIRE_TIMER_STATUS    TIM6->SR & TIM_SR_UIF
#define SINGLE_WIRE_CLEAR_STATUS    TIM6->SR &= (USIGN16)(~((USIGN16)TIM_SR_UIF))


/* --- Single Wire Configuration ------------------------------------------ */

/* @Dresser-Masoneilan project: Single Wire interface over GPIOB Pin9 */
#define SINGLE_WIRE_OUTPUT          (GPIOB->CRH |= 0x00000010)
#define SINGLE_WIRE_INPUT           (GPIOB->CRH |= 0x00000040)
#define SINGLE_WIRE_ASSERT          GPIO_ResetBits(GPIOB,GPIO_Pin_9)
#define SINGLE_WIRE_RELEASE         GPIO_SetBits(GPIOB,GPIO_Pin_9)
#define SINGLE_WIRE_READ            ((GPIOB->IDR & GPIO_Pin_9) != Bit_RESET) ? Bit_SET : Bit_RESET
#define SINGLE_WIRE_WRITE(x)        (x!=Bit_RESET) ? (GPIOB->ODR |= GPIO_Pin_9) : (GPIOB->ODR &= ~GPIO_Pin_9) //(GPIOB->ODR |= (GPIO_Pin_9 & x))//(GPIOB->ODR & 0x00000100)


/*---------------------------UART allocation ------------------------------ */
/*------------------------------------------------------------------------- */

/* --- Check if EMBOS libraries need UART --------------------------------- */

#if defined(OS_LIBMODE_DT) || defined(OS_LIBMODE_DP) || defined(OS_LIBMODE_SP)
  #define OS_NEEDS_UART
#endif

/* --- HART --- */

#ifdef HART_MASTER
/* @Dresser-Masoneilan project: USART2 is used for HART UART */
    #define HART_UART            USART2  /* Place to configure HART UART */

    #define USART2_USED

#endif /* HART_MASTER */

/* --- MODBUS --- */

#ifdef MODBUS_MASTER
    #define MODB_UART            UART_0  /* Place to configure MODBUS UART */
    #if (MODB_UART == UART_0)
      #define UART0_USED
    #else
      #define UART1_USED
    #endif
#endif /* MODBUS_MASTER */

/* --- KD30 Section --------------------------------------------------- */

#ifdef KD30
  #if   !defined(UART1_USED)    /* currently used KD30 core needs UART1 */
    #define UART1_USED
    #define KD30_UART     UART_1
  #else
    #error Error! UART1 already used! Cannot allocate UART1 for KD30 debugger !
  #endif /* !defined(UART1_USED) */
#endif /* defined (KD30) */

/* -------------------------------------------------------------------- */
/* --- UART Selection Section ----------------------------------------- */
/* -------------------------------------------------------------------- */

/* --- OS needs UART -------------------------------------------------- */

#ifdef OS_NEEDS_UART
  #if   !defined(UART1_USED)
    #define UART1_USED
    #define OS_UART     UART_1
  #elif !defined(UART0_USED)
    #define UART0_USED
    #define OS_UART     UART_0
  #else
    #error Error! Both UARTs already defined! Cannot allocate UART for OS!
  #endif /* !defined(UARTX_USED) */
  #define OS_BAUDRATE             4800UL
#endif /* defined (OS_NEEDS_UART) */

/* --- DBG needs UART ------------------------------------------------- */

#ifdef DEBUG_STRING
  #ifndef OS_NEEDS_UART  /* if OS needs UART then no separately UART needed for DEBUG */
    #if   !defined(UART1_USED)
      #define UART1_USED
      #define DBG_UART     UART_1
    #elif !defined(UART0_USED)
      #define UART0_USED
      #define DBG_UART     UART_0
    #else
      #error Error! Both UARTs already defined! Cannot define allocate UART for debug string output!
    #endif /* !defined(UARTX_USED) */
  #define DBG_BAUDRATE                19200
  #define SYS_FREQ                    (XCLOCK)
  #endif /* ifndef OS_NEEDS_UART */
#endif /* defined (DEBUG_STRING) */


/*--------------------------------------------------------------------- */
/* ----------------------UART interrupts assign------------------------ */
/*--------------------------------------------------------------------- */

/* --- HART --- */

#ifdef HART_MASTER
/* @Dresser-Masoneilan project: USART2 is used for HART UART, HART timer clocked with TIM11CLK = PCLK2 */

  #define HM_UART                   USART2
  #define USART2_RX_interrupt        HART_RX_irq
  #define USART2_TX_interrupt        HART_TX_irq

  #define TIM11CLK                  4000000UL

  #define HM_TIMER                  TIM11
  #define TIM11_interrupt           HART_timer_irq
#endif /* HART_MASTER */

/* --- MODBUS --- */

#ifdef MODBUS_MASTER
  #if   (MODB_UART == UART_0)
    #define MM_UART                   U0
    #define UART0_RX_interrupt        int_handler_modb_receive
    #define UART0_TX_interrupt        int_handler_modb_transmit
  #elif (MODB_UART == UART_1)
    #define MM_UART                   U1
    #define UART1_RX_interrupt        int_handler_modb_receive
    #define UART1_TX_interrupt        int_handler_modb_transmit
  #else
    #error No UART interrupt defined for Modbus!
  #endif /* HART_UART == UART_X */

  #define MM_TIMER                    B5
  #define TIB5_interrupt              int_handler_modb_timer
#endif /* MODBUS_MASTER */


/* --- DEBUG Interface --- */

#ifdef DEBUG_STRING
  #ifndef OS_NEEDS_UART                /* If OS needs UART then no separately UART */
                                       /* interrupt is needed for DEBUG            */
    #if   (DBG_UART == UART_0)
      #define UART0_RX_interrupt        __default_interrupt_handler
      #define UART0_TX_interrupt        dbg_tx_handler
    #elif (DBG_UART == UART_1)
      #define UART1_RX_interrupt        __default_interrupt_handler
      #define UART1_TX_interrupt        dbg_tx_handler
    #else
      #error No UART interrupt defined for debug string output!
    #endif /* DBG_UART == UART_X */
  #endif
#endif /* DEBUG_STRING */


/* --- KD30 ROM Debugger --------------------------------------------- */

#ifdef KD30
  #if   (KD30_UART == UART_0)
    #define UART0_RX_interrupt        ROM_DBG_entry_point     /* Defined in   */
    #define UART0_TX_interrupt        ROM_DBG_entry_point     /* acc. XCL-File*/
  #elif (KD30_UART == UART_1)
    #define UART1_RX_interrupt        ROM_DBG_entry_point     /* Defined in   */
    #define UART1_TX_interrupt        ROM_DBG_entry_point     /* acc. XCL-File*/
  #else
    #error No UART interrupt defined for KD30 debugger!
  #endif /* KD30_UART == UART_X */
#endif /* KD30 */


/* --- OS embOS Profiling ------------------------------------------- */

#ifdef OS_NEEDS_UART
  #if   (OS_UART == UART_0)
    #define UART0_RX_interrupt        OS_ISR_rx
    #define UART0_TX_interrupt        OS_ISR_tx
  #elif (OS_UART == UART_1)
    #define UART1_RX_interrupt        OS_ISR_rx
    #define UART1_TX_interrupt        OS_ISR_tx
  #else
    #error No UART interrupt defined for OS!
  #endif /* OS_UART == UART_X */
#endif /* OS_NEEDS_UART */


/* ----------------------------------------------------------------- */
/* Define undefined UART interrupts with __default_interrupt_handler */
/* ----------------------------------------------------------------- */

#if !defined(UART0_USED)
  #define UART0_RX_interrupt          __default_interrupt_handler
  #define UART0_TX_interrupt          __default_interrupt_handler
#endif

#if !defined(UART1_USED)
  #define UART1_RX_interrupt          __default_interrupt_handler
  #define UART1_TX_interrupt          __default_interrupt_handler
#endif


/* ------------------------ Interrupt Configuration -----------------------------*/
/*     Do not change mandatory settings! Interrupt vectors must correspond to    */
/*     all other FBC settings in file HW_IF.H.                                   */
/*     Unused interrupt vectors are initialized to __default_interrupt_handler   */
/* ------------------------------------------------------------------------------*/

  #define BRK_interrupt                 __default_interrupt_handler
  #define BUS_COLLISION_interrupt       __default_interrupt_handler
#ifdef FC_FIND1P_DMA
  #define DMA0_interrupt                int_handler_fbc_dma        /* MANDATORY !!! */
#else
  #define DMA0_interrupt                __default_interrupt_handler
#endif /* FDL_USE_DMA */
  #define DMA1_interrupt                __default_interrupt_handler
  #define KEY_INPUT_interrupt           __default_interrupt_handler
  #define ADC_interrupt                 __default_interrupt_handler
  #define UART2_TX_interrupt            __default_interrupt_handler
  #define SIO3_interrupt                __default_interrupt_handler
  #define SIO4_interrupt                __default_interrupt_handler
  #define TIA0_interrupt                TIA0_handler
#ifdef OS_EMBOS
  #define TIA1_interrupt                OS_IntHandler
#else
  #define TIA1_interrupt                __default_interrupt_handler
#endif /* OS_EMBOS */
  #define TIA2_interrupt                __default_interrupt_handler
  #define TIA3_interrupt                __default_interrupt_handler
  #define TIA4_interrupt                __default_interrupt_handler
  #define TIB0_interrupt                __default_interrupt_handler
  #define TIB1_interrupt                __default_interrupt_handler
  #define TIB2_interrupt                __default_interrupt_handler
  #define TIB3_interrupt                __default_interrupt_handler
  #define TIB4_interrupt                __default_interrupt_handler
#ifndef TIB5_interrupt
  #define TIB5_interrupt                __default_interrupt_handler
#endif /* TIB5_interrupt */
  #define IRQ0_interrupt                __default_interrupt_handler
  #define IRQ1_interrupt                __default_interrupt_handler
  #define IRQ2_interrupt                fbc_interrupt_handler
  #define IRQ3_interrupt                __default_interrupt_handler

/*--------------- Defines for using of software interrupts ---------------*/

#define software_interrupt01            __default_interrupt_handler
#define software_interrupt02            __default_interrupt_handler
#define software_interrupt03            __default_interrupt_handler
#define software_interrupt04            __default_interrupt_handler
#define software_interrupt05            __default_interrupt_handler
#define software_interrupt06            __default_interrupt_handler
#define software_interrupt07            __default_interrupt_handler
#define software_interrupt08            __default_interrupt_handler
#define software_interrupt09            __default_interrupt_handler
#define software_interrupt10            __default_interrupt_handler
#define software_interrupt11            __default_interrupt_handler
#define software_interrupt12            __default_interrupt_handler
#define software_interrupt13            __default_interrupt_handler
#define software_interrupt14            __default_interrupt_handler
#define software_interrupt15            __default_interrupt_handler
#define software_interrupt16            __default_interrupt_handler
#define software_interrupt17            __default_interrupt_handler
#define software_interrupt18            __default_interrupt_handler
#define software_interrupt19            __default_interrupt_handler
#define software_interrupt20            __default_interrupt_handler
#define software_interrupt21            __default_interrupt_handler
#define software_interrupt22            __default_interrupt_handler
#define software_interrupt23            __default_interrupt_handler
#define software_interrupt24            __default_interrupt_handler
#define software_interrupt25            __default_interrupt_handler
#define software_interrupt26            __default_interrupt_handler
#define software_interrupt27            __default_interrupt_handler
#define software_interrupt28            __default_interrupt_handler
#define software_interrupt29            __default_interrupt_handler
#define software_interrupt30            __default_interrupt_handler
#define software_interrupt31            __default_interrupt_handler
#define software_interrupt32            __default_interrupt_handler


/* --- Interrupt Level -----------------------------------------------------------------*/
/* --- NOTE: FBC_INTR_LEVEL and FDL_DMA_INTERRUPT_LEVEL defined at hw_if.h -------------*/

#define DEBUG_IF_INTR_LEVEL             1   /* IRQ level for debug interface            */
#define TIMER_INTR_LEVEL                3   /* IRQ level of the timer                   */
#define I2C_INTERRUPT_PRIORITY_LEVEL    2   /* IRQ level of I2C serial interface        */


/* --- HW-Timer Configuration --------------------------------------------------------- */

#define HW_TIM_DEF_PERIOD           62500UL        /* max. timer period for 16bit timer */
                                                   /* clocked at f32 ms units           */
//#define MAX_HW_TIME                 (HW_TIM_DEF_PERIOD * 32UL / (XCLOCK / 1000UL))
#define MAX_HW_TIME                 (HW_TIM_DEF_PERIOD * 16UL / (XCLOCK / 1000UL))
#define HW_TICKS_PER_MS             (HW_TIM_DEF_PERIOD / MAX_HW_TIME)


/* --- Port Direction Definitions ----------------------------------------------------- */

#define DIR_OUT                     1
#define DIR_IN                      0

/* --- Token LED Port Configuration and LED ON/OFF settings --------------------------- */

#define TOKEN_LED                   _BIT(P6,5)        /* Only valid for Softing BFD_HW. */
#define TOKEN_LED_DIR               _BIT(PD6,5)       /* Only valid for Softing BFD_HW. */

#define SWITCH_TOKEN_LED_ON()       {TOKEN_LED = 0;}  /* Only valid for Softing BFD_HW. */
#define SWITCH_TOKEN_LED_OFF()      {TOKEN_LED = 1;}  /* Only valid for Softing BFD_HW. */

/* --- HART trace macro definitions --------------------------------------------------- */
#ifdef HM_TRACE
#define LA_INIT()          do {\
                             P6  &= 0x0F;     /* clear P6.4 .. P6.7   */ \
                             PD6 |= 0xF0;     /* set P6.4 .. P6.7 to  */ \
                           } while (0)        /* port output mode     */

#define LA_HIGH(x)         _BIT(P6,x) = 1
#define LA_LOW(x)          _BIT(P6,x) = 0
#define LA_TRIGGER(x)      do {\
                             volatile int i;\
                             LA_HIGH(x);\
                             for (i=0; i<20; i++);\
                             LA_LOW(x);\
                           } while (0)
#else  /* ! HM_TRACE */
#define LA_INIT()
#define LA_HIGH(x)
#define LA_LOW(x)
#define LA_TRIGGER(x)
#endif /* HM_TRACE */

#endif /* __HW_CFG_H__ */
