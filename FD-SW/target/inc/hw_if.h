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

FILE_NAME          hw_if.h



DESCRIPTION
This file contains the global interface of the HW component. No changes
are allowed here, since this file is included by parts of the protocol
software and thus results in fixed values in the protocol sw libraries.


=========================================================================== */
#ifndef __HW_IF__
#define __HW_IF__

#include "osif.h"

GLOBAL_DEFINES

#define USEC_PER_MSEC               1000        /* microseconds per milliseconds */

/* ----------------------------------------------------------------------------- */
/* --- Fieldbus Controller Configuration --------------------------------------- */
/* ----------------------------------------------------------------------------- */

/* --- SPC42 Section ----------------------------------------------------------- */

#ifdef FC_SPC42
//#define SPC4_BASE_ADR               0x28000     /* FB Controller address         */
/*@Dresser-Masoneilan project modified */
#define SPC4_BASE_ADR               0x64020000     /* FB Controller address         */
#define M_SPC4_MEM                  FAR_D       /* memory attribute              */
#endif /* FC_SPC42 */

/* --- FRONTIER1+ Section ------------------------------------------------------ */

#ifdef FC_FRONTIER1_PLUS
#define FR_BASE_ADR                 0x28000     /* addr. of FRONTIER1+ in memory */
#define M_FRONTIER_MEM              FAR_D       /* memory attribute              */
#define FR_WAIT
#define HW_RESET_FRONTIER_IRQ()     _BIT(FbcIntCtrl,3) = 0
#endif /* FC_FRONTIER1_PLUS */

/* --- FIND1+ and FIND1+DMA Section -------------------------------------------- */

#if defined (FIND1_PLUS) || defined (FC_FIND1P_DMA)
#define F1_BASE_ADR                 0x28000     /* Find-1+ address               */
#define M_FIND1_MEM                 FAR_D       /* memory attribute              */
#define F1_WAIT
#ifdef FC_FIND1P_DMA
  #define DLL_DMA_CHANNEL           DMA_0
#endif /* FC_FIND1P_DMA */
#define ResetF1IntrReq()            _BIT(FbcIntCtrl,3) = 0
#endif /* FIND1_PLUS, FC_FIND1P_DMA */

 /* #define FC_FIND1P_DMA -> defined in std_settings.mak */

/* --- FBC Interrupt Level Section --------------------------------------------- */
/* --- NOTE: Change in delivered OBJECT code licence has NO influence! --------- */

#define FBC_INTR_LEVEL              3           /* IRQ level of the FC chip      */
#define FDL_DMA_INTERRUPT_LEVEL     5           /* IRQ level of the F1+-DMA      */
#define DLL_DMA_INTERRUPT_LEVEL     5           /* IRQ level of the F1+-DMA      */
#define FBC_INT_ACTIVE              (_BIT(P8,4) == 0)

#define FbcIntCtrl                  INT2IC
#define ResetFbcIntrReq()           atomic_cmd(_BIT(FbcIntCtrl,3) = 0)
#define nmemcpy(dest,src,len)       hw_nmemcpy ((dest), (src), (len))


/* --- Optimization -s9 (1.36A) for specific layer 2 functions (PROFIBUS ONLY) - */
/* --- NOTE: Change in delivered OBJECT code licence has NO influence! --------- */

#ifndef PD30
#define DL_OPTIMIZE
#endif /* PD30 */


/*********************************************************************************/


EXPORT_TYPEDEFS
/* --- FBK-2 Quiescent Current Setting ------------------------------------ */
#if defined (FBK2_HW)
typedef enum {
    IQUI_10MA = 0,
    IQUI_13MA,
    IQUI_18MA,
    IQUI_26MA
} T_FBK2_IQUI;
#endif /* FBK2_HW */

IMPORT_DATA

FUNCTION_DECLARATIONS

/*****************************************************************************/
/* global function declarations from file:      hw_init.c                    */
/*****************************************************************************/

