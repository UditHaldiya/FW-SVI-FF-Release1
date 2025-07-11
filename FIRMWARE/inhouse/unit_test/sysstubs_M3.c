/**
Copyright 2011 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file stm_rcc.c

    \brief Driver for the system reset and clocking

    OWNER:
    CPU: STM32F10xx Cortex

    $Revision:  $
*/
#include "config.h"
#include "mnwrap.h"
#include "stmcommon.h"
// #include "bsp.h"
#include "gpio.h"

#define USE_HSE     0

#define RCC_BASE              (AHBPERIPH_BASE + 0x1000u)

//#define FSMC_Bank1_R_BASE     (FSMC_R_BASE + 0x0000) /*!< FSMC Bank1 registers base address */

//lint ++flb
typedef struct RCC_TypeDef
{
    IO_REG32(CR);
    IO_REG32(CFGR);
    IO_REG32(CIR);
    IO_REG32(APB2RSTR);
    IO_REG32(APB1RSTR);
    IO_REG32(AHBENR);
    IO_REG32(APB2ENR);
    IO_REG32(APB1ENR);
    IO_REG32(BDCR);
    IO_REG32(CSR);

#ifdef STM32F10X_CL
    IO_REG32(AHBRSTR);
    IO_REG32(CFGR2);
#endif /* STM32F10X_CL */

#if defined (STM32F10X_LD_VL) || defined (STM32F10X_MD_VL) || defined (STM32F10X_HD_VL)
    IO_REG32(RESERVED0);
    IO_REG32(CFGR2);
#endif /* STM32F10X_LD_VL || STM32F10X_MD_VL || STM32F10X_HD_VL */
} RCC_TypeDef;
//lint --flb

#define RCC                 HARDWARE(RCC_TypeDef *, RCC_BASE)

//lint ++flb
// begin some RCC registers

//Bit 10 SDIOEN: SDIO clock enable
//Set and cleared by software.
//0: SDIO clock disabled
//1: SDIO clock enabled
#define SDIOEN (1u << 10)
//Bits 9 Reserved, always read as 0.
//Bit 8 FSMCEN: FSMC clock enable
//Set and cleared by software.
//0: FSMC clock disabled
//1: FSMC clock enabled
#define FSMCEN (1u << 8)
//Bit 7 Reserved, always read as 0.
//Bit 6 CRCEN: CRC clock enable
//Set and cleared by software.
//0: CRC clock disabled
//1: CRC clock enabled
#define CRCEN (1u << 6)
//Bit 5 Reserved, must be kept at reset value.
//Bit 4 FLITFEN: FLITF clock enable
//Set and cleared by software to disable/enable FLITF clock during Sleep mode.
//0: FLITF clock disabled during Sleep mode
//1: FLITF clock enabled during Sleep mode
#define FLITFEN (1u << 4)
//Bit 3 Reserved, must be kept at reset value.
//Bit 2 SRAMEN: SRAM interface clock enable
//Set and cleared by software to disable/enable SRAM interface clock during Sleep mode.
//0: SRAM interface clock disabled during Sleep mode.
//1: SRAM interface clock enabled during Sleep mode
#define SRAMEN (1u << 2)
//Bit 1 DMA2EN: DMA2 clock enable
//Set and cleared by software.
//0: DMA2 clock disabled
//1: DMA2 clock enabled
#define DMA2EN (1u << 1)
//Bit 0 DMA1EN: DMA1 clock enable
//Set and cleared by software.
//0: DMA1 clock disabled
//1: DMA1 clock enabled
#define DMA1EN (1u << 0)

