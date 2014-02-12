/**
Copyright 2011 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file stm_timer.c

    \brief Driver for timers in the STM32F10xx CPU.  Currently used for
    PWM mode for the D/A I/P output and also for PWM mode control of
    the digital outputs DO_1, DO_2.  Note: the system tick timer is handled
    elsewhere.

    OWNER:
    CPU: STM32F10xx Cortex

    $Revision:  $
*/
#include "config.h"
#include "mnwrap.h"
#include "oswrap.h"
#include "dohwif.h"
#include "stmcommon.h"
#include "pwm.h"
#include "watchdog.h"
#include "inituc.h"
#include "gpio.h"
//#include "configure.h"

//lint ++flb
//#define TIM1_BASE             (APB2PERIPH_BASE + 0x2C00)
#define TIM2_BASE             (APB1PERIPH_BASE + 0x0000u)
#define TIM3_BASE             (APB1PERIPH_BASE + 0x0400u)
#define TIM4_BASE             (APB1PERIPH_BASE + 0x0800)
#define TIM5_BASE             (APB1PERIPH_BASE + 0x0C00u)
//#define TIM6_BASE             (APB1PERIPH_BASE + 0x1000)
//#define TIM7_BASE             (APB1PERIPH_BASE + 0x1400)
//#define TIM8_BASE             (APB2PERIPH_BASE + 0x3400)
//#define TIM9_BASE             (APB2PERIPH_BASE + 0x4C00)
//#define TIM10_BASE            (APB2PERIPH_BASE + 0x5000)
//#define TIM11_BASE            (APB2PERIPH_BASE + 0x5400)
//#define TIM12_BASE            (APB1PERIPH_BASE + 0x1800)
//#define TIM13_BASE            (APB1PERIPH_BASE + 0x1C00)
//#define TIM14_BASE            (APB1PERIPH_BASE + 0x2000)
//#define TIM15_BASE            (APB2PERIPH_BASE + 0x4000)
//#define TIM16_BASE            (APB2PERIPH_BASE + 0x4400)
//#define TIM17_BASE            (APB2PERIPH_BASE + 0x4800)

#define TIM2                HARDWARE(TIMx_TypeDef *, TIM2_BASE)
#define TIM3                HARDWARE(TIMx_TypeDef *, TIM3_BASE)
#define TIM4                HARDWARE(TIMx_TypeDef *, TIM4_BASE)
#define TIM5                HARDWARE(TIMx_TypeDef *, TIM5_BASE)

typedef struct TIMx_TypeDef
{
    IO_REG16(CR1);
    IO_REG16(CR2);
    IO_REG16(SMCR);
    IO_REG16(DIER);
    IO_REG16(SR);
    IO_REG16(EGR);
    IO_REG16(CCMR1);
    IO_REG16(CCMR2);
    IO_REG16(CCER);
    IO_REG16(CNT);
    IO_REG16(PSC);
    IO_REG16(ARR);
    IO_REG16(RCR);
    IO_REG16(CCR1);
    IO_REG16(CCR2);
    IO_REG16(CCR3);
    IO_REG16(CCR4);
    IO_REG16(BDTR);
    IO_REG16(DCR);
    IO_REG16(DMAR);
} TIMx_TypeDef;

//15.4.1 TIMx control register 1 (TIMx_CR1)
//Address offset: 0x00
//Reset value: 0x0000
//15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
//Reserved
//CKD[1:0] ARPE CMS DIR OPM URS UDIS CEN
//rw rw rw rw rw rw rw rw rw rw
//Bits 15:10 Reserved, always read as 0
//Bits 9:8 CKD: Clock division
//This bit-field indicates the division ratio between the timer clock (CK_INT) frequency and
//sampling clock used by the digital filters (ETR, TIx),
#define CKD_DIV1 (0u << 8)
#define CKD_DIV2 (1u << 8)
#define CKD_DIV4 (2u << 8)
#define CKD_RESV (3u << 8)
//00: tDTS = tCK_INT
//01: tDTS = 2 × tCK_INT
//10: tDTS = 4 × tCK_INT
//11: Reserved
//Bit 7 ARPE: Auto-reload preload enable
#define ARPE (1u << 7)
//0: TIMx_ARR register is not buffered
//1: TIMx_ARR register is buffered
//Bits 6:5 CMS: Center-aligned mode selection
#define CMS_EDGE_AL  (0u << 5)
#define CMS_CEN_AL1  (1u << 5)
#define CMS_CEN_AL2  (2u << 5)
#define CMS_CEN_AL3  (3u << 5)
//00: Edge-aligned mode. The counter counts up or down depending on the direction bit
//(DIR).
//01: Center-aligned mode 1. The counter counts up and down alternatively. Output compare
//interrupt flags of channels configured in output (CCxS=00 in TIMx_CCMRx register) are set
//only when the counter is counting down.
//10: Center-aligned mode 2. The counter counts up and down alternatively. Output compare
//interrupt flags of channels configured in output (CCxS=00 in TIMx_CCMRx register) are set
//only when the counter is counting up.
//11: Center-aligned mode 3. The counter counts up and down alternatively. Output compare
//interrupt flags of channels configured in output (CCxS=00 in TIMx_CCMRx register) are set
//both when the counter is counting up or down.
//Note: It is not allowed to switch from edge-aligned mode to center-aligned mode as long as
//the counter is enabled (CEN=1)
//Bit 4 DIR: Direction
#define DIR_DN (1u << 4)
//0: Counter used as upcounter
//1: Counter used as downcounter
//Note: This bit is read only when the timer is configured in Center-aligned mode or Encoder
//mode.
//Bit 3 OPM: One-pulse mode
#define OPM (1u << 3)
//0: Counter is not stopped at update event
//1: Counter stops counting at the next update event (clearing the bit CEN)
//RM0008 General-purpose timers (TIM2 to TIM5)
//Doc ID 13902 Rev 13 387/1093
//Bit 2 URS: Update request source
//This bit is set and cleared by software to select the UEV event sources.
#define URS_OF (1u << 2)
//0: Any of the following events generate an update interrupt or DMA request if enabled.
//These events can be:
//– Counter overflow/underflow
//– Setting the UG bit
//– Update generation through the slave mode controller
//1: Only counter overflow/underflow generates an update interrupt or DMA request if enabled.
//Bit 1 UDIS: Update disable
#define UDIS (1u << 1)
//This bit is set and cleared by software to enable/disable UEV event generation.
//0: UEV enabled. The Update (UEV) event is generated by one of the following events:
//– Counter overflow/underflow
//– Setting the UG bit
//– Update generation through the slave mode controller
//Buffered registers are then loaded with their preload values.
//1: UEV disabled. The Update event is not generated, shadow registers keep their value
//(ARR, PSC, CCRx). However the counter and the prescaler are reinitialized if the UG bit is
//set or if a hardware reset is received from the slave mode controller.
//Bit 0 CEN: Counter enable
#define CEN (1u << 0)
//0: Counter disabled
//1: Counter enabled
//Note: External clock, gated mode and encoder mode can work only if the CEN bit has been
//previously set by software. However trigger mode can set the CEN bit automatically by
//hardware.
//CEN is cleared automatically in one-pulse mode, when an update event occurs.

