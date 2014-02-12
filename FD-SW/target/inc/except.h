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

FILE_NAME          except.h



FUNCTIONAL_MODULE_DESCRIPTION

This file contains the definitions for the exception handler

=========================================================================== */
#ifndef __EXCEPT__
#define __EXCEPT__


/****************************************************************************/
/*** Component / module identification                                    ***/
/****************************************************************************/

#define COMP_ID(id)         ((id)*0x100)

    /* HW Unit:     ID's 0x00 .. 0x0f*/
#define HW_CID              0x00

#define COMP_HW             COMP_ID(HW_CID)
    #define MOD_HWINIT      0x0001
    #define MOD_HWINTR      0x0002
    #define MOD_HWIO        0x0003
    #define MOD_HWMAIN      0x0004
    #define MOD_HWOS        0x0005
    #define MOD_HWTIM       0x0006
    #define MOD_I2C         0x0007
    #define MOD_HWSWDL      0x000A
    #define MOD_HWHM        0x000D
    #define MOD_HWMM        0x000E

    /* SYS Unit:    ID's 0x10 .. 0x1f*/
#define EEP_CID             0x10
#define OSIF_CID            0x11
#define DBG_CID             0x12
#define TMP_CID             0x13

#define COMP_EEP            COMP_ID(EEP_CID)
    #define MOD_EEPIF       0x0001
    #define MOD_EEPROM      0x0002

#define COMP_DBG            COMP_ID(DBG_CID)
    #define MOD_DBGOUT      0x0001

#define COMP_OSIF           COMP_ID(OSIF_CID)
    #define MOD_OSIFMAIN    0x0001
    #define MOD_OSIFTSK     0x0002
    #define MOD_OSIFSEM     0x0003
    #define MOD_OSIFCFG     0x0004
    #define MOD_OSIFTIM     0x0005

#define COMP_TMP            COMP_ID(TMP_CID)
    #define MOD_TMP_LO      0x0001
    #define MOD_TMP_HI      0x0002
    #define MOD_TMP_SN      0x0003

    /* PAC Unit:    ID's 0x20 .. 0x2f*/
#define PCIF_CID            0x20
#define FDL_CID             0x21
#define DPS_CID             0x22
#define PFBS_CID            0x23
#define DUSR_CID            0x24

#define COMP_PCIF           COMP_ID(PCIF_CID)
    #define MOD_PAC         0x0001
    #define MOD_PAC_BGD     0x0002
    #define MOD_PAC_WD      0x0003

#define COMP_FDL            COMP_ID(FDL_CID)
    #define MOD_FDLF1P      0x0001
    #define MOD_FDLFRT      0x0002
    #define MOD_FDLSPC      0x0003
    #define MOD_FDLRCV      0x0004
    #define MOD_FDLXMT      0x0005
    #define MOD_FDLUSR      0x0006
    #define MOD_FDLBRS      0x0007

#define COMP_DPS            COMP_ID(DPS_CID)
    #define MOD_DPS_MAIN    0x0001
    #define MOD_DPS_C1      0x0002
    #define MOD_DPS_C2      0x0003
    #define MOD_DPS_CYC     0x0004
    #define MOD_DPS_ACC2    0x0005
    #define MOD_DPS_IF      0x0006
    #define MOD_DPS_MEM     0x0007
    #define MOD_TOOLS       0x0008
    #define MOD_STAT        0x0009

#define COMP_PFBS           COMP_ID(PFBS_CID)
    #define MOD_PFBS_IF     0x0001
    #define MOD_PFBS_NV     0x0002
    #define MOD_PFBS_PBIF   0x0003
    #define MOD_PFBS_OD     0x0004
    #define MOD_PFBS_IM     0x0005
    #define MOD_PFBS_DM     0x0006
    #define MOD_PFBS_RW     0x0007
    #define MOD_PFBS_RUN    0x0008
    #define MOD_PFBS_DPS    0x0009
    #define MOD_PFBS_BGD    0x000A

#define COMP_DUSR           COMP_ID(DUSR_CID)
    #define MOD_DUSR        0x0001
    #define MOD_DUSR_STUB   0x0002

    /* FDC Unit:    ID's 0x30 .. 0x3f*/
#define FCIF_CID            0x30
#define DLL_CID             0x31
#define FAL_CID             0x32
#define FMG_CID             0x33
#define FFBS_CID            0x34
#define UTA_CID             0x35
#define SWDL_CID            0x36

#define COMP_FDC            COMP_ID(FCIF_CID)
    #define MOD_FDC         0x0001
    #define MOD_FDC_BGD     0x0002
    #define MOD_FDC_WD      0x0003


#define COMP_DLL            COMP_ID(DLL_CID)
    #define MOD_DLLACC      0x0001
    #define MOD_DLLADDR     0x0002
    #define MOD_DLLBAS      0x0003
    #define MOD_DLLCFG      0x0004
    #define MOD_DLLDRX      0x0005
    #define MOD_DLLFBC      0x0006
    #define MOD_DLLINTR     0x0007
    #define MOD_DLLLAS      0x0008
    #define MOD_DLLLIST     0x0009
    #define MOD_DLLLRX      0x000A
    #define MOD_DLLLTIM     0x000B
    #define MOD_DLLLTX      0x000C
    #define MOD_DLLMSG      0x000D

#define COMP_FAL            COMP_ID(FAL_CID)
    #define MOD_DOMAIN      0x0001
    #define MOD_FALABT      0x0002
    #define MOD_FALASS      0x0003
    #define MOD_FALAUX      0x0004
    #define MOD_FALDLL      0x0005
    #define MOD_FALFMS      0x0006
    #define MOD_FALOPN      0x0007
    #define MOD_FALPUB      0x0008
    #define MOD_FALSND      0x0009
    #define MOD_FALSRV      0x000A
    #define MOD_FALSUB      0x000B
    #define MOD_FALTIM      0x000C
    #define MOD_FALTSK      0x000D
    #define MOD_FALVCR      0x000E
    #define MOD_FMSCOD      0x000F
    #define MOD_FMSIF       0x0010

#define COMP_FMG            COMP_ID(FMG_CID)
    #define MOD_FMG         0x0001
    #define MOD_MIB         0x0002
    #define MOD_NMCFG       0x0003
    #define MOD_NMDOM       0x0004
    #define MOD_NMIF        0x0005
    #define MOD_NMOD        0x0006
    #define MOD_NMVCR       0x0007
    #define MOD_SM          0x0008
    #define MOD_SMTIM       0x0009
    #define MOD_SMDLS       0x000A
    #define MOD_SMDLH       0x000B

#define COMP_FFBS           COMP_ID(FFBS_CID)
    #define MOD_FBSIF       0x0001
    #define MOD_FBSINIT     0x0002
    #define MOD_FBSRUN      0x0003
    #define MOD_FBSOD       0x0004
    #define MOD_FBSTRD      0x0005
    #define MOD_FBSBLK      0x0006
    #define MOD_FBSVFD      0x0007
    #define MOD_FBSACT      0x0008
    #define MOD_FBSSCD      0x0009
    #define MOD_FBSMVC      0x000A
    #define MOD_FBSALT      0x000B
    #define MOD_FBSFBIF     0x000C
    #define MOD_FBSNV       0x000D
    #define MOD_FBSLNK      0x000E
    #define MOD_FBSRW       0x000F

#define COMP_UTA            COMP_ID(UTA_CID)
    #define MOD_UTAMN       0x0001
    #define MOD_UTA         0x0002
    #define MOD_UTAENTR     0x0003
    #define MOD_UTAARJ      0x0004
    #define MOD_UTACHK      0x0005
    #define MOD_UTACVT      0x0006
    #define MOD_UTAIF       0x0007
    #define MOD_UTADLH      0x0008

