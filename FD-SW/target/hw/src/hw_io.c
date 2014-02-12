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

FILE_NAME          hw_io.c



FUNCTIONAL_MODULE_DESCRIPTION

This modul contains the IO handler.

        getchar - get a character from the SCI, it is used for printf and
        putchar - put a character to the SCI, it is used for scanf.

=========================================================================== */
#include "keywords.h"   /* keyword defines */
#define  MODULE_ID      (COMP_HW + MOD_HWIO)

INCLUDES

#include <stdio.h>

#include "base.h"
#include "except.h"
#include "hw_if.h"
#include "hw_cfg.h"

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"


LOCAL_DEFINES

  /* @Dresser-Masoneilan project: Use the external HW Watchdog */
  #define EXTERNAL_HW_WATCHDOG

LOCAL_TYPEDEFS

FUNCTION_DECLARATIONS

EXPORT_DATA

IMPORT_DATA

  extern RCC_ClocksTypeDef RCC_ClockFreqs;

LOCAL_DATA



  STATIC USIGN8 se_cnt_psc;




FUNCTION GLOBAL BOOL hw_get_nvram_erase_jumper (VOID)

/*----------------------------------------------------------------------------

FUNCTIONAL_DESCRIPTION

Check whether NV-RAM erase jumper is set or not.

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
    BOOL ret_val;
    volatile int i;

FUNCTION_BODY

    ret_val = FALSE;

#if defined (BFD_HW)
  #define NV_ERASE_INPUT _BIT(P7,3)
#elif defined (FBK_HW)
  #define NV_ERASE_INPUT _BIT(P7,7)
#elif defined (CUSTOMER_HW)
  /*@Dresser-Masoneilan project modified: NVErase on GPIO B Pin 13*/
  #define NV_ERASE_INPUT  (GPIOB->IDR & 0x00002000)
#else
  #error Hardware platform not defined properly
#endif /* BFD_HW, FBK_HW */

    if ( NV_ERASE_INPUT == 0 )          /* Test if control pin is connected to GND  */
    {
        for (i=0; i<1000; i++);
        if ( NV_ERASE_INPUT == 0 )      /* Reread pin to elimitate ESD distortions  */
        {
            for (i=0; i<1000; i++);
            if ( NV_ERASE_INPUT == 0 )  /* Reread pin to elimitate ESD distortions  */
            {
                ret_val = TRUE;
            }
        }
    }

    return ret_val;
}/* END of hw_get_nvram_erase_jumper */


FUNCTION GLOBAL BOOL hw_get_simulate_enable_jumper (VOID)

/*----------------------------------------------------------------------------

FUNCTIONAL_DESCRIPTION

Check whether simulate enable jumper is set or not

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

#if defined (BFD_HW)
    if ( _BIT(P7,5) == 0)  /* test if P7.4 and P7.5 are connected <=> simulation enabled */
#elif defined (FBK_HW)
    if ( _BIT(P7,5) == 0 )      /* test if P7.5 is connecte to GND      */
#elif defined (CUSTOMER_HW)
    /*@Dresser-Masoneilan project modified: HW Simulate Jumper on GPIO B Pin 6*/
    if ( (GPIOB->IDR & 0x00000040) == 0 )
#else
  #error Hardware platform not defined properly
#endif /* BFD_HW, FBK_HW */
  return TRUE;         /* jumper is set */
else
  return FALSE;

}/* END of hw_get_simulate_enable_jumper */


/****************************************************************************/

