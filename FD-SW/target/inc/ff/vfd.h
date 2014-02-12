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
*                     Copyright (C) SOFTING IA GmbH 2013                     *
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
* PROJECT_NAME             Softing FF/PA FD 2.42.A                           *
*                                                                            *
* VERSION                  FF - 2.42.A                                       *
*                          PA - 2.42.A                                       *
*                                                                            *
* DATE                     14. Aug. 2013                                     *
*                                                                            *
*****************************************************************************/


/* ===========================================================================

FILE_NAME          vfd.h



DESCRIPTION        VFD & Object Descriptions definitions

=========================================================================== */
#ifndef __VFD_H__
#define __VFD_H__

/*****************************************************************************
*  This file defines the structure of object descritions of all types of
*  objects. These descriptions are used to create the VFDs and the Object
*  Dictionary.
*
*  These definitions are used by the Network Management, the FB Application
*  and the FB Shell.
*
*****************************************************************************/

/* --- Configure VFDs ----------------------------------------------------- */


#define MAX_OBJECT_NAME_LENGTH       32

#define MAX_DS_MEMBERS               38     /* Trend has 37 members         */
#define MAX_VAR_LIST_MEMBERS         33     /* PID has 31 members in View_4 */
                                            /* MVC var list has max 32 members + Revision Number */

#define MAX_VFD_STRING_LENGTH        34

#define MAX_DATATYPE_NAME_LENGTH     12

#define  NM_EXTENSION_LENGTH          1

  /*@Dresser project modified: Since Dresser intends to create a lot (>50) of data structures, 
    the AP Directory index need to be moved.
    ATTENTION: the AP Directory needs to be the first object within the STATIC OD part */
  //#define S_OD_START              298
  #define S_OD_START              398
  #define S_OD_AP_DIR             S_OD_START

  #define MAX_NO_OF_VIEW_3         16
  #define MAX_NO_OF_VIEW_4         16

  #define OD_SECTION_UNUSED     0xFFFF


/* --- codes of FMS objects ------------------------------------------------ */

/* the obj_code attribute of object description has one of the following values */
#define  NULL_OBJECT                   0    /* null object                   */
#define  OD_OBJECT                     1    /* OD object                     */
#define  DOMAIN_OBJECT                 2    /* domain object                 */
#define  INVOCATION_OBJECT             3    /* program invocation object     */
#define  EVENT_OBJECT                  4    /* event object                  */
#define  TYPE_OBJECT                   5    /* basic data type description   */
#define  TYPE_STRUCT_OBJECT            6    /* structured data type descript.*/
#define  SIMPLE_VAR_OBJECT             7    /* simple variable object        */
#define  ARRAY_OBJECT                  8    /* array object                  */
#define  RECORD_OBJECT                 9    /* record object                 */
#define  VAR_LIST_OBJECT              10    /* variable list object          */

/* --- indices in OD type description for standard data types -------------- */

#define BOOLEAN                        1
#define INTEGER8                       2
#define INTEGER16                      3
#define INTEGER32                      4
#define UNSIGNED8                      5
#define UNSIGNED16                     6
#define UNSIGNED32                     7
#define FLOATING_POINT                 8
#define VISIBLE_STRING                 9
#define OCTET_STRING                  10
#define DATE                          11
#define TIME_OF_DAY                   12
#define TIME_DIFFERENCE               13
#define BIT_STRING                    14
#define TIME_VALUE                    21

/* --- access rights ------------------------------------------------------- */

#ifdef BIG_ENDIAN
#define R_ALL                         0x01
#define W_ALL                         0x02
#define EVT_ALL                       0x06
#define DOM_DL_ALL                    0x02
#endif
#ifdef LITTLE_ENDIAN
#define R_ALL                         0x0100
#define W_ALL                         0x0200
#define EVT_ALL                       0x0600
#define DOM_DL_ALL                    0x0200
#endif
#define RW_ALL                       (USIGN16) (R_ALL|W_ALL)

/* --- error codes --------------------------------------------------------- */

#define E_OD_OVERRUN                  0x06
#define E_OD_NULL_OBJECT              0x07
#define E_OD_NO_OBJECT                0x08

/* ------------------------------------------------------------------------- */

typedef struct
{
  USIGN8     pass_word;                 /* password                          */
  USIGN8     acc_groups;                /* access groups                     */
  USIGN16    acc_right;                 /* access rights                     */
} ACCESS_S;



typedef void *          OD_ADDR;

