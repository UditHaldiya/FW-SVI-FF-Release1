/*****************************************************************************
*                                                                            *
*                     SOFTING Industrial Automation GmbH                     *
*                          Richard-Reitzner-Allee 6                          *
*                                D-85540 Haar                                *
*                        Phone: ++49-89-4 56 56-0                            *
*                          Fax: ++49-89-4 56 56-3 99                         *
*                                                                            *
*                            SOFTING CONFIDENTIAL                            *
*                                                                            *
*                     Copyright (C) SOFTING IA GmbH 2012                     *
*                             All Rights Reserved                            *
*                                                                            *
* NOTICE: All information contained herein is, and remains the property of   *
*   SOFTING Industrial Automation GmbH and its suppliers, if any. The intel- *
*   lectual and technical concepts contained herein are proprietary to       *
*   SOFTING Industrial Automation GmbH and its suppliers and may be covered  *
*   by German and Foreign Patents, patents in process, and are protected by  *
*   trade secret or copyright law. Dissemination of this information or      *
*   reproduction of this material is strictly forbidden unless prior         *
*   written permission is obtained from SOFTING Industrial Automation GmbH.  *
******************************************************************************
******************************************************************************
*                                                                            *
* PROJECT_NAME             Softing FF/PA FD 2.42                             *
*                                                                            *
* VERSION                  FF - 2.42                                         *
*                          PA - 2.42 (beta)                                  *
*                                                                            *
* DATE                     16. April 2012                                    *
*                                                                            *
*****************************************************************************/

/* ===========================================================================

FILE_NAME          eep_if.c



FUNCTIONAL_MODULE_DESCRIPTION

=========================================================================== */
#include "keywords.h"
#define  MODULE_ID      (COMP_EEP + MOD_EEPIF)


INCLUDES
#include "base.h"
#include "hw_cfg.h"
#include "hw_if.h"
#include "eep_if.h"
#include <hw_i2c.h> //for i2c mutex
#include <eep_hw.h> //for T_EEP_ADDR

#include "osif.h"

#include "except.h"
#include "dbg_out.h"

#include "mnassert.h"
#include "fram.h"
#include <ffbl_api.h>

GLOBAL_DEFINES
#define USE_CRC_HARDWARE


LOCAL_DEFINES

#define EEP_CLEAR_VALUE     0           /* EEP cell value after clear       */
#define EEP_INIT_VALUE      'q'         /* EEP RAM image value after init   */

#define RETRY_LIMIT         3           /* Retry limit for unsucc. reads    */

#define EEP_START_INIT      0           /* EEP init procedure was executed. */
#define EEP_START_ACT       1           /* EEP act procedure was executed.  */

#define VERIFY_BUFFER_SIZE  256         /* Buffer for cyclic verification   */
                                        /* of the NV RAM and the RAM image. */

typedef enum _T_EEP_SERVICE
{
    EEP_WRITE   = 1,
    EEP_READ,
    EEP_VERIFY,
    EEP_CONFIG,
    EEP_INFO
} T_EEP_SERVICE;

#define EEP_NUM_COPIES 2

//FUNCTION_DECLARATIONS

LOCAL VOID          eep_task_main (T_EVENT event);
static T_EEP_RESULT  eep_task_write(const T_EEP_DSC *p_blk_dsc);
static T_EEP_RESULT  eep_task_config(void);

LOCAL T_EEP_RESULT  eep_init_blocks (int retry_limit);
LOCAL T_EEP_RESULT  eep_read_info (VOID);
LOCAL USIGN16       eep_build_dscr (VOID);
LOCAL USIGN16       eep_config_sort (VOID);
LOCAL T_EEP_RESULT  eep_param_check (USIGN8 block_id, USIGN16 offset, USIGN16 length);
LOCAL T_EEP_RESULT  eep_put (USIGN8 block_id, USIGN16 offset, USIGN16 length);
LOCAL VOID          eep_set_cs (T_EEP_DSC *block_dscr);

EXPORT_DATA

IMPORT_DATA

LOCAL_DATA

#define EEP_WRITE_BUFFERSZ (5*1024)

#if 0
#pragma section = "EEP_IMAGE"
NO_INIT static struct eep_image_t
{
    u8 eep_ram_imageX[USABLE_EEPROM_SIZE];
    u16 write_bufferX[EEP_WRITE_BUFFERSZ/sizeof(u16)];
    u16 testpadX[EEP_NUM_COPIES][EEP_WRITE_BUFFERSZ/sizeof(u16)];
} eep_stuff @ "EEP_IMAGE";
#define eep_ram_image (eep_stuff.eep_ram_imageX)
#define write_buffer (eep_stuff.write_bufferX)
#define testpad (eep_stuff.testpadX)
#else
#pragma location="EEP_IMAGE"
static    u8 eep_ram_image[USABLE_EEPROM_SIZE];
#pragma location="EEP_IMAGE"
static    u16 write_buffer[EEP_WRITE_BUFFERSZ/sizeof(u16)];
#pragma location="EEP_IMAGE"
static    u16 testpad[EEP_NUM_COPIES][EEP_WRITE_BUFFERSZ/sizeof(u16)];
#endif

#include "da_eep.h"                               /* DATA SEGMENT DEFINITION */

//NO_INIT STATIC USIGN8            eep_ram_image[USABLE_EEPROM_SIZE];
NO_INIT STATIC T_EEP_DSC         eep_dscr[EEP_NBR_OF_BLOCKS];
NO_INIT STATIC T_EEP_BLOCK_DSC   eep_block_cfg[EEP_NBR_OF_BLOCKS];
NO_INIT STATIC T_EEP_INFO        eep_info;
NO_INIT STATIC BOOL              eep_init_sequence;


#include "da_def.h"                                 /* DEFAULT DATA SEGMENT */

//--------------- testing support API implementation -------------------------
/** \brief provides artifacts of eep_info
*/
void eep_GetInfo(T_EEP_INFO *dst)
{
    if(dst != NULL)
    {
        osif_disable_all_tasks();
        *dst = eep_info;
        osif_enable_all_tasks();
    }
}

/** \brief Provides a copy of eep block info
\param b - requested block number
\param dst - a pointer to destination
*/
void eep_GetBlockInfo(u8 b, u32 *offset, u32 *length)
{
    osif_disable_all_tasks();
    if(b < eep_info.num_blocks)
    {
        *offset = (u8 *)eep_dscr[b].checksum - eep_ram_image;
        u32 header_size = (const u8 *)eep_dscr[b].start_addr - (const u8 *)eep_dscr[b].checksum;
        *length = header_size + eep_dscr[b].len;
    }
    else
    {
        *offset = 0;
        *length = 0;
    }
    osif_enable_all_tasks();
}


/*######################  EEP Interface Functions  #########################*/

//-------------- pass-through helpers -----------------

/** \brief pass-through write helper
\param eeprom_addr - EEPROM address
\param src_addr - source address
\param count - number of bytes
\return completion code - 0 iff error
*/
ErrorCode_t eep_write_block(u16 eeprom_addr, const void *src_addr, u16 count)
{
    ErrorCode_t err;
    i2c_AcquireMutex();
    err = fram_WriteExt(src_addr, eeprom_addr, count, 0);
    i2c_ReleaseMutex();
    return err;
}

/** \brief pass-through read helper
\param dst_addr - destination address
\param eeprom_addr - EEPROM address
\param count - number of bytes
\return completion code - 0 iff error
*/
ErrorCode_t eep_read_block(void *dst_addr, u32 eeprom_addr, u16 count)
{
    ErrorCode_t err;
    i2c_AcquireMutex();
    err = fram_ReadExt(dst_addr, eeprom_addr, count, 0);
    i2c_ReleaseMutex();
    return err;
}

