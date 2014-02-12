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

FILE_NAME          hw_i2c.c



FUNCTIONAL_MODULE_DESCRIPTION

     I2C bus driver functions for M16C/63

=========================================================================== */
#include <keywords.h>
#define  MODULE_ID      (COMP_HW + MOD_I2C)

INCLUDES

#include <base.h>
// #include <hw_cfg.h>
#include <hw_if.h>
#include <osif.h>

#include "errcodes.h"
#include "fram.h"

#include <hw_i2c.h>
#include <stm32f10x.h>
#include <stm32f10x_i2c.h>
#include <stm32f10x_gpio.h>

// #include "mnwrap.h"
#include "mnassert.h"

LOCAL_DEFINES

LOCAL_TYPEDEFS

EXPORT_DATA

IMPORT_DATA

LOCAL_DATA

// Inhibit the message -- related to the STM definitions used in this file
// to refeerence H/W related registers.
/*lint -e923 */

// Suppressing message regarding the include after definitions -- for the selected block
/*lint -save -e961 */

// #include <da_near.h>                                /* DATA SEGMENT DEFINITION */
NO_INIT static u32      ucI2cWordAddress;
NO_INIT static u8      *pucI2cData;
NO_INIT static u32      uiI2cNbrOfBytes;
NO_INIT static u8       ucI2cRequestType;
NO_INIT static void          (* pvI2cCallback) (u32 numbytes);
#include <da_def.h>                                 /* DEFAULT DATA SEGMENT */

#include <hw_cfg.h> //for EE_WR_DISABLE

#ifdef EECLEAR
static u8   ee_dump[64];
#endif

#include <da_def.h>                                 /* DEFAULT DATA SEGMENT */
/*lint -restore */

FUNCTION_DECLARATIONS

//-------------------------------------------------------------------------------------------

#define     FRAM_I2C I2C2

#define     FRAM_SIZE    (64u * 1024u)

//lint ++flb

//Bit 15 SWRST: Software reset
//When set, the I2C is under reset state. Before resetting this bit, make sure the I2C lines are
//released and the bus is free.
#define SWRST (1u << 15)
//0: I2C Peripheral not under reset
//1: I2C Peripheral under reset state
//Note: This bit can be used in case the BUSY bit is set to ‘1 when no stop condition has been
//detected on the bus.
//Bit 14 Reserved, forced by hardware to 0.
//Bit 13 ALERT: SMBus alert
//This bit is set and cleared by software, and cleared by hardware when PE=0.
#define ALERT (1u << 13)
//0: Releases SMBA pin high. Alert Response Address Header followed by NACK.
//1: Drives SMBA pin low. Alert Response Address Header followed by ACK.
//Bit 12 PEC: Packet error checking
//This bit is set and cleared by software, and cleared by hardware when PEC is transferred or
//by a START or Stop condition or when PE=0.
#define PEC (1u << 12)
//0: No PEC transfer
//1: PEC transfer (in Tx or Rx mode)
//Note: PEC calculation is corrupted by an arbitration loss.
//RM0008 Inter-integrated circuit (I2C) interface
//Doc ID 13902 Rev 13 743/1093
//Bit 11 POS: Acknowledge/PEC Position (for data reception)
//This bit is set and cleared by software and cleared by hardware when PE=0.
#define POS_E (1u << 11)

//0: ACK bit controls the (N)ACK of the current byte being received in the shift register. The
//PEC bit indicates that current byte in shift register is a PEC.
//1: ACK bit controls the (N)ACK of the next byte which will be received in the shift register.
//The PEC bit indicates that the next byte in the shift register is a PEC
//Note: The POS bit is used when the procedure for reception of 2 bytes (see Method 2:
//transfer sequence diagram for master receiver when N=2) is followed. It must be
//configured before data reception starts. In this case, to NACK the 2nd byte, the ACK bit
//must be cleared just after ADDR is cleared. To check the 2nd byte as PEC, the PEC bit
//must be set during the ADDR stretch event after configuring the POS bit.
//Bit 10 ACK: Acknowledge enable
//This bit is set and cleared by software and cleared by hardware when PE=0.
#define ACK (1u << 10)
//0: No acknowledge returned
//1: Acknowledge returned after a byte is received (matched address or data)
//Bit 9 STOP: Stop generation
//The bit is set and cleared by software, cleared by hardware when a Stop condition is
//detected, set by hardware when a timeout error is detected.
//In Master Mode:
#define STOP (1u << 9)
//0: No Stop generation.
//1: Stop generation after the current byte transfer or after the current Start condition is sent.
//In Slave mode:
//0: No Stop generation.
//1: Release the SCL and SDA lines after the current byte transfer.
//Note: When the STOP, START or PEC bit is set, the software must not perform any write
//access to I2C_CR1 before this bit is cleared by hardware. Otherwise there is a risk of
//setting a second STOP, START or PEC request.
//Bit 8 START: Start generation
//This bit is set and cleared by software and cleared by hardware when start is sent or PE=0.
//In Master Mode:
#define START (1u << 8)
//0: No Start generation
//1: Repeated start generation
//In Slave mode:
//0: No Start generation
//1: Start generation when the bus is free
//Bit 7 NOSTRETCH: Clock stretching disable (Slave mode)
//This bit is used to disable clock stretching in slave mode when ADDR or BTF flag is set, until
//it is reset by software.
#define NOSTRETCH (1u << 7)
//0: Clock stretching enabled
//1: Clock stretching disabled
//Bit 6 ENGC: General call enable
#define ENGC (1u << 6)
//0: General call disabled. Address 00h is NACKed.
//1: General call enabled. Address 00h is ACKed.
//Bit 5 ENPEC: PEC enable
#define ENPEC (1u << 5)
//0: PEC calculation disabled
//1: PEC calculation enabled
//Inter-integrated circuit (I2C) interface RM0008
//744/1093 Doc ID 13902 Rev 13
//26.6.2 I2C Control register 2 (I2C_CR2)
//Address offset: 0x04
//Reset value: 0x0000
//Bit 4 ENARP: ARP enable
#define ENARP (1u << 4)
//0: ARP disable
//1: ARP enable
//SMBus Device default address recognized if SMBTYPE=0
//SMBus Host address recognized if SMBTYPE=1
//Bit 3 SMBTYPE: SMBus type
#define SMBTYPE (1u << 3)
//0: SMBus Device
//1: SMBus Host
//Bit 2 Reserved, forced by hardware to 0.
//Bit 1 SMBUS: SMBus mode
#define SMBUS (1u << 1)
//0: I2C mode
//1: SMBus mode
//Bit 0 PE: Peripheral enable
#define PE (1u << 0)
//0: Peripheral disable
//1: Peripheral enable: the corresponding IOs are selected as alternate functions depending
//on SMBus bit.
//Note: If this bit is reset while a communication is on going, the peripheral is disabled at the
//end of the current communication, when back to IDLE state.
//All bit resets due to PE=0 occur at the end of the communication.
//In master mode, this bit must not be reset before the end of the communication.
//15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
//Reserved
//LAST DMA
//EN
//ITBUF
//EN
//ITEVT
//EN
//ITERR
//EN Reserved
//FREQ[5:0]
//rw rw rw rw rw rw rw rw rw rw rw
//Bits 15:13 Reserved, forced by hardware to 0.
//Bit 12 LAST: DMA last transfer
#define LAST (1u << 12)
//0: Next DMA EOT is not the last transfer
//1: Next DMA EOT is the last transfer
//Note: This bit is used in master receiver mode to permit the generation of a NACK on the last
//received data.
//Bit 11 DMAEN: DMA requests enable
#define DMAEN (1u << 11)
//0: DMA requests disabled
//1: DMA request enabled when TxE=1 or RxNE =1
//Bit 10 ITBUFEN: Buffer interrupt enable
#define ITBUFEN (1u << 10)
//0: TxE = 1 or RxNE = 1 does not generate any interrupt.
//1:TxE = 1 or RxNE = 1 generates Event Interrupt (whatever the state of DMAEN)
//RM0008 Inter-integrated circuit (I2C) interface
//Doc ID 13902 Rev 13 745/1093
//Bit 9 ITEVTEN: Event interrupt enable
#define ITEVTEN (1u << 9)
//0: Event interrupt disabled
//1: Event interrupt enabled
//This interrupt is generated when:
//–SB = 1 (Master)
//–ADDR = 1 (Master/Slave)
//–ADD10= 1 (Master)
//–STOPF = 1 (Slave)
//–BTF = 1 with no TxE or RxNE event
//–TxE event to 1 if ITBUFEN = 1
//–RxNE event to 1if ITBUFEN = 1
//Bit 8 ITERREN: Error interrupt enable
#define ITERREN (1u << 8)
//0: Error interrupt disabled
//1: Error interrupt enabled
//This interrupt is generated when:
//– BERR = 1
//– ARLO = 1
//– AF = 1
//– OVR = 1
//– PECERR = 1
//– TIMEOUT = 1
//– SMBALERT = 1
//Bits 7:6 Reserved, forced by hardware to 0.
#define MHZ_2 0x2u   // == 2
#define MHZ_4 0x4u   // == 4
#define MHZ_36 0x24u // == 36

