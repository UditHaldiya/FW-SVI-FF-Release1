/* ===========================================================================

FILE_NAME          blk_err.h


FUNCTIONAL_MODULE_DESCRIPTION

  This file contains definitions for the DD of the FBK HART Master
  application.

=========================================================================== */


#define LBL_BLOCK_ERR_DESC_1    "|en|Block Error Description"
#define HLP_BLOCK_ERR_DESC_1    "|en|List of un- or misconfigured parameters"

#define BLK_ERR_DESCR_1_ENUM                                                                                        \
    { 0x00000002,  "Channel-Not-Init",        "Block channel is uninitialized"                                   }, \
    { 0x00000004,  "Wrong-Channel",           "Block has a wrong channel setting"                                }, \
    { 0x00000008,  "Channel-Unit-Mismatch",   "Block channel unit setting error"                                 }, \
    { 0x00000010,  "L_type-Not-Init",         "L-type is uninitialized"                                          }, \
    { 0x00000020,  "Wrong-L_type",            "The direct L-type requires same values in XD_SCALE and OUT_SCALE" }, \
    { 0x00000040,  "XD_Scale-Inconsist",      "Value EU_100 should be greater than EU_0"                         }, \
    { 0x00000080,  "OUT_Scale-Inconsist",     "Value EU_100 should be greater than EU_0"                         }, \
    { 0x00000100,  "PV_Scale-Inconsist",      "Value EU_100 should be greater than EU_0"                         }, \
    { 0x00000200,  "TRK_Scale-Inconsist",     "Value EU_100 should be greater than EU_0"                         }, \
    { 0x00000400,  "FF_Scale-Inconsist",      "Value EU_100 should be greater than EU_0"                         }, \
    { 0x00000800,  "Bypass-Not-Init",         "Bypass is not set"                                                }, \
    { 0x00001000,  "Shed_Opt-Not-Init",       "Shed opt is not set"                                              }, \
    { 0x00002000,  "SP_Limits-Inconsist",     "Hi_Lim should be greater than or equal to Lo_Lim"                 }, \
    { 0x00004000,  "OUT_Limits-Inconsist",    "Hi_Lim should be greater than or equal to Lo_Lim"                 }, \
    { 0x00008000,  "Arith_Type-Not-Init",     "No arithmetic algorithm is selected"                              }, \
    { 0x00010000,  "Select_Type-Not-Init",    "Selector processing type is not initialized"                      }, \
    { 0x00020000,  "Integ_Type-Not-Init",     "Integration type is not initialized"                              }, \
    { 0x00040000,  "Range-Inconsist",         "Hi should be greater than or equal to Lo"                         }, \
    { 0x00080000,  "Compensation-Inconsist",  "Hi_Lim should be greater than or equal to Lo_Lim"                 }, \
    { 0x00100000,  "Curve_X-Inconsist",       "Curve X is not continuous"                                        }, \
    { 0x00200000,  "Curve_X-Not-Monoton",     "Curve X is not monotonic"                                         }, \
    { 0x00400000,  "Curve_Y-Inconsist",       "Curve Y is not continuous"                                        }, \
    { 0x00800000,  "Swap_2-Not-Init",         "Swap 2 uninitialized"                                             }, \
    { 0x01000000,  "Curve_Y-Not-Monoton",     "Curve Y is not monotonic"                                         }, \
    { 0x02000000,  "In_Array-Inconsist",      "In-Array is inconsistent"                                         }, \
    { 0x04000000,  "Lockval-Not-Init",        "Lockval is not initialized"                                       }, \
    { 0x80000000,  "Period_Of_Exec-is-Zero",  "Period of execution must be greater than zero"                    }


