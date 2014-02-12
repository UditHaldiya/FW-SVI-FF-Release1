/*
Copyright 2004-2007 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/** \addtogroup mnhartcmd HART Command Module
\brief Dispatcher of HART commands

\htmlonly
<a href="../../../Doc/DesignDocs/Physical Design_HART Command Module.doc"> Design document </a><br>
<a href="../../inhouse/unit_test/Dbg/project_ESD/hclookup_ESDHTML.html"> Unit Test Report [1]</a><br>
<a href="../../inhouse/unit_test/Dbg/project_ESD/mnhartcmdHTML.html"> Unit Test Report [2]</a><br>
\endhtmlonly
*/
/**
    \file mnhartcmd.c
    \brief HART command dispatcher, a glue between data link layer and app layer

    [Originally based on MESCO's COMMAND.C]

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/ESD_Release_3.1.2/FWH6/FIRMWARE/interface/hart/mnhartcmd.c $
    $Date: 7/28/11 6:29p $
    $Revision: 55 $
    $Author: Arkkhasin $

    \ingroup mnhartcmd
*/
/* (Optional) $History: mnhartcmd.c $
 *
 * *****************  Version 55  *****************
 * User: Arkkhasin    Date: 7/28/11    Time: 6:29p
 * Updated in $/MNCB/Dev/ESD_Release_3.1.2/FWH6/FIRMWARE/interface/hart
 * LINT unused header
 *
 * *****************  Version 54  *****************
 * User: Arkkhasin    Date: 7/26/11    Time: 4:28p
 * Updated in $/MNCB/Dev/ESD_Release_3.1.2/FWH6/FIRMWARE/interface/hart
 * Port of AP HART 6 version
 *
 * *****************  Version 40  *****************
 * User: Arkkhasin    Date: 7/08/11    Time: 2:09p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/interface/hart
 * TFS:6957 Make HART dispatchers look for HC_MODIFY locks instead of
 * HC_WRITE_PROTECT
 *
 * *****************  Version 39  *****************
 * User: Arkkhasin    Date: 7/07/11    Time: 10:57p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/interface/hart
 * TFS:6936 Made HART 5 dispatcher aware of HART 6 lock in case of
 * fallback
 *
 * *****************  Version 38  *****************
 * User: Arkkhasin    Date: 5/28/11    Time: 12:37p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/interface/hart
 * TFS:6474 Addressing test hoisted to hart.c. The dispatcher reverted to
 * previous version
 *
 * *****************  Version 36  *****************
 * User: Arkkhasin    Date: 4/28/11    Time: 11:49a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/interface/hart
 * TFS:6186 Burst frame in HART 5 dispatcher was 2 bytes shorter than
 * intended
 *
 * *****************  Version 35  *****************
 * User: Arkkhasin    Date: 4/16/11    Time: 3:02p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/FIRMWARE/interface/hart
 * Based on LCX version
 *
 * *****************  Version 44  *****************
 * User: Arkkhasin    Date: 3/19/11    Time: 4:22p
 * Updated in $/MNCB/Dev/HART6Devel/FIRMWARE/interface/hart
 * Back to old prototypes for HART 5
 *
 * *****************  Version 43  *****************
 * User: Arkkhasin    Date: 3/18/11    Time: 9:52p
 * Updated in $/MNCB/Dev/HART6Devel/FIRMWARE/interface/hart
 * Removed hart_makeResponseFlags (now in mnhartcmdx.c)
 *
 * *****************  Version 42  *****************
 * User: Arkkhasin    Date: 3/16/11    Time: 9:56p
 * Updated in $/MNCB/Dev/HART6Devel/FIRMWARE/interface/hart
 * Again, the trunk version but now conditionally friendly to old code
 *
 * *****************  Version 40  *****************
 * User: Arkkhasin    Date: 2/18/11    Time: 4:00p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart
 * TFS:5661 - repaired a mess with subcommands length checking
 *
 * *****************  Version 39  *****************
 * User: Arkkhasin    Date: 10/06/10   Time: 10:06p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/interface/hart
 * TFS:4128 Dismantling io.{c,h}
 *
 * *****************  Version 38  *****************
 * User: Arkkhasin    Date: 3/30/10    Time: 6:40p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart
 * Constrained the use of nvramtypes.h
 *
 * *****************  Version 37  *****************
 * User: Arkkhasin    Date: 3/08/10    Time: 5:35p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart
 *         - return to original state as in the trunk
 *
 * *****************  Version 36  *****************
 * User: Arkkhasin    Date: 2/09/10    Time: 2:34p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/hart
 * Back port from the trunk to accommodate subcommands. Some interfaces
 * are retrofitted to avoid porting a bulk of new code
 *
 * *****************  Version 62  *****************
 * User: Arkkhasin    Date: 9/01/09    Time: 11:43a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * (Temporarily) reinstated special handling of command 11 - back port
 * from DLT 1.1.1 branch
 *
 * *****************  Version 63  *****************
 * User: Arkkhasin    Date: 8/18/09    Time: 2:58p
 * Updated in $/MNCB/Dev/DLT_Release_1.1.1/FIRMWARE/interface/hart
 * Bug 972 (response to a short cmd 11)  fixed by reintroducing an MNCB
 * wart until optional paramters are supported in our HART framework
 *
 * *****************  Version 61  *****************
 * User: Arkkhasin    Date: 3/23/09    Time: 7:12p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Project-specific status bits are now populated with a project-specific
 * plugin
 *
 * *****************  Version 60  *****************
 * User: Anatoly Podpaly Date: 3/20/09    Time: 6:45p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * FAULT_POSITION_ERROR renamed as FAULT_RANGE1.
 *
 * *****************  Version 59  *****************
 * User: Arkkhasin    Date: 1/11/09    Time: 7:46p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * include "hartapputils.h" (code migration)
 *
 * *****************  Version 58  *****************
 * User: Arkkhasin    Date: 9/11/08    Time: 6:39p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Fixed a couple of bugs introduced in the previous version
 *
 * *****************  Version 57  *****************
 * User: Arkkhasin    Date: 9/09/08    Time: 11:05a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * HART_NUM_STATUS_BYTES is no longer in the table; added in hart
 * dispatcher
 *
 * *****************  Version 56  *****************
 * User: Arkkhasin    Date: 4/07/08    Time: 1:16p
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Adapted to new API without changing the format of HART data
 *
 * *****************  Version 55  *****************
 * User: Arkkhasin    Date: 3/26/08    Time: 1:01a
 * Updated in $/MNCB/Dev/FIRMWARE/interface/hart
 * Sixth and final step toward uniform support of engineering units:
 * Formatting of numbers for local UI is now the responsibility of
 * fpconvert.c
 * Data integrity test API provided and used
 * Obsolete functions removed
 * Light linting
 *
 *
 * *****************  Version 53  *****************
 * User: Arkkhasin    Date: 3/05/08    Time: 6:24p
 * Updated in $/MNCB/Dev/FIRMWARE/hart
 * MODE_FAILSAFE is now a mode; replaces SUBMODE_OOS_FAILSAFE
 * Use mode_{Set,Get}Mode() API
 *
 * *****************  Version 52  *****************
 * User: Arkkhasin    Date: 9/20/07    Time: 4:01p
 * Updated in $/MNCB/Dev/FIRMWARE/hart
 * Removed a now-unused header
 *
 * *****************  Version 51  *****************
 * User: Arkkhasin    Date: 9/18/07    Time: 2:14p
 * Updated in $/MNCB/Dev/FIRMWARE/hart
 * Cmd 11 is no longer handled in a special way in mnhartcmd.c; the
 * processing function takes care of the peculiarities.
 *
 * *****************  Version 50  *****************
 * User: Arkkhasin    Date: 7/20/07    Time: 6:18p
 * Updated in $/MNCB/Dev/FIRMWARE/hart
 * Oops. Bug 1703 fix introduced a bug (caught by unit test): the sign of
 * return value of an app function was misinterpreted.
 * Now fixed.
 *
 * *****************  Version 49  *****************
 * User: Arkkhasin    Date: 7/19/07    Time: 3:40p
 * Updated in $/MNCB/Dev/FIRMWARE/hart
 * Lint: mnassert.h no longer used
 *
 * *****************  Version 48  *****************
 * User: Arkkhasin    Date: 7/19/07    Time: 3:08p
 * Updated in $/MNCB/Dev/FIRMWARE/hart
 * Bug 1703 fix (see description there)
 *
 * *****************  Version 47  *****************
 * User: Arkkhasin    Date: 5/21/07    Time: 11:51a
 * Updated in $/MNCB/Dev/FIRMWARE/hart
 * New tombstone header
*/


