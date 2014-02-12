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

#  Entry point to generate the Protocol Software units. Depending on the
#  protocol type of the device, it branches to the FDC or PAC part by calling
#  the according makefile.

# ===========================================================================

.PHONY: init check_environment \
        all base clean clean_base deliver dlv_base \
        pfbs dpusr clean_pfbs clean_dpusr dlv_pfbs dlv_dpusr \
        ffbs uta clean_ffbs clean_uta dlv_ffbs dlv_uta \
        pa_all pa_base pa_fbs pa_dusr clean_pa dlv_pa \
        ff_all ff_base ff_fbs ff_uta clean_ff dlv_ff

USED_MAKEFILES += $(BASE)/mak/Makefile.mak

################################################################################
# default variables and rules


################################################################################
# dependencies

ifeq ($(STACK_TYPE), DP)

all:          pa_all
base:         pa_base
pfbs:         pa_fbs
dpusr:        pa_dusr
clean:        clean_pa
clean_base:   clean_pa
deliver:      dlv_pa
dlv_base:     dlv_pa

clean_dpusr dlv_dpusr:
	@# do nothing, don't print any message
clean_pfbs dlv_pfbs:
	@# do nothing, don't print any message


ffbs uta:
	@echo "Make error: target '$@' not matching stack_type $(STACK_TYPE)"
endif

ifeq ($(STACK_TYPE), PA)

all:          pa_all
base:         pa_base
pfbs:         pa_fbs
dpusr:        pa_dusr
clean:        clean_pa
clean_base:   clean_pa
deliver:      dlv_pa
dlv_base:     dlv_pa

clean_dpusr dlv_dpusr:
	@# do nothing, don't print any message
clean_pfbs dlv_pfbs:
	@# do nothing, don't print any message


ffbs uta:
	@echo "Make error: target '$@' not matching stack_type $(STACK_TYPE)"
endif

ifeq ($(STACK_TYPE), FF_BFD)

all:          ff_all
base:         ff_base
ffbs:         ff_fbs
uta:          ff_uta
clean:        clean_ff
clean_base:   clean_ff
deliver:      dlv_ff
dlv_base:     dlv_ff

clean_uta dlv_uta:
	@# do nothing, don't print any message
clean_ffbs dlv_ffbs:
	@# do nothing, don't print any message

pfbs dusr:
	@echo "Make error: target '$@' not matching stack_type $(STACK_TYPE)"

endif

ifeq ($(STACK_TYPE), FF_LM)

all:          ff_all
base:         ff_base
ffbs:         ff_fbs
uta:          ff_uta
clean:        clean_ff
clean_base:   clean_ff
deliver:      dlv_ff
dlv_base:     dlv_ff

clean_uta dlv_uta:
	@# do nothing, don't print any message
clean_ffbs dlv_ffbs:
	@# do nothing, don't print any message

pfbs dusr:
	@echo "Make error: target '$@' not matching stack_type $(STACK_TYPE)"

endif


################################################################################
# components

#-------------------------------------------------------------------------------
# PA communication layer components
pa_all:
	@$(MAKE) $(MAKEOPTIONS) -f $(BASE)/pac/mak/Makefile.mak all
pa_base:
	@$(MAKE) $(MAKEOPTIONS) -f $(BASE)/pac/mak/Makefile.mak base_lib
pa_fbs:
	@$(MAKE) $(MAKEOPTIONS) -f $(BASE)/pac/mak/Makefile.mak pfbs_lib
pa_dusr:
	@$(MAKE) $(MAKEOPTIONS) -f $(BASE)/pac/mak/Makefile.mak dpusr_lib
clean_pa dlv_pa:
	@$(MAKE) $(MAKEOPTIONS) -f $(BASE)/pac/mak/Makefile.mak $@

#-------------------------------------------------------------------------------
# FF communication layer components

ff_all:
	@$(MAKE) $(MAKEOPTIONS) -f $(BASE)/fdc/mak/Makefile.mak all
ff_base:
	@$(MAKE) $(MAKEOPTIONS) -f $(BASE)/fdc/mak/Makefile.mak base_lib
ff_fbs:
	@$(MAKE) $(MAKEOPTIONS) -f $(BASE)/fdc/mak/Makefile.mak ffbs_lib
ff_uta:
	@$(MAKE) $(MAKEOPTIONS) -f $(BASE)/fdc/mak/Makefile.mak uta_lib
clean_ff dlv_ff:
	@$(MAKE) $(MAKEOPTIONS) -f $(BASE)/fdc/mak/Makefile.mak $@

#-------------------------------------------------------------------------------



################################################################################

.DEFAULT:
	@echo "Make error: unknown target '$@'"
	@echo "Usage: make [target]"
	@echo "Targets: "
	@echo " (all) base clean deliver"
	@echo " clean_base dlv_base"
	@echo " pfbs dpusr ffbs uta"