//Bits 5:0 FREQ[5:0]: Peripheral clock frequency
//The peripheral clock frequency must be configured using the input APB clock frequency (I2C
//peripheral connected to APB). The minimum allowed frequency is 2 MHz, the maximum
//frequency is limited by the maximum APB frequency (36 MHz) and an intrinsic limitation of
//46 MHz.
//0b000000: Not allowed
//0b000001: Not allowed
//0b000010: 2 MHz
//...
//0b100100: 36 MHz
//Higher than 0b100100: Not allowed
//Inter-integrated circuit (I2C) interface RM0008
//746/1093 Doc ID 13902 Rev 13
//26.6.3 I2C Own address register 1 (I2C_OAR1)
//Address offset: 0x08
//Reset value: 0x0000
//26.6.4 I2C Own address register 2 (I2C_OAR2)
//Address offset: 0x0C
//Reset value: 0x0000
//15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
//ADD
//MODE Reserved
//ADD[9:8] ADD[7:1] ADD0
//rw rw rw rw rw rw rw rw rw rw rw
//Bit 15 ADDMODE Addressing mode (slave mode)
#define ADDMODE (1u << 15)
//0: 7-bit slave address (10-bit address not acknowledged)
//1: 10-bit slave address (7-bit address not acknowledged)
//Bit 14 Should always be kept at 1 by software.
#define ALWAYS1  (1u << 14)
//Bits 13:10 Reserved, forced by hardware to 0.
//Bits 9:8 ADD[9:8]: Interface address
// #define  (1u << x)
//7-bit addressing mode: don’t care
//10-bit addressing mode: bits9:8 of address
//Bits 7:1 ADD[7:1]: Interface address
// #define  (1u << x)
//bits 7:1 of address
//Bit 0 ADD0: Interface address
//7-bit addressing mode: don’t care
// #define  (1u << x)
//10-bit addressing mode: bit 0 of address
//15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
//Reserved
//ADD2[7:1] ENDUAL
//rw rw rw rw rw rw rw rw
//Bits 15:8 Reserved, forced by hardware to 0.
//Bits 7:1 ADD2[7:1]: Interface address
// #define  (1u << x)
//bits 7:1 of address in dual addressing mode
//Bit 0 ENDUAL: Dual addressing mode enable
#define ENDUAL (1u << 0)
//0: Only OAR1 is recognized in 7-bit addressing mode
//1: Both OAR1 and OAR2 are recognized in 7-bit addressing mode
//RM0008 Inter-integrated circuit (I2C) interface
//Doc ID 13902 Rev 13 747/1093
//26.6.5 I2C Data register (I2C_DR)
//Address offset: 0x10
//Reset value: 0x0000
//26.6.6 I2C Status register 1 (I2C_SR1)
//Address offset: 0x14
//Reset value: 0x0000
//15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
//Reserved
//DR[7:0]
//rw rw rw rw rw rw rw rw
//Bits 15:8 Reserved, forced by hardware to 0.
//Bits 7:0 DR[7:0] 8-bit data register
//Byte received or to be transmitted to the bus.
//–Transmitter mode: Byte transmission starts automatically when a byte is written in the DR
//register. A continuous transmit stream can be maintained if the next data to be transmitted
//is put in DR once the transmission is started (TxE=1)
//–Receiver mode: Received byte is copied into DR (RxNE=1). A continuous transmit stream
//can be maintained if DR is read before the next data byte is received (RxNE=1).
//Note: In slave mode, the address is not copied into DR.
//Note: Write collision is not managed (DR can be written if TxE=0).
//Note: If an ARLO event occurs on ACK pulse, the received byte is not copied into DR and so
//cannot be read.
//15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
//SMB
//ALERT
//TIME
//OUT Res.
//PEC
//ERR OVR AF ARLO BERR TxE RxNE
//Res.
//STOPF ADD10 BTF ADDR SB
//rc_w0 rc_w0 rc_w0 rc_w0 rc_w0 rc_w0 rc_w0 r r r r r r r
//Bit 15 SMBALERT: SMBus alert
//In SMBus host mode:
#define SMBALERT (1u << 15)
//0: no SMBALERT
//1: SMBALERT event occurred on pin
//In SMBus slave mode:
//0: no SMBALERT response address header
//1: SMBALERT response address header to SMBALERT LOW received
//– Cleared by software writing 0, or by hardware when PE=0.
//Bit 14 TIMEOUT: Timeout or Tlow error
#define TIMEOUT (1u << 14)
//0: No timeout error
//1: SCL remained LOW for 25 ms (Timeout)
//or
//Master cumulative clock low extend time more than 10 ms (Tlow:mext)
//or
//Slave cumulative clock low extend time more than 25 ms (Tlow:sext)
//– When set in slave mode: slave resets the communication and lines are released by hardware
//– When set in master mode: Stop condition sent by hardware
//– Cleared by software writing 0, or by hardware when PE=0.
//Note: This functionality is available only in SMBus mode.
//Inter-integrated circuit (I2C) interface RM0008
//748/1093 Doc ID 13902 Rev 13
//Bit 13 Reserved, forced by hardware to 0.
//Bit 12 PECERR: PEC Error in reception
#define PECERR (1u << 12)
//0: no PEC error: receiver returns ACK after PEC reception (if ACK=1)
//1: PEC error: receiver returns NACK after PEC reception (whatever ACK)
//–Cleared by software writing 0, or by hardware when PE=0.
//–
//Bit 11 OVR: Overrun/Underrun
#define OVR (1u << 11)
//0: No overrun/underrun
//1: Overrun or underrun
//–Set by hardware in slave mode when NOSTRETCH=1 and:
//–In reception when a new byte is received (including ACK pulse) and the DR register has not
//been read yet. New received byte is lost.
//–In transmission when a new byte should be sent and the DR register has not been written
//yet. The same byte is sent twice.
//–Cleared by software writing 0, or by hardware when PE=0.
//Note: If the DR write occurs very close to SCL rising edge, the sent data is unspecified and a
//hold timing error occurs
//Bit 10 AF: Acknowledge failure
#define AF (1u << 10)
//0: No acknowledge failure
//1: Acknowledge failure
//–Set by hardware when no acknowledge is returned.
//–Cleared by software writing 0, or by hardware when PE=0.
//Bit 9 ARLO: Arbitration lost (master mode)
#define ARLO (1u << 9)
//0: No Arbitration Lost detected
//1: Arbitration Lost detected
//Set by hardware when the interface loses the arbitration of the bus to another master
//–Cleared by software writing 0, or by hardware when PE=0.
//After an ARLO event the interface switches back automatically to Slave mode (M/SL=0).
//Note: In SMBUS, the arbitration on the data in slave mode occurs only during the data phase,
//or the acknowledge transmission (not on the address acknowledge).
//Bit 8 BERR: Bus error
#define BERR (1u << 8)
//0: No misplaced Start or Stop condition
//1: Misplaced Start or Stop condition
//–Set by hardware when the interface detects an SDA rising or falling edge while SCL is high,
//occurring in a non-valid position during a byte transfer.
//–Cleared by software writing 0, or by hardware when PE=0.
//Bit 7 TxE: Data register empty (transmitters)
#define TxE (1u << 7)
//0: Data register not empty
//1: Data register empty
//–Set when DR is empty in transmission. TxE is not set during address phase.
//–Cleared by software writing to the DR register or by hardware after a start or a stop
//condition or when PE=0.
//TxE is not set if either a NACK is received, or if next byte to be transmitted is PEC (PEC=1)
//Note: TxE is not cleared by writing the first data being transmitted, or by writing data when
//BTF is set, as in both cases the data register is still empty.
//RM0008 Inter-integrated circuit (I2C) interface
//Doc ID 13902 Rev 13 749/1093
//Bit 6 RxNE: Data register not empty (receivers)
#define RxNE (1u << 6)
//0: Data register empty
//1: Data register not empty
//–Set when data register is not empty in receiver mode. RxNE is not set during address
//phase.
//–Cleared by software reading or writing the DR register or by hardware when PE=0.
//RxNE is not set in case of ARLO event.
//Note: RxNE is not cleared by reading data when BTF is set, as the data register is still full.
//Bit 5 Reserved, forced by hardware to 0.
//Bit 4 STOPF: Stop detection (slave mode)
#define STOPF (1u << 4)
//0: No Stop condition detected
//1: Stop condition detected
//–Set by hardware when a Stop condition is detected on the bus by the slave after an
//acknowledge (if ACK=1).
//–Cleared by software reading the SR1 register followed by a write in the CR1 register, or by
//hardware when PE=0
//Note: The STOPF bit is not set after a NACK reception.
//It is recommended to perform the complete clearing sequence (READ SR1 then
//WRITE CR1) after the STOPF is set. Refer to Figure 270: Transfer sequence diagram
//for slave receiver on page 726.
//Bit 3 ADD10: 10-bit header sent (Master mode)
#define ADD10 (1u << 3)
//0: No ADD10 event occurred.
//1: Master has sent first address byte (header).
//–Set by hardware when the master has sent the first byte in 10-bit address mode.
//–Cleared by software reading the SR1 register followed by a write in the DR register of the
//second address byte, or by hardware when PE=0.
//Note: ADD10 bit is not set after a NACK reception
//Bit 2 BTF: Byte transfer finished
#define BTF (1u << 2)
//0: Data byte transfer not done
//1: Data byte transfer succeeded
//–Set by hardware when NOSTRETCH=0 and:
//–In reception when a new byte is received (including ACK pulse) and DR has not been
//read yet (RxNE=1).
//–In transmission when a new byte should be sent and DR has not been written yet
//(TxE=1).
//–Cleared by software reading SR1 followed by either a read or write in the DR register or
//by hardware after a start or a stop condition in transmission or when PE=0.
//Note: The BTF bit is not set after a NACK reception
//The BTF bit is not set if next byte to be transmitted is the PEC (TRA=1 in I2C_SR2
//register and PEC=1 in I2C_CR1 register)
//Inter-integrated circuit (I2C) interface RM0008
//750/1093 Doc ID 13902 Rev 13
//Bit 1 ADDR: Address sent (master mode)/matched (slave mode)
//This bit is cleared by software reading SR1 register followed reading SR2, or by hardware
//when PE=0.
//Address matched (Slave)
#define ADDR (1u << 1)
//0: Address mismatched or not received.
//1: Received address matched.
//–Set by hardware as soon as the received slave address matched with the OAR registers
//content or a general call or a SMBus Device Default Address or SMBus Host or SMBus
//Alert is recognized. (when enabled depending on configuration).
//Note: In slave mode, it is recommended to perform the complete clearing sequence (READ
//SR1 then READ SR2) after ADDR is set. Refer to Figure 270: Transfer sequence
//diagram for slave receiver on page 726.
//Address sent (Master)
//0: No end of address transmission
//1: End of address transmission
//–For 10-bit addressing, the bit is set after the ACK of the 2nd byte.
//–For 7-bit addressing, the bit is set after the ACK of the byte.
//Note: ADDR is not set after a NACK reception
//Bit 0 SB: Start bit (Master mode)
#define SB (1u << 0)
//0: No Start condition
//1: Start condition generated.
//–Set when a Start condition generated.
//–Cleared by software by reading the SR1 register followed by writing the DR register, or by
//hardware when PE=0
//RM0008 Inter-integrated circuit (I2C) interface
//Doc ID 13902 Rev 13 751/1093
//26.6.7 I2C Status register 2 (I2C_SR2)
//Address offset: 0x18
//Reset value: 0x0000
//Note: Reading I2C_SR2 after reading I2C_SR1 clears the ADDR flag, even if the ADDR flag was
//set after reading I2C_SR1. Consequently, I2C_SR2 must be read only when ADDR is found
//set in I2C_SR1 or when the STOPF bit is cleared.
//15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
//PEC[7:0] DUALF SMB
//HOST
//SMBDE
//FAULT
//GEN
//CALL Res.
//TRA BUSY MSL
//r r r r r r r r r r r r r r r
//Bits 15:8 PEC[7:0] Packet error checking register
//This register contains the internal PEC when ENPEC=1.
//Bit 7 DUALF: Dual flag (Slave mode)
#define DUALF (1u << 7)
//0: Received address matched with OAR1
//1: Received address matched with OAR2
//–Cleared by hardware after a Stop condition or repeated Start condition, or when PE=0.
//Bit 6 SMBHOST: SMBus host header (Slave mode)
#define SMBHOST (1u << 6)
//0: No SMBus Host address
//1: SMBus Host address received when SMBTYPE=1 and ENARP=1.
//–Cleared by hardware after a Stop condition or repeated Start condition, or when PE=0.
//Bit 5 SMBDEFAULT: SMBus device default address (Slave mode)
#define SMBDEFAULT (1u << 5)
//0: No SMBus Device Default address
//1: SMBus Device Default address received when ENARP=1
//–Cleared by hardware after a Stop condition or repeated Start condition, or when PE=0.
//Bit 4 GENCALL: General call address (Slave mode)
#define GENCALL (1u << 4)                                                                                     7
//0: No General Call
//1: General Call Address received when ENGC=1
//–Cleared by hardware after a Stop condition or repeated Start condition, or when PE=0.
//Bit 3 Reserved, forced by hardware to 0.
//Bit 2 TRA: Transmitter/receiver
#define TRA (1u << 2)
//0: Data bytes received
//1: Data bytes transmitted
//This bit is set depending on the R/W bit of the address byte, at the end of total address
//phase.
//It is also cleared by hardware after detection of Stop condition (STOPF=1), repeated Start
//condition, loss of bus arbitration (ARLO=1), or when PE=0.
//Bit 1 BUSY: Bus busy
#define BUSY (1u << 1)
//0: No communication on the bus
//1: Communication ongoing on the bus
//–Set by hardware on detection of SDA or SCL low
//–cleared by hardware on detection of a Stop condition.
//It indicates a communication in progress on the bus. This information is still updated when
//the interface is disabled (PE=0).
//Inter-integrated circuit (I2C) interface RM0008
//752/1093 Doc ID 13902 Rev 13
//26.6.8 I2C Clock control register (I2C_CCR)
//Address offset: 0x1C
//Reset value: 0x0000
//Note: 1 To use the I2C at 400 KHz (in fast mode), the PCLK1 frequency (I2C peripheral input clock)
//must be a multiple of 10 MHz.
//2 The CCR register must be configured only when the I2C is disabled (PE = 0).
//Bit 0 MSL: Master/slave
#define MSL (1u << 0)
//0: Slave Mode
//1: Master Mode
//–Set by hardware as soon as the interface is in Master mode (SB=1).
//–Cleared by hardware after detecting a Stop condition on the bus or a loss of arbitration
//(ARLO=1), or by hardware when PE=0.
//15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
//F/S DUTY
//Reserved
//CCR[11:0]
//rw rw rw rw rw rw rw rw rw rw rw rw rw rw
//Bit 15 F/S: I2C master mode selection
#define F_S (1u << 15)
//0: Standard Mode I2C
//1: Fast Mode I2C
//Bit 14 DUTY: Fast mode duty cycle
#define DUTY (1u << 14)
//0: Fast Mode tlow/thigh = 2
//1: Fast Mode tlow/thigh = 16/9 (see CCR)
//Bits 13:12 Reserved, forced by hardware to 0.
//Bits 11:0 CCR[11:0]: Clock control register in Fast/Standard mode (Master mode)
// #define  (1u << x)
//Controls the SCL clock in master mode.
//Standard mode or SMBus:
//Thigh = CCR * TPCLK1
//Tlow = CCR * TPCLK1
//Fast mode:
//If DUTY = 0:
//Thigh = CCR * TPCLK1
//Tlow = 2 * CCR * TPCLK1
//If DUTY = 1: (to reach 400 kHz)
//Thigh = 9 * CCR * TPCLK1
//Tlow = 16 * CCR * TPCLK1
//For instance: in standard mode, to generate a 100 kHz SCL frequency:
//If FREQR = 08, TPCLK1 = 125 ns so CCR must be programmed with 0x28
//(0x28 <=> 40d x 125 ns = 5000 ns.)
//Note: 1. The minimum allowed value is 0x04, except in FAST DUTY mode where the
//minimum allowed value is 0x01
//2. thigh = tr(SCL) + tw(SCLH). See device datasheet for the definitions of parameters
//3. tlow = tf(SCL) + tw(SCLL). See device datasheet for the definitions of parameters
//4. These timings are without filters.
//5. The CCR register must be configured only when the I2C is disabled (PE = 0).
//RM0008 Inter-integrated circuit (I2C) interface
//Doc ID 13902 Rev 13 753/1093
//26.6.9 I2C TRISE register (I2C_TRISE)
//Address offset: 0x20
//Reset value: 0x0002
//15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
//Reserved
//TRISE[5:0]
// #define  (1u << x)
//rw rw rw rw rw rw
//Bits 15:6 Reserved, forced by hardware to 0.
//Bits 5:0 TRISE[5:0]: Maximum rise time in Fast/Standard mode (Master mode)
//These bits must be programmed with the maximum SCL rise time given in the I2C bus
//specification, incremented by 1.
//For instance: in standard mode, the maximum allowed SCL rise time is 1000 ns.
//If, in the I2C_CR2 register, the value of FREQ[5:0] bits is equal to 0x08 and TPCLK1 = 125 ns
//therefore the TRISE[5:0] bits must be programmed with 09h.
//(1000 ns / 125 ns = 8 + 1)
//The filter value can also be added to TRISE[5:0].
//If the result is not an integer, TRISE[5:0] must be programmed with the integer part, in order
//to respect the tHIGH parameter.
//Note: TRISE[5:0] must be configured only when the I2C is disabled (PE = 0).