FUNCTION extern VOID init_processor (VOID);

/*----------------------------------------------------------------------------

FUNCTIONAL_DESCRIPTION

The processor is initialized.

----------------------------------------------------------------------------*/

/*****************************************************************************/
/* global function declarations from file:      hw_tim.c                     */
/*****************************************************************************/

FUNCTION extern VOID hw_tim_start_ms_timer
(
     IN USIGN8      timer_id,   /* timer identifier */
     IN BOOL        ms_unit,    /* TRUE: milliseconds, FALSE: milliseconds / 32 */
     IN USIGN32     duration    /* duration */
);
/*----------------------------------------------------------------------------

FUNCTIONAL_DESCRIPTION

The timer with the id timer_id (0 .. 4) is started. If ms_unit is TRUE the
unit of duration is one millisecond. In the other case unit is 1 / 32 milli-
seconds.

----------------------------------------------------------------------------*/

FUNCTION extern VOID hw_tim_stop_timer
(
    IN  USIGN8      timer_id
);
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

The timer with the timer_id (0 .. 4) is stopped.

----------------------------------------------------------------------------*/

FUNCTION extern VOID hw_tim_start_rti (VOID);

/*----------------------------------------------------------------------------

FUNCTIONAL_DESCRIPTION

Start the real time interrupt with rate (E / 2(pow 13)) / 8

RTR1, RTR0 set to 0x03.

----------------------------------------------------------------------------*/

FUNCTION extern VOID hw_trigger_subscr_irq (void);

/*----------------------------------------------------------------------------

FUNCTIONAL_DESCRIPTION

The function sets the interrupt request of lowest priority. This is necessary
for processing incoming subscriber data with a higher priority than the
non-interrupt level, but not within the DLL interrupt.

----------------------------------------------------------------------------*/

/*****************************************************************************/
/* global function declarations from file:      hw_io.c                      */
/*****************************************************************************/

typedef enum {
    MODE_READBACK,
    MODE_OUT
} T_OUT_MODE;


FUNCTION extern BOOL hw_get_nvram_erase_jumper (VOID);

/*----------------------------------------------------------------------------

FUNCTIONAL_DESCRIPTION

Check whether NV-RAM erase jumper is set or not.

----------------------------------------------------------------------------*/


FUNCTION extern BOOL hw_get_simulate_enable_jumper (VOID);

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

Check whether simulate enable jumper is set or not
----------------------------------------------------------------------------*/


/* Definiton of HW write protection levels in accordance with
   PA-Profile 3.02, section 5.2.8.1 --------------------------------------- */
#define HW_WRITE_LOCK_INACTIVE            0
#define HW_WRITE_LOCK_WITH_MAN_OP         1
#define HW_WRITE_LOCK_WITHOUT_MAN_OP      2
#define HW_WRITE_LOCK_DEVICE_SPECIFIC   128


FUNCTION extern USIGN8 hw_write_protection (VOID);

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function checks whether the write protection jumper is set or not.

  PROFIBUS PA devices may support multiple write protection levels

  Softing's PA Slave implementation supports three protection levels
    HW_WRITE_LOCK_WITHOUT_MAN_OP  (standard)
    HW_WRITE_LOCK_WITH_MAN_OP
    HW_WRITE_LOCK_DEVICE_SPECIFIC

  Softing's FF Field Device implementation supports only the
    HW_WRITE_LOCK_WITHOUT_MAN_OP  (standard)
  protection level.

    HW_WRITE_LOCK_INACTIVE indicates unlimited access to contained parameters.
----------------------------------------------------------------------------*/


#if defined (PROTOCOL_PA)

#define INVALID_SLAVE_ADDRESS     255u
#define SLAVE_ADDRESS_SWITCH_MASK 0x7F
#define SLAVE_USE_ADDRESS_SWITCH  0x80

