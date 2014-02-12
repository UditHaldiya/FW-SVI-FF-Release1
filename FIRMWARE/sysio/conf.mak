#'sysio' conf.mak
#SOURCE, if defined, is a space-separated list of the sources in this directory
SOURCE:= \
    position.c posint.c \
    ipcurr.c \
    adtbldrv.c sysio.c

SOURCE += poscharact.c poschrtab.c doswitchff.c

ifeq ($(FEATURE_PRESSURE_INPUTS),INCLUDED)
SOURCE+=pressures.c
endif

ifeq ($(FEATURE_PVINPUT_SENSOR),AVAILABLE)
SOURCE+=inpv.c
endif

ifeq ($(FEATURE_REMOTE_POSITION_SENSOR),AVAILABLE)
SOURCE+=posext.c
endif

ifeq ($(FEATURE_AO), SUPPORTED)
SOURCE+= ao.c aotab.c
endif


#SUBDIR, if defined,  is a space-separated list of subdirectories with sources
SUBDIR:=
ifeq ($(FEATURE_LOCAL_UI),LCD)
# SUBDIR comes from module (+=lcdui)
endif
ifeq ($(FEATURE_LOCAL_UI),SIMPLE)
SUBDIR+=ledui
endif

#ISUBDIR, if defined,  is a space-separated list of header subdirectories
