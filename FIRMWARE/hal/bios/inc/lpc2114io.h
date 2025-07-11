/*****************************************************************************
*  File lpc2114io.h
*  MNCB - Peripheral Driver Functions
*
*******************************************************************************
*  WRITTEN :
*      July 2004
*  REVISION HISTORY:
*
*
*
******************************************************************************/

/*****************************************************************************/
/*                                                                           */
/*    Copyright 2004 - 2004 as an unpublished trade secret.                  */
/*                                                                           */
/*    This document and all information herein is the property of            */
/*    Valve & Controls Division of Dresser Industries,Inc. It is             */
/*    confidential and must not be made public or copied and is              */
/*    subject to return upon demand.                                         */
/*                                                                           */
/*****************************************************************************/

#ifndef LPC2114IO_H
#define LPC2114IO_H

#if 0

/* Location of FLASH checksum ************************************************/
typedef volatile struct CrcFlash_t
{
  u8 CHECKSUM_HIGH; /* CRC is saved from external checksumming program in BIG endian format */
  u8 CHECKSUM_LOW;
} CrcFlash_t;

#define FLASH_CHECKSUM_ADDRESS 0x0001DFFEL
#define FLASH_CHECKSUM HARDWARE(CrcFlash_t*,FLASH_CHECKSUM_ADDRESS)
#endif
#define REG8(x)   u8 x;  u8 x##pad[3] //lint !e961 token asting used
#define REG16(x) u16 x; u16 x##pad  //lint !e961 token asting used
#define REG32(x) u32 x
/******************************************************************************

  I2C definitions etc.

*/

typedef volatile struct i2c_t
{
	REG8(I2CONSET);                        // I2C Control Set Register

#define AA      ((u8)(1u << 2))    // Assert acknowledge flag
#define SI      ((u8)(1u << 3))    // I2C interrupt flag
#define STO     ((u8)(1u << 4))    // STOP flag
#define STA     ((u8)(1u << 5))    // START flag
#define I2EN    ((u8)(1u << 6))    // I2C interface enable

	REG8(I2STAT);                          // I2C Status Register

#define AA08 ((u8)0x08)     // A START condition has been transmitted
#define AA10 ((u8)0x10)     // A repeated START condition has been transmitted
#define AA18 ((u8)0x18)     // SLA+W has been transmitted; ACK has been received
#define AA20 ((u8)0x20)     // SLA+W has been transmitted; NOT ACK has been received
#define AA28 ((u8)0x28)     // Data byte in S1DAT has been transmitted; ACK has been received
#define AA30 ((u8)0x30)     // Data byte in S1DAT has been transmitted; NOT ACK has been received
#define AA38 ((u8)0x38)     // Arbitration lost in SLA+R/W or Data bytes
#define AA40 ((u8)0x40)     // SLA+R has been transmitted; ACK has been received
#define AA48 ((u8)0x48)     // SLA+R has been transmitted; NOT ACK has been received
#define AA50 ((u8)0x50)     // Data byte has been received; ACK has been returned
#define AA58 ((u8)0x58)     // Data byte has been received; NOT ACK has been returned
#define AA60 ((u8)0x60)     // Own SLA+W has been received; ACK has been returned
#define AA68 ((u8)0x68)     // Arbitration lost in SLA+R/W as master; Own SLA+W has been received, ACK returned
#define AA70 ((u8)0x70)     // General call address (00H) has been received; ACK has been returned
#define AA78 ((u8)0x78)     // Arbitration lost in SLA+R/W as master; General call address has been received,ACK has been returned
#define AA80 ((u8)0x80)     // Previously addressed with own SLV address; DATA has been received; ACK has been returned
#define AA88 ((u8)0x88)     // Previously addressed with own SLA; DATA byte has been received; NOT ACK has been returned
#define AA90 ((u8)0x90)     // Previously addressed with General Call; DATA byte has been received; ACK has been returned
#define AA98 ((u8)0x98)     // Previously addressed with General Call; DATA byte has been received; NOT ACK has been returned
#define AAA0 ((u8)0xA0)     // A STOP condition or repeated START condition has been received while still addressed as SLV/REC or SLV/TRX
#define AAA8 ((u8)0xA8)     // Own SLA+R has been received; ACK has been returned
#define AAB0 ((u8)0xB0)     // Arbitration lost in SLA+R/W as master; Own SLA+R has been received, ACK has been returned
#define AAB8 ((u8)0xB8)     // Data byte in S1DAT has been transmitted; ACK has been received
#define AAC0 ((u8)0xC0)     // Data byte in S1DAT has been transmitted; NOT ACK has been received
#define AAC8 ((u8)0xC8)     // Last data byte in S1DAT has been transmitted (AA = 0); ACK has been received
#define AAF8 ((u8)0xF8)     // No result
    REG8(I2DAT);                          // I2C Data Register
    REG8(I2ADR);                          // I2C Slave Address Register
    REG16(I2SCLH);                         // SCL Duty Cycle Register High Half Word
    REG16(I2SCLL);                         // SCL Duty Cycle Register Low Half Word
    REG8(I2CONCLR);                       // I2C Control Clear Register
#define AAC       ((u8)(1u << 2))
#define SIC       ((u8)(1u << 3))
#define STAC      ((u8)(1u << 5))
#define I2ENC     ((u8)(1u << 6))
} i2c_t;

