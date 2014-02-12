/**
    \file digitalsp.h
    \brief Collection of API for handling digital setpoint for normal mode

    CPU: Any

    OWNER: AK

    \ingroup control
*/
#ifndef DIGITALSP_H_
#define DIGITALSP_H_

#include "posctlmode.h" //for ctlmode_t; that will change

typedef struct digitalsp_t
{
    s32 setpoint; //!< setpoint characterized to position domain
    tick_t timestamp; //!< timestamp in ticks (must be < INT_MAX)
    u8 xmode; //!< externally requested mode (FF TB target mode)
    bool_t isValid;
    u16 CheckWord;
} digitalsp_t; //! type of digital setpoint data

enum
{
    SSO_current_position,
    SSO_fixed_setpoint,
    SSO_last_setpoint
};

typedef struct DigitalSpConf_t
{
    s32 FixedSetpoint; //!< default failed state setpoint characterized to position domain
    tick_t ShedTime; //!< timeout of setpoint receipt causing failed state mode
    tick_t InitTime; //!< timeout of FIRST setpoint receipt causing failed state mode
    bool_t IsTargetToManual; //!< Setpoint failure request: OOS or MANUAL
    u8 sp_option; //!< configurable start option
    u16 CheckWord;
} DigitalSpConf_t; //! type of configuration data for digital setpoint

extern const s32 DigitalSP_ShedTime_range[Xends];

extern const DigitalSpConf_t *digsp_GetConf(DigitalSpConf_t *dst);
extern ErrorCode_t digsp_SetConf(const DigitalSpConf_t *src);
UNTYPEACC(digsp_SetConf, digsp_GetConf);

extern const digitalsp_t *digsp_GetData(digitalsp_t *dst);
extern ErrorCode_t digsp_SetData(const digitalsp_t *src);
UNTYPEACC(digsp_SetData, digsp_GetData);

//extern ErrorCode_t digsp_SetDigitalSetpoint(s32 sp);
extern ErrorCode_t digsp_SetDigitalSetpointEx(u8 xmode, s32 sp);
extern u8 digsp_GetExternalMode(void);
extern void digsp_Mopup(void);

extern s32 digsp_GetDigitalPosSetpoint(void);
extern s32 digsp_GetDigitalSetpoint(void);

extern const s32 digitalsp_range[Xends];

#endif //DIGITALSP_H_
/* This line marks the end of the source */