//15.4.2 TIMx control register 2 (TIMx_CR2)
//Address offset: 0x04
//Reset value: 0x0000
//15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
//Reserved
//TI1S MMS[2:0] CCDS
//Reserved
//rw rw rw rw rw
//Bits 15:8 Reserved, must be kept at reset value.
//Bit 7 TI1S: TI1 selection
#define TI1S (1u << 7)
//0: The TIMx_CH1 pin is connected to TI1 input
//1: The TIMx_CH1, CH2 and CH3 pins are connected to the TI1 input (XOR combination)
//See also Section 14.3.18: Interfacing with Hall sensors on page 317
//Bits 6:4 MMS: Master mode selection
//These bits allow to select the information to be sent in master mode to slave timers for
//synchronization (TRGO). The combination is as follows:
#define MMS_0 (0u << 4)
#define MMS_1 (1u << 4)
#define MMS_2 (2u << 4)
#define MMS_3 (3u << 4)
#define MMS_4 (4u << 4)
#define MMS_5 (5u << 4)
#define MMS_6 (6u << 4)
#define MMS_7 (7u << 4)
//000: Reset - the UG bit from the TIMx_EGR register is used as trigger output (TRGO). If the
//reset is generated by the trigger input (slave mode controller configured in reset mode) then
//the signal on TRGO is delayed compared to the actual reset.
//001: Enable - the Counter enable signal, CNT_EN, is used as trigger output (TRGO). It is
//useful to start several timers at the same time or to control a window in which a slave timer is
//enabled. The Counter Enable signal is generated by a logic OR between CEN control bit and
//the trigger input when configured in gated mode.
//When the Counter Enable signal is controlled by the trigger input, there is a delay on TRGO,
//except if the master/slave mode is selected (see the MSM bit description in TIMx_SMCR
//register).
//010: Update - The update event is selected as trigger output (TRGO). For instance a master
//timer can then be used as a prescaler for a slave timer.
//011: Compare Pulse - The trigger output send a positive pulse when the CC1IF flag is to be
//set (even if it was already high), as soon as a capture or a compare match occurred. (TRGO)
//100: Compare - OC1REF signal is used as trigger output (TRGO)
//101: Compare - OC2REF signal is used as trigger output (TRGO)
//110: Compare - OC3REF signal is used as trigger output (TRGO)
//111: Compare - OC4REF signal is used as trigger output (TRGO)
//Bit 3 CCDS: Capture/compare DMA selection
#define CCDS (1u << 3)
//0: CCx DMA request sent when CCx event occurs
//1: CCx DMA requests sent when update event occurs
//Bits 2:0 Reserved, always read as 0



//15.4.3 TIMx slave mode control register (TIMx_SMCR)
//Address offset: 0x08
//Reset value: 0x0000
//15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
//ETP ECE ETPS[1:0] ETF[3:0] MSM TS[2:0]
//Res.
//SMS[2:0]
//rw rw rw rw rw rw rw rw rw rw rw rw rw rw rw
//Bit 15 ETP: External trigger polarity
//This bit selects whether ETR or ETR is used for trigger operations
#define ETP (1u << 15)
//0: ETR is non-inverted, active at high level or rising edge
//1: ETR is inverted, active at low level or falling edge
//Bit 14 ECE: External clock enable
//This bit enables External clock mode 2.
#define ECE (1u << 14)
//0: External clock mode 2 disabled
//1: External clock mode 2 enabled. The counter is clocked by any active edge on the ETRF
//signal.
//1: Setting the ECE bit has the same effect as selecting external clock mode 1 with TRGI
//connected to ETRF (SMS=111 and TS=111).
//2: It is possible to simultaneously use external clock mode 2 with the following slave modes:
//reset mode, gated mode and trigger mode. Nevertheless, TRGI must not be connected to
//ETRF in this case (TS bits must not be 111).
//3: If external clock mode 1 and external clock mode 2 are enabled at the same time, the
//external clock input is ETRF.

//Bits 13:12 ETPS: External trigger prescaler
//External trigger signal ETRP frequency must be at most 1/4 of CK_INT frequency. A
//prescaler can be enabled to reduce ETRP frequency. It is useful when inputting fast external
//clocks.
#define ETPS_0 (0u << 12)
#define ETPS_1 (1u << 12)
#define ETPS_2 (2u << 12)
#define ETPS_3 (3u << 12)
//00: Prescaler OFF
//01: ETRP frequency divided by 2
//10: ETRP frequency divided by 4
//11: ETRP frequency divided by 8
//Bits 11:8 ETF[3:0]: External trigger filter
//This bit-field then defines the frequency used to sample ETRP signal and the length of the
//digital filter applied to ETRP. The digital filter is made of an event counter in which N events
//are needed to validate a transition on the output:
#define ETF_0 (0u << 8)
#define ETF_1 (1u << 8)
#define ETF_2 (2u << 8)
#define ETF_3 (3u << 8)
#define ETF_4 (4u << 8)
#define ETF_5 (5u << 8)
#define ETF_6 (6u << 8)
#define ETF_7 (7u << 8)
//0000: No filter, sampling is done at fDTS 1000: fSAMPLING=fDTS/8, N=6
//0001: fSAMPLING=fCK_INT, N=2 1001: fSAMPLING=fDTS/8, N=8
//0010: fSAMPLING=fCK_INT, N=4 1010: fSAMPLING=fDTS/16, N=5
//0011: fSAMPLING=fCK_INT, N=8² 1011: fSAMPLING=fDTS/16, N=6
//0100: fSAMPLING=fDTS/2, N=6 1100: fSAMPLING=fDTS/16, N=8
//0101: fSAMPLING=fDTS/2, N=8 1101: fSAMPLING=fDTS/32, N=5
//0110: fSAMPLING=fDTS/4, N=6² 1110: fSAMPLING=fDTS/32, N=6
//0111: fSAMPLING=fDTS/4, N=8 1111: fSAMPLING=fDTS/32, N=8
//Bit 7 MSM: Master/Slave mode
#define MSM (1u << 7)
//0: No action
//1: The effect of an event on the trigger input (TRGI) is delayed to allow a perfect
//synchronization between the current timer and its slaves (through TRGO). It is useful if we
//want to synchronize several timers on a single external event.
//General-purpose timers (TIM2 to TIM5) RM0008
//390/1093 Doc ID 13902 Rev 13
//Bits 6:4 TS: Trigger selection
//This bit-field selects the trigger input to be used to synchronize the counter.
#define TS_0 (0u << 4)
#define TS_1 (1u << 4)
#define TS_2 (2u << 4)
#define TS_3 (3u << 4)
#define TS_4 (4u << 4)
#define TS_5 (5u << 4)
#define TS_6 (6u << 4)
#define TS_7 (7u << 4)
//000: Internal Trigger 0 (ITR0).
//001: Internal Trigger 1 (ITR1).
//010: Internal Trigger 2 (ITR2).
//011: Internal Trigger 3 (ITR3).
//100: TI1 Edge Detector (TI1F_ED)
//101: Filtered Timer Input 1 (TI1FP1)
//110: Filtered Timer Input 2 (TI2FP2)
//111: External Trigger input (ETRF)
//See Table 86: TIMx Internal trigger connection on page 390for more details on ITRx
//meaning for each Timer.
//Note: These bits must be changed only when they are not used (e.g. when SMS=000) to
//avoid wrong edge detections at the transition.
//Bit 3 Reserved, must be kept at reset value.
//Bits 2:0 SMS: Slave mode selection
//When external signals are selected the active edge of the trigger signal (TRGI) is linked to
//the polarity selected on the external input (see Input Control register and Control Register
//description.
#define SMS_0 (0u)
#define SMS_1 (1u)
#define SMS_2 (2u)
#define SMS_3 (3u)
#define SMS_4 (4u)
#define SMS_5 (5u)
#define SMS_6 (6u)
#define SMS_7 (7u)
//000: Slave mode disabled - if CEN = ‘1 then the prescaler is clocked directly by the internal
//clock.
//001: Encoder mode 1 - Counter counts up/down on TI2FP2 edge depending on TI1FP1
//level.
//010: Encoder mode 2 - Counter counts up/down on TI1FP1 edge depending on TI2FP2
//level.
//011: Encoder mode 3 - Counter counts up/down on both TI1FP1 and TI2FP2 edges
//depending on the level of the other input.
//100: Reset Mode - Rising edge of the selected trigger input (TRGI) reinitializes the counter
//and generates an update of the registers.
//101: Gated Mode - The counter clock is enabled when the trigger input (TRGI) is high. The
//counter stops (but is not reset) as soon as the trigger becomes low. Both start and stop of
//the counter are controlled.
//110: Trigger Mode - The counter starts at a rising edge of the trigger TRGI (but it is not
//reset). Only the start of the counter is controlled.
//111: External Clock Mode 1 - Rising edges of the selected trigger (TRGI) clock the counter.
//Note: The gated mode must not be used if TI1F_ED is selected as the trigger input (TS=100).
//Indeed, TI1F_ED outputs 1 pulse for each transition on TI1F, whereas the gated mode
//checks the level of the trigger signal.
//Table 86. TIMx Internal trigger connection(1)
//1. When a timer is not present in the product, the corresponding trigger ITRx is not available.
//Slave TIM ITR0 (TS = 000) ITR1 (TS = 001) ITR2 (TS = 010) ITR3 (TS = 011)
//TIM2 TIM1 TIM8 TIM3 TIM4
//TIM3 TIM1 TIM2 TIM5 TIM4
//TIM4 TIM1 TIM2 TIM3 TIM8
//TIM5 TIM2 TIM3 TIM4 TIM8