#define CORE_FREQ           (4000000u)
#define PCLK1_FREQ          (CORE_FREQ)

#define RESET_DURATION      (CORE_FREQ / 100000u)      // duration
//lint --flb

#define I2C_PRESCALE        20           // PCLK1 / 2 / I2C_PRESCALE == 100000
// #define I2C_PRESCALE        22           // PCLK1 / 2 / I2C_PRESCALE == 100000
#define TRISE_VAL           ((PCLK1_FREQ / 1000000u) + 1u)

#define I2C_START_RETRIES           (2u)

// Note -- there are NO stop retries
#define I2C_STOP_CYCLES             (100u)
#define I2C_START_CYCLES            (32u)

#define NV_ADDRESS      ((u8)0xA0)
#define IWRITE          ((u8)0)
#define IREAD           ((u8)1)

//---------------------------------------------------------------------
// For debugging purposes:
// Re-init counter...
static u32      ReInitCounter                   = 0u;

// What error to count
#define     I2C_STOP_ERROR                      (0u)
#define     I2C_START_ERROR                     (1u)
#define     I2C_STOP_RETRY                      (2u)
#define     I2C_START_RETRY                     (3u)
#define     I2C_RESTART_ERROR                   (4u)

// Diagnostic counters
static u32      ErrorCounter_StartRead          = 0u;
static u32      ErrorCounter_StartWrite         = 0u;
static u32      ErrorCounter_StopRead           = 0u;
static u32      ErrorCounter_StopWrite          = 0u;

