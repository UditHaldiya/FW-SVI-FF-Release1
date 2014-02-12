/*
Copyright 2012 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file mncharactff.c
    \brief Handling of characterization interface

    CPU: Any

    OWNER: AK
*/
/* $History: mncharactff.c $
 *
 * *****************  Version 1  *****************
 * User: victoria huang    Date: 03/13/13    Time: 15:17p
 * Char_Selection parameter write
*/

//Softing headers first
#include <softing_base.h>
#include <ptb.h>

#define MODULE_ID (MOD_APPL_TRN | COMP_PAPP)

//MN FIRMWARE headers second
#include "hartfunc.h"
#include "hartdef.h"
#include "bufutils.h"
#include "poscharact.h"
#include "crc.h"
#include "percent.h"

//Glue headers last
#include "mncharactff.h"

#define INITIALIZE_WITH_CURRENT 7

//-----------------------------------------------
static const PosCharact_t * const PosCharactTab[]= //! Central access point to predefined characterizations
{
    [CHARACT_LINEAR] = &charactFlatTable_LINEAR,
    [CHARACT_EQUAL_30] = &charactFlatTable_EQUAL_30,
    [CHARACT_EQUAL_50] = &charactFlatTable_EQUAL_50,
    [CHARACT_QUICK_OPEN] = &charactFlatTable_QUICK_OPEN,
    [CHARACT_CUSTOM] = NULL,
    [CHARACT_CAMFLEX_EQ30] = &charactFlatTable_CAMFLEX_EQ30,
};


extern T_FBIF_PTB dummy; //A dummy for size calculations by the compiler
static s16 cache_points[NELEM(dummy.custom_char_points)];
static bool_t cache_valid = false;

//A helper
static void cache_custom_points(const s16 *points)
{
    if(!cache_valid)
    {
        mn_memcpy(cache_points, points, sizeof(cache_points));
        cache_valid = true;
    }
}

/** \brief A helper to write characterization curve
\param p_block_instance - a pointer to TB instance
\param save_points - a pointer to the curve
\param idx - a block-relative index of the array variable (dangerous if wrong)
*/
static fferr_t write_curve_local(T_FBIF_BLOCK_INSTANCE *p_block_instance, s16 *save_points, u8 idx)
{
    T_FBS_WRITE_PARAM_LOCAL lwrite;
    lwrite.rel_idx = idx;
    lwrite.subindex = 0;
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;
    UNUSED_OK(p_PTB); //defined for size calc
    lwrite.length   = sizeof(p_PTB->current_char_points);
    lwrite.source = (u8 *)save_points;
    return fbs_write_param_loc(p_block_instance, &lwrite);
}

typedef struct charact_data_t
{
    u8 char_selection;
    u8 numseg;
    u16 crc;
} charact_data_t;

/** \brief A helper to retrieve the actual charact type and custom stats
*/
static fferr_t read_charact_type(charact_data_t *rsp, void *from, void *to)
{
    fferr_t fferr;

    //Populate the send buffer
    Req_ReadSettings_t *subcmd = from;
    util_PutU8(subcmd->SubCmdNum[0], 179);
    Rsp_ReadSPCharacterizationSelection_t *s = (void *)((u8 *)to + MN_OFFSETOF(Rsp_ReadSettings_t, endmark) + 2);

    u8 len = MN_OFFSETOF(Req_ReadSettings_t, endmark) + MN_OFFSETOF(Req_ReadSPCharacterizationSelection_t, endmark);
    fferr = mn_HART_acyc_cmd(170, from, len, to);

    //In case of an error, will fill with garbage. Anyone cares?
    rsp->char_selection = util_GetU8(s->SPCharacterization[0]);
    rsp->numseg = util_GetU8(s->NumCustomCharactSegments[0]);
    rsp->crc = util_GetU16(s->CustomCharactMagic[0]);

    return fferr;
}