#define I2C_ADDRESS 0xE001C000UL
#define I2C HARDWARE(i2c_t*, I2C_ADDRESS)

/******************************************************************************

  PWM definitions etc.

*/
typedef volatile struct pwm_t
{
    u32
        PWMIR,
#define    MR0INT   (1u << 0)
#define    MR1INT   (1u << 1)
#define    MR2INT   (1u << 2)
#define    MR3INT   (1u << 3)
#define    MR4INT   (1u << 8)
#define    MR5INT   (1u << 9)
#define    MR6INT   (1u << 10)

        PWMTCR,
#define  CE      (1u << 0)     // counter enable
#define  CR      (1u << 1)     // counter reset
#define  PWMEN   (1u << 3)     // PWM enable

        PWMTC,
        PWMPR,
        PWMPC,
        PWMMCR,
#define    MR0INTx   (1u << 0)
#define    MR0RES    (1u << 1)
#define    MR0STOP   (1u << 2)
#define    MR1INTx   (1u << 3)
#define    MR1RES    (1u << 4)
#define    MR1STOP   (1u << 5)
#define    MR2INTx   (1u << 6)
#define    MR2RES    (1u << 7)
#define    MR2STOP   (1u << 8)
#define    MR3INTx   (1u << 9)
#define    MR3RES    (1u << 10)
#define    MR3STOP   (1u << 11)
#define    MR4INTx   (1u << 12)
#define    MR4RES    (1u << 13)
#define    MR4STOP   (1u << 14)
#define    MR5INTx   (1u << 15)
#define    MR5RES    (1u << 16)
#define    MR5STOP   (1u << 17)
#define    MR6INTx   (1u << 18)
#define    MR6RES    (1u << 19)
#define    MR6STOP   (1u << 20)

        PWMMR0,
        PWMMR1,
        PWMMR2,
        PWMMR3,
        dummy1[6],
        PWMMR4,
        PWMMR5,
        PWMMR6,
        PWMPCR,
#define  SEL1  (1u << 1)
#define  SEL2  (1u << 2)
#define  SEL3  (1u << 3)
#define  SEL4  (1u << 4)
#define  SEL5  (1u << 5)
#define  SEL6  (1u << 6)

#define  ENA1  (1u << 9)
#define  ENA2  (1u << 10)
#define  ENA3  (1u << 11)
#define  ENA4  (1u << 12)
#define  ENA5  (1u << 13)
#define  ENA6  (1u << 14)
        PWMLER;
#define   EM0L (1u << 0)
#define   EM1L (1u << 1)
#define   EM2L (1u << 2)
#define   EM3L (1u << 3)
#define   EM4L (1u << 4)
#define   EM5L (1u << 5)
#define   EM6L (1u << 6)
}   pwm_t;

#define PWM_ADDRESS 0xe0014000UL
#define PWM HARDWARE(pwm_t*, PWM_ADDRESS)


