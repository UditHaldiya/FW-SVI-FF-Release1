/**
Copyright 2011 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file stm_uart.c

    \brief Driver for the U(S)ARTs in the STM32F10xx CPU. Initially used for
    HART over UART for FF APP CPU bringup and calibration.

    OWNER:
    CPU: STM32F10xx Cortex

    $Revision:  $
*/
#include "mnwrap.h"
#include "serial.h"
#include "hart.h"
#include "stmcommon.h"
#include "oswrap.h"
#include "mnassert.h"
#include "mntrapdef.h"
#include "hart.h"

//lint ++flb
#define UART1_BASE          (APB2PERIPH_BASE + 0x3800u)
#define UART2_BASE          (APB1PERIPH_BASE + 0x4400u)
#define UART3_BASE          (APB1PERIPH_BASE + 0x4800u)
#define UART4_BASE          (APB1PERIPH_BASE + 0x4C00u)
#define UART5_BASE          (APB1PERIPH_BASE + 0x5000u)

#define UART1               HARDWARE(UART_TypeDef *, UART1_BASE)
#define UART2               HARDWARE(UART_TypeDef *, UART2_BASE)
#define UART3               HARDWARE(UART_TypeDef *, UART3_BASE)
#define UART4               HARDWARE(UART_TypeDef *, UART4_BASE)
#define UART5               HARDWARE(UART_TypeDef *, UART5_BASE)

typedef struct UART_TypeDef
{
  IO_REG32(SR);
  IO_REG32(DR);
  IO_REG32(BRR);
  IO_REG32(CR1);
  IO_REG32(CR2);
  IO_REG32(CR3);
  IO_REG32(GTPR);
} UART_TypeDef;

//Address offset: 0x00
//Reset value: 0x00C0
//31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16
//Reserved
//15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
//Reserved
//CTS LBD TXE TC RXNE IDLE ORE NE FE PE
//rc_w0 rc_w0 r rc_w0 rc_w0 r r r r r
//Bits 31:10 Reserved, forced by hardware to 0.
//Bit 9 CTS: CTS flag
//This bit is set by hardware when the nCTS input toggles, if the CTSE bit is set. It is cleared
//by software (by writing it to 0). An interrupt is generated if CTSIE=1 in the UART_CR3
//register.
#define CTS (1u << 9)
//0: No change occurred on the nCTS status line
//1: A change occurred on the nCTS status line
//This bit is not available for UART4 & UART5.
//Bit 8 LBD: LIN break detection flag
//This bit is set by hardware when the LIN break is detected. It is cleared by software (by
//writing it to 0). An interrupt is generated if LBDIE = 1 in the UART_CR2 register.
#define LBD (1u << 8)
//0: LIN Break not detected
//1: LIN break detected
//Note: An interrupt is generated when LBD=1 if LBDIE=1
//Bit 7 TXE: Transmit data register empty
//This bit is set by hardware when the content of the TDR register has been transferred into
//the shift register. An interrupt is generated if the TXEIE bit =1 in the UART_CR1 register. It
//is cleared by a write to the UART_DR register.
#define TXE (1u << 7)
//0: Data is not transferred to the shift register
//1: Data is transferred to the shift register)
//Note: This bit is used during single buffer transmission.
//Bit 6 TC: Transmission complete
//This bit is set by hardware if the transmission of a frame containing data is complete and if
//TXE is set. An interrupt is generated if TCIE=1 in the UART_CR1 register. It is cleared by a
//software sequence (a read from the UART_SR register followed by a write to the
//UART_DR register). The TC bit can also be cleared by writing a '0' to it. This clearing
//sequence is recommended only for multibuffer communication.
#define TC (1u << 6)
//0: Transmission is not complete
//1: Transmission is complete
//Bit 5 RXNE: Read data register not empty
//This bit is set by hardware when the content of the RDR shift register has been transferred to
//the UART_DR register. An interrupt is generated if RXNEIE=1 in the UART_CR1 register.
//It is cleared by a read to the UART_DR register. The RXNE flag can also be cleared by
//writing a zero to it. This clearing sequence is recommended only for multibuffer
//communication.
#define RXNE (1u << 5)
//0: Data is not received
//1: Received data is ready to be read.
//RM0008 Universal synchronous asynchronous receiver transmitter (UART)
//Doc ID 13902 Rev 13 789/1093
//Bit 4 IDLE: IDLE line detected
//This bit is set by hardware when an Idle Line is detected. An interrupt is generated if the
//IDLEIE=1 in the UART_CR1 register. It is cleared by a software sequence (an read to the
//UART_SR register followed by a read to the UART_DR register).
#define IDLE (1u << 4)
//0: No Idle Line is detected
//1: Idle Line is detected
//Note: The IDLE bit will not be set again until the RXNE bit has been set itself (i.e. a new idle
//line occurs).
//Bit 3 ORE: Overrun error
//This bit is set by hardware when the word currently being received in the shift register is
//ready to be transferred into the RDR register while RXNE=1. An interrupt is generated if
//RXNEIE=1 in the UART_CR1 register. It is cleared by a software sequence (an read to the
//UART_SR register followed by a read to the UART_DR register).
#define ORE (1u << 3)
//0: No Overrun error
//1: Overrun error is detected
//Note: When this bit is set, the RDR register content will not be lost but the shift register will be
//overwritten. An interrupt is generated on ORE flag in case of Multi Buffer
//communication if the EIE bit is set.
//Bit 2 NE: Noise error flag
//This bit is set by hardware when noise is detected on a received frame. It is cleared by a
//software sequence (an read to the UART_SR register followed by a read to the
//UART_DR register).
#define NE (1u << 2)
//0: No noise is detected
//1: Noise is detected
//Note: This bit does not generate interrupt as it appears at the same time as the RXNE bit
//which itself generates an interrupting interrupt is generated on NE flag in case of Multi
//Buffer communication if the EIE bit is set.
//Bit 1 FE: Framing error
//This bit is set by hardware when a de-synchronization, excessive noise or a break character
//is detected. It is cleared by a software sequence (an read to the UART_SR register
//followed by a read to the UART_DR register).
#define FE (1u << 1)
//0: No Framing error is detected
//1: Framing error or break character is detected
//Note: This bit does not generate interrupt as it appears at the same time as the RXNE bit
//which itself generates an interrupt. If the word currently being transferred causes both
//frame error and overrun error, it will be transferred and only the ORE bit will be set.
//An interrupt is generated on FE flag in case of Multi Buffer communication if the EIE bit
//is set.
//Bit 0 PE: Parity error
//This bit is set by hardware when a parity error occurs in receiver mode. It is cleared by a
//software sequence (a read to the status register followed by a read to the UART_DR data
//register). The software must wait for the RXNE flag to be set before clearing the PE bit.
//An interrupt is generated if PEIE = 1 in the UART_CR1 register.
#define PE (1u << 0)
//0: No parity error
//1: Parity error