// RCC_AHBENR
//Bits 31:17 Reserved, must be kept at reset value.
//Bit 16 ETHMACRXEN: Ethernet MAC RX clock enable
//Set and cleared by software.
#define ETHMACRXEN (1u << 16)
//0: Ethernet MAC RX clock disabled
//1: Ethernet MAC RX clock enabled
//Note: In the RMII mode, if this clock is enabled, the RMII clock of the MAC is also enabled.
//Bit 15 ETHMACTXEN: Ethernet MAC TX clock enable
//Set and cleared by software.
#define ETHMACTXEN (1u << 15)
//0: Ethernet MAC TX clock disabled
//1: Ethernet MAC TX clock enabled
//Note: In the RMII mode, if this clock is enabled, the RMII clock of the MAC is also enabled.
//Bit 14 ETHMACEN: Ethernet MAC clock enable
//Set and cleared by software. Selection of PHY interface (MII/RMII) must be done before
//enabling the MAC clock.
#define ETHMACEN (1u << 14)
//0: Ethernet MAC clock disabled
//1: Ethernet MAC clock enabled
//Bit 13 Reserved, must be kept at reset value.
//Bit 12 OTGFSEN: USB OTG FS clock enable
//Set and cleared by software.
#define OTGFSEN (1u << 12)
//0: USB OTG FS clock disabled
//1: USB OTG FS clock enabled
//Bits 11:7 Reserved, must be kept at reset value.
//Bit 6 CRCEN: CRC clock enable
//Set and cleared by software.
#define CRCEN (1u << 6)
//0: CRC clock disabled
//1: CRC clock enabled
//Bit 5 Reserved, must be kept at reset value.
//Bit 4 FLITFEN: FLITF clock enable
//Set and cleared by software to disable/enable FLITF clock during sleep mode.
#define FLITFEN (1u << 4)
//0: FLITF clock disabled during Sleep mode
//1: FLITF clock enabled during Sleep mode
//Bit 3 Reserved, must be kept at reset value.
//Bit 2 SRAMEN: SRAM interface clock enable
//Set and cleared by software to disable/enable SRAM interface clock during Sleep mode.
#define SRAMEN (1u << 2)
//0: SRAM interface clock disabled during Sleep mode
//1: SRAM interface clock enabled during Sleep mode
//Bit 1 DMA2EN: DMA2 clock enable
//Set and cleared by software.
#define DMA2EN (1u << 1)
//0: DMA2 clock disabled
//1: DMA2 clock enabled
//Bit 0 DMA1EN: DMA1 clock enable
//Set and cleared by software.
#define DMA1EN (1u << 0)
//0: DMA1 clock disabled
//1: DMA1 clock enabled

// RCC_APB2ENR
//Bits 31:15 Reserved, must be kept at reset value.
//Bit 14 USART1EN: USART1 clock enable
//Set and cleared by software.
#define USART1EN (1u << 14)
//0: USART1 clock disabled
//1: USART1 clock enabled
//Bit 13 Reserved, must be kept at reset value.
//Bit 12 SPI1EN: SPI 1 clock enable
//Set and cleared by software.
#define SPI1EN (1u << 12)
//0: SPI 1 clock disabled
//1: SPI 1 clock enabled
//RM0008 Connectivity line devices: reset and clock control (RCC)
//Doc ID 13902 Rev 13 143/1093
//Bit 11 TIM1EN: TIM1 Timer clock enable
//Set and cleared by software.
#define TIM1EN (1u << 11)
//0: TIM1 timer clock disabled
//1: TIM1 timer clock enabled
//Bit 10 ADC2EN: ADC 2 interface clock enable
//Set and cleared by software.
#define ADC2EN (1u << 10)
//0: ADC 2 interface clock disabled
//1: ADC 2 interface clock enabled
//Bit 9 ADC1EN: ADC 1 interface clock enable
//Set and cleared by software.
#define ADC1EN (1u << 9)
//0: ADC 1 interface disabled
//1: ADC 1 interface clock enabled
//Bits 8:7 Reserved, must be kept at reset value.
//Bit 6 IOPEEN: I/O port E clock enable
//Set and cleared by software.
#define IOPGEN (1u << 8)
#define IOPFEN (1u << 7)
#define IOPEEN (1u << 6)
//0: I/O port E clock disabled
//1: I/O port E clock enabled
//Bit 5 IOPDEN: I/O port D clock enable
//Set and cleared by software.
#define  IOPDEN (1u << 5)
//0: I/O port D clock disabled
//1: I/O port D clock enabled
//Bit 4 IOPCEN: I/O port C clock enable
//Set and cleared by software.
#define IOPCEN (1u << 4)
//0: I/O port C clock disabled
//1:I/O port C clock enabled
//Bit 3 IOPBEN: I/O port B clock enable
//Set and cleared by software.
#define IOPBEN (1u << 3)
//0: I/O port B clock disabled
//1:I/O port B clock enabled
//Bit 2 IOPAEN: I/O port A clock enable
//Set and cleared by software.
#define IOPAEN (1u << 2)
//0: I/O port A clock disabled
//1:I/O port A clock enabled
//Bit 1 Reserved, must be kept at reset value.
//Bit 0 AFIOEN: Alternate function I/O clock enable
//Set and cleared by software.
#define AFIOEN (1u << 0)
//0: Alternate Function I/O clock disabled
//1:Alternate Function I/O clock enabled



