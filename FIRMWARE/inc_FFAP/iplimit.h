/*
Copyright 2008 by Dresser, Inc., as an unpublished trade secret.  All rights reserved.

This document and all information herein are the property of Dresser, Inc.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon
demand.
*/

/**
    \file iplimits.h
    \brief Header for Control.c with SVI2AP H/W defined values.

    CPU: Philips LPC21x4 (ARM)

    OWNER: Anatoly Podpaly

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/inc_SVI2AP/iplimit.h $
    $Date: 1/12/12 2:22p $
    $Revision: 2 $
    $Author: Anatoly Podpaly $
*/

/*
    $History: iplimit.h $
 *
 * *****************  Version 2  *****************
 * User: Anatoly Podpaly Date: 1/12/12    Time: 2:22p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_SVI2AP
 * TFS:8576 -- corrected this file for the SVI2AP project.
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 12/05/11   Time: 4:48p
 * Created in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_SVI2AP
 * TFS:8347
 *
 * *****************  Version 5  *****************
 * User: Anatoly Podpaly Date: 4/14/11    Time: 1:27p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * TFS:4870 --  corrected Low Power Limiting parameters.
 *
 * *****************  Version 4  *****************
 * User: Anatoly Podpaly Date: 2/10/11    Time: 3:11p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * TFS:4870 -- Corrected the values.
 *
 * *****************  Version 3  *****************
 * User: Anatoly Podpaly Date: 2/09/11    Time: 4:11p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * TFS 4870 -- corrections to the file for the IP limiting issue.
 *
 * *****************  Version 2  *****************
 * User: Anatoly Podpaly Date: 2/08/11    Time: 2:21p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * TFS:5576 -- corrected:
 * -- Tombstone header
 * -- Conditional compilation for LCX
 * -- Resistor R329 value
*/

/*
    This header file and associated code is used to determine upper limits for
    the PWM.  This comes into play under two circumstances:

        1. Loop current provided to the device may be insufficient to allow full
            IP current output.  e.g 3.2 - 3.9 ma loop current.
        2. Limit integral windup in the case where the IP is unplugged.

    Theory of operation:

    The hardware pressure loop controller combines pressure feedback from the
IP output (Pilot Pressure) with the PWM (DA) output.  The net effect is that
the Pilot Pressure becomes(nearly) a linear function of the PWM value.  The
hardware controller regulates IP current to achieve this function.  Overall this
counteracts undesirable charactertistics of the IP and thus significantly
improves step response of the positioner. The controller function is nominally:

        PWM - Pilot = IPOut

    In order to limit IPOut to the desired value the equation is reordered:

        PWM = IPOut + Pilot

    Since none of the above are in the same units, there are also conversion
factors (gains) involved.  The process is further complicated by having the
coefficients stored in NVRAM.  Generating the approproate integer values,
storing them in NVRAM and applying them is what this code is all about.

    Note that all compile-time computations are floating point and are converted
to integer for use at run time.

    Testing:

    1. IP Current limiting WRT Loop Current:

        a. Make sure DUT has correct coefficients loaded to NVRAM
        b. Place an operational device in Normal mode
        c. Set the position about 50% (loop current approx 12 ma)
        d. Connect a DC voltmeter from the IP- lead to device GND
        e. The voltage should be approximately .250 for an AP (?? LCX?)
        f. Remove the air supply and wait for the actuator to lose position.
        g. The voltage should be approx .450 for an AP (.870 for LCX)
        h. Above 3.95 ma loop current the voltage should remain constant.
        i. The voltage should behave linearly between 3.9 ma and 3.2 ma
            with the voltage at max near 3.9ma and 0 near 3.2ma

    2. Integral limiting


*/

#ifndef IP_LIMIT_H_
#define IP_LIMIT_H_

//lint ++flb hardware definitions
#define F2U(x)              (u32)((x) + .5)     // round and convert to unsigned integer
#define DIVIDER(R1, R2)     (R1 / (R1 + R2))    // returns ratio of R1 drop
#define CEIL_SHIFT_VALUE    (u8)8u
#define COEFF_SCALE         (cdouble_t)(1u << CEIL_SHIFT_VALUE)

// Note that FFAP matches SVI 2 AP ROHS board!
// Also note that on FFAP schematcs the Rabc nomenclature is DIFFERENT!
// Add 100 to the R number to get the R on the schematics
//   R330, for example is R430 on the schematics!
// That said, the register values were verified!
//
// platform selection
#define DBLACT      0
#define ROHS        1
#define LCX         0
//  #define LCX_PROTO_A       0
//  #define LCX_PROTO_B       0

#define AP_ESD      0   // LCX Prototype is the same as AP_ESD

// Going forward AP_ESD should be replaced by ROHS for both Single and Double-Acting

CONST_ASSERT((DBLACT + ROHS + LCX + AP_ESD) == 1);  // only one platform allowed

// Hardware constants
#if LCX                     // LCX production hardware
 // resistors in PWM MSB D/A function
 #define R311      47500.0
 #define R313     210000.0
 #define R338     121000.0
 #define R315     332000.0
 #define R316    1000000.0
 // resistors in PWM LSB D/A function
 #define R312      47500.0
 #define R314     210000.0
 #define R318    1000000.0
 // Correction per EP
 // #define R339        150.9
 #define R339        150.0

 #define R317       1210.0

 // resistors of interest in pressure loop and I/P output
 // Corrected based on SVi1000 schematics
 #if LCX_PROTO_A
   #define R329      33200.0    // press loop pilot input
 #else
   #define R329      18200.0    // press loop pilot input
 #endif
                              //
 #define R330     100000.0    // press loop pilot feedback
 #define R335        365.0    // I/P current sense resistor

