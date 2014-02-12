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

FILE_NAME          eep_hw.h



DESCRIPTION        This file contains the prototypes for EEPROM programing.

=========================================================================== */
#ifndef __EEP_HW__
#define __EEP_HW__

/****************************************************************************/

GLOBAL_DEFINES

#if defined (EEPROM_AT28LV64)

    #define EEPROM_SIZE         0x2000      /* size of the EEPROM       */
    #define EEPROM_ROW_SIZE     64          /* size of an EEPROM row    */

    typedef USIGN16 T_EEPROM_CNT;           /* max data block size      */
    typedef USIGN16 T_EEP_ADDR;             /* EEPROM address           */

#elif defined (EEPROM_AT24C32)

    #define EEPROM_SIZE         0x1000      /* size of the EEPROM       */
    #define EEPROM_ROW_SIZE     32          /* size of an EEPROM row    */

    typedef USIGN16 T_EEPROM_CNT;           /* max data block size      */
    typedef USIGN16 T_EEP_ADDR;             /* EEPROM address           */

#elif defined (EEPROM_AT24C64)

    #define EEPROM_SIZE         0x2000      /* size of the EEPROM       */
    #define EEPROM_ROW_SIZE     32          /* size of an EEPROM row    */

    typedef USIGN16 T_EEPROM_CNT;           /* max data block size      */
    typedef USIGN16 T_EEP_ADDR;             /* EEPROM address           */

#elif defined (AT25P1024)

    #define EEPROM_SIZE         0x20000     /* size of the EEPROM       */
    #define EEPROM_ROW_SIZE     128         /* size of an EEPROM row    */

    typedef USIGN32 T_EEPROM_CNT;           /* max data block size      */
    typedef USIGN32 T_EEP_ADDR;             /* EEPROM address           */

#elif defined (FRAM_FM3216)

    #define EEPROM_SIZE         0x800       /* size of the FRAM         */
    #define EEPROM_ROW_SIZE     0           /* size of an EEPROM row    */

    typedef USIGN16 T_EEPROM_CNT;           /* max data block size      */
    typedef USIGN16 T_EEP_ADDR;             /* EEPROM address           */

#elif defined (FRAM_FM3264)

    #define EEPROM_SIZE         0x2000      /* size of the FRAM         */
    #define EEPROM_ROW_SIZE     0           /* size of an FRAM row      */

    typedef USIGN16 T_EEPROM_CNT;           /* max data block size      */
    typedef USIGN16 T_EEP_ADDR;             /* EEPROM address           */

#elif defined (FRAM_FM32256)

    #define EEPROM_SIZE         0x8000      /* size of the FRAM         */
    #define USABLE_EEPROM_SIZE  0x2000      /* limit size for compatibility reasons */
    #define EEPROM_ROW_SIZE     0           /* size of an FRAM row      */

    typedef USIGN16 T_EEPROM_CNT;           /* max data block size      */
    typedef USIGN16 T_EEP_ADDR;             /* EEPROM address           */

#elif defined (FRAM_FM2464) /*@Dresser-Masoneilan project modified */

    #define EEPROM_SIZE         0x2000      /* size of the FRAM         */
    #define USABLE_EEPROM_SIZE  0x2000      /* limit size for compatibility reasons */
    #define EEPROM_ROW_SIZE     0           /* size of an FRAM row      */

    typedef USIGN16 T_EEPROM_CNT;           /* max data block size      */
    typedef USIGN16 T_EEP_ADDR;             /* EEPROM address           */

#elif defined (FRAM_FM24256) /*@Dresser-Masoneilan project modified for Phase II */

    #define EEPROM_SIZE         (0x10000-0)      /* size of the FRAM         */
    #define USABLE_EEPROM_SIZE  (0x10000-44)/2   /* half of (physical size less reserved) to accommodate 2 copies  */
    #define EEPROM_ROW_SIZE     0           /* size of an FRAM row      */

    typedef USIGN16 T_EEPROM_CNT;           /* max data block size      */
    typedef USIGN16 T_EEP_ADDR;             /* EEPROM address           */

#endif /* EEPROM TYPE */

#endif /* __EEP_HW__ */