#define COMP_SWDL           COMP_ID(SWDL_CID)
    #define MOD_SWDL        0x0001
    #define MOD_SWDLIF      0x0002
    #define MOD_UNPACK      0x0003

    /* APP Unit:    ID's 0x40 .. 0x4f*/
#define PFBL_CID            0x40
#define FFBL_CID            0x41
#define PAPP_CID            0x42
#define FAPP_CID            0x43
#define SSC_CID             0x44
#define HM_CID              0x45
#define MM_CID              0x46

#define COMP_PFBL           COMP_ID(PFBL_CID)
    #define MOD_PFBL_AI     0x0001
    #define MOD_PFBL_AO     0x0002
    #define MOD_PFBL_DI     0x0003
    #define MOD_PFBL_DO     0x0004
    #define MOD_PFBL_PHY    0x0005
    #define MOD_PFBL_TOT    0x0006
    #define MOD_PFBL_BAS    0x0007
    #define MOD_PFBL_DIA    0x0008

#define COMP_PAPP           COMP_ID(PAPP_CID)
    #define MOD_APPL_IF     0x0001            /* equal to MOD_APPL_IF of COMP_FAPP  */
    #define MOD_APPL_PHY    0x0002
    #define MOD_APPL_TRN    0x0003            /* equal to MOD_APPL_TRN of COMP_FAPP */
    #define MOD_APPL_DIA    0x0004            /* equal to MOD_APPL_DIA of COMP_FAPP */
    #define MOD_APPL_NRV    0x0005            /* equal to MOD_APPL_NRV of COMP_FAPP */
    #define MOD_APPL_IDT    0x0006

#define COMP_FFBL           COMP_ID(FFBL_CID)
    #define MOD_FBLKBAS     0x0001
    #define MOD_FBLKAI      0x0002
    #define MOD_FBLKALB     0x0003
    #define MOD_FBLKAO      0x0004
    #define MOD_FBLKDI      0x0005
    #define MOD_FBLKDO      0x0006
    #define MOD_FBLKDOM     0x0007
    #define MOD_FBLKIF      0x0009
    #define MOD_FBLKKRN     0x000A
    #define MOD_FBLKMAI     0x000B
    #define MOD_FBLKMAO     0x000C
    #define MOD_FBLKMDI     0x000D
    #define MOD_FBLKMDO     0x000E
    #define MOD_FBLKMMO     0x000F
    #define MOD_FBLKMO      0x0010
    #define MOD_FBLKPAI     0x0011
    #define MOD_FBLKPAO     0x0012
    #define MOD_FBLKPID     0x0013
    #define MOD_FBLKRES     0x0014
    #define MOD_FBLKSCD     0x0015
    #define MOD_FBLKSCL     0x0016
    #define MOD_FBLKSIM     0x0017
    #define MOD_FBLKWRT     0x0018
    #define MOD_FBLKIT      0x0019
    #define MOD_FBLKOS      0x001A
    #define MOD_FBLKAR      0x001B
    #define MOD_FBLKSC      0x001C
    #define MOD_FBLKIS      0x001D
    #define MOD_FBLKFD      0x001E

#define COMP_FAPP           COMP_ID(FAPP_CID)
    #define MOD_APPL_IF     0x0001            /* equal to MOD_APPL_IF of COMP_PAPP */
    #define MOD_APPL_RES    0x0002
    #define MOD_APPL_TRN    0x0003            /* equal to MOD_APPL_TRN of COMP_PAPP */
    #define MOD_APPL_DIA    0x0004            /* equal to MOD_APPL_DIA of COMP_PAPP */
    #define MOD_APPL_NRV    0x0005
    #define MOD_APPL_DLH    0x0006
    #define MOD_APPL_FBK2   0x0007

#define COMP_SSC            COMP_ID(SSC_CID)
    #define MOD_SSC         0x0001
    #define MOD_SSC_HW      0x0002

#define COMP_HM             COMP_ID(HM_CID)
    #define MOD_HM_IF       0x0001
    #define MOD_HM          0x0002
    #define MOD_HM_TASK     0x0003

#define COMP_MM             COMP_ID(MM_CID)
    #define MOD_MM_IF       0x0001
    #define MOD_MM          0x0002
    #define MOD_MM_TASK     0x0003

/****************************************************************************/
/*** Type definitions                                                     ***/
/****************************************************************************/
typedef enum                            /* describes the severity class of  */
{                                       /* an exception, where is:          */
    XCP_WARNING,                        /*      type  0  -> WARNING         */
    XCP_ASSERTION,                      /*      type  1  -> ASSERTION       */
    XCP_ERROR,                          /*      type  2  -> ERROR           */
    XCP_FATAL                           /*      type  3  -> FATAL ERROR     */
} T_XCP_TYPE;

typedef enum                            /* describes the source of the ex-  */
{                                       /* ception                          */
    XCP_HW     = HW_CID,
    XCP_EEP    = EEP_CID,
    XCP_OS     = OSIF_CID,
    XCP_FDL    = FDL_CID,
    XCP_DPS    = DPS_CID,
    XCP_DUSR   = DUSR_CID,
    XCP_PFBS   = PFBS_CID,
    XCP_PFBL   = PFBL_CID,
    XCP_PAPP   = PAPP_CID,
    XCP_DLL    = DLL_CID,
    XCP_FCIF   = FCIF_CID,
    XCP_FAL    = FAL_CID,
    XCP_FFBS   = FFBS_CID,
    XCP_FFBL   = FFBL_CID,
    XCP_FAPP   = FAPP_CID,
    XCP_UTA    = UTA_CID,
    XCP_HM     = HM_CID,
    XCP_MM     = MM_CID,
    XCP_ASSERT = 0xf0                   /*      code f0  -> ASSERTION       */
} T_XCP_CODE;


    /* Assertions in general */
    /*-----------------------*/
typedef struct
{
    USIGN16             line;
    char *              filename;
} T_XCP_ASSERT;


    /* HW exceptions */
    /*---------------*/
typedef struct
{
    enum {
        ERR_INV_INT     = 0x0001,
        ERR_WATCHDOG    = 0x0002,
        ERR_SW_WATCHDOG = 0x0003,
        ERR_UNDEF_INSTR = 0x0004,
        ERR_I2C_TIMEOUT = 0x0010,       /* -> detail = curr. no. of bytes   */
        ERR_SNR_READ    = 0x0020,       /* -> detail = IIC error code       */
        ERR_SNR_CHECK   = 0x0021,       /* -> detail = checksum             */
        ERR_SNR_INCONS  = 0x0022,       /* -> detail = serial number        */
        ERR_EMBOS       = 0x0100        /* -> detail = embOS error code     */
    }                   reason;
    USIGN32             detail;
} T_XCP_HW;

    /* OS / OSIF exceptions */
    /*---------------*/
typedef enum
{
        FKT_INIT        = 0x00,
        FKT_CREATE_TASK = 0x01,
        FKT_DELETE_TASK = 0x02,
        FKT_GET_TASK_ID = 0x03,
        FKT_DISABLE_TASK= 0x04,
        FKT_ENABLE_TASK = 0x05,
        FKT_WAIT_EVENT  = 0x10,
        FKT_SET_EVENT   = 0x11,
        FKT_RESUME_TASK = 0x12,
        FKT_GET_MSG     = 0x20,
        FKT_SND_MSG_RCV = 0x21,
        FKT_SND_MSG_SND = 0x22,
        FKT_INIT_SEMA   = 0x30,
        FKT_CREATE_SEMA = 0x31,
        FKT_DELETE_SEMA = 0x32,
        FKT_USE_SEMA    = 0x33,
        FKT_REQ_SEMA    = 0x34,
        FKT_REL_SEMA    = 0x35,
        FKT_CHECK_SEMA  = 0x36,
        FKT_CREATE_TIM  = 0x40,
        FKT_START_TIM   = 0x41,
        FKT_RESTART_TIM = 0x42,
        FKT_STOP_TIM    = 0x43,
        FKT_HOOK_TIM    = 0x44
} T_OS_LOC;

