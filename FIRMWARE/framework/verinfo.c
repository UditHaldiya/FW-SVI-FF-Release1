/*
Copyright 2005-2007 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \addtogroup proj Project definitions (ESD)
    \brief A collection of project-defining compile-time constants

*/
/**
    \file verinfo.c
    \brief FF device version info for firmware download

    CPU: Any

    OWNER: EP
    $Archive: /MNCB/Dev/AP_Release_3.1.x/FIRMWARE/interface/verinfo.c $
    $Date: 7/14/11 5:27p $
    $Revision: 2 $
    $Author: Arkkhasin $

    \ingroup proj
*/
/* (Optional) $History: verinfo.c $
*/
#include "mnwrap.h"
#include "mncbdefs.h"
#include "mn_flash.h"

//lint ++flb
typedef struct endimage_t
{
    VerInfo_t *verinfo;
    u16     checksum;
} endimage_t;
//extern void *__vector_table;

#define STACK_VECT_NO       0
#define END_IMAGE_VECT_NO   76
#define DOMAIN_VECT_NO      77
#define LOW_RAM_ADDR        0x20000000
#define HIGH_RAM_ADDR       0x20018000
#define LOW_FLASH_ADDR      0x8000000
//lint -flb

/** \brief Return a pointer to VerString either in the running partition
    or the other bank. This function uses a pointer in the vector table.
    \param inactive - true returns address from other bank; false from running bank 

*/
static const VerInfo_t *GetVerInfo(bool_t inactive, FFInfo_t *ffi)
{
    const VerInfo_t *retval = NULL;

    if (inactive)
    {
        // get base address of non-running bank
        u32 flashbase = flash_GetPartitionBaseAddress(0);
        u32 *vectors = HARDWARE(u32*, flashbase);
//        u32 *vectors = HARDWARE(u32*, LOW_FLASH_ADDR); // for testing
        ffi->bank = (HARDWARE(u32, (void*)vectors) >= BANK2_BASE) ? 1 : 0;
 
        // valid partition has valid stack pointer at vector 0
        if ((vectors[STACK_VECT_NO] >= LOW_RAM_ADDR) && (vectors[STACK_VECT_NO] <= HIGH_RAM_ADDR))
        {
            u32 vval = vectors[END_IMAGE_VECT_NO];
            
            // valid vector 76 points within flash (could be refined to bank
            if ((vval >= flashbase)  && (vval < (flashbase + 0x80000)))
            {
                endimage_t *rv = HARDWARE(endimage_t *, vval - sizeof(VerInfo_t *)); 
                u32 pval = HARDWARE(u32, (void*)rv->verinfo);
                
                // valid rv is less than vval and > vectors
                if ((pval < vval) && (pval > HARDWARE(u32, (void*)vectors)))
                {                
                    retval = rv->verinfo;
                }
            }
        }  
    }
    else
    {
        ffi->bank = (HARDWARE(u32, GetVerInfo) >= BANK2_BASE) ? 1 : 0; //lint !e506 GetVerInfo is not constant when linked at a different address
        retval    = &VerString[0];
    }
    return retval;
}

static void getinfo(FFInfo_t *ffi, bool_t psel)
{
    const VerInfo_t *vi;

    memset(ffi, 0, sizeof(FFInfo_t));
    vi = GetVerInfo(psel, ffi);     
    if (vi != NULL)
    {
        ffi->sw_revision            = vi->sw_revision;
        ffi->trans_spec_revision    = vi->trans_spec_revision;
        ffi->hw_revision            = vi->hw_revision;
        ffi->fw_type                = vi->fw_type;
        ffi->ManufacturerDeviceCode = vi->ManufacturerDeviceCode;
        memcpy(ffi->date_str, vi->date_str, 4);
    }
}

s8_least GetFwInfo(void *buf)
{
    FFInfo_t *ffi = buf;

    getinfo(ffi++, false); 
    getinfo(ffi++, true);

    return (s8_least)(HARDWARE(u32, ffi) - HARDWARE(u32, buf)); 
}

/* End of source */
