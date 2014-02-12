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

FILE_NAME          hm_tools.h


DESCRIPTION

=========================================================================== */
#ifndef __HM_TOOLS__
#define __HM_TOOLS__

INCLUDES

GLOBAL_DEFINES
// copy to/from bus to/from variable
#define RdSTATUS(d,s) {*(USIGN32*)d = *(USIGN32*)s;}

#define Rd1(d,s)        do {*((USIGN8 *) (d)) = *((USIGN8 *) (s));} while (0)
#define Wrt1(d,s)       Rd1(d,s)

#define Rd2(d,s)        do {    \
                            ((USIGN8 *) (d))[0] = ((USIGN8 *) (s))[1];  \
                            ((USIGN8 *) (d))[1] = ((USIGN8 *) (s))[0];  \
                        } while (0)
#define Wrt2(d,s)       Rd2(d,s)

#define Rd3(d,s)         do {    \
                            ((USIGN8 *) (d))[0] = ((USIGN8 *) (s))[2];  \
                            ((USIGN8 *) (d))[1] = ((USIGN8 *) (s))[1];  \
                            ((USIGN8 *) (d))[2] = ((USIGN8 *) (s))[0];  \
                            ((USIGN8 *) (d))[3] = 0;  \
                        } while (0)
#define Wrt3(d,s)       do {    \
                            ((USIGN8 *) (d))[0] = ((USIGN8 *) (s))[2];  \
                            ((USIGN8 *) (d))[1] = ((USIGN8 *) (s))[1];  \
                            ((USIGN8 *) (d))[2] = ((USIGN8 *) (s))[0];  \
                        } while (0)

#define Rd4(d,s)        do {    \
                            ((USIGN8 *) (d))[0] = ((USIGN8 *) (s))[3];  \
                            ((USIGN8 *) (d))[1] = ((USIGN8 *) (s))[2];  \
                            ((USIGN8 *) (d))[2] = ((USIGN8 *) (s))[1];  \
                            ((USIGN8 *) (d))[3] = ((USIGN8 *) (s))[0];  \
                        } while (0)
#define Wrt4(d,s)       Rd4(d,s)

#define Rd5(d,s)  {(d)[0] = (status == SQ_GOOD_CAS) ? (d)[0]:status; (d)[4] = (s)[0]; (d)[3] = (s)[1]; (d)[1] = (s)[3]; (d)[2] = (s)[2];}
#define Wrt5(d,s) {(d)[3] = (s)[1]; (d)[2] = (s)[2]; (d)[0] = (s)[4]; (d)[1] = (s)[3];}

#define Rd7(d,s) MkFFDate7(d,s)
#define Wrt7(d,s) MkHartDate7(d,s)

// data size of types
typedef struct {char b[3];} USIGN24;
typedef struct {char b[7];} DATE7;

#define DS65    FLOAT_S



#define RdUSIGN8        Rd1
#define RdUSIGN16       Rd2
#define RdUSIGN24       Rd3
#define RdUSIGN32       Rd4
#define RdFLOAT         Rd4
#define RdDS65          Rd5
#define RdDATE7         Rd7
#define RdTABLE         Rd99

#define WrtUSIGN8       Wrt1
#define WrtUSIGN16      Wrt2
#define WrtUSIGN24      Wrt3
#define WrtUSIGN32      Wrt4
#define WrtFLOAT        Wrt4
#define WrtDS65         Wrt5
#define WrtDATE7        Wrt7
#define WrtTABLE        Wrt99


/*----------------------------------------------------------------------------
MACRO READ_HART_CMD(hart_offset, component, ff_type)

MACRO_DESCRIPTION:
  Reads a value from the HART buffer and copies it to a FF block parameter

PARAMETERS:
  hart_offset               offset in hart buffer
  component                 FF block paramter
  ff_type                   type of FF variable

VARIABLES_AFFECTED
  fromHart                  buffer containing HART data

----------------------------------------------------------------------------*/
#define READ_HART_CMD(hart_offset, component, ff_type)     \
    do {    \
        Rd##ff_type((USIGN8*)&(component), (USIGN8*)&from_hart[hart_offset]);     \
    } while (0)


/*----------------------------------------------------------------------------
MACRO WRITE_HART_CMD(hart_offset, component, ff_type)

MACRO_DESCRIPTION:
  Writes a value from a FF block parameter into the HART buffer

PARAMETERS:
  hart_offset               offset in hart buffer
  component                 FF block paramter
  ff_type                   type of FF variable

VARIABLES_AFFECTED
  fromHart                  buffer containing HART data

----------------------------------------------------------------------------*/
#define WRITE_HART_CMD(hart_offset, component, ff_type)     \
    do {    \
        Wrt##ff_type((USIGN8*)&toHart[hart_offset], (USIGN8*)&(component));     \
    } while (0)


EXPORT_TYPEDEFS

typedef enum
{
    HART_UNINITIALIZED, // not initialized
    HART_COMM,          // communcation ok.
    HART_COMM_ERROR,    // communication error
    HART_INIT_FINISHED  // initialization finished

} HART_STATES;



IMPORT_DATA

FUNCTION_DECLARATIONS

FUNCTION extern USIGN16 conv_unit_hart2ff (USIGN8 hart_unit);
FUNCTION extern USIGN8 conv_unit_ff2hart (USIGN16 ff_unit);
FUNCTION extern int packed_ascii_to_string (const USIGN8 * pPA, STRINGV * str, int n);
FUNCTION extern int string_to_packed_ascii (const STRINGV * str, USIGN8 * pPA, int n);
FUNCTION extern void MkFFDate7(USIGN8 *d, USIGN8 *s);
FUNCTION extern void MkHartDate7(USIGN8 *d, USIGN8 *s);


#endif /* __HM_TOOLS__ */


