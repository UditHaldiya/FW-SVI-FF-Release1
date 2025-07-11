#'utilities' directory conf.mak
#SOURCE, if defined, is a space-separated list of the sources in this directory
SOURCE=configure.c \
    valveutils.c \
    cryptutils.c \
    cnfghook.c

ASOURCE:=
#SUBDIR, if defined,  is a space-separated list of subdirectories with sources
SUBDIR=
#ISUBDIR, if defined,  is a space-separated list of header subdirectories
#Compiler flags private to this directory are below
CFLAGS_LOCAL=