//15.4.4 TIMx DMA/Interrupt enable register (TIMx_DIER)
//Address offset: 0x0C
//Reset value: 0x0000
//15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
//Res.
//TDE
//Res
//CC4DE CC3DE CC2DE CC1DE UDE
//Res.
//TIE
//Res
//CC4IE CC3IE CC2IE CC1IE UIE
//rw rw rw rw rw rw rw rw rw rw rw rw
//Bit 15 Reserved, must be kept at reset value.
//Bit 14 TDE: Trigger DMA request enable
#define TDE (1u << 14)
//0: Trigger DMA request disabled.
//1: Trigger DMA request enabled.
//Bit 13 Reserved, always read as 0
//Bit 12 CC4DE: Capture/Compare 4 DMA request enable
#define CC4DE (1u << 12)
//0: CC4 DMA request disabled.
//1: CC4 DMA request enabled.
//Bit 11 CC3DE: Capture/Compare 3 DMA request enable
#define CC3DE (1u << 11)
//0: CC3 DMA request disabled.
//1: CC3 DMA request enabled.
//Bit 10 CC2DE: Capture/Compare 2 DMA request enable
#define CC2DE (1u << 10)
//0: CC2 DMA request disabled.
//1: CC2 DMA request enabled.
//Bit 9 CC1DE: Capture/Compare 1 DMA request enable
#define CC1DE (1u << 9)
//0: CC1 DMA request disabled.
//1: CC1 DMA request enabled.
//Bit 8 UDE: Update DMA request enable
#define UDE (1u << 8)
//0: Update DMA request disabled.
//1: Update DMA request enabled.
//Bit 7 Reserved, must be kept at reset value.
//Bit 6 TIE: Trigger interrupt enable
#define TIE (1u << 6)
//0: Trigger interrupt disabled.
//1: Trigger interrupt enabled.
//Bit 5 Reserved, must be kept at reset value.
//Bit 4 CC4IE: Capture/Compare 4 interrupt enable
#define CC4IE (1u << 4)
//0: CC4 interrupt disabled.
//1: CC4 interrupt enabled.
//Bit 3 CC3IE: Capture/Compare 3 interrupt enable
#define CC3IE (1u << 3)
//0: CC3 interrupt disabled.
//1: CC3 interrupt enabled.

//15.4.5 TIMx status register (TIMx_SR)
//Address offset: 0x10
//Reset value: 0x0000
//Bit 2 CC2IE: Capture/Compare 2 interrupt enable
#define CC2IE (1u << 2)
//0: CC2 interrupt disabled.
//1: CC2 interrupt enabled.
//Bit 1 CC1IE: Capture/Compare 1 interrupt enable
#define CC1IE (1u << 1)
//0: CC1 interrupt disabled.
//1: CC1 interrupt enabled.
//Bit 0 UIE: Update interrupt enable
#define UIE (1u << 0)
//0: Update interrupt disabled.
//1: Update interrupt enabled.


//15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
//Reserved
//CC4OF CC3OF CC2OF CC1OF
//Reserved
//TIF
//Res
//CC4IF CC3IF CC2IF CC1IF UIF
//rc_w0 rc_w0 rc_w0 rc_w0 rc_w0 rc_w0 rc_w0 rc_w0 rc_w0 rc_w0
//Bit 15:13 Reserved, must be kept at reset value.
//Bit 12 CC4OF: Capture/Compare 4 overcapture flag
#define CC4OF (1u << 12)
//refer to CC1OF description
//Bit 11 CC3OF: Capture/Compare 3 overcapture flag
#define CC3OF (1u << 11)
//refer to CC1OF description
//Bit 10 CC2OF: Capture/compare 2 overcapture flag
#define CC2OF (1u << 10)
//refer to CC1OF description
//Bit 9 CC1OF: Capture/Compare 1 overcapture flag
//This flag is set by hardware only when the corresponding channel is configured in input
//capture mode. It is cleared by software by writing it to ‘0’.
#define CC1OF (1u << 9)
//0: No overcapture has been detected.
//1: The counter value has been captured in TIMx_CCR1 register while CC1IF flag was
//already set
//Bits 8:7 Reserved, must be kept at reset value.
//Bit 6 TIF: Trigger interrupt flag
//This flag is set by hardware on trigger event (active edge detected on TRGI input when the
//slave mode controller is enabled in all modes but gated mode, both edges in case gated
//mode is selected). It is cleared by software.
#define TIF (1u << 6)
//0: No trigger event occurred.
//1: Trigger interrupt pending.
//Bit 5 Reserved, always read as 0
//Bit 4 CC4IF: Capture/Compare 4 interrupt flag
#define CC4IF (1u << 4)
//refer to CC1IF description
//Bit 3 CC3IF: Capture/Compare 3 interrupt flag
#define CC3IF (1u << 3)
//refer to CC1IF description
//Bit 2 CC2IF: Capture/Compare 2 interrupt flag
#define CC2IF (1u << 2)
//refer to CC1IF description

//Bit 1 CC1IF: Capture/compare 1 interrupt flag
//If channel CC1 is configured as output:
//This flag is set by hardware when the counter matches the compare value, with some
//exception in center-aligned mode (refer to the CMS bits in the TIMx_CR1 register
//description). It is cleared by software.
#define CC1IF (1u << 1)
//0: No match.
//1: The content of the counter TIMx_CNT has matched the content of the TIMx_CCR1
//register.
//If channel CC1 is configured as input:
//This bit is set by hardware on a capture. It is cleared by software or by reading the
//TIMx_CCR1 register.
//0: No input capture occurred.
//1: The counter value has been captured in TIMx_CCR1 register (An edge has been detected
//on IC1 which matches the selected polarity).
//Bit 0 UIF: Update interrupt flag
//–This bit is set by hardware on an update event. It is cleared by software.
#define UIF (1u << 0)
//0: No update occurred.
//1: Update interrupt pending. This bit is set by hardware when the registers are updated:
//–At overflow or underflow regarding the repetition counter value (update if repetition
//counter = 0) and if the UDIS=0 in the TIMx_CR1 register.
//–When CNT is reinitialized by software using the UG bit in TIMx_EGR register, if URS=0
//and UDIS=0 in the TIMx_CR1 register.
//–When CNT is reinitialized by a trigger event (refer to the synchro control register
//description), if URS=0 and UDIS=0 in the TIMx_CR1 register.


