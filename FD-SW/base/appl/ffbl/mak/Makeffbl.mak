#============================================================================#
#                                                                            #
#                     SOFTING Industrial Automation GmbH                     #
#                          Richard-Reitzner-Allee 6                          #
#                                D-85540 Haar                                #
#                        Phone: ++49-89-4 56 56-0                            #
#                          Fax: ++49-89-4 56 56-3 99                         #
#                                                                            #
#                            SOFTING CONFIDENTIAL                            #
#                                                                            #
#                     Copyright (C) SOFTING IA GmbH 2012                     #
#                             All Rights Reserved                            #
#                                                                            #
# NOTICE: All information contained herein is, and remains the property of   #
#   SOFTING Industrial Automation GmbH and its suppliers, if any. The intel- #
#   lectual and technical concepts contained herein are proprietary to       #
#   SOFTING Industrial Automation GmbH and its suppliers and may be covered  #
#   by German and Foreign Patents, patents in process, and are protected by  #
#   trade secret or copyright law. Dissemination of this information or      #
#   reproduction of this material is strictly forbidden unless prior         #
#   written permission is obtained from SOFTING Industrial Automation GmbH.  #
#============================================================================#
#============================================================================#
#                                                                            #
# PROJECT_NAME             Softing FF/PA FD 2.42                             #
#                                                                            #
# VERSION                  FF - 2.42                                         #
#                          PA - 2.42 (beta)                                  #
#                                                                            #
# DATE                     16. April 2012                                    #
#                                                                            #
#============================================================================#




# ===========================================================================

#FILE_NAME          appl/ffbl/mak/makeffbl.mak

#

#DESCRIPTION

#  Contains the correlation of FF function block kernel sources to the
#  FB types. Will be included by appl/fbif/mak/Makefile.mak and does there-
#  for not contain any statements to compile the software.

# ===========================================================================

USED_MAKEFILES += $(BASE)/appl/ffbl/mak/Makeffbl.mak

where-am-i = $(CURDIR)/$(word $(words $(USED_MAKEFILES)),$(USED_MAKEFILES))

include $(dir $(where-am-i))mn.mak

################################################################################
# block assembly

# common parts for all blocks
ALL_SRCS    =  ffbl_bas.c ffbl_res.c
ifeq ($(FIELD_DIAGNOSTICS), USE_FIELD_DIAGNOSTICS)
ALL_SRCS   +=  ffbl_fd.c
endif
ALL_INCS    =  ffbl_int.h ffbl_res.h
ALL_SCRIPTS =  resb2.gw profiles.dat views.dat std_views.dat stdStructs_unpacked.gw


# Analog Input block
AI_SRCS    =  ffbl_ai.c ffbl_pai.c
AI_INCS    =  ffbl_ai.h ffbl_pai.h
AI_SCRIPTS =  aifb.gw


# Multi Analog Input block
MAI_SRCS    =  ffbl_mai.c
MAI_INCS    =  ffbl_mai.h
MAI_SCRIPTS =  maifb.gw


# Analog Output block
AO_SRCS    =  ffbl_ao.c ffbl_mo.c ffbl_pao.c
AO_INCS    =  ffbl_ao.h ffbl_mo.h ffbl_pao.h
AO_SCRIPTS =  aofb.gw


# Multi Analog Output block
MAO_SRCS    =  ffbl_mao.c ffbl_mmo.c ffbl_pao.c
MAO_INCS    =  ffbl_mao.h ffbl_mmo.h ffbl_pao.h
MAO_SCRIPTS =  maofb.gw


# PID block
#PID_SRCS    = ffbl_pao.c ffbl_pai.c ffbl_mo.c $(MN_PID_SRCS)
PID_SRCS    += ffbl_pao.c ffbl_pai.c ffbl_mo.c
PID_INCS    = ffbl_pao.h ffbl_pai.h ffbl_mo.h $(MN_PID_INC)
PID_SCRIPTS = pidfb.gw


# Discrete Input block
DI_SRCS     = ffbl_di.c
DI_INCS     = ffbl_di.h
DI_SCRIPTS  = difb.gw


# Multi Discrete Input block
MDI_SRCS    =  ffbl_mdi.c
MDI_INCS    =  ffbl_mdi.h
MDI_SCRIPTS =  mdifb.gw


# Discrete Output block
DO_SRCS     = ffbl_do.c ffbl_mo.c
DO_INCS     = ffbl_do.h ffbl_mo.h
DO_SCRIPTS  = dofb.gw


# Multi Discrete Output block
MDO_SRCS    =  ffbl_mdo.c ffbl_mmo.c
MDO_INCS    =  ffbl_mdo.h ffbl_mmo.h
MDO_SCRIPTS =  mdofb.gw


# Integrator block
IT_SRCS    =  ffbl_it.c
IT_INCS    =  ffbl_it.h
IT_SCRIPTS =  itfb.gw


# Arithmetic block
AR_SRCS    =  ffbl_ar.c
AR_INCS    =  ffbl_ar.h
AR_SCRIPTS =  arfb.gw


# Signal Characterizer block
SC_SRCS    =  ffbl_sc.c
SC_INCS    =  ffbl_sc.h
SC_SCRIPTS =  scfb.gw


# Input Selector block
IS_SRCS    =  ffbl_is.c
IS_INCS    =  ffbl_is.h
IS_SCRIPTS =  isfb.gw


# Output Splitter block
OS_SRCS    =  ffbl_os.c
OS_INCS    =  ffbl_os.h
OS_SCRIPTS =  osfb.gw



ALL_BLK_SRCS = $(ALL_SRCS) \
               $(AI_SRCS) $(MAI_SRCS) $(AO_SRCS) $(MAO_SRCS) \
               $(DI_SRCS) $(MDI_SRCS) $(DO_SRCS) $(MDO_SRCS) \
               $(PID_SRCS) $(IT_SRCS) $(AR_SRCS) $(AR_SRCS) $(SC_SRCS) $(IS_SRCS) $(OS_SRCS) $(CS_SRCS)

ifeq ($(FBL_LIB_INCLUDE), ALL)
BLK_SRCS    = $(ALL_BLK_SRCS)
else
BLK_SRCS    = $(ALL_SRCS)    $(foreach block, $(BLOCK_LIST), $($(block:_BLOCK_AVAILABLE=)_SRCS))
endif
BLK_INCS    = $(ALL_INCS)    $(foreach block, $(BLOCK_LIST), $($(block:_BLOCK_AVAILABLE=)_INCS))
BLK_SCRIPTS = $(ALL_SCRIPTS) $(foreach block, $(BLOCK_LIST), $($(block:_BLOCK_AVAILABLE=)_SCRIPTS))

################################################################################
