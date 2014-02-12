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

#FILE_NAME          appl/fbif/mak/Makefile.mak

#

#DESCRIPTION

#  Generates the Function Block Interface component for the FF protocol software.
#  If needed, the GenVFD tool will be run. By a recusive call the generated
#  C sources will be compiled as well as the according function blocks kernel
#  sources (which are evaluated by the subsequent 'makeffbl.mak' file) and will
#  be combined into one library.

# ===========================================================================

.PHONY: $(COMPONENT) clean_$(COMPONENT) dlv_$(COMPONENT)

USED_MAKEFILES += $(TARGET)/appl/$(COMPONENT)/mak/Makefile.mak

################################################################################
# defaults

COMPONENT = fbif

ifneq ($(STACK_TYPE), FF_BFD)
  ifneq ($(STACK_TYPE), FF_LM)
    $(error Device type ($(STACK_TYPE)) not correct for generating FBIF)
  endif
endif


include $(BASE)/mak/Makeenv.mak

GEN_C    = $(TARGET)/appl/$(COMPONENT)/src
GEN_H    = $(subst /src,/inc,$(GEN_C))
MAK      = $(TARGET)/appl/$(COMPONENT)/mak
GEN_DIRS = $(GEN_C) $(GEN_H)

SRC_DIRS =  $(GEN_C):$(TARGET)/appl/ffbl/src:$(BASE)/appl/ffbl/src
INC_DIRS := $(INC_DIRS):$(subst /src,/inc,$(SRC_DIRS))


vpath %.c   $(SRC_DIRS)
vpath %.h   $(INC_DIRS)
vpath %.gw  $(subst /src,/script,$(SRC_DIRS))
vpath %.dat $(subst /src,/script,$(SRC_DIRS))
vpath %.sym $(subst /src,/ddl,$(GEN_C))


################################################################################
# target files of this component


-include $(BASE)/appl/ffbl/mak/Makeffbl.mak