//15.4.6 TIMx event generation register (TIMx_EGR)
//Address offset: 0x14
//Reset value: 0x0000
//15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
//Reserved
//TG
//Res.
//CC4G CC3G CC2G CC1G UG
//w w w w w w
//Bits 15:7 Reserved, must be kept at reset value.
//Bit 6 TG: Trigger generation
//This bit is set by software in order to generate an event, it is automatically cleared by
//hardware.
#define TG (1u << 6)
//0: No action
//1: The TIF flag is set in TIMx_SR register. Related interrupt or DMA transfer can occur if
//enabled.
#define TG (1u << 6)
//Bit 5 Reserved, must be kept at reset value.
//Bit 4 CC4G: Capture/compare 4 generation
#define CC4G (1u << 4)
//refer to CC1G description
//Bit 3 CC3G: Capture/compare 3 generation
#define CC3G (1u << 3)
//refer to CC1G description
//Bit 2 CC2G: Capture/compare 2 generation
#define CC2G (1u << 2)
//refer to CC1G description
//Bit 1 CC1G: Capture/compare 1 generation
//This bit is set by software in order to generate an event, it is automatically cleared by
//hardware.
#define CC1G (1u << 1)
//0: No action
//1: A capture/compare event is generated on channel 1:
//If channel CC1 is configured as output:
//CC1IF flag is set, Corresponding interrupt or DMA request is sent if enabled.
//If channel CC1 is configured as input:
//The current value of the counter is captured in TIMx_CCR1 register. The CC1IF flag is set,
//the corresponding interrupt or DMA request is sent if enabled. The CC1OF flag is set if the
//CC1IF flag was already high.
//Bit 0 UG: Update generation
//This bit can be set by software, it is automatically cleared by hardware.
#define UG (1u << 0)
//0: No action
//1: Re-initialize the counter and generates an update of the registers. Note that the prescaler
//counter is cleared too (anyway the prescaler ratio is not affected). The counter is cleared if
//the center-aligned mode is selected or if DIR=0 (upcounting), else it takes the auto-reload
//value (TIMx_ARR) if DIR=1 (downcounting).
//RM0008 General-purpose timers (TIM2 to TIM5)
//Doc ID 13902 Rev 13 395/1093
//15.4.7 TIMx capture/compare mode register 1 (TIMx_CCMR1)
//Address offset: 0x18
//Reset value: 0x0000
//The channels can be used in input (capture mode) or in output (compare mode). The
//direction of a channel is defined by configuring the corresponding CCxS bits. All the other
//bits of this register have a different function in input and in output mode. For a given bit,
//OCxx describes its function when the channel is configured in output, ICxx describes its
//function when the channel is configured in input. So you must take care that the same bit
//can have a different meaning for the input stage and for the output stage.

//Output compare mode
//15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
//OC2CE OC2M[2:0] OC2PE OC2FE
//CC2S[1:0]
//OC1CE OC1M[2:0] OC1PE OC1FE
//CC1S[1:0]
//IC2F[3:0] IC2PSC[1:0] IC1F[3:0] IC1PSC[1:0]
//rw rw rw rw rw rw rw rw rw rw rw rw rw rw rw rw
//Bit 15 OC2CE: Output compare 2 clear enable
//Bits 14:12 OC2M[2:0]: Output compare 2 mode
//Bit 11 OC2PE: Output compare 2 preload enable
//Bit 10 OC2FE: Output compare 2 fast enable
//Bits 9:8 CC2S[1:0]: Capture/Compare 2 selection
//This bit-field defines the direction of the channel (input/output) as well as the used input.
//00: CC2 channel is configured as output
//01: CC2 channel is configured as input, IC2 is mapped on TI2
//10: CC2 channel is configured as input, IC2 is mapped on TI1
//11: CC2 channel is configured as input, IC2 is mapped on TRC. This mode is working only
//if an internal trigger input is selected through the TS bit (TIMx_SMCR register)
//Note: CC2S bits are writable only when the channel is OFF (CC2E = 0 in TIMx_CCER).


//Bit 7 OC1CE: Output compare 1 clear enable
//OC1CE: Output Compare 1 Clear Enable
#define OCxCE (1u << 7)
//0: OC1Ref is not affected by the ETRF input
//1: OC1Ref is cleared as soon as a High level is detected on ETRF input
//Bits 6:4 OC1M: Output compare 1 mode
//These bits define the behavior of the output reference signal OC1REF from which OC1 and
//OC1N are derived. OC1REF is active high whereas OC1 and OC1N active level depends
//on CC1P and CC1NP bits.
#define OCxM_0 (0u << 4)
#define OCxM_1 (1u << 4)
#define OCxM_2 (2u << 4)
#define OCxM_3 (3u << 4)
#define OCxM_4 (4u << 4)
#define OCxM_5 (5u << 4)
#define OCxM_6 (6u << 4)
#define OCxM_7 (7u << 4)
//000: Frozen - The comparison between the output compare register TIMx_CCR1 and the
//counter TIMx_CNT has no effect on the outputs.(this mode is used to generate a timing
//base).
//001: Set channel 1 to active level on match. OC1REF signal is forced high when the counter
//TIMx_CNT matches the capture/compare register 1 (TIMx_CCR1).
//010: Set channel 1 to inactive level on match. OC1REF signal is forced low when the
//counter TIMx_CNT matches the capture/compare register 1 (TIMx_CCR1).
//011: Toggle - OC1REF toggles when TIMx_CNT=TIMx_CCR1.
//100: Force inactive level - OC1REF is forced low.
//101: Force active level - OC1REF is forced high.
//110: PWM mode 1 - In upcounting, channel 1 is active as long as TIMx_CNT<TIMx_CCR1
//else inactive. In downcounting, channel 1 is inactive (OC1REF=‘0) as long as
//TIMx_CNT>TIMx_CCR1 else active (OC1REF=1).
//111: PWM mode 2 - In upcounting, channel 1 is inactive as long as
//TIMx_CNT<TIMx_CCR1 else active. In downcounting, channel 1 is active as long as
//TIMx_CNT>TIMx_CCR1 else inactive.
//Note: 1: These bits can not be modified as long as LOCK level 3 has been programmed
//(LOCK bits in TIMx_BDTR register) and CC1S=00 (the channel is configured in
//output).
//2: In PWM mode 1 or 2, the OCREF level changes only when the result of the
//comparison changes or when the output compare mode switches from “frozen” mode
//to “PWM” mode.
//Bit 3 OC1PE: Output compare 1 preload enable
#define OCxPE (1u << 3)
//0: Preload register on TIMx_CCR1 disabled. TIMx_CCR1 can be written at anytime, the
//new value is taken in account immediately.
//1: Preload register on TIMx_CCR1 enabled. Read/Write operations access the preload
//register. TIMx_CCR1 preload value is loaded in the active register at each update event.
//Note: 1: These bits can not be modified as long as LOCK level 3 has been programmed
//(LOCK bits in TIMx_BDTR register) and CC1S=00 (the channel is configured in
//output).
//2: The PWM mode can be used without validating the preload register only in onepulse
//mode (OPM bit set in TIMx_CR1 register). Else the behavior is not guaranteed.
//Bit 2 OC1FE: Output compare 1 fast enable
//This bit is used to accelerate the effect of an event on the trigger in input on the CC output.
#define OCxFE (1u << 2)
//0: CC1 behaves normally depending on counter and CCR1 values even when the trigger is
//ON. The minimum delay to activate CC1 output when an edge occurs on the trigger input is
//5 clock cycles.
//1: An active edge on the trigger input acts like a compare match on CC1 output. Then, OC
//is set to the compare level independently from the result of the comparison. Delay to sample
//the trigger input and to activate CC1 output is reduced to 3 clock cycles. OCFE acts only if
//the channel is configured in PWM1 or PWM2 mode.
//Bits 1:0 CC1S: Capture/Compare 1 selection
//This bit-field defines the direction of the channel (input/output) as well as the used input.
#define CCxS_0 (0u << 0)
#define CCxS_1 (1u << 0)
#define CCxS_2 (2u << 0)
#define CCxS_3 (3u << 0)
//00: CC1 channel is configured as output.
//01: CC1 channel is configured as input, IC1 is mapped on TI1.
//10: CC1 channel is configured as input, IC1 is mapped on TI2.
//11: CC1 channel is configured as input, IC1 is mapped on TRC. This mode is working only
//if an internal trigger input is selected through TS bit (TIMx_SMCR register)
//Note: CC1S bits are writable only when the channel is OFF (CC1E = 0 in TIMx_CCER).