/******************************************************************************

   Timer definitions etc.

*/
typedef volatile struct timer_t
{
    u32
        IR,	   /* Interrupt Register */
#define   IR_MR0          ((u32)(1u << 0)) /* Interrupt flag for match channel 0 */
        TCR,   /* Timer control register */
#define   COUNTER_ENABLE  ((u32)(1u << 0)) /* Enable counter and prescale counter for counting */
        TC,	   /* Timer counter */
        PR,	   /* Prescale register */
        PC,	   /* Prescale counter */
        MCR,       /* Match control register */
#define   MCR_IR_MR0    ((u32)(1u << 0)) /* Interrupt if MR0 matches TC */
#define   MCR_RESET_MR0 ((u32)(1u << 1)) /* Reset TC if  MR0 matches TC */
        MR0,   /* Match register 0 */
        MR1,
        MR2,
        MR3,
        CCR,   /* Capture control register */
        CR0,   /* Capture register 0 */
        CR1,
        CR2,
        CR3,
        EMR;   /* external match register */
#define   EMR_C0_TOGGLE  ((u32)(3u << 4)) /* Toggle MATX.0 if TC=MR0 */
#define   EMR_C1_TOGGLE  ((u32)(3u << 6)) /* Toggle MATX.1 if TC=MR1 */
} timer_t;
#define TIMER0_ADDRESS    0xe0004000UL
#define TIMER1_ADDRESS    0xe0008000UL

#define T0  HARDWARE(timer_t*, TIMER0_ADDRESS)
#define T1  HARDWARE(timer_t*, TIMER1_ADDRESS)
#define T1EMR             T1->EMR
#define T1MR0             T1->MR0
#define T1TC              T1->TC

/******************************************************************************

   SPI definitions etc.

*/

#define SPI_CLK_RATE 8

typedef volatile struct spi_t
{
    REG8(SPCR);
#define CPHA   (1u << 3)    // phase control
#define CPOL   (1u << 4)    // 0 = clock active high
#define MSTR   (1u << 5)    // 1 = master
#define LSBF   (1u << 6)    // 1 = LSB first
#define SPIE   (1u << 7)    // 1 = interupt enable

    REG8(SPSR);
#define ABRT   ((u8)(1u << 3))
#define MODF   ((u8)(1u << 4))
#define ROVR   ((u8)(1u << 5))
#define WCOL   ((u8)(1u << 6))
#define SPIF   ((u8)(1u << 7))
    REG8(SPDR);
    REG8(SPCCR);
    u32    dummy[3];
    REG8(SPINT);
#define SPIINT (1u << 0)
} spi_t;

#define SPI0_ADDRESS  0xe0020000UL
#define SPI1_ADDRESS  0xe0030000UL
#define SPI0 HARDWARE(spi_t*, SPI0_ADDRESS)
#define SPI1 HARDWARE(spi_t*, SPI1_ADDRESS)

#ifdef ADC_INTERNAL


/******************************************************************************

  ADC definitions etc.

*/
typedef volatile struct adc_t
{
    u32
        ADCR,
#define SEL_MSK 0xff
#define CLKDIV  (0u << 8)
#define BURST   (1u << 16)
#define CLKS    (0u << 17)
#define PDN     (1u << 21)
#define START   (1u << 24)

        ADDR;
#define DONE    (1u << 31)
#define OVERRUN (1u << 30)
#define CHN_MSK (0x7u << 27)
#define VAL_MSK (0xffc0u << 0)

} adc_t;

#define ADC_ADDRESS 0xe0034000UL

#define ADC HARDWARE(adc_t*, ADC_ADDRESS)

#endif
/******************************************************************************

   VIC definitions etc.

*/
typedef volatile struct vic_t
{
    u32
        VICIRQStatus,
        VICFIQStatus,
        VICRawIntr,
        VICIntSelect,
        VICIntEnable,	  /* Set interrupt enable flags */
        VICIntEnClr,	  /* Clear interrupt enable Flags */
        VICSoftInt,
        VICSoftIntClear,
        VICProtection,
        resv1[(0x30 - 0x24) / 4],
        VICVectAddr;
    void
        (MN_IRQ *VICDefVectAddr)(void);
    u32	
        resv[(0x100 - 0x38) / 4];
    void
        (MN_IRQ *ViCVectAddrX[16])(void);
    u32	
        resv2[(0x200- 0x140) / 4],
        ViCVectCntlX[16];
#define VIC_VECT_SLOT_ENABLE (u32)(1 << 5) /* enable VIC vector slot */
} vic_t;

