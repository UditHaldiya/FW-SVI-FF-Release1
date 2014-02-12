#sysio/project_$(PROJ)
SOURCE+=../tcomptab.c ../project_FFAP/adtbl.c

ifeq ($(FEATURE_PRESSURE_INPUTS),INCLUDED)
SOURCE+=../prestab.c
endif