//Input capture mode
//Bits 15:12 IC2F: Input capture 2 filter
//Bits 11:10 IC2PSC[1:0]: Input capture 2 prescaler
//Bits 9:8 CC2S: Capture/compare 2 selection
//This bit-field defines the direction of the channel (input/output) as well as the used input.
//00: CC2 channel is configured as output.
//01: CC2 channel is configured as input, IC2 is mapped on TI2.
//10: CC2 channel is configured as input, IC2 is mapped on TI1.
//11: CC2 channel is configured as input, IC2 is mapped on TRC. This mode is working only if
//an internal trigger input is selected through TS bit (TIMx_SMCR register)
//Note: CC2S bits are writable only when the channel is OFF (CC2E = 0 in TIMx_CCER).
//Bits 7:4 IC1F: Input capture 1 filter
//This bit-field defines the frequency used to sample TI1 input and the length of the digital filter
//applied to TI1. The digital filter is made of an event counter in which N events are needed to
//validate a transition on the output:
//0000: No filter, sampling is done at fDTS 1000: fSAMPLING=fDTS/8, N=6
//0001: fSAMPLING=fCK_INT, N=2 1001: fSAMPLING=fDTS/8, N=8
//0010: fSAMPLING=fCK_INT, N=4 1010: fSAMPLING=fDTS/16, N=5
//0011: fSAMPLING=fCK_INT, N=8 1011: fSAMPLING=fDTS/16, N=6
//0100: fSAMPLING=fDTS/2, N=6 1100: fSAMPLING=fDTS/16, N=8
//0101: fSAMPLING=fDTS/2, N=8 1101: fSAMPLING=fDTS/32, N=5
//0110: fSAMPLING=fDTS/4, N=6 1110: fSAMPLING=fDTS/32, N=6
//0111: fSAMPLING=fDTS/4, N=8 1111: fSAMPLING=fDTS/32, N=8
//Note: In current silicon revision, fDTS is replaced in the formula by CK_INT when ICxF[3:0]= 1,
//2 or 3.
//Bits 3:2 IC1PSC: Input capture 1 prescaler
//This bit-field defines the ratio of the prescaler acting on CC1 input (IC1).
//The prescaler is reset as soon as CC1E=0 (TIMx_CCER register).
//00: no prescaler, capture is done each time an edge is detected on the capture input
//01: capture is done once every 2 events
//10: capture is done once every 4 events
//11: capture is done once every 8 events
//Bits 1:0 CC1S: Capture/Compare 1 selection
//This bit-field defines the direction of the channel (input/output) as well as the used input.
//00: CC1 channel is configured as output
//01: CC1 channel is configured as input, IC1 is mapped on TI1
//10: CC1 channel is configured as input, IC1 is mapped on TI2
//11: CC1 channel is configured as input, IC1 is mapped on TRC. This mode is working only if
//an internal trigger input is selected through TS bit (TIMx_SMCR register)
//Note: CC1S bits are writable only when the channel is OFF (CC1E = 0 in TIMx_CCER).

#define CCMR_SFT_1  (0u)
#define CCMR_SFT_2  (8u)


//15.4.8 TIMx capture/compare mode register 2 (TIMx_CCMR2)
//Address offset: 0x1C
//Reset value: 0x0000
//Refer to the above CCMR1 register description.
//Output compare mode
//15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
//OC4CE OC4M[2:0] OC4PE OC4FE
//CC4S[1:0]
//OC3CE OC3M[2:0] OC3PE OC3FE
//CC3S[1:0]
//IC4F[3:0] IC4PSC[1:0] IC3F[3:0] IC3PSC[1:0]
//rw rw rw rw rw rw rw rw rw rw rw rw rw rw rw rw
//Bit 15 OC4CE: Output compare 4 clear enable
//Bits 14:12 OC4M: Output compare 4 mode
//Bit 11 OC4PE: Output compare 4 preload enable
//Bit 10 OC4FE: Output compare 4 fast enable
//Bits 9:8 CC4S: Capture/Compare 4 selection
//This bit-field defines the direction of the channel (input/output) as well as the used input.
//00: CC4 channel is configured as output
//01: CC4 channel is configured as input, IC4 is mapped on TI4
//10: CC4 channel is configured as input, IC4 is mapped on TI3
//11: CC4 channel is configured as input, IC4 is mapped on TRC. This mode is working only if
//an internal trigger input is selected through TS bit (TIMx_SMCR register)
//Note: CC4S bits are writable only when the channel is OFF (CC4E = 0 in TIMx_CCER).
//Bit 7 OC3CE: Output compare 3 clear enable
//Bits 6:4 OC3M: Output compare 3 mode
//Bit 3 OC3PE: Output compare 3 preload enable
//Bit 2 OC3FE: Output compare 3 fast enable
//Bits 1:0 CC3S: Capture/Compare 3 selection
//This bit-field defines the direction of the channel (input/output) as well as the used input.
//00: CC3 channel is configured as output
//01: CC3 channel is configured as input, IC3 is mapped on TI3
//10: CC3 channel is configured as input, IC3 is mapped on TI4
//11: CC3 channel is configured as input, IC3 is mapped on TRC. This mode is working only if
//an internal trigger input is selected through TS bit (TIMx_SMCR register)
//Note: CC3S bits are writable only when the channel is OFF (CC3E = 0 in TIMx_CCER).


