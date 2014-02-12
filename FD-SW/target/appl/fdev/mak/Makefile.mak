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
#                     Copyright (C) SOFTING IA GmbH 2013                     #
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
# PROJECT_NAME             Softing FF/PA FD 2.42.A                           #
#                                                                            #
# VERSION                  FF - 2.42.A                                       #
#                          PA - 2.42.A                                       #
#                                                                            #
# DATE                     14. Aug. 2013                                     #
#                                                                            #
#============================================================================#




# ===========================================================================

#FILE_NAME          appl/fdev/mak/Makefile.mak

#

#DESCRIPTION

#  Generates the application specific component for the FF protocol software.

# ===========================================================================

.PHONY: $(COMPONENT) clean_$(COMPONENT) dlv_$(COMPONENT)

USED_MAKEFILES += $(TARGET)/appl/$(COMPONENT)/mak/Makefile.mak

################################################################################
# defaults

COMPONENT = fdev

include $(BASE)/mak/Makeenv.mak

SRC_DIRS =  $(TARGET)/appl/$(COMPONENT)/src
INC_DIRS := $(INC_DIRS):$(TARGET)/appl/fbif/inc:$(TARGET)/appl/ffbl/inc:$(BASE)/appl/ffbl/inc \
            $(BASE)/inc/$(COMPONENT):$(subst /src,/inc,$(SRC_DIRS))

vpath %.c   $(SRC_DIRS)
vpath %.h   $(INC_DIRS)


################################################################################
# target files of this component

CFGS = fdev_cfg.h
INCS = appl_int.h
SINCS = cs_fwinf.h cs_fdev.h da_fdev.h da_fbif.h


ifeq ($(SW_DOWNLOAD_TYPE), NO_SW_DOWNLOAD)
  SRCS = appl_if.c appl_res.c appl_dia.c appl_nrv.c
  ifeq ($(ADD_COMM_COMP), hm)
    SRCS += hm_tools.c
    INCS += appl_alert.h
  else
  ifeq ($(ADD_COMM_COMP), mm)
    SRCS += appl_trm.c
  else
    SRCS += appl_trn.c
  endif
  endif
endif

ifeq ($(SW_DOWNLOAD_TYPE), USE_SW_DOWNLOAD)
  SRCS = appl_if.c appl_res.c appl_dia.c appl_nrv.c
  ifeq ($(ADD_COMM_COMP), hm)
    SRCS += hm_tools.c
  else
  ifeq ($(ADD_COMM_COMP), mm)
    SRCS += appl_trm.c
  else
    SRCS += appl_trn.c
  endif
  endif

  SRCS += appl_dlh.c appl_svi_swdl.c
  CFGS += swdl_cfg.h
  INCS += swdl_api.h appl_svi.h
  INCS += appl_alert.h
  SINCS += cs_foot.h

  ifeq ($(HW_TYPE), FBK2_HW)
  SRCS += appl_fbk2.c
  INCS += appl_fbk2.h
  endif
endif


ifeq ($(SW_DOWNLOAD_TYPE), SW_DOWNLOAD_DEV)
  SRCS = appl_ifd.c appl_dlh.c
  CFGS += swdl_cfg.h
  INCS += swdl_api.h
  SINCS += cs_foot.h

  ifeq ($(HW_TYPE), FBK2_HW)
  SRCS += appl_fbk2.c
  INCS += appl_fbk2.h
  endif
endif

ifeq ($(ADD_COMM_COMP), hm)
INCS += hm_tools.h
SINCS += da_hm.h
else
ifeq ($(ADD_COMM_COMP), mm)
INCS += mm_tools.h
endif
endif

ifeq ($(HW_TYPE), FBK2_HW)
  $(or ifeq ($(SW_DOWNLOAD_TYPE), USE_SW_DOWNLOAD), ifeq ($(SW_DOWNLOAD_TYPE), SW_DOWNLOAD_DEV))
  SINCS += da_fbk2_dwld.h
endif

ifneq ($(MAKE_ENVIRONMENT), SOFTING)
include $(TARGET)/appl/$(COMPONENT)/mak/mn.mak
endif

OBJECTS = $(addprefix $(OBJ_DIR)/, $(addsuffix $(OBJ_SUFFIX), $(basename $(SRCS))))

LIB     = $(LIB_DIR)/$(COMPONENT)$(LIB_SUFFIX)

################################################################################
# dependencies

$(COMPONENT): $(OBJECTS) $(LIB)

$(LIB)      : $(OBJECTS)

$(OBJECTS)  : $(USED_MAKEFILES)


#-------------------------------------------------------------------------------
# Clean up

clean_$(COMPONENT):
	@echo cleaning $(COMPONENT)
	-@rm -f $(LIB) \
            $(OBJECTS) \
            $(TARGET)/appl/$(COMPONENT)/mak/makefile.dps

#-------------------------------------------------------------------------------
# Maintain dependencies

$(TARGET)/appl/$(COMPONENT)/mak/makefile.dps: $(SRCS)


#-------------------------------------------------------------------------------
# Deliver Sources

-include $(TARGET)/appl/$(COMPONENT)/mak/Makedlv.mak


################################################################################
# source file dependecies

-include $(TARGET)/appl/$(COMPONENT)/mak/makefile.dps

################################################################################