static u32      ErrorCounter_StopReadRetry      = 0u;
static u32      ErrorCounter_StartReadRetry     = 0u;
static u32      ErrorCounter_StopWriteRetry     = 0u;
static u32      ErrorCounter_StartWriteRetry    = 0u;

static u32      ErrorCounter_Restart            = 0u;

// Indicator of FRAM operation
static bool_t   FRAM_ReadOperation              = false;

// FRAM Exposed Diag

static void I2C_WriteDiagnostic(void)
{
    FRAM_ReadOperation = false;
}

static void I2C_ReadDiagnostic(void)
{
    FRAM_ReadOperation = true;
}

// Handle error counters
static void I2C_ErrorCounterHandler(u32    WhatError)
{
    switch (WhatError)
    {
        case I2C_STOP_ERROR :
            if (FRAM_ReadOperation)
            {
                ErrorCounter_StopRead++;
            }
            else
            {
                ErrorCounter_StopWrite++;
            }
            break;

        case I2C_STOP_RETRY :
            if (FRAM_ReadOperation)
            {
                ErrorCounter_StopReadRetry++;
            }
            else
            {
                ErrorCounter_StopWriteRetry++;
            }
            break;

        case I2C_START_ERROR :
            if (FRAM_ReadOperation)
            {
                ErrorCounter_StartRead++;
            }
            else
            {
                ErrorCounter_StartWrite++;
            }
            break;

        case I2C_START_RETRY :
            if (FRAM_ReadOperation)
            {
                ErrorCounter_StartReadRetry++;
            }
            else
            {
                ErrorCounter_StartWriteRetry++;
            }
            break;

        case I2C_RESTART_ERROR :
            ErrorCounter_Restart++;
            break;

        default :
            break;
    }

    UNUSED_OK(ErrorCounter_StartRead);
    UNUSED_OK(ErrorCounter_StartWrite);
    UNUSED_OK(ErrorCounter_StopRead);
    UNUSED_OK(ErrorCounter_StopWrite);
    UNUSED_OK(ErrorCounter_StopReadRetry);
    UNUSED_OK(ErrorCounter_StartReadRetry);
    UNUSED_OK(ErrorCounter_StopWriteRetry);
    UNUSED_OK(ErrorCounter_StartWriteRetry);
    UNUSED_OK(ErrorCounter_Restart);
}

