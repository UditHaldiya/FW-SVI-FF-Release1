#MP If 0 ;Tombstone header
/*
Copyright 2006-2007 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file logfileconf.inc
    \brief Project-independent generating macros for the log file configuration table

    This file yields logfdata.h and logfdata.c with definitions of log file
    system configuration

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/FIRMWARE/includes/logfileconf.inc $
    $Date: 4/15/09 11:20p $
    $Revision: 9 $
    $Author: Arkkhasin $

    \ingroup logfile
*/
/* (Optional) $History: logfileconf.inc $
 *
 * *****************  Version 9  *****************
 * User: Arkkhasin    Date: 4/15/09    Time: 11:20p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * Log file private definitions extracted from nvmempriv.h
 *
 * *****************  Version 8  *****************
 * User: Arkkhasin    Date: 5/21/07    Time: 11:59a
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * New tombstone header
*/
#MP Endif ;Tombstone header
#MP Set LogfConfig_IDs = 1
#MP Set LogfConfig_Tables = 2

#MP Macro SetLogfConfigOption ;(option)
#MP If #1# == LogfConfig_IDs
#MP     done_start = 100
#MP     done_end = 100 ;single-pass IDs production
#MP Endif
#MP If #1# == LogfConfig_Tables
#MP     done_start = 0
#MP     done_end = 1
#MP Endif
#MP Endm

#MP Macro LogfBegin ;([filename.])
#MP Ifdef  BaseFileName {STR} ;output pair
#MP     Export (0) {uJoin, BaseFileName, ".c"}
/*
Copyright 2006-2007 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file #mp%sBaseFileName.c
    \brief Project-dependent log file configuration table

    CPU: Any

    OWNER: #mp%sBaseFileName.u
    $Archive: /MNCB/Dev/FIRMWARE/includes/logfileconf.inc $
    $Date: 4/15/09 11:20p $
    $Revision: 9 $
    $Author: Arkkhasin $

    \ingroup logfile
*/
/* (Optional) $History: logfileconf.inc $
 *
 * *****************  Version 9  *****************
 * User: Arkkhasin    Date: 4/15/09    Time: 11:20p
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * Log file private definitions extracted from nvmempriv.h
 *
 * *****************  Version 8  *****************
 * User: Arkkhasin    Date: 5/21/07    Time: 11:59a
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * New tombstone header
*/
#define NEED_NVMEM_END
#include "mnwrap.h"
#include MEMMAP_H_
#include "mnassert.h"
#include "nvram.h"
#include "logfsupport.h"
#include "faultpublic.h"
#include "logfpriv.h"
#include "logfileids.h"
#include "logfile.h"
#include "#mp%sBaseFileName.h"
#MP     Export Push
#MP     Export (0) {uJoin, BaseFileName, ".h"}
#MP     Setstr hdrguard = {uJoin, BaseFileName, "_H_"}
/*
Copyright 2006-2007 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.
This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file #mp%sBaseFileName.h
    \brief A Private header for the project-dependent log file configuration table

    CPU: Any

    OWNER: #mp%sBaseFileName.u

    \ingroup logfile
*/
#ifndef #mp%shdrguard
#define #mp%shdrguard
//lint -save -e506 Bool constant is OK and necessary in constant evaluation
#MP Endif
#MP     pass = 1;
#MP   For done = done_start, done_end
#MP     count = 0;
#MP Endm

#MP Macro LogfDefine ;(size, num_copies, logf_id)
#MP     count = count + 1
#MP     prev = pass - 1
#MP     prev_count = count - 1
#MP  If done==0
#MP; The contribution of 'size' minimum calc is the 'size' if it is greater than the previous min or SIZE_BIG otherwise
#MP
#MP     If pass==1
typedef u8 contrib_#mp{%d pass}_#mp%dcount[#mp%n#1#];
#MP         Setstr NumCopies_%dcount = {#2#}
#MP         If #0# >= 4 ;event id supplied?
#MP             Setstr eventId_%dcount = {#4#}
#MP         Endif
#MP         If #0# >= 5 ;event error id supplied?
#MP             Setstr eventErrId_%dcount = {#5#}
#MP         Endif
#MP         Setstr eventLogId_%dcount = {#3#}
#MP     Else
typedef u8 contrib_#mp{%d pass}_#mp%dcount[(sizeof(typemin_#mp{%d prev}_#mp%dnum_entries)>=#mp%n#1#)?SIZE_BIG:#mp%n#1#];
#MP     Endif
#MP
#MP; size of typemin is the running min
#MP     If count==1
typedef contrib_#mp{%d pass}_#mp%dcount typemin_#mp{%dpass}_#mp%dcount;
#MP     Else
typedef u8 typemin_#mp{%dpass}_#mp%dcount[MIN(sizeof(typemin_#mp{%d pass}_#mp%dprev_count), sizeof(contrib_#mp{%d pass}_#mp%dcount))];
#MP     Endif
#MP  Endif ;done==0
#MP  If done == 1
#MP     For i=1, num_entries
#MP ;sizeof is the running number of extents required by i=th least size
#MP         If count==1
typedef u8 extnum_#mp{%di}_#mp%dcount[1 + ((sizeof(typemin_#mp{%di}_#mp%dnum_entries)<=#mp%n#1#)?#mp%n#2#:0)];
#MP         Else
#MP             prev_i = i-1
typedef u8 extnum_#mp{%di}_#mp%dcount[sizeof(extnum_#mp{%di}_#mp%dprev_count) + ((sizeof(typemin_#mp{%di}_#mp%dnum_entries)<=#mp%n#1#)?#mp%n#2#:0)];
#MP         Endif
#MP     Endfor
#MP     For i=1, num_entries
#MP         If i==1
typedef u8 exttypes_#mp{%di}_#mp%dcount[(sizeof(typemin_#mp{%di}_#mp%dnum_entries)>=#mp%n#1#)?#mp%di:#mp%dnum_entries];
#MP         Else
#MP             prev_i = i-1
typedef u8 exttypes_#mp{%di}_#mp%dcount[MIN(sizeof(exttypes_#mp{%dprev_i}_#mp%dcount), ((sizeof(typemin_#mp{%di}_#mp%dnum_entries)>=#mp%n#1#)?#mp%di:#mp%dnum_entries))];
#MP         Endif
#MP     Endfor
#MP  Endif ;done==1
#MP  If done == 100 ;dumping the ids
#define #mp%n#3# #mp%dprev_count
#MP  Endif

