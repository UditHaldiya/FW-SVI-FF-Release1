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

#FILE_NAME          mak/m16c/makedefaults.mak

#

#DESCRIPTION

#  Contains path and file names as well as some default settings concerned
#  to the M16C development environment when run with the Softing FD software

# ===========================================================================

USED_MAKEFILES += $(BASE)/mak/$(CPU)/Makedefaults.mak

################################################################################
# default values for IAR M16C

ifneq "$(origin CC_CORTEX_M3)" "undefined"
    CC_CORTEX_M3_DEFINED = 1
endif

ifneq "$(origin EMBOS)" "undefined"
    EMBOS_DEFINED = 1
endif

export CC_BIN          = $(CC_CORTEX_M3)/bin
export CC_INC          = '$(CC_CORTEX_M3)'/inc

export CC_SRC          = '$(CC_CORTEX_M3)'/src/lib/dlib
export CC_LIB          = '$(CC_CORTEX_M3)'/lib

export OS_INC          = '$(EMBOS)'/inc
export OS_LIB          = '$(EMBOS)'/lib

export CC              = $(CC_BIN)/iccarm.exe
export ASSEMBLER       = $(CC_BIN)/iasmarm.exe
export LIBRARY_MANAGER = $(CC_BIN)/iarchive.exe
export LINKER          = $(CC_BIN)/ilinkarm.exe
export ILINK_DFLTDIR   = $(CC_LIB)
export HEXCONV         = $(CC_BIN)/ielftool.exe

ifeq ($(STACK_TYPE),DP)
export GEN_VFD         = ./genVFD_PA.exe
endif
ifeq ($(STACK_TYPE),PA)
export GEN_VFD         = ./genVFD_PA.exe
endif
ifeq ($(STACK_TYPE),FF_BFD)
export GEN_VFD         = ./genVFD_FF.exe
endif
ifeq ($(STACK_TYPE),FF_LM)
export GEN_VFD         = ./genVFD_FF.exe
endif

export EXE_SUFFIX      = .elf
export LIB_SUFFIX      = .a
export OBJ_SUFFIX      = .o
export LR_SUFFIX       = .map


################################################################################
# target default make settings
#Flags
#--cpu_mode - select default mode for functions (arm or a(default); thumb or t)
#--silent   - suppress introductory messages and final statistics report
#--section  - place code or data in different address ranges
#--cpu      - processor variant for which the code is to be generated
#-D         - definition of preprocessor symbol
#-r         - make the compiler include information in the object modules for debuggers (also --debug)
#-e         - use language extensions such as extended keywords and anonymous structs and unions in source code
#--endian   - to specify the byte order of the generated code and data (little or l (default); big or b)
#[--header_context - list for each diagnostic message, not only the source position of the problem, but also the entire include stack at that point]
#-I         - to specify the search paths for #include files
#-l         - to generate an assembler or C/C++ listing to a file (A - Assembler list file with C or C++ source as comments; N - No diagnostics in file; H - Include source lines from header files in output. Without this option, only source lines from the primary source file are included)
#--no_const_align - make the compiler align const objects based on the alignment of their type otherwise the compiler uses alignment 4 for objects with a size of 4 bytes or more


export CCFLAGS              = --silent -e --endian=l --cpu=Cortex-M3 --cpu_mode=t --interwork --no_const_align --warnings_are_errors\
															--section .text=$(basename $(COMPONENT))_code\
															$(patsubst %,-I%/,$(subst :, ,$(INC_DIRS))) \
                              -Dexception_handler=default_exception_handler \
                              -I$(CC_INC)/c/ -I$(CC_INC)/ST/ -I$(CC_SRC)/ -I$(OS_INC)/ -I$(OS_PORT)/ \
                              --dlib_config $(CC_INC)/c/DLib_Config_Normal.h

export LIST_OPTION          = -lANH $(LST_DIR)/$(basename $(notdir $@))$(LST_SUFFIX)

export ASSEMBLERFLAGS       = -S -I$(TARGET)/cfg/ -I$(TARGET)/inc/ -I$(BASE)/inc/ -r --endian l --cpu Cortex-M3


export LIBRARY_MANAGERFLAGS =

export LINKERFLAGS          = --silent --cpu=Cortex-M3 #--enable_stack_usage


# predefine format options for ILINKARM
%.elf :       FORMAT_OPTION =
%.hex :       FORMAT_OPTION = --ihex
%.mot :       FORMAT_OPTION = --srec


export LIBRIAN_CMD_FILE     = Dresser.xcl

export MAKEOPTIONS          = --no-print-directory -r

################################################################################
# check environment variables and retrieve version information

first_target: all

check_environment:
  ifeq "$(origin CC_CORTEX_M3_DEFINED)" "undefined"
	@echo "Please set environment variable 'CC_CORTEX_M3' to a valid compiler path"
  endif

  ifeq "$(origin EMBOS_DEFINED)" "undefined"
	@echo "Please set environment variable 'EMBOS' to a valid embOS (Segger) path"
  endif

################################################################################
