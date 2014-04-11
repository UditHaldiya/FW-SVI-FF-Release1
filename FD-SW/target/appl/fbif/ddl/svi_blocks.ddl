/*
  Time-stamp: <2013-08-13 11:27:05 GE-300010777>

  This files includes all variable, records definitions for svi function blocks,
  except the transducer block.
  Documents:
  [[file:d:/Doc_index/FF_Index/SViFF/FF_Parameter_Mapping_RB-07-19.xlsx]]

*/

#include "svi_blocks.h"


/*===========================================/
* Resouce Block 2
/===========================================*/




/*-------------------------------------------/
** Variable & Record
/-------------------------------------------*/
RECORD svi2_rb_mode_blk
{
    LABEL           [mode_blk_label] ;
    HELP            [mode_blk_help] ;
    MEMBERS
    {
        TARGET,       svi2_rb_target_mode ;
        ACTUAL,       __actual_mode ;
        PERMITTED,    svi2_rb_permitted_mode ;
        NORMAL,       svi2_rb_normal_mode ;
    }
}

VARIABLE svi2_rb_target_mode
{
    LABEL           [target_mode] ;
    HELP            [target_mode_help] ;
    CLASS           CONTAINED & OPERATE;
    TYPE            BIT_ENUMERATED (1)
    {
        _MODE_AUTOMATIC,
        _MODE_OUT_OF_SERVICE
    }
    CONSTANT_UNIT   [blank] ;
    HANDLING        READ & WRITE ;
/*  RESPONSE_CODES  xxx ; */
}


VARIABLE  svi2_rb_permitted_mode
{
    LABEL           [permitted_mode] ;
    HELP            [permitted_mode_help] ;
    CLASS           CONTAINED & OPERATE ;
    TYPE            BIT_ENUMERATED (1)
    {
        _PERMITTED_AUTOMATIC,
        _PERMITTED_OUT_OF_SERVICE
    }
    CONSTANT_UNIT   [blank] ;
    HANDLING        READ & WRITE ;
/*  RESPONSE_CODES  xxx ; */
}

VARIABLE svi2_rb_normal_mode
{
    LABEL           [normal_mode] ;
    HELP            [normal_mode_help] ;
    CLASS           CONTAINED & OPERATE ;
    TYPE            BIT_ENUMERATED (1)
    {
        _MODE_AUTOMATIC,
        _MODE_OUT_OF_SERVICE
    }
    CONSTANT_UNIT   [blank] ;
    HANDLING        READ & WRITE ;
/*  RESPONSE_CODES  xxx ; */
}

VARIABLE capability_lev
{
    LABEL LBL_042_CAPABILITY_LEV;
    HELP  HLP_042_CAPABILITY_LEV;
    CLASS CONTAINED;
    TYPE ENUMERATED (1)
	{
        { 1, "|en|1 - No diagnostics"}
        { 2, "|en|2 - Standard Diagnostics"}
        { 3, "|en|3 - Advanced Diagnostics"}
        { 4, "|en|4 - Advanced Diagnostics with Extended Control"}
	}
	HANDLING READ;

}

VARIABLE software_rev
{
    LABEL LBL_043_SOFTWARE_REV;
    HELP  HLP_043_SOFTWARE_REV;
    CLASS CONTAINED;
    TYPE VISIBLE (32);
	HANDLING READ;
}

VARIABLE hardware_rev
{
    LABEL LBL_044_HARDWARE_REV;
    HELP  HLP_044_HARDWARE_REV;
    CLASS CONTAINED;
    TYPE VISIBLE (32);
	HANDLING READ;
}

VARIABLE compatibility_rev
{
    LABEL LBL_045_COMPATIBILITY_REV;
    HELP  HLP_045_COMPATIBILITY_REV;
    CLASS CONTAINED;
    TYPE UNSIGNED_INTEGER (1);
	HANDLING READ;
}

