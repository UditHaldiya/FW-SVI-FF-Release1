#include <base.h>
#include "sysio.h"
#include "mnassert.h"
#if 0
    #include "faultpublic.h"
#endif
#include "timer.h"
//#include "stmcommon.h"
#include <keywords.h>
#include <hw_if.h>
#include <osif.h>
#include <hw_i2c.h>

#ifndef NDEBUG
void sysio_ShowCrash(const void *message, s32 line)
{
    UNUSED_OK(message);
    UNUSED_OK(line);
}
#endif

static MN_NO_INIT u16 wsSignature;
static bool_t isWarmstart = false;

#define WARM_SIGNATURE (0x5a5a)

void reset_Warmstart(void)
{
    MN_ENTER_CRITICAL();
        wsSignature = WARM_SIGNATURE;
        HardReset_CPU();
    MN_EXIT_CRITICAL();
}

/** \brief Report warmstart state
\return true iff warm start
*/
bool_t reset_IsWarmstart(void)
{
    return isWarmstart;
}

void  reset_OnReset(void)
{
    i2c_AcquireMutex();
    mn_InitAssert();
    i2c_ReleaseMutex();
    if (wsSignature == WARM_SIGNATURE)
    {
        isWarmstart = true;
        wsSignature = 0;
    }
}

/** \brief Return the current value of the 5 ms tick counter

  \return the current tick counter LSW.
*/
u32 bios_GetTimer0Ticker(void)
{
    return osif_get_time_in_ms()/5U;
}

#if 0
//A required stub
void bios_WritePwm(u16 value)
{
    UNUSED_OK(value);
}

void error_SetFault(faultcode_fast_t ErrorCode)
{
    UNUSED_OK(ErrorCode);
}

u32 timer_EstimateFunctionLength(void (*func)(void))
{
    u32   startime = 0xffffffu;         // 8.38 seconds @ 2 Mhz; 4.34 s @4MHz
    u32   TempReload;

    TempReload      = SysTick->LOAD;    // Store he reload value

    SysTick->LOAD   = startime;         // set reload register
    SysTick->VAL    = startime;         // Load the SysTick Counter Value
    func();                             // call routine
    startime       -= SysTick->VAL;     // elapsed time
    SysTick->LOAD   = TempReload;       // Restore reload register
    SysTick->VAL    = 0u;               // Load the SysTick Counter Value
    return startime/2U; //for FFP, account for twice the clock speed without changing idle.c and customizing the headers. Fix it next release!
}
#endif

// end of file