#define OD_NULL_ADDR    (NULL)
#define OD_NO_LOC_ADDR  (void *)0xFFFFFFFF
#define OD_SET_ADDR(a)  ((void *) &(a))
#define OD_GET_ADDR(a)  (a)


/* --- OD-Object-Description -------------------------------------------------*/
typedef struct _OD_HDR
{
   USIGN8      obj_code;          /* object-code = 1                             */
   BOOL        flag;              /* => TRUE  write protected                    */
   USIGN8      length;            /* size of names (0-32)                        */
   BOOL        protection;        /* access protection supported                 */
   USIGN16     version;           /* version                                     */
   USIGN16     len_st_od;         /* length of the static type description       */
   USIGN16     first_index_s_od;  /* start index of the static object descrip.   */
   USIGN16     len_s_od;          /* length of the static object descrip.        */
   USIGN16     first_index_dv_od; /* start index of the dyn. var. list descrip.  */
   USIGN16     len_dv_od;         /* length of the  dyn. variable list descrip.  */
   USIGN16     first_index_dp_od; /* start index of the dyn. pi description      */
   USIGN16     len_dp_od;         /* length of the dyn. pi description           */

}  OD_HDR;

typedef struct _OD_HDR_SHORT
{
   BOOL        flag;              /* => TRUE  write protected                    */
   USIGN8      length;            /* size of names (0-32)                        */
   BOOL        protection;        /* access protection supported                 */
   USIGN16     version;           /* version                                     */
   USIGN16     len_st_od;         /* length of the static type description       */
   USIGN16     first_index_s_od;  /* start index of the static object descrip.   */
   USIGN16     len_s_od;          /* length of the static object descrip.        */
   USIGN16     first_index_dv_od; /* start index of the dyn. var. list descrip.  */
   USIGN16     len_dv_od;         /* length of the  dyn. variable list descrip.  */
   USIGN16     first_index_dp_od; /* start index of the dyn. pi description      */
   USIGN16     len_dp_od;         /* length of the dyn. pi description           */
} OD_HDR_SHORT;

/* --- Null-Object-Description -----------------------------------------------*/
typedef struct
{
   USIGN8      obj_code;                          /* object code                */

} NULL_OBJ_DESC;


typedef struct
{
   USIGN8      obj_code;                          /* object code                */

} OBJ_DESC;

/* --- Static-Type-Data-Type-Object-Description -----------------------------*/

typedef struct _ST_DT_DESCR
{
   USIGN8      obj_code;                           /* object code               */
   STRINGV     meaning[MAX_DATATYPE_NAME_LENGTH];  /* meaning of the type       */

}  ST_DT_DESCR;

/* --- Static-Type-Data-Type-Structure-Object-Description -------------------*/
typedef struct _DT_LIST
{
   USIGN16      index_of_type;               /* logical address of the type     */
   USIGN8       length;                      /* length of the element in octets */

}  DT_LIST;

typedef struct _ST_DS_DESCR
{
   USIGN8        obj_code;                /* object code                     */
   USIGN8        no_of_elements;          /* number of record elements       */
   DT_LIST       dt_list[MAX_DS_MEMBERS]; /* data type list                  */

}  ST_DS_DESCR;

/*--- Static-Simple-Variable-Object-Description -----------------------------------*/
typedef struct _SIMPLE_VAR_OBJ_DESC
{
   USIGN8        obj_code;                          /* object code                    */
   USIGN16       index_of_type;                     /* logical address of the type    */
   USIGN8        length;                            /* length of object in octets     */
   ACCESS_S      access;                            /* access right structure         */
   STRINGV       name[MAX_OBJECT_NAME_LENGTH];      /* name                           */
   USIGN8        e_length;                          /* length of extension            */
   INT8          e_usage;                           /* usage                          */
   USIGN32       dd_name_id;                        /*                                */
   USIGN32       dd_item_id;                        /*                                */

}  SIMPLE_VAR_OBJ_DESC;

/*--- Static-Simple-Variable-Object-Description -----------------------------------*/
typedef struct _NM_SIMPLE_VAR_OBJ_DESC
{
   USIGN8        obj_code;                          /* object code                    */
   USIGN16       index_of_type;                     /* logical address of the type    */
   USIGN8        length;                            /* length of object in octets     */
   ACCESS_S      access;                            /* access right structure         */
   OD_ADDR       local_address;                     /* local address                  */
   USIGN8        extension[NM_EXTENSION_LENGTH];   /* extension                      */

}  NM_SIMPLE_VAR_OBJ_DESC;