/** \brief Restores TB cache of custom points from APP
*/
#if 0
static fferr_t charact_Restore(T_FBIF_BLOCK_INSTANCE *p_block_instance, void *from, void *to)
{
    fferr_t fferr;

    //Populate the send buffer
    Req_ReadSettings_t *subcmd = from;
    util_PutU8(subcmd->SubCmdNum[0], 178);
    Req_ReadSPCharacterization_t *t = (void *)((u8 *)from + MN_OFFSETOF(Req_ReadSettings_t, endmark));
    Rsp_ReadSPCharacterization_t *s = (void *)((u8 *)to + MN_OFFSETOF(Rsp_ReadSettings_t, endmark) + 2);
    util_PutU8(t->SPCharacterizationIndex[0], CHARACT_CUSTOM);

    u8 len = MN_OFFSETOF(Req_ReadSettings_t, endmark) + MN_OFFSETOF(Req_ReadSPCharacterization_t, endmark);
    fferr = mn_HART_acyc_cmd(170, from, len, to);
    if(fferr == E_OK)
    {
        //Cache data from the buffer
        CharacterizationPoint_t *p = (void *)s->CharacterizationPoint[0];
        for(u8_least i=0; i<NELEM(s->CharacterizationPoint); i++)
        {
            cache_points[2*i] = util_GetS16(p[i].CharactXpoint[0]);
            cache_points[2*i+1] = util_GetS16(p[i].CharactYpoint[0]);
        }
        //And write them to the TB
        (void)write_curve_local(p_block_instance, cache_points, REL_IDX_PTB_CURRENT_CHAR_POINTS); //Ignore the error, if any
        cache_valid = true;
    }
    return fferr;
}
#else
static fferr_t charact_Restore(T_FBIF_BLOCK_INSTANCE *p_block_instance, void *from, void *to)
{
    fferr_t fferr;

    //Populate the send buffer
    Req_ReadSettings_t *subcmd = from;
    util_PutU8(subcmd->SubCmdNum[0], 177);
    Req_ReadSPCharacterizationXorY_t *t = (void *)((u8 *)from + MN_OFFSETOF(Req_ReadSettings_t, endmark));
    Rsp_ReadSPCharacterizationXorY_t *s = (void *)((u8 *)to + MN_OFFSETOF(Rsp_ReadSettings_t, endmark) + 2);
    util_PutU8(t->SPCharacterizationIndex[0], CHARACT_CUSTOM);
    CONST_ASSERT(NELEM(dummy.custom_char_points)==NELEM(s->CharactPoint)*(1U+1U)); //for X and Y

    u8 len = MN_OFFSETOF(Req_ReadSettings_t, endmark) + MN_OFFSETOF(Req_ReadSPCharacterizationXorY_t, endmark);
    util_PutU8(t->XorY[0], 0); //X first
    fferr = mn_HART_acyc_cmd(170, from, len, to);
    if(fferr == E_OK)
    {
        //Cache data from the buffer
        for(u8_least i=0; i<NELEM(s->CharactPoint); i++)
        {
            cache_points[2*i] = util_GetS16(s->CharactPoint[i]);
        }

        util_PutU8(t->XorY[0], 1); //Y second
        fferr = mn_HART_acyc_cmd(170, from, len, to);
    }
    if(fferr == E_OK)
    {
        //Cache data from the buffer
        for(u8_least i=0; i<NELEM(s->CharactPoint); i++)
        {
            cache_points[2*i+1] = util_GetS16(s->CharactPoint[i]);
        }
        //And write them to the TB
        (void)write_curve_local(p_block_instance, cache_points, REL_IDX_PTB_CURRENT_CHAR_POINTS); //Ignore the error, if any
        cache_valid = true;
    }
    return fferr;
}
#endif

#define CHARACT_INVALID 255U

/** \brief Populates a flat array of characterization points, number of points and characterization selection
\param p_block_instance - a pointer to TB block instance
\param from - a pointer to HART xmit buffer
\param to - a pointer to HART rcv buffer
\param[out] points - a flat array to populate
\param[out] numpoints - number of points
\param[in][out] selection - characterization selection. If IN value is CHARACT_INVALID, populates current selection, otherwise the specified selection
\return an FF response code
*/
static fferr_t charact_ComputeFlatCharactArray(T_FBIF_BLOCK_INSTANCE *p_block_instance, void *from, void *to, s16 *points, u8 *numpoints, u8 *selection)
{
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;
    fferr_t fferr;

    charact_data_t rsp;
    fferr = read_charact_type(&rsp, from, to);
    if(fferr != E_OK)
    {
        return fferr;
    }

    u8 numseg;
    u8_least i;
    u8 sel = *selection;
    if(sel == CHARACT_INVALID)
    {
        sel = rsp.char_selection;
        *selection = sel;
    }

    cache_custom_points((s16*)p_PTB->current_char_points);
    if(sel == CHARACT_CUSTOM)
    {
        numseg = rsp.numseg;
        //NOTE: Here is the place to compare CRC and on mismatch repair the points.
        u16 crc = hCrc16(CRC_SEED, cache_points, NELEM(cache_points));
        if(crc != rsp.crc)
        {
            fferr = charact_Restore(p_block_instance, from, to);
        }
        if(fferr != E_OK)
        {
            return fferr;
        }
        mn_memcpy(points, cache_points, sizeof(cache_points));
    }
    else
    {
        numseg = PosCharactTab[sel]->NumSeg;
        //populate **internal** points from flash
        for(i=1; i<numseg; i++)
        {
            points[2*(i-1)] = PosCharactTab[sel]->point[i].x[Xlow];
            points[2*(i-1)+1] = PosCharactTab[sel]->point[i].x[Xhi];
        }
    }

    //Fill the remaining points with a dummy value per Vlad's request
    for(i=2*(numseg-1); i<NELEM(cache_points); i++)
    {
        points[i] = STANDARD_100;
    }

    *numpoints = numseg-1;
    return E_OK;
}

