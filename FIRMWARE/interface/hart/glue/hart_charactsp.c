/**
    \file hart_charactsp.c
    \brief HART glue layer for setpoint characterization

    CPU: Any

    OWNER: AK

    \ingroup HARTapp
*/
#include "mnwrap.h"
#include "numutils.h"
#include "hartfunc.h"
#include "hartcmd.h"
#include "hartdef.h"
#include "ctllimits.h"
#include "devicemode.h"
#include "bufutils.h"
#include "poscharact.h"
#include "digitalsp.h"
#include "ipcdefs.h"
#include "crc.h"

#define CHR_ASSERT(a) CONST_ASSERT((a) == (s32)HART_SP_##a) //lint !e961 (19.13 ## used)
CHR_ASSERT(CHARACT_LINEAR);
CHR_ASSERT(CHARACT_EQUAL_30);
CHR_ASSERT(CHARACT_EQUAL_50);
CHR_ASSERT(CHARACT_QUICK_OPEN);
CHR_ASSERT(CHARACT_CUSTOM);
CHR_ASSERT(CHARACT_CAMFLEX_EQ30);

//Write characterization
s8_least hartcmd_SelectSPCharacterization(const u8 *src, u8 *dst)
{
    const Req_SelectSPCharacterization_t *s = (const void *)src;
    Rsp_SelectSPCharacterization_t *d = (void *)dst;
    UNUSED_OK(d->SPCharacterization); //rely on HART framework to fill
    if(oswrap_IsContext(TASKID_IPCCOMM))
    {
        /* Per TFS:15789, Do not allow change in TB MAN because
        MAN setpoint is characterized and writing the same value
        will cause valve movement not expected by user
        */
        if(digsp_GetExternalMode() != IPC_MODE_OOS)
        {
            return HART_ACCESS_RESTRICTED; //same as HART_WRONG_MODE
        }
    }
    return err2hart(poscharact_Select(util_GetU8(s->SPCharacterization[0])));
}

