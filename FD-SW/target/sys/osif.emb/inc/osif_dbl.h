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

#ifndef OSIF_DBL_H
#define OSIF_DBL_H

typedef struct _FB_DOUBLE
{
    unsigned long lower;
    long          upper;

} FB_DOUBLE;

/* convert float to double */
void osif_float_to_double (float a1, FB_DOUBLE *res);

/* convert double to float */
float osif_double_to_float (const FB_DOUBLE *a1);

/* add two doubles */
void osif_add_double (const FB_DOUBLE *a1, const FB_DOUBLE *a2, FB_DOUBLE *res);

/* subtract two doubles */
void osif_sub_double (const FB_DOUBLE *a1, const FB_DOUBLE *a2, FB_DOUBLE *res);

#endif /* OSIF_DBL_H */
