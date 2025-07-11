/*
Copyright 2012 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file ispcomm.h
    \brief API of misc. comm. channels

    CPU: Any

    OWNER: AK

    \ingroup HARTapp
*/

#ifndef ISPCOMM_H_
#define ISPCOMM_H_

//lint -sem(isp_CommTask, thread_mono) designate it as a task
extern void isp_CommTask(void *p_arg);
//lint -sem(ipc_CommTask, thread_mono) designate it as a task
extern void ipc_CommTask(void *p_arg);

#endif //ISPCOMM_H_
/* This line marks the end of the source */
