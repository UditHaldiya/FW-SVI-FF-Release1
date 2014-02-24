LINT=lint-nt
LINT_CFG_DIR=LintCfg
LINT_INC=-i$(LINT_CFG_DIR)

lint_incdir:= \
    base\appl\ffbl\inc \
    target\appl\ffbl\inc \
    target\appl\fbif\inc \
    target\appl\fdev\inc \
    target\appl\hm\inc \
    target\cfg \
    target\inc \
    target\inc\ff \
    target\inc\segm \
    target\sys\osif.emb\inc \
	target\STM32F10x_StdPeriph_Driver\inc \
    _embOS_CortexM_IAR\Start\Inc

ffroot:=..\FD-SW
LINTDIR:=$(ffroot)\target\appl\fdev\src
LINC:=$(addprefix -i$(ffroot)\, $(lint_incdir))
#IMPORTANT:
# Global suppressions that cannot go into options.lnt may go here:
LINT_SUPPRESS= $(LINTDIR)\ffrelax.lnt
LINT_EXTRAS= $(LINTDIR)\lintextras.lnt

LINT_CFG=$(LINT_CFG_DIR)\std.lnt co5.lnt -dPROJPARAM_H_="\"project_FFAP.h\"" -dMN_BUILD_ENV -dCortex_M3 $(LINT_SUPPRESS) $(LINT_CMDLINE)
#export LINTFILE=$(PROJDIR)$(SLASH)project.lnt
LINTCLUDE:=cmdbuild\$(DIR)\project_FFAP\lintclude.lnt

mnmak:=$(shell dir /B/S $(ffroot)\mn.mak)
include $(mnmak)
cfiles:=$(shell dir /B/S $(ffroot)\*.c)
LSOURCE:=$(filter $(addprefix %,$(MNSOURCE)), $(cfiles))

#uncomment to test $(error $(LSOURCE))

include ../FD-SW/base/mak/Cortex_M3/Makedefaults.mak
predef:=iccarm.predef.$(IARver)
sysheaders:="$(ProgramFiles)\IAR Systems\Embedded Workbench 6.30\ARM\inc\c"
LINTCMD:=-header($(predef)) -i$(sysheaders) +libdir($(sysheaders)) $(LINTCLUDE) $(LINT_INC) $(LINC) $(LINT_CFG) $(LINTARG)