//27.6.2 Data register (UART_DR)
//Address offset: 0x04
//Reset value: Undefined
//27.6.3 Baud rate register (UART_BRR)
//Note: The baud counters stop counting if the TE or RE bits are disabled respectively.
//Address offset: 0x08
//Reset value: 0x0000
//31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16
//Reserved
//15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
//Reserved
//DR[8:0]
//rw rw rw rw rw rw rw rw rw
//Bits 31:9 Reserved, forced by hardware to 0.
//Bits 8:0 DR[8:0]: Data value
//Contains the Received or Transmitted data character, depending on whether it is read from
//or written to.
//The Data register performs a double function (read and write) since it is composed of two
//registers, one for transmission (TDR) and one for reception (RDR)
//The TDR register provides the parallel interface between the internal bus and the output
//shift register (see Figure 1).
//The RDR register provides the parallel interface between the input shift register and the
//internal bus.
//When transmitting with the parity enabled (PCE bit set to 1 in the UART_CR1 register), the
//value written in the MSB (bit 7 or bit 8 depending on the data length) has no effect because
//it is replaced by the parity.
//When receiving with the parity enabled, the value read in the MSB bit is the received parity
//bit.


//31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16
//Reserved
//15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
//DIV_Mantissa[11:0] DIV_Fraction[3:0]
//rw rw rw rw rw rw rw rw rw rw rw rw rw rw rw rw
//Bits 31:16 Reserved, forced by hardware to 0.
//Bits 15:4 DIV_Mantissa[11:0]: mantissa of UARTDIV
//These 12 bits define the mantissa of the UART Divider (UARTDIV)
//Bits 3:0 DIV_Fraction[3:0]: fraction of UARTDIV
//These 4 bits define the fraction of the UART Divider (UARTDIV)
#define MANT_SFT    4

//27.6.4 Control register 1 (UART_CR1)
//Address offset: 0x0C
//Reset value: 0x0000
//31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16
//Reserved
//15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0

