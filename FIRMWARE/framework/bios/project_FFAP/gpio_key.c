/**
Copyright 2004-2005 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file key.c
    \brief BIOS routines for support of the keyboard buttons
    keyboard/display reconnect is also supported

    CPU: Any (with standard C compiler)

    OWNER: EP
    \n RCS: $Id$
    Created: 9/27/04 Author:  EP
*/


#include "mnwrap.h"
#include "stmcommon.h"
// #include "io.h"
#include "oswrap.h"
#include "config.h"
#include "mnassert.h"
#include "faultpublic.h"
#include "key.h"
#include "gpio.h"
// #include "spi.h"

#define NO_I2C_CLOCK_PULLUP      0

#define AFIO_BASE           (APB2PERIPH_BASE + 0x0000)
#define AFIO                HARDWARE(AFIO_TypeDef *, AFIO_BASE)

//lint ++flb
#define GPIOA_BASE          (APB2PERIPH_BASE + 0x0800u)
#define GPIOB_BASE          (APB2PERIPH_BASE + 0x0C00u)
#define GPIOC_BASE          (APB2PERIPH_BASE + 0x1000u)
#define GPIOD_BASE          (APB2PERIPH_BASE + 0x1400u)
#define GPIOE_BASE          (APB2PERIPH_BASE + 0x1800u)
#define GPIOF_BASE          (APB2PERIPH_BASE + 0x1C00u)
#define GPIOG_BASE          (APB2PERIPH_BASE + 0x2000u)

#define GPIOA               HARDWARE(GPIO_TypeDef *, GPIOA_BASE)
#define GPIOB               HARDWARE(GPIO_TypeDef *, GPIOB_BASE)
#define GPIOC               HARDWARE(GPIO_TypeDef *, GPIOC_BASE)
#define GPIOD               HARDWARE(GPIO_TypeDef *, GPIOD_BASE)
#define GPIOE               HARDWARE(GPIO_TypeDef *, GPIOE_BASE)
#define GPIOF               HARDWARE(GPIO_TypeDef *, GPIOF_BASE)
#define GPIOG               HARDWARE(GPIO_TypeDef *, GPIOG_BASE)

typedef struct
{
    IO_REG32(CRL);
    IO_REG32(CRH);
    IO_REG32(IDR);
    IO_REG32(ODR);
    IO_REG32(BSRR);
    IO_REG32(BRR);
    IO_REG32(LCKR);
} GPIO_TypeDef;

typedef struct
{
    IO_REG32(EVCR);
    IO_REG32(MAPR);
    IO_REG32(EXTICR[4]);
    u32 RESERVED0;
    IO_REG32(MAPR2);
} AFIO_TypeDef;

#define TIM5CH4_IREMAP (1u << 16)       // used to measure LSI frequency


typedef struct
{
    IO_REG32(IMR);
    IO_REG32(EMR);
    IO_REG32(RTSR);
    IO_REG32(FTSR);
    IO_REG32(SWIER);
    IO_REG32(PR);
} EXTI_TypeDef;

#define EXTI_BASE               (APB2PERIPH_BASE + 0x0400u)
#define EXTI                    HARDWARE(EXTI_TypeDef *, EXTI_BASE)
#define EXTI_IRQn                  40


//lint --flb

#define NM(x,y,n)   CAT(x,CAT(n,y))
#define S(x,y,n)    ((NM(x,y,n) & 0xfu) << ((n) * PF_SHFT))

//lint -emacro( {912,572}, M)   // excessive shift is OK
#define M(x,y,n,i)  (((NM(x,y,n) >> 4) & 1u) << (n + i))

#define GRP(x,y)        (S(x,y,0)   | S(x,y,1)   | S(x,y,2)   | S(x,y,3)   | S(x,y,4)   | S(x,y,5)   | S(x,y,6)   | S(x,y,7))
#define BTTVAL(x,y,i)   (M(x,y,0,i) | M(x,y,1,i) | M(x,y,2,i) | M(x,y,3,i) | M(x,y,4,i) | M(x,y,5,i) | M(x,y,6,i) | M(x,y,7,i))
#define VAL(x)          (BTTVAL(x,L,0) | BTTVAL(x,H,8))

//lint ++flb
// In input mode (MODE[1:0] == 00):
#define INA (0x0u << 2) //: Analog mode
#define INF (0x1u << 2) // Floating input (reset state)
#define INx (0x2u << 2) // Input with pull-up / pull-down

// In output mode (MODE[1:0] > 00):
#define OPP (0x0u << 2) // General purpose output push-pull
#define OOD (0x1u << 2) // General purpose output Open-drain
#define APP (0x2u << 2) // Alternate function output Push-pull
#define AOD (0x3u << 2) // Alternate function output Open-drain

#define INP 0x0u // Input mode (reset state)
#define O10 0x1u // Output mode, max speed 10 MHz.
#define O02 0x2u // Output mode, max speed 2 MHz.
#define O50 0x3u // Output mode, max speed 50 MHz.

#define ST1 (1u << 4)
#define ST0 (0u << 4)