FUNCTION GLOBAL USIGN8 hw_write_protection (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION
  This function checks whether the write protection jumper is set or not.

  PROFIBUS PA devices may support multiple write protection levels

  Softing's PA Slave implementation supports three protection levels
    HW_WRITE_LOCK_WITHOUT_MAN_OP  (standard)
    HW_WRITE_LOCK_WITH_MAN_OP
    HW_WRITE_LOCK_DEVICE_SPECIFIC

  Softing's  FF Field Device implementation supports only the
    HW_WRITE_LOCK_WITHOUT_MAN_OP  (standard)
  protection level.

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

/* ---------------------------------------------------------------------------
 In accordance with the FF ITK Automation Toolkit the setting of the HW write
 protection jumper is changed in the FF FD-SW V2.41. Now, HW write protection
 is enabled when the jumper is set and HW write protection is disabled if the
 jumper is not set.
---------------------------------------------------------------------------- */

#if defined (BFD_HW)
    if ( _BIT(P7,6) == 0)       /* test if P7.6 and P7.7 are connected  */
#elif defined (FBK_HW)
    if ( _BIT(P7,6) == 0 )      /* test if P7.6 is connected to GND     */
#elif defined (CUSTOMER_HW)
    /*@Dresser-Masoneilan project modified: HW Write Protect on GPIO B Pin 7*/
    if ( (GPIOB->IDR & 0x00000080) == 0 )
#else
  #error Hardware platform not defined properly
#endif /* BFD_HW, FBK_HW */
    return HW_WRITE_LOCK_WITHOUT_MAN_OP;  /* Jumper is set, protection is enabled */
  else
#if defined (PD30) && defined (FBK_HW)
    return 0;
#else

#ifdef PROTOCOL_PA
#if defined (FBK_SK_HART_APP) || defined (FBK_SK_MODB_APP)
    /* HW Write Lock not supported for FBK2-Start */
    return 0;

#else /* any other Device Type */

    return HW_WRITE_LOCK_WITHOUT_MAN_OP;
#endif
#else /* Protocol_FF */

    return 0;  /* Jumper is not set, protection is disabled */
#endif

#endif /* PD30 */
}


/****************************************************************************/

#if defined (PROTOCOL_PA)

FUNCTION GLOBAL USIGN8 hw_io_get_addr_switches (VOID)
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

  Read the address value from the hardware address switch. In case the address
  setting should work over HW address switch, the highest bit has to be 1. If
  the highest bit is 0, the HW address setting is not active. Then only the
  software address setting is active.

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  /* In case of FBK HW we have no HW address switch  */
  /* so return an invalid slave address              */
  /*-------------------------------------------------*/

   return (127);
}

#endif /* PROTOCOL_PA */




/****************************************************************************/

FUNCTION GLOBAL USIGN16 calc_crc
(
    IN  USIGN8 *binStream,          /* pointer to data array                */
    IN  USIGN16 nrBytes             /* size of data array in bytes          */
)
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

This function calculates the checksum from an array of bytes adressed by
'binStream' with size 'nrBytes'.

Note on the NON-HARDWARE supported version:

This quick and dirty implementation produces the same results
as the M16C without using the M16C hardware registers.
the test sequence is as follows (see M16C manual, Fig. 1.22.3):

CRCD = 0x0000;
CRCIN = 0x01;
confirm CRCD == 0x1189 !
CRCIN = 0x23;
confirm CRCD == 0x0A41 !

RETURN_VALUE
    The checksum as calculated

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
    USIGN16         cs;
    USIGN32         cs_tmp;
#ifdef USE_CRC_HARDWARE
    USIGN32         crc_sav;
#else
    USIGN16         CRCD;
    USIGN8          CRCIN;
    int             i;
#endif /* USE_CRC_HARDWARE */

FUNCTION_BODY

#ifdef USE_CRC_HARDWARE
    crc_sav = CRC->DR;                     /* To provide reentrance, save CRC  */
#endif /* USE_CRC_HARDWARE */

    CRC->CR = 0x01;

    while (nrBytes--)
    {
        CRC->DR = *binStream++;           /* Write stream to CRC input reg.   */

#ifndef USE_CRC_HARDWARE
        for (i=0; i<8; i++)             /* use CRC polynom X^16+X^12+X^5+1  */
        {
            if ((CRCIN ^ CRCD) & 1)
            {
                CRCIN >>= 1;
                CRCD  >>= 1;
                CRCD  ^= 0x8408;
            }
            else
            {
                CRCIN >>= 1;
                CRCD  >>= 1;
            }
        }
#endif /* USE_CRC_HARDWARE */
    }

    cs_tmp = ~CRC->DR;

    cs = (USIGN16)((USIGN16)cs_tmp^(USIGN16)(cs_tmp>>16));

#ifdef USE_CRC_HARDWARE
    CRC->DR = crc_sav;                     /* Restore CRC data register        */
#endif /* USE_CRC_HARDWARE */

    return cs;
} /* FUNCTION calc_crc */




/****************************************************************************/

