include $(scripts)
include $(toolconf)

CMDDIR_ROOT__:=$(PROJDIR)\$(UTestHome)\unit_test

# AK:FUTURE
ifneq ($(UTSET),)
ifeq ($(output_format),HTML)
export EXTRACT_INFO:=EXTRACT_INFO
export EXTRACT_FILE:=$(PROJDIR)\extracts.txt
endif
#Redefine the target directory root
CMDDIR_ROOT:=$(_CURDIR)$(SLASH)$(UTestHome)$(SLASH)unit_test

#If UTSET is given without directory path, give it the default
ifeq ($(strip $(UTSET)),$(strip $(notdir $(UTSET))))
UTSETINC=$(CMDDIR_ROOT)$(SLASH)$(UTSET)
export OBJPREFIX:=$(output_format)$(UTSET:.mak=)
else
UTSETINC=$(CMDDIR_ROOT)$(SLASH)$(UTSET)
#UTSETINC=$(UTSET)
endif

#Include the unit test definition
include $(UTSETINC)
endif


# AK:TODO: Take UTEST_HARNESS out of dirwalk.mak somehow
export UTEST_HARNESS

ifeq ($(output_format),HTML)
export EXTRACT_INFO:=EXTRACT_INFO
export EXTRACT_FILE:=$(PROJDIR)\extracts.txt
endif

#Define the main (project) library
MAINLIB:=$(CMDDIR_PROJ)$(SLASH)$(PROG)$(FileExt_LIB)
override RUNTIME_MODEL:=$(RUNTIME_MODEL_DEBUG)

#Only now Redefine the target directory root
#CMDDIR_ROOT:=$(CMDDIR_ROOT__)
CMDDIR=$(CMDDIR_ROOT)$(SLASH)$(DIR)
CMDDIR_PROJ=$(CMDDIR)$(SLASH)$(PROJ_PREFIX)$(PROJ)$(PROJ_SUFFIX)

# Object directory OBJDIR is received on the command line
OBJDIR_PROJ:=$(OBJDIR)$(SLASH)$(PROJ_PREFIX)$(PROJ)$(PROJ_SUFFIX)

export RELINK=$(OBJDIR_PROJ)\$(basename $(UTSET)).relink

#Unit test harness/instrumentation build support (ISUBDIR and SUBDIR) come from top conf.mak

UTRUNS_: #for debugging
    @echo PROJDIR=$(PROJDIR)
    @echo PATH2PROJROOT=$(PATH2PROJROOT)
    @echo CMDDIR_ROOT=$(CMDDIR_ROOT)
    @echo CMDDIR=$(CMDDIR)
    @echo CMDDIR_PROJ=$(CMDDIR_PROJ)
    @echo OBJDIR_RELATIVE=$(OBJDIR_RELATIVE)
    @echo OBJDIR_EFFECTIVE=$(OBJDIR_EFFECTIVE)
    @echo OBJDIR=$(OBJDIR)



#Redefine the build/execute target name
BTARGET=$(OBJDIR_PROJ)$(SLASH)$(notdir $(basename $(UTSET)))$(output_format)$(FileExt_EXE)

# AK:TODO: Commonize with buildexec.mak, get projparam from makefile better
export hprojparam:=$(projparam).h
PROJSPEC:=PROJPARAM_H_="\"$(hprojparam)\""

CFLAGS:=$(CFLAGS_COMMON) $(CFLAGS_Utest) -D$(PROJSPEC)

ifneq ($(UTEST_INSTRUM_HDR),)
CFLAGS+="-DINSTRUM_HEADER=<$(UTEST_INSTRUM_HDR)>"
endif
ifeq ($(output_format),HTML)
CFLAGS+=-DHTML -DHTML_OUT
LOG_EXT:=.html
else
LOG_EXT:=.log
endif
ifneq ($(UTSET),)
CFLAGS+=--diag_suppress Pe177,Pe181
INCDIR+=$(PRIVATE_INCDIR) $(CMDDIR_ROOT)
else
CFLAGS+=--library_module
endif
export CFLAGS
export UTEST_HARNESS
export UTEST_C
export UTEST_A
export proj


# ========== Unit test build/exec/regress =============
TESTLOG:=$(basename $(BTARGET))$(LOG_EXT)
#Windows-specific (echo) angle brackets
export LBANGLE:=^<
export RBANGLE:=^>
ifeq ($(output_format),HTML)
TEE_APPEND:=-a
endif


UTTARGET=$(OBJDIR_PROJ)\$(basename $(UTSET))$(output_format)$(FileExt_EXE)
# AK:TODO Commonize with buildexec.mak
export LINKLIST:=$(basename $(UTTARGET))filelist.log
LINKFILES:=$(basename $(UTTARGET))filelist$(FileExt_LINKCMD)
# Update the list of object files if it is different
$(LINKFILES) : $(LINKLIST)
    $(Hide)echo . cmpcpy=$(cmpcpy)
    $(Hide)echo . Updating $@
    $(Hide)-$(cmpcpy) $< $@ >nul
    $(Hide)fc  $< $@ >nul


