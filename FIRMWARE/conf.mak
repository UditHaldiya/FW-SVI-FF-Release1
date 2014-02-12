#top level conf.mak
#SOURCE, if defined, is a space-separated list of the sources in this directory
SOURCE:=

#SUBDIR, if defined,  is a space-separated list of subdirectories with sources
SUBDIR:=includes inc_$(PROJ) sysio HAL tasks UTILITIES interface nvram \
    services framework \
    diagnostics

#ISUBDIR, if defined,  is a space-separated list of header subdirectories
ISUBDIR:=includes inc_$(PROJ) HAL\bsp hal\bios\inc
# RED_FLAG: Is this supposed to be here? HAL\{port,bsp}

#Supported instrumentation
ifneq (,$(wildcard mn_instrum/$(MN_INSTRUM)/conf.mak))
SUBDIR+=mn_instrum
endif

ifneq ($(UTSET),)
SUBDIR+=inhouse
ISUBDIR+=mn_instrum\maestra
endif