#'nvram/project_FFAP' directory conf.mak

UFILES_C:=

ifeq ($(FEATURE_LOGFILES),USED)
UFILES_C+=../logfdata.u
SOURCE+=../facdefaults.c ../clonenvram.c
endif

