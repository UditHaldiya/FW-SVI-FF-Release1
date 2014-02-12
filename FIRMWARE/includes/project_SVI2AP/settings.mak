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

#Doxygen version and location
DOXVER?=1.6.2
DOXYGEN="C:\Program Files\doxygen $(DOXVER)\bin\doxygen.exe"


