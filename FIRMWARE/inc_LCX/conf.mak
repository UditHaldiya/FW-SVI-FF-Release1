#inc_LCX conf.mak

#an include for project-specific feature-independent headers:
ISUBDIR = ../includes/project_$(PROJ)

UFILES_H=../includes/dimensions.u

ifeq ($(FEATURE_LOGFILES),USED)
UFILES_H+=../includes/logfileids.u
endif