typedef struct
{
    enum {
        ERR_INV_TASK    = 0x0001,       /* Invalid task ID detected         */
        ERR_TASKID_AMB  = 0x0002,       /* Ambigous task IDs detected       */
        ERR_INV_MAINFKT = 0x0003,       /* Task main function invalid       */
        ERR_NO_TASK_ID  = 0x0004,       /* Current task ID not found        */
        ERR_TASK_NEXIST = 0x0005,       /* Task with ID [id] not existent   */
        ERR_INV_EVTMASK = 0x0010,
        ERR_INV_EVENT   = 0x0011,
        ERR_INV_MSGEVT  = 0x0020,
        ERR_INSUFF_MBX  = 0x0021,
        ERR_INSUFF_MBUF = 0x0022,
        ERR_MULT_EVENTS = 0x0023,
        ERR_MSG_NOT_PROC= 0x0024,
        ERR_INV_MSG_PTR = 0x0025,
        ERR_SEMA_OV     = 0x0030,       /* Semaphore memory not sufficient  */
        ERR_SEMA_INCONS = 0x0031,       /* Semaphore manager corrupted      */
        ERR_SEMA_INV    = 0x0032,       /* Semaphore pointer not valid      */
        ERR_TIM_INV     = 0x0040,       /* Timer ID invalid                 */
        ERR_TIM_NEXIST  = 0x0041,       /* Timer with ID [id] not created   */
        ERR_TIM_HOOK    = 0x0042,       /* Timer hook function invalid      */
        ERR_TIM_VALUE   = 0x0043,       /* Timer start value exceeds limit  */
        ERR_TIM_STATUS  = 0x0043,       /* inv.tim.state for current action */
        ERR_TIM_ERROR   = 0x0044,       /* timer expired unexpectedly       */
        ERR_TIM_OV      = 0x0045,       /* No more timers available         */
        ERR_EMBOS_INT   = 0x0100        /* -> detail = embOS error code     */
    }                   reason;
    T_OS_LOC            location;
    USIGN32             detail;
} T_XCP_OS;


    /* EEP exceptions */
    /*----------------*/
typedef enum
{
        EEP_INIT        = 0x00
} T_EEP_LOC;

typedef struct
{
    enum {
        ERR_MEMSIZE     = 0x0001,       /* -> detail = eep_offset           */
        ERR_NO_MSG      = 0x0003,       /* -> detail = 0                    */
        ERR_INISIZE     = 0x0004,       /* -> detail = size of ini-data     */
        ERR_BLKIDINVAL  = 0x0005,       /* -> detail = block id             */
        WRN_MAXBLKID    = 0xFF01,       /* -> detail = EEP_NBR_OF_BLOCKS    */
        WRN_INVBLKID    = 0xFF02,       /* -> detail = 0                    */
        WRN_BLKLEN      = 0xFF03,       /* -> detail = offset + len         */
        WRN_INCONS      = 0xFF04,       /* -> detail = location             */
        WRN_EEPCLR      = 0xFF05,       /* -> detail = success || 0         */
        WRN_EEPWRT      = 0xFF06,       /* -> detail = EEPROM address       */
        WRN_EEPRD       = 0xFF07        /* -> detail = EEPROM address       */
    }                   reason;
    USIGN32             detail;
    USIGN8              block_id;
} T_XCP_EEP;


    /* FDL exceptions */
    /*----------------*/
typedef struct
{
    enum {
        ERR_INV_SRV     = 0x0001,       /* -> detail = service id           */
        ERR_RCV_MSG     = 0x0002,       /* -> detail = current number       */
        ERR_DP_MODE     = 0x0003,       /* -> detail = 0                    */
        ERR_F1_EI       = 0x0004,       /* -> detail = F1 interrupt reason  */
        ERR_F1_XFIFO    = 0x0101,       /* -> detail = F1 xmit length reg.  */
        ERR_F1_RFIFO    = 0x0102,       /* -> detail = F1 FIFO status       */
        ERR_F1_NEPT     = 0x0103,       /* -> detail = 0                    */
        ERR_F1_TLM      = 0x0104,       /* -> detail = F1 xmit length reg.  */
        ERR_F1_JAB      = 0x0105,       /* -> detail = 0                    */
        ERR_F1_CNS      = 0x0106,       /* -> detail = 0                    */
        ERR_F1_CLK      = 0x0107,       /* -> detail = clock frequency      */
        ERR_TXOV        = 0x0110,       /* -> detail = request. xmit length */
        ERR_DMA_RX      = 0x0200,       /* -> detail = 0                    */
        ERR_DMA_TX      = 0x0201,       /* -> detail = 0                    */
        ERR_SPC         = 0x0300,       /* -> detail = SPC4-2 IRR           */
        WRN_INT         = 0xFF01,       /* -> detail = F1 interrupt reason  */
        WRN_RXOV        = 0xFF02        /* -> detail = 0                    */
    }                   reason;
    USIGN32             detail;
} T_XCP_FDL;



    /* DPS exceptions */
    /*----------------*/

typedef struct
{
    enum {

        /* all modules */
        XCP_DPS_OTHER                   = 0x0000,
        XCP_DPS_UNEXPECTED_EVENT        = 0x0001,
        XCP_DPS_WRONG_SRC_TASK          = 0x0002,
        XCP_DPS_BLOCK_MEMORY            = 0x0003,
        XCP_DPS_WRONG_BLOCK_TYPE        = 0x0004,
        XCP_DPS_BLOCK_USAGE_COUNT       = 0x0005,
        XCP_DPS_ILLEGAL_SRD_BLOCK       = 0x0006,
        XCP_DPS_INVALID_USER_ACTION     = 0x0007,

        /* DPS_MAIN */
        XCP_DPS_UNKNOWN_OS_EVENT        = 0x1001,
        XCP_DPS_UNKNOWN_MSG_TYPE        = 0x1002,
        XCP_DPS_UNKNOWN_SRC_TASK_ID     = 0x1003,
        XCP_DPS_CORRUPTED_OS_MESSAGE    = 0x1004,
        XCP_DPS_CORRUPTED_FDL_ID_PTR    = 0x1005,
        XCP_DPS_ILLEGAL_SUBTASK         = 0x1006,
        XCP_DPS_INTERNAL_TIMER_ERROR    = 0x1007,
        XCP_DPS_ILLEGAL_TIMER_REFERENCE = 0x1008,
        XCP_DPS_ILLEGAL_USER_SERVICE    = 0x1009,
        XCP_DPS_ILLEGAL_USER_EVENT      = 0x100A,

        /* DPS_C1 */
        XCP_DPS_UNEXPECTED_DP_PRIMITIVE = 0x2003,
        XCP_DPS_UNEXPECTED_FDL_PRIMITIVE= 0x2004,
        XCP_DPS_WRONG_STATE             = 0x2005,
        XCP_DPS_SERVICE_ACTIVE          = 0x2006,
        XCP_DPS_ILL_SERVICE             = 0x2007,
        XCP_DPS_ILL_EVENT               = 0x2008,

        /* DPS_CYC */
        XCP_DPS_CYC_UNEXPECTED_EVENT    = 0x3002,
        XCP_DPS_CYC_INVALID_ACT_FUNKTION= 0x3003,
        XCP_DPS_CYC_INVALID_PRD_FUNKTION= 0x3004,
        XCP_DPS_CYC_UNEXPECTED_FDL_CON  = 0x3005,
        XCP_DPS_CYC_UNEXPECTED_SAP      = 0x3007,

        /* DPS_C2 */
        XCP_DPS_ILLEGAL_USER_REQ        = 0x4006,
        XCP_DPS_NO_MORE_SAP             = 0x4007,
        XCP_DPS_SAP_NOT_USED            = 0x4008,
        XCP_DPS_SAP_NOT_IN_LIST         = 0x4009,

        /* DPS_ACC2 */
        XCP_DPS_ILLEGAL_SAP_BLOCK       = 0x5002,
        XCP_DPS_ILLEGAL_SERVICE         = 0x5004

    }                   reason;

    USIGN32             detail_1;
    USIGN32             detail_2;
    USIGN16             line;
    char *              filename;

} T_XCP_DPS;


    /* PB DP USER exceptions */
    /*-----------------------*/