#define ALT_PP   (O02 | APP | ST0)  // Alternate function output push-pull
#define ALT_PUP  (O02 | APP | ST1)  // Alternate function output push-pull with option to switch to Input with Pull-up
#define ALT_OD   (O02 | AOD | ST0)  // Alternate function output open drain
#define INPUT    (INP | INF | ST0)  // Input
#define INPUT_PUP (INP | INx | ST1)  // Input with Internal Pull-ups enabled
#define GP_PP_0  (O02 | OPP | ST0)  // GPIO ouput - initially 0
#define GP_PP_1  (O02 | OPP | ST1)  // GPIO ouput - initially 1
#define DEDIC    (INP | INx | ST0)  // dedicated function (JTAG) with PULL DOWN
#define DEDIC_PUP (INP | INx | ST1)  // dedicated function (JTAG) with PULL UP
#define GP_OD_0  (O02 | OOD | ST0)  // GPIO open-drain initial value '1'
#define GP_OD_1  (O02 | OOD | ST1)	// GPIO open-drain initial value '1'

#if NO_I2C_CLOCK_PULLUP
#define I2CLK   ALT_PP       // alt push-pull
#define GPCLK   GP_PP_0      // GPIO push-pull
#else
#define I2CLK   ALT_OD       // alt ppen-drain
#define GPCLK   GP_OD_0      // GPIO open-drain
#endif
#define I2DAT   ALT_OD       // alt open-drain
#define GPDAT   GP_OD_0      // GPIO open drain
//lint --flb

#define PF_SHFT     4

// keep this list in sync with the enum in stmcommon.h
//      Hand    Val        Port Schematic Name
#define PA0L    ALT_PP  // PA 0 DO_1
#define PA1L    ALT_PP  // PA 1 DO_2
#define PA2L    ALT_PP  // PA 2 APP2FF_APP
#define PA3L    INPUT   // PA 3 FF2APP_APP
#define PA4L    GP_PP_0 // PA 4 WDT_APP
#define PA5L    ALT_PP  // PA 5 SCLK1
#define PA6L    INPUT   // PA 6 MISO1
#define PA7L    GP_PP_0 // PA 7 not used
#define PA0H    GP_PP_0 // PA 8 not used
#define PA1H    ALT_PP  // PA 9 ISP_TX_APP
#define PA2H    INPUT   // PA10 ISP_RX_APP
#define PA3H    GP_PP_0 // PA11 not used
#define PA4H    GP_PP_1 // PA12 /FRAM_WR
#define PA5H    DEDIC_PUP   // PA13 JTMS_APP
#define PA6H    DEDIC   // PA14 JTCK_APP
#define PA7H    DEDIC_PUP   // PA15 JTDI_APP
   //
#define PB0L    ALT_PP  // PB 0 PWM_IP_0
#define PB1L    ALT_PP  // PB 1 PWM_IP_1
#define PB2L    INPUT   // PB 2 PULLDOWN
#define PB3L    DEDIC_PUP   // PB 3 JTDO_APP
#define PB4L    DEDIC_PUP   // PB 4 /JRES_APP
#define PB5L    INPUT   // PB 5 WP_APP
#define PB6L    I2CLK   // PB 6 SCL
#define PB7L    I2DAT   // PB 7 SDA
#define PB0H    INPUT   // PB 8 SI_PB
#define PB1H    GP_PP_0 // PB 9 RS_LCD
#define PB2H    INPUT   // PB10 /BOOTLOAD_FF
#define PB3H    GP_PP_0 // PB11 DI_SUP
#define PB4H    INPUT   // PB12 DI
#define PB5H    ALT_PUP     // PB13 SCL_LCD
#define PB6H    GP_PP_1     // PB14 /CSB_LCD
#define PB7H    ALT_PUP     // PB15 SO_LCD
//
#define PC0L    GP_PP_0 // PC 0 ADC_MUX0
#define PC1L    GP_PP_0 // PC 1 ADC_MUX1
#define PC2L    GP_PP_0 // PC 2 ADC_MUX2
#define PC3L    GP_PP_0 // PC 3 not used
#define PC4L    GP_PP_1 // PC 4 EN_IP
#define PC5L    GP_PP_1 // PC 5 CS_AD
#define PC6L    GP_PP_0 // PC 6 PRESS_MUX0
#define PC7L    GP_PP_0 // PC 7 PRESS_MUX1
#define PC0H    GP_PP_1 // PC 8 /P_POS_REM
#define PC1H    GP_PP_1 // PC 9 /P_POS_INT
#define PC2H    GP_PP_0 // not used
#define PC3H    INPUT_PUP   // PC11 NRESET_FF_EXT
#define PC4H    ALT_PP  // PC12 DBG_TX_APP
#define PC5H    INPUT_PUP   // PC13 NRESET_FF
#define PC6H    GP_PP_1 // PC14 PCON_SW
#define PC7H    GP_PP_1 // PC15 /P_PRESS

