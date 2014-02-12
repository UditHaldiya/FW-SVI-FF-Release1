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

FILE_NAME          hm_tools.c



FUNCTIONAL_MODULE_DESCRIPTION

  This module contains helper functions for the mapping of the hart master.


=========================================================================== */
  #include "keywords.h"
  #define  MODULE_ID      (COMP_FAPP + MOD_APPL_TRN)

INCLUDES
  #include <stdlib.h>
  #include <string.h>
  #include <ctype.h>

  #include "base.h"
  #include "hw_if.h"
  #include "except.h"

  #include "hm_tools.h"



// days of a year, relative to the first day of a mounth
const USIGN16 DaysOfMon[12]={0,31,59,90,120,151,181,212,243,273,304,334};

/* Unit conversion table */
const USIGN16 HART_2_FF_units[256] =
{
//           0,        1,        2,        3,        4,        5,        6,        7,        8,        9
//  0             in(wt)    in(mr)    ft(wt)    mm(wt)    mm(mr)    lb/in2       bar      mbar     g/cm2
             1,     1148,     1056,     1154,     1151,     1158,        0,     1137,     1138,     1144,
// 10   kg/cm2        Pa       kPa      torr       atm   ft3/min   gal/min     l/min  igal/min      m3/h
          1145,     1130,     1133,     1139,     1140,     1357,     1363,     1352,     1368,     1349,
// 20     ft/s       m/s     gal/s   Mgal/dy       l/s     Ml/dy     ft3/s    ft3/dy      m3/s     m3/dy
          1067,     1061,     1365,     1366,     1351,     1355,     1356,     1359,     1347,     1350,
// 30  igal/hr   igal/dy        °C        °F        °R         K        mV       Ohm        Hz        mA
          1369,     1370,     1001,     1002,     1003,     1000,     1243,     1281,     1077,     1211,
// 40      gal         l      igal        m3        ft         m       bbl        in        cm        mm
          1048,     1038,     1049,     1034,     1018,     1010,     1051,     1019,     1012,     1013,
// 50      min         s         h        dy       cSt        cP        uS         %         V        pH
          1058,     1054,     1059,     1060,     1164,     1162,     1290,     1342,     1240,     1422,
// 60        g        kg         t        lb       sht       lgt     mS/cm     uS/cm         N        Nm
          1089,     1088,     1092,     1094,     1095,     1096,     1302,     1586,     1120,     1126,
// 70      g/s     g/min       g/h      kg/s    kg/min      kg/h     kg/dy     t/min       t/h      t/dy
          1318,     1319,     1320,     1322,     1323,     1324,     1325,     1327,     1328,     1329,
// 80     lb/s    lb/min      lb/h     lb/dy   sht/min     sht/h    sht/dy     lgt/h    lgt/dy      dkth
          1330,     1331,     1332,     1333,     1335,     1336,     1337,     1340,     1341,     1184,
// 90      sgu     g/cm3     kg/m3    lb/gal    lb/ft3      g/ml      kg/l       g/l    lb/in3   sht/yd3
          1114,     1100,     1097,     1108,     1107,     1104,     1103,     1105,     1106,     1109,
//100    °twad     °brix   °bm(hv)   °bm(li)      °API  %sol/wgt  %sol/Vol      °bal   prf/Vol  prf/mass
          1110,     1426,     1111,     1112,     1113,     1343,     1344,     1427,     1428,     1429,
//110   bushel       yd3       ft3       in3      in/s    in/min    ft/min       °/s     rev/s   rev/min
          1050,     1044,     1043,     1042,     1066,     1069,     1070,        0,     1084,     1085,
//120      m/h   (n)m3/h    (n)l/h stft3/min   bbl-liq        oz      ftlb        KW       KWh        hp
          1063,     1524,     1534,     1360,     1052,     1093,     1185,     1190,     1179,     1198,
//130    ft3/h    m3/min     bbl/s   bbl/min     bbl/h    bbl/dy     gal/h    igal/s       l/h       ppm
          1358,     1348,     1371,     1372,     1373,     1374,     1364,     1367,     1353,     1423,
//140   Mcal/h      MJ/h     Btu/h         °         r    in(wt)      ug/l     ug/m3    % cons      Vol%
          1195,     1196,     1197,     1005,     1004,        0,     1101,        0,     1343,        0,
//150      %Sq     ft/16    ft3/lb        pF      ml/l      ul/l
          1345,        0,        0,     1249,        0,        0,        1,        1,        1,        1,
//160   %Plato      %lel      Mcal      kOhm        MJ       Btu     (n)m3      (n)l     stft3       ppb
          1346,        0,     1182,     1284,     1172,     1183,     1521,     1531,     1053,     1424,
//170   cm(wt)     m(wt)    cm(mr)    lb/ft2       hPa    lb/in2     kg/m2    ft(wt)    ft(wt)     m(mr)
             0,        0,        0,        0,     1136,        0,        0,     1153,        0,        0,
//180
             1,        1,        1,        1,        1,        1,        1,        1,        1,        1,
//190
             1,        1,        1,        1,        1,        1,        1,        1,        1,        1,
//200
             1,        1,        1,        1,        1,        1,        1,        1,        1,        1,
//210
             1,        1,        1,        1,        1,        1,        1,        1,        1,        1,
//220
             1,        1,        1,        1,        1,        1,        1,        1,        1,        1,
//230                                                     gal/dy         hl      MPa    in(wt)    mm(wt)
             1,        1,        1,        1,        1,     1365,      1041,    1132,     1147,     1050,
//240
             1,        1,        1,        1,        1,        1,        1,        1,        1,        1,
//250
             1,        1,        1,        1,        1,        1
};



