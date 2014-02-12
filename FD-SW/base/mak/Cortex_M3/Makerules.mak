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


Hide?=@


# ===========================================================================

#FILE_NAME          mak/m16c/makerules.mak

#

#DESCRIPTION

#  Provides the rules to generate M16C software from the Softing FD sources.

# ===========================================================================

include Makesettings.mak

export MAKERULES = ARM

USED_MAKEFILES += $(BASE)/mak/$(CPU)/Makerules.mak


################################################################################
# implicit rules for IAR M16C

###---   Compile C-Source to Object-File   ----------------------------------###

$(OBJ_DIR)/%$(OBJ_SUFFIX) : %.c
	@echo
	@echo "compiling $<"
	@echo ../FD-SW/target/mak/FF_SVI_DBG/$< >>allfiles.lnt
	@'$(CC)' $(CCFLAGS) $< -o $(OBJ_DIR)/$(@F)


###---   Assemble to Object-File   ------------------------------------------###

define ASSEMBLE_ARM
	@echo
	@echo "assembling $<"
	@'$(ASSEMBLER)' $(ASSEMBLERFLAGS) -o $(OBJ_DIR)/$(@F) -l lst/$(notdir $*).lst $<
endef

$(OBJ_DIR)/%$(OBJ_SUFFIX) : %.s
	$(ASSEMBLE_ARM)


###---   Generate Library   -------------------------------------------------###

define LIB_FROM_OBJ
	@echo "building static library from objects"
	@echo "$(LIB_DIR)/$(@F)" > $(LIBRIAN_CMD_FILE)
	$(foreach OBJMODULE, $(filter %$(OBJ_SUFFIX), $^),\
	    `echo "$(OBJMODULE)" >> $(LIBRIAN_CMD_FILE)` \
	 )
	@echo "build lib"
	@'$(LIBRARY_MANAGER)' $(LIBRARY_MANAGERFLAGS) -f $(LIBRIAN_CMD_FILE)
	@rm -f $(LIBRIAN_CMD_FILE)
	@echo "put obj in txt"
	@echo $(filter %$(OBJ_SUFFIX), $^) > $(subst .a,.txt,$(LIB_DIR)/$(@F))
endef

define LIB_FROM_LIB
	@echo "building static library from libs"
	@echo "$(LIB_DIR)/$(@F)" > $(LIBRIAN_CMD_FILE)
	$(foreach LIBMODULE, $(filter %$(LIB_SUFFIX), $^),\
	    `echo "$(shell cat $(subst .a,.txt,$(LIBMODULE)))" >> $(LIBRIAN_CMD_FILE)` \
	 )
	@'$(LIBRARY_MANAGER)' $(LIBRARY_MANAGERFLAGS) -f $(LIBRIAN_CMD_FILE)
	@rm -f $(LIBRIAN_CMD_FILE)
endef



%.a :
	@echo
	@echo "building static library $@"
	@rm -f $(LIB_DIR)/$(@F)
	@$(if $(filter %$(OBJ_SUFFIX), $^), \
	   $(LIB_FROM_OBJ) \
	  )
	@$(if $(filter %$(LIB_SUFFIX), $^), \
	   $(LIB_FROM_LIB) \
	  )


###---   Link to Executable   -----------------------------------------------###
rombank?=0
LINKERFLAGS+= --config_def rombank=$(rombank)
Hide?=@

define LINK_ARM
	@echo
	@echo "linking $@"
	$(Hide)'$(LINKER)' $(LINKERFLAGS) --map $*.map --log modules --log_file $*.txt --config $(filter %.icf, $^) -o $@ $(filter %$(LIB_SUFFIX), $^) $(filter %$(OBJ_SUFFIX), $^) $(SYSTEM_LIBS)
endef


define CONV_ELF
	@echo
	@echo "convert $@ to .mot"
	@'$(HEXCONV)' --srec $@  $(subst .elf,.mot,$(EXECUTABLE_LOC)/$(@F))
endef

%.elf :
	$(LINK_ARM) 
	$(CONV_ELF)
	
%.out :
	$(LINK_ARM)	

%.hex :
	$(HEXCONV) $(FORMAT_OPTION) $%.elf $%.hex

%.mot :
	$(HEXCONV) $(FORMAT_OPTION) $%.elf $%.mot


###---   Maintain Dependecies in Makefiles   --------------------------------###
#Make sure home directories of source files are searched

%.dps :
	@echo Updating makefile $@
	@rm -f $@
	@echo MNSOURCE=$(MNSOURCE)
	$(Hide)gcc -MM -w $(filter -D%, $(CCFLAGS)) -D__IAR_SYSTEMS_ICC $(patsubst %,-I%,$(subst :, ,$(INC_DIRS))) $(addprefix -I,$(dir $(filter %.c, $^))) -I- $(filter %.c, $^) \
		| sed 's/^\(.*\)\.o:/\$$(OBJ_DIR)\/\1\$$(OBJ_SUFFIX):/' > $@


###############################################################################