#define CHARACT_ACTIVATE_CUSTOM 255U

/** \brief Writes characterization selection

Order of operations:
                    1. Write characterization points locally. If fails, reject command
                    2. If custom characterization (is being activated)
                        write curve to APP; If fails, restore local points; reject command
                    3. Write selection locally. Do not bother to write # of points. If fails, accept command
                    4. Write selection to APP. If fails, restore local selection, accept command

1.	Write characterization points locally; If fails, reject command
2.	If custom characterization (is being activated):
    a.	write curve to APP; If fails, reject command
3.	Write selection locally. Do not bother to write # of points. If fails, reject command if not custom characterization
4.	If prev. step OK: Write selection to APP. If fails, restore local selection, accept command.

Read current curve:
If the curve is custom, compare CRC and number of points of the local and APP versions. If mismatch, read from APP to populate local.
*/
fferr_t ffcharact_WriteSelection(T_FBIF_BLOCK_INSTANCE *p_block_instance, T_FBIF_WRITE_DATA *p_write, void *from, void *to)
{
    UNUSED_OK(p_block_instance);
    fferr_t fferr = E_OK;
    u8 *source = p_write->source;
    u8 charact = source[0];
    if(p_write->subindex == 0)
    {
        p_write->subindex = 1; //the only writable field
    }
    if(p_write->subindex != 1)
    {
        //Attempt to write a RO parameter
        fferr = E_FB_DATA_NO_WRITE;
    }
    else
    {
        T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;
        s16 points[NELEM(cache_points)];
        u8 numpoints;
        if (charact == CHARACT_CUSTOM)
        {
            //compare CRC and number of points of the local and APP versions.
            // If mismatch, read from APP to populate local.
            fferr = charact_ComputeFlatCharactArray(p_block_instance, from, to, points,
                                             &numpoints, &charact);
        }

        Req_WriteSettings_t *subcmd = from;

        if(charact == CHARACT_ACTIVATE_CUSTOM)
        {
            charact = CHARACT_CUSTOM;
            source[0] = charact;
            /* Now we need to activate the curve points. They come from
            from custom charact scratchpad for custom
            */

            //Push custom curve to APP
            u8 num_custom_points = p_PTB->custom_char.component_1;
            //Populate the send buffer
            util_PutU8(subcmd->SubCmdNum[0], 178);
            Req_WriteCustomCharacterization_t *s = (void *)((u8 *)from + MN_OFFSETOF(Req_WriteSettings_t, endmark));
            util_PutU8(s->NumCharactPoints[0], num_custom_points);
            CharacterizationPoint_t *cp = (void *)(s->CharacterizationPoint[0]);
            for(u8_least i=0; i<NELEM(s->CharacterizationPoint); i++)
            {
                util_PutS16(cp[i].CharactXpoint[0], p_PTB->custom_char_points[2*i]);
                util_PutS16(cp[i].CharactYpoint[0], p_PTB->custom_char_points[2*i+1]);
            }
            u8 len = MN_OFFSETOF(Req_WriteSettings_t, endmark) + MN_OFFSETOF(Req_WriteCustomCharacterization_t, endmark);
            fferr = mn_HART_acyc_cmd(171, from, len, to);

            if(fferr == E_OK)
            {
                //successfully pushed the curve
                cache_valid = false;
                fferr_t e = write_curve_local(p_block_instance, (s16 *)p_PTB->custom_char_points, REL_IDX_PTB_CURRENT_CHAR_POINTS);
                UNUSED_OK(e); //for relase build
                _ASSERT(e==E_OK);
            }
        }

        //Now, write char type to APP
        if(fferr == E_OK)
        {
            //Populate the send buffer
            util_PutU8(subcmd->SubCmdNum[0], 179);
            Req_SelectSPCharacterization_t *s = (void *)((u8 *)from + MN_OFFSETOF(Req_WriteSettings_t, endmark));

            util_PutU8(s->SPCharacterization[0], charact);

            u8 len = MN_OFFSETOF(Req_WriteSettings_t, endmark) + MN_OFFSETOF(Req_SelectSPCharacterization_t, endmark);
            fferr = mn_HART_acyc_cmd(171, from, len, to);
        }

        //Now, write char type locally (for Vlad only; we won't be using it)
        if(fferr == E_OK)
        {
#if 0
            fferr =
#else
            (void)
#endif
                fbs_write_param(p_block_instance, p_write);
        }
    }
    return fferr;
}


