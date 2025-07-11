#include "mnwrap.h"
#include "uishow.h"
#include "ui_dispmode.h"

#ifndef NDEBUG
void *ui_getUIDispmode(void)
{
    return ui_getDispmode();
}
#endif