/* EXPLANATION:
NvMutex aims at protecting NVMEM access from non-reentrant data structures in RAM
all the way down to physical layer.
This means that it must be acquired/released at a fairly high level.

Currently, we don't have access to the write sequences
<eep_provide_block,eep_start_write,eep_write>
made by Softing code provided without source.

So, we use a second (internal) mutex to protect against interactions between
Softing accesses and ours, in a hope that Softing does their part w.r.t.
data structures correctly.
This internal mutex doesn't affect the API at the call points.

When/If Softing call points can be protected, the internal mutex stuff
will become a redundant time-consuming thing and could/should be removed
*/
#define MUTEX_ENABLED 1
#define MUTEX_CONFLICT_WATCH 0

#if MUTEX_ENABLED != 0
static OS_RSEMA NvMutex;
static OS_RSEMA NvMutex_Internal;
#endif

void eep_CreateMutex(void)
{
#if MUTEX_ENABLED != 0
    OS_CREATERSEMA(&NvMutex);
    OS_CREATERSEMA(&NvMutex_Internal);
#endif
    i2c_CreateMutex();
}

#if MUTEX_ENABLED != 0
#if MUTEX_CONFLICT_WATCH != 0

extern u8 tcb2index(const OS_TASK *tcb);

typedef struct mutex_conflict_t
{
    u8 requestor;
    u8 owner;
} mutex_conflict_t;

mutex_conflict_t Mutex_conflict[10];
u8 Mutex_conflict_count;

mutex_conflict_t LocalMutex_conflict[10];
u8 LocalMutex_conflict_count;

#endif
#endif

void eep_AcquireMutex(void)
{
#if MUTEX_ENABLED != 0
    if(OS_Request(&NvMutex) == 0)
    {
        //Mutex not available
#if MUTEX_CONFLICT_WATCH != 0
        u8 Mutex_conflict_count1 = Mutex_conflict_count;
        OS_TASK* tcb = OS_GetResourceOwner(&NvMutex);
        if(Mutex_conflict_count1 < NELEM(Mutex_conflict))
        {
            Mutex_conflict[Mutex_conflict_count1].owner = tcb2index(tcb);
            Mutex_conflict[Mutex_conflict_count1].requestor = osif_get_current_task_id();
            Mutex_conflict_count= Mutex_conflict_count1+1;
        }
#endif
        OS_Use(&NvMutex); //wait
    }
#endif
}

void eep_ReleaseMutex(void)
{
#if MUTEX_ENABLED != 0
    OS_Unuse(&NvMutex);
#endif
}

static void eep_AcquireLocalMutex(void)
{
#if MUTEX_ENABLED != 0
    if(OS_Request(&NvMutex_Internal) == 0)
    {
        //Mutex not available
#if MUTEX_CONFLICT_WATCH != 0
        u8 Mutex_conflict_count1 = LocalMutex_conflict_count;
        OS_TASK* tcb = OS_GetResourceOwner(&NvMutex_Internal);
        if(Mutex_conflict_count1 < NELEM(LocalMutex_conflict))
        {
            LocalMutex_conflict[Mutex_conflict_count1].owner = tcb2index(tcb);
            LocalMutex_conflict[Mutex_conflict_count1].requestor = osif_get_current_task_id();
            LocalMutex_conflict_count = Mutex_conflict_count1+1;
        }
#endif
        OS_Use(&NvMutex_Internal); //wait
    }
#endif
}

static void eep_ReleaseLocalMutex(void)
{
#if MUTEX_ENABLED != 0
    OS_Unuse(&NvMutex_Internal);
#endif
}



FUNCTION GLOBAL T_EEP_RESULT eep_init(VOID)
/*----------------------------------------------------------------------------

FUNCTIONAL_DESCRIPTION

Initializes the EEP interface: sets EEP block description structure, then
initializes the physical EEPROM device interface by calling "eeprom_init" and
finally reads all data from the EEPROM device to the RAM image.

!! CAUTION !! This function is NOT reentrant. It may be executed only during
initialization before tasks are started or when task scheduling is disabled!

RETURN_VALUE
    EEP_OK              => completed successfuly
    EEP_READ_ERR        => EEP info block could not be read
    EEP_CS_ERR          => checksum invalid in min. one block
    EEP_INCONSISTENT    => block descriptions are inconsistent

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
    T_EEP_RESULT            result;
    USIGN16                 retry_cnt;
    T_EEP_ADDR              data_size;

FUNCTION_BODY

    memset((USIGN8 *)eep_dscr, 0, sizeof(eep_dscr));
    retry_cnt = 0;
    eep_init_sequence = EEP_START_INIT;

    eep_AcquireMutex();

        /* Read data structure info from EEPROM device */
        /*---------------------------------------------*/
    do {
        result = eep_read_info ();
    } while ((result != EEP_OK) && (retry_cnt++ < RETRY_LIMIT));

    if ( result != EEP_OK )
    {
        DBG_PRINT_WAIT_( ("eep_init: 'eep_read_info ()' failed (code %d)\n", result) );
    }
    else if ( (eep_info.size_eep_data + sizeof (T_EEP_INFO) + (eep_info.num_blocks * sizeof (T_EEP_BLOCK_DSC))) > USABLE_EEPROM_SIZE )
    {
        eep_info.state = EEP_UNAVAIL;
        _EEP_WARNING(WRN_INCONS, 0, FKT_INIT);
        result = (EEP_NOT_AVAIL);
    }
    else
    {
        data_size = eep_build_dscr ();

        if ( data_size != eep_info.size_eep_data )
        {
            _EEP_WARNING(WRN_INCONS, 0, FKT_INIT);
            result = (EEP_NOT_AVAIL);
        }
        else
        {
            result = eep_init_blocks (RETRY_LIMIT - retry_cnt);
            if ( result != EEP_OK )
            {
                DBG_PRINT_WAIT_( ("eep_init: 'eep_init_blocks ()' failed (code %d)\n", result) );
            }
        }
    }

    eep_ReleaseMutex();

    //Only now can we create the task to handle writes and tests
    osif_create_task (EEP_TASK_ID, EEP_TASK_PRIO, eep_task_main, OS_IF_EVENT_EEPROM);

    return (result);
} /* eep_init */


//----------------- selftest business --------------------


typedef enum testphase_t
{
    eeptest_cat_r1, //read catalog first copy
    eeptest_cat_r2, //read catalog second copy
    eeptest_cat_w1, //write catalog first copy
    eeptest_cat_w2, //write catalog second copy

    eeptest_r1, //read block first copy
    eeptest_r2, //read block second copy
    eeptest_w1, //write block first copy
    eeptest_w2, //write block second copy
} testphase_t;
#define BLK_CATALOG ((u8)~0U)
static u8 Atomic_ block_under_test = BLK_CATALOG; //start with the catalog
static u8 Atomic_ bad_block = 0;
static testphase_t testphase = eeptest_cat_r1;
static T_EEP_RESULT prev_result;