//Reserved
//UE M WAKE PCE PS PEIE TXEIE TCIE RXNEIE IDLEIE TE RE RWU SBK
//rw rw rw rw rw rw rw rw rw rw rw rw rw rw
//Bits 31:14 Reserved, forced by hardware to 0.
//Bit 13 UE: UART enable
//When this bit is cleared the UART prescalers and outputs are stopped and the end of the
//current
//byte transfer in order to reduce power consumption. This bit is set and cleared by software.
#define UE (1u << 13)
//0: UART prescaler and outputs disabled
//1: UART enabled
//Bit 12 M: Word length
//This bit determines the word length. It is set or cleared by software.
#define M (1u << 12)
//0: 1 Start bit, 8 Data bits, n Stop bit
//1: 1 Start bit, 9 Data bits, n Stop bit
//Note: The M bit must not be modified during a data transfer (both transmission and reception)
//Bit 11 WAKE: Wakeup method
//This bit determines the UART wakeup method, it is set or cleared by software.
#define WAKE (1u << 11)
//0: Idle Line
//1: Address Mark
//Bit 10 PCE: Parity control enable
//This bit selects the hardware parity control (generation and detection). When the parity
//control is enabled, the computed parity is inserted at the MSB position (9th bit if M=1; 8th bit
//if M=0) and parity is checked on the received data. This bit is set and cleared by software.
//Once it is set, PCE is active after the current byte (in reception and in transmission).
#define PCE (1u << 10)
//0: Parity control disabled
//1: Parity control enabled
//Bit 9 PS: Parity selection
//This bit selects the odd or even parity when the parity generation/detection is enabled (PCE
//bit set). It is set and cleared by software. The parity will be selected after the current byte.
#define PS (1u << 9)
//0: Even parity
//1: Odd parity
//Bit 8 PEIE: PE interrupt enable
//This bit is set and cleared by software.
#define PEIE (1u << 8)
//0: Interrupt is inhibited
//1: A UART interrupt is generated whenever PE=1 in the UART_SR register
//Bit 7 TXEIE: TXE interrupt enable
//This bit is set and cleared by software.
#define TXEIE (1u << 7)
//0: Interrupt is inhibited
//1: A UART interrupt is generated whenever TXE=1 in the UART_SR register
//Bit 6 TCIE: Transmission complete interrupt enable
//This bit is set and cleared by software.
#define TCIE (1u << 6)
//0: Interrupt is inhibited
//1: A UART interrupt is generated whenever TC=1 in the UART_SR register
//Bit 5 RXNEIE: RXNE interrupt enable
//This bit is set and cleared by software.
#define RXNEIE (1u << 5)
//0: Interrupt is inhibited
//1: A UART interrupt is generated whenever ORE=1 or RXNE=1 in the UART_SR register
//Bit 4 IDLEIE: IDLE interrupt enable
//This bit is set and cleared by software.
#define IDLEIE (1u << 4)
//0: Interrupt is inhibited
//1: A UART interrupt is generated whenever IDLE=1 in the UART_SR register
//Bit 3 TE: Transmitter enable
//This bit enables the transmitter. It is set and cleared by software.
#define TE (1u << 3)
//0: Transmitter is disabled
//1: Transmitter is enabled
//Note: 1: During transmission, a “0” pulse on the TE bit (“0” followed by “1”) sends a preamble
//(idle line) after the current word, except in smartcard mode.
//2: When TE is set there is a 1 bit-time delay before the transmission starts.
//Bit 2 RE: Receiver enable
//This bit enables the receiver. It is set and cleared by software.
#define RE (1u << 2)
//0: Receiver is disabled
//1: Receiver is enabled and begins searching for a start bit
//Bit 1 RWU: Receiver wakeup
//This bit determines if the UART is in mute mode or not. It is set and cleared by software and
//can be cleared by hardware when a wakeup sequence is recognized.
#define RWU (1u << 1)
//0: Receiver in active mode
//1: Receiver in mute mode
//Note: 1: Before selecting Mute mode (by setting the RWU bit) the UART must first receive a
//data byte, otherwise it cannot function in Mute mode with wakeup by Idle line detection.
//2: In Address Mark Detection wakeup configuration (WAKE bit=1) the RWU bit cannot
//be modified by software while the RXNE bit is set.
//Bit 0 SBK: Send break
//This bit set is used to send break characters. It can be set and cleared by software. It should
//be set by software, and will be reset by hardware during the stop bit of break.
#define SBK (1u << 0)
//0: No break character is transmitted
//1: Break character will be transmitted
//27.6.5 Control register 2 (UART_CR2)