FUNCTION extern USIGN8 hw_io_get_addr_switches (VOID);
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

  Read the address value from the hardware address switch. In case the address
  setting should work over HW address switch, the highest bit has to be 1. If
  the highest bit is 0, the HW address setting is not active. Then only the
  software address setting is active.

----------------------------------------------------------------------------*/
#endif /* PROTOCOL_PA */

FUNCTION extern USIGN16 calc_crc
(
    IN  USIGN8 *binStream,          /* pointer to data array                */
    IN  USIGN16 nrBytes             /* size of data array in bytes          */
);
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

This function calculates the checksum from an array of bytes adressed by
'binStream' with size 'nrBytes'.

RETURN_VALUE
    The checksum as calculated

----------------------------------------------------------------------------*/

/*@Dresser-Masoneilan project modified: No HUGE memory model type with 32bit addressing */
FUNCTION extern USIGN16 calc_crc_dwnld
(
    //IN  USIGN8 huge *binStream,          /* pointer to data array                */
    IN  USIGN8  *binStream,          /* pointer to data array                */
    IN  USIGN32 nrBytes              /* size of data array in bytes          */
);



FUNCTION extern USIGN32 hw_io_generate_fbc_clock (VOID);

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

Enables the FBC chip clock supply and releases the FBC RESET line.


RETURN_VALUE
    The frequency of the FBC clock in Hz

----------------------------------------------------------------------------*/


FUNCTION extern VOID hw_io_stop_fbc_clock (VOID);

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:

    Disables the FBC chip clock supply and asserts the FBC RESET line.


PARAMETERS:
    none

GLOBALS_AFFECTED:

RETURN_VALUES:
    none

----------------------------------------------------------------------------*/


FUNCTION extern USIGN32 hw_io_get_cpu_freq (VOID);

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

Provides the current effective CPU clock frequency.


RETURN_VALUE
    The frequency of the effective CPU clock in Hz

----------------------------------------------------------------------------*/

#if defined (PROTOCOL_FF)

FUNCTION extern VOID device_in_token_ring (VOID);

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

Indicates that the device is in the token ring.
----------------------------------------------------------------------------*/



FUNCTION extern VOID device_not_in_token_ring (VOID);

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

Indicates that the device is not in the token ring.
----------------------------------------------------------------------------*/

#endif /* PROTOCOL_FF */


FUNCTION extern VOID hw_io_start_wd_timer (VOID);

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

Starts the HW watchdog timer at system start
----------------------------------------------------------------------------*/



FUNCTION extern VOID hw_io_reset_wd_timer (VOID);

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

Reset the HW watchdog timer
----------------------------------------------------------------------------*/



FUNCTION extern USIGN8 hw_io_di1 (VOID);

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
    Reads discrete input line 1 (located at M16C pin 7.4)

PARAMETERS:
    none

GLOBALS_AFFECTED:

RETURN_VALUES:
    0   =>  if port has low level
    1   =>  if port has high level

----------------------------------------------------------------------------*/


FUNCTION extern USIGN8 hw_io_di2 (VOID);

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
    Reads discrete input line 2 (located at M16C pin 7.6)

PARAMETERS:
    none

GLOBALS_AFFECTED:

RETURN_VALUES:
    0   =>  if port has low level
    1   =>  if port has high level

----------------------------------------------------------------------------*/


FUNCTION extern USIGN8 hw_io_do1 (USIGN8 dout, T_OUT_MODE mode);

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
    Handles discrete output line 1 (located at M16C pin 7.5)

PARAMETERS:
    dout:   logic level to be set
    mode:   if set to MODE_OUT, the dout value is forwarded to the port pin

GLOBALS_AFFECTED:

RETURN_VALUES:
    0   =>  if port has low level
    1   =>  if port has high level

----------------------------------------------------------------------------*/


FUNCTION extern USIGN8 hw_io_do2 (USIGN8 dout, T_OUT_MODE mode);

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
    Handles discrete output line 2 (located at M16C pin 7.7)

