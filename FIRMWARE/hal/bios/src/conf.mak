#HAL/bios/src
#SOURCE, if defined, is a space-separated list of the sources in this directory
ifeq ($(FEATURE_CPU), STM32F10xxE)
SRC1=
SRC1= timer2.c
else
SRC1= timer.c
endif
SRC2=

SOURCE:=$(SRC1) $(SRC2)
ifeq ($(FEATURE_LOCAL_UI),SIMPLE)
SOURCE+=quad.c led.c
endif

#ASOURCE, if defined, is a space-separated list of assembler files
#ASOURCE=
#SUBDIR, if defined,  is a space-separated list of subdirectories with sources
#SUBDIR=
#ISUBDIR, if defined,  is a space-separated list of header subdirectories
#Compiler flags private to this directory are below
CFLAGS_LOCAL= $(LIB_OPTION)
