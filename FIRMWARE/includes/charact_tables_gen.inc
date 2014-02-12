#MP If 0 ;Tombstone header
/**
Copyright 2004 by Dresser, Inc., as an unpublished work.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.

    \file charact_tables_gen.inc
    \brief Unimal macros to generate const characterization tables and
           related symbolic ids.

    CPU: Any

    OWNER: AK
    \n RCS: $Id: $
     $DateTime: $ $Change: $ $Author: Arkkhasin $
*/
#MP Endif ;Tombstone header
#MP
#MP Set point_pos = 16 ;point position in a 32-bit number
#MP
#MP ;---------------- mode setting macros ------------------
#MP Macro CharactMode_TablesOnly
#MP     Set start_pass = 0
#MP     Set end_pass = 1
#MP Endm
#MP
#MP
#MP Macro CharactMode_IdsOnly
#MP     Set start_pass = 2
#MP     Set end_pass = 2
#MP Endm
#MP
#MP
#MP Macro CharactMode_TablesAndIds
#MP     Set start_pass = 0
#MP     Set end_pass = 2
#MP Endm
#MP
#MP ;---------------- end mode setting macros ------------------
#MP Macro ComputeSlopeScale
#MP     Set SLOPE_SCALE_UP = 1
#MP     Expand LeftShift(SLOPE_SCALE_UP, point_pos)
#MP Endm
#MP
#MP
#MP Macro CharactTables
#MP     Expand ComputeSlopeScale()
#MP     For pass = start_pass, end_pass
#MP         Set table_num = 0
#MP Endm
#MP
#MP
#MP Macro EndCharactTables
#MP If 0
#MP         If pass == 0
#MP             ;Render the table of tables

#MP ;const u8 num_charactTables = (u8)#mp%utable_num;
const charactTable_t charactTable[] =
{
#MP         Endif
#MP         If pass == 1
#MP             ;Close the table of tables
};
#MP         Endif
#MP         If pass == 2
#MP             ;Render stats

//excluding Hall characterization, which is absolutely MUST be the LAST:
#MP             Set temp = table_num - 1
#define MAX_CHARACTERIZATION_ID #mp%dtemp

#define SLOPE_SCALE_UP #mp%uSLOPE_SCALE_UP
#MP         Endif
#MP     Endfor
#MP Endif ;0
#MP Endm
#MP
#MP
#MP Macro StartCharactTable ;(charact_name)
#MP     If 0 && (pass == 1) ;Global pass 1 - render the entry in the table of tables
#MP         If num_points_total%utable_num == 0 ;empty table?
    {
        0, //dummy entry
#MP ;later        charactTable_#mp%s#1#
        NULL
    },
#MP         Else ;empty table?
    {
        NELEM(charactTable_#mp%s#1#)-1u, //max segment #
        charactTable_#mp%s#1#
    },
#MP         Endif ;empty table?
#MP     Endif ;Global pass 1 - render the entry in the table of tables
#MP     If pass == 2 ;Global pass 2 - render the definition of the charact option
#define CHARACT_#mp%s#1# #mp%dtable_num
#MP     Endif ;Global pass 2 - render the definition of the charact option
#MP     If pass == 0 ;Global pass 0 - generate the table
#MP     For pass%utable_num = 0, 1 ;Only in global pass 0 do we scan the point definitions
#MP         Set point_count = 0
#MP         If pass%utable_num  == 0 ;pass 0 - collect data
#MP             Set num_points_total%utable_num = 0;
#MP         Endif ;pass 0 - collect data
#MP         If pass%utable_num  == 1; pass 1 - render the table
#MP             If !(num_points_total%utable_num == 0) ;empty table?
#MP                 ;The table is not empty: render the defintion
const stdXY_t charactTable_#mp%s#1#[] =
{
#MP             Endif ;empty table?
#MP         Endif ;pass 1 - render the table
#MP Endm
#MP
#MP
#MP Macro DefineCharactPoint ;(x, y)
#MP     If pass%utable_num == 0
#MP         Set point_x_%upoint_count = #1#
#MP         Set point_y_%upoint_count = #2#
#MP     Endif
#MP     If pass%utable_num == 1
#MP         Set temp = point_count + 1
#MP         Ifdef point_y_%utemp ;last point?
#MP             Set slope = 1+((point_y_%utemp - #2#)*SLOPE_SCALE_UP)/(point_x_%utemp - #1#) ;worst =-1
#MP            ; Set slope = (1+2*((point_y_%utemp - #2#)*SLOPE_SCALE_UP)/(point_x_%utemp - #1#))/2 ;worst = -+1
#MP             ;Set delta = (point_x_%utemp - #1#)/2
#MP             ;Set slope = (((point_y_%utemp - #2#)*SLOPE_SCALE_UP) + delta)/(point_x_%utemp - #1#)
#MP         Else ;last point?
#MP             ;We are at the last point; keep the previous slope for extrapolation
#MP             Ifdef slope
#MP                 ;OK: we have the previous segment
#MP             Else
#MP                 ;A single-point table has zero segments; treat it as const value
#MP                 Set slope = 0
#MP             Endif ;defined slope
#MP         Endif ;last point?
#MP         ;Render the struct
    {
        #mp%d#1#, #mp%d#2#, #mp%dslope
    },
#MP     Endif
#MP     Set point_count = point_count + 1
#MP
#MP Endm
#MP
#MP
#MP Macro EndCharactTable ;()
#MP         Set num_points_total%utable_num = point_count
#MP         If pass%utable_num  == 1
#MP             If !(num_points_total%utable_num == 0) ;empty table?
#MP                 ;The table is not empty: close the defintion
};
#MP             Endif ;empty table?
#MP         Endif ;pass 1
#MP     Endfor ;over pass%table_num
#MP     Endif ;Global pass 0 - generate the table
#MP     Set table_num = table_num + 1 ;count the tables
#MP Endm
#MP
#MP Macro LeftShift ;(dst, count)
#MP     For lloopp = 1, #2#
#MP         Set #1# = 2*#1#
#MP     Endfor
#MP Endm
#MP
#MP
#MP ;/* This line marks the end of the source */