/******************************************************************************/

FUNCTION PUBLIC USIGN16 conv_unit_hart2ff
  (
    IN USIGN8   hart_unit
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
    Converts HART unit codes to FF unit codes.

PARAMETERS:
    hart_unit:          unit code of HART

GLOBALS_AFFECTED:
    HART_2_FF_units:    code coversion table

RETURN_VALUES:
    0 =>                conversion not possible
    1 =>                invalid HART unit code
    ff_unit =>          corresponding FF unit code

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

    return (HART_2_FF_units[hart_unit]);
} /* FUNCTION conv_unit_hart2ff */


/******************************************************************************/

FUNCTION PUBLIC USIGN8 conv_unit_ff2hart
  (
    IN USIGN16   ff_unit
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
    Converts FF unit codes to HART unit codes.

PARAMETERS:
    ff_unit:            unit code of FF

GLOBALS_AFFECTED:
    HART_2_FF_units:    code coversion table

RETURN_VALUES:
    0 =>                conversion not possible
    hart_unit =>        corresponding HART unit code

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
    int i;

FUNCTION_BODY
    for (i=0; i<sizeof(HART_2_FF_units); i++)
    {
        if ( HART_2_FF_units[i] == ff_unit )
        {
            break;      /* Corresponding HART unit detected */
        }
    } /* end for i */
    if ( i == sizeof(HART_2_FF_units) )
    {
        i = 0;          /* Searching did not succeed        */
    }

    return (i);
} /* FUNCTION conv_unit_ff2hart */


/******************************************************************************/

FUNCTION PUBLIC int packed_ascii_to_string
  (
    IN  const USIGN8 *  pPA,
    OUT STRINGV *       str,
    IN  int             n
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
    Converts a packed ascii into a (not zero terminated!) string.

PARAMETERS:
    pPA:                pointer to packed ascii string
    str:                pointer to buffer to put unpacked string in
    n:                  size of packed ascii in bytes (not in characters)

GLOBALS_AFFECTED:
    none

RETURN_VALUES:
    size of string
------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
    USIGN32 i_3ByteBuffer;
    int i, string_size;

FUNCTION_BODY
    string_size = ((n*4)/3);
    str += string_size - 1;   // set str pointer to last char

    while (n > 0)
    {
        n -= 3;

            /* copy last three bytes into an USIGN32 variable as 24-bit register */
            /*-------------------------------------------------------------------*/
        i_3ByteBuffer = (((USIGN32)(pPA[n])) << 16) + (((USIGN32)(pPA[n+1])) << 8 ) + (USIGN32)(pPA[n+2]);

        for (i=4; i>0; i--)     /* repeat 4 times: 4 chars in 3 bytes   */
        {
            *str = (i_3ByteBuffer & 0x3f) + (( (USIGN8)i_3ByteBuffer & 0x20) == 0 ? 0x40 : 0x00);
            i_3ByteBuffer >>= 6;
            str--;
        } /* end for i */
    } /* end while */

    return (string_size);
} /* FUNCTION packed_ascii_to_string */


/******************************************************************************/

