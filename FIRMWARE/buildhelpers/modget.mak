ifeq (yin,yang)
    $Archive: /MNCB/Dev/LCX2AP/FIRMWARE/buildhelpers/modget.mak $
    $Date: 11/07/11 3:50p $
    $Revision: 19 $
    $Author: Arkkhasin $
    $History: modget.mak $
 *
 * *****************  Version 19  *****************
 * User: Arkkhasin    Date: 11/07/11   Time: 3:50p
 * Updated in $/MNCB/Dev/LCX2AP/FIRMWARE/buildhelpers
 * TFS:8154 echo trouble
 *
 * *****************  Version 17  *****************
 * User: Arkkhasin    Date: 9/29/10    Time: 11:23a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/buildhelpers
 * Fixed a mkdir command
 *
 * *****************  Version 16  *****************
 * User: Arkkhasin    Date: 9/28/10    Time: 12:47a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/buildhelpers
 * TFS4203 [Part 2] Module's helper makefiles integrated correctly
 *
 * *****************  Version 15  *****************
 * User: Arkkhasin    Date: 9/27/10    Time: 10:10p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/buildhelpers
 * TFS4203 [Part 1] - Project-specific Unimal sources resolved correctly
 *
 *
 * *****************  Version 14  *****************
 * User: Arkkhasin    Date: 9/17/10    Time: 12:45p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/buildhelpers
 * Syntax fix as in the trunk
 *
 * *****************  Version 13  *****************
 * User: Arkkhasin    Date: 9/16/10    Time: 7:21p
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/buildhelpers
 * Interim update for TFS:4067 (XML can be in modules)
 *
 * *****************  Version 6  *****************
 * User: Arkkhasin    Date: 9/15/10    Time: 10:39p
 * Updated in $/MNCB/Dev/FIRMWARE/buildhelpers
 * Import xml definitions from modules (HCMD keyword)
 *
 * *****************  Version 5  *****************
 * User: Arkkhasin    Date: 9/15/10    Time: 2:46p
 * Updated in $/MNCB/Dev/FIRMWARE/buildhelpers
 * Brought forward LCX's ver. 12.
 * Bug TFS:4038 still outstanding
 *
 * *****************  Version 12  *****************
 * User: Arkkhasin    Date: 9/02/10    Time: 11:14a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/buildhelpers
 * Temporary check-in to enable OFFICIAL and devel builds for th etime
 * being
 *
 * *****************  Version 11  *****************
 * User: Arkkhasin    Date: 9/02/10    Time: 1:22a
 * Updated in $/MNCB/Dev/AP_Release_3.1.x/LCX_devel/FIRMWARE/buildhelpers
 * An attempt to fix bug 4038
 *
 * *****************  Version 4  *****************
 * User: Arkkhasin    Date: 6/11/10    Time: 9:10a
 * Updated in $/MNCB/Dev/FIRMWARE/buildhelpers
 * Ported LCX revision 9: supports explicit revisions of modules
endif

.PHONY : modsetup all

comma:=,
empty:=
space:= $(empty) $(empty)
DOLLAR:=$$

OFFuser :="Release Builder"
OFFpass:=rbuilder
#Old VCS_ENV:=SSDIR=p:\sourcesafe\HART
#New - note that \ and $ are escaped twice - because they are passed twice
#VCS_ENV:=SSDIR=\\\\vault.corp.dresser.com\\swdev-sourcesafe$$$$\\HART
#VCS_ENV:=SSDIR=C:\SourceSafe\HART
OFFroot := .
OFFVCS:="$(ProgramFiles)\Microsoft Visual Studio 10.0\Common7\IDE\tf.exe"#Version control system command line; must be in the path
VCS_MODULES_ROOT:=$(DOLLAR)/Core/FIRMWARE

#mycopy:=xcopy /R/K/Y
mycopy:=.\cp.exe -f -p

ifneq ($(module),)
include $(firstword $(subst $(comma),$(space),$(module)))
$(info module=[$(module)] REQUIRE=[$(REQUIRE)])
$(info SUB=$(SUB))
$(info SRC=$(SRC))
$(info MCPY=$(MCPY))
$(info ++++++++++++++++++++ included $(firstword $(subst $(comma),$(space),$(module))) )
endif

FILEMEMBERS=$(SRC) $(HDR) $(ASRC) $(USRC) $(UHDR) $(HCMD)
MEMBERS=$(FILEMEMBERS) $(SUB) $(MCPY) $(MKINC)

#LOGIN:=/login:CORP\TFSBUILD,Dresser123
LOGIN:=

mchangeset?=T
mitem=$(subst ;T,;$(mchangeset), $(subst $(comma),;,$@))
fitem=$(subst ;T,;$(mchangeset), $(subst $(comma),;,$^))