/*--- Static-Array-Variable-Object-Description ------------------------------------*/
typedef struct _ARRAY_OBJ_DESC
{
   USIGN8        obj_code;                          /* object code                    */
   USIGN16       index_of_type;                     /* logical address of the type    */
   USIGN8        length;                            /* length of an element in octets */
   USIGN8        no_of_element;                     /* number of array elements       */
   ACCESS_S      access;                            /* access right structure         */
   STRINGV       name[MAX_OBJECT_NAME_LENGTH];      /* name                           */
   USIGN8        e_length;                          /* length of extension            */
   INT8          e_usage;                           /* usage                          */
   USIGN32       dd_name_id;                        /*                                */
   USIGN32       dd_item_id;                        /*                                */

}  ARRAY_OBJ_DESC;

/*--- Static-Array-Variable-Object-Description ------------------------------------*/
typedef struct _NM_ARRAY_OBJ_DESC
{
   USIGN8        obj_code;                          /* object code                    */
   USIGN16       index_of_type;                     /* logical address of the type    */
   USIGN8        length;                            /* length of an element in octets */
   USIGN8        no_of_element;                      /* number of array elements       */
   ACCESS_S      access;                            /* access right structure         */
   OD_ADDR       local_address;                     /* local address                  */
   USIGN8        extension[NM_EXTENSION_LENGTH];   /* extension                      */

}  NM_ARRAY_OBJ_DESC;

/*--- Static-Record-Variable-Object-Description -----------------------------------*/
typedef struct _RECORD_OBJ_DESC
{
   USIGN8         obj_code;                         /* object code                    */
   USIGN16        index_of_type;                    /* logical address of the type    */
   ACCESS_S       access;                           /* access right structure         */
   STRINGV        name[MAX_OBJECT_NAME_LENGTH];     /* name                           */
   USIGN8         e_length;                         /* length of extension            */
   INT8           e_usage     ;                     /* type of storage                */
   USIGN32        dd_name_id;                       /*                                */
   USIGN32        dd_item_id;                       /*                                */

}  RECORD_OBJ_DESC;

/*--- Static-Record-Variable-Object-Description -----------------------------------*/
typedef struct _NM_RECORD_OBJ_DESC
{
   USIGN8         obj_code;                         /* object code                    */
   USIGN16        index_of_type;                    /* logical address of the type    */
   ACCESS_S       access;                           /* access right structure         */
   USIGN8         extension[NM_EXTENSION_LENGTH];  /* extension                      */
   OD_ADDR        local_address;              /* local address: No list supported */

}  NM_RECORD_OBJ_DESC;


/*--- Domain-Object-Description ---------------------------------------------------*/
typedef struct _DOMAIN_OBJ_DESC
{
   USIGN8         obj_code;                         /* object code                    */
   USIGN16        max_octets;                       /* max octets                     */
   ACCESS_S       access;                           /* access right structure         */
   USIGN8         state;                            /* domain state                   */
   USIGN8         upl_state;                        /* upload state                   */
   USIGN8         counter;                          /* counter                        */
   STRINGV        name[MAX_OBJECT_NAME_LENGTH];     /* name                           */
   USIGN8         e_length;                         /* length of extension            */
   INT8           e_usage;                          /* usage                          */
   USIGN32        dd_name_id;                       /*                                */
   USIGN32        dd_item_id;                       /*                                */

}  DOMAIN_OBJ_DESC;

/*--- Domain-Object-Description ---------------------------------------------------*/
typedef struct _NM_DOMAIN_OBJ_DESC
{
   USIGN8         obj_code;                         /* object code                    */
   USIGN16        max_octets;                       /* max octets                     */
   ACCESS_S       access;                           /* access right structure         */
   OD_ADDR        local_address;                    /* local address: No list supported */
   USIGN8         state;                            /* domain state                   */
   USIGN8         upl_state;                        /* upload state                   */
   USIGN8         counter;                          /* counter                        */
   USIGN8         extension[NM_EXTENSION_LENGTH];  /* extension                      */

}  NM_DOMAIN_OBJ_DESC;


/*--- Dynamic-Variable-List-Object-Description ------------------------------------*/
typedef struct
{
   USIGN8         obj_code;                          /* object code                   */
   USIGN8         no_of_var;                         /* number of variables           */
   ACCESS_S       access;                            /* access right                  */
   BOOL           deletable;                         /* TRUE - deletable; else FALSE  */
   STRINGV        name[MAX_OBJECT_NAME_LENGTH];      /* name                          */
   USIGN8         e_length;                          /* length of extension           */
   INT8           e_usage;                           /* usage                         */
   USIGN32        dd_name_id;                        /*                               */
   USIGN32        dd_item_id;                        /*                               */

}  VAR_LIST_HDR;