/** \brief Reads characterization selection
*/
fferr_t ffcharact_ReadSelection(T_FBIF_BLOCK_INSTANCE *p_block_instance, T_FBIF_READ_DATA *p_read, void *from, void *to)
{
    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;
    UNUSED_OK(p_read);
    s16 points[NELEM(cache_points)];
    u8 numpoints;
    u8 selection = CHARACT_INVALID; //get current selection
    fferr_t fferr = charact_ComputeFlatCharactArray(p_block_instance, from, to, points, &numpoints, &selection);

    if(fferr != E_OK)
    {
        return fferr;
    }

    _CHAR_TYPE *chr = &p_PTB->char_selection;
    chr->component_0 = selection;
    chr->component_1 = numpoints;
    mn_memcpy(p_PTB->current_char_points, points, sizeof(p_PTB->current_char_points));
    return E_OK;
}

//-----------------------------------------------------

/** \brief Writes custom characterization into editable area of TB variables
*/
fferr_t ffcharact_WriteCustomCharacterization(T_FBIF_BLOCK_INSTANCE *p_block_instance, T_FBIF_WRITE_DATA *p_write, void *from, void *to)
{
    fferr_t fferr = E_OK;
    if(p_write->subindex == 0)
    {
        //replace the whole array
    }
    else
    {
        //replace a single element
        if(p_write->subindex > NELEM(dummy.custom_char_points)) //not >= since subindex is 1-based
        {
            fferr = E_FB_PARA_LIMIT;
        }
    }
    // ... whatever else ...

    if(fferr == E_OK)
    {
        fferr = fbs_write_param(p_block_instance, p_write);
    }

    UNUSED_OK(from);
    UNUSED_OK(to);
    return fferr;
}

/** \brief Writes custom characterization into editable area of TB variables
*/
fferr_t ffcharact_InitCustomCharacterization(T_FBIF_BLOCK_INSTANCE *p_block_instance, T_FBIF_WRITE_DATA *p_write, void *from, void *to)
{
    fferr_t fferr = E_OK;
    bool_t set_array = false;
    bool_t set_numpoints = false;

    T_FBIF_PTB *p_PTB = p_block_instance->p_block_desc->p_block;

    u8 numpoints = p_write->source[1];
    switch(p_write->subindex)
    {
    case 0: //all
        set_array = true;
        set_numpoints = true;
        break;
    case 1: //array init only
        set_array = true;
        break;
    case 2: //numpoints init only
        set_numpoints = true;
        numpoints = p_write->source[0];
        break;
    default:
        fferr = E_FB_PARA_CHECK;
        break;
    }

    if(fferr == E_OK)
    {
        if(set_numpoints)
        {
            if(numpoints > NELEM(dummy.custom_char_points))
            {
                fferr = E_FB_PARA_LIMIT;
            }
        }
    }

    u8 init_curve = p_write->source[0];
    if(fferr == E_OK)
    {
        if(set_array)
        {
            if(init_curve == 255)
            {
                //OK but nothing to do
            }
            else if(init_curve == INITIALIZE_WITH_CURRENT)
            {
                mn_memcpy(p_PTB->custom_char_points, p_PTB->current_char_points, sizeof(p_PTB->custom_char_points));
            }
            else if(init_curve >= NELEM(PosCharactTab))
            {
                fferr = E_FB_PARA_LIMIT;
            }
            else
            {
                //Initialize the curve from the cache of custom points
                s16 points[NELEM(cache_points)];
                u8 numpoints1; //default number of points
                fferr = charact_ComputeFlatCharactArray(p_block_instance, from, to, points, &numpoints1, &init_curve);
                if(fferr == E_OK)
                {
                    fferr = write_curve_local(p_block_instance, points, REL_IDX_PTB_CUSTOM_CHAR_POINTS);
                }
                if(!set_numpoints)
                {
                    p_write->source[1] = numpoints1;
                    p_write->subindex = 0;
                }
            }
        }
    }
    if(fferr == E_OK)
    {
        fferr = fbs_write_param(p_block_instance, p_write);
    }

    return fferr;
}


/** \brief Reads selected (current) characterization curve
*/
fferr_t ffcharact_ReadCharacterizationCurve(T_FBIF_BLOCK_INSTANCE *p_block_instance, T_FBIF_READ_DATA *p_read, void *from, void *to)
{
    return ffcharact_ReadSelection(p_block_instance, p_read, from, to);
}




/* This line marks the end of the source */
