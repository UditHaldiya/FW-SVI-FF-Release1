#'HART\glue' directory conf.mak
#USOURCE is what comes from the Unimal preprocessor, if any
SOURCE:=hart_posctl.c hart_ctllimits.c hart_pneuparams.c hart_charactsp.c \
    hart_readato.c hart_ext_analysis.c \
    hart_dosw.c hart_stops_adjust.c \
    hart_testassert.c hart_write_ctlout.c \
    hart_engunitsif.c \
        hart_errlimits.c

ifeq ($(FEATURE_DIGITAL_SETPOINT),SUPPORTED)
SOURCE+=hart_digsp.c
endif

ifeq ($(FEATURE_SIGNAL_SETPOINT),SUPPORTED)
SOURCE+=hart_sigsp.c
endif

#Candidates for packaging in a module
ifeq ($(FEATURE_LOCAL_UI),SIMPLE)
SOURCE+=hart_hwuisimple.c hart_stops_adjust_conf.c
endif

ifneq ($(FEATURE_LOCAL_UI),SIMPLE)
SOURCE+=hart_writeato.c hart_writectlselector.c
endif

ifeq ($(FEATURE_ACTIVATION),ID_SN)
SOURCE+=hart_activate.c
endif

ifeq ($(FEATURE_AO), SUPPORTED)
SOURCE+= hart_ao.c
endif

ifeq ($(FEATURE_BUMPLESS_XFER), ENABLED)
SOURCE+= hart_bumpless.c
endif

#Candidates for packaging in a module
ifeq ($(PROJ),LCX)
SOURCE+=hart_digipot.c hart_sernumchip.c
endif

#Kludgy conditional!
ifneq ($(PROJ),LCX)
SOURCE+=hart_vdiag.c
endif

ifeq ($(PROJ),FFAP)
SOURCE+=hart_facdefaults.c hart_nvmclone.c
endif

ifeq ($(PROJ),FFAP)
SOURCE+=hart_dbuftest.c
endif

# Glue layer for Old API; candidates for refactoring
SOURCE+=oldapicollection.c \
   hart_smoothtempr.c \
   hart_smoothpos.c

ifeq ($(FEATURE_LOOPSIGNAL_SENSOR), SUPPORTED)
SOURCE+=hart_smoothsig.c \
    hart_insignal.c
endif

ifeq ($(FEATURE_REMOTE_POSITION_SENSOR), AVAILABLE)
SOURCE+=hart_sensortype.c
endif

ifeq ($(FEATURE_LOGFILES),USED)
SOURCE+=hart_logfile.c hart_diagrw.c
endif

SOURCE+=hart_setpoint.c hart_lowpwrdata.c hart_refvoltage.c

#SUBDIR, if defined,  is a space-separated list of subdirectories with sources
#ISUBDIR, if defined,  is a space-separated list of header subdirectories
#Compiler flags private to this directory are below
CFLAGS_LOCAL=