/** \brief Read a single copy of an eep block into destination buffer
Pre-requisite: eep_dscr must have been built
\param dst - a pointer to the destination buffer
\param b - block id (no range check!)
\param copy - a copy number to read (must be < EEP_NUM_COPIES but no range check)
\return an EEP error code
*/
static T_EEP_RESULT eep_readblock_helper(void *dst, u8 b, u8 copy)
{
    T_EEP_RESULT ret;
    /* read EEP data */
    osif_disable_all_tasks();
    T_EEP_ADDR eep_addr = (T_EEP_ADDR) ((USIGN8 *)eep_dscr[b].checksum - eep_ram_image) + eep_info.start_data;
    size_t header_size = (const u8 *)eep_dscr[b].start_addr - (const u8 *)eep_dscr[b].checksum;
    size_t length = eep_dscr[b].len;
    osif_enable_all_tasks();
    size_t total_length = header_size + length;

    if ( eep_read_block(dst, eep_addr + copy*eep_info.size_eep_data,
                              total_length) != ERR_OK)
    {
        /* error in read command */
        ret= EEP_READ_ERR;
    }
    else
    {
        u16 cs = calc_crc ((u8 *)dst + header_size, length);

        if ( cs == *(u16*)dst )
        {
            ret = EEP_OK;
        }
        else
        {
            ret = EEP_CS_ERR;
        }
    }
    return ret;
}

/** \brief Write a single copy of an eep block from a source buffer
Pre-requisite: eep_dscr must have been built
\param src - a pointer to the source buffer
\param b - block id (no range check!)
\param copy - a copy number to write (must be < EEP_NUM_COPIES but no range check)
\return an EEP error code
*/
static T_EEP_RESULT eep_writeblock_helper(const void *src, const T_EEP_DSC *p_blk_dsc, u8 copy)
{
    osif_disable_all_tasks();
    size_t total_length = ((const u8 *)p_blk_dsc->start_addr - (const u8 *)p_blk_dsc->checksum) + p_blk_dsc->len;
    T_EEP_ADDR eep_addr = (T_EEP_ADDR) ((USIGN8 *)p_blk_dsc->checksum - eep_ram_image) + eep_info.start_data;
#ifndef NDEBUG
    u8 b = p_blk_dsc->id; //block id
#endif
    osif_enable_all_tasks();
    T_EEP_RESULT ret = EEP_PRG_ERR;

    /* write EEP data */
    if (ERR_OK != eep_write_block (eep_addr + copy*eep_info.size_eep_data,
                              src, total_length) )
    {
        /* error in write command */
        _EEP_WARNING(WRN_EEPWRT, b, (USIGN32) eep_addr);
    }
    else
    {
        ret = EEP_OK;
    }
    return ret;
}

#ifdef _lint
#define LintCritSect(...) do {osif_disable_all_tasks(); __VA_ARGS__; osif_enable_all_tasks(); } while(0)
#else
#define LintCritSect(...) __VA_ARGS__
#endif

/** \brief Initialize eep test for the next block
*/
static void nextblock(void)
{
    testphase = eeptest_r1;
    u8 b = block_under_test + 1; //wraps around to 0 after BLK_CATALOG
    if(b == EEP_NBR_OF_BLOCKS)
    {
        b = BLK_CATALOG;
        testphase = eeptest_cat_r1;
    }
    else
    {
        testphase = eeptest_r1;
    }
    block_under_test = b;
}

/** \brief Phase 1 of eep block test: read the block under test into testpad[0]
*/
static void eeptest_eepblock_r1(void)
{
    u8 b = block_under_test;
    size_t len;
    LintCritSect(len = eep_dscr[b].len); //OK to peek at an atomic member
    if (len == 0) //empty block
    {
        nextblock();
    }
    else
    {
        prev_result = eep_readblock_helper(testpad[0], b, 0);
        testphase = eeptest_r2;
    }
}

/** \brief Marks first bad block and advances the test to the next
*/
static void badblock(void)
{
    //both copies are bad
    if(bad_block == 0)
    {
        bad_block = block_under_test;
    }
    //no hope: - next block
    nextblock();
}


/** \brief Phase 2 of eep block test: read the block under test (copy 2) into testpad[1],
compare and decide whether repair is needed
*/
static void eeptest_eepblock_r2(void)
{
    u8 b = block_under_test;
    size_t len;
    LintCritSect(len = eep_dscr[b].len); //OK to peek at an atomic member
    T_EEP_RESULT result = eep_readblock_helper(testpad[1], b, 1);
    if(prev_result == EEP_OK)
    {
        if(memcmp(testpad[0], testpad[1], len) == 0)
        {
            if(bad_block == b)
            {
                bad_block = 0;
            }
            //binary match - next block
            nextblock();
        }
        else
        {
            testphase = eeptest_w2;
        }
    }
    else
    {
        if(result == EEP_OK)
        {
            testphase = eeptest_w1;
        }
        else
        {
            badblock();
        }
    }
}

#define EEP_DIAG_REPORT_WORST_BLOCK 0
#define EEP_DIAG_ALLOW_DISABLE_WRITE 0

#if EEP_DIAG_REPORT_WORST_BLOCK
u32 Atomic_ eep_repair_count[EEP_NBR_OF_BLOCKS];
u8 Atomic_ worst_block_id;
#else
u32 Atomic_ eep_repair_count;
#endif

u32 Atomic_ eep_repair_failed;

/** \brief Phase 3 of eep block test: repair a failed copy from the opposite testpad[]
\param copy - a copy to write to (0 or 1)
*/
static void eeptest_eepblock_w(u8 copy)
{
    T_EEP_DSC *p;
    LintCritSect(p = &eep_dscr[block_under_test]); // Lint mistakenly believes that taking address of means access to.
    T_EEP_RESULT result = eep_writeblock_helper(testpad[1U-copy], p, copy); //The source is opposite
    if(result == EEP_OK)
    {
#if EEP_DIAG_REPORT_WORST_BLOCK
        eep_repair_count[block_under_test]++;
        if(eep_repair_count[block_under_test] > eep_repair_count[worst_block_id])
        {
            worst_block_id = block_under_test;
        }
#else
        eep_repair_count++;
#endif
    }
    else
    {
        eep_repair_failed++;
    }
    nextblock();
}

/** \brief A helper to read a copy of epp catalog into supplied buffers
\param[out] info - a pointer to catalog header
\param[out] block_info - pointer to catalog of eep blocks
\param copy - a copy number (0 or 1)
\return EEP error code
*/
static T_EEP_RESULT eep_read_catalog_helper(T_EEP_INFO *info, T_EEP_BLOCK_DSC *block_info, u8 copy)
{
    T_EEP_RESULT result = EEP_OK;
    eep_AcquireLocalMutex();

    ErrorCode_t err = eep_read_block((u8 *)info, 0+copy*sizeof(T_EEP_INFO), sizeof(T_EEP_INFO));
    if(err != ERR_OK)
    {
        result = EEP_READ_ERR;
    }
    else
    {
        //"OK" path
        u16 crc = calc_crc ((void *)info, MN_OFFSETOF(T_EEP_INFO, CheckWord));
        if(crc != info->CheckWord)
        {
            result = EEP_CS_ERR;
        }
    }

    if(result == EEP_OK)
    {
        err = eep_read_block(block_info,
                               sizeof(T_EEP_INFO)*EEP_NUM_COPIES + copy*info->num_blocks * sizeof (T_EEP_BLOCK_DSC),
                               info->num_blocks * sizeof (T_EEP_BLOCK_DSC));
        if(err != ERR_OK)
        {
            result = EEP_READ_ERR;
        }
    }

    if(result == EEP_OK)
    {
        u16 crc = calc_crc ((u8 *)block_info, info->num_blocks * sizeof (T_EEP_BLOCK_DSC));
        if(crc != info->crc)
        {
            result = EEP_CS_ERR;
        }
    }
    eep_ReleaseLocalMutex();
    return result;
}