GEN_SRCS = $(wildcard $(GEN_C)/*.c)
GEN_INCS = $(wildcard $(GEN_H)/*.h)

ifeq ($(STACK_TYPE), FF_LM)
	LAS_SRCS = ffbl_dom.c
else
	LAS_SRCS =
endif

DDL_FILE = $(DEVICE_REV)01.sy5

GEN_VFD_EXE   = $(TARGET)/appl/$(COMPONENT)/script/$(GEN_VFD)

FBL_SRCS = $(LAS_SRCS) $(BLK_SRCS)
SRCS     = $(GEN_SRCS)

FBL_INCS = fbap.h fbif.h ffbl_api.h ffbl_if.h

ifeq ($(FIELD_DIAGNOSTICS), USE_FIELD_DIAGNOSTICS)
  SCRIPTS  = $(MAIN_SCRIPT) $(BLK_SCRIPTS) $(TBLK_SCRIPTS) fd.gw
else
  SCRIPTS  = $(MAIN_SCRIPT) $(BLK_SCRIPTS) $(TBLK_SCRIPTS)
endif

DEPS_SRCS  =  $(SRCS)
OBJECTS  = $(addprefix $(OBJ_DIR)/, $(addsuffix $(OBJ_SUFFIX), $(notdir $(basename $(SRCS)))))
FBL_OBJ  = $(addprefix $(OBJ_DIR)/, $(addsuffix $(OBJ_SUFFIX), $(notdir $(basename $(FBL_SRCS)))))

LIB      = $(LIB_DIR)/$(COMPONENT)$(LIB_SUFFIX)
FBL_LIB  = $(LIB_DIR)/ffbl$(LIB_SUFFIX)
FGEN_LIB = $(LIB_DIR)/fgen$(LIB_SUFFIX)

################################################################################
# dependencies

# Prepare Multi-Device support
DEVICE_TYPES ?= $(DEVICE_TYPE)
MAIN_SCRIPTS ?= $(MAIN_SCRIPT)
DDL_FILES    ?= $(DDL_FILE)

ALL_INDEX = 1 2 3 4 5 6 7 8 9
REAL_INDEX = $(wordlist 1, $(words $(DEVICE_TYPES)), $(ALL_INDEX))

Hide?=@

define RUN_GENVFD
	$(Hide)cd $(TARGET)/appl/$(COMPONENT)/script; \
		$(GEN_VFD) ../inc ../src ../ddl/$(word $(1), $(DEVICE_TYPES))/$(word $(1), $(DDL_FILES)) $(word $(1), $(MAIN_SCRIPTS)) -unpacked_structures
endef


# Generate Firmware

-include $(MAK)/dev_type.mak

$(COMPONENT):
ifneq ($(filter $(DEVICE_TYPES),$(PREVIOUS_DEVICE_TYPES)), $(DEVICE_TYPES))
	$(Hide)rm -f $(MAK)/dev_type.mak
endif
	$(Hide)$(MAKE) -f $(MAK)/Makefile.mak make_fbif

make_fbif: $(OBJECTS) $(LIB)

#$(LIB)      : $(OBJECTS) $(FBL_LIB)
$(LIB)      : $(FGEN_LIB) $(FBL_LIB)

$(FGEN_LIB) : $(OBJECTS)

$(OBJECTS)  : $(USED_MAKEFILES)

$(FBL_OBJ)  : $(USED_MAKEFILES)

$(MAK)/dev_type.mak: $(SCRIPTS) $(USED_MAKEFILES) $(GEN_DIRS) $(GEN_VFD_EXE)
	@echo Processing FBLK scripts
	$(Hide)rm -f $(GEN_C)/*.c
	$(Hide)rm -f $(GEN_H)/*.h
	$(Hide)echo PREVIOUS_DEVICE_TYPES = $(DEVICE_TYPES) > $(MAK)/dev_type.mak
	$(Hide)$(MAKE) -f $(MAK)/Makefile.mak compile_scripts


$(GEN_DIRS):
	-$(Hide)mkdir -p $@

compile_scripts:  $(REAL_INDEX)
	$(Hide)touch $(MAK)/dev_type.mak

$(REAL_INDEX):
	@echo CURDIR=$(CURDIR)
	$(call RUN_GENVFD, $@)
	$(Hide)rm -f $(MAK)/makefile.dps \
           $(TARGET)/appl/fdev/mak/makefile.dps

#-------------------------------------------------------------------------------

ifeq ($(DELIVERY_TYPE), SOURCE)

$(FBL_LIB)  : $(FBL_OBJ)
DEPS_SRCS  +=  $(FBL_SRCS)

endif


ifeq ($(DELIVERY_TYPE), OBJECT)

$(FBL_LIB)  : $(FBL_OBJ)
DEPS_SRCS  +=  $(FBL_SRCS)

endif


ifeq ($(DELIVERY_TYPE), FBK)

ifeq ($(MAKE_ENVIRONMENT), SOFTING)

$(FBL_LIB)  : $(FBL_OBJ)
DEPS_SRCS  +=  $(FBL_SRCS)

else

$(FBL_LIB)  :

endif

endif



#-------------------------------------------------------------------------------
# Clean up

clean_$(COMPONENT):
	$(Hide)echo cleaning $(COMPONENT)
	-$(Hide)rm -f $(LIB) \
						$(FGEN_LIB) \
            $(OBJECTS) \
            $(MAK)/dev_type.mak \
            $(MAK)/makefile.dps
ifeq ($(MAKE_ENVIRONMENT), SOFTING)
	-$(Hide)rm -f $(FBL_LIB) \
            $(FBL_OBJ)
endif


#-------------------------------------------------------------------------------
# Maintain dependencies

$(MAKEDIR)/appl/$(COMPONENT)/mak/makefile.dps: $(DEPS_SRCS)


#-------------------------------------------------------------------------------
# Deliver Sources

-include $(MAKEDIR)/appl/$(COMPONENT)/mak/Makedlv.mak


################################################################################
# source file dependecies

-include $(MAKEDIR)/appl/$(COMPONENT)/mak/makefile.dps

################################################################################
