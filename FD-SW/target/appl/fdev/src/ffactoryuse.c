/*
Copyright 2013 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file ffactoryuse.c
    \brief factory_use_{1,2} support in FFP

    HART pass-through and FFP intercepts
*/
#include <softing_base.h>
#include <ptb.h>
#include <eep_if.h>

#define NEED_NVMEM_END //lint !e962 to see assert_t

#include "ffactoryuse.h"

#include "hartfunc.h"
//#include "mnassert.h"
#include "hartcmd.h"
#include "bufutils.h"
#include "mn_flash.h"

#define USE_ASSERT_STUB 0
#if USE_ASSERT_STUB
extern volatile MN_NO_INIT assert_t assert_msg; //make sure the compiler doesn't optimize out writes to it
#define ASSERT_HEADER_SIZE (1/*subcmd*/+1/*trap index*/+2/*trap count*/)
#endif

/*factory_use_1 layout
0 - cmd
1 - length
2+ - data payload
*/
/*factory_use_2 layout
0 - cmd
1 - delimiter
2 - response code 1
3 - response code 2
4+ - data payload
*/
#define FFACT_DATA_OFFSET 4

    //--------- subcommand 3 --------------------
//lint -esym(641, FFPMemorySelectionEnum_t) -esym(769, FFPMemorySelectionEnum_t) OK to not use or store as int
typedef enum FFPMemorySelectionEnum_t
{
    FFPNVMEM = 1,
    FFPRAM = 0
} FFPMemorySelectionEnum_t;
typedef struct Req_ReadFFPmemory_t
{
    u8 FFPMemorySelection[1][sizeof(FFPMemorySelectionEnum_t)];
    u8 FFPMemoryaddress[1][sizeof(H_UINT4)];
    u8 endmark; //required marker
} Req_ReadFFPmemory_t;
typedef struct Rsp_ReadFFPmemory_t
{
    u8 FFPMemorySelection[1][sizeof(FFPMemorySelectionEnum_t)];
    u8 FFPMemoryaddress[1][sizeof(H_UINT4)];
    u8 FFPMemoryContent[24][sizeof(H_UCHAR)];
    u8 endmark; //required marker
} Rsp_ReadFFPmemory_t;
static s8_least hartcmd_ReadFFPmemory(const u8 *src, u8 *dst)
{
    s8_least ret = HART_NO_COMMAND_SPECIFIC_ERRORS;
    const Req_ReadFFPmemory_t *s = (const void *)src;
    Rsp_ReadFFPmemory_t *d = (void *)dst;
    UNUSED_OK(d->FFPMemorySelection); //rely on framework to fill
    UNUSED_OK(d->FFPMemoryaddress); //rely on framework to fill
    FFPMemorySelectionEnum_t memtype = (FFPMemorySelectionEnum_t)util_GetU8(s->FFPMemorySelection[0]);
    u32 addr = util_GetU32(s->FFPMemoryaddress[0]);
    u16 len = sizeof(d->FFPMemoryContent);
    switch(memtype)
    {
        case FFPNVMEM:
        {
            ErrorCode_t err = eep_read_block(d->FFPMemoryContent[0], (u16)addr, len);
            if(err != ERR_OK)
            {
                ret = TRANSMITTER_SPECIFIC_COMMAND_ERROR;
            }
            break;
        }
        case FFPRAM:
        {
            memcpy(d->FFPMemoryContent[0], HARDWARE(void *, addr), len);
            break;
        }
        default:
        {
            ret = HART_PASSED_PARAMETER_TOO_LARGE;
            break;
        }
    }
    return ret;
}

    //--------- subcommand 4 --------------------