//lint ++flb
#define PD0L    INPUT_PUP   // PD 0 OSC_INOSC_IN
#define PD1L    GP_PP_0 // PD 1 OSC_OUT
#define PD2L    INPUT_PUP   // PD 2 DEBUG_RX_APP
// Internal Pins
#define PD3L    INPUT_PUP // PD 3 not used
#define PD4L    INPUT_PUP // PD 4 not used
#define PD5L    INPUT_PUP // PD 5 not used
#define PD6L    INPUT_PUP // PD 6 not used
#define PD7L    INPUT_PUP // PD 7 not used
#define PD0H    INPUT_PUP // PD 8 not used
#define PD1H    INPUT_PUP // PD 9 not used
#define PD2H    INPUT_PUP // PD 10 not used
#define PD3H    INPUT_PUP // PD 11 not used
#define PD4H    INPUT_PUP // PD 12 not used
#define PD5H    INPUT_PUP // PD 13 not used
#define PD6H    INPUT_PUP // PD 14 not used
#define PD7H    INPUT_PUP // PD 15 not used

// #define PE0L    INPUT_PUP // PE  0 not used
// #define PE1L    INPUT_PUP // PE  1 not used
// #define PE2L    INPUT_PUP // PE  2 not used
// #define PE3L    INPUT_PUP // PE  3 not used
// #define PE4L    INPUT_PUP // PE  4 not used
// #define PE5L    INPUT_PUP // PE  5 not used
// #define PE6L    INPUT_PUP // PE  6 not used
// #define PE7L    INPUT_PUP // PE  7 not used
// #define PE0H    INPUT_PUP // PE  8 not used
// #define PE1H    INPUT_PUP // PE  9 not used
// #define PE2H    INPUT_PUP // PE 10 not used
// #define PE3H    INPUT_PUP // PE 11 not used
// #define PE4H    INPUT_PUP // PE 12 not used
// #define PE5H    INPUT_PUP // PE 13 not used
// #define PE6H    INPUT_PUP // PE 14 not used
// #define PE7H    INPUT_PUP // PE 15 not used
//
// #define PF0L    INPUT_PUP // PF  0 not used
// #define PF1L    INPUT_PUP // PF  1 not used
// #define PF2L    INPUT_PUP // PF  2 not used
// #define PF3L    INPUT_PUP // PF  3 not used
// #define PF4L    INPUT_PUP // PF  4 not used
// #define PF5L    INPUT_PUP // PF  5 not used
// #define PF6L    INPUT_PUP // PF  6 not used
// #define PF7L    INPUT_PUP // PF  7 not used
// #define PF0H    INPUT_PUP // PF  8 not used
// #define PF1H    INPUT_PUP // PF  9 not used
// #define PF2H    INPUT_PUP // PF 10 not used
// #define PF3H    INPUT_PUP // PF 11 not used
// #define PF4H    INPUT_PUP // PF 12 not used
// #define PF5H    INPUT_PUP // PF 13 not used
// #define PF6H    INPUT_PUP // PF 14 not used
// #define PF7H    INPUT_PUP // PF 15 not used
//
// #define PG0L    INPUT_PUP // PG  0 not used
// #define PG1L    INPUT_PUP // PG  1 not used
// #define PG2L    INPUT_PUP // PG  2 not used
// #define PG3L    INPUT_PUP // PG  3 not used
// #define PG4L    INPUT_PUP // PG  4 not used
// #define PG5L    INPUT_PUP // PG  5 not used
// #define PG6L    INPUT_PUP // PG  6 not used
// #define PG7L    INPUT_PUP // PG  7 not used
// #define PG0H    INPUT_PUP // PG  8 not used
// #define PG1H    INPUT_PUP // PG  9 not used
// #define PG2H    INPUT_PUP // PG 10 not used
// #define PG3H    INPUT_PUP // PG 11 not used
// #define PG4H    INPUT_PUP // PG 12 not used
// #define PG5H    INPUT_PUP // PG 13 not used
// #define PG6H    INPUT_PUP // PG 14 not used
// #define PG7H    INPUT_PUP // PG 15 not used

//lint --flb

// #if DEBUG_GPIO
// CONST_ASSERT(GRP(PA,L) == 0x24a24aaa);
// CONST_ASSERT(GRP(PA,H) == 0x888224a2);
// CONST_ASSERT(GRP(PB,L) == 0xee4884aa);
// CONST_ASSERT(GRP(PB,H) == 0xa2a42424);
// CONST_ASSERT(GRP(PC,L) == 0x22222222);
// CONST_ASSERT(GRP(PC,H) == 0x224a4222);
// CONST_ASSERT(GRP(PD,L) == 0x22222422);
// CONST_ASSERT(VAL(PA) == 0x1000);
// CONST_ASSERT(VAL(PB) == 0x4000);
// CONST_ASSERT(VAL(PC) == 0xc330);
// #endif

#if !defined(NDEBUG) && !defined(_lint)
u32 pal = GRP(PA,L);
u32 pah = GRP(PA,H);
u32 pbl = GRP(PB,L);
u32 pbh = GRP(PB,H);
u32 pcl = GRP(PC,L);
u32 pch = GRP(PC,H);

u32 pad = VAL(PA);
u32 pbd = VAL(PB);
u32 pcd = VAL(PC);
#endif

static GPIO_TypeDef * const gad[] =
{
    GPIOA,
    GPIOB,
    GPIOC,
    GPIOD,
    GPIOE,
    GPIOF,
    GPIOG
};


//---------------------------------------------------------------------------
// This function is for use in GPIO file ONLY!

extern void spi_SetEnableDisableDev2(bool_t disable);

//---------------------------------------------------------------------------
// single bit operations (port encoded in pin)

