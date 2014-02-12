#framework/bios/project_LCX
#SOURCE, if defined, is a space-separated list of the sources in this directory
SOURCE:=inituc.c key.c sernumchip.c digital_pot.c
#SUBDIR, if defined,  is a space-separated list of subdirectories with sources
SUBDIR:=
#ISUBDIR, if defined,  is a space-separated list of header subdirectories
SOURCE+=dohwif.c spi.c adhwconf.c fram.c i2c.c
#ASOURCE, if defined, is a space-separated list of assembler files
ASOURCE=snchip_a.s79
#Compiler flags private to this directory are below
#CFLAGS_LOCAL= $(LIB_OPTION)