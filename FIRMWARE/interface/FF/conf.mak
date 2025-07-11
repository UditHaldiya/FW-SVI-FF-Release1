#'hart' directory conf.mak
UFILES_C:=
#SOURCE, if defined, is a space-separated list of the sources in this directory
ifeq ($(PROJ),FFAP)
SOURCE:=resblockvars.c aoblockvars.c pidblockvars.c doblockvars.c tbblockvars.c aiblockvars.c ai2blockvars.c ai3blockvars.c do2blockvars.c isblockvars.c pid2blockvars.c devicevars.c positionConfVars.c
endif

#SUBDIR, if defined,  is a space-separated list of subdirectories with sources
SUBDIR:=
#ISUBDIR, if defined,  is a space-separated list of header subdirectories
#Compiler flags private to this directory are below
CFLAGS_LOCAL:=