/** \brief initialize the I2C controller
    \return Errorcode, indicating the success or failure of teh init actions
*/
static ErrorCode_t i2c_Initialize(void)
{
    ErrorCode_t   Result = ERR_OK;
    volatile u32  rdur = RESET_DURATION;
    volatile u8   bitbangs = 13u;

    // reset is now done in stm_rcc.c - but
    //  it doesn;t always work on FF CPU

    FRAM_I2C->CR1     = SWRST;               // Reset I2C peripheral
    while (rdur-- != 0u)
    {
        //  just counting
    }
    FRAM_I2C->CR1     = 0u;

    // Configure I2C peripheral
    FRAM_I2C->CR2     = MHZ_4;          // 4 mhz
    FRAM_I2C->TRISE   = TRISE_VAL;      // 1000 ns
    FRAM_I2C->CCR     = I2C_PRESCALE;   // low/high - 2 mhz / 16 = 125 khz
    FRAM_I2C->CR1     = ACK | PE;

/* if the SDA line for the I@C controller is stuck low, issue som clocks
    Not sure its relevant if this routine gets stuck.  The I2C bus is stuck
    so we'll not get far/
  */

    while (((GPIOB->IDR & (1u << 11)) == 0) && (bitbangs != 0u))        //lint !e960 Wait
    // test PB11 (SDA)
    {
        GPIOB->CRH &= ~(0x8u << 8);                 // put PB10 in GPIO mode
        GPIOB->CRH |=  (0x8u << 8);                 // put PB10 in Alt mode (I2C)
        bitbangs--;
    }

    // Now check the results
    if ((GPIOB->IDR & (1u << 11)) == 0)
    {   // Not OK
        Result = ERR_I2C_BUS_STUCK;
    }
    else
    {   /* enable I2C2 peripheral */
        Result = ERR_OK;
    }

    return Result;
}