/** \brief Reads a copy of epp catalog into a test buffer
\param copy - a copy number (0 or 1)
\return EEP error code
*/
static T_EEP_RESULT eeptest_catr(u8 copy)
{
    T_EEP_INFO *info = (void *)testpad[copy];
    void *block_info = &info[1];
    return eep_read_catalog_helper(info, block_info, copy);
}

/** \brief Phase 1 of eep catalog test: read the catalog into testpad[0]
*/
static void eeptest_catr1(void)
{
    prev_result = eeptest_catr(0);
    testphase = eeptest_cat_r2;
}

/** \brief Phase 2 of eep catalog test: read the catalog (copy 2) into testpad[1]
and decide whether a repair is needed
*/
static void eeptest_catr2(void)
{
    T_EEP_RESULT result = eeptest_catr(1);
    if(prev_result == EEP_OK)
    {
        T_EEP_INFO *info = (void *)testpad[0];
        if( (result == EEP_OK)
           && (memcmp(testpad[0], testpad[1], sizeof(T_EEP_INFO) + info->num_blocks * sizeof (T_EEP_BLOCK_DSC)) == 0) )
        {
            //match: OK
            nextblock();
        }
        else
        {
            testphase = eeptest_cat_w2;
        }
    }
    else
    {
        //will try to restore
        if(result == EEP_OK)
        {
            testphase = eeptest_cat_w1;
        }
        else
        {
            badblock();
        }
    }
}

/** \brief A helper to write a copy of epp catalog from supplied buffers
\param[in] info - a pointer to catalog header
\param[in] block_info - pointer to catalog of eep blocks
\param copy - a copy number (0 or 1) to write to
\return EEP error code
*/
static T_EEP_RESULT eep_writecat_helper(const T_EEP_INFO *info, const T_EEP_BLOCK_DSC *block_cfg, u8 copy)
{
    T_EEP_RESULT ret = EEP_OK;
    /* write EEP structure data */
    /*--------------------------*/
    eep_AcquireLocalMutex();
    if (ERR_OK != eep_write_block (sizeof(T_EEP_INFO)*EEP_NUM_COPIES + copy*info->num_blocks * sizeof (T_EEP_BLOCK_DSC),
                              (USIGN8 *)block_cfg,
                              info->num_blocks * sizeof (T_EEP_BLOCK_DSC)) )
    {
        ret = EEP_PRG_ERR;
    }

    if(ret == EEP_OK)
    {
        if (ERR_OK != eep_write_block (0 + copy*sizeof (T_EEP_INFO), (USIGN8 *)info, sizeof (T_EEP_INFO)) )
        {
            ret = EEP_PRG_ERR;
        }
    }
    eep_ReleaseLocalMutex();
    return ret;
}

/** \brief Phase 3 of eep catalog test: repair a failed copy from the opposite testpad[]
\param copy - a copy to write to (0 or 1)
*/
static void eeptest_catw(u8 copy)
{
    T_EEP_INFO *info = (void *)testpad[1U-copy]; //source copy is the opposite
    void *block_cfg = &info[1];

    T_EEP_RESULT result = eep_writecat_helper(info, block_cfg, copy);
    if(result == EEP_OK)
    {
#if EEP_DIAG_REPORT_WORST_BLOCK
        eep_repair_count[0]++;
        if(eep_repair_count[0] > eep_repair_count[worst_block_id])
        {
            worst_block_id = 0;
        }
#else
        eep_repair_count++;
#endif
    }
    else
    {
        eep_repair_failed++;
    }
    nextblock();
}

/** \brief A simple dispatcher of eep self-tests
*/
static void eeptest_eepblock(void)
{
    Reset_Watchdog();
    switch(testphase)
    {
        case eeptest_cat_r1:
            eeptest_catr1();
            break;
        case eeptest_cat_r2:
            eeptest_catr2();
            break;
        case eeptest_cat_w1:
            eeptest_catw(0); //from 2 to 1
            break;
        case eeptest_cat_w2:
            eeptest_catw(1); //from 1 to 2
            break;
        case eeptest_r1:
            eeptest_eepblock_r1();
            break;
        case eeptest_r2:
            eeptest_eepblock_r2();
            break;
        case eeptest_w1:
            eeptest_eepblock_w(0); //from 2 to 1
            break;
        case eeptest_w2:
            eeptest_eepblock_w(1); //from 1 to 2
            break;
        default: //error: reinit
            testphase = eeptest_cat_r1;
            block_under_test = BLK_CATALOG;
            break;
    }
}

static u32 Atomic_ max_allwrite_time = 0;

/** \brief Provides access to internal stats
\param repair_count - a pointer to repair attempts
\param repair_failed - a pointer to repair failures
\param eep_write_time - a pointer max write time
\return first failed block
*/
u8 eeptest_GetStats(u32 *repair_count, u32 *repair_failed, u32 *eep_write_time)
{
    if(repair_failed != NULL)
    {
        *repair_failed = eep_repair_failed;
    }
    if(repair_count != NULL)
    {
#if EEP_DIAG_REPORT_WORST_BLOCK
        *repair_count = eep_repair_count[worst_block_id];
#else
        *repair_count = eep_repair_count;
#endif
    }
    if(eep_write_time != NULL)
    {
#if EEP_DIAG_REPORT_WORST_BLOCK
        *eep_write_time = (worst_block_id==0)?BLK_CATALOG:worst_block_id;
#else
        *eep_write_time = max_allwrite_time;
#endif
    }
    return bad_block;
}

#if EEP_DIAG_ALLOW_DISABLE_WRITE
static bool_t Atomic_ eep_write_disabled = false;
#endif

/** \brief A test API to inspect behavior (and time consumed) when all
eep blocks are written. Inspect max time by calling eeptest_GetStats().
\param clear_max_time - iff non-0, forget previous max time measument
\param full_load - iff non-0, force all eep blocks to be written.
*/
void eeptest_WriteAllBlocks(u8 clear_max_time, u8 full_load)
{
    if(clear_max_time != 0)
    {
        max_allwrite_time = 0;
    }
    switch(full_load)
    {
        case 1:
        {
            for(u8_least b=1; b<EEP_NBR_OF_BLOCKS; b++)
            {
                (void)eep_put(b, 0, 0);
            }
            break;
        }
#if EEP_DIAG_ALLOW_DISABLE_WRITE
        case 2:
        {
            eep_write_disabled = true;
            break;
        }
#endif
        default:
        {
#if EEP_DIAG_ALLOW_DISABLE_WRITE
            eep_write_disabled = false;
#endif
            break;
        }
    }
}

//----------------- end selftest business --------------------

/****************************************************************************/

static T_EEP_RESULT eep_init_blocks (s8_least retry_limit)
/*----------------------------------------------------------------------------

FUNCTIONAL_DESCRIPTION

Checks the EEP blocks for consistency and initializes them to ROM defaults
if necessary

Pre-requisite: eep_dscr must have been built

RETURN_VALUE
    EEP_OK         => no error occured
    EEP_CS_ERR     => at least one block checksum inconsistent
    EEP_READ_ERR   => EEPROM could not be read

----------------------------------------------------------------------------*/
{
    T_EEP_RESULT  ret = EEP_OK; //OK if 0 blocks

    do
    {
        eep_AcquireLocalMutex();
        memset((USIGN8 *)&eep_ram_image, EEP_INIT_VALUE, sizeof(eep_ram_image));

        //Read one block at a time because of the watchdog (tickled automatically in the driver)
        for (u8_least b=0; b<EEP_NBR_OF_BLOCKS; b++)
        {
            for(u8_least i=0; i<EEP_NUM_COPIES; i++)
            {
                if (eep_dscr[b].len != 0) //non-empty block
                {
                    /* read EEP data */
                    ret = eep_readblock_helper(eep_dscr[b].checksum, b, i);
                    if(ret == EEP_READ_ERR)
                    {
                        eep_info.state = EEP_UNAVAIL; //mimic what Softing original did
                    }
                    if(ret == EEP_OK)
                    {
                        break; //good read
                    }
                }
            }
        }
        eep_ReleaseLocalMutex();
    } while ((ret != EEP_OK) && (retry_limit-- > 0));

    return (ret);
} /* eep_init_blocks */