FUNCTION extern USIGN32 hw_io_generate_fbc_clock (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

Enables the FBC chip clock supply and releases the FBC RESET line.


RETURN_VALUE
    The frequency of the FBC clock in Hz

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
#ifdef FC_SPC42
  volatile USIGN16  i;
#endif

FUNCTION_BODY

#if ( (defined (FIND1_PLUS)  || defined (FC_FIND1P_DMA) || defined (FC_FRONTIER1_PLUS)) && \
      (  (CPU_FREQ == 1000000UL) \
      || (CPU_FREQ == 2000000UL) \
      || (CPU_FREQ == 4000000UL) \
      || (CPU_FREQ == 8000000UL) \
      ) ||                       \
      defined (FC_SPC42)         \
    )

#ifdef FC_SPC42
  for (i=0; i<100;i++);     /* Wait a little while after SPC4-2 clock supply    */
  FBC_RELEASE_RESET;        /* Release SPC4-2 reset pin  */
  for (i=0; i<100;i++);     /* Wait a little while after SPC4-2 start           */
#endif /* FC_SPC42 */

#if defined (FIND1_PLUS)  || defined (FC_FIND1P_DMA) || defined (FC_FRONTIER1_PLUS)
  FBC_RESET = 1;        /* Release F1 reset pin  */
#endif /* FIND1_PLUS, FC_FIND1P_DMA, FC_FRONTIER1_PLUS */

  /*@Dresser-Masoneilan project modified: SPC4-2 not clocked with CPU Frequency of 4MHz but with 2MHz */
  return (CPU_FREQ/2);  /* Return FBC frequency */
#else
    #error Valid clock generation for Find-1+ impossible !!
#endif /* CPU_FREQ */
}


/******************************************************************************/

FUNCTION GLOBAL VOID hw_io_stop_fbc_clock (VOID)
/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:
    Disables the FBC chip clock supply and asserts the FBC RESET line.

PARAMETERS:
    none

GLOBALS_AFFECTED:

RETURN_VALUES:
    none

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES
#ifdef FC_SPC42
    volatile USIGN16  i;
#endif

FUNCTION_BODY
#ifdef FC_SPC42
    FBC_ASSERT_RESET;              /* Assert SPC4-2 reset pin                  */
    for (i = 0;i < 100;i++);    /* Wait a little while after SPC4-2 stop    */
#endif

#if defined (FIND1_PLUS)  || defined (FC_FIND1P_DMA) || defined (FC_FRONTIER1_PLUS)
    FBC_RESET = 0;        /* Assert F1 reset pin  */
#endif /* FIND1_PLUS, FC_FIND1P_DMA, FC_FRONTIER1_PLUS */

    return;
} /* FUNCTION hw_io_stop_fbc_clock */


/****************************************************************************/

FUNCTION extern USIGN32 hw_io_get_cpu_freq (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

Provides the current effective CPU clock frequency.


RETURN_VALUE
    The frequency of the effective CPU clock in Hz

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  return (CPU_FREQ);  /* Return find1+ frequence */
}



#if defined (PROTOCOL_FF)

FUNCTION extern VOID device_in_token_ring (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

Indicates that the device is in the token ring.
----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  //SWITCH_TOKEN_LED_ON ();
  return;
}



FUNCTION extern VOID device_not_in_token_ring (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

Indicates that the device is not in the token ring.
----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  //SWITCH_TOKEN_LED_OFF ();
  return;
}

#endif /* PROTOCOL_FF */




#ifdef USE_SEC_EEPROM /* ------------------------------------------------------- */

FUNCTION GLOBAL VOID hw_io_cfg_se_timer (VOID)

/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  SINGLE_WIRE_STOP_TIMER;

  se_cnt_psc = TIM_GetPrescaler(TIM6)+1;

  return;
}





#pragma optimize=none
FUNCTION GLOBAL VOID hw_io_se_delay_us
  (
    USIGN16   Delay
  )

/*------------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION:

------------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

  volatile USIGN16 delay_ticks;

FUNCTION_BODY

  /* calculate timer ticks for Delay */
  delay_ticks = (USIGN16)(Delay * (RCC_ClockFreqs.PCLK1_Frequency/1000000uL) / se_cnt_psc);

  SINGLE_WIRE_SET_TIMER(delay_ticks);
  SINGLE_WIRE_START_TIMER;

  const volatile uint16_t *p = &TIM6->SR;
  while((*p) < 1U)
  {
      //nothing
  }

  SINGLE_WIRE_STOP_TIMER;

  SINGLE_WIRE_CLEAR_STATUS;

  return;
}
#endif /* USE_SEC_EEPROM */