//Input capture mode
//15.4.9 TIMx capture/compare enable register (TIMx_CCER)
//Address offset: 0x20
//Reset value: 0x0000
//Bits 15:12 IC4F: Input capture 4 filter
//Bits 11:10 IC4PSC: Input capture 4 prescaler
//Bits 9:8 CC4S: Capture/Compare 4 selection
//This bit-field defines the direction of the channel (input/output) as well as the used input.
//00: CC4 channel is configured as output
//01: CC4 channel is configured as input, IC4 is mapped on TI4
//10: CC4 channel is configured as input, IC4 is mapped on TI3
//11: CC4 channel is configured as input, IC4 is mapped on TRC. This mode is working only if
//an internal trigger input is selected through TS bit (TIMx_SMCR register)
//Note: CC4S bits are writable only when the channel is OFF (CC4E = 0 in TIMx_CCER).
//Bits 7:4 IC3F: Input capture 3 filter
//Bits 3:2 IC3PSC: Input capture 3 prescaler
//Bits 1:0 CC3S: Capture/Compare 3 selection
//This bit-field defines the direction of the channel (input/output) as well as the used input.
//00: CC3 channel is configured as output
//01: CC3 channel is configured as input, IC3 is mapped on TI3
//10: CC3 channel is configured as input, IC3 is mapped on TI4
//11: CC3 channel is configured as input, IC3 is mapped on TRC. This mode is working only if
//an internal trigger input is selected through TS bit (TIMx_SMCR register)
//Note: CC3S bits are writable only when the channel is OFF (CC3E = 0 in TIMx_CCER).
//15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
//Reserved
//CC4P CC4E
//Reserved
//CC3P CC3E
//Reserved
//CC2P CC2E
//Reserved
//rw rw rw rw rw rw rw rw


//Bits 15:14 Reserved, must be kept at reset value.
//Bit 13 CC4P: Capture/Compare 4 output polarity
//refer to CC1P description
//Bit 12 CC4E: Capture/Compare 4 output enable
//refer to CC1E description
//Bits 11:10 Reserved, must be kept at reset value.
//Bit 9 CC3P: Capture/Compare 3 output polarity
//refer to CC1P description
//Bit 8 CC3E: Capture/Compare 3 output enable
//refer to CC1E description
//Bits 7:6 Reserved, must be kept at reset value.
//Bit 5 CC2P: Capture/Compare 2 output polarity
//refer to CC1P description
//General-purpose timers (TIM2 to TIM5) RM0008
//400/1093 Doc ID 13902 Rev 13
//Note: The state of the external IO pins connected to the standard OCx channels depends on the
//OCx channel state and the GPIOand AFIO registers.
//15.4.10 TIMx counter (TIMx_CNT)
//Address offset: 0x24
//Reset value: 0x0000
//Bit 4 CC2E: Capture/Compare 2 output enable
//refer to CC1E description
//Bits 3:2 Reserved, must be kept at reset value.
//Bit 1 CC1P: Capture/Compare 1 output polarity
//CC1 channel configured as output:
#define CCxP (1u << 0)
//0: OC1 active high.
//1: OC1 active low.
//CC1 channel configured as input:
//This bit selects whether IC1 or IC1 is used for trigger or capture operations.
//0: non-inverted: capture is done on a rising edge of IC1. When used as external trigger, IC1
//is non-inverted.
//1: inverted: capture is done on a falling edge of IC1. When used as external trigger, IC1 is
//inverted.
//Bit 0 CC1E: Capture/Compare 1 output enable
//CC1 channel configured as output:
#define CCxE (1u << 0)
//0: Off - OC1 is not active.
//1: On - OC1 signal is output on the corresponding output pin.
//CC1 channel configured as input:
//This bit determines if a capture of the counter value can actually be done into the input
//capture/compare register 1 (TIMx_CCR1) or not.
//0: Capture disabled.
//1: Capture enabled.
//Table 87. Output control bit for standard OCx channels
//CCxE bit OCx output state
//0 Output Disabled (OCx=0, OCx_EN=0)
//1 OCx=OCxREF + Polarity, OCx_EN=1

#define CCER_SFT_1 (0u)
#define CCER_SFT_2 (4u)
#define CCER_SFT_3 (8u)
#define CCER_SFT_4 (12u)

//15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0

