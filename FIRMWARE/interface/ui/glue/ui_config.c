#include "mnwrap.h"
#include "numutils.h"
#include "ui_config.h"
#include "selftune.h"

static CtlLimits_t ui_CtlLimits;
static uivar_t ui_ATO;
static u8 ui_charact;
static u8 ui_pidctrlset;

const void *ui_GetAirAction(void)
{
    return &ui_ATO;
}

const void *ui_GetSpCharact(void)
{
    return &ui_charact;
}

CtlLimits_t *ui_GetCtlLimits(void)
{
    return &ui_CtlLimits;
}

const void *ui_GetPidParamSet(void)
{
    return &ui_pidctrlset;
}


/** A node function
    \brief gets data set in the menus (not in long processes)
    \return false (always success)
*/
bool_t ui_GetSysCtlLimits(void)
{
    (void)control_GetLimits(&ui_CtlLimits);
    return false;
}
bool_t ui_GetSysAirAction(void)
{
    ui_ATO = bool2int(pos_GetPositionConf(NULL)->bATO);
    return false;
}
bool_t ui_GetSysSpCharact(void)
{
    ui_charact = poscharact_GetCharacterizationSel(NULL)->CharactSel;
    return false;
}
bool_t ui_GetSysPidParamSet(void)
{
    ui_pidctrlset = posctl_GetCtlSetIndex();
    return false;
}

/** A node function
    \brief save data set in the menus (not in long processes)
    \return TRUE (always success)
*/
bool_t ui_SaveSysCtlLimits(void)
{
    //success: disable the failure node
    return (ERR_OK == control_SetLimits(&ui_CtlLimits));
}
bool_t ui_SaveSysAirAction(void)
{
    ErrorCode_t err;
    PositionConf_t posdata;
    (void)pos_GetPositionConf(&posdata);
    if(ui_ATO == bool2int(pos_GetPositionConf(NULL)->bATO))
    {
        //no change: nothing to do
        err = ERR_OK;
    }
    else
    {
        posdata.bATO = int2bool(ui_ATO);
        err = pos_SetPositionConf(&posdata);
    }
    return (ERR_OK == err);
}
bool_t ui_SaveSysSpCharact(void)
{
    //success: disable the failure node
    return (ERR_OK == poscharact_Select(ui_charact));
}
bool_t ui_SaveSysPidParamSet(void)
{
    CtlSetSlot_t ctlset;
    (void)posctl_GetCtlset(&ctlset);
    ctlset.slot = ui_pidctrlset;
    return (ERR_OK == posctl_SetCtlset(&ctlset));
}
bool_t ui_SaveSysLanguage(void)
{
    return (ERR_OK == ui_RepairSetUIData(ui_GetLocalUIData(NULL)) );
}

/* this mark the end of the file*/