/** \brief Given an enumerated pin number (see stmcommon.h), decode the Port #
    and bit number and set the requested bit.

    \param pin - an enumerated value representing the pin #.  pins 0..15 are in PORTA, 16..31 in PORTB, etc.
*/
MN_INLINE void gpio_BitSet(u32 pin)
{
    u8          r = (u8)(pin / 16u);
    gad[r]->BSRR  = (1u << (pin & 0x0Fu));
}

/** \brief Given an enumerated pin number (see stmcommon.h), decode the Port #
    and bit number and reset the requested bit.

    \param pin - an enumerated value representing the pin #.  pins 0..15 are in PORTA, 16..31 in PORTB, etc.
*/
MN_INLINE void gpio_BitReset(u32 pin)
{
    u8          r = (u8)(pin / 16u);
    gad[r]->BRR   = (1u << (pin & 0x0Fu));
}

/** \brief Given an enumerated pin number (see stmcommon.h), decode the Port #
    and bit number and return the currtent state of the pin

    \param pin - an enumerated value representing the pin #.  pins 0..15 are in PORTA, 16..31 in PORTB, etc.
*/
MN_INLINE bool_t gpio_BitState(u32 pin)
{
    u8          r = (u8)(pin / 16u);
    return (gad[r]->IDR & (1u << (pin & 0x0Fu))) != 0u;
}

//---------------------------------------------------------------------------
/** \brief Select External A/D Converter

    \param  - None.
    \return - Nothing
*/
void    gpio_SelectExtAD(void)
{
    gpio_BitReset(CS_AD);
}

/** \brief De-Select External A/D Converter

    \param  - None.
    \return - Nothing
*/
void    gpio_DeSelectExtAD(void)
{
    gpio_BitSet(CS_AD);
}

/** \brief Drive the MUX and Power bits
    Set or Reset 'mask' bits a bit in PortC.  See STM user manual
    for how GPIOx->BSRR works.

    \param mask = a 32-bit (16 set, 16 reset) mask filed sent to the BSRR
*/
void    gpio_SelectMUXPWRbits(u32   bmask)
{
    GPIOC->BSRR = bmask;
}

//---------------------------------------------------------------------------
// Bypass Switch control
void gpio_SetBypassSwitch(bool_t value)
{
    //Note negative logic
    if (value)
    {   /* PWM D/A to U/I converter */
        gpio_BitReset(PCON_SW);
    }
    else
    {   /* PWM D/A to pressure controller */
        gpio_BitSet(PCON_SW);
    }
}

/** \brief Read I/P closed loop bypass
    \return true = bypas spressure controller , false = pressure controller active
*/
bool_t gpio_ReadBypassSwitch(void)
{
    //Note negative logic
    return !gpio_BitState(PCON_SW);
}

//----------------------------------------------------------------------------------------------------------------------
/** \brief Set WD signal HI
    \param  - None.
    \return - Nothing
*/
void gpio_SetWatchDogHI(void)
{
    gpio_BitSet(WDT_APP);
}

/** \brief Set WD signal LO
    \param  - None.
    \return - Nothing
*/
void gpio_SetWatchDogLO(void)
{
    gpio_BitReset(WDT_APP);
}

//----------------------------------------------------------------------------------------------------------------------
// FRAM Write Enable and Disable code

void FRAM_Write_Enable(void)
{
    gpio_BitReset(FRAM_WR);
}

void FRAM_Write_Disable(void)
{
    gpio_BitSet(FRAM_WR);
}

//----------------------------------------------------------------------------------------------------------------------
// LCD and KBD controls

/** \brief Select External LCD controller

    \param  - None.
    \return - Nothing
*/
void    lcd_SelectExtLCD(void)
{
    gpio_BitReset(CSB_LCD);             /* select LCD controller */
}

/** \brief De-Select External LCD controller

    \param  - None.
    \return - Nothing
*/
void    lcd_DeSelectExtLCD(void)
{
    gpio_BitSet(CSB_LCD);               /* deselect LCD controller and command mode */
}

/** \brief Set Command mode for External LCD

    \param  - None.
    \return - Nothing
*/
void    lcd_SetCommandMode_ExtLCD(void)
{
    gpio_BitReset(RS_LCD);              /* Set Command mode for External LCD */
}

/** \brief Set Data mode for External LCD

    \param  - None.
    \return - Nothing
*/
void    lcd_SetDataMode_ExtLCD(void)
{
    gpio_BitSet(RS_LCD);                /* Set Data mode for external LCD */
}

/** \brief Check if LCD is selected (means - active)
    \param  - None.
    \return - bool_t - True - selected, False - not so.
*/
static bool_t    lcd_IsLCDSelected(void)
{
    return !gpio_BitState(CSB_LCD);
}

//----------------------------------------------------------------------------------------------------------------------
/** \brief Set External Shift Register Clock signal HI
    Used in KEY.C driver

    \param  - None.
    \return - Nothing
*/
static void    lcd_SetKeyShift_HI(void)
{
    gpio_BitSet(RS_LCD);                /* Set Data mode for external LCD */
}

