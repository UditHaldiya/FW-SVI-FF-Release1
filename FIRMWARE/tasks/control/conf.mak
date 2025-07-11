#tasks/control
#SOURCE, if defined, is a space-separated list of the sources in this directory
SOURCE:=atune.c control.c ctllimits.c pneumatics.c errlimits.c fullthrottle.c

#This is temporary; soon various projects will get their pidselect_$(PROJ).c or such
SOURCE+=pidselect.c

#If we use signal setpoint
ifeq ($(FEATURE_SIGNAL_SETPOINT),SUPPORTED)
SOURCE+=signalsp.c
endif

ifeq ($(FEATURE_AO_RETRANSMIT),AVAILABLE)
SOURCE+=posretran.c
endif

#SUBDIR, if defined,  is a space-separated list of subdirectories with sources
SUBDIR=
#ISUBDIR, if defined,  is a space-separated list of header subdirectories
#Compiler flags private to this directory are below
CFLAGS_LOCAL=