#MP Endm

#MP Macro LogfEnd ;()
#MP    If done==0
#MP     If pass == 1
#MP         num_entries = count
#define SIZE_BIG 999999 //Any big number will do, esp. greater than 0xffff
#MP     Endif
#MP     If pass == num_entries
#MP         ;done = 2;
#MP         pass = 1
#MP     Else
#MP         pass = pass + 1
#MP         done = done-1
#MP     Endif
#MP    Endif
#MP    If done==1
#MP     If pass < num_entries
#MP         pass = pass + 1
#MP         ;done = done-1
#MP     Endif
#MP
#MP    Endif
#MP   Endfor
#MP     ;This is a private stuff, not applicable to public id output
#MP   If done_start < 100
     //-------- Extent sizes ----------
#define EXTENT_SIZE_1 (sizeof(u16) + sizeof(typemin_1_#mp%dnum_entries))
#MP         For i=2, num_entries
#MP             prev_i = i-1
#define EXTENT_SIZE_#mp%di ((sizeof(typemin_#mp{%di}_#mp%dnum_entries)==SIZE_BIG)?0:(sizeof(typemin_#mp{%di}_#mp%dnum_entries) - sizeof(typemin_#mp{%dprev_i}_#mp%dnum_entries)))
#MP         Endfor
     //-------- Numbers of extents needed per extent type
#MP         For i=1, num_entries
#define EXTENT_NUM_#mp%di ((sizeof(typemin_#mp{%di}_#mp%dnum_entries)==SIZE_BIG)?0:sizeof(extnum_#mp{%di}_#mp%dnum_entries))
#MP         Endfor
     //-------- Base addresses per extent type --------
#define EXTENT_BASEADDR_1 0
#MP         For i=2, num_entries+1
#MP             prev_i = i-1
typedef u8 base_addr_#mp%di[EXTENT_BASEADDR_#mp%dprev_i + (EXTENT_SIZE_#mp%dprev_i * EXTENT_NUM_#mp%dprev_i)];
#define EXTENT_BASEADDR_#mp%di sizeof(base_addr_#mp%di)
#MP         Endfor
#MP         i=num_entries+1
#define EXTENTS_TOTAL_MEMORY_PROJ EXTENT_BASEADDR_#mp%di
    //-------- total number of extents
#define EXTENTS_TOTAL_PROJ (0\
#MP         For i=1, num_entries
    +EXTENT_NUM_#mp%di\
#MP         Endfor
)
    //-------- Numbers of extent types needed per a log file
#MP         For i=1, num_entries
#define LOGF_NUM_EXTENT_TYPES_#mp%di sizeof(exttypes_#mp{%dnum_entries}_#mp%di)
#MP         Endfor
#MP         Ifdef hdrguard {STR}
//lint -restore turn 506 Bool constant back on
#endif //#mp%shdrguard
#MP             Undef hdrguard {STR}
#MP             Export Pop ;restore output to the C file
#MP         Endif
    //-------- Reference table of extents indexed by type (==ascending number in unique sizes of extents
const extdef_t extdef[] =
{
#MP         For i=1, num_entries
    {
        EXTENT_BASEADDR_#mp%di,
        EXTENT_SIZE_#mp{%di},
        EXTENT_NUM_#mp{%di}
    },
#MP         Endfor
};
//i-th entry is Number of extent types needed to hold the log file i
const logdef_t logdef[] =
{
#MP         For i=1, num_entries
    LogDef(#mp%sNumCopies_%di, LOGF_NUM_EXTENT_TYPES_#mp%di),
#MP         Endfor
};
CONST_ASSERT(#mp%dnum_entries <= LOGF_MAX_NUM_LOGS); //make sure project-independent max number of logs is sufficient
CONST_ASSERT(EXTENTS_TOTAL_PROJ <= LOGF_EXT_COUNT); //Check that project-independent LOGF_EXT_COUNT is sufficient

//total number of extents (for test of definition)
const u8 logf_extents_total = EXTENTS_TOTAL_PROJ;
const u8 logf_total = #mp%dnum_entries;

// ------------------------ events (faults) to set/clear on newest log write/read
const logfEvent_t logf_EventTable[#mp%dnum_entries] =
{
#MP         For i=1, num_entries
    [#mp%seventLogId_%di] =
    {
        .newdata =
#MP             Ifdef eventId_%di {STR}
                    #mp%seventId_%di,
#MP                 Undef eventId_%di
#MP             Else
                    FAULT_NONE,
#MP             Endif
        .dataerr =
#MP             Ifdef eventErrId_%di {STR}
                    #mp%seventErrId_%di,
#MP                 Undef eventErrId_%di
#MP             Else
                    FAULT_NONE,
#MP             Endif
    },
#MP             Undef eventLogId_%di
#MP         Endfor
};

#MP   Endif ;done_start < 100

#MP ;Cleanup
#MP Undef done_start
#MP Undef done_end
#MP Endm

