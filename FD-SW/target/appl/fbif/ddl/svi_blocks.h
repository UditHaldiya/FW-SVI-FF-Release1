#ifndef SVI_BLOCKS_H_2012_07_25_10_07_35_GE_300010777
#define SVI_BLOCKS_H_2012_07_25_10_07_35_GE_300010777

/*
  Time-stamp: <2013-08-16 11:30:23 GE-300010777>

  This files includes all labels, helps and macros for svi function blocks,
  except the transducer block.

  Documents:
  [[file:d:/Doc_index/FF_Index/SViFF/FF_Parameter_Mapping_RB-07-19.xlsx]]
*/

/*===========================================/
* Resource block 2
/===========================================*/

/*-------------------------------------------/
** Defines for Labels and Helps
/-------------------------------------------*/
#define LBL_042_CAPABILITY_LEV          "Capability Lev"        
#define LBL_043_SOFTWARE_REV            "Software Rev"          
#define LBL_044_HARDWARE_REV            "Hardware Rev"          
#define LBL_045_COMPATIBILITY_REV       "Compatibility Rev"     
#define LBL_046_BLOCK_ERR_DESC_1        "Block Err Desc 1"      
#define LBL_047_WRONG_CONFIGURATION     "Wrong Configuration"   
#define LBL_047_01_INDEX                "Index"                 
#define LBL_047_02_SUB_INDEX            "Sub Index"             
#define LBL_047_03_DESCRIPTION          "Description"           
#define LBL_048_SOFTWARE_REV_FF         "Software Rev FF"       
#define LBL_049_SOFTWARE_REV_APP        "Software Rev App"      

#ifdef DD4

#define HLP_042_CAPABILITY_LEV          "|en|"
#define HLP_043_SOFTWARE_REV            "|en|"
#define HLP_044_HARDWARE_REV            "|en|"
#define HLP_045_COMPATIBILITY_REV       "|en|"
#define HLP_046_BLOCK_ERR_DESC_1        "|en|"
#define HLP_047_WRONG_CONFIGURATION     "|en|"
#define HLP_047_01_INDEX                "|en|"
#define HLP_047_02_SUB_INDEX            "|en|"
#define HLP_047_03_DESCRIPTION          "|en|"
#define HLP_048_SOFTWARE_REV_FF         "|en|"
#define HLP_049_SOFTWARE_REV_APP        "|en|"

#else

#define HLP_042_CAPABILITY_LEV                                                              \
  "|en| This parameter may be included in a device to indicate the capability"          \
  "level supported by a device."                                                        \
  "\n 1 - No diagnostics"                                                               \
  "\n 2 - Standard Diagnostics"                                                         \
  "\n 3 - Advanced Diagnostics"                                                         \
  "\n 4 Advanced Diagnostics with Extended Control"                                     
#define HLP_043_SOFTWARE_REV                                                                    \
  "|en| Software revision including the Software revision of the Application"           \
  "Processor."                                                                          \
  "\nShall be presented as a bigger number between the SOFTWARE_REV_FF and"             \
  "SOFTWARE_REV_APP"                                                                        
#define HLP_044_HARDWARE_REV                                                                    \
  "|en| Hardware Revision"                                                              
#define HLP_045_COMPATIBILITY_REV                                                           \
  "|en| Presumes the DEV_REV value of the replaced device is equal or lower than"       \
  "the COMPATABILITY_REV value (see AN-007)."                                           
#define HLP_046_BLOCK_ERR_DESC_1                                                                \
  "|en| Detailed description of the errors detected in Resource Block."                 \
  "\nThe following enumeration should be followed:"                                     \
  "\n 0x00000000 - No error"                                                            \
  "\n 0x00000001 - 0x00000008 - Reserved"                                               \
  "\n APPlication problems (0x00000010 - 0x00008000) , e.g.:"                           \
  "\n 0x00000010 - Catastrofic Failure in valve APPlication"                            \
  "\n 0x00000020 - APP is forced to Fail State"                                         \
  "\n Resource Block Problems (0x00010000 - 0x80000000 - ) , e.g.:"                     \
  "\n 0x00010000 -No Connection to the APPlication"                                     \
  "\n 0x00020000 -Configuration Error - see WRONG CONFIGURATION parameter"              
#define HLP_047_WRONG_CONFIGURATION                                                         \
  "|en| A value of 0 indicates that wrong configuration is not detected"                \
  "\nParameter number within the resource block will indicate the first detected"       \
  "parameter, that has wrong configuration"                                             
