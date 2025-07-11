/*
Copyright 2012 by GE Energy / Dresser, Inc., as an unpublished trade secret. All rights reserved.

This document and all information herein are the property of GE Energy.
It and its contents are  confidential and have been provided as a part of
a confidential relationship.  As such, no part of it may be made public,
decompiled, reverse engineered or copied and it is subject to return upon demand.
*/

/**
    \file ipcdefs.h
    \brief Public definitions for the IPC commands.

    CPU: Any

    OWNER: AP
*/

#ifndef IPCDEFS_H_
#define IPCDEFS_H_

//---------------------------------------------------
// This section is to control of the definitions of Var Sub-status
// values:

// This definition forces the use of IPC - specific sub-statuses.
// This should be avoided and used for diagnosic of IPC commands.
#define IPC_STATUS_IPC_SPECIFIC                 (0u)

// This definition forces the use of FF - Compatible sub-statuses.
// This should be used normally.
#define IPC_STATUS_FF_COMPATIBLE                (1u)

//---------------------------------------------------

//  #define IPC_STATUS_DEFINITIONS                  (IPC_STATUS_IPC_SPECIFIC)
#define IPC_STATUS_DEFINITIONS                  (IPC_STATUS_FF_COMPATIBLE)

//---------------------------------------------------

//lint ++flb
//---------------------------------------------------
// Define structures to pass parameters

typedef struct IPC_ReadPtrs_t
{
    u8          IPC_DataBlockNum;
    u8          *pVarStatus;
    void        *pIPC_VarBuffer;

} IPC_ReadPtrs_t;

typedef struct IPC_WritePtrs_t
{
    u8          IPC_DataBlockNum;
    u8 const    *pVarStatus;
    void const  *pIPC_VarBuffer;

} IPC_WritePtrs_t;

//---------------------------------------------------
// IPC data length
#define IPC_WRITE_ARRAY_DATASIZE                 (11u)
#define IPC_READ_ARRAY_DATASIZE                  (11u)

// Variable Type - Long or short or UNUSED entry

#define IPC_VAR_UNUSED                          (0x00u)
#define IPC_SHORT_VAR                           (0x01u)
#define IPC_LONG_VAR                            (0x02u)

//---------------------------------------------------
// Limit Status codes

#define IPC_STATUS_LIMITED_UPPER                (0x02u)
#define IPC_STATUS_LIMITED_LOWER                (0x01u)
#define IPC_STATUS_LIMITED                      (IPC_STATUS_LIMITED_UPPER | IPC_STATUS_LIMITED_LOWER)

//---------------------------------------------------
// Definitions for Target and Actual modes
// Modes are bitmaps!
// page 55 of ff-890.pdf
// NOTE:
// There is a idiosyncrazy in definition of the mode bitmaps in FF
// The actual bits shall be little endian, while teh spcecification describes the
// mode btmaps in big endian fashion ON THE BIT level.
// Thus, the bitmap shall be reversed. This purely internal representation.
// For reference -- see SOfting's code and definition -- fbap.h

#define IPC_MODE_NO_CHANGE                      (0x00u)

#define IPC_MODE_OOS                            (0x01u)
#define IPC_MODE_IMAN                           (0x02u)
#define IPC_MODE_LOVERRIDE                      (0x04u)
#define IPC_MODE_MANUAL                         (0x08u)
#define IPC_MODE_AUTO                           (0x10u)
#define IPC_MODE_CAS                            (0x20u)
#define IPC_MODE_RCAS                           (0x40u)
#define IPC_MODE_ROUT                           (0x80u)

#define IPC_MODE_NORMAL                         IPC_MODE_AUTO

//---------------------------------------------------
// Status codes

#define IPC_QUALITY_GOOD                        (0x02u << 6u)
#define IPC_QUALITY_BAD                         (0x00u << 6u)
#define IPC_QUALITY_UNCERTAIN                   (0x01u << 6u)
#define IPC_QUALITY_GOODCASCADE                 (0x03u << 6u)
#define IPC_QUALITY_MASK                        (0xC0u)

#define SelectQuality(x)                        ((x) & IPC_QUALITY_MASK)