/** \brief Set External Shift Register Clock signal LO
    Used in KEY.C driver

    \param  - None.
    \return - Nothing
*/
static void    lcd_SetKeyShift_LO(void)
{
    gpio_BitReset(RS_LCD);              /* Set Command mode for External LCD */
}

//----------------------------------------------------------------------------------------------------------------------

/** \brief Start DI Power
    \param  - None.
    \return - Nothing
*/
static void    key_StartDIPower(void)
{
    gpio_BitSet(DI_SUP);
}

/** \brief Stop DI Power
    \param  - None.
    \return - Nothing
*/
static void    key_StopDIPower(void)
{
    gpio_BitReset(DI_SUP);              /* Set Command mode for External LCD */
}

#if 0
/** \brief Return State of DI pin
    \param  - None.
    \return - true - ACtive, false - passive
*/
static bool_t  key_GetDIState(void)
{
    return gpio_BitState(DI_SUP);
}
#endif

//----------------------------------------------------------------------------------------------------------------------

bool_t gpio_ReadWriteProtectInput(void)
{
    return !gpio_BitState(WP_APP);
}

//----------------------------------------------------------------------------------------------------------------------
// multi-bit operations on a single port

#if 0

/** \brief Set or Reset 'mask' bits a bit in PortC.  See STM user manual
    for how GPIOx->BSRR works.

    \param mask = a 32-bit (16 set, 16 reset) mask filed sent to the BSRR
*/
void gpio_SetPortCBits(u32 mask)
{
    GPIOC->BSRR = mask;
}

/** \brief Set 0 or more bits in Port B according to the mask

    \param mask - the bit pattern to set
*/
static void gpio_PortBMaskSet(u32 mask)
{
    GPIOB->BSRR = mask;
}

/** \brief Reset 0 or more bits in Port B according to the mask

    \param mask - the bit pattern to reset
*/
static void gpio_PortBMaskReset(u32 mask)
{
    GPIOB->BRR = mask;
}

/** \brief And the current state of the Port B pins with the provided mask.
    Return true if any of the masked bits is set.

    \param mask - a 16-bit mask containing the bit pattern to test
*/
static bool_t gpio_PortBBitState(u32 mask)
{
    return (GPIOB->IDR & mask) != 0u;
}
#endif

/** \brief Enable or disable the I/P output based on the passed parameter.

    \param on - a boolean indicatiing whether the I/P is to be enabled (false) or disabled (true)
*/
void bios_SetEnableIP(bool_t on)
{
    if (on)
    {
        gpio_BitSet(EN_IP);
    }
    else
    {
        gpio_BitReset(EN_IP);
    }
}

/** \brief Intiialize all GPIO port bits to  a default state.  Those that use alternate
    functions such as SPI, U2c, etc. are also configured here.  Not tha there are no
    remaps.
*/
void gpio_PortInit(void)
{
    GPIOA->ODR = VAL(PA);
    GPIOB->ODR = VAL(PB);
    GPIOC->ODR = VAL(PC);
    GPIOD->ODR = VAL(PD);

    // GPIOE->ODR = VAL(PE);
    // GPIOF->ODR = VAL(PF);
    // GPIOG->ODR = VAL(PG);

    GPIOA->CRL = GRP(PA,L);
    GPIOA->CRH = GRP(PA,H);
    GPIOB->CRL = GRP(PB,L);
    GPIOB->CRH = GRP(PB,H);
    GPIOC->CRL = GRP(PC,L);
    GPIOC->CRH = GRP(PC,H);

    GPIOD->CRL = GRP(PD,L);
    GPIOD->CRH = GRP(PD,H);

    // GPIOE->CRL = GRP(PE,L);
    // GPIOE->CRH = GRP(PE,H);
    // GPIOF->CRL = GRP(PF,L);
    // GPIOF->CRH = GRP(PF,H);
    // GPIOG->CRL = GRP(PG,L);
    // GPIOG->CRH = GRP(PG,H);

#if USE_INTERNAL_WATCHDOG == OPTION_ON
    AFIO->MAPR |= TIM5CH4_IREMAP;
#endif
    // shuld ports d,e,f,g be set as outputs to avoid internal floats?
//    GPIOD->CRL =
//    GPIOD->CRH =
}

// special gpio operations

#if 0

/** \brief Change the mode of the I2C1 SDA pin to GPIO or Alternate according to the
    passed parameter.  This is used by the I2C driver to free the controller if it
    was reset during a read operation (e.g. FRAM read).  Note that gpio_PortInit set
    the ODR for pin SDA to 0.  So toggling back and forth between Alternate and GPIO
    will issue I2C clocks unless something is holding the SDA line low.

    \param which - true the pin is set to GPIO, false the pin is set to Alternate mode.
*/
void gpio_SetI2cClk(bool_t which)
{
    u32 val = which ? GPCLK : I2CLK;
    u32 sft = (u32)PF_SHFT * ((u32)SCL & 0x7u);

    GPIOB->CRL &= ~(0xfu << sft);
    GPIOB->CRL |= val << sft;
}

/** \brief return the current input state of GPIOB

    \return the current value of GPIOB->IDR
*/
u32 gpio_GetI2cDat(void)
{
    return GPIOB->IDR;
}
#endif

//---------------------------------------------------------------------------

typedef u32 rawkbd_t;

