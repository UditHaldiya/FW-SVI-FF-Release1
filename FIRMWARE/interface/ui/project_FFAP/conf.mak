#interface/ui/project_FFAP/conf.mak

include $(settings)

#Where UI finds its wiring to the system
ISUBDIR+=../glue

#The tables and generators come from the UI module, except this legacy
SOURCE:=../glue/uifunc.c