/* VARIABLE block_err_desc_1 */
/* { */
/*     LABEL LBL_046_BLOCK_ERR_DESC_1; */
/*     HELP  HLP_046_BLOCK_ERR_DESC_1; */
/*     CLASS CONTAINED; */
/*     TYPE  BITSTRING (32); */
/* } */

RECORD wrong_configuration
{
    LABEL LBL_047_WRONG_CONFIGURATION;
    HELP  HLP_047_WRONG_CONFIGURATION;
	MEMBERS
    {
        INDEX_WRONG_PARAM,       index_wrong_param;
        SUB_INDEX,   sub_index;
        DESCRIPTION, description;
    }
}

/* } */

VARIABLE index_wrong_param
{
    LABEL LBL_047_01_INDEX;
    HELP  HLP_047_01_INDEX;
    CLASS CONTAINED;
    TYPE UNSIGNED_INTEGER (4);
	HANDLING READ;
}

VARIABLE sub_index
{
    LABEL LBL_047_02_SUB_INDEX;
    HELP  HLP_047_02_SUB_INDEX;
    CLASS CONTAINED;
    TYPE UNSIGNED_INTEGER (4);
	HANDLING READ;
}

VARIABLE description
{
    LABEL LBL_047_03_DESCRIPTION;
    HELP  HLP_047_03_DESCRIPTION;
    CLASS CONTAINED;
    TYPE VISIBLE (32);
	HANDLING READ;

}

VARIABLE software_rev_ff
{
    LABEL LBL_048_SOFTWARE_REV_FF;
    HELP  HLP_048_SOFTWARE_REV_FF;
    CLASS CONTAINED;
    TYPE UNSIGNED_INTEGER (4);
}

VARIABLE software_rev_app
{
    LABEL LBL_049_SOFTWARE_REV_APP;
    HELP  HLP_049_SOFTWARE_REV_APP;
    CLASS CONTAINED;
    TYPE UNSIGNED_INTEGER (4);

}

/*-------------------------------------------/
** Methods
/-------------------------------------------*/

METHOD  restart
{
    LABEL "|en|restart";
	HELP "|en|restart";
    CLASS OUTPUT;
    DEFINITION
    {
        unsigned long   id;                 /*  item id  */
        unsigned long   dummy[1];
        long            status;             /*  error return from builtins  */
        int             ivalue;

        id = ITEM_ID(PARAM.RESTART);
        status = select_from_menu("|en|Please select the restart type.\n", dummy, dummy, 0,
                                   "Run;Resource;Defaults;Processor;Factory Defaults", &ivalue);

        switch( ivalue ) {
        case 1:
            status = put_unsigned_value(id, 0, 1);
            break;
        case 2:
            status = put_unsigned_value(id, 0, 2);
            break;
        case 3:
            status = put_unsigned_value(id, 0, 3);
            break;
        case 4:
            status = put_unsigned_value(id, 0, 4);
            break;
        case 5:
            status = put_unsigned_value(id, 0, 5);
            break;
        default:
            break;
        }

        SEND_PARAM(id, 0, "RB_RESTART");

        return;
    }
}

/*===========================================/
* Input Selector Block
/===========================================*/


/*===========================================/
* Multiple Analog Input block (MAI)
/===========================================*/


/*===========================================/
* Analog Input block (AI)
/===========================================*/



/*===========================================/
* Digital Input block (DI)
/===========================================*/



/*===========================================/
* Analog Output block (AO)
/===========================================*/



/*===========================================/
* Digital Output block (DO)
/===========================================*/

/*-------------------------------------------/
** Variable & Record
/-------------------------------------------*/

/*===========================================/
* Proportional Integral Derivative block (PID)
/===========================================*/



/*===========================================/
* Output Splitter Block
/===========================================*/



/*===========================================/
* AI transducer block
/===========================================*/

/*===========================================/
* Revision History
/===========================================*/

/****************************************************************************
 <2012-09-03> #25213: add the entries for menus and methods for TB & RB
 <2012-08-21> #25006: updated for new FF_Parameter_Mapping_RB.xlsx(08-13)
*****************************************************************************/