#include "mnwrap.h"

// #include "errcodes.h"
// #include "mncbtypes.h" //for configure.h prototypes

#include "hart.h"
#include "hartfunc.h"
#include "hartdef.h"

#include "wprotect.h" //for write protect input

// #include "nvram.h" //for configuration changed flag

#include "devicemode.h"
#include "faultpublic.h"
#include "uipublic.h"
#include "uistartnodes.h"
#include "process.h"
#include "bufutils.h"
#include "fpconvert.h"


static
s8_least hart_CmdExec(const hartDispatch_t *disp, const u8 *src, u8 *dst, u8 req_data_len)
{
    s8_least ret;
    if (disp == NULL)                         	  /* command not implementd ? */
    {
        ret = -COMMAND_NOT_IMPLEMENTED;
    }
    else if(req_data_len < disp->req_len)
    {
        /* There are not enough databytes received*/
        ret = -HART_TOO_FEW_DATA_BYTES_RECEIVED;
    }
    else if ( ((disp->flags & HC_WRITE_PROTECT) != 0) && bios_ReadWriteProtectInput()) //lint !e960 Violates MISRA 2004 Required Rule 12.4 AK:TEMPORARY UNTIL LINT 9
    {
        /* write protect violation attempt */
        ret = -HART_WRITE_PROTECT_MODE;
    }
    else if ( ((disp->flags & HC_MODIFY) != 0)
             && (hart_GetHartData()->hart_version < HART_REV_5) //fallback to old implementation
             && !ifman_IsAccessEnabled(hart_MsgFromSecondaryMaster(src)?iflock_owner_hart_primary:iflock_owner_hart_secondary)
            )
    {
        /* lock protect violation attempt */
        ret = -HART_ACCESS_RESTRICTED;
    }
    else if( (disp->flags & ((HC_WRITE_COMMAND | HC_PROCESS_COMMAND) & ~process_GetProcessFlags()))!= 0 )
    {
        ret = -HART_BUSY;
    }
    else
    {
        ret = -hart_CheckMode(disp->flags);
        if(ret == HART_NO_COMMAND_SPECIFIC_ERRORS)
        {
            //Test engineering units before possible use
            fpconvert_TestData();

            //Inform UI we are changing things (as needed)
            if ( (disp->flags & HC_WRITE_PROTECT) != 0)
            {
#ifdef OLD_DEVMODE
                const ModeData_t *md = mode_GetMode();
                if((md->mode==MODE_OOS) && (md->submode == SUBMODE_OOS_NORMAL))
#else
                if(mode_GetMode() == MODE_SETUP)
#endif
                {
                    ui_setNext(HART_OVERRIDE_INFO_NODE);
                }
            }

            /* Copy request data to the response buffer unconditionally so that if
            a specific command's processor feels like that, it can modify/replace
            the default response as it sees fit.
            If this data is not needed, it will be ignored.
            */
            util_GetU8Array(src, (size_t)(disp->rsp_len), dst);

            //All tests passed: process the command; disp is known to be a non-NULL
            ret = -disp->proc(src, dst); /* perform the command which returns a positive on error */
        }

        //a routine can return the length by setting the return to the negative of it (sign reversed above)
        //      This means HART_NO_COMMAND_SPECIFIC_ERRORS but different length
        if(ret >= HART_NO_COMMAND_SPECIFIC_ERRORS)
        {
            if(ret == HART_NO_COMMAND_SPECIFIC_ERRORS)
            {
                ret = disp->rsp_len;
            }
            else
            {
                //this was a length - command() wants us to return a positive number
            }
        }
    }
    return ret;
}