#define HLP_047_01_INDEX                                                                        \
  "|en| Index of the parameter with problematic configuration"                          
#define HLP_047_02_SUB_INDEX                                                                    \
  "|en| Sub-Index of the parameter with problematic configuration"                      
#define HLP_047_03_DESCRIPTION                                                              \
  "|en| Description of the problem"                                                     
#define HLP_048_SOFTWARE_REV_FF                                                             \
  "|en| Software revision of the FF Processor. Each of the 4 bytes will have the"       \
  "number for the version."                                                             
#define HLP_049_SOFTWARE_REV_APP                                                                \
  "|en| Software revision of the Application Processor.Each of the 4 bytes will"        \
  "have the number for the version."                                                    \

#endif
/*-------------------------------------------/
** Added parameters mapping
/-------------------------------------------*/

#define ADD_SVI_ADDITION_PARA_RB                           \
     /* ADD CAPABILITY_LEV,         capability_lev; */      \
     ADD SOFTWARE_REV,           software_rev;        \
     ADD HARDWARE_REV,           hardware_rev;        \
     ADD COMPATIBILITY_REV,      compatibility_rev;   \
     ADD BLOCK_ERR_DESC_1,       block_err_desc_1;    \
     /* ADD WRONG_CONFIGURATION,    wrong_configuration; */ \
     ADD SOFTWARE_REV_FF,        software_rev_ff;     \
     ADD SOFTWARE_REV_APP,       software_rev_app;

#define SVI2_ADDITION_PARA_RB                           \
    CAPABILITY_LEV,         capability_lev;      \
    SOFTWARE_REV,           software_rev;        \
    HARDWARE_REV,           hardware_rev;        \
    COMPATIBILITY_REV,      compatibility_rev;   \
    BLOCK_ERR_DESC_1,       block_err_desc_1;    \
    WRONG_CONFIGURATION,    wrong_configuration; \
    SOFTWARE_REV_FF,        software_rev_ff;     \
    SOFTWARE_REV_APP,       software_rev_app;
 
/*-------------------------------------------/
** Added additional items for views
/-------------------------------------------*/

#define ADD_SVI_ADDITION_VIEW3_ITEMS_RB							\
     ADD VL_BLOCK_ERR_DESC_1, PARAM.BLOCK_ERR_DESC_1;			\
     ADD VL_WRONG_CONFIGURATION, PARAM.WRONG_CONFIGURATION;		

#define ADD_SVI_ADDITION_VIEW4_ITEMS_RB							\
     ADD VL_CAPABILITY_LEV,        PARAM.CAPABILITY_LEV;		\
     ADD VL_SOFTWARE_REV,          PARAM.SOFTWARE_REV;			\
     ADD VL_HARDWARE_REV,          PARAM.HARDWARE_REV;			\
     ADD VL_COMPATIBILITY_REV,     PARAM.COMPATIBILITY_REV;		\
     ADD VL_BLOCK_ERR_DESC_1,      PARAM.BLOCK_ERR_DESC_1;		\
     ADD VL_WRONG_CONFIGURATION,   PARAM.WRONG_CONFIGURATION;	\
     ADD VL_SOFTWARE_REV_FF,       PARAM.SOFTWARE_REV_FF;		\
     ADD VL_SOFTWARE_REV_APP,      PARAM.SOFTWARE_REV_APP;		
/*-------------------------------------------/
** RB block MENUS list
/-------------------------------------------*/

#define RB_MENU_LIST					\
   device_root_menu_rb,					\
   diagnostic_root_menu_rb,				\
   process_variables_root_menu_rb

/*-------------------------------------------/
** RB block METHOD LIST
/-------------------------------------------*/
#define RB_METHOD_LIST					\
restart								

/*===========================================/
* Input Selector Block
/===========================================*/

/*===========================================/
* Multiple Analog Input block (MAI)
/===========================================*/

/*===========================================/
* Analog Input block (AI)
/===========================================*/

/*===========================================/
* Digital Input block (DI)
/===========================================*/

/*===========================================/
* Analog Output block (AO)
/===========================================*/

/*===========================================/
* Digital Output block (DO)
/===========================================*/

/*===========================================/
* Proportional Integral Derivative block (PID)
/===========================================*/

/*===========================================/
* Output Splitter Block
/===========================================*/

/*===========================================/
* AI transducer block
/===========================================*/

#endif

/*===========================================/
* Revision History
/===========================================*/


/****************************************************************************

 <2012-08-21>#25006: updated for new FF_Parameter_Mapping_RB.xlsx(08-13)

*****************************************************************************/

