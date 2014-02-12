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

FILE_NAME          appl_svi_swdl.c


FUNCTIONAL_MODULE_DESCRIPTION

  This module implements the domain download handler for use with Dresser HW.
  The download handler is the counter part of the download server, which
  is a part of the System Management Agent.

  The download server implements the download state machine as specified
  in FF-883. The download handler is responsible for checking and storing
  the downloaded domain. Both, download handler and download server implement
  the software download function of a device.

  This module is intended to be an example for class 1 as well as for class 3
  software download. Its current state implements an example for class 1
  software download only.

  This module is intended to be an example for single domain software download.

=========================================================================== */
  #include "keywords.h"
  #define  MODULE_ID      (COMP_FAPP + MOD_APPL_SVI)

INCLUDES
  #include <string.h>
  #include <math.h>

  #include "base.h"
  #include "hw_if.h"
  #include "except.h"
  #include "osif.h"
  #include "swdl_api.h"
  #include "appl_svi.h"
  #include "hw_svi_swdl_if.h"

LOCAL_DEFINES


LOCAL_TYPEDEFS

  /* Disable info 754: local structure member '_PRIVATE_HEADER::xxx' not referenced */
  /*lint -esym(754,_PRIVATE_HEADER::platform_id)   */
  /*lint -esym(754,_PRIVATE_HEADER::software_type) */
  /*lint -esym(754,_PRIVATE_HEADER::offset_1)      */
  /*lint -esym(754,_PRIVATE_HEADER::len_1)         */
  /*lint -esym(754,_PRIVATE_HEADER::checksum_1)    */
  /*lint -esym(754,_PRIVATE_HEADER::offset_2)      */
  /*lint -esym(754,_PRIVATE_HEADER::len_2)         */
  /*lint -esym(754,_PRIVATE_HEADER::checksum_2)    */
  typedef struct _PRIVATE_HEADER
  {
    USIGN8  platform_id[10];  /* platform ID */
    USIGN8  software_type[2]; /* software type */
    USIGN32 offset_1;     /* offset of FD-SW in binary */
    USIGN32 len_1;        /* length of FD-SW binary */
    USIGN16 checksum_1;   /* checksum of FD-SW binary */
    USIGN32 offset_2;     /* offset of additional Appl SW in binary */
    USIGN32 len_2;        /* length of additional Appl SW binary */
    USIGN16 checksum_2;   /* checksum of additional Appl SW binary */
  } T_PRIVATE_HEADER;


FUNCTION_DECLARATIONS

EXPORT_DATA

IMPORT_DATA

LOCAL_DATA

#include "da_fdev.h"                             /* DATA SEGMENT DEFINITION */

  NO_INIT STATIC T_PRIVATE_HEADER     pri_header;
#if 0
  NO_INIT STATIC USIGN32              binary_data_len;      /* length of the downloaded binary --> included within private header */
  NO_INIT STATIC USIGN32              binary_data_count;    /* already received data of the downloaded binary */

  NO_INIT STATIC BOOL                 isDataCompressed;     /* flag to indicate whether the binary data within the downloaded domain is compressed or not */
#endif
#include "da_def.h"                              /* DEFAULT DATA SEGMENT */




/****************************************************************************/




FUNCTION PUBLIC VOID init_svi_dwnld()

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

PARAMETERS

RETURN_VALUES

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES


FUNCTION_BODY

  memset(&pri_header,0,sizeof(T_PRIVATE_HEADER));
#if 0
  binary_data_len               = 0;
  binary_data_count             = 0;

  isDataCompressed              = FALSE;
#endif
  return;
}





/* ------------- Functions for handling of FD domain download ------------- */

FUNCTION PUBLIC BOOL appl_fd_sw_available (VOID)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

  This function gives information about the running software

PARAMETERS

RETURN_VALUES
  TRUE      The field device software is running
  FALSE     The download device software is running (Class 3 Download only)

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  return TRUE;
}





#if 0
FUNCTION PUBLIC VOID start_prepare_swdl()

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

    Invalidate the old FD-SW image.

PARAMETERS

RETURN_VALUES

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  binary_data_count = 0;

  flash_OpenPartition();

}
#endif



FUNCTION PUBLIC USIGN8 finish_prepare_swdl()

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

    Check whether the OpenPartiion call was successful.

PARAMETERS

RETURN_VALUES

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

  USIGN8  result = HW_DWNLD_OPEN_PARTIION_FAILED;

FUNCTION_BODY

 // result = (USIGN8)flash_GetStatus ();

//  result = (result == EOP) ? HW_DWNLD_OPEN_PARTIION_OK : HW_DWNLD_OPEN_PARTIION_FAILED;
  result = HW_DWNLD_OPEN_PARTIION_OK;
  return (result);

}


#if 0

FUNCTION PUBLIC USIGN8 appl_check_fd_dom_hdr
  (
    IN T_DOMAIN_HDR_API *   p_dwnld_dom_hdr
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

PARAMETERS

RETURN_VALUES

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  /* No checks in this implementation ------------------------------------ */

  _UNREF_PARAM(p_dwnld_dom_hdr);

  return E_OK;
}

#endif