/****************************************************************************/

FUNCTION GLOBAL T_EEP_RESULT eep_provide_block
  (
    IN  USIGN8      block_id,           /* ID of block                              */
    IN  USIGN16     block_len,          /* size of block                            */
    OUT VOID * *    p_ram_image         /* pointer to data image                    */
  )
/*----------------------------------------------------------------------------

FUNCTIONAL_DESCRIPTION

Checks existence / consistency of a block and provides a pointer to the block
start address, if successful, NULL in case of error.

RETURN_VALUE
    EEP_OK              => no error, '*p_ram_image' points to start of block
    EEP_NOT_EXIST       => block with ID 'block_id' does not exist
    EEP_INCONSISTENT    => size of block does not agree with 'block_len'
    EEP_CS_ERR          => checksum of block invalid -> data probably corrupt
    EEP_READ_ERR        => EEPROM could not be read

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES


FUNCTION_BODY
    T_EEP_RESULT ret;
    *p_ram_image = NULL;

    //Ensure eep states remain unchanged
    eep_AcquireLocalMutex();

    if ( eep_info.state == EEP_UNAVAIL )
    {
        DBG_OUT_NOWAIT_("eep_provide_block: EEPROM not available\n");
        ret = (EEP_READ_ERR);
    }
    else if ( eep_param_check (block_id, 0, 0) != EEP_OK )
    {
        DBG_PRINT_NOWAIT_( ("eep_provide_block: block %d not existent\n", block_id) );
        ret = (EEP_NOT_EXIST);
    }
    else if ( block_len != eep_dscr[block_id].len )
    {
        DBG_PRINT_NOWAIT_( ("eep_provide_block: block %d not consistent\n", block_id) );
        ret = (EEP_INCONSISTENT);
    }
    else
    {
        *p_ram_image = eep_dscr[block_id].start_addr;
        /* NOTE:
        Softing used to check the CRC of the block here.
        Since
        1. we compute the CRC in the buffer
        2. the original implementation is not thread-safe
        there is no value in maintaining the CRC live
        */
        ret = EEP_OK;
    }

    eep_ReleaseLocalMutex();
    return ret;
} /* FUNCTION eep_provide_block */


/******************************************************************************/

FUNCTION GLOBAL T_EEP_RESULT eep_config_change
  (
    IN USIGN8               no_of_eep_block_dsc,
    IN T_EEP_BLOCK_DSC *    p_eep_block_dsc
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
Inserts 'no_of_eep_block_dsc' block descriptions into the global descriptor
structure. If descriptions for certain block IDs are already existing, they
are overwritten.

possible return values:
    EEP_OK              => completed successfully
    EEP_PARAM_ERR       => block ID(s) exceed limit

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
    int i, k;
    T_EEP_RESULT ret = EEP_OK;

FUNCTION_BODY

    eep_AcquireMutex();

    if (eep_info.state != EEP_IN_CONFIG)
    {
      memset((USIGN8 *)&eep_block_cfg, 0, sizeof(eep_block_cfg));

      eep_info.state = EEP_IN_CONFIG;
    }

    for (i=0; i<no_of_eep_block_dsc; i++) {
        if ( p_eep_block_dsc[i].block_id >= EEP_NBR_OF_BLOCKS )
        {
            ret = EEP_PARAM_ERR;
            break;
        }

        if ( p_eep_block_dsc[i].block_id == 0 )
        {                                   /* ID 0 is not valid, but indicates */
            continue;                       /* an unused entry                  */
        }

        for (k=0; k <EEP_NBR_OF_BLOCKS; k++) {
            if ( (eep_block_cfg[k].block_id == p_eep_block_dsc[i].block_id) || (eep_block_cfg[k].block_id == 0) )
            {
                eep_block_cfg[k] = p_eep_block_dsc[i];
                break;
            }
        } /* end for k */
    } /* end for i */

    eep_ReleaseMutex();

    return ret;
} /* FUNCTION eep_config_change */


/******************************************************************************/

FUNCTION GLOBAL T_EEP_RESULT eep_config_act (VOID)
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
Activates the new EEP configuration, i.e. builds new management structures and
writes the structure layout information to the EEPROM (if successful)

possible return values:
    EEP_OK          => activation succeeded
    EEP_MEM_INSUFF  => data does not fit into EEPROM
    EEP_PRG_ERR     => error while programming EEPROM

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
    USIGN16 data_size, block_num;
    USIGN16 total_size;
    T_EEP_RESULT ret;

FUNCTION_BODY

    eep_AcquireMutex();

    block_num = eep_config_sort ();

        /* Init block descriptions */
        /*-------------------------*/
    data_size = eep_build_dscr ();
    total_size = data_size + sizeof (T_EEP_INFO) + (block_num * sizeof (T_EEP_BLOCK_DSC));
    if ( total_size > USABLE_EEPROM_SIZE )
    {
        DBG_PRINT_( ("eep_config_act: EEP_MEM_INSUFF (block_num = %d, data_size = %d)\n", block_num, data_size) );
        ret = (EEP_MEM_INSUFF);
    }
    else
    {
        eep_info.size_eep_data = data_size;
        eep_info.num_blocks    = block_num;
        eep_info.start_data    = EEP_NUM_COPIES*(sizeof (T_EEP_INFO) + (block_num * sizeof (T_EEP_BLOCK_DSC)));

        eep_init_sequence = EEP_START_ACT;

        ret = eep_task_config();
    }

    eep_ReleaseMutex();
    return ret;

} /* FUNCTION eep_config_act */


/******************************************************************************/

FUNCTION LOCAL USIGN16 eep_config_sort (VOID)
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
Sorts the array of block configuration data in a way that all empty
entries are moved to the back end.

possible return values:
Returns the number of non-empty block configuration entries.
------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
    int i, k;

FUNCTION_BODY
    k = EEP_NBR_OF_BLOCKS - 1;

    for (i=0; i<k; i++) {
        if ( eep_block_cfg[i].block_size == 0 )
        {
            do {
                if ( eep_block_cfg[k].block_size != 0 )
                {
                    eep_block_cfg[i] = eep_block_cfg[k];
                    memset ((void *) &eep_block_cfg[k], 0, sizeof (T_EEP_BLOCK_DSC));
                    k--;
                    break;
                }
                 k--;
            } while (k>i);
        }
    } /* end for i */


    return (i);
} /* FUNCTION eep_config_sort */


/****************************************************************************/


/****************************************************************************/