FUNCTION PUBLIC int string_to_packed_ascii
  (
    IN  const STRINGV *    str,
    OUT USIGN8 *           packed_ascii,
    IN  int                n
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
    Converts a string into packed ascii.

PARAMETERS:
    str:                pointer to buffer to put unpacked string in
    packed_ascii:       pointer to packed ascii string
    n:                  size of packed ascii in bytes (not in characters)

GLOBALS_AFFECTED:
    none

RETURN_VALUES:
    none

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
    BOOL end_of_string = FALSE;
    USIGN32 buffer;
    char pchar;
    int i, return_value;


FUNCTION_BODY

    return_value = n;

        /* Init packed string to '???...' */
        /*--------------------------------*/
    memset (packed_ascii, 0xFF, n);

    while ((n > 0) || (end_of_string != TRUE))
    {
        for (i=0; i<4; i++)
        {
            buffer <<= 6;
            pchar = toupper (*str);

            if ( end_of_string == TRUE )
            {
                pchar = 0x3f;       /* Set characters to '?'        */
            }
            else if ( pchar == 0 )
            {
                pchar = ' ';
                end_of_string = TRUE;
            }
            else if ( (pchar >= 0x20) && (pchar < 0x60) )
            {
                pchar &= ~0x40;
            }
            else
            {                       /* Character not convertible    */
                pchar = 0x3f;       /* set to '?'                   */
            }
            buffer |= pchar;
            str++;
        } /* end for i */

            /* Swap bytes when copying 24-bit register into packed-ascii string */
            /*------------------------------------------------------------------*/
        packed_ascii[0] = ((USIGN8 *) &buffer)[2];
        packed_ascii[1] = ((USIGN8 *) &buffer)[1];
        packed_ascii[2] = ((USIGN8 *) &buffer)[0];

        n -= 3;
        packed_ascii += 3;

    } /* end while */


    return (return_value);
} /* FUNCTION string_to_packed_ascii */


/******************************************************************************/

FUNCTION PUBLIC BOOL isLeapYear
  (
    IN USIGN32  year
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
    This function returns TRUE, if the year is a leap year.

PARAMETERS:
    year:               year to be evaluated.

GLOBALS_AFFECTED:
    none

RETURN_VALUES:
    TRUE =>             year is a leap year
    FALSE =>            year is not a leap year

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

    if(year % 4)
    {
      return FALSE;         /* Not multiple of 4        */
    }

    if(year % 100)
    {
      return TRUE;          /* Not multiple of 100      */
    }

    if(year % 400)
    {
      return FALSE;         /* Not multiple of 400      */
    }

    return TRUE;
} /* FUNCTION isLeapYear */


/******************************************************************************/

FUNCTION PUBLIC USIGN8 DayOfWeek
  (
    IN USIGN8   d,
    IN USIGN8   m,
    IN USIGN32  y
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
    This function evaluates the day of a week (1=monday,...,7=sunday).

PARAMETERS:
    d:                  day of month (1 .. 31).
    m:                  month of year (1=january ... 12=december)
    y:                  year

GLOBALS_AFFECTED:
    none

RETURN_VALUES:
    1 =>                monday
    2 =>                tuesday
    3 =>                wednesday
    4 =>                thursday
    5 =>                friday
    6 =>                saturday
    7 =>                sunday

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
    USIGN16 i;
    USIGN32 days = 0;

FUNCTION_BODY

    for(i=1900; i< (USIGN16) y; i++)
    {
        days += (isLeapYear(i)) ? 366 : 365;
    } /* end for i */

    days += DaysOfMon[m-1];
    if ( (isLeapYear(y)) && (m>2) )
    {
        days++;
    }

    days += d-1;

    return ((USIGN8) (days % 7)+1);
} /* FUNCTION DayOfWeek */


/******************************************************************************/

FUNCTION PUBLIC VOID pkstr_swap
  (
    INOUT USIGN8 *  pChar
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
    This function changes the byte ordering of an USIGN24 value.

PARAMETERS:
    pChar:              pointer to a USIGN24 value

GLOBALS_AFFECTED:
    none

RETURN_VALUES:
    none

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
    USIGN8 ch;

FUNCTION_BODY
    ch = pChar[0];
    pChar[0] = pChar[2];
    pChar[2] = ch;

    return;
} /* FUNCTION pkstr_swap */


/******************************************************************************/

FUNCTION PUBLIC VOID str_swap
  (
    INOUT USIGN8 *  pChar
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
    This function changes the byte ordering of an USIGN32 value.

PARAMETERS:
    pChar:              pointer to a USIGN32 value

GLOBALS_AFFECTED:
    none

RETURN_VALUES:
    none

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
    USIGN8 ch;

FUNCTION_BODY
    ch = pChar[0];
    pChar[0] = pChar[3];
    pChar[3] = ch;

    ch = pChar[1];
    pChar[1] = pChar[2];
    pChar[2] = ch;

    return;
} /* FUNCTION str_swap */


/******************************************************************************/

