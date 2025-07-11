#'hart' directory conf.mak
UFILES_C:=
#SOURCE, if defined, is a space-separated list of the sources in this directory
SOURCE:=hart.c hartconf.c mnhartcmd.c

SOURCE+=haddressing.c hartutils.c mnhartcmdx.c

ifeq ($(PROJ),FFAP)
SOURCE+=stm_ispcomm.c
endif

#For now only FFAP. We'll decide what to do with (abandoned?) hartfsk_2-0.mak later
ifeq ($(PROJ),FFAP)
SOURCE+=serial.c
endif

#SUBDIR, if defined,  is a space-separated list of subdirectories with sources
SUBDIR:=glue
#ISUBDIR, if defined,  is a space-separated list of header subdirectories
#Compiler flags private to this directory are below
CFLAGS_LOCAL=
