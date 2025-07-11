#ui/glue conf.mak

#SUBDIR, if defined,  is a space-separated list of subdirectories with sources
#ISUBDIR, if defined,  is a space-separated list of header subdirectories

#Compiler flags private to this directory are below
CFLAGS_LOCAL= $(LIB_OPTION)


SOURCE:=ui_engunits.c ui_locks.c ui_faults.c ui_devmodes.c ui_uisetup.c ui_common.c \
	ui_config.c

#Those are known glue headers that don't have separate implementations
#Their .h and .inc version must nonetheless come in the UI table build.

GLUEHEADERS_ONLY:=ui_dispmode.h

ifeq ($(PROJ),DLT)
SOURCE += ui_sigctl.c ui_calib_sg_dlt.c ui_advsetup_dlt.c ui_diagnostics_dlt.c
endif
ifeq ($(FEATURE_PRESSURE_INPUTS),INCLUDED)
SOURCE+=ui_pressures.c
endif
ifeq ($(FEATURE_UI_SHOW_TRIPPED),ENABLED)
SOURCE+=ui_latch.c
endif
ifeq ($(PROJ),FFAP)
SOURCE+=ui_ffinfo.c ui_ffcustomvar.c
endif
ifeq ($(FEATURE_PARTIAL_STROKE_TEST),SUPPORTED)
SOURCE+=ui_pst.c
endif
ifeq ($(FEATURE_POSITION_CONTROL),INCLUDED)
SOURCE+=ui_sp.c ui_findstops.c
endif

GLUEHEADERS_ONLY+=ui_charact.h

#Temporary until further refactoring of MNCB setup menus
ifeq ($(FEATURE_HARDWARE_PLATFORM),MNCB)
GLUEHEADERS_ONLY+= ui_calib.h
endif

ifeq ($(FEATURE_SIGNAL_SETPOINT),SUPPORTED)
SOURCE+=ui_sigsp.c
endif
