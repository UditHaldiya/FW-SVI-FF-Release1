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

#FILE_NAME          Makeenv.mak

#

#DESCRIPTION

#

# ===========================================================================

################################################################################
# default variables and rules

USED_MAKEFILES += $(BASE)/mak/Makeenv.mak

include $(BASE)/mak/$(CPU)/Makedefaults.mak
include $(BASE)/mak/$(CPU)/Makerules.mak

-include $(BASE)/mak/Makesofting.mak

################################################################################
# locate makefiles for generating components
# use shell command [ -f ] to check if makefile in TARGET path exist
define MAKE_COMPONENT
	@[ -f $(TARGET)/$(CURRENT_MAKEFILE) ] && MAKEDIR=$(TARGET) || MAKEDIR=$(BASE); \
	$(MAKE) $(MAKEOPTIONS) -f $$MAKEDIR/$(CURRENT_MAKEFILE) $@ MAKEDIR=$$MAKEDIR
endef

################################################################################
# Perform file delivery with adding a certain header to each file
# Syntax: call DELIVER_FILES, LIST_OF_SOURCE_FILES, DESTINATION_DIRECTORY
DELIVER_FILES  = $(foreach SOURCE, $(1), $(shell cat $(CFILEHEADER) $(SOURCE) > $(2)/$(notdir $(SOURCE))) )
DELIVER_MFILES = $(foreach SOURCE, $(1), $(shell cat $(MFILEHEADER) $(SOURCE) > $(2)/$(notdir $(SOURCE))) )

################################################################################
# Checks a list of components for containing any of LIST_OF_BASE and removes
# them, if necessary
# Syntax: call CHECK_COMPONENTS, LIST_OF_ALL_COMPONENTS, LIST_OF_BASE
#ifeq ($(USERDOMAIN), SOFTING)
ifeq ($(MAKE_ENVIRONMENT), SOFTING)
    CHECK_COMPONENTS = $(1)
else
  ifeq ($(DELIVERY_TYPE), SOURCE)
    CHECK_COMPONENTS = $(1)
  else
    CHECK_COMPONENTS = $(filter-out $(2), $(1))
  endif
endif

################################################################################
# Generate the non-ambiguous list of currently used Function Block types
# from the listing as usually specified in MAKECONFIG.MAK
ifneq (,$(findstring ai,$(BLOCKS)))
BLOCK_LIST += AI_BLOCK_AVAILABLE
endif

ifneq (,$(findstring m_a_i,$(BLOCKS)))
BLOCK_LIST += MAI_BLOCK_AVAILABLE
endif

ifneq (,$(findstring ao,$(BLOCKS)))
BLOCK_LIST += AO_BLOCK_AVAILABLE
endif

ifneq (,$(findstring m_a_o,$(BLOCKS)))
BLOCK_LIST += MAO_BLOCK_AVAILABLE
endif

ifneq (,$(findstring di,$(BLOCKS)))
BLOCK_LIST += DI_BLOCK_AVAILABLE
endif

ifneq (,$(findstring m_d_i,$(BLOCKS)))
BLOCK_LIST += MDI_BLOCK_AVAILABLE
endif

ifneq (,$(findstring do,$(BLOCKS)))
BLOCK_LIST += DO_BLOCK_AVAILABLE
endif

ifneq (,$(findstring m_d_o,$(BLOCKS)))
BLOCK_LIST += MDO_BLOCK_AVAILABLE
endif

ifneq (,$(findstring tot,$(BLOCKS)))
BLOCK_LIST += TOT_BLOCK_AVAILABLE
endif

ifneq (,$(findstring ar,$(BLOCKS)))
BLOCK_LIST += AR_BLOCK_AVAILABLE
endif

ifneq (,$(findstring it,$(BLOCKS)))
BLOCK_LIST += IT_BLOCK_AVAILABLE
endif

ifneq (,$(findstring sc,$(BLOCKS)))
BLOCK_LIST += SC_BLOCK_AVAILABLE
endif

ifneq (,$(findstring is,$(BLOCKS)))
BLOCK_LIST += IS_BLOCK_AVAILABLE
endif

ifneq (,$(findstring os,$(BLOCKS)))
BLOCK_LIST += OS_BLOCK_AVAILABLE
endif

ifneq (,$(findstring pid,$(BLOCKS)))
BLOCK_LIST += PID_BLOCK_AVAILABLE
endif

ifneq (,$(findstring sdt,$(BLOCKS)))
BLOCK_LIST += SDT_BLOCK_AVAILABLE
endif

ifneq (,$(findstring cs,$(BLOCKS)))
BLOCK_LIST += CS_BLOCK_AVAILABLE
endif

#export BLOCK_LIST


################################################################################
# Prevent any .DEFAULT rule to generate messages if .mak files do not exist
%.mak: ;