// RCC_APB1ENR
//Bits 31:30 Reserved, must be kept at reset value.
//Bit 29 DACEN: DAC interface clock enable
//Set and cleared by software.
#define DACEN (1u << 29)
//0: DAC interface clock disabled
//1: DAC interface clock enable
//Bit 28 PWREN: Power interface clock enable
//Set and cleared by software.
#define PWREN (1u << 28)
//0: Power interface clock disabled
//1: Power interface clock enable
//Bit 27 BKPEN: Backup interface clock enable
//Set and cleared by software.
#define BKPEN (1u << 27)
//0: Backup interface clock disabled
//1: Backup interface clock enabled
//Bit 26 CAN2EN: CAN2 clock enable
//Set and cleared by software.
#define CAN2EN (1u << 26)
//0: CAN2 clock disabled
//1: CAN2 clock enabled
//Bit 25 CAN1EN: CAN1 clock enable
//Set and cleared by software.
#define CAN1EN (1u << 25)
//0: CAN1 clock disabled
//1: CAN1 clock enabled
//Bits 24:23 Reserved, must be kept at reset value.
//Bit 22 I2C2EN: I2C 2 clock enable
//Set and cleared by software.
#define I2C2EN (1u << 22)
//0: I2C 2 clock disabled
//1: I2C 2 clock enabled
//Bit 21 I2C1EN: I2C 1 clock enable
//Set and cleared by software.
#define I2C1EN (1u << 21)
//0: I2C 1 clock disabled
//1: I2C 1 clock enabled
//Bit 20 UART5EN: USART 5 clock enable
//Set and cleared by software.
#define UART5EN (1u << 20)
//0: USART 5 clock disabled
//1: USART 5 clock enabled
//Bit 19 UART4EN: USART 4 clock enable
//Set and cleared by software.
#define UART4EN (1u << 19)
//0: USART 4 clock disabled
//1: USART 4 clock enabled
//Bit 18 USART3EN: USART 3 clock enable
//Set and cleared by software.
#define USART3EN (1u << 18)
//0: USART 3 clock disabled
//1: USART 3 clock enabled
//Bit 17 USART2EN: USART 2 clock enable
//Set and cleared by software.
#define USART2EN (1u << 17)
//0: USART 2 clock disabled
//1: USART 2 clock enabled
//Bits 16 Reserved, must be kept at reset value.
//Bit 15 SPI3EN: SPI 3 clock enable
//Set and cleared by software.
#define SPI3EN (1u << 15)
//0: SPI 3 clock disabled
//1: SPI 3 clock enabled
//Bit 14 SPI2EN: SPI 2 clock enable
//Set and cleared by software.
#define SPI2EN (1u << 14)
//0: SPI 2 clock disabled
//1: SPI 2 clock enabled
//Bits 13:12 Reserved, must be kept at reset value.
//Bit 11 WWDGEN: Window watchdog clock enable
//Set and cleared by software.
#define WWDGEN (1u << 11)
//0: Window watchdog clock disabled
//1: Window watchdog clock enabled
//Bits 10:6 Reserved, must be kept at reset value.
//Bit 5 TIM7EN: Timer 7 clock enable
//Set and cleared by software.
#define TIM7EN (1u << 5)
//0: Timer 7 clock disabled
//1: Timer 7 clock enabled
//Bit 4 TIM6EN: Timer 6 clock enable
//Set and cleared by software.
#define TIM6EN (1u << 4)
//0: Timer 6 clock disabled
//1: Timer 6 clock enabled
//Bit 3 TIM5EN: Timer 5 clock enable
//Set and cleared by software.
#define TIM5EN (1u << 3)
//0: Timer 5 clock disabled
//1: Timer 5 clock enabled
//Bit 2 TIM4EN: Timer 4 clock enable
//Set and cleared by software.
#define TIM4EN (1u << 2)
//0: Timer 4 clock disabled
//1: Timer 4 clock enabled
//Bit 1 TIM3EN: Timer 3 clock enable
//Set and cleared by software.
#define TIM3EN (1u << 1)
//0: Timer 3 clock disabled
//1: Timer 3 clock enabled
//Bit 0 TIM2EN: Timer 2 clock enable
//Set and cleared by software.
#define TIM2EN (1u << 0)
//0: Timer 2 clock disabled
//1: Timer 2 clock enabled
//31 30 29 28 27 26 25