typedef struct
{
    enum {

        XCP_DUSR_OTHER                   = 0x0000,
        XCP_DUSR_ILLEGAL_SERVICE         = 0x0001,
        XCP_DUSR_STATE_MACHINE           = 0x0002,
        XCP_DUSR_ILLEGAL_MESSAGE_SOURCE  = 0x0003,
        XCP_DUSR_ILLEGAL_MESSAGE_TYPE    = 0x0004

    }                   reason;

    USIGN32             detail_1;
    USIGN32             detail_2;
    USIGN16             line;
    char *              filename;

} T_XCP_DUSR;



    /* PB FBS exceptions */
    /*-------------------*/

typedef struct
{
    enum {

        XCP_PFBS_OTHER                   = 0x0000,
        XCP_PFBS_ILLEGAL_SERVICE         = 0x0001,
        XCP_PFBS_SLAVE_SERVICE_FAILED    = 0x0002,
        XCP_PFBS_ACCESS_CYCLIC_OBJECT    = 0x0003,
        XCP_PFBS_DATABASE_CORRUPT        = 0x0004,
        XCP_PFBS_EEPROM_ERROR            = 0x0005,
        XCP_PFBS_HEAP_INSUFF             = 0x0006,

        XCP_PFBS_WRN_EXNV_RESET          = 0xFF00,
        XCP_PFBS_WRN_UPD_NV_FAILED       = 0xFF01,
        XCP_PFBS_WRN_UPD_EXNV_FAILED     = 0xFF01

    }                   reason;

    USIGN32             detail_1;
    USIGN32             detail_2;
    USIGN16             line;
    char *              filename;

} T_XCP_PFBS;


    /* PB FBLK exceptions */
    /*--------------------*/

typedef struct
{
    enum {

        XCP_PFBL_OTHER                   = 0x0000,
        XCP_PFBL_DATABASE_CORRUPT        = 0x0001,
        XCP_PFBL_EEP_ERR                 = 0x0002
    }                   reason;

    USIGN32             detail_1;
    USIGN32             detail_2;
    USIGN16             line;
    char *              filename;

} T_XCP_PFBL;


    /* PB APPL exceptions */
    /*--------------------*/

typedef struct
{
    enum {

        XCP_PAPP_OTHER                   = 0x0000,
        XCP_PAPP_INVALID_OS_EVENT        = 0x0001,
        XCP_PAPP_EEP_ERR                 = 0x0002
    }                   reason;

    USIGN32             detail_1;
    USIGN32             detail_2;
    USIGN16             line;
    char *              filename;

} T_XCP_PAPP;



    /* FDC exceptions */
    /*----------------*/
typedef struct
{
    enum {
        XCP_FDC_INVALID_OS_EVENT    = 0x0001        /* detail: event            */

    }                   reason;

    enum {
        XCP_FDC_LAS_UPDATE_TASK     = 0x0001,
        XCP_FDC_FB_EXEC_TASK        = 0x0002,
        XCP_FDC_SUBSCRIBER_TASK     = 0x0003,
        XCP_FDC_SEND_MVC_TASK       = 0x0004,
        XCP_FDC_BASE_TASK           = 0x0005,
        XCP_FDC_BACKGROUND_TASK   = 0x0006,
        XCP_FDC_WATCHDOG_TASK     = 0x0007
#ifdef SW_DOWNLOAD
        ,
        XCP_FDC_DWNLD_TASK   = 0x0008
#endif /* SW_DOWNLOAD */

    }                   location;

    USIGN32             detail;
} T_XCP_FDC;


    /* DLL exceptions */
    /*----------------*/
typedef struct
{
    enum {
        XCP_DLL_F1_EI               = 0x0004,       /* -> detail = F1 interrupt reason  */
        XCP_DLL_F1_XFIFO            = 0x0101,       /* -> detail = F1 xmit length reg.  */
        XCP_DLL_F1_RFIFO            = 0x0102,       /* -> detail = F1 FIFO status       */
        XCP_DLL_F1_NEPT             = 0x0103,       /* -> detail = 0                    */
        XCP_DLL_F1_TLM              = 0x0104,       /* -> detail = F1 xmit length reg.  */
        XCP_DLL_F1_JAB              = 0x0105,       /* -> detail = 0                    */
        XCP_DLL_F1_CNS              = 0x0106,       /* -> detail = 0                    */
        XCP_DLL_F1_CLK              = 0x0107,       /* -> detail = clock frequency      */
        XCP_DLL_TXOV                = 0x0110,       /* -> detail = request. xmit length */
        XCP_DLL_DMA_RX              = 0x0200,       /* -> detail = 0                    */
        XCP_DLL_DMA_TX              = 0x0201,       /* -> detail = 0                    */
        XCP_DLL_SPC                 = 0x0300,       /* -> detail = SPC4-2 IRR           */
        XCP_DLL_RCV_QUE_OVERFLOW    = 0x1001,
        XCP_DLL_QUE_INCONSIST       = 0x1002,
        WRN_DLL_INT                 = 0xFF01,       /* -> detail = F1 interrupt reason  */
        WRN_DLL_RXOV                = 0xFF02        /* -> detail = 0                    */
    }
                        reason;
    enum {
        XCP_LOC_NONRELEV            = 0x0000,
        XCP_DLL_MSG                 = 0x0001,
        XCP_DLL_FBC                 = 0x0002

    }                   location;

    USIGN32             detail;
} T_XCP_DLL;



    /* FAL exceptions */
    /*----------------*/
typedef struct
{
    enum {
        XCP_FAL_R                   = 0x0000,
        XCP_FAL_HEAP_INSUFF         = 0x0001,      /* detail: vcr           */
        XCP_FAL_SRV_GET             = 0x0002,
        XCP_FAL_SRV_PUT             = 0x0003,      /* details: table index  */
        XCP_FAL_ILLEGAL_DLL_MSG     = 0x0004,
        XCP_FAL_ILLEGAL_FAL_MSG     = 0x0005,
        XCP_FAL_RCV_QUE_OVERFLOW    = 0x0006,
        XCP_FAL_ABT_MONITOR         = 0x0007


    }                   reason;

    enum {
        XCP_FAL_SRV_TBL             = 0x0000,
        XCP_FAL_START               = 0x0001,
        XCP_FAL_TASK                = 0x0002,
        XCP_FAL_VCR                 = 0x0003

    }                   location;

    USIGN32             detail;
} T_XCP_FAL;



    /* FFBS exceptions */
    /*-----------------*/
