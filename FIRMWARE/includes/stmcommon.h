#ifndef STMCOMMON_H_
#define STMCOMMON_H_

#include "tmbase_proj.h"
#define TICKS_PER_SEC   LCL_TICKS_PER_SEC        // system tick == 5 ms

#ifdef __STDC__         // so can include in assembly language
#include "errcodes.h"   // to map error codes returned by functions
#include "timebase.h"   // to map error codes returned by functions

#define PERIPH_BASE           0x40000000u
#define APB1PERIPH_BASE       (PERIPH_BASE + 0u)
#define APB2PERIPH_BASE       (PERIPH_BASE + 0x10000u)
#define AHBPERIPH_BASE        (PERIPH_BASE + 0x20000u)

#define IO_REG16(x)  volatile u16 x; u16 CAT(x,dummy)
#define IO_REG32(x)  volatile u32 x

extern const struct vectors_t __vector_table[];

//lint -esym(526,PendSV_Handler)  PendSV_Handler is in os_cpu_a.s79
extern MN_IRQ void  PendSV_Handler(void);

//lint -esym(526,__iar_program_start)  __iar_program_start is in cmain.s (library)
extern MN_IRQ void __iar_program_start(void);
//lint -esym(526,jmpstart)  jmpstart is in os_cpu_a_M3.s79
//lint -esym(757,jmpstart)  not referenced - may or may not be used (a weak reset method)
extern void jmpstart(u32 addr);

// NVIC API
MN_DECLARE_API_FUNC(scb_DoHardReset)
typedef s32 IRQn_Type;
extern void NVIC_SetVectors(void);
extern void NVIC_EnableIRQ(IRQn_Type IRQn);
extern MN_IRQ void SysTick_Handler(void);
extern void systickdly(u32 clocks);
extern void systickdly_with_start(u32 StartTime, u32 clocks);
extern void SysTick_Config(u32 ticks);
extern void scb_DoHardReset(void);

// RCC API
//extern void SystemInit (void);
extern void rcc_enable_HSI(void);
extern void rcc_disable_HSI(void);
extern void rcc_DisableTimer5(void);
extern bool_t rcc_WDTimeout(void);


MN_DECLARE_API_FUNC(__low_level_init) //__low_level_init is calledfrom cmain.s (library)
extern s32 __low_level_init(void);

MN_DECLARE_API_FUNC(startWD) // startWD is not used
extern void startWD(void);
//extern volatile u16 * tmr_GetTM2(void);

// DMA API
//lint ++flb
typedef enum {DMA_Chan_1, DMA_Chan_2, DMA_Chan_3, DMA_Chan_4, DMA_Chan_5, DMA_Chan_6, DMA_Chan_7,   // channels in DMA1
              DMA_Chan_8, DMA_Chan_9, DMA_Chan_10, DMA_Chan_11, DMA_Chan_12} DMA_Chan;              // channels in DMA2
//lint --flb
extern void DMA_ConfigUART(const volatile void *pBuffer, u32 BufferSize, u32 devadr, DMA_Chan chn);
extern void DMA_SetUART(DMA_Chan chntx, DMA_Chan chnrx);
extern u32 DMA_getcount(DMA_Chan chan);


extern u32 tmr_GetLSIFreq(void);

#ifdef TIMER1_NOT_AVAILABLE                 // Profiler
extern void PRF_InitTimer(void);
#endif

// flash API                // the flash programming API is in mn_flash.h
extern void flash_SetACR(void);

// HART / UART API
MN_DECLARE_API_FUNC(UART2_IRQHandler,UART3_IRQHandler,UART4_IRQHandler,uart_Send)
enum {UART_1, UART_2, UART_3, UART_4, UART_5};
extern void uart_Send(u32 uartNo, u16_least length, const u8 *buff);
extern MN_IRQ void UART1_IRQHandler(void);
extern MN_IRQ void UART2_IRQHandler(void);
extern MN_IRQ void UART3_IRQHandler(void);
extern MN_IRQ void UART4_IRQHandler(void);
extern bool_t uart_ProcessUART_DMA_Chars(u8 uartNo, void (*prochar)(u8 c, u8 uartNo, u8_least hart_channel));
extern void uart_init(u8_least uartNo);