/* Type with all Keyboard and DI related variables */
typedef struct
{
    rawkbd_t key_debounced;
    rawkbd_t key_debounced_l;
    rawkbd_t key_latch;
    rawkbd_t key_up_transition;
    rawkbd_t key_down_transition;
    bool_t keyboard_locked ; /* 1=locked 0=keyboard usable */
    bool_t di_debounced; /* sampled and debounced state of DI pin. */
    bool_t di_latch;     /* latch to sample T-1 DI input state */
//    bool_t keyboard_connected; /* True if keyboard is recognized */
                           /* False if no connected keyboard is recognized */
    bool_t keyboard_reconnected ; /* TRUE= not connected to connected transitition detected */
                              /* FALSE= keyboard connected or disconnected */
    u8 latch_count;
}   KeyData_t;

/* Variable with all Keyboard and DI related variables */
static KeyData_t KeyData;

#define KBD_NUM_TEST_INPUTS 1U //up to 4, depending on paranoia level, but we agreed on at least 1.
#define KBD_NUM_BUTTONS 4U //3 functional and connected
#define KBD_NUM_READS (KBD_NUM_BUTTONS + KBD_NUM_TEST_INPUTS)
#define KBD_PIN ((u32)SI_PB & 15u)

//---------------------------------------------------------------------------

#define KBD_DEBOUNCE_COUNT (3U)     // will yield 4=(3+1) matches

//While at it: we want to be able to shift all 8 bits comfortably; hence the shift direction
CONST_ASSERT(KBD_PIN > 7u);

#define KBD_SHIFT_IN(keymask) ( ((keymask) >> 1) | (GPIOB->IDR &  BIT(SI_PB)))

// After everything is shifted in:
#define KBD_TEST_MASK               (((1u << KBD_NUM_TEST_INPUTS) - 1u) << 4)
#define KBD_MASK_KBDPRESENT         (1u << 3)
#define KBD_MASK_PUSHBUTTON_PLUS    (1u << 2)
#define KBD_MASK_PUSHBUTTON_STAR    (1u << 1)
#define KBD_MASK_PUSHBUTTON_MINUS   (1u << 0)

#define KBD_TEST_VALUE KBD_TEST_MASK //in active-low form, non-existent buttons are not pressed (pulled up)
#define RSTATE (KBD_MASK_PUSHBUTTON_PLUS | KBD_MASK_PUSHBUTTON_STAR | KBD_MASK_PUSHBUTTON_MINUS | KBD_TEST_MASK)  /* assumed state is present, not pressed, negative logic */

/**
    \brief Initialize the keyboard driver

    \param none
*/
void bios_InitKeyboardDriver(void)
{
    MN_ENTER_CRITICAL();
        KeyData.key_debounced        = RSTATE;  /* assumed state is present, not pressed, negative logic */
        KeyData.key_debounced_l      = RSTATE;
        KeyData.key_latch            = RSTATE;   /* assumed level is high */
        KeyData.key_up_transition    = 0U; /* assume no tranitions are recognized, positive logic */
        KeyData.key_down_transition  = 0U;
        KeyData.keyboard_locked      = true;
        KeyData.di_latch             = true; /* assumed level is high */
        KeyData.di_debounced         = true ;	/* assumed state is open */
        //    KeyData.keyboard_connected   = true; /* assume a connceted keyboard */
        KeyData.keyboard_reconnected = false; /* "keypad connected" flag */
    MN_EXIT_CRITICAL();

    return;
}

/** \brief A mandatory mopup interface
This version monitors and switches between LCD nad ISP comm
*/
void key_Mopup(void)
{
    MN_ENTER_CRITICAL();
        if(KeyData.keyboard_reconnected)
        {
            //Enable LCD SPI and ISP connection monitor; switch to LCD comm
            GPIOB->CRH = GRP(PB,H);
            gpio_SetEXTI();
            spi_SetEnableDisableDev2(false);             // tell SPI to relinquish the GPIO pins
            KeyData.keyboard_locked = false;
        }
        else
        {
            //Disable LCD SPI and ISP connection monitor; switch to ISP comm
            EXTI_IRQHandler();
        }
    MN_EXIT_CRITICAL();
}


static void gpio_SetSPI0Clk(bool_t which);


