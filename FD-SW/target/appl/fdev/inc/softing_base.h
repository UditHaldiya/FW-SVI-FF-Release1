#ifndef SOFTING_BASE_H_
#define SOFTING_BASE_H_

#include <base.h>
#include <fbap.h>
#include <keywords.h>
#include <fbs_api.h>
#include <ffbl_api.h>
#include <fbif.h>
#include <fbif_idx.h>
#include <eep_if.h>
#include <hm_api.h>
#include <osif.h>
#include <except.h>
#include <fbif_fct.h>
#include <fbif_cfg.h>

//These functions are criminally prototyped in a C source (appl_if.c)
  FUNCTION GLOBAL USIGN16 Appl_read_handler_RESB (T_FBIF_BLOCK_INSTANCE *, T_FBIF_READ_DATA *);
  FUNCTION GLOBAL USIGN16 Appl_write_handler_RESB (T_FBIF_BLOCK_INSTANCE *, T_FBIF_WRITE_DATA *);
  FUNCTION GLOBAL VOID Appl_background_RESB (T_FBIF_BLOCK_INSTANCE *);
  FUNCTION GLOBAL VOID Appl_get_default_value (T_FBIF_BLOCK_INSTANCE *, T_FBIF_READ_DATA *);
  FUNCTION GLOBAL T_CHAN_UNIT_CHECK Appl_check_channel_unit (USIGN16, SCALE *);
  FUNCTION GLOBAL VOID Appl_UpdateSerialNumber(USIGN32 DeviceSN);

#ifdef FBL_BLOCK_INST /* -------------------------------------------------- */
  FUNCTION GLOBAL USIGN8 Appl_check_block_state (USIGN16, USIGN8);
#endif /* FBL_BLOCK_INST */

#ifdef USE_FIELD_DIAGNOSTICS /* ------------------------------------------- */
  FUNCTION GLOBAL USIGN16 Appl_get_recommend_fd_action (USIGN32, BOOL);
  FUNCTION GLOBAL USIGN32 Appl_get_diagnostic_value    (VOID);
#endif /* USE_FIELD_DIAGNOSTICS */

  FUNCTION GLOBAL VOID appl_task (T_EVENT);

  #define HM_RUN 0x01
  FUNCTION GLOBAL USIGN8 appl_check_hm_state(void);

//The foolowing is of interest to others, so went from appl_ptb.c to the header
extern USIGN8 comm_state; /* variable used for HART communication state */
FUNCTION GLOBAL VOID IncrementStaticRevision(T_FBIF_BLOCK_INSTANCE*, s8);
#endif //SOFTING_BASE_H_
