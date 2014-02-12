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

Hide?=@

# ===========================================================================

#FILE_NAME          Std_makefile.mak

#

#DESCRIPTION

#   This is the 'standard' (= common) makefile to generate the Softing
#   Field Device Software. In general, it's valid for all protocols and
#   variants. If special adoptions have to be made for certain variants,
#   copy this file to the variant specific 'makefile' and enter any
#   adaptions as required.
#   Any changes here will affect all variants!

#   Caution! All relativ directory paths are as seen from the calling makefile!

# ===========================================================================


.PHONY: init check_environment \
        all clean deliver \
        dev_pa dev_dpusr dev_bfd dev_uta libs_only \
        $(ALL_COMPS) $(CLEAN_SUBCOMPONENTS) $(DLV_SUBCOMPONENTS)

################################################################################
# defaults

MAKEOPTIONS = --no-print-directory -r -s
export USED_MAKEFILES += ../std_makefile.mak

ifeq (, $(findstring ', $(MAKE)))
  export MAKE := '$(MAKE)'
endif


################################################################################
# configure target
# all 'standard' settings are to be made in Makeconfig.mak

include Makeconfig.mak


################################################################################
# Default variables and rules
# Usually NOT subject to changes!

export BASE     = ../../../base
export TARGET   = ../..

include $(BASE)/mak/Makeenv.mak

vpath %$(OBJ_SUFFIX) $(OBJ_DIR)
vpath %$(LIB_SUFFIX) $(LIB_DIR)

ifeq ($(STACK_TYPE),DP)
PROT_INC        = pa
PROT_DIR        = pac
endif
ifeq ($(STACK_TYPE),PA)
PROT_INC        = pa
PROT_DIR        = pac
endif
ifeq ($(STACK_TYPE),FF_BFD)
PROT_INC        = ff
PROT_DIR        = fdc
endif
ifeq ($(STACK_TYPE),FF_LM)
PROT_INC        = ff
PROT_DIR        = fdc
endif

OSIF_INC = $(TARGET)/sys/$(OSIF)/inc:$(BASE)/sys/$(OSIF)/inc:../_embOS_CortexM_IAR/Start/Inc
CFG_INC  = $(TARGET)/cfg
GLB_INC  = $(TARGET)/inc:$(TARGET)/inc/$(PROT_INC):$(TARGET)/inc/segm \
           $(BASE)/inc:$(BASE)/inc/$(PROT_INC):$(BASE)/$(PROT_DIR)/inc

export INC_DIRS = $(CFG_INC):$(GLB_INC):$(OSIF_INC)

export LST_DIR  = lst
export OBJ_DIR  = obj
export LIB_DIR  = lib
RELEASE_LIB_DIR = ../FF_SVI_REL/$(LIB_DIR)
#RELEASE_LIB_DIR = $(LIB_DIR)

export EXE_DIR  = $(EXECUTABLE_LOC)

GEN_DIRS        = $(LST_DIR) $(OBJ_DIR) $(LIB_DIR) $(EXE_DIR)

PA_LIBS         = $(addprefix $(LIB_DIR)/, $(addsuffix $(LIB_SUFFIX), $(PA_COMPS)))
DUSR_LIBS       = $(addprefix $(LIB_DIR)/, $(addsuffix $(LIB_SUFFIX), $(DUSR_COMPS)))

#Always link against Release version of libraries
BFD_LIBS        = $(addprefix $(LIB_DIR)/, $(addsuffix $(LIB_SUFFIX), $(FFBS_COMPS_BUILD))) \
				$(addprefix $(RELEASE_LIB_DIR)/, $(addsuffix $(LIB_SUFFIX), $(BASE_FFBS)))
UTA_LIBS        = $(addprefix $(LIB_DIR)/, $(addsuffix $(LIB_SUFFIX), $(UTA_COMPS_BUILD))) \
				$(addprefix $(RELEASE_LIB_DIR)/, $(addsuffix $(LIB_SUFFIX), $(BASE_UTA)))

$(warning UTA_LIBS=$(UTA_LIBS))
$(warning BFD_LIBS=$(BFD_LIBS))

C_LIB           = $(CC_LIB)/dl7M_tln.a $(CC_LIB)/shb_l.a
OS              = '$(OS_LIB)'/os7m_tl__$(EMBOS_MODE).a

SYSTEM_LIBS     = $(C_LIB) $(OS)

PA_FILE         = $(EXE_DIR)/$(PA_EXECUTABLE)
DPUSR_FILE      = $(EXE_DIR)/$(DPUSR_EXECUTABLE)
BFD_FILE        = $(EXE_DIR)/$(BFD_EXECUTABLE)
UTA_FILE        = $(EXE_DIR)/$(UTA_EXECUTABLE)
EXE_FILES       = $(addprefix $(EXE_DIR)/, $(BFD_EXECUTABLE) $(UTA_EXECUTABLE) $(PA_EXECUTABLE) $(DPUSR_EXECUTABLE))
LINK_REPORT_FILES   = $(addsuffix $(LR_SUFFIX), $(basename $(EXE_FILES)))


#-------------------------------------------------------------------------------
# Components to be cleaned up

CLEAN_SUBCOMPONENTS = $(addprefix clean_, $(ALL_COMPS))
ifeq ($(STACK_TYPE),DP)
CLEAN_BASE          = $(addprefix clean_, $(BASE_PFBS) $(BASE_DUSR))
endif
ifeq ($(STACK_TYPE),PA)
CLEAN_BASE          = $(addprefix clean_, $(BASE_PFBS) $(BASE_DUSR))
endif
ifeq ($(STACK_TYPE),FF_BFD)
  ifeq ($(SW_DOWNLOAD_TYPE),SW_DOWNLOAD_DEV)
    CLEAN_BASE          = $(addprefix clean_, $(BASE_BFD))
  else
    CLEAN_BASE          = $(addprefix clean_, $(BASE_FFBS) $(BASE_UTA))
  endif
endif
ifeq ($(STACK_TYPE),FF_LM)
  ifeq ($(SW_DOWNLOAD_TYPE),SW_DOWNLOAD_DEV)
    CLEAN_BASE          = $(addprefix clean_, $(BASE_BFD))
  else
    CLEAN_BASE          = $(addprefix clean_, $(BASE_FFBS) $(BASE_UTA))
  endif
endif


################################################################################
# dependencies

#-------------------------------------------------------------------------------
# Initialization

all clean libs_only $(SUBCOMPONENTS) $(CLEAN_SUBCOMPONENTS): init

all dev_bfd dev_uta dev_pa dev_dpusr libs_only: check_environment $(GEN_DIRS)


#-------------------------------------------------------------------------------
# Generate software

# Do not attempt to generate FF parts and PA parts at the same time! This will
# result in a conflict with 'base.lib', which has the identical name for either
# the FF protocol as well as the PA protocol kernel!
ifeq ($(STACK_TYPE), DP)
  all: dev_dpusr dev_pa
endif

ifeq ($(STACK_TYPE), PA)
  all: dev_dpusr dev_pa
  libs_only: $(PA_LIBS) $(DUSR_LIBS) $(USED_MAKEFILES)
endif

ifeq ($(STACK_TYPE), FF_BFD)
  ifeq ($(SW_DOWNLOAD_TYPE), SW_DOWNLOAD_DEV)
    all: dev_bfd
    libs_only: $(BFD_LIBS) $(USED_MAKEFILES)
  else
    all: dev_uta dev_bfd
    libs_only: $(BFD_LIBS) $(UTA_LIBS) $(USED_MAKEFILES)
  endif
endif

ifeq ($(STACK_TYPE), FF_LM)
  ifeq ($(SW_DOWNLOAD_TYPE), SW_DOWNLOAD_DEV)
    all: dev_bfd
    libs_only: $(BFD_LIBS) $(USED_MAKEFILES)
  else
    all: dev_uta dev_bfd
    libs_only: $(BFD_LIBS) $(UTA_LIBS) $(USED_MAKEFILES)
  endif
endif

dev_bfd:   $(BFD_FILE)
dev_uta:   $(UTA_FILE)
dev_pa:    $(PA_FILE)
dev_dpusr: $(DPUSR_FILE)

$(PA_FILE):    $(PA_LIBS)   $(LINK_FILE) $(USED_MAKEFILES)
$(DPUSR_FILE): $(DUSR_LIBS) $(LINK_FILE) $(USED_MAKEFILES)
$(BFD_FILE):   $(BFD_LIBS)  $(LINK_FILE) $(USED_MAKEFILES)
$(UTA_FILE):   $(UTA_LIBS)  $(LINK_FILE) $(USED_MAKEFILES)

# Generate dependencies in form of
#    lib/xxx.lib:  xxx
# but with respect to components that may be not existent in source code
# Duplicate entries are removed by using 'sort'
GEN_LIBS = $(addprefix $(LIB_DIR)/, $(addsuffix $(LIB_SUFFIX), \
                                                $(call CHECK_COMPONENTS,$(UTA_COMPS),$(BASE_UTA))   \
                                                $(call CHECK_COMPONENTS,$(BFD_COMPS),$(BASE_FFBS))  \
                                                $(call CHECK_COMPONENTS,$(PA_COMPS),$(BASE_PFBS))   \
                                                $(call CHECK_COMPONENTS,$(DUSR_COMPS),$(BASE_DUSR)) \
                                     ) \
            )
$(sort $(GEN_LIBS)):   $(LIB_DIR)/%$(LIB_SUFFIX):  %
	@#nothing to be done here

$(GEN_DIRS):
	-@mkdir -p $@

#-------------------------------------------------------------------------------
# Clean file structure

clean: $(call CHECK_COMPONENTS,$(CLEAN_SUBCOMPONENTS),$(CLEAN_BASE))
	@echo
	@echo cleaning target $(TARGET_NAME)
	-@rm -f $(EXE_FILES) \
            $(LINK_REPORT_FILES) \
            $(LST_DIR)/*.* \
            $(OBJ_DIR)/*.*

#-------------------------------------------------------------------------------
# Compile delivery

-include ../Makedlv.mak


################################################################################
# components
#-------------------------------------------------------------------------------

hw dlv_hw clean_hw:			CURRENT_MAKEFILE = hw/mak/Makefile.mak

#-------------------------------------------------------------------------------

STM32F10x_StdPeriph_Driver dlv_STM32F10x_StdPeriph_Driver clean_STM32F10x_StdPeriph_Driver:			CURRENT_MAKEFILE = STM32F10x_StdPeriph_Driver/mak/Makefile.mak

#-------------------------------------------------------------------------------

dbg dlv_dbg clean_dbg:		CURRENT_MAKEFILE = sys/dbg/mak/Makefile.mak

#-------------------------------------------------------------------------------

eep dlv_eep clean_eep:		CURRENT_MAKEFILE = sys/eep/mak/Makefile.mak

#-------------------------------------------------------------------------------

osif.emb dlv_osif.emb clean_osif.emb:	CURRENT_MAKEFILE = sys/osif.emb/mak/Makefile.mak

#-------------------------------------------------------------------------------

pbif dlv_pbif clean_pbif:	CURRENT_MAKEFILE = appl/pbif/mak/Makefile.mak

#-------------------------------------------------------------------------------

pdev dlv_pdev clean_pdev:	CURRENT_MAKEFILE = appl/pdev/mak/Makefile.mak

#-------------------------------------------------------------------------------

fbif dlv_fbif clean_fbif:	CURRENT_MAKEFILE = appl/fbif/mak/Makefile.mak

#-------------------------------------------------------------------------------

ifneq ($(SW_DOWNLOAD_TYPE), SW_DOWNLOAD_DEV)
fdev: fbif
endif

fdev dlv_fdev clean_fdev:	CURRENT_MAKEFILE = appl/fdev/mak/Makefile.mak

#-------------------------------------------------------------------------------

hm dlv_hm clean_hm:		CURRENT_MAKEFILE = appl/hm/mak/Makefile.mak

#-------------------------------------------------------------------------------

mm dlv_mm clean_mm:		CURRENT_MAKEFILE = appl/mm/mak/Makefile.mak

#-------------------------------------------------------------------------------

ssc dlv_ssc clean_ssc:		CURRENT_MAKEFILE = appl/ssc/mak/Makefile.mak

#-------------------------------------------------------------------------------

stub clean_stub dlv_stub:	CURRENT_MAKEFILE = stub/mak/Makefile.mak

#-------------------------------------------------------------------------------

base clean_base dlv_base: 	CURRENT_MAKEFILE = mak/Makefile.mak

ffbs clean_ffbs dlv_ffbs: 	CURRENT_MAKEFILE = mak/Makefile.mak

uta  clean_uta  dlv_uta:	CURRENT_MAKEFILE = mak/Makefile.mak

pfbs clean_pfbs dlv_pfbs: 	CURRENT_MAKEFILE = mak/Makefile.mak

dpusr clean_dpusr dlv_dpusr:	CURRENT_MAKEFILE = mak/Makefile.mak

#-------------------------------------------------------------------------------

$(ALL_COMPS) $(CLEAN_SUBCOMPONENTS) $(DLV_SUBCOMPONENTS):
	$(MAKE_COMPONENT)


################################################################################

init: check_exec_names
  ifneq ($(MAKELEVEL), 0)
	@echo ""
	@echo "*******************************************************************"
	@echo ""
	@echo $(MESSAGE)
	@echo ""
  endif

check_exec_names:
ifneq "$(origin BFD_EXECUTABLE)" "undefined"
  ifeq ($(BFD_EXECUTABLE), $(UTA_EXECUTABLE))
    $(error The executables have the same name!)
  endif
endif

ifneq "$(origin PA_EXECUTABLE)" "undefined"
  ifeq ($(PA_EXECUTABLE), $(DPUSR_EXECUTABLE))
    $(error The executables have the same name!)
  endif
endif

#-------------------------------------------------------------------------------

.DEFAULT:
	@echo "Make error: unknown target '$@'"
	@echo "Usage: make [target]"
	@echo "Targets: "
	@echo "  (all) dev_bfd dev_uta "
	@echo "  clean deliver"

