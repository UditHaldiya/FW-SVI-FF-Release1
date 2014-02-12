#sysio/project_$(PROJ)
SOURCE+=../tcomptab.c adtbl.c

ifeq ($(FEATURE_PRESSURE_INPUTS),INCLUDED)
SOURCE+=../prestab.c
endif