//Bit 31 LPWRRSTF: Low-power reset flag
#define LPWRRSTF (1u << 31)
//Set by hardware when a Low-power management reset occurs.
//Cleared by writing to the RMVF bit.
//0: No Low-power management reset occurred
//1: Low-power management reset occurred
//For further information on Low-power management reset, refer to Low-power management
//reset.
//Bit 30 WWDGRSTF: Window watchdog reset flag
#define WWDGRSTF (1u << 30)
//Set by hardware when a window watchdog reset occurs.
//Cleared by writing to the RMVF bit.
//0: No window watchdog reset occurred
//1: Window watchdog reset occurred
//Bit 29 IWDGRSTF: Independent watchdog reset flag
#define IWDGRSTF (1u <<  29)
//Set by hardware when an independent watchdog reset from VDD domain occurs.
//Cleared by writing to the RMVF bit.
//0: No watchdog reset occurred
//1: Watchdog reset occurred
//Bit 28 SFTRSTF: Software reset flag
#define SFTRSTF (1u << 28)
//Set by hardware when a software reset occurs.
//Cleared by writing to the RMVF bit.
//0: No software reset occurred
//1: Software reset occurred
//Bit 27 PORRSTF: POR/PDR reset flag
#define PORRSTF (1u << 27)
//Set by hardware when a POR/PDR reset occurs.
//Cleared by writing to the RMVF bit.
//0: No POR/PDR reset occurred
//1: POR/PDR reset occurred
//Low-, medium-, high- and XL-density reset and clock control (RCC) RM0008
//118/1093 Doc ID 13902 Rev 13
//Bit 26 PINRSTF: PIN reset flag
#define PINRSTF (1u << 26)
//Set by hardware when a reset from the NRST pin occurs.
//Cleared by writing to the RMVF bit.
//0: No reset from NRST pin occurred
//1: Reset from NRST pin occurred
//Bit 25 Reserved, must be kept at reset value.
//Bit 24 RMVF: Remove reset flag
#define RMVF (1u << 24)
//Set by software to clear the reset flags.
//0: No effect
//1: Clear the reset flags
//Bits 23:2 Reserved, must be kept at reset value.
//Bit 1 LSIRDY: Internal low-speed oscillator ready
#define LSIRDY (1u << 1)
//Set and cleared by hardware to indicate when the internal RC 40 kHz oscillator is stable.
//After the LSION bit is cleared, LSIRDY goes low after 3 internal RC 40 kHz oscillator clock
//cycles.
//0: Internal RC 40 kHz oscillator not ready
//1: Internal RC 40 kHz oscillator ready
//Bit 0 LSION: Internal low-speed oscillator enable
#define LSION (1u << 0)
//Set and cleared by software.
//0: Internal RC 40 kHz oscillator OFF
//1: Internal RC 40 kHz oscillator ON
//RM0008 Low-, medium-, high- and XL-density reset and clock control (RCC)
//