// Wrapper for the I2C init -- to be invoked from withing the I2C code.
static ErrorCode_t i2c_InitializeFromOperation(void)
{
    ErrorCode_t     Result;

    ReInitCounter++;                // Increment Re-init counter
    Result = i2c_Initialize();      // Do the init

    UNUSED_OK(ReInitCounter);

    return  Result;                 // Return the result
}

/** \brief release the I2C controller if it is BUSY
*/
static ErrorCode_t I2C_Stop (void)
{
    ErrorCode_t     Result  = ERR_OK;
    ErrorCode_t     ResInit;
    volatile u32    LoopCounts;                     // Retries

    if ((FRAM_I2C->SR2 & BUSY) == BUSY)             // is it BUSY?
    {   // Yes -- do the STOP
        FRAM_I2C->CR1 |= STOP;                      // Kick in STOP condition
        LoopCounts = I2C_STOP_CYCLES;
        while (((FRAM_I2C->CR1 & STOP) == STOP) && (LoopCounts != 0u))     //lint !e960 Wait until BUSY bit reset and retries are not exausted
        {
            LoopCounts--;
        }

        if ((FRAM_I2C->CR1 & STOP) == STOP)         // is it still BUSY?
        {   // Yeah, error.
            Result = ERR_I2C_BUS_STUCK;
            I2C_ErrorCounterHandler(I2C_STOP_ERROR);

            // Looks like we failed. Try to re-init the I2C
            ResInit = i2c_InitializeFromOperation();

            if (ResInit == ERR_OK)
            {   // we did succeed...
                Result = ResInit;
            }
            else
            {   // No success -- error on retry...
                I2C_ErrorCounterHandler(I2C_STOP_RETRY);
            }
        }
    }

    return Result;
}

/** \brief send the slave address to the slave device
     DOES NOT READ STATUS REGISTER 2!!

    \psaram adr - 8-bit slave address
    \return map of ADDR or AF fail -whichever occurred
*/
static u32 I2C_AddrOnly(u8 adr)
{
    u32 rslt;

    FRAM_I2C->DR = adr;
    while ((rslt = (FRAM_I2C->SR1 & (ADDR | AF))) == 0u)
    {
        // wait for ADDR or Acknowledge Failure
    }
    return rslt;
}

/** \brief send the slave address to the slave device
     DOES READ STATUS REGISTER 2!!

    \psaram adr - 8-bit slave address
    \return map of ADDR or AF fail -whichever occurred
*/
static u32 I2C_Addr(u8 adr)
{
    u32 rslt;

    rslt = I2C_AddrOnly(adr);
    (void)FRAM_I2C->SR2;
    return rslt;
}

/** \brief Put a data byte to the I2C device.

    \param[in] byte - the data byte to put
*/
static void I2C_PutByte(u8 byte)
{
    FRAM_I2C->DR = byte;            // Send the data
    while ((FRAM_I2C->SR1 & BTF) == 0u)
    {
        ;
    }
}

/** \brief Write a block of data baytes to the active I2C device

    \param[in] data - pointer to the data buffer
    \param[in] length - length of the data block
*/
static void I2C_TransferOut(const u8 *data, u32 length)
{
    while (length-- != 0u)
    {
        FRAM_I2C->DR = *data++;            // Send the data
        while ((FRAM_I2C->SR1 & BTF) == 0u)
        {
        }
    }
}

/** \brief Set FRAM to one byte to the active I2C device

    \param[in] data - data byte to be used
    \param[in] length - length of the data block
*/
static void I2C_TransferSet(u8 data, u32 length)
{
    while (length-- != 0u)
    {
        FRAM_I2C->DR = data;                // Send the data
        while ((FRAM_I2C->SR1 & BTF) == 0u)
        {
        }
    }
}

// This LOCAL function generates START condition with check for teh SB flag,
// retires if neccessary.
static ErrorCode_t I2C_MakeStartCondition(bool_t RestartEnabled)
{
    ErrorCode_t     ret;
    volatile u32    cycle_counter;
    u32             StartRetries = I2C_START_RETRIES;

    do
    {
        FRAM_I2C->CR1 |= START;
        cycle_counter = I2C_START_CYCLES;
        ret           = ERR_I2C_BUS_STUCK;

        while (((FRAM_I2C->SR1 & SB) != SB) && (cycle_counter != 0))        //lint !e960 Wait
        {   // Wait for SB flag or for time out
            cycle_counter--;
        }

        if ((FRAM_I2C->SR1 & SB) == SB)     // Check if SB flag is set
        {   // Yup.
            ret = ERR_OK;
        }
        else
        {   // Nope, not set
            I2C_ErrorCounterHandler(I2C_START_ERROR);
            if (RestartEnabled)
            {   // We are permitted to retry
                I2C_ErrorCounterHandler(I2C_START_RETRY);
                // Retry but only few times...
                StartRetries--;
                // Ignore the result -- we will be trying the Start again right away
                (void)i2c_InitializeFromOperation();
            }
            else
            {   // Just give up
                StartRetries = 0u;
            }
        }

    } while ((ret != ERR_OK) && (StartRetries != 0));

    return ret;
}

/** \brief Send a 'start' followed by the slave address and command (R/W).  Both the Start
       and slave address have a timeout.

    \param slaveaddress - the command byte to the device
    \return ErrorCode_t - ERR_OK if command byte accepted and status OK, otherwise an error code
*/
static ErrorCode_t I2C_Start(u8 slaveaddr)
{
    ErrorCode_t ret;

    // Read statuc registers to clear potentially pending flags
    (void)FRAM_I2C->SR1;
    (void)FRAM_I2C->SR2;

    // This is the START condition -- we can restart
    ret = I2C_MakeStartCondition(true);

    if (ret == ERR_OK)
    {   // START condition was successfully generated
        ret = ((I2C_Addr(slaveaddr) & AF) == AF) ? ERR_I2C_NO_DEVICE : ERR_OK;      // send slave address
    }

    return ret;
}

/** \brief Send a 'restart' followed by the slave address and command (R/W)

    \param[in] slaveaddress - the command byte to the device
*/
static void I2C_Restart(u8 slaveaddress)
{
    ErrorCode_t ret;

    // Here we call the Start Condition without complete RESTART --
    // We cannot recover anyway...
    ret = I2C_MakeStartCondition(false);

    if (ret == ERR_OK)
    {   // Do the address only if the re-start was successful
        (void)I2C_AddrOnly(slaveaddress);
    }
    else
    {
        I2C_ErrorCounterHandler(I2C_RESTART_ERROR);
    }

    MN_ASSERT(ret == ERR_OK);
}