#define VIC_ADDR    0xFFFFF000UL
#define VIC	    	HARDWARE(vic_t*, VIC_ADDR)
#define VICProtection  (VIC->VICProtection)
#define VICIntEnClear  VIC->VICIntEnClr
#define VICDefVectAddr VIC->VICDefVectAddr
#define VICVectCntl    VIC->VICVectCntlX
#define VICVectAddr	   VIC->VICVectAddr
#define VICIntSelect   VIC->VICIntSelect
#define VICVectAddr1   VIC->VICVectAddrX[1]
#define VICVectCntl1   VIC->VICVectCntlX[1]
#define VICIntEnable   VIC->VICIntEnable

#define VIC_WDT          0U  /* Watchdog                           */
#define VIC_SW           1U  /* Software interrupts                */
#define VIC_DEBUGRX      2U  /* Embedded ICE, DbgCommRx            */
#define VIC_DEBUGTX      3U  /* Embedded ICE, DbgCommTx            */
#define VIC_TIMER0       4U  /* Timer 0 (Match 0-3 Capture 0-3)    */
#define VIC_TIMER1       5U  /* Timer 1 (Match 0-3 Capture 0-3)    */
#define VIC_UART0        6U  /* UART 0  (RLS, THRE, RDA, CTI)      */
#define VIC_UART1        7U  /* UART 1  (RLS, THRE, RDA, CTI, MSI) */
#define VIC_PWM0         8U  /* PWM 0   (Match 0-6 Capture 0-3)    */
#define VIC_I2C          9U  /* I2C     (SI)                       */
#define VIC_SPI0        10U  /* SPI0     (SPIF, MODF)              */
#define VIC_SPI1        11U  /* SPI0     (SPIF, MODF)              */
#define VIC_PLL         12U  /* PLL lock (PLOCK)                   */
#define VIC_RTC         13U  /* RTC     (RTCCIF, RTCALF)           */
#define VIC_EINT0       14U  /* External interrupt 0 (EINT0)       */
#define VIC_EINT1       15U  /* External interrupt 1 (EINT1)       */
#define VIC_EINT2       16U  /* External interrupt 2 (EINT2)       */
#define VIC_EINT3       17U  /* External interrupt 2 (EINT2)       */
#define VIC_ADC         18u
#define VIC_CAN_CMN     19u
#define VIC_CAN1_TX     20u
#define VIC_CAN2_TX     21u
#define VIC_CAN3_TX     22u
#define VIC_CAN4_TX     23u
//#define VIC_          24u
//#define VIC_          25u
#define VIC_CAN1_RX     26u
#define VIC_CAN2_RX     27u
#define VIC_CAN3_RX     28u
#define VIC_CAN4_RX     29u
//#define VIC_          30u
//#define VIC_          31u

#define VIC_IRQ_PRIO_0   0U
#define VIC_IRQ_PRIO_1   1U
#define VIC_IRQ_PRIO_2   2U
#define VIC_IRQ_PRIO_3   3U
#define VIC_IRQ_PRIO_4   4U
#define VIC_IRQ_PRIO_5   5U
#define VIC_IRQ_PRIO_6   6U
#define VIC_IRQ_PRIO_7   7U
#define VIC_IRQ_PRIO_8   8U
#define VIC_IRQ_PRIO_9   9U
#define VIC_IRQ_PRIO_10 10U
#define VIC_IRQ_PRIO_11 11U
#define VIC_IRQ_PRIO_12 12U
#define VIC_IRQ_PRIO_13 13U
#define VIC_IRQ_PRIO_14 14U
#define VIC_IRQ_PRIO_15 15U
#define VIC_IRQ_PRIO_16 16U


/******************************************************************************

    map of the EXTINT control registers in the System Control Block

*/

