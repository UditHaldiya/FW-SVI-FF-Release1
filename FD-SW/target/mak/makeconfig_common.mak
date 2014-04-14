#============================================================================#
#                                                                            #
#                     SOFTING Industrial Automation GmbH                     #
#                          Richard-Reitzner-Allee 6                          #
#                                D-85540 Haar                                #
#                        Phone: ++49-89-4 56 56-0                            #
#                          Fax: ++49-89-4 56 56-3 99                         #
#                                                                            #
#                            SOFTING CONFIDENTIAL                            #
#                                                                            #
#                     Copyright (C) SOFTING IA GmbH 2012                     #
#                             All Rights Reserved                            #
#                                                                            #
# NOTICE: All information contained herein is, and remains the property of   #
#   SOFTING Industrial Automation GmbH and its suppliers, if any. The intel- #
#   lectual and technical concepts contained herein are proprietary to       #
#   SOFTING Industrial Automation GmbH and its suppliers and may be covered  #
#   by German and Foreign Patents, patents in process, and are protected by  #
#   trade secret or copyright law. Dissemination of this information or      #
#   reproduction of this material is strictly forbidden unless prior         #
#   written permission is obtained from SOFTING Industrial Automation GmbH.  #
#============================================================================#
#============================================================================#
#                                                                            #
# PROJECT_NAME             Softing FF/PA FD 2.42                             #
#                                                                            #
# VERSION                  FF - 2.42                                         #
#                          PA - 2.42 (beta)                                  #
#                                                                            #
# DATE                     16. April 2012                                    #
#                                                                            #
#============================================================================#



# ===========================================================================

#FILE_NAME          Makeconfig.mak

#

#DESCRIPTION

#  Configuration settings to generate a Release (Flash) version of the
#  FF Device software V2.x

# ===========================================================================


USED_MAKEFILES += ../makeconfig_common.mak

################################################################################
# configure target

    # Define hardware platform: BFD_HW, FBK_HW, FBK2_HW
export HW_TYPE = CUSTOMER_HW

    # Define CPU type: M16C, 7700, 900L
export CPU = Cortex_M3

    # Define CPU clock prescaler value: 2
export CFG_SYS_DIV = 2

    # Define CPU clock prescaler value for FBC interrupt routine: 2
export CFG_ISR_SYS_DIV = 2

    # Define fieldbus controller chip: SPC42
export FB_CHIP       = SPC42

    # Define operating system: osif.emb, osif.cmx, osif.sos
export OSIF          = osif.emb

    # In case of embOS: define mode: R, S, SP, D, DP, DT; MUST correspond to OSIF.H
export EMBOS_MODE    = D

    # Define FF field diagnostic: USE_FIELD_DIAGNOSTICS, NO_FIELD_DIAGNOSTICS
export FIELD_DIAGNOSTICS = USE_FIELD_DIAGNOSTICS

    #Define support of MVC Objects: NO_MVC, USE_MVC
export MVC_TYPE      = USE_MVC

    #Define support of software download: NO_SW_DOWNLOAD, USE_SW_DOWNLOAD, SW_DOWNLOAD_DEV
export SW_DOWNLOAD_TYPE = USE_SW_DOWNLOAD

		#Valid only in case of software download: SINGLE_DOM_DWNLD, MULTI_DOM_DWNLD
export SW_DOWNLOAD_MODE = SINGLE_DOM_DWNLD

    # Define use of the security eeprom: USE_SEC_EEPROM, NO_SEC_EEPROM
export LICENSE_TYPE = USE_SEC_EEPROM

    # Define additional communication component: hm, mm
export ADD_COMM_COMP = hm

    # Define NV data handling: NV_DATA_DOUBLE_BUFFER
export NV_DATA_STORAGE = NV_DATA_DOUBLE_BUFFER

    # Define type of target device: PA, FF_BFD, FF_LM
export STACK_TYPE    = FF_LM

    # Define function block application:
export BLOCKS      = ao ai di do pid is os m_a_i cs ar
export MAIN_SCRIPT = SVI_Positioner_APP.gw
#export DEVICE_TYPE = 0008
#export DEVICE_REV  = 01

    # Define standard transducer blocks: ai_tb.gw
    # or manufacturer specific transducer block
export TBLK_SCRIPTS  = positioner_tb.gw

    # Define type of function block application: FBL_STD, FBL_BLOCK_INST
export FBL_TYPE      = FBL_STD

    # Define IAR C Compiler version, 32x (3.21F) or 35x (3.50)
export IAR_COMPILER_VER = 35x

    #  Define memory model for M16C far, huge (Compiler 3.xx)
export MEM_MODEL   = far

    # Define type of project delivery: OBJECT
export DELIVERY_TYPE = OBJECT

    # Name of linker definition file
  LINK_FILE          = ../svi_pos_ff.icf

    # Names of (executable) firmware files (will reside in directory defined by $(EXECUTABLE_LOC))
EXECUTABLE_LOC       = exe

BFD_EXECUTABLE       ?= svi_ff.out
UTA_EXECUTABLE       ?= svi_ff_uta.out

    # Lists of software components
SYSCOMPONENTS        = $(OSIF)
BASE_FFBS            = base ffbs
FFBS_COMPS_BUILD      = hw eep STM32F10x_StdPeriph_Driver fbif fdev $(ADD_COMM_COMP) $(SYSCOMPONENTS)
BASE_UTA             = base uta stub
BFD_COMPS            = $(FFBS_COMPS_BUILD) $(BASE_FFBS)
UTA_COMPS_BUILD      = hw eep STM32F10x_StdPeriph_Driver $(SYSCOMPONENTS)
UTA_COMPS            = $(UTA_COMPS_BUILD) $(BASE_UTA) 
ALL_COMPS            = $(sort $(BFD_COMPS) $(UTA_COMPS))

# Components contained in $(BASE_COMPS) will be delivered only in object code, if
# $(DELIVERY_TYPE) is not set to 'SOURCE'
export BASE_COMPS    = $(sort $(BASE_FFBS) $(BASE_UTA))

################################################################################