#define IsQualityGOOD(x)                        (SelectQuality(x) == IPC_QUALITY_GOOD)
#define IsQualityGOODCASCADE(x)                 (SelectQuality(x) == IPC_QUALITY_GOODCASCADE)
// #define IsQualityBAD(x)                         (SelectQuality(x) == IPC_QUALITY_BAD)
// #define IsQualityUNCERTAIN(x)                   (SelectQuality(x) == IPC_QUALITY_UNCERTAIN)
// #define IsQuality_IGNORE(x)                     ((IsQualityBAD(x)) || (IsQualityUNCERTAIN(x)))
#define IsQuality_ACCEPT(x)                     ((IsQualityGOODCASCADE(x)) || (IsQualityGOOD(x)))

// IPC Error codes
#define IPC_ERROR_ErrorCode(x)                  ((x) << 2)

//---------------------------------------------------
// FF-compatible status definitions
/* Substatus for all Qualities */
#define IPC_SUB_NON_SPECIFIC             	    (IPC_ERROR_ErrorCode(0u))

/* Substatus for BAD Quality */
#define IPC_SUB_CONFIGURATION_ERROR      	    (IPC_ERROR_ErrorCode(1u))
#define IPC_SUB_NOT_CONNECTED            	    (IPC_ERROR_ErrorCode(2u))
#define IPC_SUB_DEVICE_FAILURE           	    (IPC_ERROR_ErrorCode(3u))
#define IPC_SUB_SENSOR_FAILURE           	    (IPC_ERROR_ErrorCode(4u))
#define IPC_SUB_NO_COMM_WITH_LUV         	    (IPC_ERROR_ErrorCode(5u))
#define IPC_SUB_NO_COMM_NO_LUV           	    (IPC_ERROR_ErrorCode(6u))
#define IPC_SUB_OUT_OF_SERVICE           	    (IPC_ERROR_ErrorCode(7u))
#define IPC_SUB_BAD_TB_IN_MANUAL         	    (IPC_ERROR_ErrorCode(8u))

/* Substatus for UNCERTAIN Quality */
#define IPC_SUB_LUV                      	    (IPC_ERROR_ErrorCode(1u))
#define IPC_SUB_SUBSTITUTE               	    (IPC_ERROR_ErrorCode(2u))
#define IPC_SUB_INITIAL_VALUE            	    (IPC_ERROR_ErrorCode(3u))
#define IPC_SUB_SENSOR_CONV_NOT_ACCURATE 	    (IPC_ERROR_ErrorCode(4u))
#define IPC_SUB_EU_RANGE_VIOLATION       	    (IPC_ERROR_ErrorCode(5u))
#define IPC_SUB_SUB_NORMAL               	    (IPC_ERROR_ErrorCode(6u))
#define IPC_SUB_UNCERT_TB_IN_MANUAL      	    (IPC_ERROR_ErrorCode(7u))

/* Substatus for GOOD (Non-cascade) Quality */
#define IPC_SUB_ACTIVE_BLOCK_ALARM       	    (IPC_ERROR_ErrorCode(1u))
#define IPC_SUB_ACTIVE_ADVISORY_ALARM    	    (IPC_ERROR_ErrorCode(2u))
#define IPC_SUB_ACTIVE_CRITICAL_ALARM    	    (IPC_ERROR_ErrorCode(3u))
#define IPC_SUB_UNACK_BLOCK_ALARM        	    (IPC_ERROR_ErrorCode(4u))
#define IPC_SUB_UNACK_ADVISORY_ALARM     	    (IPC_ERROR_ErrorCode(5u))
#define IPC_SUB_UNACK_CRITICAL_ALARM     	    (IPC_ERROR_ErrorCode(6u))

/* Substatus for GOOD (Cascade) Quality */
#define IPC_SUB_IA                       	    (IPC_ERROR_ErrorCode(1u))
#define IPC_SUB_IR                       	    (IPC_ERROR_ErrorCode(2u))
#define IPC_SUB_NI                       	    (IPC_ERROR_ErrorCode(3u))
#define IPC_SUB_NS                       	    (IPC_ERROR_ErrorCode(4u))
#define IPC_SUB_DNS                      	    (IPC_ERROR_ErrorCode(5u))
#define IPC_SUB_LO                       	    (IPC_ERROR_ErrorCode(6u))
#define IPC_SUB_FSA                      	    (IPC_ERROR_ErrorCode(7u))
#define IPC_SUB_IFS                      	    (IPC_ERROR_ErrorCode(8u))
//---------------------------------------------------

