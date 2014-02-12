#tasks
#SOURCE, if defined, is a space-separated list of the sources in this directory
SOURCE=diagnostics.c diagnosticsUniversal.c procshims.c #cycle. cmain.c

ifeq ($(FEATURE_LOCAL_UI),SIMPLE)
#Add UI processes
SOURCE+=proc4uisimple.c
endif

#SUBDIR, if defined,  is a space-separated list of subdirectories with sources
SUBDIR=control
#ISUBDIR, if defined,  is a space-separated list of header subdirectories

#Compiler flags private to this directory are below
CFLAGS_LOCAL= $(LIB_OPTION)


