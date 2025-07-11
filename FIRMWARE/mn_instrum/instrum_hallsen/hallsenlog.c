#include "mnwrap.h"
#include "mn_instrum.h"
#include "adtype.h"
#include "bufferhandler.h"

static bool_t active = true;

void instrum_HallSensorLog(u8_least channel, u16 value)
{
    if(channel == AD_POS_INT)
    {
        if(active)
        {
            bool_t isfull = buffer_AddXDiagnosticData1(DIAGBUF_AUX, (diag_t)value);
            if(isfull)
            {
                active = false; //don't clobber the buffer until it is read
            }
        }
    }
    if(channel == AD_PRESS_PILOT)
    {
        //use this channel opportunistically to reenable sampling
        if(buffer_GetEntriesInXDiagnosticBuffer(DIAGBUF_AUX) == 0)
        {
            active = true;
        }
    }
}

