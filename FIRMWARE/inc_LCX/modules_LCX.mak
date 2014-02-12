ifeq (yin,yang)
This file contains a collection of modules the project uses.
It may be empty (for a project not using modules) but it must exist.

A module may be listed as foo_x-y.mak,z which references revision z
or as foo_x-y.mak which references the latest revision.
All traceable builds are required to specifiy explicit revisions
of all modules.

    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/inc_LCX/modules_LCX.mak $
    $Date: 1/30/12 3:49p $
    $Revision: 59 $
    $Author: Arkkhasin $
    $History: modules_LCX.mak $
 *
 * *****************  Version 59  *****************
 * User: Arkkhasin    Date: 1/30/12    Time: 3:49p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_LCX
 * TFS:8782
 *
 * *****************  Version 58  *****************
 * User: Arkkhasin    Date: 1/30/12    Time: 2:10p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_LCX
 * TFS:7749 Restore NVMEMDUMP build
 *
 * *****************  Version 57  *****************
 * User: Anatoly Podpaly Date: 1/17/12    Time: 5:25p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_LCX
 * TFS:8608 - I2C driver moved from module to Project - specific section.
 *
 * *****************  Version 56  *****************
 * User: Arkkhasin    Date: 1/15/12    Time: 8:45p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_LCX
 * Update of LCX modules FBO TFS:8680
 *
 * *****************  Version 55  *****************
 * User: Anatoly Podpaly Date: 1/13/12    Time: 3:51p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_LCX
 * COrrected Module inclusion
 *
 * *****************  Version 54  *****************
 * User: Arkkhasin    Date: 1/10/12    Time: 12:19p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_LCX
 * (Partially) restored LCX build broken by work on AP build (TFS:8577).
 * No LCX code change except fault table where flash test timeout is still
 * inactive
 *
 * *****************  Version 53  *****************
 * User: Arkkhasin    Date: 1/05/12    Time: 3:36p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_LCX
 * TFS:8577 - Modules update
 *
 * *****************  Version 52  *****************
 * User: Arkkhasin    Date: 12/09/11   Time: 7:07p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_LCX
 * TFS:8370 (Oops)
 *
 * *****************  Version 51  *****************
 * User: Arkkhasin    Date: 12/09/11   Time: 3:42p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_LCX
 * TFS:8370 Module drop-in -- engunits_1-0
 *
 * *****************  Version 50  *****************
 * User: Arkkhasin    Date: 12/07/11   Time: 12:24p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_LCX
 * TFS:8204 - features for I/O channels
 *
 * *****************  Version 49  *****************
 * User: Justin Shriver Date: 11/30/11   Time: 2:47p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_LCX
 * -- AP -- TFS:7889; 7899 -- Use the latest revision of pbypass module.
 *
 * *****************  Version 48  *****************
 * User: Arkkhasin    Date: 11/29/11   Time: 3:49p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_LCX
 * Latest modules FBO TFS:8313
 *
 * *****************  Version 47  *****************
 * User: Arkkhasin    Date: 11/18/11   Time: 1:52p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_LCX
 * TFS:8248, 8249
 *
 * *****************  Version 46  *****************
 * User: Arkkhasin    Date: 11/15/11   Time: 8:13p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_LCX
 * bug workaround (renamed module) See TFS:8210
 *
 * *****************  Version 45  *****************
 * User: Arkkhasin    Date: 11/15/11   Time: 6:46p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_LCX
 * Preliminary check-in for
 * TFS:8051 new tempcomp
 * TFS:8202 decouple I/O subsystem
 *
 * *****************  Version 44  *****************
 * User: Arkkhasin    Date: 11/07/11   Time: 4:29p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_LCX
 * Some modules latest (for now)
 *
 * *****************  Version 43  *****************
 * User: Arkkhasin    Date: 11/07/11   Time: 11:57a
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/inc_LCX
 * TFS:8072 NVMEM upgrade
 * TFS:8140 OS PORT upgrade
 *
 * *****************  Version 41  *****************
 * User: Arkkhasin    Date: 6/07/11    Time: 11:05a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * TFS:6561,4285
 *
 * *****************  Version 40  *****************
 * User: Arkkhasin    Date: 5/06/11    Time: 1:12a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * hartfsk_1-0.mak,2 FBO TFS:6124
 *
 * *****************  Version 39  *****************
 * User: Arkkhasin    Date: 4/19/11    Time: 6:41p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * TFS:6096
 *
 * *****************  Version 38  *****************
 * User: Arkkhasin    Date: 3/01/11    Time: 12:14p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * TFS:5748: SmoothCoefTempr added range check model
 *
 * *****************  Version 37  *****************
 * User: Arkkhasin    Date: 2/25/11    Time: 3:34p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * Profiler module, if used, is always the latest (FBO TFS:5438)
 *
 * *****************  Version 36  *****************
 * User: Arkkhasin    Date: 1/27/11    Time: 11:58p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/inc_LCX
 * TFS:5505 Step 4 - Enable profiling module
 *
endif


#MUST go first
SUBMODULES:=projconf_1-1.mak,22842

#Local UI
SUBMODULES+=uicommonapi_1-0.mak,T

#makebasicprimer_1-0.mak,22777

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
    pbypass_1-0.mak,22858 \
    hartfsk_2-0.mak,T


#HART Glue layer
SUBMODULES+= corefac_1-0.mak,T \
    hart_base_1-0.mak,T \
    faultsif_1-0.mak,T \
    smoothingif_0-0.mak,22777 \
    doswif_0-0.mak,T \
    pbypassif_1-0.mak,T \
    hwswitchesif_1-0.mak,22777 \
    pressuresif_1-0.mak,T \
    trimsigif_1-0.mak,T

SUBMODULES+=hcframework_1-0.mak,T
SUBMODULES+=faultsimport_1-0.mak,22777

#NVMEM
SUBMODULES+=nvmemdump_1-0.mak,T

#RTOS
SUBMODULES+=uC_OS_II_2_74_1-0.mak,T \
    taskhelpers_1-0.mak,T \
	osport_1-0.mak,T

#I/O Subsystem
SUBMODULES+=temprmncb_1-0.mak,T \
    tempr_1-0.mak,T \
    insignal_1-0.mak,T \
    smoothing_1-0.mak,T \
    signaldef_1-0.mak,T

#Tempcomp engine
SUBMODULES+=tempcomp_2-0.mak,T

#Floating point
SUBMODULES+=engunits_1-0.mak,T

#Always the latest profiler module
ifeq ($(MN_INSTRUM),profiler)
SUBMODULES+= instrum_profiling_2-0.mak,T
endif


#Instrumentation extras

