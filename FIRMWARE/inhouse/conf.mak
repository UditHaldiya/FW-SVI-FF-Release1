#INHOUSE conf.mak

ifneq ($(UTSET),)
SUBDIR=unit_test
else
#SOURCE, if defined, is a space-separated list of the sources in this directory
#SUBDIR, if defined,  is a space-separated list of subdirectories with sources
#ISUBDIR, if defined,  is a space-separated list of header subdirectories
SOURCE:=
SUBDIR:=
CFLAGS_LOCAL:=
endif