PARAMETERS:
    dout:   logic level to be set
    mode:   if set to MODE_OUT, the dout value is forwarded to the port pin

GLOBALS_AFFECTED:

RETURN_VALUES:
    0   =>  if port has low level
    1   =>  if port has high level

----------------------------------------------------------------------------*/


#if defined (FBK2_HW)
FUNCTION extern BOOL hw_io_set_current
  (
    IN T_FBK2_IQUI  current,
    IN BOOL         wait_after_set
  )
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
  Sets the rated current consumption of the FBK-2.
  CAUTION! Improper setting may affect operation of the FBK-2 or even the
  whole H1 link!

PARAMETERS:
    current:        code for the current setting
    wait_after_set: wait after setting to allow current to stabilize at new value

GLOBALS_AFFECTED:
    FBK2_current:   global variable to store the value as set


RETURN_VALUES:
    FALSE =>        invalid parameter value, current setting not changed
    TRUE =>         current was set successfully

------------------------------------------------------------------------------*/
;
#endif /* FBK2_HW */

FUNCTION extern VOID HardReset_CPU(VOID);

/* @Dresser-Maisoneilan Project: Reset the external Watchdog */
FUNCTION extern VOID Reset_Watchdog (VOID);
extern void Reset_CPU(void);

/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

Executes a software reset
------------------------------------------------------------------------------*/


#if defined(COMPILER_IAR)
  #if __VER__ >= 200
    #ifdef NEVER /* inline=forced does not work yet */
      #pragma inline=forced
    #endif /* NEVER */
  #endif /* __VER__ */
#endif /* COMPILER_IAR */
FUNCTION extern VOID hw_init_set_fbc_isr_clk (VOID);


#if defined(COMPILER_IAR)
  #if __VER__ >= 200
    #ifdef NEVER /* inline=forced does not work yet */
      #pragma inline=forced
    #endif /* NEVER */
  #endif /* __VER__ */
#endif /* COMPILER_IAR */
FUNCTION extern VOID hw_init_set_default_clk (VOID);


FUNCTION extern VOID hw_tim_start_timer
(
    IN USIGN8   timer_id,   /* timer identifier */
    IN USIGN16  duration    /* duration */
);


FUNCTION extern VOID hw_tim_stop_timer
(
    IN  USIGN8  timer_id
);


/*@Dresser-Masoneilan project modified: not usable here */
#ifdef M16C
#if defined(COMPILER_IAR)
  #if __VER__ >= 300   /* Required only for ICCM16C 3.x */
    #pragma type_attribute=__simple
  #endif /* __VER__ */
#endif /* COMPILER_IAR */
FUNCTION extern GLOBAL VOID hw_nmemcpy
(
    VOID NEAR_D *   destination,
    VOID *          source,
    USIGN16         length
);
#endif



#ifdef USE_SEC_EEPROM /* ------------------------------------------------ */

FUNCTION GLOBAL VOID hw_io_cfg_se_timer (VOID)
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:

------------------------------------------------------------------------------*/
;


FUNCTION GLOBAL VOID hw_io_se_delay_us
  (
    IN USIGN16 Delay
  );

/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:

------------------------------------------------------------------------------*/
;

#endif /* USE_SEC_EEPROM */



#if defined (HART_MASTER)

/*****************************************************************************/
/* global function declarations from file:      hw_hm.c                      */
/*****************************************************************************/

FUNCTION extern VOID hw_hm_init(VOID);
FUNCTION extern USIGN8 hw_hm_set_baudrate (USIGN16 baudrate);

FUNCTION extern bool_t Remote_WP_Jumper(VOID); //required interface - see ffbl_res.c
FUNCTION extern bool_t Lui_Simulation(VOID); //required interface - see ffbl_res.c

#endif /* HART_MASTER */

#endif /* __HW_IF__ */
