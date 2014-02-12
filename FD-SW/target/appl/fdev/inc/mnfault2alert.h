#ifndef MNFAULT2ALERT_H_
#define MNFAULT2ALERT_H_

#include <fbap.h> //for block errors
#include <ptb.h> //T_FBIF_PTB
#include <spec_ds.h> //_ALERT_ACTION

#include "faultpublic.h"

typedef struct MnFaultAlarmMap_t
{
    u16 BlockError;
    u8 BlockAlertX; //alert index
    u8 diag_alarm_number; //??
    u8 recommended_action; //??
    u8 priority; //??
} MnFaultAlarmMap_t;

typedef struct MnRtAlarmMap_t
{
    u16 BlockError;
    u8 BlockAlertX; //alert index
    u8 diag_alarm_number; //??
    u8 recommended_action; //??
    u8 priority; //??
} MnRtAlarmMap_t;


typedef struct MnAlertsEtc_t
{
    u32 alert_map; //diag alert as in Softing
    u32 block_alert_map; //block alert as in Vlad'd TB parameters
    u16 recom_action;
    u8 priority; //of the most severe diag
} MnAlertsEtc_t;


#define ALERT_NONE 0xFFU //no block alert for a fault
#define ALERT_INDEX(alert_name) (MN_OFFSETOF(_ALERT_STATE, alert_name))


typedef struct
{
    u16 old_block_alarm_bits;
    u16 rect_period;
} T_APPL_TB_DATA;

extern const MnFaultAlarmMap_t MnFaultAlarmMap[MAX_FAULT_NUMBER];

extern u16 mnfaults_Import(const T_FBIF_PTB *p_PTB, MnAlertsEtc_t *AlertsEtc);
extern void mnptb_ProcErrors(const T_FBIF_BLOCK_INSTANCE *p_block_instance);
extern u16 mnptb_GetTBRecommendedAction(void);


//Softing goodies
//from appl_dia.c
  #define FD_NO_RECOMM_ACT          1


#endif //MNFAULT2ALERT_H_
