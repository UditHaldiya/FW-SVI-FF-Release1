#Do not change unless instructed
manufacturer_ID:=004745
export type_ID:=0008
DEVICE_REV:=00
DD_REV:=02
MIN_DD_REV:=01
RES_DEV_MINOR_REV:=0
RES_BUILD:=0
RES_BUILD_REV:=17

#debug
Hide:=@

#defaults
rombank?=0
FFOUTBASENAME=fffp_
BFD_EXECUTABLE=$(FFOUTBASENAME)$(rombank).out
#export rombank=0
#export BFD_EXECUTABLE
export FFOUTBASENAME

ffroot:=$(dir $(CURDIR))FD-SW
export ffroot
EMBOS:= $(ffroot)/_embOS_CortexM_IAR/Start
export EMBOS
IARver:=6.30
export CC_CORTEX_M3=$(ProgramFiles)/IAR Systems/Embedded Workbench $(IARver)/arm
MN_STD_INC:=includes inc_FFAP mn_instrum/noinstrum hal/bios/inc
#MNINCLLUDE:=$(addprefix $(CURDIR),$(MN_STD_INC))
MNINCLLUDE:=$(addprefix ../../../../FIRMWARE/,$(MN_STD_INC))
export MNINCLLUDE

makecmd:=$(MAKE) -f $(CURDIR)/handover.mak -C $(ffroot)/target/mak BFD_EXECUTABLE=$(BFD_EXECUTABLE) rombank=$(rombank) \
    DEVICE_TYPE=$(type_ID) DEVICE_REV=$(DEVICE_REV) DD_REV=$(DD_REV)

include

all:
    echo $(EMBOS)

DIR?=Dbg
OFFICIAL MOCKOFF: OFFDir?=Rel
OFFICIAL MOCKOFF: DIR=$(OFFDir)
OFFICIAL MOCKOFF SEND: export NO_MNS:=1
fflint?=fflint
ffthreads?=ffthreads

ifneq (,$(filter $(DIR), Rel REL rel))
#We have decided to use D(ebug - our code) D(ebug - Softing source code) R(elease - Softing libraries).
#The latter R is controlled by library configuration (in Softing makefiles)
#The former D,D are controlled here.
#TargetSuffix:=release
#TargetDir:=FF_SVI_REL
TargetSuffix:=debug
TargetDir:=FF_SVI_DBG
fflint : build_raw 
ffthreads : build_raw
build : $(ffthreads) $(fflint) build_raw
else
TargetSuffix:=debug
TargetDir:=FF_SVI_DBG
build : build_raw
endif


#FFP_MNS:=$(ffroot)\target\mak\$(TargetDir)\exe\svi_ff_$(TargetSuffix).mns
FFP_MNS=$(ffroot)\target\mak\$(TargetDir)\exe\$(BFD_EXECUTABLE:.out=.mns)

clean :
	-del v.u
    -del ..\FD-SW\target\mak\FF_SVI_DBG\allfiles.lnt
    $(makecmd) $@ TargetSuffix=$(TargetSuffix)

build_raw : #unimal
    $(makecmd) build TargetSuffix=$(TargetSuffix)
    "$(subst /,\,$(CC_CORTEX_M3))"\bin\ielftool.exe --ihex --verbose $(FFP_MNS:.mns=.out) $(FFP_MNS:.mns=.hex)
    mnhextool.exe -i$(FFP_MNS:.mns=.hex) -b$(FFP_MNS)
	sed --text $(subst /,\,$(FFP_MNS)) -n -e s/\\(.*\\)/\\1/pg

#-------------- Abandoned for now ---------

ifeq (0,1)
INCDIR_INCLUDE_UNIMAL=-Iincludes -Iinc_FFAP -Iincludes\project_FFAP

#   Canned Unimal preprocessing command
#   It works its way to generate header dependencies via an intermediate file so that
#   if it fails at any point, the dependency file is not trash, and the next build can proceed.
define U_Command
    @echo . Making $@
    @echo . Preprocessing $< (new = $?)
    $(Hide)echo $(notdir $(<:.u=.h)) $(notdir $(<:.u=.c)) $(OBJDIR_RELATIVE)/$(notdir $(<:.u=.ud)) : \>$(REALNAME).tmp
    $(Hide)$(FIXDEP) $(REALNAME).ud_>>$(REALNAME).tmp
    $(Hide)echo $< $(LOCAL_CONF) $(local_topdep)>>$(REALNAME).tmp
    $(Hide)$(MN_MV) $(REALNAME).tmp $(REALNAME).ud
endef

uprojparam:=project_FFAP.inc
UPROJSPEC:=PROJPARAM_INC_=$(uprojparam)
UPARAMS:=-S$(UPROJSPEC)

# Unimal preprocessing rule
%.h %.c %.ud : %.u %.ud
    Unimal -I. $(INCDIR_INCLUDE_UNIMAL) -d$(basename $<).ud $(UPARAMS) $< -o.

NVRAMDIR:=$(ffroot)/target/sys/eep/src
vpath %.u $(NVRAMDIR)

unimal : nvram