#else // ROHS | DBLACT | MNCB(AP/ESD and LCX-proto)

 // resistors in PWM MSB D/A function
 #define R311      47500.0
 #define R313     210000.0
 #define R338      20000.0
 #define R315     511000.0
 #define R316    1000000.0
 // resistors in PWM LSB D/A function
 #define R312      47500.0
 #define R314     210000.0
 #define R318    1000000.0
 #define R339         90.9
 #define R317       1210.0

 // resistors of interest in pressure loop and I/P output
 #define R329      35700.0    // press loop pilot input
 #define R335        267.0    // I/P current sense resistor

 #if ROHS
  #define R330    100000.0    // press loop pilot feedback
 #elif DBLACT
  #define R330     75000.0    // press loop pilot feedback
 #elif AP_ESD
  #define R330    121000.0    // press loop pilot feedback
 #else
  #error No R330 here
 #endif
#endif

// resistors in signal sensor path
#define R202	        10.0        // current sense resistor
#define R241	        3010.0      // gain resistor 1
#define R203	        27400.0     // gain resistor 2
#define R216            301000.0    // gain feedback resistor

// some conversion constants
#define MAX_AD     		65535.0     // max count from A/D chip
#define MAX_PWM    		65535.0     // max count from A/D chip
#define AD_REF_VOLTAGE	2.5      	// will give MAX_AD counts
#define PWM_REF       	3.0       	// PWM reference voltage

// some combinatorials
#define R_EQUIV_1	(R311 + R313 + R338 + R315)
#define R_EQUIV_2	(R312 + R314 + R318)
#define R_EQUIV_3	(R339 + R317)

// end hardware constants

// Limit constants
#if LCX
// #define IP_MAX_CUR         .0022   // in amps LCX IP current limit at SIG_MARGINAL_HIGH
 #define IP_MAX_CUR         .00225    // in amps LCX IP current limit at SIG_MARGINAL_HIGH
 #define SIG_MARGINAL_HIGH  .004    // in amps - point at which limiting begins
 // #define SIG_MARGINAL_HIGH  .0039    // in amps - point at which limiting begins
#else
 #define IP_MAX_CUR         .0016   // in amps (AP/ESD) IP current limt at SIG_MARGINAL_HIGH
 #define SIG_MARGINAL_HIGH  .0039   // in amps - point at which limiting begins
#endif

#define IP_LOW_CURRENT     (.000150)     // in amps (150 microamps)
#define IP_TRIP_CURRENT    (.000051)     // in amps

// marginal power constants
#define SIG_MARGINAL_LOW    .0032   // "       - point at which IP current is 0
#define SIG_MARGINAL_THRESH .00375  // "
#define SIG_HYSTERESIS      .00005  // "
#define SIG_MARGINAL_RANGE  (SIG_MARGINAL_HIGH - SIG_MARGINAL_LOW)

// end limit constants

// Derived PWM factors
#define PWM_MSB_DIVIDER	  		DIVIDER(R316, R_EQUIV_1)
#define IC306A_GAIN_MINUS		(R_EQUIV_3 / R_EQUIV_2)
#define IC306A_GAIN_PLUS		(IC306A_GAIN_MINUS + 1.0)

//#define PWM_RATIO ((PWM_MSB_DIVIDER * IC306A_GAIN_PLUS) / IC306A_GAIN_MINUS)

#define PWM_GAIN                (((PWM_REF / MAX_PWM) * PWM_MSB_DIVIDER) * IC306A_GAIN_PLUS)
#define PWM_FULLSCALE_VOLTAGE   (PWM_GAIN * MAX_PWM)
#define PWM_TO_IP_FACT          ((PWM_GAIN * IC306B_GAIN_PLUS) / R335)
#define IP_CUR_TO_PWM_COUNTS(x) ((x) / PWM_TO_IP_FACT)

// derived pressure loop comparator gains
#define IC306B_GAIN_MINUS		(R330 / R329)
#define IC306B_GAIN_PLUS		(IC306B_GAIN_MINUS + 1.0)
#define PRESSURE_DIV            DIVIDER(R330, R329)
#define PRESSURE_COEF	        (((PRESSURE_DIV * AD_REF_VOLTAGE) / PWM_FULLSCALE_VOLTAGE) * COEFF_SCALE)

// signal ma to counts
#define IC200_GAIN_MINUS		(R216 / (R241 + R203))
#define SIGNAL_AMPLITUDE(x)		(((x) * R202) * IC200_GAIN_MINUS)
#define SIGNAL_TO_COUNTS(x)  	((SIGNAL_AMPLITUDE(x) / AD_REF_VOLTAGE) * MAX_AD)

// IP current to counts
#define IP_CUR_TO_AD_FACTOR     ((MAX_AD * R335) / AD_REF_VOLTAGE)
#define IP_CUR_TO_COUNTS(x)     ((x) * IP_CUR_TO_AD_FACTOR)
#define TRIP_THRESHOLD          IP_CUR_TO_COUNTS(IP_TRIP_CURRENT)

// signal range to PWM conversion factor
#define POWER_SLOPE             ((IP_CUR_TO_PWM_COUNTS(IP_MAX_CUR) / SIGNAL_TO_COUNTS(SIG_MARGINAL_RANGE)) * COEFF_SCALE)

// constant for limiting integral if I/P is unplugged
#define MIN_IP_IN_SIG_UNITS ((SIGNAL_TO_COUNTS(SIG_MARGINAL_RANGE) / IP_MAX_CUR) * IP_LOW_CURRENT)

//lint --flb end hardware definitions

#endif //IP_LIMIT_H_

/* This line marks the end of the source */