FUNCTION PUBLIC USIGN8 appl_check_fd_dom_segm
  (
    IN USIGN8           segment_id,
    IN USIGN8           segment_len,
    IN USIGN8 *         p_segment
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

PARAMETERS

RETURN_VALUES

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  /* No checks in this implementation ------------------------------------ */

  _UNREF_PARAM(segment_id);
  _UNREF_PARAM(segment_len);
  _UNREF_PARAM(p_segment);

  return E_OK;
}





FUNCTION PUBLIC USIGN8 appl_store_fd_dom_hdr
  (
    IN T_DOMAIN_HDR_API *   p_dwnld_dom_hdr
  )

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

PARAMETERS

RETURN_VALUES

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  /* The header is not stored in this implementation ---------------------- */

  _UNREF_PARAM(p_dwnld_dom_hdr);

  return E_OK;
}



#if 0
FUNCTION PUBLIC BOOL activate_fd_sw()

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

PARAMETERS

RETURN_VALUES

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  return TRUE;
}

#endif



FUNCTION PUBLIC USIGN8 verify_activation()
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

PARAMETERS

RETURN_VALUES

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES


FUNCTION_BODY

  /* here it could be checked whether the domain from the intended Flash bank was activated */

  return E_OK;
}





FUNCTION PUBLIC BOOL complete_fd_dom_is_avail (VOID)
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

PARAMETERS

RETURN_VALUES

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

FUNCTION_BODY

  /*  here a handling could be implemented for the case that the Device was
      reset after download and before activation */

  return TRUE;

}


#if 0
/*********************************************************************************/
/* Functions for Handling, possibly  and Write to Flash of Segement Data        */
/*********************************************************************************/

FUNCTION PUBLIC USIGN8 dwnld_handle_data
(
  IN USIGN8           segment_id,
  IN USIGN8           data_length,
  IN USIGN8 *         p_data
)
/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

  Handles the downloaded data.

  The FF Header is already removed from first data segment when this function is called,
  but the private Header is still contained within the first data segment that is passed
  to this function.

PARAMETERS
  None

RETURN_VALUES
  None

----------------------------------------------------------------------------*/
{
LOCAL_VARIABLES

  USIGN8  result = E_OK;
  BOOL    more_follows;

FUNCTION_BODY


  if ( (data_length == 0) || (p_data == NULL) )
  {
    return(NO_DATA);
  }

  /* the passed through segement_id could be one of:
      SWDL_LOAD_FIRST_SEGM  - First Data Segment Recevied
      SWDL_LOAD_SEGM        - with Second Data Segment Received
      SWDL_LOAD_LAST_SEGM   - Last Data Segment Received
  */
  /* as long as the Last Data Segment wasn't received, more data will follow to the currently handled data segment */
  more_follows = (segment_id < SWDL_LOAD_LAST_SEGM) ? TRUE : FALSE;

  if ( binary_data_count < PRI_H_LEN )
  {
    /* for the example it is assumped that the private header is contained completed within the first data segment */
    memcpy((USIGN8 *)&pri_header, p_data, PRI_H_LEN);

    binary_data_len = pri_header.len_1;

    /* - set the Flag that indicates whether the downloaded domain data will be compressed or not
       - this information could be included within the private header
       - no data compression implemented in this sample implementation */
    isDataCompressed = FALSE;

    /* store private header in flash memory if desired to keep those information persistant */
    result = flash_ProgramBlock((const USIGN16 *)&pri_header, (USIGN16 *) FLASH_START_ADDRESS_BANK1, PRI_H_LEN);

    /* point to the data behind private header */
    p_data = p_data+PRI_H_LEN;

    /* substract private header length from data_length */
    data_length -= PRI_H_LEN;

    /* count private header length to received binary data length variable binary_data_count */
    binary_data_count += PRI_H_LEN;

    /* no more data contained in that data segment || the program of the private header failed --> return */
    if ( (data_length == 0) || (result == FALSE) )
    {
      return result;
    }
  }


  if (  ((binary_data_count < binary_data_len) && (more_follows == TRUE)) ||
        ( more_follows == FALSE )
     )
  {
    /* count the length of the received data segments */
    binary_data_count += data_length;

    if (isDataCompressed == FALSE)
    {
      result = flash_ProgramBlock((const USIGN16 *)p_data, (USIGN16 *) FLASH_START_ADDRESS_BANK1, data_length);
    }
    else /* isDataCompressed == TRUE */
    {
      /* call a uncrompress function if needed */
    }
  }

  /* the BOOLean result from flash_ProgramBlock need to be converted to handle the result proper within the stack */
  if (result == TRUE)
  {
    result = HW_DWNLD_WRITE_OK;
  }
  else
  {
    result = HW_DWNLD_WRITE_ERR;
  }

  /* check for failure */
  if ( (binary_data_count >= binary_data_len) && (more_follows == TRUE) ) /* any expected data handled but more_follows still TRUE */
  {
    result = SWDL_DATA_OVERRUN;
  }
  else if ( (binary_data_count < binary_data_len) && (more_follows == FALSE) ) /* more data expected to be handled but more_follows already FALSE */
  {
    result = SWDL_DATA_INCOMPLETE;
  }

  return (result);
}
#endif
