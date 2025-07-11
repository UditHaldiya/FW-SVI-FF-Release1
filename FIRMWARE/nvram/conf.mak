#'nvram' directory conf.mak
UFILES_C=
#SOURCE, if defined, is a space-separated list of the sources in this directory
SOURCE:=
#nvram.c confchanged.c

ifeq ($(FEATURE_LOGFILES),USED)
SOURCE+=logfile.c
endif

#SUBDIR, if defined,  is a space-separated list of subdirectories with sources
#ISUBDIR, if defined,  is a space-separated list of header subdirectories

#We need to fix it when we take new NVMEM. Until now, access nvramtable.h as is
ISUBDIR:=project_$(PROJ)

#Compiler flags private to this directory are below
CFLAGS_LOCAL=
