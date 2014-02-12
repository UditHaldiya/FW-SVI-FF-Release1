/*
Copyright 2013 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file clonenvram.c
    \brief Copy NVMEM to the opposite bank

    CPU: Any

    OWNER: AK

    \ingroup nvram
*/
#define NEED_NVMEM_END
#include "mnwrap.h"
#include "process.h"
#include "fram.h"
#include "faultpublic.h"

//#include "nvram.h"
#include "nvmempriv.h"
#include "nvramtypes.h"
#include MEMMAP_H_
#include "nvramtable.h"
#include "clonenvram.h"


//Redundant-ish definitions for the first cut
//lint -emacro({826}, CRC_ADDR1)  area too small:  The checksum is the two bytes beyond (dst + len)
#define CRC_ADDR1(addr, len) ((u16 *)((u8*)(addr) + (len)))
#define NVM_H_MAXSIZE1 ((NVM_MAXSIZE+(sizeof(u16)-1U))/sizeof(u16)) //size in halfwords

static u16 nvbuf1[NVM_H_MAXSIZE1];

static ErrorCode_t completion_err = ERR_NOT_SUPPORTED;

/** \brief Process of cloning nvram to the opposite bank
(opposite flash download support)
\return a process completion code
*/
procresult_t nvmem_Clone(s16 *procdetails)
{
    UNUSED_OK(procdetails);
    procresult_t procresult = PROCRESULT_FAILED;

#if defined(NVRAM_PARTITION_REMAP_SIZE) && (NVRAM_PARTITION_REMAP_SIZE!=0)

    ErrorCode_t err = ERR_OK;
    u16_least len;
    u16_least offs = fram_GetRemapOffset(NVRAM_PARTITION_REMAP_SIZE);
    u16_least src_offset;
    u16_least dst_offset;
    if(offs >= NVRAM_PARTITION_REMAP_SIZE)
    {
        dst_offset = offs - NVRAM_PARTITION_REMAP_SIZE;
        src_offset = NVRAM_PARTITION_REMAP_SIZE;
    }
    else
    {
        src_offset = offs;
        dst_offset = NVRAM_PARTITION_REMAP_SIZE;
    }

    nvramId_fast_t id;
    for(id=0; id<NVRAM_ENTRIES; id++)
    {
        if(err != ERR_OK)
        {
            break;
        }

        len = nvram_map[id].length;  //for CRC field
        u16 *pCheckWord = CRC_ADDR1(nvbuf1, len);
        MN_FRAM_ACQUIRE();
            err = fram_ReadExt((void *)nvbuf1,
                               (nvram_map[id].offset + src_offset),
                               len + sizeof(u16),
                               nvram_map[id].volid);
        MN_FRAM_RELEASE();

        if(err == ERR_OK)
        {
            u16 crc = hCrc16(CRC_SEED, nvbuf1, len/2U); //length is in halfwords
            if(crc != *pCheckWord)
            {
                err = ERR_NVMEM_CRC;
            }
        }

        if(err == ERR_OK)
        {
            (void)process_WaitForTimeExt(1U, CANCELPROC_MODE); //let periodic services (mopup and WD) run
            //Keep going!

            len = nvram_map[id].length + sizeof(u16);
            MN_FRAM_ACQUIRE();
                err = fram_WriteExt((void *)nvbuf1,
                               (nvram_map[id].offset + dst_offset),
                               len + sizeof(u16),
                               nvram_map[id].volid);
            MN_FRAM_RELEASE();
        }
        if(err == ERR_OK) //wait again
        {
            (void)process_WaitForTimeExt(1U, CANCELPROC_MODE);
            //Keep going!
        }
    }

    if(err == ERR_OK)
    {
        procresult = PROCRESULT_OK;
    }
    completion_err = err;
#endif
    return procresult;
}

static const nvramId_t clone_sync_tab[] =
{
    NVRAMID_ModeData,
    NVRAMID_digitalsp,
};

/** \brief Repairs a few things that may go out of sync
*/
ErrorCode_t nvmem_CloneSync(void)
{
    if(completion_err != ERR_OK)
    {
        return completion_err;
    }
    ErrorCode_t err = ERR_OK;
    u16_least len;
    u16_least offs = fram_GetRemapOffset(NVRAM_PARTITION_REMAP_SIZE);
    u16_least dst_offset;
    if(offs >= NVRAM_PARTITION_REMAP_SIZE)
    {
        dst_offset = offs - NVRAM_PARTITION_REMAP_SIZE;
    }
    else
    {
        dst_offset = NVRAM_PARTITION_REMAP_SIZE;
    }

    nvramId_fast_t id;
    for(u8_least i=0; i<NELEM(clone_sync_tab); i++)
    {
        id = clone_sync_tab[i];
        (void)nvmem_GetItemById(nvbuf1, id);
        len = nvram_map[id].length;  //for CRC field
        u16 *pCheckWord = CRC_ADDR1(nvbuf1, len);
        *pCheckWord = hCrc16(CRC_SEED, nvbuf1, len/2U); //length is in halfwords

        MN_FRAM_ACQUIRE();
            err = fram_WriteExt((void *)nvbuf1,
                           (nvram_map[id].offset + dst_offset),
                           len + sizeof(u16),
                           nvram_map[id].volid);
        MN_FRAM_RELEASE();
        if(err != ERR_OK)
        {
            break;
        }
    }
    if(err == ERR_OK)
    {
        completion_err = ERR_NOT_SUPPORTED; //don't call it again
    }

    return err;
}

/* This line marks the end of the source */