typedef struct _VAR_LIST_OBJ_DESC
{
   USIGN8         obj_code;                          /* object code                   */
   USIGN8         no_of_var;                         /* number of variables           */
   ACCESS_S       access;                            /* access right                  */
   BOOL           deletable;                         /* TRUE - deletable; else FALSE  */
   STRINGV        name[MAX_OBJECT_NAME_LENGTH];      /* name                          */
   USIGN8         e_length;                          /* length of extension           */
   INT8           e_usage;                           /* usage                         */
   USIGN32        dd_name_id;                        /*                               */
   USIGN32        dd_item_id;                        /*                               */
   USIGN16        var_list[MAX_VAR_LIST_MEMBERS];    /* list of variables             */

}  VAR_LIST_OBJ_DESC;





/*--- Static-Event-Object-Description ---------------------------------------*/

typedef struct _EVENT_OBJ_DESC
{
  USIGN8         obj_code;                           /* object code             */
  USIGN16        index_event_data;                   /* index of event-data     */
  USIGN8         data_length;                        /* size of event data      */
  ACCESS_S       access;                             /* access protection       */
  BOOL           enabled;                            /* =>TRUE event is enabled */
  STRINGV        name[MAX_OBJECT_NAME_LENGTH];       /* symbolic name           */
  USIGN8         e_length;                           /* length of extension     */
  INT8           e_usage;                            /* usage                   */
  USIGN32        dd_name_id;                         /*                         */
  USIGN32        dd_item_id;                         /*                         */

} EVENT_OBJ_DESC;

typedef struct _EVENT_OBJ_DESC_SHORT
{
  USIGN16        index_event_data;                   /* index of event-data     */
  USIGN8         data_length;                        /* size of event data      */
  BOOL           enabled;                            /* =>TRUE event is enabled */

} EVENT_OBJ_DESC_SHORT;

/* --- COMMON-OBJECT-DESCRIPTION ----------------------------------------------- */
typedef union
{
   OD_HDR               od_obj_descr;
   OBJ_DESC             obj_descr;
   NULL_OBJ_DESC        null_obj_descr;
   ST_DT_DESCR          dt_obj_descr;
   ST_DS_DESCR          ds_obj_descr;
   SIMPLE_VAR_OBJ_DESC  s_var_obj_descr;
   ARRAY_OBJ_DESC       a_var_obj_descr;
   RECORD_OBJ_DESC      r_var_obj_descr;
   VAR_LIST_OBJ_DESC    vlist_obj_descr;
   EVENT_OBJ_DESC       evn_obj_descr;
   DOMAIN_OBJ_DESC      dom_obj_descr;

}  T_OBJ_DESC;

/* --- COMMON-OBJECT-DESCRIPTION ----------------------------------------------- */
typedef union
{
   OD_HDR                   od_obj_descr;
   NULL_OBJ_DESC            null_obj_descr;
   ST_DT_DESCR              dt_obj_descr;
   ST_DS_DESCR              ds_obj_descr;
   NM_SIMPLE_VAR_OBJ_DESC   s_var_obj_descr;
   NM_ARRAY_OBJ_DESC        a_var_obj_descr;
   NM_RECORD_OBJ_DESC       r_var_obj_descr;
   NM_DOMAIN_OBJ_DESC       dom_obj_descr;

}  NM_COMMON_ODESC;

/* --- COMMON-POINTER-TO-OBJECT-DESCRIPTION --------------------------------- */
typedef  union
{
   OD_HDR *               od_obj_descr;
   NULL_OBJ_DESC *        null_obj_descr;
   ST_DT_DESCR *          dt_obj_descr;
   ST_DS_DESCR *          ds_obj_descr;
   SIMPLE_VAR_OBJ_DESC *  s_var_obj_descr;
   ARRAY_OBJ_DESC *       a_var_obj_descr;
   RECORD_OBJ_DESC *      r_var_obj_descr;
   VAR_LIST_OBJ_DESC *    vlist_obj_descr;
   EVENT_OBJ_DESC *       evn_obj_descr;
   DOMAIN_OBJ_DESC *      dom_obj_descr;

} A_OF_ODESC;