FUNCTION GLOBAL T_EEP_RESULT eep_start_write
(
    IN  USIGN8   block_id           /* ID of block                          */
)
/*----------------------------------------------------------------------------

FUNCTIONAL_DESCRIPTION
Prepares a block for write operations.


RETURN_VALUE
    EEP_OK          => no error occured
    EEP_PARAM_ERR   => wrong parameter
    EEP_IN_CHANGE   => EEP configuration not consistent temporarily

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

    T_EEP_RESULT  result;

FUNCTION_BODY

    //Ensure eep states remain unchanged
    eep_AcquireLocalMutex();

    /* Check parameters */
    if ( (result = eep_param_check (block_id, 0, 0)) == EEP_OK )
    {
        /* Change block state */
        eep_dscr[block_id].state = BLK_WRITE;
    }

    eep_ReleaseLocalMutex();
    return (result);

} /* FUNCTION eep_start_write */


/****************************************************************************/

FUNCTION GLOBAL T_EEP_RESULT eep_write
(
    IN  USIGN8   block_id,          /* ID of block                          */
    IN  USIGN16  offset,            /* data offset inside the block         */
    IN  USIGN16  length,            /* length of data in bytes              */
    IN  USIGN8   mode               /* selected write mode                  */
)
/*----------------------------------------------------------------------------

FUNCTIONAL_DESCRIPTION

Writes data to the RAM image and, if needed, to the EEPROM. The only valid
value for the 'mode' parameter is :
  EEP_WAIT          the function returns after the final write cycle is
                    completed in the EEPROM device
If 'offset' == 0 and 'length' == 0 the whole block is written.

RETURN_VALUE
    EEP_OK          => no error occured
    EEP_PARAM_ERR   => wrong parameter
    EEP_PRG_ERR     => error while programming EEPROM
    EEP_IN_CHANGE   => EEP configuration not consistent temporarily

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

    T_EEP_RESULT  result;

FUNCTION_BODY

    /* Don't write when the length is 0 and not the */
    /* whole block should be written. */
    if ( (offset != 0) && (length == 0) )
    {
        return (EEP_OK);
    }

    /* ONLY EEP_WAIT is supported! */
    if ( mode != EEP_WAIT )
    {
        _ASSERT(FALSE);
        return (EEP_PARAM_ERR);
    }

    eep_AcquireLocalMutex();
    /* Check other parameters */
    if ( (result = eep_param_check (block_id, offset, length)) == EEP_OK )
    {
        /* Write data to EEPROM area */
        result = eep_put (block_id, offset, length);
    }
    eep_ReleaseLocalMutex();

    return (result);

} /* FUNCTION eep_write */



/****************************************************************************/

FUNCTION GLOBAL VOID eep_prepare_extra_nv_block
(
    IN  USIGN8       block_id,       /* ID of block          */
    IN  USIGN8     * p_ram_addr,     /* address in image     */
    IN  USIGN16      ram_len,        /* length of this block */
    OUT USIGN32    * p_eep_addr      /* eeprom address       */
)
/*----------------------------------------------------------------------------

FUNCTIONAL_DESCRIPTION

  The function prepares the extra NV data for writing into
  the eeprom.

RETURN_VALUE

  None

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

    T_CHKSUM    cs;

FUNCTION_BODY

    _ASSERT((block_id == NV_EXTRA_EEP_BLK) || (block_id == (NV_EXTRA_EEP_BLK + 1)));

    *p_eep_addr = (T_EEP_ADDR) ((USIGN8*)eep_dscr[block_id].start_addr - eep_ram_image) + eep_info.start_data;

    _ASSERT(ram_len != 0);
    _ASSERT(p_ram_addr != NULL);

    /* Calculate the checksum */
    cs = calc_crc (p_ram_addr, ram_len);
    *((T_CHKSUM *)(p_ram_addr - 2)) = cs;

    return;

} /* FUNCTION eep_prepare_extra_nv_block */

#if 0 //AK: This function is not used and hasn't been modified to account for two copies
FUNCTION GLOBAL VOID eep_put_extra_nv_data
(
    IN  USIGN32      eep_addr,       /* eeprom address       */
    IN  USIGN8     * p_ram_addr,     /* address in image     */
    IN  USIGN16      eep_len         /* length of this block */
)
/*----------------------------------------------------------------------------

FUNCTIONAL_DESCRIPTION

  The function copies the extra NV data into the eeprom.

RETURN_VALUE

  None

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

    eeprom_write_block(eep_addr, p_ram_addr, eep_len, NULL);

    return;

} /* FUNCTION eep_put_extra_nv_data */
#endif

/*#############################  EEP Task  #################################*/
#define EEP_ADDR_PREWRITE (65535 - 44) //still want to reserve mn_assert area, whether we'll use it or not

static void eep_task_wrapup(T_EEP_RESULT result, T_USR_MSG_BLOCK * msg)
{
    if ( result != 0 )
    {
        _USR_MSG_SET_RESULT(msg, E_NEG_RES);
        _USR_MSG_SET_LAYER(msg, result);        /* Encode the result into "layer"   */
    }
    else
    {
        _USR_MSG_SET_RESULT(msg, E_POS_RES);
        _USR_MSG_SET_LAYER(msg, 0);
    }

    _USR_MSG_SET_PRIMITIVE(msg, CON);
    osif_send_msg (OS_IF_EVENT_EEPROM, _USR_MSG_GET_SND(msg), EEP_TASK_ID, (T_OS_MSG_HDR *) msg);
}

static u8 eep_blkwrite_req[EEP_NBR_OF_BLOCKS]; //rely on zero init
static u8 currblock = 0; //where we start polling for writes
static u16 writes_pending = 0;

/** \brief Finds and writes one eep block (if any) starting with currblock
*/
static void eep_write_wrapper(void)
{
    u8 i=currblock;
    for(;;)
    {
        u8 write_req;

        osif_disable_all_tasks();
        write_req = eep_blkwrite_req[i];
        eep_blkwrite_req[i]= 0;
        const T_EEP_DSC *p = &eep_dscr[i];
        osif_enable_all_tasks();

        if(write_req != 0)
        {
            if(block_under_test == i)
            {
                //reset the test for this block to avoid inconsistency
                testphase = eeptest_r1;
            }
            Reset_Watchdog();
            T_EEP_RESULT result;
#if EEP_DIAG_ALLOW_DISABLE_WRITE
            if(eep_write_disabled)
            {
                result = EEP_OK; //just skip a write
            }
            else
#endif
            {
                (void)eep_write_block(EEP_ADDR_PREWRITE, &i, sizeof(i)); //don't sweat it if fails
                result = eep_task_write(p);
            }

            osif_disable_all_tasks();
            if(result == EEP_OK)
            {
                writes_pending--;
            }
            else
            {
                eep_blkwrite_req[i] = 1; //will try next time
            }
            osif_enable_all_tasks();
        }
        i = (i+1)%EEP_NBR_OF_BLOCKS;
        if((i==currblock) || (write_req != 0)) //we finished the round or found what to write
        {
            break;
        }
    }
    currblock = i; //start from here next time
}

static u32 capture=0U;

u32 eep_get_stamp(void)
{
    return capture;
}