// end some RCC registers

//lint --flb

//lint ++flb

#define HSI_FREQ    8000000u         // internal oscillator
#define HSE_FREQ    4000000u         // external crystal
#ifdef HSE_FREQ
  #if ((HSE_FREQ < 4000000u) || (HSE_FREQ > 25000000u))
    #error
  #endif
#endif

#define SF(x,y)  ((x << 16) | y)
#define F(x)    (x & 0xffffu)
#define S1(x)    ((x >> 16) & 0xffffu)

#define MCO_NONE   0u   // 0xx: No clock
#define MCO_SYS    4u   // 100: System clock (SYSCLK) selected
#define MCO_HSI    5u   // 101: HSI clock selected
#define MCO_HSE    6u   // 110: HSE clock selected
#define MCO_PLL    7u   // 111: PLL clock divided by 2 selected
#define MCO_SHFT   24
#define MCO_SEL    MCO_NONE

// PLL config constants and multipliers
#define PLL_X2   SF(  0u,  2u)  // 0000: PLL input clock x 2
#define PLL_X3   SF(  1u,  3u)  // 0001: PLL input clock x 3
#define PLL_X4   SF(  2u,  4u)  // 0010: PLL input clock x 4
#define PLL_X5   SF(  3u,  5u)  // 0011: PLL input clock x 5
#define PLL_X6   SF(  4u,  6u)  // 0100: PLL input clock x 6
#define PLL_X7   SF(  5u,  7u)  // 0101: PLL input clock x 7
#define PLL_X8   SF(  6u,  8u)  // 0110: PLL input clock x 8
#define PLL_X9   SF(  7u,  9u)  // 0111: PLL input clock x 9
#define PLL_X10  SF(  8u, 10u)  // 1000: PLL input clock x 10
#define PLL_X11  SF(  9u, 11u)  // 1001: PLL input clock x 11
#define PLL_X12  SF( 10u, 12u)  // 1010: PLL input clock x 12
#define PLL_X13  SF( 11u, 13u)  // 1011: PLL input clock x 13
#define PLL_X14  SF( 12u, 14u)  // 1100: PLL input clock x 14
#define PLL_X15  SF( 13u, 15u)  // 1101: PLL input clock x 15
#define PLL_X16  SF( 14u, 16u)  // 1110: PLL input clock x 16
#define PLL_X16A SF( 15u, 16u)  // 1111: PLL input clock x 16
#define PLL_SHFT  18
#define PLL_FACT  F(PLL_SEL)
#define PLL_VAL   S1(PLL_SEL)

// HSE prescale inputs to PLL
#define HSE_D1  0u   // 0: HSE clock not divided
#define HSE_D2  1u   // 1: HSE clock divided by 2
#define HSE_SHFT 17

// PLL input selection
#define PLL_HSI_IN  0u      // 0: H0: HSI oscillator clock / 2 selected as PLL input clock
#define PLL_HSE_IN  1u      // 0: H1: HSE oscillator clock selected as PLL input clock
#define PLL_IN_SHFT 16

// system clock selection
#define SW_SEL_HSI  0u      //00: HSI selected as system clock
#define SW_SEL_HSE  1u      //01: HSE selected as system clock
#define SW_SEL_PLL  2u      //10: PLL selected as system clock
#define SW_SHFT     0

// peripheral clock 2 selection
#define APB2_D1     0u      //0xx: HCLK not divided
#define APB2_D2     4u      //100: HCLK divided by 2
#define APB2_D4     5u      //101: HCLK divided by 4
#define APB2_D8     6u      //110: HCLK divided by 8
#define APB2_D16    7u      //111: HCLK divided by 16
#define APB2_SHFT   11

// peripheral clock 1 selection
#define APB1_D1     0u      //00xx: HCLK not divided
#define APB1_D2     4u      //1100: HCLK divided by 2
#define APB1_D4     5u      //1101: HCLK divided by 4
#define APB1_D8     6u      //1110: HCLK divided by 8
#define APB1_D16    7u      //1111: HCLK divided by 16
#define APB1_SHFT   8