/**
    \brief poll the keyboard - called by A/D convert - but only if the LCD is NOT being updated

    \param none
    \return true -- The KBD and DI scan has been performed; false -- NO Scan
*/
void  bios_HandleDigitalInput(void)
{
    u32 key_temp;
    u8_least keybit;

    if (lcd_IsLCDSelected())
    {   // Check if LCD is busy - exit if so.
        return;
    }

    // turn on DI_SUP and set CSB_LCD low
    key_StartDIPower();                /* at first start power for DI (charge DI input capacitor) */
    lcd_SelectExtLCD();            /* S0=high(inverter) to shift out data bits */
    // GPIOB->BSRR = BIT(DI_SUP) | (BIT(CSB_LCD) << 16);

    key_temp = 0U;
    // Shift in all bits (active low)
    for(keybit = KBD_NUM_READS; keybit > 0u; --keybit)
    {
        lcd_SetKeyShift_LO();          /* clk low->high (inverter) first data bit is avialable */
        // GPIOB->BRR = BIT(RS_LCD);  /* clk low->high (inverter) first data bit is avialable */
        /* So=low(inverter) for shifing out data */
        lcd_SetKeyShift_HI();
        lcd_DeSelectExtLCD();              // Set BOTH Select and RS HIGH
        // GPIOB->BSRR = BIT(RS_LCD) | BIT(CSB_LCD);

        (void)GPIOB->IDR;

        key_temp = KBD_SHIFT_IN(key_temp);
    }
    key_temp >>= (KBD_PIN - KBD_NUM_READS) + 1u;
    //Include connectedness in the debouncing stuff

    MN_ENTER_CRITICAL();
        if (key_temp == KeyData.key_latch)
        {
            if(KeyData.latch_count == 0)
            {
                //So, this is the new (or the old) stable value;
                if((key_temp & KBD_TEST_MASK) != KBD_TEST_VALUE)
                {
                    if(!KeyData.keyboard_locked)
                    {
                        //ISP cable is not in
                        //keyboard fault: log it but continue assuming no key pressed
                        error_SetFault(FAULT_KEYPAD);
                        bios_InitKeyboardDriver();
                        //keep it false: KeyData.keyboard_reconnected = true; //to deal with a wiggly insertion of the LCD/keypad module
                    }
                }
                else
                {
                    //edges are mismatches with the previous
                    u32 edge = key_temp ^ KeyData.key_debounced_l;

                    //see if keypad is reconnected (was high, now low)
                    if((~key_temp & KBD_MASK_KBDPRESENT) != 0u)
                    {
                        KeyData.keyboard_reconnected = true;
                    }
                    else
                    {
                        KeyData.keyboard_reconnected = false;
                    }

                    //Transitions are in normal logic (1= active)
                    KeyData.key_down_transition |= ~key_temp & edge;
                    KeyData.key_up_transition   |= key_temp  & edge;
                    KeyData.key_debounced        = key_temp;
                    KeyData.key_debounced_l      = key_temp;
                }
            }
            else
            {
                --KeyData.latch_count;
            }
        }
        else
        {
            //The keys are bouncing; consider none pressed (active low!), but preserve last known connectedness (need it?)
            // We do not need to clear the Debounced value - it will be cleared automatically
            // by the debouncing code above. Just let it do the work.

            //The keys are bouncing; consider none pressed (active low!), but preserve last known connectedness (need it?)
            // KeyData.key_debounced |= KBD_MASK_PUSHBUTTON_PLUS|KBD_MASK_PUSHBUTTON_STAR|KBD_MASK_PUSHBUTTON_MINUS|KBD_TEST_MASK;
            KeyData.key_latch      = key_temp;
            KeyData.latch_count    = KBD_DEBOUNCE_COUNT - 1u;
        }
    MN_EXIT_CRITICAL();

    /* DI input line is charged now */
    /* Debounce DI input line *****************************************************************/

    MN_ENTER_CRITICAL();
        if (BIT(DI) == (GPIOB->IDR &  BIT(DI)))
        {   /* switch open recognized */
            if (true == KeyData.di_latch) /* If current state is same as in last cycle save debounced state */
            {
                KeyData.di_debounced = true ; /* debounced state is "open" */
            }
            KeyData.di_latch = true; /* sample level */
        }
        else
        {   /* switch closed recognized */
            if (false == KeyData.di_latch) /* If current state is same as in last cycle save debounced state */
            {
                KeyData.di_debounced = false ; /* debounced state is "closed" */
            }
            KeyData.di_latch = false; /* sample level */
        }
    MN_EXIT_CRITICAL();

    // raise turn off power for DI
    key_StopDIPower();
}

/**
    \brief return the stae of the pushbuttons
    After call of function, the "pressed" flags are reset.

    \param[out] bool_t - state of digital input - true == open; false closed
     : bitcoded pushbuttons PUSHBUTTON_STAR, PUSHBUTTON_MINUS, PUSHBUTTON_PLUS
                  return value = pressed (down and released) state of pushbuttons
                  If bit = 1 (down and released)
                  pUpDown = state of pushbuttons
                  0 = pushbutton is up
                  1 = pushbutton is down
*/
u8 bios_ReadPushButtonStatus(u8 *pUpdown)
{
    u8 retval;
    u8 keystate;
    u32 keystate_raw;
    u32 edge_raw;

    MN_ASSERT(NULL != pUpdown);
    keystate = 0U;
    retval = 0U;
    if (KeyData.keyboard_locked)
    {
        /* with keyboard locked pushbutton state is "up" and no edges are recognized */
    }
    else
    {   /* acquire pushbutton levels */
        keystate_raw = KeyData.key_debounced ;
        /* acquire pending edges */
        MN_ASSERT(!(*pUpdown > PUSHBUTTON_RELEASE_EDGE));
        MN_ENTER_CRITICAL();
            switch (*pUpdown)
            {
                case PUSHBUTTON_DOWN_EDGE:
                    edge_raw = KeyData.key_down_transition;
                    KeyData.key_down_transition = 0U;
                    break;

                case PUSHBUTTON_RELEASE_EDGE:
                default:
                    edge_raw = KeyData.key_up_transition;
                    KeyData.key_up_transition = 0U;
                    break;

            }
        MN_EXIT_CRITICAL();
        if((keystate_raw & KBD_MASK_PUSHBUTTON_PLUS)==0U)
        {
            keystate = PUSHBUTTON_PLUS;
        }
        if((keystate_raw & KBD_MASK_PUSHBUTTON_MINUS)==0U)
        {
            keystate |= PUSHBUTTON_MINUS;
        }
        if((keystate_raw & KBD_MASK_PUSHBUTTON_STAR)==0U)
        {
            keystate |= PUSHBUTTON_STAR;
        }
        if((edge_raw & KBD_MASK_PUSHBUTTON_PLUS)!=0U)
        {
            retval = PUSHBUTTON_PLUS;
        }
        if((edge_raw & KBD_MASK_PUSHBUTTON_MINUS)!=0U)
        {
            retval |= PUSHBUTTON_MINUS;
        }
        if((edge_raw & KBD_MASK_PUSHBUTTON_STAR)!=0U)
        {
            retval |= PUSHBUTTON_STAR;
        }

    } /* end of if keyboard locked */
    *pUpdown = keystate ;
    return (retval);
}