//CNT[15:0]
//rw rw rw rw rw rw rw rw rw rw rw rw rw rw rw rw
//Bits 15:0 CNT[15:0]: Counter value
//RM0008 General-purpose timers (TIM2 to TIM5)
//Doc ID 13902 Rev 13 401/1093
//15.4.11 TIMx prescaler (TIMx_PSC)
//Address offset: 0x28
//Reset value: 0x0000
//15.4.12 TIMx auto-reload register (TIMx_ARR)
//Address offset: 0x2C
//Reset value: 0x0000
//15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
//PSC[15:0]
//rw rw rw rw rw rw rw rw rw rw rw rw rw rw rw rw
//Bits 15:0 PSC[15:0]: Prescaler value
//The counter clock frequency CK_CNT is equal to fCK_PSC / (PSC[15:0] + 1).
//PSC contains the value to be loaded in the active prescaler register at each update event.
//15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
//ARR[15:0]
//rw rw rw rw rw rw rw rw rw rw rw rw rw rw rw rw
//Bits 15:0 ARR[15:0]: Prescalervalue
//ARR is the value to be loaded in the actual auto-reload register.
//Refer to the Section 15.3.1: Time-base unit on page 351 for more details about ARR update
//and behavior.
//The counter is blocked while the auto-reload value is null.
//General-purpose timers (TIM2 to TIM5) RM0008
//402/1093 Doc ID 13902 Rev 13
//15.4.13 TIMx capture/compare register 1 (TIMx_CCR1)
//Address offset: 0x34
//Reset value: 0x0000
//15.4.14 TIMx capture/compare register 2 (TIMx_CCR2)
//Address offset: 0x38
//Reset value: 0x0000
//15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
//CCR1[15:0]
//rw rw rw rw rw rw rw rw rw rw rw rw rw rw rw rw
//Bits 15:0 CCR1[15:0]: Capture/Compare 1 value
//If channel CC1 is configured as output:
//CCR1 is the value to be loaded in the actual capture/compare 1 register (preload value).
//It is loaded permanently if the preload feature is not selected in the TIMx_CCMR1 register (bit
//OC1PE). Else the preload value is copied in the active capture/compare 1 register when an
//update event occurs.
//The active capture/compare register contains the value to be compared to the counter
//TIMx_CNT and signaled on OC1 output.
//If channel CC1is configured as input:
//CCR1 is the counter value transferred by the last input capture 1 event (IC1).
//15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
//CCR2[15:0]
//rw rw rw rw rw rw rw rw rw rw rw rw rw rw rw rw
//Bits 15:0 CCR2[15:0]: Capture/Compare 2 value
//If channel CC2 is configured as output:
//CCR2 is the value to be loaded in the actual capture/compare 2 register (preload value).
//It is loaded permanently if the preload feature is not selected in the TIMx_CCMR2 register
//(bit OC2PE). Else the preload value is copied in the active capture/compare 2 register when
//an update event occurs.
//The active capture/compare register contains the value to be compared to the counter
//TIMx_CNT and signalled on OC2 output.
//If channel CC2 is configured as input:
//CCR2 is the counter value transferred by the last input capture 2 event (IC2).
//RM0008 General-purpose timers (TIM2 to TIM5)
//Doc ID 13902 Rev 13 403/1093
//15.4.15 TIMx capture/compare register 3 (TIMx_CCR3)
//Address offset: 0x3C
//Reset value: 0x0000
//15.4.16 TIMx capture/compare register 4 (TIMx_CCR4)
//Address offset: 0x40
//Reset value: 0x0000
//15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
//CCR3[15:0]
//rw rw rw rw rw rw rw rw rw rw rw rw rw rw rw rw
//Bits 15:0 CCR3[15:0]: Capture/Compare value
//If channel CC3 is configured as output:
//CCR3 is the value to be loaded in the actual capture/compare 3 register (preload value).
//It is loaded permanently if the preload feature is not selected in the TIMx_CCMR3 register
//(bit OC3PE). Else the preload value is copied in the active capture/compare 3 register when
//an update event occurs.
//The active capture/compare register contains the value to be compared to the counter
//TIMx_CNT and signalled on OC3 output.
//If channel CC3is configured as input:
//CCR3 is the counter value transferred by the last input capture 3 event (IC3).
//15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
//CCR4[15:0]
//rw rw rw rw rw rw rw rw rw rw rw rw rw rw rw rw
//Bits 15:0 CCR4[15:0]: Capture/Compare value
//1/ if CC4 channel is configured as output (CC4S bits):
//CCR4 is the value to be loaded in the actual capture/compare 4 register (preload value).
//It is loaded permanently if the preload feature is not selected in the TIMx_CCMR4 register (bit
//OC4PE). Else the preload value is copied in the active capture/compare 4 register when an
//update event occurs.
//The active capture/compare register contains the value to be compared to the counter
//TIMx_CNT and signalled on OC4 output.
//2/ if CC4 channel is configured as input (CC4S bits in TIMx_CCMR4 register):
//CCR4 is the counter value transferred by the last input capture 4 event (IC4).
//General-purpose timers (TIM2 to TIM5) RM0008
//404/1093 Doc ID 13902 Rev 13
//15.4.17 TIMx DMA control register (TIMx_DCR)
//Address offset: 0x48
//Reset value: 0x0000
//15.4.18 TIMx DMA address for full transfer (TIMx_DMAR)
//Address offset: 0x4C
//Reset value: 0x0000
//Example of how to use the DMA burst feature
//In this example the timer DMA burst feature is used to update the contents of the CCRx
//registers (x = 2, 3, 4) with the DMA transferring half words into the CCRx registers.
//15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
//Reserved
//DBL[4:0]
//Reserved
//DBA[4:0]
//rw rw rw rw rw rw rw rw rw rw
//Bits 15:13 Reserved, always read as 0
//Bits 12:8 DBL[4:0]: DMA burst length
//This 5-bit vector defines the number of DMA transfers (the timer recognizes a burst transfer
//when a read or a write access is done to the TIMx_DMAR address).
//00000: 1 transfer,
//00001: 2 transfers,
//00010: 3 transfers,
//...
//10001: 18 transfers.
//Bits 7:5 Reserved, always read as 0
//Bits 4:0 DBA[4:0]: DMA base address
//This 5-bit vector defines the base-address for DMA transfers (when read/write access are
//done through the TIMx_DMAR address). DBA is defined as an offset starting from the
//address of the TIMx_CR1 register.
//Example:
//00000: TIMx_CR1,
//00001: TIMx_CR2,
//00010: TIMx_SMCR,
//...
//Example: Let us consider the following transfer: DBL = 7 transfers & DBA = TIMx_CR1. In this
//case the transfer is done to/from 7 registers starting from the TIMx_CR1 address..
//15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
//DMAB[15:0]
//rw rw rw rw rw rw rw rw rw rw rw rw rw rw rw rw
//Bits 15:0 DMAB[15:0]: DMA register for burst accesses
//A read or write operation to the DMAR register accesses the register located at the address
//(TIMx_CR1 address) + (DBA + DMA index) x 4
//where TIMx_CR1 address is the address of the control register 1, DBA is the DMA base
//address configured in TIMx_DCR register, DMA index is automatically controlled by the DMA
//transfer, and ranges from 0 to DBL (DBL configured in TIMx_DCR).
//RM0008 General-purpose timers (TIM2 to TIM5)
//Doc ID 13902 Rev 13 405/1093
//This is done in the following steps:
//1. Configure the corresponding DMA channel as follows:
//– DMA channel peripheral address is the DMAR register address
//– DMA channel memory address is the address of the buffer in the RAM containing
//the data to be transferred by DMA into CCRx registers.
//– Number of data to transfer = 3 (See note below).
//– Circular mode disabled.
//2. Configure the DCR register by configuring the DBA and DBL bit fields as follows:
//DBL = 3 transfers, DBA = 0xE.
//3. Enable the TIMx update DMA request (set the UDE bit in the DIER register).
//4. Enable TIMx
//5. Enable the DMA channel
//Note: This example is for the case where every CCRx register to be updated

//lint --flb



#define PWM_TEST    0
#if PWM_TEST
 #define PWM_WIDTH (6u)
 #define PWM_MSB_BITS (3u)
#else
 #define PWM_WIDTH (16u)
 #define PWM_MSB_BITS (9u)
#endif
#define PWM_LSB_BITS (PWM_WIDTH - PWM_MSB_BITS)
#define PWM_PERIOD ((1u << PWM_MSB_BITS) - 1u)
//#define PWM_MSB_SHFT PWM_LSB_BITS
#define PWM_LSB_SHFT (PWM_MSB_BITS-PWM_LSB_BITS)
#define PWM_LSB_MASK ((1u << (PWM_LSB_BITS + PWM_LSB_SHFT)) - 1u)



CONST_ASSERT(PWM_MSB_BITS >= PWM_LSB_BITS);
#if !PWM_TEST
CONST_ASSERT(PWM_LSB_SHFT == 2u);
CONST_ASSERT(PWM_LSB_MASK == 0x1ffu);
#else
unsigned long a = PWM_MSB_BITS, b = PWM_LSB_SHFT, c = PWM_LSB_MASK;
#endif

// Set PWM to 1900 Hz(/2) vs. 3800 Hz(/1)
// FF is using Divider 0 (/1). For compatibility with SVI 2AP should be 1 (/2)
// #define __TIM3_PSC              (0u)
#define __TIM3_PSC              (1u)


#define __TIM3_ARR              (PWM_PERIOD - 1u)
#define __TIM3_CR1              URS_OF
//#define __TIM3_CR2              (0u)
#define __TIM3_SMCR             (0u)
#define PWM_MODE                (OCxM_6 | OCxPE)
//#define __TIM3_CCMR1            (0u)
#define __TIM3_CCMR2            ((PWM_MODE << CCMR_SFT_1) | (PWM_MODE << CCMR_SFT_2))          // 54
#define __TIM3_CCER             ((CCxE << CCER_SFT_3) | (CCxE << CCER_SFT_4))
//#define __TIM3_DIER             (CC3IE | CC4IE)

#define DO_PERIOD           (16u)       //PCLK1_FREQ / 16 == 125 khz
#define DO_ON_VALUE         (DO_PERIOD / 2u)
#define DO_OFF_VALUE        (0u)

CONST_ASSERT (PCLK1_FREQ == 2000000u);

//----------------------------------------------------------------------------
// Profiler timer controls

#ifdef TIMER1_NOT_AVAILABLE                 // Profiler

#define PRF_DISABLE_TIMER       (0x00u)
#define PRF_CONFIG_TIMER_OFF    (PRF_DISABLE_TIMER | URS_OF)
#define PRF_START_TIMER         (PRF_CONFIG_TIMER_OFF | CEN)

#endif
//----------------------------------------------------------------------------

/** \brief Write the passed 16 bit value to the main PWM for control of the  I/P.
    The value is broken into two parts and fed to channels 3 and 4 of timer 3.

    \param value - the 16 bit unsigned value to be written to the PWM
*/
static void WritePWM(u16_least val)
{
    TIM3->CCR3  = (u16)(val >> PWM_LSB_BITS);
    val         = val << PWM_LSB_SHFT;
    TIM3->CCR4  = ~val & PWM_LSB_MASK;                            //
}