//Address offset: 0x10
//Reset value: 0x0000
//31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16
//Reserved
//15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
//Res.
//LINEN STOP[1:0] CLK
//EN CPOL CPHA LBCL Res. LBDIE LBDL Res. ADD[3:0]
//rw rw rw rw rw rw rw rw rw rw rw rw rw rw
//Bits 31:15 Reserved, forced by hardware to 0.
//Bit 14 LINEN: LIN mode enable
//This bit is set and cleared by software.
#define LINEN (1u << 14)
//0: LIN mode disabled
//1: LIN mode enabled
//The LIN mode enables the capability to send LIN Synch Breaks (13 low bits) using the SBK
//bit in the UART_CR1 register, and to detect LIN Sync breaks.
//Bits 13:12 STOP: STOP bits
//These bits are used for programming the stop bits.
#define  STOP_10 (0u << 12)
#define  STOP_05 (1u << 12)
#define  STOP_20 (2u << 12)
#define  STOP_15 (3u << 12)
//00: 1 Stop bit
//01: 0.5 Stop bit
//10: 2 Stop bits
//11: 1.5 Stop bit
//The 0.5 Stop bit and 1.5 Stop bit are not available for UART4 & UART5.
//Bit 11 CLKEN: Clock enable
//This bit allows the user to enable the CK pin.
#define CLKEN (1u << 11)
//0: CK pin disabled
//1: CK pin enabled
//This bit is not available for UART4 & UART5.
//Bit 10 CPOL: Clock polarity
//This bit allows the user to select the polarity of the clock output on the CK pin in synchronous
//mode. It works in conjunction with the CPHA bit to produce the desired clock/data
//relationship
#define CPOL (1u << 10)
//0: Steady low value on CK pin outside transmission window.
//1: Steady high value on CK pin outside transmission window.
//This bit is not available for UART4 & UART5.
//Bit 9 CPHA: Clock phase
//This bit allows the user to select the phase of the clock output on the CK pin in synchronous
//mode. It works in conjunction with the CPOL bit to produce the desired clock/data
//relationship (see figures 288 to 289)
#define CPHA (1u << 9)
//0: The first clock transition is the first data capture edge.
//1: The second clock transition is the first data capture edge.
//This bit is not available for UART4 & UART5.
//Universal synchronous asynchronous receiver transmitter (UART) RM0008
//794/1093 Doc ID 13902 Rev 13
//Note: These 3 bits (CPOL, CPHA, LBCL) should not be written while the transmitter is enabled.
//27.6.6 Control register 3 (UART_CR3)
//Address offset: 0x14
//Reset value: 0x0000
//Bit 8 LBCL: Last bit clock pulse
//This bit allows the user to select whether the clock pulse associated with the last data bit
//transmitted (MSB) has to be output on the CK pin in synchronous mode.
#define LBCL (1u << 8)
//0: The clock pulse of the last data bit is not output to the CK pin
//1: The clock pulse of the last data bit is output to the CK pin
//The last bit is the 8th or 9th data bit transmitted depending on the 8 or 9 bit format selected
//by the M bit in the UART_CR1 register.
//This bit is not available for UART4 & UART5.
//Bit 7 Reserved, forced by hardware to 0.
//Bit 6 LBDIE: LIN break detection interrupt enable
//Break interrupt mask (break detection using break delimiter).
#define LBDIE (1u << 6)
//0: Interrupt is inhibited
//1: An interrupt is generated whenever LBD=1 in the UART_SR register
//Bit 5 LBDL: lin break detection length
//This bit is for selection between 11 bit or 10 bit break detection.
#define LBDL (1u << 5)
//0: 10 bit break detection
//1: 11 bit break detection
//Bit 4 Reserved, forced by hardware to 0.
//Bits 3:0 ADD[3:0]: Address of the UART node
#define ADR_MASK 0xf
//This bit-field gives the address of the UART node.
//This is used in multiprocessor communication during mute mode, for wake up with address
//mark detection.
//31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16
//Reserved
//15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
//Reserved
//CTSIE CTSE RTSE DMAT DMAR SCEN NACK HDSEL IRLP IREN EIE
//rw rw rw rw rw rw rw rw rw rw rw
//Bits 31:11 Reserved, forced by hardware to 0.
//Bit 10 CTSIE: CTS interrupt enable
#define CTSIE (1u << 10)
//0: Interrupt is inhibited
//1: An interrupt is generated whenever CTS=1 in the UART_SR register
//This bit is not available for UART4 & UART5.
//Bit 9 CTSE: CTS enable
#define CTSE (1u << 9)
//0: CTS hardware flow control disabled
//1: CTS mode enabled, data is only transmitted when the nCTS input is asserted (tied to 0). If
//the nCTS input is deasserted while a data is being transmitted, then the transmission is
//completed before stopping. If a data is written into the data register while nCTS is asserted,
//the transmission is postponed until nCTS is asserted.
//This bit is not available for UART4 & UART5.
//RM0008 Universal synchronous asynchronous receiver transmitter (UART)
//Doc ID 13902 Rev 13 795/1093
//Bit 8 RTSE: RTS enable
#define RTSE (1u << 8)
//0: RTS hardware flow control disabled
//1: RTS interrupt enabled, data is only requested when there is space in the receive buffer.
//The transmission of data is expected to cease after the current character has been
//transmitted. The nRTS output is asserted (tied to 0) when a data can be received.
//This bit is not available for UART4 & UART5.
//Bit 7 DMAT: DMA enable transmitter
//This bit is set/reset by software
#define DMAT (1u << 7)
//1: DMA mode is enabled for transmission
//0: DMA mode is disabled for transmission
//This bit is not available for UART5.
//Bit 6 DMAR: DMA enable receiver
//This bit is set/reset by software
#define DMAR (1u <<6)
//1: DMA mode is enabled for reception
//0: DMA mode is disabled for reception
//This bit is not available for UART5.
//Bit 5 SCEN: Smartcard mode enable
//This bit is used for enabling Smartcard mode.
#define SCEN (1u << 5)
//0: Smartcard Mode disabled
//1: Smartcard Mode enabled
//This bit is not available for UART4 & UART5.
//Bit 4 NACK: Smartcard NACK enable
#define NACK (1u << 4)
//0: NACK transmission in case of parity error is disabled
//1: NACK transmission during parity error is enabled
//This bit is not available for UART4 & UART5.
//Bit 3 HDSEL: Half-duplex selection
//Selection of Single-wire Half-duplex mode
#define HDSEL (1u << 3)
//0: Half duplex mode is not selected
//1: Half duplex mode is selected
//Bit 2 IRLP: IrDA low-power
//This bit is used for selecting between normal and low-power IrDA modes
#define IRLP (1u << 2)
//0: Normal mode
//1: Low-power mode
//Bit 1 IREN: IrDA mode enable
//This bit is set and cleared by software.
#define IREN (1u << 1)
//0: IrDA disabled
//1: IrDA enabled
//Bit 0 EIE: Error interrupt enable
//Error Interrupt Enable Bit is required to enable interrupt generation in case of a framing error,
//overrun error or noise error (FE=1 or ORE=1 or NE=1 in the UART_SR register) in case of
//Multi Buffer Communication (DMAR=1 in the UART_CR3 register).
#define EIE (1u << 0)
//0: Interrupt is inhibited
//1: An interrupt is generated whenever DMAR=1 in the UART_CR3 register and FE=1 or
//ORE=1 or NE=1 in the UART_SR register.