#if ((IPC_STATUS_DEFINITIONS != IPC_STATUS_FF_COMPATIBLE) && (IPC_STATUS_DEFINITIONS != IPC_STATUS_IPC_SPECIFIC))
// Error -- incorrect constant is specified
    error Incorrect Status Definitions are used!
#endif

#if (IPC_STATUS_DEFINITIONS == IPC_STATUS_FF_COMPATIBLE)
// FF-compatible status definitions

  #define IPC_NO_ERROR                          (IPC_SUB_NON_SPECIFIC)
  #define IPC_VARID_NA                          (IPC_SUB_NON_SPECIFIC)
  #define IPC_BAD_INDEX                         (IPC_SUB_NON_SPECIFIC)
  #define IPC_BAD_BLOCKNUM                      (IPC_SUB_NON_SPECIFIC)
  #define IPC_VALUE_NOTAVAIL                    (IPC_SUB_NON_SPECIFIC)
  #define IPC_TYPE_CONFLICT                     (IPC_SUB_NON_SPECIFIC)
  #define IPC_DATA_INCONSISTENT                 (IPC_SUB_NON_SPECIFIC)

  #define IPC_DATA_NOTWRITABLE                  (IPC_SUB_NON_SPECIFIC)
  #define IPC_DATA_OUT_OF_RANGE                 (IPC_SUB_NON_SPECIFIC)
  #define IPC_DATA_OUT_OF_ENUM                  (IPC_SUB_NON_SPECIFIC)
  #define IPC_DATA_VALIDATION_FAILED            (IPC_SUB_NON_SPECIFIC)
  #define IPC_DATA_STATE_CONFLICT               (IPC_SUB_NON_SPECIFIC)
  #define IPC_RESERVED_ERROR_1                  (IPC_SUB_NON_SPECIFIC)
  #define IPC_RESERVED_ERROR_2                  (IPC_SUB_NON_SPECIFIC)
  #define IPC_RESERVED_ERROR_3                  (IPC_SUB_NON_SPECIFIC)

  #define IPC_EXTENDED_ERROR                    (IPC_SUB_NON_SPECIFIC)

#else
// IPC-Specific status definitions

  #define IPC_NO_ERROR                          (IPC_ERROR_ErrorCode(0x00u))
  #define IPC_VARID_NA                          (IPC_ERROR_ErrorCode(0x01u))
  #define IPC_BAD_INDEX                         (IPC_ERROR_ErrorCode(0x02u))
  #define IPC_BAD_BLOCKNUM                      (IPC_ERROR_ErrorCode(0x03u))
  #define IPC_VALUE_NOTAVAIL                    (IPC_ERROR_ErrorCode(0x04u))
  #define IPC_TYPE_CONFLICT                     (IPC_ERROR_ErrorCode(0x05u))
  #define IPC_DATA_INCONSISTENT                 (IPC_ERROR_ErrorCode(0x06u))

  #define IPC_DATA_NOTWRITABLE                  (IPC_ERROR_ErrorCode(0x07u))
  #define IPC_DATA_OUT_OF_RANGE                 (IPC_ERROR_ErrorCode(0x08u))
  #define IPC_DATA_OUT_OF_ENUM                  (IPC_ERROR_ErrorCode(0x09u))
  #define IPC_DATA_VALIDATION_FAILED            (IPC_ERROR_ErrorCode(0x0Au))
  #define IPC_DATA_STATE_CONFLICT               (IPC_ERROR_ErrorCode(0x0Bu))
  #define IPC_RESERVED_ERROR_1                  (IPC_ERROR_ErrorCode(0x0Cu))
  #define IPC_RESERVED_ERROR_2                  (IPC_ERROR_ErrorCode(0x0Du))
  #define IPC_RESERVED_ERROR_3                  (IPC_ERROR_ErrorCode(0x0Eu))

  // Extended Error Code to indicate Vraiable / Function / Device specific error
  // with additional Error code provided separately
  #define IPC_EXTENDED_ERROR                      (IPC_ERROR_ErrorCode(0x0Fu))
#endif

//---------------------------------------------------
// The end
//lint --flb

#endif //IPCDEFS_H_
