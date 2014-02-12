ifeq (yin,yang)
    $Archive: /MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/buildhelpers/sharebox.mak $
    $Date: 1/19/11 6:19p $
    $Revision: 2 $
    $Author: Arkkhasin $
    $History: sharebox.mak $
 * 
 * *****************  Version 2  *****************
 * User: Arkkhasin    Date: 1/19/11    Time: 6:19p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/buildhelpers
 * Made sharebox compatible with local build environment
 *
 * *****************  Version 1  *****************
 * User: Arkkhasin    Date: 9/29/10    Time: 2:54p
 * Created in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/buildhelpers
 * First shot at shared code delivery
endif

ShareBox:=C:\Projects\mncb\Dev\ShareBox

#include conf.mak
include $(LOCAL_CONF)

.PHONY=$(ISUBDIR) mkfiles

all: sharemodules corefiles
    echo $(ISUBDIR)

SYNC : $(ShareBox) force
    $(MAKE) $@ -f $(miscexec) SSDIR=$(VCS_ENV) OFFroot=$(ShareBox) VCS_PROJ_ROOT=\mncb\Dev\ShareBox

$(ISUBDIR) includes\project_$(PROJ) : SYNC
    -CMD.EXE /E /C mkdir $(ShareBox)\$@
    cp -fpv $@\\*.h $(ShareBox)\$@
    cp -fpv $@\\*.inc $(ShareBox)\$@
#DO NOT copy .u and .mak files indiscriminately
    $(pause)

corefiles : $(ISUBDIR) includes\project_$(PROJ)
#Copy core makefiles and tools to prime the process
    cp -fpv makefile $(ShareBox)
    -CMD.EXE /E /C mkdir $(ShareBox)\buildhelpers
    cp -fpv $(addprefix buildhelpers\, modget.mak miscexec.mak scripts.mak) $(ShareBox)\buildhelpers
    -CMD.EXE /E /C mkdir $(ShareBox)\tools
    cp -fpv tools\gnumake.exe $(ShareBox)\tools
    cp -fpv tools\rm.exe $(ShareBox)\tools
    cp -fpv tools\echo.exe $(ShareBox)\tools
#Copy all HART xml from known locations (TODO: find them automatically from the depenency file)
    -CMD.EXE /E /C mkdir $(ShareBox)\interface\hart\commonxml
    cp -fpv interface\hart\commonxml/*.xml $(ShareBox)\interface\hart\commonxml
    cp -fpv inc_$(PROJ)/*.xml $(ShareBox)\inc_$(PROJ)
#Copy the fault matrix
    cp -fpv $(PROJ)mncb_FAULT_matrix.xls $(ShareBox)
#Copy the project library
    cp -fpv cmdbuild\$(DIR)\project_$(PROJ)\$(PROJ).a $(ShareBox)
#Copy the project settings
    cp -fpv includes\project_$(PROJ)\settings.mak $(ShareBox)\includes\project_$(PROJ)
    cp -fpv includes\project_$(PROJ)\projfeatures.mak $(ShareBox)\includes\project_$(PROJ)
    rm -frv  $(ShareBox)\mn_instrum
    cp -fpvdr mn_instrum $(ShareBox)
#Make room for Lint configuration
    -CMD.EXE /E /C mkdir $(ShareBox)\LintCfg
    cp -fpv LintCfg\$(PROJ)_Lint_Exceptions.lnt $(ShareBox)\LintCfg
#Make room for IDE generators
    -CMD.EXE /E /C mkdir $(ShareBox)\idebits5


#This may overwrite some previously copied files
sharemodules : corefiles
    $(MAKE) -f $(ShareBox)\makefile -C $(ShareBox) MODULES
    echo MAINLIB=$(PROJ).a >> $(ShareBox)\modconf_$(PROJ).mak.candidate
    echo ISUBDIR+=mn_instrum/$(MN_INSTRUM) >> $(ShareBox)\modconf_$(PROJ).mak.candidate


$(ShareBox) : force
    -CMD.EXE /E /C mkdir $@

force : ;