//27.6.7 Guard time and prescaler register (UART_GTPR)
//Address offset: 0x18
//Reset value: 0x0000
//31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16
//Reserved
//15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
//GT[7:0] PSC[7:0]
//rw rw rw rw rw rw rw rw rw rw rw rw rw rw rw rw
//Bits 31:16 Reserved, forced by hardware to 0.
//Bits 15:8 GT[7:0]: Guard time value
//This bit-field gives the Guard time value in terms of number of baud clocks.
//This is used in Smartcard mode. The Transmission Complete flag is set after this guard time
//value.
//This bit is not available for UART4 & UART5.
//Bits 7:0 PSC[7:0]: Prescaler value
//– In IrDA Low-power mode:
//PSC[7:0] = IrDA Low-Power Baud Rate
//Used for programming the prescaler for dividing the system clock to achieve the low-power
//frequency:
//The source clock is divided by the value given in the register (8 significant bits):
//00000000: Reserved - do not program this value
//00000001: divides the source clock by 1
//00000010: divides the source clock by 2
//...
//– In normal IrDA mode: PSC must be set to 00000001.
//– In smartcard mode:
//PSC[4:0]: Prescaler value
//Used for programming the prescaler for dividing the system clock to provide the smartcard
//clock.
//The value given in the register (5 significant bits) is multiplied by 2 to give the division factor
//of the source clock frequency:
//00000: Reserved - do not program this value
//00001: divides the source clock by 2
//00010: divides the source clock by 4
//00011: divides the source clock by 6
//...
//Note: Bits [7:5] have no effect if Smartcard mode is used.
//This bit is not available for UART4 & UART5.

#define UART1_IRQn                  37
#define UART2_IRQn                  38
#define UART3_IRQn                  39
#define UART4_IRQn                  52
#define UART5_IRQn                  53

// combine the M and PCE bits to get the word/partity combinations
#define NO_PARITY_9     M
#define NO_PARITY_8     0u
#define PARITY_8        (M | PCE)
#define PARITY_7        PCE

#define RATE1(x) ((((PCLK1_FREQ * 2u) / x) + 1u) / 2u)
#define RATE2(x) ((((PCLK2_FREQ * 2u) / x) + 1u) / 2u)

#define UART_BUF_SZ 256u
CONST_ASSERT(UART_BUF_SZ >= (RX_MAXBUFLEN + 10)); //for 10 preambles
CONST_ASSERT((UART_BUF_SZ & (UART_BUF_SZ - 1u)) == 0u);
static volatile u8 uart_buf[2][UART_BUF_SZ] = {"", ""};


typedef struct uart_info_t
{
    UART_TypeDef   *adr;        // physical device address
    IRQn_Type       irq;        // IRQ number for physical device
    DMA_Chan        txDMA,      // dma channel for transmit
                    rxDMA;      // dma channel for receive
    volatile u8     *buffer;     // address of circular buffer
    u8 (*timehanlder)(u8_least hart_channel); //what to do every 5 ms. NB: This is an afterthought; should belong to HART config, not UART config.
    u16 rate; //bps
    u16 parityEtc; //comm options
    u8 hart_channel;
    taskid_t taskid;
} uart_info_t;

static u8 timehandler_dma(u8_least hart_channel);
static u8 timehandler_irq(u8_least hart_channel);
static u8 timehandler_crash(u8_least hart_channel);