// internal ADC clock selection (based on APB2 selection)
#define ADCPS2      0u      //00: PCLK2 divided by 2
#define ADCPS4      1u      //01: PCLK2 divided by 4
#define ADCPS6      2u      //10: PCLK2 divided by 6
#define ADCPS8      3u      //11: PCLK2 divided by 8
#define ADCPS_SHFT  14

// system clock to core clock divider selection
#define APB_PD1     0u      //00xxx: SYSCLK not divided
#define APB_PD2     8u      //1000: SYSCLK divided by 2
#define APB_PD4     9u      //1001: SYSCLK divided by 4
#define APB_PD8     10u     //1010: SYSCLK divided by 8
#define APB_PD16    11u     //1011: SYSCLK divided by 16
#define APB_PD64    12u     //1100: SYSCLK divided by 64
#define APB_PD128   13u     //1101: SYSCLK divided by 128
#define APB_PD256   14u     //1110: SYSCLK divided by 256
#define APB_PD512   15u     //1111: SYSCLK divided by 512
#define APB_SHFT    4

#define CLK_DIV_4 (APB_PD4 << (u32)APB_SHFT)

// USB not present in 101xx - not used or checked
#define USB_PD1_5   0u      //0: PLL clock is divided by 1.5
#define USB_PD1     1u      //1: PLL clock is not divided0
#define USB_SHFT    22

#define PLL_MASK (0x3fu << 16)

#define PLL_SEL    PLL_X6
#define MCO        MCO_NONE    // 0 .. 7   // no MCO output
#define USBPRE     USB_PD1_5   // 0 .. 1   // USB not used
#define PLLMUL     PLL_VAL     // 0 .. 15  // PLL not used
#define PLLXTRPRE  HSE_D1      // 0 .. 1   // PLL not used
#define PLL_SRC    PLL_HSI_IN  // 0 .. 1   // PLL not used
#define ADC_PRE    ADCPS2      // 0 .. 3   // ADC not used
#define PPRE2      APB2_D1     // 0 .. 7   // peripheral clk2 = core clk
#define PPRE1      APB1_D1     // 0 .. 7   // peripheral clk2 = core clk
#if USE_HSE
#define SW         SW_SEL_HSE  // 0 .. 3   // system src = HSI
#define HPRE       APB_PD2     // 0 .. 15  // core clk = HSE / 4
#else
#define SW         SW_SEL_HSI  // 0 .. 3   // system src = HSI
#define HPRE       APB_PD4     // 0 .. 15  // core clk = HSE / 4
#endif

#define CLKWD (\
        (MCO       << MCO_SHFT)    |\
        (USBPRE    << USB_SHFT)    |\
        (PLL_VAL   << PLL_SHFT)    |\
        (PLLXTRPRE << HSE_SHFT)    |\
        (PLL_SRC   << PLL_IN_SHFT) |\
        (ADC_PRE   << ADCPS_SHFT)  |\
        (PPRE2     << APB2_SHFT)   |\
        (PPRE1     << APB1_SHFT)   |\
        (HPRE      << APB_SHFT)    |\
        (SW        << SW_SHFT))

#if PLLXTRPRE == HSE_D1
#define PREDIVIN HSE_FREQ
#elif PLLXTRPRE == HSE_D2
#define PREDIVIN (HSE_FREQ / 2)
#else
#error  // invalid HSE to PLL selection
#endif

#if PLL_SRC == PLL_HSI_IN
#define PLL_INPUT (HSI_FREQ / 2u)
#elif PLL_SRC == PLL_HSE_IN
#define PLL_INPUT PREDIVIN
#else
#error  // invalid PLL input selection
#endif

#if SW == SW_SEL_HSI
#define SYSCLK_FREQ HSI_FREQ
#elif SW == SW_SEL_HSE
#define SYSCLK_FREQ HSE_FREQ
#elif SW == SW_SEL_PLL
#define SYSCLK_FREQ (PLL_INPUT * PLL_FACT)
#else
#error  // invalid SysClock selection
#endif