#include $(NVRAMDIR)/ffnvramtable.ud

nvram : $(addprefix $(NVRAMDIR)/ffnvramtable., c h ud)

$(addprefix $(NVRAMDIR)/ffnvramtable., c h ud) :
    Unimal $(basename $@).u -I. $(INCDIR_INCLUDE_UNIMAL) -d$(basename $<).ud $(UPARAMS) $< -o.
endif
# -------------------------------------------------------------------------

#Do not change unless you know what you are doing
TokenizerDir:=c:\ff
FFTokenizerpath:=$(TokenizerDir)\tok\bin
dictfile:=$(TokenizerDir)\ddl\standard.dct
SurpressWarning:=718
includepath:=$(TokenizerDir)\ddl
releasepath:=$(TokenizerDir)\release
imagepath:=$(TokenizerDir)\ddl\htk

SOURCE_BINARY_DD:=$(releasepath)\$(manufacturer_ID)\$(type_ID)
TARGET_BINARY_DD:=$(subst /,\,$(ffroot))\target\appl\fbif\ddl\$(type_ID)
GW_DIR:=..\FD-SW\target\appl\fbif\script
DDLDIR:=$(ffroot)/target/appl/fbif/ddl
DDLSRC:=$(DDLDIR)/svi_positioner.ddl
DDLINC:=$(DDLDIR)/svi_ids.h

_tok: $(pretok)
    $(FFTokenizerpath)/ff_tok32.exe $(pretok)
ifneq ($(option),)
    cp $(SOURCE_BINARY_DD)/$(DEVICE_REV)$(DD_REV).ffo $(dst)/
    cp $(SOURCE_BINARY_DD)/$(DEVICE_REV)$(DD_REV).sym $(dst)/
else
    cp $(SOURCE_BINARY_DD)/$(DEVICE_REV)$(DD_REV).ff5 $(dst)/
    cp $(SOURCE_BINARY_DD)/$(DEVICE_REV)$(DD_REV).sy5 $(dst)/
endif

$(pretok) : $(DDLSRC) force
    @echo option=$(option)
    $(FFTokenizerpath)/ffpretok.exe $(option) -d $(dictfile) -w $(SurpressWarning) -I$(includepath) -R $(releasepath) -p "$(imagepath)" $< $@
	$(pause)

tok: $(DDLINC) $(GW_DIR)\ids.gw
    buildhelpers\cmpcpy.bat $(includepath)\standard.sym $(releasepath)\standard.sym
    -cmd /E /C mkdir $(manufacturer_ID)\$(type_ID)
    -rm -f -r $(SOURCE_BINARY_DD)
	-cmd /E /C mkdir $(SOURCE_BINARY_DD)
    $(MAKE) -f $(CURDIR)\$(firstword $(MAKEFILE_LIST)) -C $(releasepath) _tok DDLSRC=$(DDLSRC) pretok=$(TARGET_BINARY_DD)\_tmptok-4 dst=$(TARGET_BINARY_DD) option="-a -DDD4 -4"
    $(MAKE) -f $(CURDIR)\$(firstword $(MAKEFILE_LIST)) -C $(releasepath) _tok DDLSRC=$(DDLSRC) pretok=$(TARGET_BINARY_DD)\_tmptok dst=$(TARGET_BINARY_DD) option=

$(DDLINC) : $(MAKEFILE_LIST)
    @echo MAKEFILE_LIST = $(MAKEFILE_LIST)
    echo MANUFACTURER      0x$(manufacturer_ID),>$@
    echo DEVICE_TYPE       0x$(type_ID),>>$@
    echo DEVICE_REVISION   $(DEVICE_REV),>>$@
    echo DD_REVISION       $(DD_REV)>>$@

$(GW_DIR)\ids.gw  : ids.gw
    cp $< $@

ids.gw: v.u gw_rb_helper.u
    Unimal -Iincludes -O. gw_rb_helper.u

v.u : $(MAKEFILE_LIST)
    @echo MAKEFILE_LIST = $(MAKEFILE_LIST)
    $(Hide)echo #MP Setstr DEVICE_REV="$(DEVICE_REV)" >$@
    $(Hide)echo #MP Setstr RES_DEV_MINOR_REV="$(RES_DEV_MINOR_REV)" >>$@
    $(Hide)echo #MP Setstr RES_BUILD="$(RES_BUILD)" >>$@
    $(Hide)echo #MP Setstr RES_BUILD_REV="$(RES_BUILD_REV)" >>$@
    $(pause)


#--------------------------------------------------------------------------------------
fflint ffthreads DOX : force
	$(MAKE) -f fflint.mak $@ IARver=$(IARver)

OFFICIAL SEND:
    $(MAKE) $@ proj=FFAP plugin=$(CURDIR)/ffplugin.mak TargetDir=$(TargetDir) TargetSuffix=$(TargetSuffix) fflint=$(fflint) ffthreads=$(ffthreads) DIR=$(OFFDir) PROG=FFAP_0

MOCKOFF:
	$(MAKE) -f ffbuild.mak OFFICIAL SS= OFFroot=. buildname=test

force : ;