//lint ++flb         This is hardware we cannot omit unused structure elements
typedef volatile struct extint_t
{
    u32
        EXTINT,     // External Interrupt Flag Register R/W 0 0xE01F C140
        EXTWAKE,    // External Interrupt Wakeup Register R/W 0 0xE01F C144
        EXTMODE,    // External Interrupt Mode Register R/W 0 0xE01F C148
        EXTPOLAR;   // External Interrupt Polarity Register R/W 0 0xE01F C14C
} extint_t;
//lint --flb

#define EXT_ADDRESS 0xE01fc140UL
#define EXT HARDWARE(extint_t*, EXT_ADDRESS)

/******************************************************************************

   PINSEL definitions etc.

*/
typedef volatile struct pinsel_t
{
    u32
        PINSEL0,
        PINSEL1,
        resv[3],
        PINSEL2;
} pinsel_t;

#define PINSEL_ADDR 0xE002C000UL
#define PINSEL  HARDWARE(pinsel_t*, PINSEL_ADDR)
#define PINSEL0 PINSEL->PINSEL0
#define PINSEL1 PINSEL->PINSEL1

/******************************************************************************

   GPIO definitions etc.

*/
typedef volatile struct gpio_t
{
    u32
        IOPIN,
        IOSET,
        IODIR,
        IOCLR;
} gpio_t;

#define GPIO0_ADDR  0xE0028000UL
#define GPIO1_ADDR  0xE0028010UL
#define GPIO0   HARDWARE(gpio_t*, GPIO0_ADDR)
#define GPIO1   HARDWARE(gpio_t*, GPIO1_ADDR)
#define IO0PIN  GPIO0->IOPIN
#define IO0SET  GPIO0->IOSET
#define IO0DIR  GPIO0->IODIR
#define IO0CLR  GPIO0->IOCLR
#define IO1PIN  GPIO1->IOPIN
#define IO1SET  GPIO1->IOSET
#define IO1DIR  GPIO1->IODIR
#define IO1CLR  GPIO1->IOCLR

/******************************************************************************

   System Control Block (partial) definitions etc.

*/
typedef volatile struct ctl_t
{
    u32
	    MEMMAP,   /* Memory mapping control */
#define MEMMAP_BOOTLOADER_MODE (u32)(0)
#define MEMMAP_USER_FLASH_MODE (u32)(1)
#define MEMMAP_USER_RAM_MODE (u32)(2)
#define MEMMAP_EXTERNAL_MODE (u32)(3)
		RESV1[(0x80-0x44) /4], /* gap. do not use */
		PLLCON,	/* PLL control register */
		PLLCFG,	/* PLL configuration register */
		PLLSTAT, /* PLL status register */
		PLLFEED, /* PLL feed register */
		RESV2[(0xC0-0x90) /4], /* gap. do not use */
        PCON,	/* Power control register */
        PCONP,	/* power control for peripherals */
#define PCTIM0  ((u32)(1u << 1)) /* Timer0 power down */
#define PCTIM1  ((u32)(1u << 2)) /* Timer1 power down */
#define PCCURT0 ((u32)(1u << 3)) /* Uart0 power down */
#define PCCURT1 ((u32)(1u << 4)) /* Uart1 power down */
#define PCPWM0  ((u32)(1u << 5)) /* PWM0 power down */
#define PCI2C   ((u32)(1u << 7)) /* I2c power down */
#define PCSPI0  ((u32)(1u << 8)) /* SPI0 power down */
#define PCRTC   ((u32)(1u << 9)) /* RTC power down */
#define PCSPI1  ((u32)(1u << 10)) /* SPI1 power down */
#define PRESERVED11 ((u32)(1u << 11)) /* reserved pin should be 0 to reduce power */
#define PCAD    ((u32)(1u << 12)) /* A/D power down */
        RESV3[(0x100 - 0xc8) / 4], /* gap. do not use */
        VPBDIV;	/* VPB divider control */
#define VPBDIV_CLOCKDIV4 ((u32)(0u)) /* VPB bus clock is one fourth of the processor clock */
#define VPBDIV_CLOCKDIV1 ((u32)(1u)) /* VPB bus clock is same as the processor clock */
#define VPBDIV_CLOCKDIV2 ((u32)(2u)) /* VPB bus clock is one half of the processor clock */
} ctl_t;

