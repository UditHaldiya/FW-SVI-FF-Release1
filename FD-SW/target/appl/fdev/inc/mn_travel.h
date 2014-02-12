/*
Copyright 2012 by GE Energy / Dresser, Inc., as an unpublished trade secret. All rights reserved.

This document and all information herein are the property of GE Energy.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon demand.
*/

/**
    \file appl_alert.h
    \brief Position and other types of Alerts (Level monitors) headers

    CPU: Any

    OWNER: AP
*/

#ifndef MN_TRAVEL_H__
#define MN_TRAVEL_H__

//------------------------------------------------------------------
// Function API

/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
  Returns pointer to FLOAT_S parameter -- represnts Working Position in Engineering Units
  defined by the Travel parameter.

  NO ACTUAL CALCULATIONS ARE PERFORMED IN THIS FUNCTION.

PARAMETERS:
  p_block_instance         Pointer to the block to be started

RETURN_VALUES:
  Pointer to FLOAT_S structure containing WorkingPositino In Travel Units and its status.

------------------------------------------------------------------------------*/

FUNCTION PUBLIC FLOAT_S *TB_Get_WorkingPos_InTravelUnits(const T_FBIF_BLOCK_INSTANCE *    p_block_instance);

//------------------------------------------------------------------

#endif /* MN_TRAVEL__ */
