#'uisimple' directory conf.mak
include $(PROJECT_INCDIR)/projfeatures.mak
#USOURCE is what comes from the Unimal preprocessor, if any

#SOURCE, if defined, is a space-separated list of the sources in this directory
SOURCE:=uimain.c ui_ledpattern.c uijumpers.c uihw_test.c

ifeq ($(FEATURE_UI_OPSTOP_ADJ),PROCESS)
SOURCE+=osadjust.c
endif

#SUBDIR, if defined,  is a space-separated list of subdirectories with sources

#ISUBDIR, if defined,  is a space-separated list of header subdirectories
#Compiler flags private to this directory are below