typedef struct
{
    enum {
        XCP_FFBS_R                   = 0x0000,
        XCP_FFBS_HEAP_INSUFF         = 0x0001,  /* detail: requested mem size */

        XCP_FFBS_WRN_EXNV_RESET      = 0xFF00,
        XCP_FFBS_WRN_UPD_NV_FAILED   = 0xFF01,
        XCP_FFBS_WRN_UPD_EXNV_FAILED = 0xFF01

    }                   reason;

    enum {
        XCP_FFBS_L                   = 0x0000,
        XCP_FFBS_FBS_START           = 0x0001,

        XCP_FFBS_FBSNV_RESET_EXNV    = 0x0002,
        XCP_FFBS_FBSNV_CHK_EXNV      = 0x0003,
        XCP_FFBS_FBSNV_UPD_EXNV      = 0x0004,
        XCP_FFBS_FBSNV_UPD_NV        = 0x0005

    }                   location;

    USIGN32             detail;
} T_XCP_FFBS;



    /* FFBL exceptions */
    /*-----------------*/
typedef struct
{
    enum {
        XCP_FFBL_R                   = 0x0000,
        XCP_FFBL_BLK_ACC             = 0x0001   /* detail: index          */

    }                   reason;

    enum {
        XCP_FFBL_L                   = 0x0000,
        XCP_FFBL_READ_HANDLER        = 0x0001,
        XCP_FFBL_WRITE_HANDLER       = 0x0002

    }                   location;

    USIGN32             detail;
} T_XCP_FFBL;



    /* FAPP exceptions */
    /*-----------------*/
typedef struct
{
    enum {
        XCP_FAPP_R                   = 0x0000,
        XCP_FAPP_INVALID_OS_EVENT    = 0x0001,  /* detail: event             */
        XCP_FAPP_EEP_ERR             = 0x0002,  /* detail: EEP result        */
        XCP_FAPP_FDC_ERR             = 0x0003,  /* detail: FDC result        */
        XCP_FAPP_FBS_ERR             = 0x0004   /* detail: FBS result        */
    }                   reason;

    enum {
        XCP_FAPP_L                   = 0x0000,
        XCP_FAPP_BASE_TASK           = 0x0001,
        XCP_FAPP_CONFIG_GET          = 0x0002,
        XCP_FAPP_CONFIG_CHANGE       = 0x0003,
        XCP_FAPP_CONFIG_ACTIVATE     = 0x0004,
        XCP_FAPP_CONFIG_WRITE        = 0x0005,
        XCP_FAPP_CONFIG_LOAD         = 0x0006,
        XCP_FAPP_IDENT_LOAD          = 0x0007,
        XCP_FAPP_EEPROM_ACCESS       = 0x0008,
        XCP_FAPP_FIELD_DIAG_MISSING  = 0x0009
#if defined (DOWNLOAD_DEVICE) || defined (SW_DOWNLOAD)
        ,
        XCP_FAPP_FBK2_DWNLD_HANDLER_TASK = 0x000A
#endif

    }                   location;

    USIGN32             detail;
} T_XCP_FAPP;


    /* UTA exceptions */
    /*----------------*/
typedef struct
{
    enum {
        XCP_UTA_R                    = 0x0000,
        XCP_UTA_INVALID_OS_EVENT     = 0x0001,  /* detail: event             */
        XCP_UTA_EEP_ERR              = 0x0002,  /* detail: EEP result        */
        XCP_UTA_FDC_ERR              = 0x0003   /* detail: FDC result        */

    }                   reason;

    enum {
        XCP_UTA_L                    = 0x0000,
        XCP_UTA_BASE_TASK            = 0x0001,
        XCP_UTA_CONFIG_GET           = 0x0002,
        XCP_UTA_CONFIG_CHANGE        = 0x0003,
        XCP_UTA_CONFIG_ACTIVATE      = 0x0004,
        XCP_UTA_CONFIG_WRITE         = 0x0005,
        XCP_UTA_CONFIG_LOAD          = 0x0006,
        XCP_UTA_IDENT_LOAD           = 0x0007

    }                   location;

    USIGN32             detail;
} T_XCP_UTA;


    /* HM exceptions */
    /*-----------------*/
typedef struct
{
    enum {
        XCP_HM_R                   = 0x0000,
        XCP_HM_INVALID_OS_EVENT    = 0x0001, /* detail: invalid event     */
        XCP_HM_DB_INVALID_POINTER  = 0x0002  /* detail: invalid pointer to HM_DB - because of wrong dev_type */

    }                   reason;

    enum {
        XCP_HM_L                   = 0x0000,
        XCP_HM_BASE_TASK           = 0x0001,
        XCP_HM_HM_INIT             = 0x0002

    }                   location;

    USIGN32             detail;
} T_XCP_HM;

    /* MM exceptions */
    /*-----------------*/
typedef struct
{
    enum {
        XCP_MM_R                   = 0x0000,
        XCP_MM_INVALID_OS_EVENT    = 0x0001, /* detail: invalid event     */
        XCP_MM_DB_INVALID_POINTER  = 0x0002  /* detail: invalid pointer to MM_DB - because of wrong dev_type */

    }                   reason;

    enum {
        XCP_MM_L                   = 0x0000,
        XCP_MM_BASE_TASK           = 0x0001,
        XCP_MM_MM_INIT             = 0x0002

    }                   location;

    USIGN32             detail;
} T_XCP_MM;

    /* common interface for all exceptions */
    /*-------------------------------------*/
typedef union
{
    T_XCP_ASSERT    assertion;
    T_XCP_HW        hw;
    T_XCP_OS        os;
    T_XCP_EEP       eep;
    T_XCP_FDL       fdl;
    T_XCP_DPS       dps;
    T_XCP_DUSR      dusr;
    T_XCP_PFBS      pfbs;
    T_XCP_PFBL      pfbl;
    T_XCP_PAPP      papp;
    T_XCP_FDC       fdc;
    T_XCP_DLL       dll;
    T_XCP_FAL       fal;
    T_XCP_FFBS      ffbs;
    T_XCP_FFBL      ffbl;
    T_XCP_FAPP      fapp;
    T_XCP_UTA       uta;
    T_XCP_HM        hm;
    T_XCP_MM        mm;
} T_XCP_DSCR_U;

typedef struct
{
    T_XCP_CODE      xcp_code;
    T_XCP_DSCR_U    u;

} T_XCP_DSCR;


typedef VOID (T_EXCEPTION_HANDLER)
(
    T_XCP_TYPE          xcp_type,
    USIGN16             task_id,
    USIGN16             module_id,
    T_XCP_DSCR *        xcp_dscr
);

/* --- Exception codes ---------------------------------------------------- */
/* --- function declarations ---------------------------------------------- */

FUNCTION extern  T_EXCEPTION_HANDLER exception_handler;


/****************************************************************************/
/*** MACROS                                                               ***/
/****************************************************************************/

  #include "da_except.h"
  NO_INIT extern T_XCP_DSCR xcp_dscr;
  #include "da_def.h"

/* --- Assertions --------------------------------------------------------- */

#ifdef _DEBUG

  #define _ASSERT(condition)                            \
      {                                                                     \
        if ( !(condition) )                                                 \
        {                                                                   \
            xcp_dscr.xcp_code = XCP_ASSERT;                                 \
            xcp_dscr.u.assertion.filename = __FILE__;                       \
            xcp_dscr.u.assertion.line = __LINE__;                           \
            exception_handler (XCP_ASSERTION, 0, MODULE_ID, &xcp_dscr);     \
        }                                                                   \
      }
#else
  #define _ASSERT(condition)
#endif


/* --- Warnings ----------------------------------------------------------- */

