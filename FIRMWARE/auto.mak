TF = "$(ProgramFiles)\Microsoft Visual Studio 10.0\Common7\IDE\tf.exe"
TFPT = "$(ProgramFiles)\Microsoft Team Foundation Server 2010 Power Tools\tfpt.exe"

appserverc = $(firstword $(shell sort /R appserverc.txt))
synccmd = $(TF) get . /recursive /noprompt
out_dir := C:\FF_Auto_Builds\FromReleasesBranch\FromRelease1
buildname = C$(appserverc)
uniqroot = C:\tfsbuildR\SVIFF\Release1
OFFroot = $(uniqroot)\FIRMWARE
OFFmodroot = $(uniqroot)\Core\FIRMWARE

all :
    $(TF) history . /noprompt /sort:ascending /recursive | sed -e :a -e "$$!d" >appserverc.txt
    $(TF) history ..\FD-SW /noprompt /sort:ascending /recursive | sed -e :a -e "$$!d" >>appserverc.txt
    @echo $(appserverc)
    $(synccmd)
    if not exist $(out_dir)\$(buildname) cmd /C mkdir $(out_dir)\$(buildname) && $(MAKE) -f ffbuild.mak OFFICIAL notask=1 buildname=$(buildname) OFFver=$(buildname) \
	OFFroot=$(OFFroot) OFFmodroot=$(OFFmodroot) MNS_OFFICIAL_DIR=$(out_dir)\$(buildname) silent=1 >$(out_dir)\$(buildname)\build.log 2>&1