typedef struct Req_WriteFFPmemory_t
{
    u8 FFPMemorySelection[1][sizeof(FFPMemorySelectionEnum_t)];
    u8 FFPMemoryaddress[1][sizeof(H_UINT4)];
    u8 FFPMemoryBlockLength[1][sizeof(H_UCHAR)];
    u8 FFPMemoryContent[24][sizeof(H_UCHAR)];
    u8 endmark; //required marker
} Req_WriteFFPmemory_t;
typedef struct Rsp_WriteFFPmemory_t
{
    u8 FFPMemorySelection[1][sizeof(FFPMemorySelectionEnum_t)];
    u8 FFPMemoryaddress[1][sizeof(H_UINT4)];
    u8 FFPMemoryBlockLength[1][sizeof(H_UCHAR)];
    u8 FFPMemoryContent[24][sizeof(H_UCHAR)];
    u8 endmark; //required marker
} Rsp_WriteFFPmemory_t;
static s8_least hartcmd_WriteFFPmemory(const u8 *src, u8 *dst)
{
    s8_least ret = HART_NO_COMMAND_SPECIFIC_ERRORS;
    const Req_WriteFFPmemory_t *s = (const void *)src;
    Rsp_WriteFFPmemory_t *d = (void *)dst;
    UNUSED_OK(d->FFPMemorySelection); //rely on framework
    UNUSED_OK(d->FFPMemoryaddress); //rely on framework
    UNUSED_OK(d->FFPMemoryBlockLength); //rely on framework
    UNUSED_OK(d->FFPMemoryContent); //rely on framework
    FFPMemorySelectionEnum_t memtype = (FFPMemorySelectionEnum_t)util_GetU8(s->FFPMemorySelection[0]);
    u32 addr = util_GetU32(s->FFPMemoryaddress[0]);
    u16 len = util_GetU8(s->FFPMemoryBlockLength[0]);
    switch(memtype)
    {
        case FFPNVMEM:
        {
            ErrorCode_t err = eep_write_block((u16)addr, s->FFPMemoryContent[0], len);
            if(err != ERR_OK)
            {
                ret = TRANSMITTER_SPECIFIC_COMMAND_ERROR;
            }
            break;
        }
        case FFPRAM:
        {
            memcpy(HARDWARE(void *, addr), s->FFPMemoryContent[0], len);
            break;
        }
        default:
        {
            ret = HART_PASSED_PARAMETER_TOO_LARGE;
            break;
        }
    }
    return ret;
}

    //--------- subcommand 5 --------------------
