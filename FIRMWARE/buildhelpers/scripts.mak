# This script adds a prefix and a suffix to each line
SURROUND=$(MN_SED) -e s%%^\(.*\)%%$(SURROUND_PREFIX)\x20\1\x20$(SURROUND_SUFFIX)%%g

# This script removes lines which contain at most spaces
REMOVE_EMPTY_LINES=$(MN_SED) -e "/^ *$$/d"

# This sed script will replace filename separating spaces with newlines
ONEPERLINE:=$(MN_SED) -e s%%\x20%%\n%%g

# This sed script will replace absolute paths with project-relative paths
RELPATHS:=$(MN_SED) -n -e s%%\($(subst \,\\,$(PROJDIR))\\\)%%%%pg

# This sed script will replace \foo\../ with \ (nothing important but .zip security concern)
KILL2DOTS:=$(MN_SED) -e s%%[/\\][_a-zA-Z0-9.]*[/\\]\.\.[/\\]%%\\%%g

# This sed script will end each line with " \" (without quotes)
RE_EOL=$#
ADDBACKSLASH:=$(MN_SED) -n -e s%%$(value RE_EOL)%%\x20\\%%gp

# This sed script will filter out the binary part of the file (beyond ^Z)
TEXTHEADER:=$(MN_SED) -n -e s/\\(.*\\)/\\1/pg

# This batch script copies a file if it is different from destination
cmpcpy=$(helpers)/cmpcpy.bat

# The following command:
# - eliminates system header dependencies
# - eliminates dependencies on *Untitled*
# - appends a space and a line continuation to each line (" \")
# Usage: cat <filename> | $(FIXDEP) (or simply $(FIXDEP) <filename>) to send to stdout
FIXDEP:=$(MN_SED) -e /Untitled/d -n -e s%%\($(subst \,\\,$(PROJDIR)).*\)%%\1\x20\\%%pg
# old FIXDEP:=$(MN_SED) -n -e s%%\($(subst \,\\,$(PROJDIR)).*\)%%\1\x20\\%%pg


# Sort
MN_SORT:=sort

# Uniq
MN_UNIQ:=$(MNTOOLS)\uniq.exe


LINT2LDRA:= $(MN_SED) -n -e s/-i/\x20\x201\x20/gp
SLASH2BACKSLASH := $(MN_SED) -e s.\/.\\\\.g
REMDOT:= $(MN_SED) -e s/\\\.//g

