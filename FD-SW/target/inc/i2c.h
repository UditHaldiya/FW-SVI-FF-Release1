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

FILE_NAME          i2c.h



DESCRIPTION        external declarations of 'i2c.c'

=========================================================================== */
#ifndef __i2c_h__
#define __i2c_h__


/* --- Defines ----------------------------------------------------------- */

/* the following defines are used with the function 'i2cBusRequest(...)': */

/* masks to build up I2C bus request types */
#define I2C_WRITE               0x00
#define I2C_READ                0x01
#define I2C_SET                 0x02

#define I2C_ADDR8               0x00
#define I2C_ADDR16              0x04

#define I2C_NOWAIT              0x00
#define I2C_WAIT                0x08

/* I2C bus request types (argument 'ucRequestType') */
#define I2C_WRITE_ADDR8_NOWAIT  (I2C_WRITE | I2C_ADDR8  | I2C_NOWAIT)
#define I2C_WRITE_ADDR8_WAIT    (I2C_WRITE | I2C_ADDR8  | I2C_WAIT)
#define I2C_WRITE_ADDR16_NOWAIT (I2C_WRITE | I2C_ADDR16 | I2C_NOWAIT)
#define I2C_WRITE_ADDR16_WAIT   (I2C_WRITE | I2C_ADDR16 | I2C_WAIT)
#define I2C_READ_ADDR8_NOWAIT   (I2C_READ  | I2C_ADDR8  | I2C_NOWAIT)
#define I2C_READ_ADDR8_WAIT     (I2C_READ  | I2C_ADDR8  | I2C_WAIT)
#define I2C_READ_ADDR16_NOWAIT  (I2C_READ  | I2C_ADDR16 | I2C_NOWAIT)
#define I2C_READ_ADDR16_WAIT    (I2C_READ  | I2C_ADDR16 | I2C_WAIT)
#define I2C_SET_ADDR8_NOWAIT    (I2C_SET   | I2C_ADDR8  | I2C_NOWAIT)
#define I2C_SET_ADDR8_WAIT      (I2C_SET   | I2C_ADDR8  | I2C_WAIT)
#define I2C_SET_ADDR16_NOWAIT   (I2C_SET   | I2C_ADDR16 | I2C_NOWAIT)
#define I2C_SET_ADDR16_WAIT     (I2C_SET   | I2C_ADDR16 | I2C_WAIT)

/* I2C bus error codes (return values) */
#define I2C_NO_ERROR            0
#define I2C_ERROR_NO_ACK        1
#define I2C_ERROR_BUS_BUSY      2


/* --- Typedefinitions --------------------------------------------------- */

/* --- Global Constants and Variables ------------------------------------ */

/* --- Prototypes -------------------------------------------------------- */

//void i2cInit(void);
void hw_I2C_init(void);

unsigned char i2cBusRequest(unsigned char ucSlaveAddr,
                            unsigned int  uiWordAddr,
                            unsigned char *pucData,
                            unsigned int  uiNbrOfBytes,
                            unsigned char ucRequestType,
                            void          (*pvCallback) (unsigned int)
                           );

#endif /* __i2c_h__ */

