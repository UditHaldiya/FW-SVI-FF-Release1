/*
Copyright 2004-2007 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/
/**
    \file mncbdefs_ESD.h
    \brief Project-specific companion to mncbdefs.h

	The first entries are for noise band.

    CPU: Any

    OWNER: AK
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/inc_FFAP/mncbdefs_proj.h $
    $Date: 1/30/12 1:50p $
    $Revision: 5 $
    $Author: Arkkhasin $

    \ingroup proj
*/
/* (Optional) $History: mncbdefs_proj.h $
 * 
 * *****************  Version 5  *****************
 * User: Arkkhasin    Date: 1/30/12    Time: 1:50p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_FFAP
 * Provided project identification FBO TFS:8782
 * 
 * *****************  Version 3  *****************
 * User: Arkkhasin    Date: 1/12/12    Time: 11:57p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_SVI2AP
 * **Tempporarily** device type 02 (SA compatibility)
 * 
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 1/12/12    Time: 12:31p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_SVI2AP
 * **Tempporarily** device type 01
 * 
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 12/05/11   Time: 4:48p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_SVI2AP
 * TFS:8347
 *
 * *****************  Version 6  *****************
 * User: Arkkhasin    Date: 2/10/10    Time: 6:58p
 * Updated in $/MNCB/Dev/FIRMWARE/inc_SVI2AP
 * TXSPEC_REV=3
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

#define MNCB_MANUFACTURER_DEVICE_CODE   0x03 //0xfe //202U //! HART device id (AP)

#define ASCII_F 0x46u
//#define ASCII_F 0x46u
#define MNCB_MAGIC_NUMBER ((ASCII_F<<8)|ASCII_F) //!Device type FRAM id

#define MNCB_HW_REV 1U
#define SW_REV 1U //TBD
#define TXSPEC_REV 1U

#define FIRMWARE_TYPE 3 //FF AP

#define NVRAM_VERSION 1 //Manually maintained. Absolutely must be kept current
#define MNCB_VERSION_NUMBER_ NVRAM_VERSION //Manually maintained. Absolutely must be kept current

#endif //MNCBDEFS_PROJ_H_
/* This line marks the end of the source */