/** \brief A lowest-priority mopup service for pending eep block writes and
self-test. Does NOT return!
For backward compatibility, consumes T_EVENT messages the same way as the original.
*/
static void eep_task_main (T_EVENT event)
{
    T_USR_MSG_BLOCK * msg;
    T_EEP_RESULT result;

    _ASSERT((event != OS_IF_EVENT_EEPROM) || (event != OS_IF_EVENT_SYSSTART));

    __enable_interrupt(); //least priority task gets the system running

    UNUSED_OK(writes_pending); //future

    //First, deal with the possibly incomplete write before reset
    ErrorCode_t err = eep_read_block((u8 *)&block_under_test, EEP_ADDR_PREWRITE, sizeof(block_under_test));
    if((err == ERR_OK) && (block_under_test < EEP_NBR_OF_BLOCKS))
    {
        testphase = eeptest_r1;
        do
        {
            eeptest_eepblock();
        } while((testphase != eeptest_r1) && (testphase != eeptest_cat_r1));
    }
    block_under_test = BLK_CATALOG;
    testphase = eeptest_cat_r1;

    for(;;)
    {
        capture = OS_GetTime32();
        //Consume messages - temporary until all removed
        while ((msg = (T_USR_MSG_BLOCK *) osif_get_msg (EEP_TASK_ID, OS_IF_EVENT_EEPROM)) != NULL)
        {
            Reset_Watchdog();
            //eep_AcquireMutex();
            T_EEP_SERVICE op = (T_EEP_SERVICE) _USR_MSG_GET_SERVICE(msg);
            switch (op) {
            case EEP_VERIFY:
                result = EEP_OK; //eep_task_verify ((T_EEP_TASK_IF *) _USR_MSG_GET_DATA(msg));
                break;
            default:
                result = EEP_INVALID_SERVICE;
                break;
            } /* end switch */
            //eep_ReleaseMutex();

            eep_task_wrapup(result, msg);

        } /* end while */

        //Clear a few pending writes - or, come to think of it, ALL writes
        for(u8_least i=0; i<EEP_NBR_OF_BLOCKS; i++)
        {
            eep_write_wrapper();
        }

        //Do one step of background test here
        eeptest_eepblock();

        OS_Delay(200); //sleep for 200 ms
        u32 time_spent = OS_GetTime32() - capture;
        max_allwrite_time = MAX(max_allwrite_time, time_spent);
    }

} /* FUNCTION eep_task_main */


/** \brief A helper to atomically write an eep block (all copies).
Requires that it is called from a mop-up (lowest useful priority) task.
NOTE: This function doesn't return until writes are completed.
\param p_blk_dsc - a pointer to description of the block to write
\return EEP error code (EEP_OK if at least one copy was written OK)
*/
static T_EEP_RESULT eep_task_write(const T_EEP_DSC *p_blk_dsc)
{
    osif_disable_all_tasks();
    size_t header = (const u8 *)p_blk_dsc->start_addr - (const u8 *)p_blk_dsc->checksum;
    size_t total_length = header + p_blk_dsc->len;
    memcpy(write_buffer, p_blk_dsc->checksum, total_length);
    osif_enable_all_tasks();
    u16 crc = calc_crc ((u8 *)write_buffer + header, p_blk_dsc->len);
    osif_disable_all_tasks();
    write_buffer[0] = crc;
    osif_enable_all_tasks();

    T_EEP_RESULT ret = EEP_PRG_ERR;
    for(u8_least i=0; i<EEP_NUM_COPIES; i++)
    {
        /* write EEP data */
        T_EEP_RESULT result = eep_writeblock_helper(write_buffer, p_blk_dsc, i);
        if ( result == EEP_OK )
        {
            ret = EEP_OK; //only one success required
        }
    }

    return ret;
} /* FUNCTION eep_task_write */


/** \brief A helper to write all copies of the eep catalog.
NOTE: This function doesn't return until writes are completed.
\return EEP error code (EEP_OK if at least one copy was written OK)
*/
static T_EEP_RESULT eep_task_config(void)
{
    T_EEP_RESULT ret = EEP_PRG_ERR;
    T_EEP_INFO info = eep_info;
    info.crc   = calc_crc ((USIGN8 *) &eep_block_cfg, info.num_blocks * sizeof (T_EEP_BLOCK_DSC));
    info.state = EEP_VALID;
    info.CheckWord = calc_crc((USIGN8 *)&info, MN_OFFSETOF(T_EEP_INFO, CheckWord));

    for(u8_least i=0; i<EEP_NUM_COPIES; i++)
    {
        /* write EEP catalog */
        /*--------------------------*/
        if(eep_writecat_helper(&info, eep_block_cfg, i) == EEP_OK)
        {
            ret = EEP_OK; //only one success required
        }
    }

    if(ret == EEP_OK)
    {
        eep_info = info;
    }

    return ret;
} /* FUNCTION eep_task_config */

/*########################  Internal functions  ############################*/

/** \brief EEP catalog initializer from EEP.

Initializes the EEP interface: sets EEP block description structure, then
initializes the physical EEPROM device interface by calling "eeprom_init" and
finally reads all data from the EEPROM device to the RAM image.

\return EEP error code (EEP_OK if at least one eep copy was read OK)
*/
static T_EEP_RESULT eep_read_info(void)
/*----------------------------------------------------------------------------

FUNCTIONAL_DESCRIPTION


RETURN_VALUE
    EEP_OK          => writing of data to the RAM image, previously read from the
                       EEPROM device, was successful. No Error.
    EEP_READ_ERR    => it was not possible to read data from the EEPROM.
    EEP_CS_ERR      => checksum error.

----------------------------------------------------------------------------*/
{
    T_EEP_RESULT result;

    /* Clear data structures - necessary for Softing code in eep_build_dscr.*/
    memset(&eep_info, 0, sizeof(eep_info));
    memset(eep_block_cfg, 0, sizeof(eep_block_cfg));

        /* Initialize low level routines */
        /*-------------------------------*/
    fram_Initialize();
    USIGN8 page_size = 0; //whatever that was to mean - serial FRAM returned 0.

        /* get EEPROM info */
        /*-----------------*/

    eep_AcquireLocalMutex();
    for(u8_least i=0; i<EEP_NUM_COPIES; i++)
    {
        result = eep_read_catalog_helper(&eep_info, eep_block_cfg, i);
        if(result == EEP_OK)
        {
            if ( page_size != eep_info.page_size )
            {
                result = EEP_INCONSISTENT;
            }
            if(eep_info.num_blocks > EEP_NBR_OF_BLOCKS)
            {
                result = EEP_INCONSISTENT;
            }
        }
        if(result == EEP_OK)
        {
            break;
        }
    }

    if ( result != EEP_OK )
    {
        eep_info.page_size = page_size;
        if ( result == EEP_READ_ERR )
        {
            eep_info.state = EEP_UNAVAIL;
        }
    }
    eep_ReleaseLocalMutex();
    return (result);
} /* FUNCTION eep_read_info */


/****************************************************************************/

FUNCTION LOCAL USIGN16 eep_build_dscr (VOID)
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

The function builds the block configuration in the EEPROM.

possible return values:
- returns the data size used for the block configuration in the EEPROM
------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
    USIGN16 eep_offset;
    int i, k;

FUNCTION_BODY

    eep_offset = 0;
    memset((USIGN8 *)eep_dscr, 0, sizeof(eep_dscr));

        /* prepare EEPROM management data structure */
        /*------------------------------------------*/
    for (i=0; i<EEP_NBR_OF_BLOCKS; i++) {
        for (k=0; k<EEP_NBR_OF_BLOCKS; k++) {
            if ( (eep_block_cfg[k].block_id != 0) && (eep_block_cfg[k].block_id == i) )
                break;
        } /* end for k */

        if ( k < EEP_NBR_OF_BLOCKS ) {      /* Block with id 'i' found */
            eep_dscr[i].id         = i;
            eep_dscr[i].len        = eep_block_cfg[k].block_size;
            eep_dscr[i].state      = BLK_NOT_RDY;
            eep_dscr[i].checksum   = (T_CHKSUM FAR_D *) (eep_ram_image + eep_offset);
#ifdef UNPACKED_ACCESS
            eep_dscr[i].auto_size  = (eep_block_cfg[k].auto_write_length == 0) ? 0 : (eep_block_cfg[k].auto_write_length + 2);
            eep_offset            += sizeof (USIGN32);
#else
            eep_dscr[i].auto_size  = eep_block_cfg[k].auto_write_length;
            eep_offset            += sizeof (T_CHKSUM);
#endif /* UNPACKED_ACCESS */
            eep_dscr[i].start_addr = (USIGN8 FAR_D *) (eep_ram_image + eep_offset);

#ifdef UNPACKED_ACCESS
            /* align offset to multiple of 4 byte */
            eep_offset += ((eep_dscr[i].len + 3u) & ~3u);
#else
            eep_offset += eep_dscr[i].len;
#endif /* UNPACKED_ACCESS */
            eep_set_cs (&eep_dscr[i]);
        }

    } /* end for i */

    return (eep_offset);
} /* FUNCTION eep_build_dscr */