/** \brief Wait for 1 data byte available from I2C, read it in
    and return the value.

    \return the data
*/
static u32 I2CGetbyte(void)
{
    while ((FRAM_I2C->SR1 & BTF) == 0u)
    {
    }
    return FRAM_I2C->DR;
}

/** \brief Wait for last data byte available from I2C, read it in
    and return the value.

    \return the data
*/
static u32 I2CGetLastByte(void)
{
    while ((FRAM_I2C->SR1 & RxNE) == 0u)
    {
    }
    return FRAM_I2C->DR;
}

#define DISABLE_INT_LOCAL { __disable_interrupt
#define ENABLE_INT_LOCAL()  __set_PRIMASK(iVal); } (void)0

/** \brief Local wait for the Byte Transfer Finished
*/
static void WaitForBTF(void)
{
    while ((FRAM_I2C->SR1 & BTF) != BTF)
    {
    }
}

/** \brief Transfer in (read) <length. bytes from the active I2C device (FRAM)
    This is complicated by the two-byte buffering in the controller versus
    proper issuance of the STOP signal.

    \param data - pointer to the RAM buffer for the read
    \param length - length of the transfer in bytes
    \param addr - I2C addr of the device
*/
static void I2C_TransferIn(u8 *data, u32 length, u8 addr)
{
    u32 iVal = __get_PRIMASK();             // get current state of INT flag

    FRAM_I2C->CR1 = ACK | PE;

    I2C_Restart(addr);
    switch (length)                         // behavior special if transfer < 3 characters
    {
        case 1:
            FRAM_I2C->CR1 &= ~ACK;          // turn off the ACK
            DISABLE_INT_LOCAL();                      // must have tight timing
                (void)FRAM_I2C->SR2;        // release the address phase
                FRAM_I2C->CR1 |= STOP;      // set stop flag
            ENABLE_INT_LOCAL();                       // end of tight timimg
            *data = (u8)I2CGetLastByte();   // fetch the single dtat byte
            break;

        case 2:
            FRAM_I2C->CR1 |= POS_E;         // indicate delayed ACK off
            DISABLE_INT_LOCAL();                      // begin tight timing
                (void)FRAM_I2C->SR2;        // release the address pahse
                FRAM_I2C->CR1 &= ~ACK;      // turn off ACK
            ENABLE_INT_LOCAL();                       // end tight timing
            WaitForBTF();                   // wait for both bytes in input regs
            DISABLE_INT_LOCAL();                      // begin tight timing
                FRAM_I2C->CR1 |= STOP;      // issue stop
                *data++ = (u8)FRAM_I2C->DR; // fetch 1st byte
            ENABLE_INT_LOCAL();                       // end tight timing
            *data = (u8)FRAM_I2C->DR;       // fetch 2nd byte
            break;

        default:
            (void)FRAM_I2C->SR2;            // release address phase
            while (length > 3u)             // transfer in all but 3 bytes
            {
                *data++ = (u8)I2CGetbyte();
                length--;
            }
            WaitForBTF();                   // wait for first 2 of 3 in input regs
            FRAM_I2C->CR1 &= ~ACK;          // turn off ACK
            DISABLE_INT_LOCAL();                      // begin tight timing
                *data++ = (u8)FRAM_I2C->DR; // load first of the three bytes
                FRAM_I2C->CR1 |= STOP;      // signal stop
                *data++ = (u8)FRAM_I2C->DR; // read second of the 3
            ENABLE_INT_LOCAL();                       // end tight timing
            *data = (u8)I2CGetLastByte();   // read third byte
            break;
    }
    while ((FRAM_I2C->CR1 & STOP) == STOP)  // wait for STOPped
    {
    }
    FRAM_I2C->CR1 = ACK | PE;
}

static u8 device;

/** \brief perform an FRAM write operation

    \param[in] address - FRAM address
    \param length = length of the transfer - for range check
    \param[in] unit - device selector 0 = mainboard, 1 = sensor board
*/
static ErrorCode_t FRAM_Address(u32 address, u32 length, u8_least unit)
{
    ErrorCode_t ret = ERR_NVMEM_RANGE;

    device = NV_ADDRESS | (u8)(unit << 1);

    address &= ((u32)FRAM_SIZE - 1u);
    if ((address + length) <= (u32)FRAM_SIZE)
    {
        ret = I2C_Start(device | IWRITE);
        if (ret == ERR_OK)                              // acquire bus, send slave address
        {
            I2C_PutByte(((u8)(address >> 8)));    // FRAM addr MSB
            I2C_PutByte((u8)(address));           //  "        LSB
        }
    }
    return ret;
}

static void FRAM_Write_Enable(void)
{
  /* Disable the FRAM Write Protect */
  GPIOB->ODR &= (USIGN16)~((USIGN16)0x1000);              /* set GPIOB Pin12 to 0 */
}

static void FRAM_Write_Disable(void)
{
  GPIOB->ODR |= (USIGN16)(0x1000);                        /* set GPIOB Pin12 to 1 */
}

/** \brief perform an FRAM write operation

  \param[in] *data - pointer to the write buffer
  \param[in] addr - FRAM address
  \param[in] length - length of the data block
  \param[in] unit - device selector 0 = mainboard, 1 = sensor board
  \return ERR_OK if operation successful, otherwise error code
*/
ErrorCode_t fram_WriteExt(const u8 *data, u32 addr, u32 length, u8_least unit)
{
    ErrorCode_t ret;

	UNUSED_OK(unit);

    I2C_WriteDiagnostic();

    FRAM_Write_Enable();                    // Write-enable to device

    Reset_Watchdog ();

    ret = FRAM_Address(addr, length, 0u);     // transfer command and FRAM address to device
    if (ret == ERR_OK)
    {
        // Note the TransferOut does not internally invoke the STop
        I2C_TransferOut(data, length);
        ret = I2C_Stop();
    }
    else
    {
        (void)I2C_Stop();
    }

    Reset_Watchdog ();

    FRAM_Write_Disable();

    return ret;                         // return result
}

