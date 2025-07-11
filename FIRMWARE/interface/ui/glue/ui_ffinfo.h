/**
Copyright 2009 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file ui_ffinfo.h
    \brief interface to support FF variables

    CPU: Any

    OWNER: ej

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/interface/ui/glue/ui_ffinfo.h $
    $Date: 1/07/12 3:15p $
    $Revision: 1 $
    $Author: ericj $

*/
/* $History: ui_ffinfo.h $
 * *****************  Version 1  *****************
 * User: eric j    Date: 1/07/12    Time: 10:18p
 * Created in $/MNCB/Dev/FIRMWARE/interface/ui/glue
 * Compartmentalizing the UI glue layer
*/
#ifndef UI_FFINFO_H_
#define UI_FFINFO_H_

#define FFSTR_MODE_NA   "     "
#define FFSTR_MODE_ROUT "ROUT "
#define FFSTR_MODE_RCAS "RCAS "
#define FFSTR_MODE_CAS  "CAS  "
#define FFSTR_MODE_AUTO "AUTO "
#define FFSTR_MODE_MAN  "MAN  "
#define FFSTR_MODE_LO   "LO   "
#define FFSTR_MODE_IMAN "IMAN "
#define FFSTR_MODE_OOS  "OOS  "
//#define FFSTR_MODE_NOSC "NOSC "

//#define FFSTR_ERR_NA  "    "
#define FFSTR_ERR_ERR "ERR "
#define FFSTR_ERR_OK  "OK  "

extern bool_t ui_ff_InfoRB(const uistate_t *state);
extern bool_t ui_ff_Tag1RB(const uistate_t *state);
extern bool_t ui_ff_Tag2RB(const uistate_t *state);
extern bool_t ui_ff_Tag3RB(const uistate_t *state);
extern bool_t ui_ff_Tag4RB(const uistate_t *state);

extern bool_t ui_ff_InfoTB(const uistate_t *state);
extern bool_t ui_ff_Tag1TB(const uistate_t *state);
extern bool_t ui_ff_Tag2TB(const uistate_t *state);
extern bool_t ui_ff_Tag3TB(const uistate_t *state);
extern bool_t ui_ff_Tag4TB(const uistate_t *state);

extern bool_t ui_ff_InfoAO(const uistate_t *state);
extern bool_t ui_ff_Tag1AO(const uistate_t *state);
extern bool_t ui_ff_Tag2AO(const uistate_t *state);
extern bool_t ui_ff_Tag3AO(const uistate_t *state);
extern bool_t ui_ff_Tag4AO(const uistate_t *state);

extern bool_t ui_ff_InfoDO(const uistate_t *state);
extern bool_t ui_ff_Tag1DO(const uistate_t *state);
extern bool_t ui_ff_Tag2DO(const uistate_t *state);
extern bool_t ui_ff_Tag3DO(const uistate_t *state);
extern bool_t ui_ff_Tag4DO(const uistate_t *state);

extern bool_t ui_ff_InfoDO2(const uistate_t *state);
extern bool_t ui_ff_Tag1DO2(const uistate_t *state);
extern bool_t ui_ff_Tag2DO2(const uistate_t *state);
extern bool_t ui_ff_Tag3DO2(const uistate_t *state);
extern bool_t ui_ff_Tag4DO2(const uistate_t *state);

extern bool_t ui_ff_InfoPID(const uistate_t *state);
extern bool_t ui_ff_Tag1PID(const uistate_t *state);
extern bool_t ui_ff_Tag2PID(const uistate_t *state);
extern bool_t ui_ff_Tag3PID(const uistate_t *state);
extern bool_t ui_ff_Tag4PID(const uistate_t *state);

extern bool_t ui_ff_InfoPID2(const uistate_t *state);
extern bool_t ui_ff_Tag1PID2(const uistate_t *state);
extern bool_t ui_ff_Tag2PID2(const uistate_t *state);
extern bool_t ui_ff_Tag3PID2(const uistate_t *state);
extern bool_t ui_ff_Tag4PID2(const uistate_t *state);

extern bool_t ui_ff_devID1(const uistate_t *state);
extern bool_t ui_ff_devID2(const uistate_t *state);
extern bool_t ui_ff_devID3(const uistate_t *state);
extern bool_t ui_ff_devID4(const uistate_t *state);
extern bool_t ui_ff_devAddr(const uistate_t *state);
extern bool_t ui_ff_devTag1(const uistate_t *state);
extern bool_t ui_ff_devTag2(const uistate_t *state);
extern bool_t ui_ff_devTag3(const uistate_t *state);
extern bool_t ui_ff_devTag4(const uistate_t *state);

extern bool_t ui_Clr_Lin3(const uistate_t *state);
extern bool_t ui_Clr_Lin3Silent(const uistate_t *state);

//extern bool_t ui_IsSimulationActive(const uistate_t *state);
//extern bool_t ui_IsSimulationDeactive(const uistate_t *state);
extern bool_t ui_SimulationActive(const uistate_t *state);
extern bool_t ui_SimulationDeactive(const uistate_t *state);

#endif //UI_FFINFO_H_