/****************************************************************************/

FUNCTION LOCAL T_EEP_RESULT eep_param_check
(
    IN  USIGN8   block_id,         /* ID of block                           */
    IN  USIGN16  offset,           /* data offset inside the block          */
    IN  USIGN16  length            /* length of data in bytes               */
)
/*----------------------------------------------------------------------------

FUNCTIONAL_DESCRIPTION

Checks the calling parameters against EEP block description.

RETURN_VALUE
    EEP_OK          => parameters ok
    EEP_PARAM_ERR   => parameters do not agree with block description
    EEP_IN_CHANGE   => EEP configuration not consistent temporarily

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

    T_EEP_DSC * p_blk_dsc;      /* pointer to block descriptor*/

FUNCTION_BODY
        /* check current state of EEP */
    if ( eep_info.state != EEP_VALID )
    {
        return (EEP_IN_CHANGE);
    }

        /* Is block_id within the valid range? */
    if ( block_id >= EEP_NBR_OF_BLOCKS )
    {
        _EEP_WARNING(WRN_MAXBLKID, block_id, EEP_NBR_OF_BLOCKS);
        return (EEP_PARAM_ERR);
    }

    /* Get the descriptor of the desired block */
    p_blk_dsc = &eep_dscr[block_id];

    /* Check if block is supported */
    if ( p_blk_dsc->len == 0 )
    {
        /* block not supprted */
        //_EEP_WARNING(WRN_INVBLKID, block_id, 0);
        return (EEP_PARAM_ERR);
    }


    /* Are the data within the block boundaries? */
    if ( (offset + length) > p_blk_dsc->len )
    {
        _EEP_WARNING(WRN_BLKLEN, block_id, offset + length);
        return (EEP_PARAM_ERR);
    }

    return (EEP_OK);
} /* FUNCTION eep_param_check */


/****************************************************************************/

/** \brief creates a request for an eep block to be written.
\param block_id - A block to write
\param offset - where to write from (from the beginning of the blocks) NO LONGER USED
\param length - # of bytes to write NO LONGER USED
\return EEP_OK
*/
static T_EEP_RESULT eep_put
(
    IN  USIGN8   block_id,          /* ID of block                          */
    IN  USIGN16  offset,            /* data offset inside the block         */
    IN  USIGN16  length             /* length of data in bytes              */
)
/*----------------------------------------------------------------------------

FUNCTIONAL_DESCRIPTION

Writes data to the RAM image and, if needed, to the EEPROM.
If 'offset' == 0 and 'length' == 0 the whole block is written.

RETURN_VALUE
    EEP_OK         => no error occured
    EEP_PRG_ERR    => error while programming EEPROM

----------------------------------------------------------------------------*/
{
    osif_disable_all_tasks();
    u8 id = eep_dscr[block_id].id;
    if(eep_blkwrite_req[id] == 0)
    {
        eep_blkwrite_req[id] = 1;
        writes_pending++;
    }
    osif_enable_all_tasks();
    return EEP_OK; //TODO: Can we do better with delayed writes?

} /* FUNCTION eep_put */


/****************************************************************************/

FUNCTION LOCAL VOID eep_set_cs
(
    IN  T_EEP_DSC * block_dscr         /* descritor of block */
)
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

This function calculates the chechsum of the block data including the version
number and store it at the end of the block

RETURN_VALUE
    none
----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

    T_CHKSUM         cs;

FUNCTION_BODY

    /* calculate checksum */
    cs = calc_crc (block_dscr->start_addr, block_dscr->len);

    *(block_dscr->checksum) = cs;
    block_dscr->state = BLK_OK;

    return;
} /* FUNCTION eep_set_cs */


/****************************************************************************/

#if 0 //AK: This helper function is not used because the function below isn't

FUNCTION LOCAL USIGN8 eep_get_subblk_no
  (
    IN  USIGN8  block_id
  )
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

  The function returns the subblk identifier.

RETURN_VALUE

  Subblock ID
----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

  USIGN8 i;

FUNCTION_BODY

  for (i=0; i<eep_info.num_blocks; i++)
  {
     if(eep_block_cfg[i].block_id == block_id)
     {
       return(i);
     }
  }

  _EEP_EXCEPTION(ERR_BLKIDINVAL, 0, i);

  return(0);

} /* FUNCTION eep_get_subblk_no */
#endif

/****************************************************************************/

#if 0 //AK: This function is not used and hasn't been modified to account for two copies
FUNCTION GLOBAL T_EEP_RESULT eep_reset_extra_nv_ram
  (
    IN   USIGN8        unsorted_block_id,   /* EEP/FRAM block identifier (unsorted)   */
    IN   USIGN8    *   p_ram_addr,          /* Address in the extra NV block in RAM   */
    IN   USIGN32       eep_offset,          /* Offset of the extra NV block in NV RAM */
    IN   USIGN16       length               /* Length of the specific block in NV RAM */
  )
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

  The function is used to clear the extra NV RAM in EEPROM/FRAM and
  in the memory.

PARAMETERS

  unsorted_block_id               Unsorted block identifier of the extra NV
                                  RAM in EEPROM/FRAM.

  p_ram_addr                      Address of the extra NV ram.

  eep_offset                      Offset inside the EEPROM/FRAM.

  length                          Length of the sub-block.

RETURN_VALUE

   EEP_OK     No error
  !EEP_OK     Error

----------------------------------------------------------------------------*/
{
    eep_AcquireMutex();
LOCAL_VARIABLES

  USIGN8        block_id = eep_get_subblk_no(unsorted_block_id);
  T_EEP_RESULT  eep_result;

FUNCTION_BODY

    /* Initialize the NV RAM with 0 (Only for this sub-block!) */
    memset((p_ram_addr + eep_offset), 0, length);

    if ((eep_result = eep_start_write(block_id)) != EEP_OK)
    {
        //nothing
    }
    else if ((eep_result = eep_write(block_id, eep_offset, length, EEP_WAIT)) != EEP_OK)
    {
        //nothing
    }
    else
    {
        eep_result = EEP_OK;
    }

    eep_ReleaseMutex();

    return eep_result;

} /* FUNCTION eep_reset_extra_nv_ram */
#endif

/****************************************************************************/

FUNCTION GLOBAL BOOL eep_startup_for_the_first_time (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

  The function checks if the device was started for the first time.

RETURN_VALUE

  TRUE    Device was started for the first time or was re-initialized.

  FALSE   Device was NOT started for the first time.

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  if (eep_init_sequence == EEP_START_ACT)
    return(TRUE);

  return(FALSE);

} /* FUNCTION eep_startup_for_the_first_time */

/****************************************************************************/


