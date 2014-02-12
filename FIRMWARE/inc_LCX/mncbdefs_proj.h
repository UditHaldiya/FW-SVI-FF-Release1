/*
Copyright 2004-2007 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file mncbdefs_proj.h
    \brief LCX Project-specific companion to mncbdefs.h

	The first entries are for noise band.

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/inc_LCX/mncbdefs_proj.h $
    $Date: 12/09/11 3:44p $
    $Revision: 7 $
    $Author: Arkkhasin $

    \ingroup proj
*/
/* (Optional) $History: mncbdefs_proj.h $
 * 
 * *****************  Version 7  *****************
 * User: Arkkhasin    Date: 12/09/11   Time: 3:44p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_LCX
 * TFS:8370 Module drop-in -- engunits_1-0
 *
 * *****************  Version 6  *****************
 * User: Arkkhasin    Date: 12/07/11   Time: 12:29p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_LCX
 * TFS:8204 - features for I/O channels
 *
 * *****************  Version 5  *****************
 * User: Arkkhasin    Date: 11/29/11   Time: 12:52p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_LCX
 * TFS:8313 Lint
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 11/21/11   Time: 1:39a
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_LCX
 * NVM layout 8 FBO TFS:8255
 *
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 11/04/11   Time: 4:24p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_LCX
 * TFS:8072 NVMEM upgrade
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 3/08/10    Time: 12:42p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * 8.
 * Getting $/MNCB/Dev/FIRMWARE/inc_LCX/dimensions.inc
 * Getting $/MNCB/Dev/FIRMWARE/inc_LCX/mncbdefs_proj.h
 *
 * *****************  Version 7  *****************
 * User: Arkkhasin    Date: 11/25/09   Time: 5:37p
 * Updated in $/MNCB/Dev/FIRMWARE/inc_LCX
 * Initial LCX settings
 *
 * *****************  Version 5  *****************
 * User: Arkkhasin    Date: 11/17/09   Time: 6:07p
 * Updated in $/MNCB/Dev/FIRMWARE/inc_SVI2AP
 * HW_REV 3U; SW_REV 1U TXSPEC_REV 2U (made unsigned and HART spec rev 2)
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 5/26/08    Time: 9:34p
 * Updated in $/MNCB/Dev/FIRMWARE/inc_SVI2AP
 * Added number of NVMEM volumes. The header is no longer invoked by its
 * symbolic name (and renamed and moved)
 *
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 4/11/08    Time: 10:24a
 * Updated in $/MNCB/Dev/FIRMWARE/includes
 * Tentative fix for FRAM layout generator
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 4/08/08    Time: 7:01p
 * Created in $/MNCB/Dev/FIRMWARE/includes
 * Project-specific companion to mncbdefs.h
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 4/07/08    Time: 10:22p
 * Created in $/MNCB/Dev/FIRMWARE/includes
 * Project-specific companion to mncbdefs.h
*/
#ifndef MNCBDEFS_PROJ_H_
#define MNCBDEFS_PROJ_H_

#define MNCB_MANUFACTURER_DEVICE_CODE   0xCCU //! HART device id (Low cost AP)
#define MNCB_HW_REV 1U

#define ASCII_L 0x4CU
#define ASCII_C 0x43U
#define MNCB_MAGIC_NUMBER ((ASCII_L<<8)|ASCII_C) //!Device type FRAM id

#define HW_REV 1U
#define SW_REV 1U //TBD
#define TXSPEC_REV 1U

#define FIRMWARE_TYPE 1 //AP

#define NVRAM_VERSION 10 //[fpconvert] Manually maintained. Absolutely must be kept current
#define MNCB_VERSION_NUMBER_ NVRAM_VERSION //Manually maintained. Absolutely must be kept current

#endif //MNCBDEFS_PROJ_H_
/* This line marks the end of the source */
