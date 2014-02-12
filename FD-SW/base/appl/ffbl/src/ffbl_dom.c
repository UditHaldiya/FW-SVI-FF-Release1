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

FILE_NAME          ffbl_dom.c



FUNCTIONAL_MODULE_DESCRIPTION

     THIS FILE IS USED ONLY FOR TESTING THE FUNCTION BLOCK SHELL
     THE FUNCTIONS SHOWN IN THIS FILE DO NOT REPRESENT ANY FUNCTION
     BLOCK APPLICATION

=========================================================================== */
  #include "keywords.h"
  #define  MODULE_ID      (COMP_FFBL + MOD_FBLKDOM)

INCLUDES
  #include "base.h"
  #include "except.h"

#if defined (DOM_DOWNLOAD_SUPP) || defined (LINK_MASTER)

LOCAL_DEFINES

#define DOM_READY                5  /* Content defined; writeable     */

LOCAL_TYPEDEFS

FUNCTION_DECLARATIONS

IMPORT_DATA

EXPORT_DATA

LOCAL_DATA






/****************************************************************************/

GLOBAL USIGN16 Domain_get_index (void)
{
     return 457;
}



GLOBAL USIGN8    Domain_get_state (void)
{
     return DOM_READY;
}



GLOBAL USIGN8    Domain_handler
     (
     USIGN8     type_of_ind,
     USIGN8     len_of_domain_seg,
     USIGN8 *   a_of_domain_seg
     )
{
     _UNREF_PARAM (type_of_ind);
     _UNREF_PARAM (len_of_domain_seg);
     _UNREF_PARAM (a_of_domain_seg);

     return E_OK;
}
#endif /* DOM_DOWNLOAD_SUPP, LINK_MASTER */

