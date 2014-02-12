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
*                     Copyright (C) SOFTING IA GmbH 2012                     *
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
* PROJECT_NAME             Softing FF/PA FD 2.42                             *
*                                                                            *
* VERSION                  FF - 2.42                                         *
*                          PA - 2.42 (beta)                                  *
*                                                                            *
* DATE                     16. April 2012                                    *
*                                                                            *
*****************************************************************************/

/* ===========================================================================

FILE_NAME          appl_svi.h



DESCRIPTION

=========================================================================== */

#ifndef __APPL_SVI_H__
#define __APPL_SVI_H__


/*--- Return Codes of DWNLD APP --------------------------------------------*/


//#define DWNLD_OK                0         /* NO Error */

//#define ACTIVATE_OK             1         /* ACTIVATE command successful */
//#define ACTIVATE_NOT_OK         2         /* ACTIVATE command not successful */

#define NO_DATA                 20        /* received data length was zero */
//#define PREP_WAITING            21        /* Preparation has not finished */

//#define DOM_HEADER_VALID        0         /* received domain header is valid for this device */
//#define DOM_HEADER_INVALID      22        /* received domain header is invalid for this device */

//#define DOMAIN_HEADER_OK        0
//#define INVALID_MANUFACTURER_ID 23
//#define INVALID_DEVICE_FAMILY   24
//#define INVALID_DOMAIN_NAME     25
//#define WRITE_TO_EEPROM_FAILED  26

//#define SWDL_CHECKSUM_ERR       12
#define SWDL_DATA_INCOMPLETE    13
#define SWDL_DATA_OVERRUN       14

/*--- Binary Header definitions --------------------------------------------*/

#define PRI_H_LEN               32        /* length of private header in Softing's sample implementation */

/****************************************************************************/
/*                  FUNCTION DECLARATIONS                                   */
/****************************************************************************/

FUNCTION PUBLIC BOOL appl_fd_sw_available (VOID);

FUNCTION PUBLIC USIGN8 appl_check_fd_dom_hdr
  (
    IN T_DOMAIN_HDR_API *   p_dwnld_dom_hdr
  );

FUNCTION PUBLIC USIGN8 appl_store_fd_dom_hdr
  (
    IN T_DOMAIN_HDR_API *   p_dwnld_dom_hdr
  );

FUNCTION PUBLIC USIGN8 appl_check_fd_dom_segm
  (
    IN USIGN8           segment_id,
    IN USIGN8           segment_len,
    IN USIGN8 *         p_segment
  );

/* field device download operation function */
FUNCTION PUBLIC VOID    init_svi_dwnld(VOID);
FUNCTION PUBLIC VOID    start_prepare_swdl(VOID);
FUNCTION PUBLIC USIGN8  finish_prepare_swdl(VOID);
FUNCTION PUBLIC BOOL    activate_fd_sw(VOID);
FUNCTION PUBLIC BOOL    complete_fd_dom_is_avail(VOID);
FUNCTION PUBLIC USIGN8  dwnld_handle_data(USIGN8,USIGN8,USIGN8*);

/* common domain download function */
FUNCTION PUBLIC USIGN8  verify_activation(VOID);

#endif  /* __APPL_SVI_H__ */