static const uart_info_t uartInfo[] =
{
    [UART_1] =
    {
        .adr = UART1, //ISP UART
        .irq = UART1_IRQn,
        .txDMA = DMA_Chan_4,
        .rxDMA = DMA_Chan_5,
        .buffer = uart_buf[0],
        .timehanlder = timehandler_dma,
        .rate = RATE1(1200),
        .parityEtc = PARITY_8 | PS,
        .hart_channel = HART_ISP_CHANNEL,
        .taskid = TASKID_ISPCOMM,
    },
    [UART_2] =
    {
        .adr = UART2, //IPC
        .irq = UART2_IRQn,
        .txDMA = DMA_Chan_7,
        .rxDMA = DMA_Chan_6,
        .buffer = uart_buf[1],
        .timehanlder = timehandler_dma,
        .rate = RATE2(38400),
        .parityEtc = PARITY_8 | PS,
        .hart_channel = HART_IPC_CHANNEL,
        .taskid = TASKID_IPCCOMM,
    },
    [UART_3] =
    {
        .adr = NULL, //UART3,
        .irq = UART3_IRQn,
        .txDMA = DMA_Chan_3,
        .rxDMA = DMA_Chan_2,
        .buffer = NULL,
        .timehanlder = timehandler_crash,
        .rate = RATE2(1200),
        .parityEtc = PARITY_8 | PS,
    },
    [UART_4] =
    {
        .adr = NULL, //UART4,
        .irq = UART4_IRQn,
        .txDMA = DMA_Chan_12,
        .rxDMA = DMA_Chan_10,
        .buffer = NULL,
        .timehanlder = timehandler_crash,
        .rate = RATE2(1200),
        .parityEtc = PARITY_8 | PS,
    },
    [UART_5] =
    {
        .adr = UART5, //Debug channel
        .irq = UART5_IRQn,
        .txDMA = DMA_Chan_1,
        .rxDMA = DMA_Chan_1,
        .buffer = NULL,
        .timehanlder = timehandler_irq,
        .rate = RATE2(1200),
        .parityEtc = PARITY_8 | PS,
        .hart_channel = HART_MAIN_CHANNEL,
        .taskid = TASKID_HART,
    }
};
//lint --flb

/** \brief Does channel-appropriate things for 5-ms processing (by delegating)
\param hart_channel - one of the valid HART channels
\return 0 iff burst on the channel is allowed, !=0 otherwise
*/
u8 HART_timehandler(u8_least hart_channel)
{
    return uartInfo[comm_channel_map[hart_channel]].timehanlder(hart_channel);
}

/** \brief Reverse comm channel map - from hart channels to UARTs.
Must match uartInfo
*/
const u8 comm_channel_map[HART_NUM_CHANNELS] =
{
    [HART_MAIN_CHANNEL] = UART_5,
    [HART_IPC_CHANNEL] = UART_2,
    [HART_ISP_CHANNEL] = UART_1,
};

/** \brief A helper to route UART requests to HART processing
\param uartNo - UART number
\param[out] pTaskId - a pointer to task id holder (may be NULL if not needed)
\param[out] pIsBlockDevice - a pointer to a holder of a flag indicating block (not a character)
access (may be NULL if not needed)
\return hart channel
*/
u8 uart_GetHartTaskAndChannel(u8 uartNo, taskid_t *pTaskId, bool_t *pIsBlockDevice)
{
    if(pTaskId != NULL)
    {
        *pTaskId = uartInfo[uartNo].taskid;
    }
    if(pIsBlockDevice != NULL)
    {
        *pIsBlockDevice = (uartInfo[uartNo].buffer != NULL);
    }
    return uartInfo[uartNo].hart_channel;
}

static u16 Atomic_ index[NELEM(uartInfo)];        // process index for DMA circular buffers, 1 per UART

/** \brief Send the first character of an outgoing HART frame

    \param ch - the 8-bit character to send (typically PREAMBLE)
*/
static void HART_Transmit(UART_TypeDef *uart, u8 ch)
{
    uart->DR = ch;
    uart->CR1  |= TXEIE;
}

#if 0 //Ernie-ized transmitter doesn't use states
/* states of the send statemachine */
#define SEND_PREAMBLE       0x10
#define PREAMBLES_SENT      0x11
#define DATA_SENT           0x12
#define LASTBYTE_SENT       0x16
#define TRANSMIT_IDLE       0x17
#endif //0

// Transmitter variables
static const u8     *ser_send_ptr[HART_NUM_CHANNELS];        // pointer to the sendbuffer
static u8_least           chr_cnt[HART_NUM_CHANNELS];              // number of bytes to send

/** \brief  Process a transmitter empty interrupt (doesn't send a dribble character)
    \param[in] uart - a pointer to the uart 0 hardware
*/
static void xmt_complete(UART_TypeDef *uart, const uart_info_t *info)
{
    u8 hart_channel = info->hart_channel;
    if(chr_cnt[hart_channel] == 0)
    {
        //nothing to send; disable Tx interrupt
        uart->CR1 &= ~TXEIE;            // if message complete, disable TX interrupts
        //notify end of transmission
        hart_xmit_ok(info->taskid);
    }
    else
    {
        uart->DR  = *ser_send_ptr[hart_channel]++;    /* Character to the sendbuffer */
        --chr_cnt[hart_channel];
    }
}



static const u8 uart2hart[] =           // UART to HART receive error translation table
{
    [0]                = 0U,
    [FE]               = FRAMING,
    [PE]               = PARITY_ERR,
    [ORE]              = OVERRUN,
    [FE | PE]          = FRAMING | PARITY_ERR,
    [FE | ORE]         = FRAMING | OVERRUN,
    [PE | ORE]         = PARITY_ERR | OVERRUN,
    [FE | PE | ORE]    = FRAMING | PARITY_ERR | OVERRUN
};