/**
\brief Central dispatcher of HART requests

Checks the app-level correctness of the data in the request
and dispatches processing to the appropriate app-level function

\param cmd - HART command to process
\param src - request data source as a byte array
\param dst - response data destination as a byte array
\param req_data_len - length (in bytes) of the src array
\return if negative, -error, otherwise, the length of the response in dst
*/
s8_least hart_Dispatch(u8_least cmd, const u8 *src, u8 *dst, u8 req_data_len)
{
    const hartDispatch_t *disp;
    const hartDispatch_t *subdisp;

    //See if the Command has a subcommand
    const SubCommandInfo_t *sub = hart_GetSubCommandInfo(cmd);
    s8_least ret = HART_NO_COMMAND_SPECIFIC_ERRORS;
    if(sub == NULL)
    {
        //No subcommand: the easy old way
        disp = HART_cmdlookup(cmd); /* look up the dispatch table entry */
        ret = hart_CmdExec(disp, src, dst, req_data_len);

        /** The exception in HART is that if command 11 is too short, there shall be no response.
        A good architecture would be to drop the frame in the command 11 handler, but it requires
        support of optional parameters in the HART framework.
        Until it is implemented, the wart is reinstated here.
        */
        if((ret == -HART_TOO_FEW_DATA_BYTES_RECEIVED) && (cmd == HART_COMMAND_11_ReadUniqueIdentifierByTag))
        {
            Hart_DropFrame();
        }
    }
    else
    {
        //subcommand exists; check its data lengths
        if(req_data_len < sub->data_offset)
        {
            //Not enough bytes even for the top-level command
            ret = -HART_TOO_FEW_DATA_BYTES_RECEIVED;
        }
        else if(req_data_len < sub->type_offset)
        {
            //If we ever get here, the const dispatcher table is misconfigured.
            //AK:FUTURE: probably can assert here
            ret = -HART_TOO_FEW_DATA_BYTES_RECEIVED;
        }
        else
        {
            subdisp = sub->lookup_func(src[sub->type_offset]);
            if(subdisp == NULL)
            {
                ret = -HART_INVALID_SELECTION;
            }
            else
            {
                //Preliminary conditions are met; execute the main command first
                disp = HART_cmdlookup(cmd); /* look up the dispatch table entry */
                ret = hart_CmdExec(disp, src, dst, sub->data_offset);
                s8_least len = ret;
                if(ret >= HART_NO_COMMAND_SPECIFIC_ERRORS)
                {
                    //Now we are in a position to execute the subcommand
                    ret = hart_CmdExec(subdisp, src + sub->data_offset, dst + len, req_data_len - sub->data_offset);
                    if(ret >= HART_NO_COMMAND_SPECIFIC_ERRORS)
                    {
                        ret += len; //combined length of responses to command and subcommand
                    }

                }
            }
        }
    }
    //a routine can return the length by setting the return to the negative of it (sign reversed noted)
    //      This means HART_NO_COMMAND_SPECIFIC_ERRORS but different length
    if(ret >= HART_NO_COMMAND_SPECIFIC_ERRORS)
    {
        ret = HART_NUM_STATUS_BYTES + ret;
    }
    return ret;
}

