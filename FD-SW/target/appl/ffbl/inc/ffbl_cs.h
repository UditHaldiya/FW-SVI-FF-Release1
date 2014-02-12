/*****************************************************************************
*                                                                            *
* PROJECT_NAME																 *
*                                                                            *
* VERSION																	 *
*																			 *
*                                                                            *
* DATE                     29. Jan. 2013                                     *
*                                                                            *
*****************************************************************************/


/* ===========================================================================

FILE_NAME          ffbl_cs.h



=========================================================================== */
#ifndef __FFBL_CS_H__
#define __FFBL_CS_H__

/*****************************************************************************
*  Function Block Application can add other attributes to this structure.
*  These other attributes must be added at the end, and are not visible to
*  Function Block Shell and to Communication.
*****************************************************************************/
#define CS_PAR_BLK_DATA          0
#define CS_PAR_ST_REV            1
#define CS_PAR_TAG_DESC          2
#define CS_PAR_STRATEGY          3
#define CS_PAR_ALERT_KEY         4
#define CS_PAR_MODE_BLK          5
#define CS_PAR_BLOCK_ERR         6
#define CS_PAR_OUT               7
#define CS_PAR_OUT_SCALE         8
#define CS_PAR_GRANT_DENY        9
#define CS_PAR_STATUS_OPTS      10
#define CS_PAR_SEL_1            11
#define CS_PAR_SEL_2            12
#define CS_PAR_SEL_3            13
#define CS_PAR_SEL_TYPE			14

#define CS_PAR_OUT_HI_LIM		16
#define CS_PAR_OUT_LO_LIM		17

#define CS_PAR_UPDATE_EVT       21
#define CS_PAR_BLOCK_ALM        22
#define CS_PAR_BLOCK_ERR_DESC_1 23

INCLUDES

    //#include "csfb.h"

    GLOBAL_DEFINES

    LOCAL_DEFINES

    EXPORT_TYPEDEFS

    LOCAL_TYPEDEFS

    //That structure presents the block parameters as defined in GW script.
    //
    typedef struct
{
    F_BLOCK               blk_data;
    USIGN16               st_rev;
    STRINGV               tag_desc[32];
    USIGN16               strategy;
    USIGN8                alert_key;
    MODE                  mode_blk;
    USIGN16               block_err;
    FLOAT_S               out;
    SCALE                 out_scale;
    ACCESS_PERM           grant_deny;
    USIGN16               status_opts;
    FLOAT_S               sel_1;
    FLOAT_S               sel_2;
    FLOAT_S               sel_3;
    USIGN8				sel_type;
    FLOAT_S               bkcal_in;
    FLOAT                 out_hi_lim;
    FLOAT                 out_lo_lim;
    FLOAT_S               bkcal_sel_1;
    FLOAT_S               bkcal_sel_2;
    FLOAT_S               bkcal_sel_3;
    EVENT                 update_evt;
    ALARM_DISCRETE        block_alm;
    USIGN32               block_err_desc_1;
} T_CSFB;
//
//#define T_CSFB T_FBIF_CSFB

//That structure is used to snapp parameters, pass data between function, and store data between execution cycles.
typedef struct
{
    FLOAT_S                   out;
    USIGN16                   old_block_alarm_bits;
} T_CSFB_DATA;


//Allowed values for STATUS_OPTS parameter
#define CS_STATUS_OPTS  (STATUS_OPT_USE_UNCERT_AS_GOOD | STATUS_OPT_IFS_IF_BAD_IN)

//Allowed values for SEL_TYPE parameter
#define CS_SEL_TYPE_UNINITIALIZED   0
#define CS_SEL_TYPE_HI				      1
#define CS_SEL_TYPE_LOW				      2
#define CS_SEL_TYPE_MIDDLE          3

typedef struct
{
    FLOAT_S * p_sel;
    FLOAT_S * p_bkcal_sel;
} T_IO_PAIR;

/* BLOCK_ERR_DESC_1 For CS block */
#define BLK_ERR_CS_SELECT_TYPE_UNINIT       BLK_ERR_SELECT_TYPE_UNINIT //0x00008000
#define BLK_ERR_CS_EXE_TIME                 BLK_ERR_EXE_TIME //0x00000001
#define BLK_ERR_CS_HI_LIM_RANGE             BLK_ERR_OUT_LIM_SET
#define BLK_ERR_CS_LO_LIM_RANGE             BLK_ERR_OUT_LIM_SET


#endif /* __FFBL_CS_H__ */