//Read characterization
s8_least hartcmd_ReadSPCharacterizationSelection(const u8 *src, u8 *dst)
{
    UNUSED_OK(src);
    Rsp_ReadSPCharacterizationSelection_t *d = (void *)dst;
    util_PutU8(d->SPCharacterization[0], poscharact_GetCharacterizationSel(NULL)->CharactSel);
    const PosCharact_t *p = poscharact_GetCustomTable(NULL);
    util_PutU8(d->NumCustomCharactSegments[0], p->NumSeg);
    //Compute CRC without fixed endpoints - that's the magic number
    util_PutU16(d->CustomCharactMagic[0], hCrc16(CRC_SEED, p->point+1, (NELEM(p->point)-2U)*NELEM(p->point->x)));
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

/**
\brief Returns the custom configuration data

Notes:
This command is allowed in all modes, even when write busy or process busy is set
*/
s8_least hartcmd_ReadSPCharacterization(const u8 *src, u8 *dst)
{
    const Req_ReadSPCharacterization_t *s = (const void *)src;
    u8 charact = util_GetU8(s->SPCharacterizationIndex[0]);

    Rsp_ReadSPCharacterization_t *d1 = (void *)dst;
    UNUSED_OK(d1->SPCharacterizationIndex); //rely on HART framework to fill
    CharacterizationPoint_t *d = (void *)d1->CharacterizationPoint[0];

    u8_least i;
    PosCharact_t tab;
    const PosCharact_t *result = poscharact_GetTable(charact, &tab);
    if(result == NULL)
    {
        return HART_INVALID_DATA;
    }

    CONST_ASSERT(NELEM(d1->CharacterizationPoint) == (NELEM(result->point)-2U)); //Account for assumed endpoints
    //Omit fixed endponts
    for(i=0; i<NELEM(d1->CharacterizationPoint); i++)
    {
        util_PutS16(d[i].CharactXpoint[0], tab.point[i+1].x[Xlow]);
        util_PutS16(d[i].CharactYpoint[0], tab.point[i+1].x[Xhi]);
    }

    util_PutU8(d1->NumCharactPoints[0], tab.NumSeg - 1);
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}

/**
\brief Returns the custom configuration data

Notes:
This command is allowed in all modes, even when write busy or process busy is set
*/
s8_least hartcmd_ReadSPCharacterizationXorY(const u8 *src, u8 *dst)
{
    const Req_ReadSPCharacterizationXorY_t *s = (const void *)src;
    u8 charact = util_GetU8(s->SPCharacterizationIndex[0]);

    Rsp_ReadSPCharacterizationXorY_t *d = (void *)dst;
    UNUSED_OK(d->SPCharacterizationIndex); //rely on HART framework to fill
    UNUSED_OK(d->XorY); //rely on HART framework to fill

    u8_least i;
    PosCharact_t tab;
    const PosCharact_t *result = poscharact_GetTable(charact, &tab);
    if(result == NULL)
    {
        return HART_INVALID_DATA;
    }
    u8 XorY=util_GetU8(s->XorY[0]);
    if(XorY > 1)
    {
        return HART_INVALID_DATA;
    }

    CONST_ASSERT(NELEM(d->CharactPoint) == (NELEM(result->point)-2U)); //Account for assumed endpoints
    //Omit fixed endponts
    for(i=0; i<NELEM(d->CharactPoint); i++)
    {
        if(XorY == 0)
        {
            util_PutS16(d->CharactPoint[i], tab.point[i+1].x[Xlow]);
        }
        else
        {
            util_PutS16(d->CharactPoint[i], tab.point[i+1].x[Xhi]);
        }
    }

    util_PutU8(d->NumCharactPoints[0], tab.NumSeg - 1);
    return HART_NO_COMMAND_SPECIFIC_ERRORS;
}


/**
\brief Writes custom characterization; models as a COMMAND command to save on response bytes xfer
*/
s8_least hartcmd_WriteCustomCharacterization(const u8 *src, u8 *dst)
{
    const Req_WriteCustomCharacterization_t *s1 = (const void *)src;
    const CharacterizationPoint_t *s = (const void *)s1->CharacterizationPoint[0];
    UNUSED_OK(dst);

    u8_least i;
    PosCharact_t tab;
    const PosCharact_t *result = poscharact_GetTable(CHARACT_CUSTOM, &tab);
    if(result == NULL)
    {
        return HART_INVALID_DATA;
    }

    tab.NumSeg = util_GetU8(s1->NumCharactPoints[0]) + 1; //Internal point only in the HART command
    if(tab.NumSeg >= NELEM(tab.point))
    {
        return HART_INVALID_DATA;
    }
    //prepare fixed first values
    tab.point[0].x[Xlow] = INT_PERCENT_OF_RANGE(0.0);
    tab.point[0].x[Xhi] = INT_PERCENT_OF_RANGE(0.0);

    //fill internal points from the HART buffer
    for(i=1; i<tab.NumSeg; i++)
    {
        tab.point[i].x[Xlow] = util_GetS16(s[i-1].CharactXpoint[0]);
        tab.point[i].x[Xhi] = util_GetS16(s[i-1].CharactYpoint[0]);
    }
    /* prepare fixed last values: fill the whole table to minimize IPC transfer
    if present (CRC would match). It is not needed but harmless in projects
    without IPC
    */
    for(; i<NELEM(tab.point); i++)
    {
        tab.point[i].x[Xlow] = INT_PERCENT_OF_RANGE(100.0);
        tab.point[i].x[Xhi] = INT_PERCENT_OF_RANGE(100.0);
    }

    ErrorCode_t err = poscharact_SetCustomTable(&tab);
    return err2hart(err);
}

#if 0
//for now, an empty stub
s8_least hartcmd_CommitCustomCharacterization(const u8 *src, u8 *dst)
{
    UNUSED_OK(src);
    UNUSED_OK(dst);
    return COMMAND_NOT_IMPLEMENTED;
}
#endif

/* This line marks the end of the source */