modules_path:=$(lastword $(shell $(OFFVCS) workfold $(VCS_MODULES_ROOT) $(LOGIN)))

all: $(module) $(SUBMODULES) $(MEMBERS) filelist.txt filecpy.butt modget
    $(MN_ECHO) $(addprefix modules\,$(SUBMODULES)) >> $(submodfile)

.PHONY :$(MEMBERS) filelist.txt filecpy.butt

filelist.txt : $(FILEMEMBERS)
	echo $(addprefix $(VCS_MODULES_ROOT)/,$(fitem)) \>>$@

filecpy.butt : $(FILEMEMBERS)
	
#Maybe, we don't want it. Or we can add a marker as before
$(module) : inc_$(PROJ)\modules_$(PROJ).mak


# Set up environment and root module
modsetup :
	-mkdir modules
    $(MN_ECHO) #SUBMODULES > $(submodfile)
    $(mycopy) inc_$(PROJ)\modules_$(PROJ).mak modules\modules_$(PROJ).mak
	echo $(modules_path)
    $(pawz)
    $(pause)

modcomplete :
    $(pawz)

modget : $(FILEMEMBERS)
ifneq ($(strip $(FILEMEMBERS)),)
	echo. $(MODARG)  $(LOGIN)>>filelist.txt
	$(OFFVCS) @filelist.txt
    $(pawz)
	.\cp.exe filecpy.butt filecpy.bat
	filecpy.bat
endif


#Create read marker in the last operation
define modgetMod
    $(OFFVCS) get $(VCS_MODULES_ROOT)/modules_store/$(notdir $(mitem)) $(MODARG)  $(LOGIN)
	echo get $(SLASH)>filelist.txt
	$(MN_ECHO) Rem Files to copy > filecpy.butt
	$(mycopy) $(modules_path)\modules_store\$(firstword $(subst $(comma),$(space),$(notdir $@))) modules\$(firstword $(subst $(comma),$(space),$(notdir $@)))
    @echo got module $@
endef

modmember=$(subst /,\,$(firstword $(subst $(comma),$(space),$@)))

define modget
	echo $(mycopy) $(modules_path)\$(modmember) $(modmember) >> filecpy.butt
    $(pause)
endef

modconf=$(dir $@)modconf_$(PROJ).mak.candidate
modcpy=$(subst ->,$(space),$@)
$(MCPY) : modconf=$(lastword $(modcpy)).candidate
$(MCPY) : SourceVar=$(firstword $(modcpy))

$(MKINC) : modconf=$(lastword $(modcpy)).candidate
$(MKINC) : SourceVar=include $(firstword $(modcpy))

$(SRC): SourceVar=SOURCE
$(ASRC): SourceVar=ASOURCE
$(USRC): SourceVar=UFILES_C
$(UHDR): SourceVar=UFILES_H
$(SUB): SourceVar=SUBDIR

$(info SourceVar=[$(SourceVar)])

define write_modconf
    $(MN_ECHO) $(SourceVar)+=$(firstword $(subst $(comma),$(space),$(notdir $@))) >> $(modconf)
endef

define write_var
    $(MN_ECHO) $(SourceVar) >> $(modconf)
endef

$(SUBMODULES) : filelist.txt filecpy.butt $(module) force
    @echo Active when ifneq ($(module),modules_$(PROJ).mak)
    $(MN_ECHO) $(REQUIRE) >> $(modreqfile)
    $(paws)
    $(modgetMod)
    $(Hide)echo Read marker >"$@"
	echo SUB=[$(SUB)]
    $(MN_ECHO) $(REQUIRE) >> $(modreqfile)
    $(MAKE) -f $(ModSync) module=modules\$@

$(SRC) $(ASRC) $(USRC) $(UHDR) : $(SUBMODULES)
    @echo getting=$@ (module=$(module))
    $(pause)
    $(modget)
    $(write_modconf)

$(HDR) : $(SUBMODULES)
    $(modget)

#directory $(SUB) may exist hence force
$(SUB) : $(SUBMODULES) force
    $(write_modconf)
    if not exist $@ cmd.exe /E /C mkdir $(subst /,\,$@)
	$(pawz)

$(MCPY) $(MKINC) : $(SUBMODULES)
    echo SourceVar=[$(SourceVar)]
    echo modconf=[$(modconf)]
    $(write_var)
    $(pause)


#XML-defined HART commands in the modules
$(HCMD) : $(SUBMODULES)
    $(modget)
    $(MN_ECHO) $(firstword $(subst $(comma),$(space),$@)) >> $(hcmdlist).candidate


# ---------------------------------------------------
force : ;