/** \brief perform an FRAM write (SET) operation

  \param[in] *data - pointer to the write buffer
  \param[in] addr - FRAM address
  \param[in] length - length of the data block
  \return ERR_OK if operation successful, otherwise error code
*/
static ErrorCode_t fram_SetExt(const u8 *data, u32 addr, u32 length)
{
    ErrorCode_t ret;

    I2C_WriteDiagnostic();

    FRAM_Write_Enable();                    // Write-enable to device

    Reset_Watchdog ();

    ret = FRAM_Address(addr, length, 0u);     // transfer command and FRAM address to device
    if (ret == ERR_OK)
    {
        // Note the TransferOut does not internally invoke the STop
        I2C_TransferSet(*data, length);
        ret = I2C_Stop();
    }
    else
    {
        (void)I2C_Stop();
    }

    Reset_Watchdog ();

    FRAM_Write_Disable();

    return ret;                         // return result
}

/** \brief perform an FRAM read operation

  \param[in] *data - pointer to the read buffer
  \param[in] addr - FRAM address
  \param[in] length - length of the data block
  \param[in] unit - device selector 0 = mainboard, 1 = sensor board
  \return ERR_OK if operation successful, otherwise error code
*/
ErrorCode_t fram_ReadExt(u8 *data, u32 addr, u32 length, u8_least unit)
{
    ErrorCode_t ret;

	UNUSED_OK(unit);

    I2C_ReadDiagnostic();

    Reset_Watchdog ();

    ret = FRAM_Address(addr, length, 0u); // transfer command and FRAM address to device
    if (ret == ERR_OK)
    {
        I2C_TransferIn(data, length, (u8)(device | IREAD));
    }
    else
    {
        // Note the TransferIn does internally invoke the Stop, thus assymmetry
        (void)I2C_Stop();   // Keep the previous error code, in case Stop succeeds
    }

    Reset_Watchdog ();

    return ret;                             // return result
}

// Convert the error codes to Softing's domain.
static u8 I2CErrorConvert(u8 ucI2cError)
{
  u8 ret;

  switch (ucI2cError)
  {
    case ERR_OK :
      ret = I2C_NO_ERROR;
      break;

    case ERR_I2C_BUS_STUCK :
      ret = I2C_ERROR_BUS_BUSY;
      break;

    case ERR_I2C_NO_DEVICE :
      ret = I2C_ERROR_BUS_BUSY;
      break;

    default :
      ret = I2C_ERROR_BUS_BUSY;
      break;
  }

  return ret;
}


//----------------------------------------------------------------------------------------------------------

FUNCTION VOID hw_I2C_init(VOID)
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:

  Initialization of the I2C bus. The micro is used as an I2C bus master.

PARAMETERS:

GLOBALS_AFFECTED:

RETURN_VALUES:

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  (void)i2c_Initialize();

  return;
}

static OS_RSEMA i2cMutex;
void i2c_CreateMutex(void)
{
    OS_CREATERSEMA(&i2cMutex);
}

//lint -sem(i2c_AcquireMutex, thread_lock)
void i2c_AcquireMutex(void)
{
    if((u8)OS_Request(&i2cMutex) == 0)
    {
        //Mutex not available
        (void)OS_Use(&i2cMutex); //wait
    }
}

//lint -sem(i2c_ReleaseMutex, thread_unlock)
void i2c_ReleaseMutex(void)
{
    OS_Unuse(&i2cMutex);
}


/* -----------------------------------------------------------------------
 * description: Interface function handling the access to the I2C bus
 *              (writing or reading data to/from an I2C slave device).
 *              Can be used either with 8-bit or 16-bit word addresses.
 *              Also the caller can choose whether this function shall
 *              wait for the end of the I2C bus transaction or not, since
 *              an interrupt-driven state machine performs the transaction
 *              after the slave device has acknowledged its address, i.e.
 *              the I2C bus transaction may run in parallel.
 *
 * entries: ucSlaveAddr  : address of I2C slave device (bit #0 should be 0)
 *          uiWordAddr   : word address (8-bit or 16-bit)
 *          *pucData     : pointer to data buffer (transmit or receive)
 *          uiNbrOfBytes : # of data bytes to transmit/receive
 *          ucRequestType: one of the following:
 *                         I2C_WRITE_ADDR8_NOWAIT
 *                         I2C_WRITE_ADDR8_WAIT
 *                         I2C_WRITE_ADDR16_NOWAIT
 *                         I2C_WRITE_ADDR16_WAIT
 *                         I2C_READ_ADDR8_NOWAIT
 *                         I2C_READ_ADDR8_WAIT
 *                         I2C_READ_ADDR16_NOWAIT
 *                         I2C_READ_ADDR16_WAIT
 *
 * returns: ucI2cError   : I2C_NO_ERROR (0) if everything is ok,
 *                         I2C_ERROR_NO_ACK (1) if slave device not ready,
 *                         I2C_ERROR_BUS_BUSY (2) if I2C bus is busy
 * ----------------------------------------------------------------------- */
u8  i2cBusRequest(u8    ucSlaveAddr,
                  u32   uiWordAddr,
                  u8   *pucData,
                  u32   uiNbrOfBytes,
                  u8    ucRequestType,
                  void  (*pvCallback) (u32 numbytes)
                 )
{
  u8 ucI2cError = (u8)ERR_OK;

  UNUSED_OK(ucSlaveAddr);

  if (uiNbrOfBytes != 0)
  {
      i2c_AcquireMutex();

      ucI2cWordAddress = uiWordAddr;
      pucI2cData = pucData;
      uiI2cNbrOfBytes = uiNbrOfBytes;
      ucI2cRequestType = ucRequestType;
      pvI2cCallback = pvCallback;


      if ((ucI2cRequestType & I2C_READ) != 0u)
      {
        ucI2cError = (u8)fram_ReadExt(pucI2cData,
                                     ucI2cWordAddress,
                                     uiI2cNbrOfBytes, 0);
      }
      else if ((ucI2cRequestType & I2C_SET) != 0u)
      {
        ucI2cError = (u8)fram_SetExt(pucI2cData,
                                      ucI2cWordAddress,
                                      uiI2cNbrOfBytes);
      }
      else  /* WRITE */
      {
        ucI2cError = (u8)fram_WriteExt(pucI2cData,
                                       ucI2cWordAddress,
                                       uiI2cNbrOfBytes, 0);
      }
      i2c_ReleaseMutex();
  }

  ucI2cError = I2CErrorConvert(ucI2cError);

  if (pvI2cCallback != NULL)
  {
    pvI2cCallback (ucI2cError);
  }

  return ucI2cError;
}

/** \brief Perform any FRAM specific initialization
*/
void fram_Initialize(void)
{
    EE_WR_DISABLE;
    hw_I2C_init();
}
