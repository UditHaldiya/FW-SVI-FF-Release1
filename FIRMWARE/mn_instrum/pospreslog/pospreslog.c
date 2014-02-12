#include "mnwrap.h"
#include "mn_instrum.h"
#include "bufferhandler.h"
#include "ext_analysis.h"
#include "mncbtypes.h"
#include "errcodes.h"
#include "cycle.h"
#include "control.h"
#include "bufferhandler.h"
#include "hartdef.h"
#include "hartfunc.h"
#include "bufutils.h"

static bool_t active = false;
static u8 SkipCount = 0; //Sample every SkipCount-th control cycle
static u8 skips = 0; //number of actual skips after last sample
const ExtAnalysParams_t *pCtlParams; //pointer to parameters from control.c


s8_least hartcmd_RestartSampling(const u8 *src, u8 *dst)
{
    const Req_RestartSampling_t *s = (const void *)src;
    UNUSED_OK(dst);
    SkipCount = util_GetU8(s->SkipCount[0]);
    skips = 0;
    buffer_InitializeXDiagnosticBuffer(DIAGBUF_AUX);
    active = true; //start sampling
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

void instrum_PosPresLog(void)
{
    bool_t isfull;

    if(active)
    {
        if(skips < SkipCount)
        {
            skips++;
            if(skips == SkipCount)
            {
                skips = 0;
                /** TFS:6154 */
                //obtain pointer to parameters from control.c
                //these are the same parameters as used by HART command 242,1
                //Attention: replace parameters as needed from the structure <ExtAnalysParams_t>, file "ext_analysis.h"
                pCtlParams = control_FillExtAnalysParams();
                isfull = buffer_AddXDiagnosticData2( DIAGBUF_AUX,
                                                     pCtlParams->RateLimitedSetpoint, pCtlParams->PositionScaled );
                if(!isfull)
                {
                    isfull = buffer_AddXDiagnosticData1( DIAGBUF_AUX, pCtlParams->P_pilot );
                }
                if(!isfull)
                {
                    isfull = buffer_AddXDiagnosticData2( DIAGBUF_AUX,
                                                         (diag_t)pCtlParams->CtlOutput, (diag_t)pCtlParams->PWM );
                }
                if(isfull)
                {
                    active = false;
                }
            }
        }
    }
}