#ifdef _DEBUG
#define _EEP_WARNING(xcp_reason, blk_id, xcp_detail)    \
      do {                                                                  \
          xcp_dscr.xcp_code = XCP_EEP;                                      \
          xcp_dscr.u.eep.reason   = xcp_reason;                             \
          xcp_dscr.u.eep.detail   = (USIGN32)(xcp_detail);                  \
          xcp_dscr.u.eep.block_id = (USIGN8)(blk_id);                       \
          exception_handler (XCP_WARNING, 0, MODULE_ID, &xcp_dscr);         \
      } while (0)

#define _FDL_WARNING(xcp_reason, xcp_detail)                    \
      do {                                                                  \
          T_XCP_DSCR xcp_dscr;                                              \
          xcp_dscr.xcp_code = XCP_FDL;                                      \
          xcp_dscr.u.fdl.reason   = xcp_reason;                             \
          xcp_dscr.u.fdl.detail   = (USIGN32)(xcp_detail);                  \
          exception_handler (XCP_WARNING, 0, MODULE_ID, &xcp_dscr);         \
      } while (0)

#define _DPS_WARNING(xcp_reason, xcp_detail_1, xcp_detail_2)    \
      do {                                                                  \
          xcp_dscr.xcp_code = XCP_DPS;                                      \
          xcp_dscr.u.dps.reason   = xcp_reason;                             \
          xcp_dscr.u.dps.detail_1 = (USIGN32)(xcp_detail_1);                \
          xcp_dscr.u.dps.detail_2 = (USIGN32)(xcp_detail_2);                \
          xcp_dscr.u.dps.filename = __FILE__;                               \
          xcp_dscr.u.dps.line = __LINE__;                                   \
          exception_handler (XCP_WARNING, DPS_TASK_ID, MODULE_ID, &xcp_dscr);   \
      } while (0)

#define _PFBS_WARNING(xcp_reason, xcp_detail_1, xcp_detail_2)   \
      do {                                                                  \
          xcp_dscr.xcp_code = XCP_PFBS;                                     \
          xcp_dscr.u.pfbs.reason   = xcp_reason;                            \
          xcp_dscr.u.pfbs.detail_1 = (USIGN32)(xcp_detail_1);               \
          xcp_dscr.u.pfbs.detail_2 = (USIGN32)(xcp_detail_2);               \
          xcp_dscr.u.pfbs.filename = __FILE__;                              \
          xcp_dscr.u.pfbs.line = __LINE__;                                  \
          exception_handler (XCP_WARNING, FBS_TASK_ID, MODULE_ID, &xcp_dscr);   \
      } while (0)

#define _PFBL_WARNING(xcp_reason, xcp_detail_1, xcp_detail_2)   \
      do {                                                                  \
          xcp_dscr.xcp_code = XCP_PFBL;                                     \
          xcp_dscr.u.pfbl.reason   = xcp_reason;                            \
          xcp_dscr.u.pfbl.detail_1 = (USIGN32)(xcp_detail_1);               \
          xcp_dscr.u.pfbl.detail_2 = (USIGN32)(xcp_detail_2);               \
          xcp_dscr.u.pfbl.filename = __FILE__;                              \
          xcp_dscr.u.pfbl.line = __LINE__;                                  \
          exception_handler (XCP_WARNING, FB_TASK_ID, MODULE_ID, &xcp_dscr);\
      } while (0)

#define _FFBS_WARNING(xcp_reason, xcp_location, xcp_detail)     \
      do {                                                                  \
          xcp_dscr.xcp_code = XCP_FFBS;                                     \
          xcp_dscr.u.ffbs.reason   = xcp_reason;                            \
          xcp_dscr.u.ffbs.location = xcp_location;                          \
          xcp_dscr.u.ffbs.detail   = xcp_detail;                            \
          exception_handler (XCP_WARNING, BGD50_TASK_ID, MODULE_ID, &xcp_dscr);   \
      } while (0)

#define _DLL_WARNING(xcp_reason, xcp_detail)                    \
      do {                                                                  \
          xcp_dscr.xcp_code = XCP_DLL;                                      \
          xcp_dscr.u.fdl.reason   = xcp_reason;                             \
          xcp_dscr.u.fdl.detail   = (USIGN32)(xcp_detail);                  \
          exception_handler (XCP_WARNING, 0, MODULE_ID, &xcp_dscr);         \
      } while (0)

#else
#define _EEP_WARNING(xcp_reason, block_id, xcp_detail)
#define _FDL_WARNING(xcp_reason, xcp_detail)
#define _DPS_WARNING(xcp_reason, xcp_detail_1, xcp_detail_2)
#define _DLL_WARNING(xcp_reason, xcp_detail)
#define _PFBS_WARNING(xcp_reason, xcp_detail_1, xcp_detail_2)
#define _FFBS_WARNING(xcp_reason, xcp_location, xcp_detail)
#endif /* _DEBUG */


/* --- Error Exceptions --------------------------------------------------- */

#define _HW_EXCEPTION(xcp_reason, xcp_detail)                   \
      do {                                                                  \
          xcp_dscr.xcp_code = XCP_HW;                                       \
          xcp_dscr.u.hw.reason   = xcp_reason;                              \
          xcp_dscr.u.hw.detail   = (USIGN32)(xcp_detail);                   \
          exception_handler (XCP_ERROR, 0, MODULE_ID, &xcp_dscr);           \
      } while (0)

#define _OS_EXCEPTION(xcp_reason, xcp_taskid, xcp_loc, xcp_detail)  \
      do {                                                                  \
          xcp_dscr.xcp_code = XCP_OS;                                       \
          xcp_dscr.u.os.reason   = xcp_reason;                              \
          xcp_dscr.u.os.location = xcp_loc;                                 \
          xcp_dscr.u.os.detail   = (USIGN32)(xcp_detail);                   \
          exception_handler (XCP_ERROR, (USIGN16) xcp_taskid, MODULE_ID, &xcp_dscr);  \
      } while (0)

#define _EEP_EXCEPTION(xcp_reason, blk_id, xcp_detail)          \
      do {                                                                  \
          xcp_dscr.xcp_code = XCP_EEP;                                      \
          xcp_dscr.u.eep.reason   = xcp_reason;                             \
          xcp_dscr.u.eep.detail   = (USIGN32)(xcp_detail);                  \
          xcp_dscr.u.eep.block_id = (USIGN8)(blk_id);                       \
          exception_handler (XCP_ERROR, 0, MODULE_ID, &xcp_dscr);           \
      } while (0)

#define _FDL_EXCEPTION(xcp_reason, xcp_detail)                  \
      do {                                                                  \
          xcp_dscr.xcp_code = XCP_FDL;                                      \
          xcp_dscr.u.fdl.reason   = xcp_reason;                             \
          xcp_dscr.u.fdl.detail   = (USIGN32)(xcp_detail);                  \
          exception_handler (XCP_ERROR, 0, MODULE_ID, &xcp_dscr);           \
      } while (0)

#define _DPS_EXCEPTION(xcp_reason, xcp_detail_1, xcp_detail_2)  \
      do {                                                                  \
          xcp_dscr.xcp_code = XCP_DPS;                                      \
          xcp_dscr.u.dps.reason   = xcp_reason;                             \
          xcp_dscr.u.dps.detail_1 = (USIGN32)(xcp_detail_1);                \
          xcp_dscr.u.dps.detail_2 = (USIGN32)(xcp_detail_2);                \
          xcp_dscr.u.dps.filename = __FILE__;                               \
          xcp_dscr.u.dps.line = __LINE__;                                   \
          exception_handler (XCP_ERROR, DPS_TASK_ID, MODULE_ID, &xcp_dscr); \
      } while (0)