/** \brief set the specified digital output to the specified state.

    \param param - selector: DO_1 or DO_2
    \param value - true = enabled (FET on) , false = disabled (FET off)
*/
void bios_SetDigitalOutput(u8 param, bool_t value)
{
    if ((param == DO_1) || (param == DO_2))
    {   // Check the param
        volatile u16 *adr = (param == DO_1) ? &TIM2->CCR1 : &TIM2->CCR2;
        u16           val = (value == true) ? DO_ON_VALUE : DO_OFF_VALUE;
        *adr = val;
    }
}

/** \brief set the specified digital output to the specified state.

    \param param - selector: DO_1 or DO_2
    \return true = enabled (FET on) , false = disabled (FET off)
*/
bool_t  bios_GetDigitalOutput(u8 param)
{
    bool_t  RetVal = false;

    if ((param == DO_1) || (param == DO_2))
    {   // Check the param
        volatile u16 *adr = (param == DO_1) ? &TIM2->CCR1 : &TIM2->CCR2;
        RetVal = (*adr == DO_ON_VALUE);
    }

    return RetVal;
}

/** \brief configure timer 2 for use as drivers for digital outputs.  Channels 1 and 2 are used
*/
static void set2(void)
{
    TIM2->CR1   = 0;                                // reset command register 1
    TIM2->CR2   = 0;                                // reset command register 2
    TIM2->PSC   = 0;                                // set prescaler
    TIM2->ARR   = DO_PERIOD - 1u;                   // set auto-reload
    TIM2->CCMR1 = ((PWM_MODE << CCMR_SFT_1) | (PWM_MODE << CCMR_SFT_2));                           //
    TIM2->CCMR2 = 0;
    TIM2->CCER  = ((CCxE << CCER_SFT_1) | (CCxE << CCER_SFT_2));    // set capture/compare enable register
    TIM2->SMCR  = 0;
    TIM2->CR1   = __TIM3_CR1 | CEN;                 // set command register 1

    TIM2->CR1 |= (u16)CEN;                               // enable timer
}

/** \brief configure timer 3 for use as the main PWM.  Channels 3 and 4 are used
*/
static void set3(void)
{
    TIM3->CR1   = 0;                                    // reset command register 1
    TIM3->CR2   = 0;                                    // reset command register 2

    TIM3->PSC   = __TIM3_PSC;                           // set prescaler
    TIM3->ARR   = __TIM3_ARR;                           // set auto-reload
    WritePWM(0);
    TIM3->CCMR1 = 0;
    TIM3->CCMR2 = __TIM3_CCMR2;                         //
    TIM3->CCER  = __TIM3_CCER;                          // set capture/compare enable register
    TIM3->SMCR  = __TIM3_SMCR;                          // set slave mode control register
    TIM3->CR1   = __TIM3_CR1;                           // set command register 1

    TIM3->CR1 |= (u16)CEN;                                   // enable timer
}

#if USE_INTERNAL_WATCHDOG == OPTION_ON
/** \brief use timer 5 to measure the time between four successive ticks of
    the nominal 40 khz RC oscillator.

    \return u32 - the rounded average time for one tick (nominal 50 timer ticks)
*/
static u32 measure40khz(void)
{
    u32 x, y = 0u;
    u32 snap;

    for(x = 0; x < 5; x++)              // count off 5 LSI events
    {
        while ((TIM5->SR & 0x10u) == 0u)  // wait here for an LSI event
        {
        }
        TIM5->SR &= (u16)~0x10u;        // reset event in timer 5
        snap = TIM5->CCR4;              // Capture the timer
        switch (x)
        {
            case 0:
                y = snap;               // get start timer 5 value
                break;
            case 4:
                y = snap - y;           // comput 4 LSI event interval
                break;
            default:
                break;
        }
    }
    return (y + 2) / 4;                 // round and divide by 4
}

/** \brief Measure the frequency of the internal Low Speed Oscillator

    \return u32 - the frequency in hertz
*/
u32 tmr_GetLSIFreq(void)
{
    u32 TimeFor1tick;
    u32 LSIFreq;

    TIM5->CR1   = 0;                            // reset command register 1
    TIM5->CR2   = 0;                            // reset command register 2
    TIM5->PSC   = 0;                            // set prescaler
    TIM5->ARR   = 0xffff;                       // set max count interval

    TIM5->CR1  |= (u16)URS_OF;                  //

    TIM5->CCMR1 = 0;
    TIM5->CCMR2 = CCxS_1 << CCMR_SFT_2;         // chan 4 is input, IC1 is mapped on TI1.
    TIM5->CCER  = CCxE << CCER_SFT_4;           // set capture/compare enable register
    TIM5->EGR   = UG;                           // Generate Update
    TIM5->CR1  |= (u16)CEN;                     // enable timer

    TimeFor1tick = measure40khz();
    if (TimeFor1tick != 0u)
    {
        LSIFreq = PCLK1_FREQ / TimeFor1tick;
    }
    else
    {
        LSIFreq = 0u;                           // Will cause an error
    }

    return LSIFreq;
}
#endif

/** \brief configure timer2 so it can be used to drive the digital outputs
*/
void  bios_InitTimer1(void)
{
    set2();
}


/** \brief Configure Timer TIM4 PROFILER timer.
 *  For profiling purposes!
 Single 16-bit timer: TIM4
 Prescaler is set to 19 (divide by 20)
 2MHz clock / 20 -- 1 tick is 10 us

 Function does not retuirn anything, does not receive parameters.

 The function shall exist but the code could be commented.
*/
void PRF_InitTimer(void)
{
#ifdef TIMER1_NOT_AVAILABLE                 // Profiler

    TIM4->CR1   = PRF_DISABLE_TIMER;        // reset command register 1
    TIM4->CR2   = 0u;

    // Now the timer is disabled!
    // Proceed with Initialization
    TIM4->CCMR1 = 0u;
    TIM4->CCMR2 = 0u;                       // chan 4 is input, IC1 is mapped on TI1.
    TIM4->CCER  = 0u;                       // set capture/compare enable register
    TIM4->SMCR  = 0u;

    TIM4->PSC   = 19u;                      // set prescaler to divide by 20, 10us per tick
    TIM4->ARR   = 0xffffu;                  // set max count interval
    TIM4->CR1   = PRF_CONFIG_TIMER_OFF;     // Event control

    TIM4->EGR   = UG;                       // Generate Update

    TIM4->CR1   = PRF_START_TIMER;          // enable timer

#endif                                      // End Profiler --
}

#ifdef TIMER1_NOT_AVAILABLE                 // Profiler
/** \brief Configure Timer TIM4 as Profiler timer.
 *  For profiling purposes!
    \return u16 - the 16-bit Ticks counter -- TIM4
*/
u16 instrum_GetHighResolutionTimer(void)
{
    u16 TempLO;

    TempLO = TIM4->CNT;
    return TempLO;
}
#endif


static u16 Atomic_ lastVal;

/** \brief Initialize PWM counter, channel 4,5 of PWM outputs and module
    specific variables. PWM output is set to zero.

    \param none
*/
void bios_InitPwm(void)
{
    set3();
}

/** \brief return the current (most recent) raw PWM value

*/
u16 pwm_GetValue(void)
{
    return lastVal;
}


/** \brief output the specified 16 bit value to the dual PWM

    \param[in] u16 value - the PWM value 0..65535
*/
void bios_WritePwm(u16 value)
{
    bool_t  EnableIP;

    lastVal = value;

    EnableIP = (value > (u16)MIN_DA_VALUE);
    bios_SetEnableIP(EnableIP);

    WritePWM(value);
}

bool_t bios_DOCardPresent(void)
{
    return true; //always present
}

// end of source