ALLFILES?=../FD-SW/target/mak/FF_SVI_DBG/allfiles.lnt
__ALLFILES=type $(ALLFILES

fflint : $(predef)
    echo MNSOURCE=$(MNSOURCE)
	echo $(LINTCMD) $(LSOURCE) $(LINT_EXTRAS) > temp.lnt
    $(LINT) temp.lnt > $(LINTDIR)\lint.log

ffthreads : $(predef)
	si-cat $(ALLFILES) | sort | uniq >uniqfiles.lnt
    $(LINT)  $(LINTCMD) -passes(1) -e* -e686 +e900 +e454 +e455 +e456 +e457 +e458 +e460 +e461 $(FFMORE) uniqfiles.lnt > $(LINTDIR)\lintthreads.log

$(predef) : force
    $(CC) $(CCFLAGS) --predef_macros $@ dummy.c

#------------- Stick Doxygen here

# - - - Masoneilan/Ernie's/Ark's/Unix-like add-ons - - - -
MNTOOLS:=.\tools
MNS_CONVERT:=$(MNTOOLS)\mnhextool.exe
MN_SED:=$(MNTOOLS)\sed.exe --text
UNIMAL:=$(MNTOOLS)\Unimal.exe
MN_TEE:=$(MNTOOLS)\ptee.exe
MN_CAT:=$(MNTOOLS)\si-cat.exe
MN_ZIP:=$(MNTOOLS)\7za.exe -tzip
MN_RM:=$(MNTOOLS)\rm.exe
MN_MV:=$(MNTOOLS)\mv.exe
#Win32 built-in echo is crippled; we take the one from GNU shelutils
MN_ECHO:=$(MNTOOLS)\echo.exe -E
_CURDIR:=$(subst /,\,$(CURDIR))
export PROJDIR:=$(_CURDIR)

dpsfiles:=$(shell dir /B/S $(ffroot)\*.dps)

content=$(shell $(MN_CAT) ffpdps.tmp )
input=$(filter %.c %.h, $(content))

scripts:=buildhelpers\scripts.mak
settings:=includes\project_FFAP\settings.mak
include $(scripts)
include $(settings)
FILELIST:=filelist_FFP.dox

ffdoxfiles:=ffdoxfiles.txt
doxinc:=doxinc_FFP.dox

$(doxinc) : force
	
$(FILELIST) : $(ALLFILES) force
	copy $< $@

ffpdps.tmp : force
    -del $@
    for %%i in ($(dpsfiles)) do type %%i >>$@

ffdoxraw.tmp : ffpdps.tmp
    $(Hide)$(KILL2DOTS) ffpdps.tmp | $(KILL2DOTS) | $(ONEPERLINE) | $(MN_SORT) | $(MN_UNIQ) -i > $@

ffdoxfiles.tmp : ffdoxraw.tmp
	$(Hide)$(MN_SED) \
	-e /$$(OBJ_DIR).*/d \
	-e s/.*FIRMWARE\///g -e s/\.\.\\/\.\.\/FD-SW\//g \
	-e s/\.\.\/\.\.\//\.\.\/FD-SW\/target\//g $< \
	| $(ADDBACKSLASH) > $@
	$(pause)

$(ffdoxfiles): ffdoxraw.tmp
    $(Hide)$(MN_ECHO) PREDEFINED = PROJPARAM_H_=\"project_FFAP.h\" >$@
    $(Hide)$(MN_ECHO) PREDEFINED = _lint= >>$@
    $(Hide)$(MN_ECHO) HTML_OUTPUT = html_FF>>$@
#	$(MN_ECHO) $(input) > file1.txt
    $(Hide)echo STRIP_FROM_PATH = ../FD-SW/target/mak/FF_SVI_DBG/../../>>$@
    $(Hide)echo PROJECT_NAME = FFP>>$@
    $(Hide)echo INCLUDE_PATH = \>>$@
	$(Hide)$(MN_ECHO) $(addprefix ..\FD-SW\, $(lint_incdir)) >>$@
#    $(Hide)$(ONEPERLINE) $(doxinc) | $(MN_SORT) | $(MN_UNIQ) -i | $(RELPATHS) | $(KILL2DOTS) > $(doxinc).tmp
#    $(Hide)$(ADDBACKSLASH) $(doxinc).tmp >>$@
    $(Hide)$(MN_ECHO) >>$@
    $(Hide)echo INPUT = \>>$@
#    $(Hide)$(ADDBACKSLASH) $(basename $@).tmp >>$@
	$(Hide)$(MN_SED) \
	-e /$$(OBJ_DIR).*/d \
	-e s/.*FIRMWARE\///g -e s/\.\.\\/\.\.\/FD-SW\//g \
	-e s/\.\.\/\.\.\//\.\.\/FD-SW\/target\//g $< \
	| $(ADDBACKSLASH) >> $@
    $(Hide)$(MN_ECHO) >>$@
	$(pause)

DOX : $(ffdoxfiles) force
    $(Hide)$(MN_ECHO) @INCLUDE = Doxyfile.base > DoxyfileFF
    $(Hide)$(MN_ECHO) CHM_FILE = ffdox.chm >> DoxyfileFF
    $(Hide)$(MN_ECHO) @INCLUDE = $(ffdoxfiles) >> DoxyfileFF
    $(Hide)$(MN_ECHO) EXCLUDE_PATTERNS = *.dep *.ewd *ewp *eww dox*.* >> DoxyfileFF
    $(Hide)$(MN_ECHO) DOT_PATH = \"$(ProgramFiles)\Graphviz2.32\bin\" >> DoxyfileFF
    $(Hide)$(MN_ECHO) WARN_LOGFILE = ffdoxygen.log >> DoxyfileFF
    $(DOXYGEN) DoxyfileFF

#----------- End Doxygen

force : ;