#define CTL_ADDR    0xE01fc040UL
#define CTL	        HARDWARE(ctl_t*, CTL_ADDR)
#define VPBDIV		CTL->VPBDIV
#define PCONP		CTL->PCONP
#define MEMMAP		CTL->MEMMAP

/******************************************************************************

   MAM definitions etc.

*/
typedef volatile struct mam_t
{
    u32
        MAMCR,
        MAMTIM;
} mam_t;

#define MAM_ADDR    0xE01fc000UL
#define MAM	        HARDWARE(mam_t*, MAM_ADDR)
#define MAMCR		MAM->MAMCR
#define MAMTIM		MAM->MAMTIM

/******************************************************************************

   UART definitions etc.

*/
typedef volatile struct uart_t
{
    REG8(RBR);    // 0
    REG8(IER);    // 4
#define RBRIE	(0x01UL)	/* Receiveint enable */
#define THRIE  (0x02UL)	/* Transmitint enable */
#define LSIE   (0x04UL) /* Line Status int enable */
    REG8(IIR);    // 8
#define IIP_INTID  ((u8)(0x0e)) /* Interrupt identification */
#define INTID_RDA	 ((u8)(0x04)) /* Receive data available */
#define INTID_THRE ((u8)(0x02)) /* THRE int  */
#define INTID_LSIE ((u8)(0x06)) /* THRE int  */

#define FCRFIFOENA ((u8)(0x01)) /* FIFO enable */
#define FCRRSTRX   ((u8)(0x02))
#define FCRRSTTX   ((u8)(0x04))
    REG8(LCR);    // c
#define LCRLENSEL ((u8)(0x03)) /* Wordlenselect, 8bit */
#define LCRSTPBIT ((u8)(0x04)) /* number of stopbits  */
#define LCRPARENA	((u8)(0x08)) /* Parity enable */
#define LCRPARSELODD  ((u8)(0x00)) /* odd parity */
#define LCRPARSELEVEN ((u8)(0x10)) /* even parity */
#define LCRBRKCTRL    ((u8)(0x40)) /* Break control */
#define LCRDLAB       ((u8)(0x80)) /* Divisor latch access bit */
    REG8(MSR);    // 10
    REG8(LSR);    // 14
#define LSR_RDR	((u8)(0x01))
#define LSR_OE    ((u8)(0x02))	/* Overrun error  */
#define LSR_PE    ((u8)(0x04))	/* Parity error  */
#define LSR_FE    ((u8)(0x08))	/* Framing error  */
#define LSR_THRE  ((u8)(0x20))    /* Transmitter Holding Register Empty */
    REG8(SPR);    // 1c
} uart_t;

#define FCR IIR
#define THR RBR
#define DLL RBR
#define DLM IER
#define IER_RBR	

#define UART0_ADDR  0xE000c000UL
#define UART1_ADDR  0xE0010000UL
#define UART0       HARDWARE(uart_t*, UART0_ADDR)
#define UART1       HARDWARE(uart_t*, UART1_ADDR)
#define COM1        UART0
#define COM2        UART1

#define U0LSR		UART0->LSR
#define U0RBR		UART0->RBR
#define U0THR		UART0->THR
#define U0IER		UART0->IER
#define U0IIR		UART0->IIR
#define U0LCR		UART0->LCR
#define U0FCR		UART0->FCR
#define U0DLL		UART0->DLL
#define U0DLM		UART0->DLM
/******************************************************************************

   Watch Dog definitions etc.

*/
typedef volatile struct wdog_t
{
    REG8(WDMOD);
    #define WDEN        (1u << 0)
    #define WDRESET     (1u << 1)
    #define WDTOF       (1u << 2)
    #define WDTINT      (1u << 3)
    REG32(WDTC);
    REG8(WDFEED);
    REG32(WDTV);
} wdog_t;
#define WD_ADDR     0xE0000000UL
#define WDOG        HARDWARE(wdog_t*, WD_ADDR)

#endif //LPC2114IO_H
