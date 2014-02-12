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

FILE_NAME          hw_svi_swdl_if.h



DESCRIPTION

=========================================================================== */

#ifndef __HW_SVI_SWDL_IF_H__
#define __HW_SVI_SWDL_IF_H__


/*--- Return Codes of the interface functions ------------------------------*/
#define HW_DWNLD_WRITE_OK             0         /* Successful write  */
#define HW_DWNLD_WRITE_ERR            11        /* Write fails */

#define HW_DWNLD_OPEN_PARTIION_FAILED 1         /* Flash Partition could not be opened */
#define HW_DWNLD_OPEN_PARTIION_OK     0         /* Flash Partition successful opened for storage of new FW domain */



/* Return Codes of GE/Dresser interface functions */
#define EOP (1u << 5)         // normal - not an error
#define WRPRTERR (1u << 4)    // device write protected
#define PGERR (1u << 2)       // program or erase error
#define BSY (1u << 0)         // device still busy after timeout period
#define RNG (1u << 6)         // attempt to program/erase outside of bank address range



/*--- HW specific definitions -----------------------------------------------*/

#define FLASH_START_ADDRESS_BANK0     0x8000800uL /* Start Address of Fimware in Flash Memory Bank0 */
#define FLASH_START_ADDRESS_BANK1     0x8080800uL /* Start Address of Fimware in Flash Memory Bank1 */
#define FW_PAGES_IN_FLASH             255         /* Number of Flash Sectors used for a FW --> 512kB / 2kB (Page Size) minus the Partion Manager */
#define FLASH_PAGE_SIZE               0x800uL     /* Size of a Flash Page is 2kByte */


/****************************************************************************/
/*                  FUNCTION DECLARATIONS                                   */
/****************************************************************************/

/* flash operation function */
//BOOL flash_OpenPartition(void);
//USIGN32 flash_GetStatus(void);
//BOOL flash_ClosePartition(void);
//BOOL flash_ProgramBlock(const USIGN16 *,USIGN16 *,USIGN32);

#endif  /* __HW_SVI_SWDL_IF_H__ */