#define _DUSR_EXCEPTION(xcp_reason, xcp_detail_1, xcp_detail_2) \
      do {                                                                  \
          xcp_dscr.xcp_code = XCP_DUSR;                                     \
          xcp_dscr.u.dusr.reason   = xcp_reason;                            \
          xcp_dscr.u.dusr.detail_1 = (USIGN32)(xcp_detail_1);               \
          xcp_dscr.u.dusr.detail_2 = (USIGN32)(xcp_detail_2);               \
          xcp_dscr.u.dusr.filename = __FILE__;                              \
          xcp_dscr.u.dusr.line = __LINE__;                                  \
          exception_handler (XCP_ERROR, DUSR_TASK_ID, MODULE_ID, &xcp_dscr);    \
      } while (0)

#define _PFBS_EXCEPTION(xcp_reason, xcp_detail_1, xcp_detail_2) \
      do {                                                                  \
          xcp_dscr.xcp_code = XCP_PFBS;                                     \
          xcp_dscr.u.pfbs.reason   = xcp_reason;                            \
          xcp_dscr.u.pfbs.detail_1 = (USIGN32)(xcp_detail_1);               \
          xcp_dscr.u.pfbs.detail_2 = (USIGN32)(xcp_detail_2);               \
          xcp_dscr.u.pfbs.filename = __FILE__;                              \
          xcp_dscr.u.pfbs.line = __LINE__;                                  \
          exception_handler (XCP_ERROR, FBS_TASK_ID, MODULE_ID, &xcp_dscr);     \
      } while (0)

#define _PFBL_EXCEPTION(xcp_reason, xcp_detail_1, xcp_detail_2) \
      do {                                                                  \
          xcp_dscr.xcp_code = XCP_PFBL;                                     \
          xcp_dscr.u.pfbl.reason   = xcp_reason;                            \
          xcp_dscr.u.pfbl.detail_1 = (USIGN32)(xcp_detail_1);               \
          xcp_dscr.u.pfbl.detail_2 = (USIGN32)(xcp_detail_2);               \
          xcp_dscr.u.pfbl.filename = __FILE__;                              \
          xcp_dscr.u.pfbl.line = __LINE__;                                  \
          exception_handler (XCP_ERROR, FB_TASK_ID, MODULE_ID, &xcp_dscr);  \
      } while (0)

#define _FDC_EXCEPTION(xcp_reason, xcp_location, xcp_detail)    \
      do {                                                                  \
          xcp_dscr.xcp_code = XCP_FCIF;                                     \
          xcp_dscr.u.fdc.reason   = xcp_reason;                             \
          xcp_dscr.u.fdc.location = xcp_location;                           \
          xcp_dscr.u.fdc.detail   = (USIGN32)(xcp_detail);                  \
          exception_handler (XCP_ERROR, 0, MODULE_ID, &xcp_dscr);           \
      } while (0)

#define _DLL_EXCEPTION(xcp_reason, xcp_location, xcp_detail)    \
      do {                                                                  \
          xcp_dscr.xcp_code = XCP_DLL;                                      \
          xcp_dscr.u.dll.reason   = xcp_reason;                             \
          xcp_dscr.u.dll.location = xcp_location;                           \
          xcp_dscr.u.dll.detail   = (USIGN32)(xcp_detail);                  \
          exception_handler (XCP_ERROR, 0, MODULE_ID, &xcp_dscr);           \
      } while (0)

#define _FAL_EXCEPTION(xcp_reason, xcp_location, xcp_detail)    \
      do {                                                                  \
          xcp_dscr.xcp_code = XCP_FAL;                                      \
          xcp_dscr.u.fal.reason   = xcp_reason;                             \
          xcp_dscr.u.fal.location = xcp_location;                           \
          xcp_dscr.u.fal.detail   = (USIGN32)(xcp_detail);                  \
          exception_handler (XCP_ERROR, 0, MODULE_ID, &xcp_dscr);           \
      } while (0)

#define _FFBS_EXCEPTION(xcp_reason, xcp_location, xcp_detail)   \
      do {                                                                  \
          xcp_dscr.xcp_code = XCP_FFBS;                                     \
          xcp_dscr.u.ffbs.reason   = xcp_reason;                            \
          xcp_dscr.u.ffbs.location = xcp_location;                          \
          xcp_dscr.u.ffbs.detail   = (USIGN32)(xcp_detail);                 \
          exception_handler (XCP_ERROR, 0, MODULE_ID, &xcp_dscr);           \
      } while (0)

#define _FFBL_EXCEPTION(xcp_reason, xcp_location, xcp_detail)   \
      do {                                                                  \
          xcp_dscr.xcp_code = XCP_FFBL;                                     \
          xcp_dscr.u.ffbl.reason   = xcp_reason;                            \
          xcp_dscr.u.ffbl.location = xcp_location;                          \
          xcp_dscr.u.ffbl.detail   = (USIGN32)(xcp_detail);                 \
          exception_handler (XCP_ERROR, 0, MODULE_ID, &xcp_dscr);           \
      } while (0)

#define _FAPP_EXCEPTION(xcp_reason, xcp_location, xcp_detail)   \
      do {                                                                  \
          xcp_dscr.xcp_code = XCP_FAPP;                                     \
          xcp_dscr.u.fapp.reason   = xcp_reason;                            \
          xcp_dscr.u.fapp.location = xcp_location;                          \
          xcp_dscr.u.fapp.detail   = (USIGN32)(xcp_detail);                 \
          exception_handler (XCP_ERROR, 0, MODULE_ID, &xcp_dscr);           \
      } while (0)

/* --- Fatal Exceptions --------------------------------------------------- */

#define _HW_FATAL(xcp_reason, xcp_detail)                       \
      do {                                                                  \
          xcp_dscr.xcp_code = XCP_HW;                                       \
          xcp_dscr.u.hw.reason   = xcp_reason;                              \
          xcp_dscr.u.hw.detail   = (USIGN32)(xcp_detail);                   \
          exception_handler (XCP_FATAL, 0, MODULE_ID, &xcp_dscr);           \
      } while (0)

#define _OS_FATAL(xcp_reason, xcp_taskid, xcp_loc, xcp_detail)  \
      do {                                                                  \
          xcp_dscr.xcp_code = XCP_OS;                                       \
          xcp_dscr.u.os.reason   = xcp_reason;                              \
          xcp_dscr.u.os.location = xcp_loc;                                 \
          xcp_dscr.u.os.detail   = (USIGN32)(xcp_detail);                   \
          exception_handler (XCP_FATAL, (USIGN16) xcp_taskid, MODULE_ID, &xcp_dscr);  \
      } while (0)

#define _EEP_FATAL(xcp_reason, blk_id, xcp_detail)              \
      do {                                                                  \
          xcp_dscr.xcp_code = XCP_EEP;                                      \
          xcp_dscr.u.eep.reason   = xcp_reason;                             \
          xcp_dscr.u.eep.detail   = (USIGN32)(xcp_detail);                  \
          xcp_dscr.u.eep.block_id = (USIGN8)(blk_id);                       \
          exception_handler (XCP_FATAL, 0, MODULE_ID, &xcp_dscr);           \
      } while (0)

#define _FDL_FATAL(xcp_reason, xcp_detail)                      \
      do {                                                                  \
          xcp_dscr.xcp_code = XCP_FDL;                                      \
          xcp_dscr.u.fdl.reason   = xcp_reason;                             \
          xcp_dscr.u.fdl.detail   = (USIGN32)(xcp_detail);                  \
          exception_handler (XCP_FATAL, 0, MODULE_ID, &xcp_dscr);           \
      } while (0)