/**
    \brief return the debounced state of the digital input

    \param[out] bool_t - state of digital input - true == open; false closed
*/
bool_t bios_ReadDiState(void)
{
    return(KeyData.di_debounced);
}

/**
    \brief Setup external interrupts on PA 10 (ISP_RX_APP). This is done to
    catch when someone plugs the ISP adapter into the ISP/LCD connector and
    enables some host program (e.g. Smarts Assistant).

    The interrupt happens either from LCD traffic (SCL_LCD) or from the ISP adapter.
    In either case it means that /BOOTLOAD is active and wants to cennect through to the
    U(S)ART.
*/
void gpio_SetEXTI(void)
{
    MN_ENTER_CRITICAL();
        EXTI->IMR  = (0u << 10);
        EXTI->PR   = (1u << 10);
        EXTI->IMR  = (1u << 10);
        EXTI->FTSR = (1u << 10);
        NVIC_EnableIRQ(EXTI_IRQn);
    MN_EXIT_CRITICAL();
}

static void gpio_SetSPI0Clk(bool_t which)
{
    u32 val;
    u32 sft;
    u32 tempreg;

    // Calculate shift
    sft = (u32)PF_SHFT * ((u32)SCL_LCD & 0x7u);

    // Calculate mask
    val  = which ? INPUT_PUP : ALT_PUP;
    val &= ~ST1;                    // Clear ST1 bit
    val |= val << 8;                //replicate the INPUT_PUP or ALT_PP parameter

    MN_ENTER_CRITICAL();
        tempreg  = GPIOB->CRH;
        tempreg &= ~(0xf0fu << sft); //mask off (clear) the INPUT or a that is there now
        tempreg |= val << sft;       //set the new INPUT or ALT_PP
        GPIOB->CRH = tempreg;

        // GPIOB->CRH &= ~(0xf0fu << sft); //mask off (clear) the INPUT or a that is there now
        // GPIOB->CRH |= val << sft;       //set the new INPUT or ALT_PP

        spi_SetEnableDisableDev2(which);             // tell SPI to relinquish the GPIO pins
    MN_EXIT_CRITICAL();
}

/**
    \brief An EXT interrupt has occured indicating activity on PA10 (ISP_RX_APP).
    Call the function to disconnect the LCD SPI so that UART traffic can get through.
*/
MN_IRQ void EXTI_IRQHandler(void)
{
    EXTI->IMR  = (0u << 10);        //turn off PA10 interrupts
    EXTI->PR = (1u << 10);          //clear pending interrupt
    gpio_SetSPI0Clk(true);
    KeyData.keyboard_locked = true; //disable keypad polling
    KeyData.keyboard_reconnected = false; //don't assume keypad is in until re-detected
}

//----------------------------------------------------------------------------------
// I2C support -- special gpio operations

/** \brief Change the mode of the I2C1 SDA pin to GPIO or Alternate according to the
    passed parameter.  This is used by the I2C driver to free the controller if it
    was reset during a read operation (e.g. FRAM read).  Note that gpio_PortInit set
    the ODR for pin SDA to 0.  So toggling back and forth between Alternate and GPIO
    will issue I2C clocks unless something is holding the SDA line low.

    \param which - true the pin is set to GPIO, false the pin is set to Alternate mode.
*/
void gpio_I2C_SetI2cClk(bool_t which)
{
    u32 val = which ? GPCLK : I2CLK;
    u32 sft = (u32)PF_SHFT * ((u32)SCL & 0x7u);

    // GPIOB->CRL &= ~(0xfu << sft);
    // GPIOB->CRL |= val << sft;

    u32 tempreg;

    tempreg     = GPIOB->CRL;
    tempreg    &= ~(0xfu << sft);
    tempreg    |= val << sft;
    GPIOB->CRL  = tempreg;
}

bool_t  gpio_I2C_SDAState(void)
{
    return gpio_BitState(SDA);
}

//----------------------------------------------------------------------------------
// Read teh WHOLE GPIO B port
u32     gpio_ReadGPIOB(void)
{
    return  GPIOB->IDR;
}

//----------------------------------------------------------------------------------

/* End of the source */
