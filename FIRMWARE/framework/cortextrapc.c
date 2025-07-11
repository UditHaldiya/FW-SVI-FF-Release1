#include "mnwrap.h"
#include "mnassert.h"
#include "stmcommon.h"
#include "cortextrap.h"

#define VECT_MASK   0x1ffu      // mask for active interrupt vector in SCB->ICSR

/* NOTE: Unlike Ernie's implementation (negative trap values)
I make them positive to look like coming from MN_ASSERT.
This is made to minimize changes with trap routing (which would involve
two routing tables). If address field doesn't point to a filename,
we know it's a trap. I modified ui_io.c not to go beyond the 9 LCD characters
*/

/** \brief This function receives the address of the trap stack frame from the assembly
    language routine Cortex_Trap in cortextrap.s79.
\param spVal - a pointer to trap stack layout
*/
void Cortex_TrapC(const volatile stk_t *spVal)
{
    u32 vectactive = SCB->ICSR & VECT_MASK;
    mn_assert(spVal->ret, (s32)vectactive); //Cast is valid: see VECT_MASK value
}

/** \brief This function receives the address of the trap stack frame from the assembly
    language routine Cortex_Trap in cortextrap.s79.
\param spVal - a pointer to trap stack layout
*/
void NVIC_TrapC(const volatile stk_t *spVal)
{
    u32 vectactive = SCB->ICSR & VECT_MASK;
    //Add a 1000 to distinguish between traps 1 and 2
    mn_assert(spVal->ret, 1000+(s32)vectactive); //Cast is valid: see VECT_MASK value
}