typedef struct Req_ReadFFPNVMEMheader_t
{
    u8 endmark; //required marker
} Req_ReadFFPNVMEMheader_t;
typedef struct Rsp_ReadFFPNVMEMheader_t
{
    u8 FFPNVMEMNumberOfBlocks[1][sizeof(H_UCHAR)];
    u8 FFPNVMEMSizeofBlocks[1][sizeof(H_UINT4)];
    u8 FFPNVMEMBlocksBaseAddress[1][sizeof(H_UINT4)];
    u8 FFPNVMEMBlockDescriptorSize[1][sizeof(H_UINT)];
    u8 endmark; //required marker
} Rsp_ReadFFPNVMEMheader_t;
static s8_least hartcmd_ReadFFPNVMEMheader(const u8 *src, u8 *dst)
{
    T_EEP_INFO info;
    eep_GetInfo(&info);
    const Req_ReadFFPNVMEMheader_t *s = (const void *)src;
    Rsp_ReadFFPNVMEMheader_t *d = (void *)dst;
    UNUSED_OK(s);
    util_PutU8(d->FFPNVMEMNumberOfBlocks[0], info.num_blocks);
    util_PutU32(d->FFPNVMEMSizeofBlocks[0], info.size_eep_data);
    util_PutU32(d->FFPNVMEMBlocksBaseAddress[0], info.start_data);
    util_PutU16(d->FFPNVMEMBlockDescriptorSize[0], sizeof(T_EEP_BLOCK_DSC));
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

    //--------- subcommand 6 --------------------
typedef struct Req_ReadFFPNVMEMblocksdirectory_t
{
    u8 FFPNVMEMBlockSelector[1][sizeof(H_UCHAR)];
    u8 endmark; //required marker
} Req_ReadFFPNVMEMblocksdirectory_t;
typedef struct Rsp_ReadFFPNVMEMblocksdirectory_t
{
    u8 FFPNVMEMBlockSelector[1][sizeof(H_UCHAR)];
    u8 FFPNVMEMBlocksBaseAddress[1][sizeof(H_UINT4)];
    u8 FFPNVMEMBlockSize[1][sizeof(H_UINT4)];
    u8 endmark; //required marker
} Rsp_ReadFFPNVMEMblocksdirectory_t;
static s8_least hartcmd_ReadFFPNVMEMblocksdirectory(const u8 *src, u8 *dst)
{
    const Req_ReadFFPNVMEMblocksdirectory_t *s = (const void *)src;
    Rsp_ReadFFPNVMEMblocksdirectory_t *d = (void *)dst;
    UNUSED_OK(d->FFPNVMEMBlockSelector); //rely on framework
    u8 blocknum = util_GetU8(s->FFPNVMEMBlockSelector[0]);
    s8_least ret;
    if(blocknum >= EEP_NBR_OF_BLOCKS)
    {
        ret = HART_PASSED_PARAMETER_TOO_LARGE;
    }
    else
    {
        u32 offset;
        u32 length;
        eep_GetBlockInfo(blocknum, &offset, &length);
        util_PutU32(d->FFPNVMEMBlocksBaseAddress[0], offset);
        util_PutU32(d->FFPNVMEMBlockSize[0], length);
        ret = HART_NO_COMMAND_SPECIFIC_ERRORS;
    }
    return ret;
}

    //--------- subcommand 7 --------------------
typedef struct Req_ReadFFPNVMEMTestStatistics_t
{
    u8 endmark; //required marker
} Req_ReadFFPNVMEMTestStatistics_t;
typedef struct Rsp_ReadFFPNVMEMTestStatistics_t
{
    u8 FFPNVMEMFailedBlock[1][sizeof(H_UCHAR)];
    u8 FFPNVMEMSuccessfulRepairs[1][sizeof(H_UINT4)];
    u8 FFPNVMEMFailedRepairs[1][sizeof(H_UINT4)];
    u8 FFPNVMEMmaxWriteTime[1][sizeof(H_UINT4)];
    u8 endmark; //required marker
} Rsp_ReadFFPNVMEMTestStatistics_t;
static s8_least hartcmd_ReadFFPNVMEMTestStatistics(const u8 *src, u8 *dst)
{
    const Req_ReadFFPNVMEMTestStatistics_t *s = (const void *)src;
    Rsp_ReadFFPNVMEMTestStatistics_t *d = (void *)dst;
    UNUSED_OK(s);
    u32 repair_count;
    u32 repair_failed;
    u32 maxtime;
    u8 failed_block = eeptest_GetStats(&repair_count, &repair_failed, &maxtime);
    util_PutU8(d->FFPNVMEMFailedBlock[0], failed_block);
    util_PutU32(d->FFPNVMEMSuccessfulRepairs[0], repair_count);
    util_PutU32(d->FFPNVMEMFailedRepairs[0], repair_failed);
    util_PutU32(d->FFPNVMEMmaxWriteTime[0], maxtime);
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

    //--------- subcommand 8 --------------------
typedef struct Req_TestWorstEEPNVMEMWriteTime_t
{
    u8 FFPNVMEMForgetLastResult[1][sizeof(H_CHAR)];
    u8 FFPNVMEMRequestAllWrites[1][sizeof(H_CHAR)];
    u8 endmark; //required marker
} Req_TestWorstEEPNVMEMWriteTime_t;
typedef struct Rsp_TestWorstEEPNVMEMWriteTime_t
{
    u8 endmark; //required marker
} Rsp_TestWorstEEPNVMEMWriteTime_t;
static s8_least hartcmd_TestWorstEEPNVMEMWriteTime(const u8 *src, u8 *dst)
{
    const Req_TestWorstEEPNVMEMWriteTime_t *s = (const void *)src;
    Rsp_TestWorstEEPNVMEMWriteTime_t *d = (void *)dst;
    UNUSED_OK(d);
    u8 fresh = util_GetU8(s->FFPNVMEMForgetLastResult[0]);
    u8 stress = util_GetU8(s->FFPNVMEMRequestAllWrites[0]);
    eeptest_WriteAllBlocks(fresh, stress);
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}


    //--------- subcommand 9 --------------------
typedef struct Req_ReadFFPActiveFlashBank_t
{
    u8 endmark; //required marker
} Req_ReadFFPActiveFlashBank_t;
typedef struct Rsp_ReadFFPActiveFlashBank_t
{
    u8 FFPFlashBank[1][sizeof(H_UCHAR)];
    u8 endmark; //required marker
} Rsp_ReadFFPActiveFlashBank_t;
static s8_least hartcmd_ReadFFPActiveFlashBank(const u8 *src, u8 *dst)
{
	const Req_ReadFFPActiveFlashBank_t *s = (const void *)src;
	UNUSED_OK(s);
	Rsp_ReadFFPActiveFlashBank_t *d = (void *)dst;
	util_PutU8(d->FFPFlashBank[0], flash_GetRunningPartitionID());
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}



static void mini_wrapper(u8 *src, u8 *dst)
{
    mn_memcpy(dst + FFACT_DATA_OFFSET, src + 2, MIN(sizeof(((T_FBIF_PTB *)NULL)->factory_use_2) - FFACT_DATA_OFFSET, src[1])); //copy request to response
    s8_least ret;
    u8 *subdata_src = src + 3;
    u8 *subdata_dst = dst + FFACT_DATA_OFFSET + 1;
    switch(src[2]) //subcommands
    {
        case 3: //read memory
        {
            ret = hartcmd_ReadFFPmemory(subdata_src, subdata_dst);
            break;
        }
        case 4: //write memory
        {
            ret = hartcmd_WriteFFPmemory(subdata_src, subdata_dst);
            break;
        }
        case 5: //read NVMEM header
        {
            ret = hartcmd_ReadFFPNVMEMheader(subdata_src, subdata_dst);
            break;
        }
        case 6: //read NVMEM block directory
        {
            ret = hartcmd_ReadFFPNVMEMblocksdirectory(subdata_src, subdata_dst);
            break;
        }
        case 7: //read NVMEM selftest stats
        {
            ret = hartcmd_ReadFFPNVMEMTestStatistics(subdata_src, subdata_dst);
            break;
        }
        case 8: //test NVMEM write load
        {
            ret = hartcmd_TestWorstEEPNVMEMWriteTime(subdata_src, subdata_dst);
            break;
        }
        case 9: //Active flash bank
        {
            ret = hartcmd_ReadFFPActiveFlashBank(subdata_src, subdata_dst);
            break;
        }

        default:
            //we say "command not implemented"
            ret = COMMAND_NOT_IMPLEMENTED;
            break;
    }
    dst[2] = (u8)ret;
}


/** \brief Command 255 intercept handler
\param p_PTB - a pointer to TB parameters
*/
static void ffact_WriteIntercept(T_FBIF_PTB *p_PTB)
{
    switch(p_PTB->factory_use_1[2]) //subcommands
    {
#if USE_ASSERT_STUB
        case 0: //trap records
            mn_memset(&p_PTB->factory_use_2[FFACT_DATA_OFFSET], 0, ASSERT_HEADER_SIZE); //for today's crippled implementation
            mn_memcpy(&p_PTB->factory_use_2[FFACT_DATA_OFFSET]+ASSERT_HEADER_SIZE, (const void *)&assert_msg, sizeof(assert_msg)); //lint !e960 cast away volatile - intended
            p_PTB->factory_use_2[2] = HART_NO_COMMAND_SPECIFIC_ERRORS;
            break;
#endif
        case 2: //Enable/disable IPC traffic
        {
            u8 rsp = HART_NO_COMMAND_SPECIFIC_ERRORS;
            switch(p_PTB->factory_use_1[3])
            {
                case 0: //Disable regular IPC traffic
                    HART_set_active(DEACTIVATE, DEACTIVATE);
                    break;
                case 1: //Enable regular IPC traffic
                    HART_set_active(ACTIVATE, ACTIVATE);
                    break;
                default:
                    rsp = HART_INVALID_SELECTION;
                    break;
            }
			p_PTB->factory_use_2[FFACT_DATA_OFFSET+1] = p_PTB->factory_use_1[3]; //command data
            p_PTB->factory_use_2[2] = rsp;
            break;
        }
        default:
            mini_wrapper(p_PTB->factory_use_1, p_PTB->factory_use_2);
            break;
    }
    p_PTB->factory_use_2[FFACT_DATA_OFFSET] = p_PTB->factory_use_1[2]; //subcommand
    p_PTB->factory_use_2[3] = 0; //bogus status 2
}

/** \brief Handler of Write to FACTORY_USE_1
Processes a "HART command" in FACTORY_USE_1 to
\param p_block_instance - a pointer to block instance
\return E_OK
*/
fferr_t ffact_WriteFactoryUse1(const T_FBIF_BLOCK_INSTANCE* p_block_instance, void* snd_buf, void* rcv_buf)
{
    UNUSED_OK(snd_buf);
    UNUSED_OK(rcv_buf);
    u8_least i;

    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;
    //clear factory_use_2 PTB array before filling it from HART
    for ( i = 2; i < NELEM(p_PTB->factory_use_2); i++ )
    {
        p_PTB->factory_use_2[i] = 0x0;
    }
    //write command number to factory_use_2 to return it in the read
    p_PTB->factory_use_2[0] = p_PTB->factory_use_1[0];
    p_PTB->factory_use_2[1] = 0xFF; // delimiter between cmd number and returned values

    if(p_PTB->factory_use_1[0] == 0xFF)
    {
        //This is fake command 255==0xFF intercepted here
        ffact_WriteIntercept(p_PTB);
    }
    else
    {
        //send the requested HART command
        (void)mn_HART_acyc_cmd( p_PTB->factory_use_1[0], &p_PTB->factory_use_1[2],
                                p_PTB->factory_use_1[1], &p_PTB->factory_use_2[2] );
    }

    return E_OK;
}

fferr_t ffact_ReadFactoryUse1or2(const T_FBIF_BLOCK_INSTANCE* p_block_instance, void* snd_buf, void* rcv_buf)
{
    UNUSED_OK(snd_buf);
    UNUSED_OK(rcv_buf);
    //clear factory_use_1 PTB array
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;
    mn_memset(p_PTB->factory_use_1, 0xFF, sizeof(p_PTB->factory_use_1));
    return E_OK;
}
/* This line marks the end of the source */
