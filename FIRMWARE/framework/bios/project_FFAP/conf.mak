#framework/bios/project_SVI2AP
#SOURCE, if defined, is a space-separated list of the sources in this directory
SOURCE:= gpio_key.c spi.c adhwconf.c fram.c i2c.c

ifeq ($(FEATURE_ACTIVATION),ID_SN)
SOURCE+=sernumchip.c
endif

#SUBDIR, if defined,  is a space-separated list of subdirectories with sources
SUBDIR:=
#ISUBDIR, if defined,  is a space-separated list of header subdirectories

#Compiler flags private to this directory are below
#CFLAGS_LOCAL= $(LIB_OPTION)