#TestdefHtml1:=$(addsuffix $(RBANGLE),$(addprefix $(LBANGLE)A href=,$(UTSET)))
#TestdefHtml2:=$(addsuffix $(LBANGLE)/A$(RBANGLE),$(UTSET))

UTRUNS : UTBUILD force
ifeq ($(output_format),HTML)
    echo $(LBANGLE)HTML$(RBANGLE)$(LBANGLE)HEAD$(RBANGLE)$(LBANGLE)TITLE$(RBANGLE) > $(TESTLOG)
    echo $(UTSET:.mak=) >> $(TESTLOG)
    echo $(LBANGLE)/TITLE$(RBANGLE)$(LBANGLE)META http-equiv="Content-Type" content="text/html; charset=iso-8859-1"$(RBANGLE) >> $(TESTLOG)
    echo $(LBANGLE)/HEAD$(RBANGLE)$(LBANGLE)BODY$(RBANGLE) >> $(TESTLOG)
    echo $(LBANGLE)H1 align="center"$(RBANGLE)$(LBANGLE)A name="TOP"$(RBANGLE)$(LBANGLE)/A$(RBANGLE)Unit Test Set Report$(LBANGLE)/H1$(RBANGLE) >> $(TESTLOG)
    echo Executed by %USERNAME% %DATE% %TIME% $(LBANGLE)br$(RBANGLE) >> $(TESTLOG)
    echo Test definition: $(LBANGLE)A href=../../$(UTSET)$(RBANGLE) $(UTSET) $(LBANGLE)/A$(RBANGLE) $(LBANGLE)br$(RBANGLE) >> $(TESTLOG)
    $(MN_CAT) $(EXTRACT_FILE) >> $(TESTLOG)
    echo $(LBANGLE)br$(RBANGLE)$(LBANGLE)br$(RBANGLE)$(LBANGLE)b$(RBANGLE)Note:$(LBANGLE)/b$(RBANGLE) >> $(TESTLOG)
    echo This is a presentation output. Regression test is done on plain-text file $(UTSET:.mak=.log) vs. $(UTSET:.mak=.ref)$(LBANGLE)br$(RBANGLE)$(LBANGLE)br$(RBANGLE) >> $(TESTLOG)
    $(DEBUGGER) --silent $(DEBUGOPTIONS) $(subst /,$(SLASH),$(UTTARGET)) $(DEBUG_PLUGIN) $(DEBUGOPTIONS_BACKEND) >> $(TESTLOG)
else
    $(DEBUGGER) --silent $(DEBUGOPTIONS) $(subst /,$(SLASH),$(UTTARGET)) $(DEBUG_PLUGIN) $(DEBUGOPTIONS_BACKEND) | $(MN_TEE) $(TEE_APPEND) $(TESTLOG)
endif
ifeq ($(output_format),HTML)
    echo $(LBANGLE)/BODY$(RBANGLE)$(LBANGLE)/HTML$(RBANGLE) >> $(TESTLOG)
endif

UTREGRESS : UTRUNS force
    $(Hide)fc $(UTTARGET:$(FileExt_EXE)=.log) $(UTTARGET:$(FileExt_EXE)=.ref)

UTBUILD : $(EXTRACT_INFO) utbuild $(UTTARGET)
    @echo In $@

EXTRACT_INFO:
    echo UUT details: > $(EXTRACT_FILE)

utbuild :
    @echo In $@: OBJPREFIX=$(OBJPREFIX)
    @echo In $@: PROJDIR=$(PROJDIR)
    @echo In $@: CMDDIR_PROJ=$(CMDDIR_PROJ)
    @echo In $@: OBJDIR=$(OBJDIR)
    -mkdir $(subst /,$(SLASH),$(CMDDIR))
    -mkdir $(subst /,$(SLASH),$(CMDDIR_PROJ))
    $(Hide)$(MN_ECHO) > $(LINKLIST)
    $(MAKE) -r -f $(buildmak) build


$(UTTARGET): $(RELINK) $(LINKFILES) $(MAKEFILE_LIST)
    @echo LINKFILES=$(LINKFILES)
    @echo MAKEFILE_LIST=$(MAKEFILE_LIST)
    @echo UTSETINC=$(UTSETINC)
    @echo BTARGET=$(BTARGET)
    @echo UTTARGET=$(UTTARGET)
    @echo OBJDIR=$(OBJDIR)
    @echo OBJDIR_PROJ=$(OBJDIR_PROJ)
    $(LinkCmd_UTest)

# ---------------------------------------------------
force : ;