/** Process a physical interrupt from the channel assigned to HART communications

*/
static void HART_Interrupt(u8 uartNo)
{
    UART_TypeDef *uart = uartInfo[uartNo].adr;
    u32 status = uart->SR;                // get the status register
    u32 com_err = status & (ORE | FE | PE); //AK: what about NE etc.?

    if (com_err != 0u)
    {
        if ((com_err & ORE) != 0)
        {
            //AK: Do we need anything for STM UART5?
            //uart->FCR = FCRFIFOENA | FCRRSTRX;         /* discard Rx FIFO content */
        }
        //And notify the state machine
        rcv_exception(uart2hart[com_err], uartInfo[uartNo].hart_channel);
    }

    if ((status & RXNE) != 0u)              // is there a receive character?
    {
        u8 ch = (u8)uart->DR;
        //AK: isU5 = true;
        rcv_char(ch, uartNo, uartInfo[uartNo].hart_channel);  // call serial.c to process the character
    }

    if (((uart->CR1 & TXEIE) != 0u) &&      // are we in transmit interrupt mode?
       ((status & TXE) == TXE))             // is the transmit holding register empty?
    {
#if 0
        if (!xmt_complete((u8*)&uart->DR))  // call serial.c to send a character
        {
            uart->CR1 &= ~TXEIE;            // if message complete, disable TX interrupts
        }
#else
        xmt_complete(uart, &uartInfo[uartNo]);         /* Sendstatemachine     */
#endif
    }
}

/** \brief Handle a physical interrupt fron UART 1
*/
MN_IRQ void UART1_IRQHandler(void)
{
    mn_assert(HARDWARE(const void *, UART1_IRQHandler), -TRAPID_FIQ); //lint !e611 suspicious cast
}
CONST_ASSERT(sizeof(const void *) == sizeof(&UART1_IRQHandler));

/** \brief Handle a physical interrupt fron UART 2
*/
MN_IRQ void UART2_IRQHandler(void)
{
    mn_assert(HARDWARE(const void *, UART2_IRQHandler), -TRAPID_FIQ); //lint !e611 suspicious cast
}

/** \brief Handle a physical interrupt fron UART 3
*/
MN_IRQ void UART3_IRQHandler(void)
{
    mn_assert(HARDWARE(const void *, UART3_IRQHandler), -TRAPID_FIQ); //lint !e611 suspicious cast
}

/** \brief Handle a physical interrupt fron UART 4
*/
MN_IRQ void UART4_IRQHandler(void)
{
    mn_assert(HARDWARE(const void *, UART4_IRQHandler), -TRAPID_FIQ); //lint !e611 suspicious cast
}

/** \brief Handle a physical interrupt fron UART 5
*/
MN_IRQ void Hart_ISR(void)
{
    HART_Interrupt(UART_5);
}

/** \brief expire gap timer
\param hart_channel - HART channel
\return !0 for NO burst request
*/
static u8 timehandler_irq(u8_least hart_channel)
{
    u8 *pgaptm = serial_GetHartGapTimerPtr(hart_channel);
    *pgaptm >>= 1U; //expire gap timer
    return *pgaptm;
}

static u8 timehandler_crash(u8_least hart_channel)
{
    //Must NEVER be here
    MN_RT_ASSERT(timehandler_crash == NULL); //lint !e506 intentional const boolean to force crash
    UNUSED_OK(hart_channel);
    return 1U;
}


/** \brief Check the DMA circular buffers for 'not empty'.  If there are unprocessed
    chars, refresh gap timer and wake the HART task.
\param hart_channel - HART channel
\return !0 for NO burst request
*/
static u8 timehandler_dma(u8_least hart_channel)
{
    u8 uartNo = comm_channel_map[hart_channel];

    MN_DBG_ASSERT(uartInfo[uartNo].buffer != NULL); //must be a UART with DMA receiver

    bool_t got_something = false;
    /* NOTE:
    Seeming code duplication below is not.
    On comm error, we want to discard ALL characters AFTER status read
    */
    if((uartInfo[uartNo].adr->SR & (ORE | FE | PE)) != 0) //AK: what about NE etc.?
    {
        index[uartNo] = (u16)(UART_BUF_SZ - DMA_getcount(uartInfo[uartNo].rxDMA));
    }
    else
    {
        u32 tmp = UART_BUF_SZ - DMA_getcount(uartInfo[uartNo].rxDMA);

        if (tmp != (u32)index[uartNo])
        {
            got_something = true;
        }
    }
    if(got_something)
    {
        //Got characters: reset the gap timer. It is OK even if we received a complete frame (but don't know it yet)
        serial_RearmGapTimer(hart_channel);
        //Post the task semaphore but only if it has been consumed.
        //More monitoring could be done there
        os_event_t *sem = oswrap_GetTaskSemaphorePtr(uartInfo[uartNo].taskid);
        if(oswrap_GetEventCount(*sem) == 0)
        {
            oswrap_IntrEnter();
            oswrap_PostTaskSemaphore(uartInfo[uartNo].taskid);
            oswrap_IntrExit();
        }
    }
    else
    {
        //expire the gap timer
        serial_ExpireGapTimer(hart_channel);
    }
    return 1;
}