//Moved from stm_nvic.c
//lint -esym(768,SysTick_Type::*) Not accessed member OK for a memory-mapped peripheral (would be 754 if SCB_Type were local to a .c file)
typedef struct SysTick_Type
{
    IO_REG32(CTRL);                    /*!< Offset: 0x000 (R/W)  SysTick Control and Status Register */
#define SysTick_CTRL_CLKSOURCE_Pos          2                                             /*!< SysTick CTRL: CLKSOURCE Position */
#define SysTick_CTRL_CLKSOURCE_Msk         (1UL << SysTick_CTRL_CLKSOURCE_Pos)            /*!< SysTick CTRL: CLKSOURCE Mask */
#define SysTick_CTRL_TICKINT_Pos            1                                             /*!< SysTick CTRL: TICKINT Position */
#define SysTick_CTRL_TICKINT_Msk           (1UL << SysTick_CTRL_TICKINT_Pos)              /*!< SysTick CTRL: TICKINT Mask */
#define SysTick_CTRL_ENABLE_Pos             0                                             /*!< SysTick CTRL: ENABLE Position */
#define SysTick_CTRL_ENABLE_Msk            (1UL << SysTick_CTRL_ENABLE_Pos)               /*!< SysTick CTRL: ENABLE Mask */
    IO_REG32(LOAD);                    /*!< Offset: 0x004 (R/W)  SysTick Reload Value Register       */
#define SysTick_LOAD_RELOAD_Pos             0                                             /*!< SysTick LOAD: RELOAD Position */
#define SysTick_LOAD_RELOAD_Msk            (0xFFFFFFUL << SysTick_LOAD_RELOAD_Pos)        /*!< SysTick LOAD: RELOAD Mask */
    IO_REG32(VAL);                     /*!< Offset: 0x008 (R/W)  SysTick Current Value Register      */
    IO_REG32(CALIB);                   /*!< Offset: 0x00C (R/)  SysTick Calibration Register        */
} SysTick_Type;

#define SysTick             HARDWARE(SysTick_Type *, SysTick_BASE)    /*!< SysTick configuration struct      */
#define SysTick_BASE        (SCS_BASE +  0x0010UL)                    /*!< SysTick Base Address              */
#define SCS_BASE            (0xE000E000UL)                            /*!< System Control Space Base Address */

//lint -esym(768,SCB_Type::*) Not accessed member OK for a memory-mapped peripheral (would be 754 if SCB_Type were local to a .c file)
typedef volatile struct SCB_Type
{
    IO_REG32(CPUID);                   /*!< Offset: 0x000 (R/)  CPU ID Base Register                                  */
    IO_REG32(ICSR);                    /*!< Offset: 0x004 (R/W)  Interrupt Control State Register                      */
    const struct vectors_t *volatile VTOR;                    /*!< Offset: 0x008 (R/W)  Vector Table Offset Register                          */
    IO_REG32(AIRCR);                   /*!< Offset: 0x00C (R/W)  Application Interrupt / Reset Control Register        */
#if 0
#define SCB_AIRCR_VECTKEY_Pos              16                                             /*!< SCB AIRCR: VECTKEY Position */
#define SCB_AIRCR_VECTKEY_Msk              (0xFFFFUL << SCB_AIRCR_VECTKEY_Pos)            /*!< SCB AIRCR: VECTKEY Mask */
#define SCB_AIRCR_PRIGROUP_Pos              8                                             /*!< SCB AIRCR: PRIGROUP Position */
#define SCB_AIRCR_PRIGROUP_Msk             (7UL << SCB_AIRCR_PRIGROUP_Pos)                /*!< SCB AIRCR: PRIGROUP Mask */
#endif //0
#define SCB_AIRCR_SYSRESETREQ               ((u32)0x00000004)        /*!< Requests chip control logic to generate a reset */

    IO_REG32(SCR);                     /*!< Offset: 0x010 (R/W)  System Control Register                               */
    IO_REG32(CCR);                     /*!< Offset: 0x014 (R/W)  Configuration Control Register                        */
    u8      SHP[12];                   /*!< Offset: 0x018 (R/W)  System Handlers Priority Registers (4-7, 8-11, 12-15) */
    IO_REG32(SHCSR);                   /*!< Offset: 0x024 (R/W)  System Handler Control and State Register             */
    IO_REG32(CFSR);                    /*!< Offset: 0x028 (R/W)  Configurable Fault Status Register                    */
    IO_REG32(HFSR);                    /*!< Offset: 0x02C (R/W)  Hard Fault Status Register                            */
    IO_REG32(DFSR);                    /*!< Offset: 0x030 (R/W)  Debug Fault Status Register                           */
    IO_REG32(MMFAR);                   /*!< Offset: 0x034 (R/W)  Mem Manage Address Register                           */
    IO_REG32(BFAR);                    /*!< Offset: 0x038 (R/W)  Bus Fault Address Register                            */
    IO_REG32(AFSR);                    /*!< Offset: 0x03C (R/W)  Auxiliary Fault Status Register                       */
    IO_REG32(PFR[2]);                  /*!< Offset: 0x040 (R/)  Processor Feature Register                            */
    IO_REG32(DFR);                     /*!< Offset: 0x048 (R/)  Debug Feature Register                                */
    IO_REG32(ADR);                     /*!< Offset: 0x04C (R/)  Auxiliary Feature Register                            */
    IO_REG32(MMFR[4]);                 /*!< Offset: 0x050 (R/)  Memory Model Feature Register                         */
    IO_REG32(ISAR[5]);                 /*!< Offset: 0x060 (R/)  ISA Feature Register                                  */
} SCB_Type;

#define SCB_BASE            (SCS_BASE +  0x0D00UL)                    /*!< System Control Block Base Address */
#define SCB                 HARDWARE(SCB_Type *, SCB_BASE)         /*!< SCB configuration struct          */


#endif // __SRDC__

#endif // STMCOMMON_H_

// end of source
