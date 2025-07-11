#HAL
#SOURCE, if defined, is a space-separated list of the sources in this directory
SOURCE=
#SUBDIR, if defined,  is a space-separated list of subdirectories with sources

#No-one else is interested; this should move, too
ifeq ($(PROJ),LCX)
SUBDIR=bios
endif

#ISUBDIR, if defined,  is a space-separated list of header subdirectories
ISUBDIR=


