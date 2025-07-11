#framework
#SOURCE, if defined, is a space-separated list of the sources in this directory
SOURCE:=mncbdefs.c

ASOURCE+=endofimage.s79
#SUBDIR, if defined,  is a space-separated list of subdirectories with sources
SUBDIR=bios
#ISUBDIR, if defined,  is a space-separated list of header subdirectories

ifeq ($(FEATURE_CPU),STM32F10xxE)
ASOURCE+=mncortexm.s79 cortextrap.s79
SOURCE+=cortextrapc.c verinfo.c
endif

#Compiler flags private to this directory are below
#CFLAGS_LOCAL= $(LIB_OPTION)