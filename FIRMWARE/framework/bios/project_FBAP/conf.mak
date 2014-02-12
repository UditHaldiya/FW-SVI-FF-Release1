#framework/bios/project_FBAP
#SOURCE, if defined, is a space-separated list of the sources in this directory
SOURCE:=da.c inituc.c  dohwif.c  pwm.c fram.c i2c.c
SOURCE+=../project_SVI2AP/spi.c ../project_SVI2AP/key.c ../project_SVI2AP/adhwconf.c
#SUBDIR, if defined,  is a space-separated list of subdirectories with sources
SUBDIR:=
#ISUBDIR, if defined,  is a space-separated list of header subdirectories

#Compiler flags private to this directory are below
#CFLAGS_LOCAL= $(LIB_OPTION)