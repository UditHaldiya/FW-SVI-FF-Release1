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

#FILE_NAME          Makefile.mak

#

#DESCRIPTION

#

# ===========================================================================


.PHONY: $(COMPONENT) clean_$(COMPONENT) deps_$(COMPONENT)

USED_MAKEFILES += $(MAKEDIR)/appl/$(COMPONENT)/mak/Makefile.mak

################################################################################
# defaults

COMPONENT = hm

include $(BASE)/mak/Makeenv.mak

SRC_DIRS =  $(TARGET)/appl/$(COMPONENT)/src:$(BASE)/appl/$(COMPONENT)/src
INC_DIRS := $(INC_DIRS):$(subst /src,/inc,$(SRC_DIRS)):$(TARGET)/STM32F10x_StdPeriph_Driver/inc


vpath %.c   $(SRC_DIRS)
vpath %.h   $(INC_DIRS)


################################################################################
# target files of this component

SRCS = hm.c hm_if.c hm_task.c hm_dma.c

INCS = hm.h hm_dma.h
GINCS = hm_api.h hm_if.h
CFGS = hm_cfg.h

ifeq ($(STACK_TYPE), DP)
  CFGS += pdev_cfg.h
endif

ifeq ($(STACK_TYPE), PA)
	CFGS += pdev_cfg.h
endif

ifeq ($(STACK_TYPE), FF_BFD)
	CFGS += fdev_cfg.h
endif

ifeq ($(STACK_TYPE), FF_LM)
  CFGS += fdev_cfg.h
endif

OBJECTS = $(addprefix $(OBJ_DIR)/, $(addsuffix $(OBJ_SUFFIX), $(basename $(SRCS))))

LIB = $(LIB_DIR)/$(COMPONENT)$(LIB_SUFFIX)

################################################################################
# dependencies

# Generate Firmware

$(COMPONENT): $(LIB)

$(LIB)      : $(OBJECTS)

$(OBJECTS)  : $(USED_MAKEFILES)

#-------------------------------------------------------------------------------
# Clean up

clean_$(COMPONENT):
				@echo cleaning $(COMPONENT)
				-@rm -f $(LIB) \
            $(OBJECTS) \
            $(BASE)/appl/$(COMPONENT)/mak/makefile.dps


#-------------------------------------------------------------------------------
# Maintain dependencies

$(MAKEDIR)/appl/$(COMPONENT)/mak/makefile.dps: $(SRCS)


#-------------------------------------------------------------------------------
# Handle delivery

-include $(MAKEDIR)/appl/$(COMPONENT)/mak/Makedlv.mak


################################################################################
# source file dependecies

-include $(MAKEDIR)/appl/$(COMPONENT)/mak/makefile.dps

################################################################################