FUNCTION PUBLIC void mkPackedTimeFromString
  (
    OUT USIGN8 *    pPA,
    IN  USIGN8 *    str
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
    Converts a FF Time format variable into the HART PackedTime format

PARAMETERS:
    pPA:                pointer to variable in HART PackedTime format
    str:                pointer to variable in FF Time format

GLOBALS_AFFECTED:
    none

RETURN_VALUES:
    none

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
    USIGN32 ulTime=0;
    USIGN8 *pStr = str, *pVal=(USIGN8*)&ulTime;

FUNCTION_BODY
    ulTime  = (USIGN32)(pStr[0]&0x3F)<<26;
    ulTime |= (USIGN32)(pStr[1]&0x3F)<<20;
    ulTime |= (USIGN32)(pStr[2]&0x3F)<<14;
    ulTime |= (USIGN32)(pStr[3]&0x3F)<<8;


    *pPA++ = pVal[3];
    *pPA++ = pVal[2];
    *pPA++ = pVal[1];

    pStr += 4;

    ulTime  = (USIGN32)(pStr[0]&0x3F)<<26;
    ulTime |= (USIGN32)(pStr[1]&0x3F)<<20;
    ulTime |= (USIGN32)(pStr[2]&0x3F)<<14;
    ulTime |= (USIGN32)(pStr[3]&0x3F)<<8;

    *pPA++ = pVal[3];
    *pPA++ = pVal[2];
    *pPA++ = pVal[1];

    return;
} /* FUNCTION mkPackedTimeFromString */


/******************************************************************************/

FUNCTION PUBLIC VOID mkTimeFromPacked
  (
    IN USIGN8   *pPA,
    OUT USIGN8  *str
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
    Converts a HART PackedTime format variable into the FF Time format

PARAMETERS:
    pPA:                pointer to variable in HART PackedTime format
    str:                pointer to variable in FF Time format

GLOBALS_AFFECTED:
    none

RETURN_VALUES:
    none

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
    USIGN8 *ppstr;    //packed
    USIGN8 *pstr;  // not packed


FUNCTION_BODY
    for (ppstr = &pPA[6], pstr = &str[4]; ppstr > pPA; pstr -= 4, ppstr -= 3)
    {
        unsigned long string = 0;
        USIGN8 *ppChar = ppstr-3;

        memcpy(&string,ppChar,3);
        pkstr_swap((USIGN8*)&string);

        //        //byte1          // byte2                 // byte3                   // byte4
        string = (string & 0x0F) | ((string & 0x03C0)<<2) | ((string & 0x00F000)<<4) | ((string & 0x3C0000)<<6);

        str_swap((USIGN8*)&string);

        *(USIGN32*)pstr = string;
    } /* end for ppstr, pstr */

    return;
} /* FUNCTION mkTimeFromPacked */


/******************************************************************************/

FUNCTION PUBLIC VOID MkFFDate7
  (
    OUT USIGN8  *d,
    IN USIGN8   *s
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
    This function converts the HART date into the FF date format.

PARAMETERS:
    d:                  (destination) pointer to FF date
    s:                  (source) pointer to HART date

GLOBALS_AFFECTED:
    none

RETURN_VALUES:
    none

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  USIGN8 y=s[2], m=s[1], day=s[0], min, h;
  USIGN16 ms;
  USIGN8 buf[10];

FUNCTION_BODY

    mkTimeFromPacked(&s[3],buf);

    h   =  buf[0]*10 + buf[1];
    min =  buf[3]*10 + buf[4];
    ms  = (buf[6]*10 + buf[7])*1000;

    memcpy(d,&ms,2);
    d[2] = min;
    d[3] = h;

    d[4] = day | (DayOfWeek(day,m,y+1900)<<5);
    d[5] = m;
    d[6] = y % 100;

    return;
} /* FUNCTION MkFFDate7 */


/******************************************************************************/

FUNCTION PUBLIC VOID MkHartDate7
  (
    OUT USIGN8  *d,
    IN USIGN8   *s
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
    This function converts the FF date into the HART date format.

PARAMETERS:
    d:                  (destination) pointer to HART date
    s:                  (source) pointer to FF date

GLOBALS_AFFECTED:
    none

RETURN_VALUES:
    none

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
  USIGN8 h=s[3], min=s[2], buf[15];
  USIGN16 sec;

FUNCTION_BODY

    memcpy(&sec,s,2);
    sec = (sec+500)/1000;   /* add 500 to round up ms -> sec    */

    d[0] = s[4] & 0x1F;
    d[1] = s[5] & 0x3F;
    d[2] = (s[6] & 0x7F) + 100;


    buf[0] = h/10+'0';
    buf[1] = h%10+'0';
    buf[2] = ':';

    buf[3] = min/10+'0';
    buf[4] = min%10+'0';
    buf[5] = ':';

    buf[6] = sec/10+'0';
    buf[7] = sec%10+'0';
    buf[8] = 0;

    mkPackedTimeFromString(&d[3], buf);

    MkFFDate7(buf,d);

    return;
} /* FUNCTION MkHartDate7 */



