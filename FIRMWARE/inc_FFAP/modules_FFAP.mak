ifeq (yin,yang)

This file contains a collection of modules the project uses.
It may be empty (for a project not using modules) but it must exist.

A module may be listed as foo_x-y.mak,z which references revision z
or as foo_x-y.mak which references the latest revision.
All traceable builds are required to specifiy explicit revisions
of all modules.

endif

#MUST go first
SUBMODULES:=projconf_1-2.mak,28702

#Local UI
SUBMODULES+=uicommonapi_1-0.mak,27138
SUBMODULES+=uiengine_1-0.mak,27138 uiif_1-0.mak,31913
#LCD driver
SUBMODULES+=lcd_1-0.mak,27138
#ASCII utility
SUBMODULES+=asciiutils_1-0.mak,27138

#makebasicprimer_1-0.mak,2

#Build environment
SUBMODULES+=lint_2-0.mak,29030 \
    buildutils_1-0.mak,27138 \
    ideutils_1-0.mak,27138 \
    makefiles_2-0.mak,31286


SUBMODULES+= MNEnvir_2-0.mak,31754 \
    process_2-1.mak,33450 \
    faultman_1-0.mak,30541 \
    mnassert_1-0.mak,27138 \
    bufutils_1-0.mak,27138 \
    nvram_3-2.mak,32232 \
    resource_1-0.mak,27138 \
    errcodes_1-0.mak,27138

#General helpers
SUBMODULES+=numutils_1-0.mak,27138 \
    bitutils_1-0.mak,27138

#BIOS
SUBMODULES+=wprotect_1-0.mak,27138 \
    pbypass_1-0.mak,27138


#HART Glue layer
SUBMODULES+= corefac_1-0.mak,28702 \
    hart_base_1-0.mak,27138 \
    faultsif_1-0.mak,27138 \
    smoothingif_0-0.mak,27138 \
    pbypassif_1-0.mak,27138 \
    hwswitchesif_1-0.mak,29467 \
    pressuresif_1-0.mak,27138

#Since DI is present
SUBMODULES+=diswif_1-0.mak,27138


SUBMODULES+=hcframework_1-0.mak,31286
SUBMODULES+=faultsimport_1-0.mak,27138

#NVMEM
SUBMODULES+=nvmemdump_1-0.mak,27138

#RTOS
SUBMODULES+=uC_OS_II_2_74_1-0.mak,27138 \
    taskhelpers_1-0.mak,27138 \
	osport_1-0.mak,29955

#I/O Subsystem
SUBMODULES+=temprmncb_1-0.mak,27138 \
    tempr_1-0.mak,27138 \
    smoothing_1-0.mak,27138 \
    signaldef_1-0.mak,27138 \
    lcduiio_2-0.mak,30922

#Tempcomp engine
SUBMODULES+=tempcomp_2-0.mak,30987

#Floating point
SUBMODULES+=engunits_1-0.mak,28562


#Always the latest profiler module
ifeq ($(MN_INSTRUM),profiler)
SUBMODULES+= instrum_profiling_2-0.mak,T
endif




