#Modeled after definitions in miscexec.mak
# := doesn't work below - late eval needed
ffroot=$(OFFroot)\..\FD-SW
FFP_BBASE=$(ffroot)\target\mak\$(TargetDir)\exe\$(FFOUTBASENAME)
FFP_BTARGET0=$(ffroot)\target\mak\$(TargetDir)\exe\$(FFOUTBASENAME)0.mns
FFP_FW_HEADER0=$(shell type $(subst /,\,$(FFP_BTARGET0)))
FFP_FW_CRCID0=$(subst $(FW_CRC),,$(filter $(FW_CRC)%,$(FFP_FW_HEADER0)))
FFP_FW_VERID0=$(subst $(FW_VER),,$(filter $(FW_VER)%,$(FFP_FW_HEADER0)))
ifeq ($(FFP_FW_CRCID0)$(FFP_FW_VERID0),)
SEND: send_error=$(error Unable to parse FF MNS header)
endif
FFP_MNS_OFFICIAL_BNAME0=FFFP_$(OFFDir)_$(OFFver)_$(FFP_FW_VERID0)_$(FFP_FW_CRCID0)

FFP_BTARGET1=$(ffroot)\target\mak\$(TargetDir)\exe\$(FFOUTBASENAME)1.mns
FFP_FW_HEADER1=$(shell type $(subst /,\,$(FFP_BTARGET1)))
FFP_FW_CRCID1=$(subst $(FW_CRC),,$(filter $(FW_CRC)%,$(FFP_FW_HEADER1)))
FFP_FW_VERID1=$(subst $(FW_VER),,$(filter $(FW_VER)%,$(FFP_FW_HEADER1)))
ifeq ($(FFP_FW_CRCID1)$(FFP_FW_VERID1),)
SEND: send_error=$(error Unable to parse FF MNS header)
endif
FFP_MNS_OFFICIAL_BNAME1=FFFP_$(OFFDir)_$(OFFver)_$(FFP_FW_VERID1)_$(FFP_FW_CRCID1)

fdp=$(shell type fdp.txt)
fdpath=$(lastword $(strip $(fdp)))\..\FD-SW

#What to do after FFAP modules are sync'ed
define VC_PLUGIN
    -$(MN_RM) -f -r $(ffroot)
    @echo Now getting FFP
    $(OFFVCS) workfold /map "$$/$(TFSProject)/Release1/FD-SW" $(ffroot) /workspace:$(OFFworkspace) $(OFFlogin)
    $(PAUSE)
    echo %TIME% Sync FF >> $(PROJDIR)\buildtime.log
    $(OFFVCS) get $(ffroot);$(OFFver) /recursive /force $(OFFlogin)
endef

ifeq ($(NO_MNS),1)
mns_cp:=echo Skipping #
else
mns_cp:=cp.exe
endif