/**
\brief Central dispatcher of HART burst responses

Dispatches processing to the appropriate app-level function

\param cmd - HART command to process
\param dst - response data destination as a byte array
\return the length of the response in dst or if <0 an error code
*/
s8_least hart_BurstDispatch(u8_least cmd, u8 *dst)
{
    s8_least ret  = 0;
    const hartDispatch_t *disp;

    disp = HART_cmdlookup(cmd); /* look up the dispatch table entry */
    if(disp != NULL)
    {
        if(disp->proc == NULL)
        {
            //The command is listed but not implemented. Mark it as such:
            disp = NULL;
        }
    }

    if (disp == NULL)                         	  /* command not implementd ? */
    {
        ret = -COMMAND_NOT_IMPLEMENTED;
    }
    else
    {
        //Test engineering units before possible use
        fpconvert_TestData();

        /* perform the command; it must be happy to take a NULL request and infer length
        from its burst configuration
        */
        ret = disp->proc(NULL, dst);

        if(ret > HART_NO_COMMAND_SPECIFIC_ERRORS)
        {
            //propagate the error up as a negative
            ret = -ret;
        }
        else
        {
            if(ret == HART_NO_COMMAND_SPECIFIC_ERRORS)
            {
                ret = (s8_least)disp->rsp_len;
            }
            else
            {
                ret = -ret; //positive length
            }
            ret += HART_NUM_STATUS_BYTES; //length
        }
    }
    return ret;
}

/* This line marks the end of the source */
