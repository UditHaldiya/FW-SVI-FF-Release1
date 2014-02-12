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

#FILE_NAME          Makefile

#

#DESCRIPTION

#

# ===========================================================================
.PHONY: all clean deliver\
        ff clean_ff \
        ff_svi_release clean_ff_svi_release dlv_ff_svi_release \
        ff_svi_debug clean_ff_svi_debug dlv_ff_svi_debug \
        ff_dld_release clean_ff_dld_release dlv_ff_dld_release \
        ff_dld_debug clean_ff_dld_debug dlv_ff_dld_debug

################################################################################
# defaults

MAKEOPTIONS = --no-print-directory -r
ifeq (, $(findstring ', $(MAKE)))
  export MAKE := '$(MAKE)'
endif

#-------------------------------------------------------------------------------
# Prepare delivery
export DLV_TO ?= $(TEMP)/Delivery_FF_PA
DLV_DESTINATION = $(subst \,/,$(DLV_TO))


################################################################################


all:       ff
clean:     clean_ff
deliver:   deliver_ff

ff:        ff_release ff_debug
clean_ff:  clean_ff_release clean_ff_dbg clean_ff_dld_release clean_ff_dld_debug
deliver_ff: dlv_ff_svi_release dlv_ff_svi_debug dlv_ff_dld_release dlv_ff_dld_debug

ff_release: ff_svi_release ff_dld_release
ff_dbg:     ff_svi_debug ff_dld_debug
clean_ff_release: clean_ff_svi_release clean_ff_dld_release
clean_ff_dbg:     clean_ff_svi_debug clean_ff_dld_debug

release:    ff_release

################################################################################
#                           PA Targets                                         #
################################################################################



################################################################################
#                           FF Targets                                         #
################################################################################

ff_dld_release:
	@$(MAKE) $(MAKEOPTIONS) -C ff_dld_rel all
clean_ff_dld_release:
	@$(MAKE) $(MAKEOPTIONS) -C ff_dld_rel clean

ff_dld_dbg:
	@$(MAKE) $(MAKEOPTIONS) -C ff_dld_dbg all
clean_ff_dld_dbg:
	@$(MAKE) $(MAKEOPTIONS) -C ff_dld_dbg clean

ff_svi_release:
	@$(MAKE) $(MAKEOPTIONS) -C ff_svi_rel all
clean_ff_svi_release:
	@$(MAKE) $(MAKEOPTIONS) -C ff_svi_rel clean
dlv_ff_svi_release: ff_svi_release deliver_makefile
	@$(MAKE) $(MAKEOPTIONS) -C ff_svi_rel deliver

ff_svi_debug:
	@$(MAKE) $(MAKEOPTIONS) -C ff_svi_dbg all
clean_ff_svi_debug:
	@$(MAKE) $(MAKEOPTIONS) -C ff_svi_dbg clean
dlv_ff_svi_debug: ff_svi_debug deliver_makefile
	@$(MAKE) $(MAKEOPTIONS) -C ff_svi_dbg deliver


#-------------------------------------------------------------------------------
deliver_makefile: check_dir
	$(shell cat dlv_hdrM.txt makefile > $(DLV_DESTINATION)/target/mak/makefile)

check_dir:
	-@mkdir -p $(DLV_DESTINATION)/target/mak




#-------------------------------------------------------------------------------

all clean release \
pa pa_emu pa_osview clean_pa_emu clean_pa_osview  \
ff ff_r_emu ff_r_osview clean_ff_r_emu clean_ff_r_osview ff_r_dbg clean_ff_r_dbg \
ff_m_emu clean_ff_m_emu ff_m_release clean_ff_m_release \
deliver: init

################################################################################

init:
  ifneq ($(MAKELEVEL), 0)
	@echo "FD protocol software on SFD / M16C"
  endif

#-------------------------------------------------------------------------------

.DEFAULT:
	@echo "                                                          "
	@echo " Make error: unknown target '$@'                          "
	@echo "                                                          "
	@echo " Usage: make [target]                                     "
	@echo "                                                          "
	@echo " Targets:                                                 "
	@echo "                                                          "
	@echo "             [vv][xx][yy][zz]                             "
	@echo "                                                          "
	@echo "   [vv]  =  clean_  | deliver_ | (build)                  "
	@echo "   [xx]  =  ff_     |  pa_     | pa_fbk_ | ff_fbk_        "
	@echo "   [yy]  =  r_      |  m_      |  j_                      "
	@echo "   [yy]  =  rfbk_   |  mm_     |  hm_                     "
	@echo "   [zz]  =  release |  osview  |  dbg  |  emu  |  libs    "
	@echo "                                                          "
	@echo "   Note(!):    [yy] only if [xx] = ff_                    "
	@echo "                                                          "
	@echo "   Examples:       pa_release                             "
	@echo "                   clean_ff_m_releas                      "
	@echo "                   deliver_ff_r_emu                       "
	@echo "                   pa_fbk_hm_release                      "
	@echo "                   clean_ff_fbk_hm_dbg                    "
	@echo "                   dlv_ff_fbk_mm_release                  "
	@echo "                                                          "
	@echo "   Also:                                                  "
	@echo "         all     =  ff_r_release + pa_release             "
	@echo "         clean   =  clean_ff_r_release + clean_pa_release "
	@echo "         emu     =  ff_r_emu + pa_emu                     "
	@echo "         release =  ff_r_release + pa_release             "
	@echo "         deliver =  dlv_ff_r_release + deliver_pa_release "
	@echo "                                                          "


