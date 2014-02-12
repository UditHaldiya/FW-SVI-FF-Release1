ifeq (yin,yang)
This file contains a collection of modules the project uses.
It may be empty (for a project not using modules) but it must exist.

A module may be listed as foo_x-y.mak,z which references revision z
or as foo_x-y.mak which references the latest revision.
All traceable builds are required to specifiy explicit revisions
of all modules.

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/inc_FFAP/modules_FFAP.mak $
    $Date: 1/30/12 3:49p $
    $Revision: 47 $
    $Author: Arkkhasin $
    $History: modules_FFAP.mak $
 * 
 * *****************  Version 47  *****************
 * User: Arkkhasin    Date: 1/30/12    Time: 3:49p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_FFAP
 * TFS:8782
 * 
 * *****************  Version 46  *****************
 * User: Arkkhasin    Date: 1/30/12    Time: 2:08p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_FFAP
 * Removed signal setpoint and loop signal data FBO TFS:8782
 * TFS:7749 Restore NVMEMDUMP build
 * 
 * *****************  Version 44  *****************
 * User: Anatoly Podpaly Date: 1/17/12    Time: 5:24p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_SVI2AP
 * TFS:8608 - I2C driver moved from module to Project - specific section.
 *
 * *****************  Version 43  *****************
 * User: Arkkhasin    Date: 1/15/12    Time: 2:50p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_SVI2AP
 * TFS:8680 -write air action and pid slot
 *
 * *****************  Version 42  *****************
 * User: Anatoly Podpaly Date: 1/13/12    Time: 3:51p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_SVI2AP
 * Corrected module revs.
 *
 * *****************  Version 41  *****************
 * User: Arkkhasin    Date: 1/13/12    Time: 3:17p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_SVI2AP
 * TFS:8680 HART interface for UI
 *
 * *****************  Version 40  *****************
 * User: Arkkhasin    Date: 1/09/12    Time: 6:36p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_SVI2AP
 * projfeatures forgotten (same TFS:8577)
 *
 * *****************  Version 39  *****************
 * User: Arkkhasin    Date: 1/09/12    Time: 3:11p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_SVI2AP
 * Modules update FBO TFS:8577
 *
 * *****************  Version 38  *****************
 * User: Arkkhasin    Date: 1/05/12    Time: 3:35p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_SVI2AP
 * TFS:8577 - Modules update
 *
 * *****************  Version 37  *****************
 * User: Arkkhasin    Date: 1/05/12    Time: 3:26p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_SVI2AP
 * TFS:8577 - Modules update
 *
 * *****************  Version 36  *****************
 * User: Arkkhasin    Date: 12/15/11   Time: 5:20p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_SVI2AP
 * Conditional local UI + missing sources (forgotten when dropping in
 * engunits)
 *
 * *****************  Version 35  *****************
 * User: Arkkhasin    Date: 12/07/11   Time: 12:24p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_SVI2AP
 * TFS:8204 - features for I/O channels
 *
 * *****************  Version 34  *****************
 * User: Arkkhasin    Date: 12/05/11   Time: 4:20p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_SVI2AP
 * TFS:8347 - Restore AP build
 *
 * *****************  Version 32  *****************
 * User: Arkkhasin    Date: 12/06/10   Time: 11:12p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_SVI2AP
 * Interim check-in FOB TFS:4922
 *
 endif

#MUST go first
SUBMODULES:=projconf_1-1.mak,T

#Local UI
SUBMODULES+=uicommonapi_1-0.mak,T
SUBMODULES+=uiengine_1-0.mak,T uiif_1-0.mak,T
#LCD driver
SUBMODULES+=lcd_1-0.mak,T
#ASCII utility
SUBMODULES+=asciiutils_1-0.mak,T

#makebasicprimer_1-0.mak,2

#Build environment
SUBMODULES+=lint_1-0.mak,T \
    buildutils_1-0.mak,T \
    ideutils_1-0.mak,T \
    makefiles_1-0.mak,T


SUBMODULES+= MNEnvir_1-2.mak,T \
    process_2-0.mak,T \
    faultman_1-0.mak,T \
    mnassert_1-0.mak,T \
    bufutils_1-0.mak,T \
    nvram_3-1.mak,T \
    resource_1-0.mak,T \
    errcodes_1-0.mak,T

#General helpers
SUBMODULES+=numutils_1-0.mak,T \
    bitutils_1-0.mak,T

#BIOS
SUBMODULES+=wprotect_1-0.mak,T \
    pbypass_1-0.mak,T \
    hartfsk_2-0.mak,T


#HART Glue layer
SUBMODULES+= corefac_1-0.mak,T \
    hart_base_1-0.mak,T \
    faultsif_1-0.mak,T \
    smoothingif_0-0.mak,T \
    doswif_0-0.mak,T \
    pbypassif_1-0.mak,T \
    hwswitchesif_1-0.mak,T \
    pressuresif_1-0.mak,T

#Since DI is present
SUBMODULES+=diswif_1-0.mak,T


SUBMODULES+=hcframework_1-0.mak,T
SUBMODULES+=faultsimport_1-0.mak,T

#NVMEM
SUBMODULES+=nvmemdump_1-0.mak,T

#RTOS
SUBMODULES+=uC_OS_II_2_74_1-0.mak,T \
    taskhelpers_1-0.mak,T \
	osport_1-0.mak,T

#I/O Subsystem
SUBMODULES+=temprmncb_1-0.mak,T \
    tempr_1-0.mak,T \
    smoothing_1-0.mak,T \
    signaldef_1-0.mak,T \
    lcduiio_1-0.mak,T

#Tempcomp engine
SUBMODULES+=tempcomp_2-0.mak,T

#Floating point
SUBMODULES+=engunits_1-0.mak,T


#Always the latest profiler module
ifeq ($(MN_INSTRUM),profiler)
SUBMODULES+= instrum_profiling_2-0.mak,T
endif




