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

#FILE_NAME          Std_makefile.mak

#

#DESCRIPTION

#   This is the 'standard' (= common) file defining the settings for the
#   development tools when generating the Softing Field Device Software.
#   In general, it's valid for all protocols and variants. If any special
#   adoptions have to be made for certain variants, copy this file to the
#   variant specific 'Makesettings.mak' and enter the adaptions as required.

#   Any changes here will affect all variants!

# ===========================================================================


export USED_MAKEFILES += ../std_settings.mak

################################################################################
# compiler section

ifneq ($(MAKE_ENVIRONMENT), SOFTING)
#Masoneilan build environment
CCFLAGS+=-DMN_BUILD_ENV -DPROJPARAM_H_="\"project_FFAP.h\""
INC_DIRS+=$(MNINCLLUDE)
endif

CCFLAGS += -DRES_VAL_DEV_TYPE=0x$(DEVICE_TYPE) -DRES_VAL_DEV_REV=0x$(DEVICE_REV) -D$(HW_TYPE)
CCFLAGS += -DOS_EMBOS -DOS_LIBMODE_$(EMBOS_MODE)
CCFLAGS += -DSYS_DIV=0x$(CFG_SYS_DIV) -DISR_SYS_DIV=0x$(CFG_ISR_SYS_DIV) -D$(CPU)
CCFLAGS += $(foreach block, $(BLOCK_LIST), -D$(block))

#CCFLAGS += $(if $(filter 32x 35x 3xx, $(IAR_COMPILER_VER)),--diag_suppress=Pa082)

#@Dresser project modified: intentional suppress of "Warning[Pa039]: use of address of unaligned structure member"
CCFLAGS += --diag_suppress=Pa039

ifeq ($(ACTIVE_MODE), RELEASE)
  CCFLAGS += -r -Om -DNDEBUG
else
  CCFLAGS += -r -Om -DPD30 -D_DEBUG
endif

# if list files are needed:
CCFLAGS += $(LIST_OPTION)

ifeq ($(FIELD_DIAGNOSTICS), USE_FIELD_DIAGNOSTICS)
CCFLAGS += -DUSE_FIELD_DIAGNOSTICS
endif


ifeq ($(STACK_TYPE), FF_LM)
CCFLAGS += -DLINK_MASTER
endif

ifeq ($(FBL_TYPE), FBL_MODULAR)
  $(error 'FBL_TYPE = FBL_MODULAR' is not supported anymore!)
endif

ifeq ($(FBL_TYPE), FBL_BLOCK_INST)
  CCFLAGS += -DFBL_BLOCK_INST
else
  CCFLAGS += -DFBL_STD
endif

ifeq ($(MVC_TYPE), USE_MVC)
CCFLAGS += -DMVC_OBJECTS
endif

ifeq ($(SW_DOWNLOAD_TYPE), USE_SW_DOWNLOAD)
CCFLAGS += -DSW_DOWNLOAD
endif

ifeq ($(SW_DOWNLOAD_TYPE), SW_DOWNLOAD_DEV)
CCFLAGS += -DSW_DOWNLOAD -DDOWNLOAD_DEVICE
endif

ifeq ($(SW_DOWNLOAD_MODE), MULTI_DOM_DWNLD)
		CCFLAGS += -DMULTI_DOMAIN_DWNLD
endif

ifeq ($(STACK_TYPE), PA)
    CCFLAGS += -D$(DEVICE_TYPE)
endif

ifeq ($(FB_CHIP), FRONTIER1P)
      CCFLAGS += -DFC_FRONTIER1_PLUS
      ASSEMBLERFLAGS += -DFC_FRONTIER1_PLUS
else
  ifeq ($(FB_CHIP), SPC42)
      CCFLAGS += -DFC_SPC42
      ASSEMBLERFLAGS += -DFC_SPC42
  else
    ifeq ($(FB_CHIP), FIND1P_DMA)
      CCFLAGS += -DFC_FIND1P_DMA
      ASSEMBLERFLAGS += -DFC_FIND1P_DMA
    else
      CCFLAGS += -DFIND1_PLUS
      ASSEMBLERFLAGS += -DFIND1_PLUS
    endif
  endif
endif

ifeq ($(STACK_TYPE), DP)
    CCFLAGS += -DPROTOCOL_DP
endif

ifeq ($(STACK_TYPE), PA)
    CCFLAGS += -DPROTOCOL_PA
endif

ifeq ($(STACK_TYPE), FF_BFD)
    CCFLAGS += -DPROTOCOL_FF
endif

ifeq ($(STACK_TYPE), FF_LM)
    CCFLAGS += -DPROTOCOL_FF
endif

ifneq (,$(findstring hm,$(ADD_COMM_COMP)))
      CCFLAGS += -DHART_MASTER
endif

ifneq (,$(findstring mm,$(ADD_COMM_COMP)))
      CCFLAGS += -DMODBUS_MASTER
endif

ifeq ($(NV_DATA_STORAGE), NV_DATA_DOUBLE_BUFFER)
      CCFLAGS += -DUSE_NV_DATA_DOUBLE_BUFFER
endif

ifeq ($(HW_TYPE), FBK2_HW)
      CCFLAGS += -DFBK_HW
endif

ifeq ($(MAKE_ENVIRONMENT), SOFTING)
  CCFLAGS +=-DSOFTING
endif

ifeq ($(LICENSE_TYPE), USE_SEC_EEPROM)
			CCFLAGS += -DUSE_SEC_EEPROM
endif

################################################################################
# assembler section

ASSEMBLERFLAGS += -DOS_EMBOS -DOS_LIBMODE_$(EMBOS_MODE)
ASSEMBLERFLAGS += -D$(HW_TYPE) -DSYS_DIV=0x$(CFG_SYS_DIV) -DISR_SYS_DIV=0x$(CFG_ISR_SYS_DIV)

ifneq ($(ACTIVE_MODE), RELEASE)
  ASSEMBLERFLAGS += -DPD30 -D_DEBUG
endif

################################################################################
# librian section

LIBRARY_MANAGERFLAGS +=

################################################################################
# linker section

LINKERFLAGS +=