/* --- COMMON-POINTER-TO-OBJECT-DESCRIPTION --------------------------------- */
typedef  union
{
   OD_HDR *                  od_obj_descr;
   NULL_OBJ_DESC *           null_obj_descr;
   ST_DT_DESCR *             dt_obj_descr;
   ST_DS_DESCR *             ds_obj_descr;
   NM_SIMPLE_VAR_OBJ_DESC *  s_var_obj_descr;
   NM_ARRAY_OBJ_DESC *       a_var_obj_descr;
   NM_RECORD_OBJ_DESC *      r_var_obj_descr;
   NM_DOMAIN_OBJ_DESC *      dom_obj_descr;

} NM_A_OF_ODESC;



  typedef union
  {
    OBJ_DESC             obj_descr;
    SIMPLE_VAR_OBJ_DESC  s_var_obj_descr;
    ARRAY_OBJ_DESC       a_var_obj_descr;
    RECORD_OBJ_DESC      r_var_obj_descr;
  }
    T_BLK_OBJ_DESC;


  typedef struct
  {
    USIGN16                         no_of_fix_par;
    const T_BLK_OBJ_DESC * const *  list_of_fix_par;
  }
    T_OD_BLK_DESC;


  typedef struct
  {
    USIGN8                no_of_float_trend;
    USIGN8                no_of_disc_trend;
    USIGN8                no_of_bitstr_trend;
  }
    T_OD_TREND_DESC;


  typedef struct
  {
    USIGN8                no_of_view_3;
    USIGN8                no_of_view_4;
    VAR_LIST_OBJ_DESC *   p_view_1;
    VAR_LIST_OBJ_DESC *   p_view_2;
    VAR_LIST_OBJ_DESC *   view_3_array[MAX_NO_OF_VIEW_3];
    VAR_LIST_OBJ_DESC *   view_4_array[MAX_NO_OF_VIEW_4];
  }
    T_OD_VIEW_DESC;


  typedef struct
  {
    VAR_LIST_OBJ_DESC *   p_mvc_list;
  }
    T_OD_MVC_LIST_DESC;


  typedef struct
  {
    USIGN16         start_index;
    USIGN16         end_index;
    T_OBJ_DESC *    (* a_of_get_od) (USIGN8, USIGN16);
  }
    T_OD_SECTION;


  typedef struct
  {
    USIGN16         start_index;
    USIGN16         end_index;
    USIGN16         no_of_sections;
    T_OD_SECTION *  od_section;

  } T_OD_PART;


#ifdef UNPACKED_ACCESS
typedef struct
{
   USIGN8       offset;
   USIGN8       size;

} T_FB_DATA_DESCR;


typedef struct
{
   USIGN16             n_elems;
   USIGN16             struct_size;
   T_FB_DATA_DESCR     descr[MAX_DS_MEMBERS];

} T_FB_DATA_STRUCTURES;
#endif /* UNPACKED_ACCESS */

/* ------   VFD   ------------------------------------------------------------*/

typedef  struct _VFD_STATUS
{
   USIGN8   phy_status;          /* Physical status of VFD     */
   USIGN8   local_detail[3];     /* Local detail of VFD status */

}  VFD_STATUS;

typedef struct _FB_VFD
{

   USIGN32     vfd_ref;                         /* VFD reference number     */
   STRINGV     vfd_tag[MAX_VFD_STRING_LENGTH];  /* length, VFD TAG, \0      */
   STRINGV     v_name[MAX_VFD_STRING_LENGTH];   /* length, Vendor name, \0  */
   STRINGV     m_name[MAX_VFD_STRING_LENGTH];   /* length, Model name, \0   */
   STRINGV     revision[MAX_VFD_STRING_LENGTH]; /* length, Revision, \0     */
   USIGN8      profile[2];                      /* Profile number           */

}  FB_VFD;

typedef struct _NM_FB_VFD
{
   NM_A_OF_ODESC  * obj_dictionary; /* OD header and list of data types     */
   NM_A_OF_ODESC  * obj_st_ds;      /* List of data structures              */
   NM_A_OF_ODESC  * obj_static_od;  /* List of static objects descriptions  */
#ifdef SW_DOWNLOAD
   NM_A_OF_ODESC  * obj_swdl_od;    /* List of static SWDL object descriptions */
#endif /* SW_DOWNLOAD */

   USIGN8         no_of_dt;         /* Number of OD data types + OD header  */
   USIGN8         no_of_ds;         /* Number of data structures in OD      */
   USIGN16        no_of_s_object;   /* Number of static objects             */
#ifdef SW_DOWNLOAD
   USIGN8         no_of_swdl_obj;   /* Number of SWDL objects               */
#endif /* SW_DOWNLOAD */

}  NM_FB_VFD;

#endif /* __VFD_H__ */
