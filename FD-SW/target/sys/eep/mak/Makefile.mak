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

#FILE_NAME          Makefile.mak

#

#DESCRIPTION

#  Generates the EEPROM interface component for for the Softing FD software,
#  not depending on the fieldbus protocol (FF or PROFIBUS-PA).

# ===========================================================================

.PHONY: $(COMPONENT) clean_$(COMPONENT) deps_$(COMPONENT) dlv_$(COMPONENT)

USED_MAKEFILES += $(TARGET)/sys/$(COMPONENT)/mak/Makefile.mak

################################################################################
# defaults

COMPONENT = eep

include $(BASE)/mak/Makeenv.mak

SRC_DIRS  =  $(TARGET)/sys/$(COMPONENT)/src:$(BASE)/sys/$(COMPONENT)/src
INC_DIRS  := $(INC_DIRS):$(subst /src,/inc,$(SRC_DIRS))

vpath %.c   $(SRC_DIRS)
vpath %.h   $(INC_DIRS)


################################################################################
# target files of this component

SRCS  = eep_if.c
INCS  = eep_hw.h
GINCS = eep_if.h

include $(TARGET)/sys/$(COMPONENT)/mak/mn.mak

$(info MNSOURCE=$(MNSOURCE))
$(info MNINCLLUDE=$(MNINCLLUDE))

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
            $(TARGET)/sys/$(COMPONENT)/mak/makefile.dps

#-------------------------------------------------------------------------------
# Maintain dependencies

$(MAKEDIR)/sys/$(COMPONENT)/mak/makefile.dps: $(SRCS)


#-------------------------------------------------------------------------------
# Deliver Sources

-include $(MAKEDIR)/sys/$(COMPONENT)/mak/Makedlv.mak


################################################################################
# source file dependecies

-include $(MAKEDIR)/sys/$(COMPONENT)/mak/makefile.dps

################################################################################
