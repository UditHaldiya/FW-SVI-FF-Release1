#Version control common configuration


#Version control root of the project
export VCS_PROJ_ROOT:="\mncb\dev\LCX2AP\firmware"

#Place for sharing xml commands definitions with tools
export VCS_SHARED_ROOT:="\Common Source\Dev\Current"


#Version control builder username
#Version control builder password

#Default compiler version
ver?=5.50

#Lint configuration
LINTARG=codebase.lnt

MN_INSTRUM=ffprofiler

#Doxygen version and location
DOXVER?=1.8.5
DOXYGEN:="$(ProgramFiles)\doxygen_$(DOXVER)\bin\doxygen.exe"
#graphviz_dotpath:="$(ProgramFiles)\Graphviz2.32\bin"

# Local UI languages setup - handy to have here
# Space-separated list which must be a subset of column names in the spreadsheet
# ... and of names defined in $(HELPERS)\uitext.pl (.xls extractor helper script)
LanguagePack ?= English French Spanish Portuguese Japanese Italian German