#if SYSCLK_FREQ > 72000000u
#error  // SYSCLK may not be > 72 mhz
#endif

#if HPRE < APB_PD2
#define CORE_CLK SYSCLK_FREQ
#elif HPRE <= APB_PD512
#define CORE_CLK (SYSCLK_FREQ / (2u << (HPRE - APB_PD2)))
#else
#error // invalid core clock divider selection
#endif

#if PPRE1 < APB1_D2
#define  PCLK1 CORE_CLK
#elif PPRE2 <= APB1_D16
#define  PCLK1 (CORE_CLK / (2u << (PPRE1 - APB1_D2)))
#else
#error  // invalid PCLK1 divider selection
#endif

#if PPRE2 < APB2_D2
#define  PCLK2 CORE_CLK
#elif PPRE2 <= APB2_D16
#define  PCLK2 (CORE_CLK / (2u << (PPRE2 - APB2_D2)))
#else
#error  // invalid PCLK2 divider selection
#endif
//lint --flb

CONST_ASSERT(CORE_FREQ  == CORE_CLK);
CONST_ASSERT(PCLK1_FREQ == PCLK1);
CONST_ASSERT(PCLK2_FREQ == PCLK2);

#define NEED_PLLON   (SW == SW_SEL_PLL)
#define NEED_HSE_ON ((SW == SW_SEL_HSE) || (NEED_PLLON && (PLL_SRC == PLL_HSE_IN)))
#define NEED_HSION  ((SW == SW_SEL_HSI) || (NEED_PLLON && (PLL_SRC == PLL_HSI_IN)))
//lint ++flb
#if NEED_PLLON
#define     PLL_CR      (1u << 24)
#else
#define     PLL_CR      0u
#endif
#if NEED_HSE_ON
#define HSE_CR      (1u << 16)
#else
#define HSE_CR      0u
#endif
#define CR_VALUE    (PLL_CR | HSE_CR)
#define CR_READY    (CR_VALUE << 1)
//lint --flb

#if 0 //ndef NDEBUG   // a way to see the derived frequencies, etc.
u32 syslck  = SYSCLK_FREQ;
u32 coreclk = CORE_CLK;
u32 pclk1   = PCLK1;
u32 pclk2   = PCLK2;
u32 clkwd   = CLKWD;
bool_t tst1 = NEED_PLLON;
bool_t tst2 = NEED_HSE_ON;
u32 tsta    = CR_VALUE;
u32 pll1    = PLL_FACT;
u32 pll2    = PLL_VAL;
#endif

#define HSI_ON          (1u << 0)
#define HSI_RDY         (1u << 1)
#define HSI_TRIM        (0x10u << 3)
#define ALL_PERIPH2     ((0x7u << 19) | 0xfffdu)    // reset bits for all peripherals
#define ALL_PERIPH1     0x3afec9ffu                 // reset bits for all peripherals
#define CIR_CLR         0x009F0000u

#define RETURN_TO_RESET 1

#undef return

/** \brief  Perform low level initialize.  First, cut the clock frequency.
    Second reset all peripherals and then power on the ones we need.
    Third relay to GPIO services to configure the ports.
    Called from first instruction in cmain.s
*/
s32 __low_level_init(void)
{
    return 1;
}

#if USE_INTERNAL_WATCHDOG == OPTION_ON
/** No longer need timer5 - disable it
*/
void rcc_DisableTimer5(void)
{
}

/** Test if reset was caused by IWDG timout

    \retune tru if IWDG timout, otherwise false
*/
bool_t rcc_WDTimeout(void)
{
    return 0;
}
#endif

/** \brief Enable the HSI oscillator - called by the flash driver because
    HSI is needed for flash erase/write
*/
void rcc_enable_HSI(void)
{
}

/** \brief Configure the system clocks the way we want to run

*/
void stm_SystemInit(void) //was SystemInit
{
}

void bios_SetVPBDividier_to2(bool_t divval)
{
    UNUSED_OK(divval);
}

// end of source
