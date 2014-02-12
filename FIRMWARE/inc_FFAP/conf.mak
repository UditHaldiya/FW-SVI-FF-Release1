#inc_FFAP conf.mak

#an include for project-specific feature-independent headers:
ISUBDIR = ../includes/project_$(PROJ)

UFILES_H=../includes/dimensions.u

ifeq ($(FEATURE_LOCAL_UI),LCD)
ISUBDIR+=../interface/ui ../interface/ui/glue ../interface/ui/project_$(PROJ)
UFILES_H+=../includes/uistartnodes.u
endif

ifeq ($(FEATURE_LOGFILES),USED)
UFILES_H+=../includes/logfileids.u
endif