/** \brief Process any outstanding characters in the DMA [circular] buffer
    assigned to the specified UART.
    \param uartNo - the UART number to look at
    \param prochar - a callback for processing each received character

\return true iff a message found
*/
bool_t uart_ProcessUART_DMA_Chars(u8 uartNo, void (*prochar)(u8 c, u8 uartNo, u8_least hart_channel))
{
    u32 x = index[uartNo];
    bool_t ret = false;
    for (;!ret;)
    {
        u32 tmp = UART_BUF_SZ - DMA_getcount(uartInfo[uartNo].rxDMA);  // number of unprocessed characters

        if (tmp == x)                                  // all done for now
        {
            index[uartNo] = (u16)x;
            break;
        }
        while (tmp != x)
        {
            prochar(uartInfo[uartNo].buffer[x++], uartNo, uartInfo[uartNo].hart_channel);       // process the character
            x &= (UART_BUF_SZ - 1u);                  // modulus buffer size
            if(hart_IsFrameReceived(uartInfo[uartNo].hart_channel))
            {
                hart_ClearFrameReceived(uartInfo[uartNo].hart_channel);
                //ignore any UART errors for now
                ret = true;
                index[uartNo] = (u16)x;
                break;
            }
        }
    }
    return ret;
}


/** \brief Initialize the specified UART according to the passed parameters

    \param uartNo - enumeration of the UART port (see stmcommon.h)
    \param rate - rate in bits-per-second
    \param parityEtc - bit mask to be or'd into CR1
    \param var - address of buffer
    \param sz - size of buffer in bytes
*/
void uart_init(u8_least uartNo)
{

    UART_TypeDef *uart = uartInfo[uartNo].adr;
    if(uart == NULL)
    {
        return;                         // prerequisites check
    }
    uart->BRR = uartInfo[uartNo].rate;

    if (uartInfo[uartNo].buffer != NULL)
    {
        DMA_SetUART(uartInfo[uartNo].txDMA, uartInfo[uartNo].rxDMA);
        DMA_ConfigUART(uartInfo[uartNo].buffer, UART_BUF_SZ, HARDWARE(u32, &uart->DR), uartInfo[uartNo].rxDMA);
        uart->CR1 = UE | TE | RE | uartInfo[uartNo].parityEtc;
        uart->CR2 = STOP_10;
        uart->CR3 = EIE | DMAR | DMAT;
        uart->GTPR = 0u;
    }
    else
    {
        uart->CR1 = UE | TE | RE | uartInfo[uartNo].parityEtc | RXNEIE;
        uart->CR2 = STOP_10;
        uart->CR3 = EIE;
        uart->GTPR = 0u;
        NVIC_EnableIRQ(uartInfo[uartNo].irq);
    }

}

/** \brief Setup the Debug UART for use in the FF APP CPU (compatibility)
*/
void uart_setup(void)
{
    uart_init(UART_5);
}

/** \brief  Send the packet to the specified UART using DMA or interrupts as configured
    Called from task context to initiate frame transmit, the rest of the frame
    will be sent in interrupt context or by DMA all by itself.

    \param uartNo - the uart number 0..4 (UART_1..UART_5)
    \param len - length of the transmission including preambles and checksum
    \param bufptr - point to send buffer including preambles and checksum
*/

void serial_SendFrame(u8 uartNo, u8_least len, const u8 *bufptr)
{
    if(uartNo >= NELEM(uartInfo))
    {
        return; //pre-requisites check
    }
    UART_TypeDef *uart = uartInfo[uartNo].adr;
    if(uart == NULL)
    {
        //Not configured
        return; //pre-requisites check
    }
    if(uartInfo[uartNo].buffer == NULL)
    {
        u8 hart_channel = uartInfo[uartNo].hart_channel;
        //Character interface
        MN_ENTER_CRITICAL();
            ser_send_ptr[hart_channel]   = bufptr+1;                        /* Initialize the send buffer */
            chr_cnt[hart_channel]        = len - 1;

            HART_Transmit(uartInfo[uartNo].adr, *bufptr);
        MN_EXIT_CRITICAL();
    }
    else
    {
        //DMA block interface
        DMA_ConfigUART(bufptr, len, HARDWARE(u32, &uart->DR), uartInfo[uartNo].txDMA);
    }
}

bool_t serial_CarrierDetect(u8_least hart_channel)
{
    /* AK:
    For this compatibility hack we always say "carrier is present".
    IOW, we are *always* ready to receive a charcter and
    *never* ready to send burst.
    That's because without modem, we don't have separate signalling
    to allow voluntary transmission.
    A way to repair it and make burst mode functional, if need be,
    is to have something like
    bool_t serial_CanTransmit(void);
    which would return true with no modem and !serial_CarrierDetect()
    with modem. A test in maintain_timers() (see hart.c) would be
    serial_CanTransmit() instead of !serial_CarrierDetect()
    */
    UNUSED_OK(hart_channel);
    return true;
}

// end of source