#define _DPS_FATAL(xcp_reason, xcp_detail_1, xcp_detail_2)      \
      do {                                                                  \
          xcp_dscr.xcp_code = XCP_DPS;                                      \
          xcp_dscr.u.dps.reason   = xcp_reason;                             \
          xcp_dscr.u.dps.detail_1 = (USIGN32)(xcp_detail_1);                \
          xcp_dscr.u.dps.detail_2 = (USIGN32)(xcp_detail_2);                \
          xcp_dscr.u.dps.filename = __FILE__;                               \
          xcp_dscr.u.dps.line = __LINE__;                                   \
          exception_handler (XCP_FATAL, DPS_TASK_ID, MODULE_ID, &xcp_dscr);     \
      } while (0)

#define _PFBS_FATAL(xcp_reason, xcp_detail_1, xcp_detail_2)     \
      do {                                                                  \
          xcp_dscr.xcp_code = XCP_PFBS;                                     \
          xcp_dscr.u.pfbs.reason   = xcp_reason;                            \
          xcp_dscr.u.pfbs.detail_1 = (USIGN32)(xcp_detail_1);               \
          xcp_dscr.u.pfbs.detail_2 = (USIGN32)(xcp_detail_2);               \
          xcp_dscr.u.pfbs.filename = __FILE__;                              \
          xcp_dscr.u.pfbs.line = __LINE__;                                  \
          exception_handler (XCP_FATAL, 0, MODULE_ID, &xcp_dscr);           \
      } while (0)

#define _PFBL_FATAL(xcp_reason, xcp_detail_1, xcp_detail_2)     \
      do {                                                                  \
         xcp_dscr.xcp_code = XCP_PFBL;                                      \
         xcp_dscr.u.pfbl.reason   = xcp_reason;                             \
         xcp_dscr.u.pfbl.detail_1 = (USIGN32)(xcp_detail_1);                \
         xcp_dscr.u.pfbl.detail_2 = (USIGN32)(xcp_detail_2);                \
         xcp_dscr.u.pfbl.filename = __FILE__;                               \
         xcp_dscr.u.pfbl.line = __LINE__;                                   \
         exception_handler (XCP_FATAL, 0, MODULE_ID, &xcp_dscr);            \
      } while (0)

#define _PAPP_FATAL(xcp_reason, xcp_detail_1, xcp_detail_2)     \
      do {                                                                  \
         xcp_dscr.xcp_code = XCP_PAPP;                                      \
         xcp_dscr.u.papp.reason   = xcp_reason;                             \
         xcp_dscr.u.papp.detail_1 = (USIGN32)(xcp_detail_1);                \
         xcp_dscr.u.papp.detail_2 = (USIGN32)(xcp_detail_2);                \
         xcp_dscr.u.papp.filename = __FILE__;                               \
         xcp_dscr.u.papp.line = __LINE__;                                   \
         exception_handler (XCP_FATAL, 0, MODULE_ID, &xcp_dscr);            \
      } while (0)

#define _FDC_FATAL(xcp_reason, xcp_location, xcp_detail)        \
      do {                                                                  \
          xcp_dscr.xcp_code = XCP_FCIF;                                     \
          xcp_dscr.u.fdc.reason   = xcp_reason;                             \
          xcp_dscr.u.fdc.location = xcp_location;                           \
          xcp_dscr.u.fdc.detail   = (USIGN32)(xcp_detail);                  \
          exception_handler (XCP_FATAL, 0, MODULE_ID, &xcp_dscr);           \
      } while (0)

#define _DLL_FATAL(xcp_reason, xcp_location, xcp_detail)        \
      do {                                                                  \
          xcp_dscr.xcp_code = XCP_DLL;                                      \
          xcp_dscr.u.dll.reason   = xcp_reason;                             \
          xcp_dscr.u.dll.location = xcp_location;                           \
          xcp_dscr.u.dll.detail   = (USIGN32)(xcp_detail);                  \
          exception_handler (XCP_FATAL, 0, MODULE_ID, &xcp_dscr);           \
      } while (0)

#define _FAL_FATAL(xcp_reason, xcp_location, xcp_detail)        \
      do {                                                                  \
          xcp_dscr.xcp_code = XCP_FAL;                                      \
          xcp_dscr.u.fal.reason   = xcp_reason;                             \
          xcp_dscr.u.fal.location = xcp_location;                           \
          xcp_dscr.u.fal.detail   = (USIGN32)(xcp_detail);                  \
          exception_handler (XCP_FATAL, 0, MODULE_ID, &xcp_dscr);           \
      } while (0)

#define _FFBS_FATAL(xcp_reason, xcp_location, xcp_detail)     \
      do {                                                                  \
          xcp_dscr.xcp_code = XCP_FFBS;                                     \
          xcp_dscr.u.ffbs.reason   = xcp_reason;                            \
          xcp_dscr.u.ffbs.location = xcp_location;                          \
          xcp_dscr.u.ffbs.detail   = (USIGN32)(xcp_detail);                 \
          exception_handler (XCP_FATAL, 0, MODULE_ID, &xcp_dscr);           \
      } while (0)

#define _FFBL_FATAL(xcp_reason, xcp_location, xcp_detail)     \
      do {                                                                  \
          xcp_dscr.xcp_code = XCP_FFBL;                                     \
          xcp_dscr.u.ffbl.reason   = xcp_reason;                            \
          xcp_dscr.u.ffbl.location = xcp_location;                          \
          xcp_dscr.u.ffbl.detail   = (USIGN32)(xcp_detail);                 \
          exception_handler (XCP_FATAL, 0, MODULE_ID, &xcp_dscr);           \
      } while (0)

#define _FAPP_FATAL(xcp_reason, xcp_location, xcp_detail)     \
      do {                                                                  \
          xcp_dscr.xcp_code = XCP_FAPP;                                     \
          xcp_dscr.u.fapp.reason   = xcp_reason;                            \
          xcp_dscr.u.fapp.location = xcp_location;                          \
          xcp_dscr.u.fapp.detail   = (USIGN32)(xcp_detail);                 \
          exception_handler (XCP_FATAL, 0, MODULE_ID, &xcp_dscr);           \
      } while (0)

#define _UTA_FATAL(xcp_reason, xcp_location, xcp_detail)     \
      do {                                                                  \
          xcp_dscr.xcp_code = XCP_UTA;                                      \
          xcp_dscr.u.uta.reason   = xcp_reason;                             \
          xcp_dscr.u.uta.location = xcp_location;                           \
          xcp_dscr.u.uta.detail   = (USIGN32)(xcp_detail);                  \
          exception_handler (XCP_FATAL, 0, MODULE_ID, &xcp_dscr);           \
      } while (0)

#define _HM_FATAL(xcp_reason, xcp_location, xcp_detail)     \
                        {                                                   \
                            xcp_dscr.xcp_code = XCP_HM;                          \
                            xcp_dscr.u.hm.reason   = xcp_reason;                 \
                            xcp_dscr.u.hm.location = xcp_location;               \
                            xcp_dscr.u.hm.detail   = (USIGN32)(xcp_detail);      \
                            exception_handler (XCP_FATAL, 0, MODULE_ID, &xcp_dscr);  \
                        }

#define _MM_FATAL(xcp_reason, xcp_location, xcp_detail)     \
                        {                                                   \
                            xcp_dscr.xcp_code = XCP_MM;                          \
                            xcp_dscr.u.mm.reason   = xcp_reason;                 \
                            xcp_dscr.u.mm.location = xcp_location;               \
                            xcp_dscr.u.mm.detail   = (USIGN32)(xcp_detail);      \
                            exception_handler (XCP_FATAL, 0, MODULE_ID, &xcp_dscr);  \
                        }

#endif /* __EXCEPT__ */

