#'interface' directory conf.mak
include $(PROJECT_INCDIR)/projfeatures.mak
#USOURCE is what comes from the Unimal preprocessor, if any

#SOURCE, if defined, is a space-separated list of the sources in this directory
SOURCE:=ifman.c
#SUBDIR, if defined,  is a space-separated list of subdirectories with sources
SUBDIR:=hart
ifeq ($(FEATURE_LOCAL_UI),LCD)
SUBDIR+=ui
endif
ifeq ($(FEATURE_LOCAL_UI),SIMPLE)
SUBDIR+=uisimple
endif
ifeq ($(FEATURE_LOCAL_UI),NONE)
SOURCE+=uistubs.c
endif
ifeq ($(PROJ),FFAP)
SUBDIR+=FF
endif

#ISUBDIR, if defined,  is a space-separated list of header subdirectories
#Compiler flags private to this directory are below