MNS_OFFICIAL_NAME_1=$(subst $(PROG),$(PROJ)_1,$(MNS_OFFICIAL_NAME))
#What to do after FFAP output is uploaded
define SEND_PLUGIN
    @echo Checking in APP bank 1 image
    $(mns_cp) -f -p $(BTARGET:$(PROG).mns=$(PROJ)_1.mns) $(MNS_OFFICIAL_DIR)\$(MNS_OFFICIAL_NAME_1)
    $(mns_cp) -f -p $(BTARGET:$(PROG).mns=$(PROJ)_1.mns) $(MNS_OFFICIAL_DIR)\$(MNS_OFFICIAL_NAME_1:.mns=.map)
    @echo Also checking in FFP image
    @echo OFFroot=$(OFFroot) OFFDir=$(OFFDir) FFP_BTARGET=$(FFP_BTARGET)
    $(PAUSE)
    $(mns_cp) -f -p $(FFP_BBASE)0.mns $(MNS_OFFICIAL_DIR)\$(FFP_MNS_OFFICIAL_BNAME0)_0.mns
    $(mns_cp) -f -p $(FFP_BBASE)1.mns $(MNS_OFFICIAL_DIR)\$(FFP_MNS_OFFICIAL_BNAME1)_1.mns
    cp.exe -f -p $(FFP_BBASE)0.map $(MNS_OFFICIAL_DIR)\$(FFP_MNS_OFFICIAL_BNAME0)_0.map
    cp.exe -f -p $(FFP_BBASE)1.map $(MNS_OFFICIAL_DIR)\$(FFP_MNS_OFFICIAL_BNAME1)_1.map
    cp.exe -f -p $(ffroot)\target\appl\fbif\ddl\$(type_ID)/*.cff $(MNS_OFFICIAL_DIR)
    cp.exe -f -p $(ffroot)\target\appl\fbif\ddl\$(type_ID)/*.ff5 $(MNS_OFFICIAL_DIR)
    cp.exe -f -p $(ffroot)\target\appl\fbif\ddl\$(type_ID)/*.sy5 $(MNS_OFFICIAL_DIR)
    cp.exe -f -p $(ffroot)\target\appl\fbif\ddl\$(type_ID)/*.ffo $(MNS_OFFICIAL_DIR)
    cp.exe -f -p $(ffroot)\target\appl\fbif\ddl\$(type_ID)/*.sy5 $(MNS_OFFICIAL_DIR)
    @echo Also checking in .ffd files
    cp.exe -f -p $(BTARGET:.mns=.ffd) $(MNS_OFFICIAL_DIR)\ALLFF_$(OFFDir)_$(OFFver)_F$(FFP_FW_VERID0)_$(FFP_FW_CRCID0)_A$(FW_VERID)_$(FW_CRCID).ffd
    cp.exe -f -p $(BTARGET:.mns=1.ffd) $(MNS_OFFICIAL_DIR)\FFAP_$(OFFDir)_$(OFFver)_$(FW_VERID)_$(FW_CRCID).ffd
    cp.exe -f -p $(BTARGET:.mns=0.ffd) $(MNS_OFFICIAL_DIR)\FFFP_$(OFFDir)_$(OFFver)_$(FFP_FW_VERID0)_$(FFP_FW_CRCID0).ffd
    cp.exe -f -p tools\partman* $(MNS_OFFICIAL_DIR)
    $(PAUSE)
	@echo Now creating FFP snapshot
    $(PAUSE)
	$(MAKE) -C $(OFFroot) -f snapshot snapshot=snapshot_$(buildname)
    $(PAUSE)
endef

#What to do after FFAP is built
#	$(MAKE) -C $(OFFroot) -f ffbuild.mak clean tok $(fflint) build_raw $(ffthreads) DIR=$(OFFDir)
define BUILD_PLUGIN
    @echo Now building FFP at ROM bank 0
    $(PAUSE)
	$(MAKE) -C $(OFFroot) -f ffbuild.mak clean tok build
    @echo Now building APP at ROM bank 1
	cd
    $(PAUSE)
	cp $(BTARGET:.mns=.xlib) $(BTARGET:$(PROG).mns=$(PROJ)_1.xlib)
	cp $(BTARGET:.mns=.icf) $(BTARGET:$(PROG).mns=$(PROJ)_1.icf)
	$(MAKE) -C $(OFFroot) proj=FFAP MEMMAP MEMMAP_ROMSTART=08080000 MEMMAP_ROMEND=080F7777 -B
	$(MAKE) -C $(OFFroot) proj=FFAP BUILD NOGEN=1 NOLINKREFRESH=1 PROG=$(PROJ)_1
	$(MAKE) -C $(OFFroot) proj=FFAP MEMMAP -B
    @echo Now building FFP at ROM bank 1
    $(PAUSE)
	$(MAKE) -C $(OFFroot) -f ffbuild.mak build_raw rombank=1
    @echo Now building ffd images
    $(PAUSE)
	hex2ffd.exe $(FFP_BBASE)0.hex $(FFP_BBASE)1.hex $(BTARGET:$(PROG).mns=$(PROJ)_0.hex) $(BTARGET:$(PROG).mns=$(PROJ)_1.hex) $(BTARGET:.mns=.ffd)
endef

#What else to clean
define CLEAN_PLUGIN
endef
