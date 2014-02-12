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

#FILE_NAME          Makefile.mak

#

#DESCRIPTION

#  Generates the operating system interface for Segger embOS M16C, not depending
#  on the fieldbus protocol (FF or PROFIBUS-PA).

# ===========================================================================

.PHONY: $(COMPONENT) clean_$(COMPONENT) deps_$(COMPONENT) dlv_$(COMPONENT)

USED_MAKEFILES += $(TARGET)/sys/$(COMPONENT)/mak/makefile.mak

################################################################################
# defaults

COMPONENT = osif.emb

include $(BASE)/mak/Makeenv.mak

SRC_DIRS  =  $(TARGET)/sys/$(COMPONENT)/src:$(BASE)/sys/$(COMPONENT)/src
INC_DIRS  := $(INC_DIRS):$(subst /src,/inc,$(SRC_DIRS))

vpath %.c   $(SRC_DIRS)
vpath %.h   $(INC_DIRS)


################################################################################
# target files of this component

#SRCS  = osif_mn.c osif_tsk.c JLINKMEM_Process.c
SRCS  = osif_mn.c osif_tsk.c

KRN_SRCS = osif_cfg.c osif_tim.c osif_sem.c osif_dbl.c

SRCS += xmtx.c

INCS  = osif.h osif_int.h osif_cfg.h osif_dbl.h
SINCS = da_embos.h

OBJECTS = $(addprefix $(OBJ_DIR)/, $(addsuffix $(OBJ_SUFFIX), $(basename $(SRCS))))
KRN_OBJS = $(addprefix $(OBJ_DIR)/, $(addsuffix $(OBJ_SUFFIX), $(basename $(KRN_SRCS))))

LIB = $(LIB_DIR)/$(COMPONENT)$(LIB_SUFFIX)
KRN_LIB = $(LIB_DIR)/osif_krn.emb$(LIB_SUFFIX)
MAIN_LIB = $(LIB_DIR)/$(COMPONENT)_main$(LIB_SUFFIX)

################################################################################
# dependencies

all: $(COMPONENT)

$(COMPONENT): $(LIB)

$(OBJECTS)  : $(USED_MAKEFILES)

$(KRN_OBJS)  : $(USED_MAKEFILES)


ifeq ($(DELIVERY_TYPE), SOURCE)

  $(LIB)      : $(OBJECTS) $(KRN_LIB)
  $(KRN_LIB)  : $(KRN_OBJS)

endif


ifeq ($(DELIVERY_TYPE), OBJECT)

  $(LIB)      : $(MAIN_LIB) $(KRN_LIB)
  $(KRN_LIB)  : $(KRN_OBJS)
  $(MAIN_LIB) : $(OBJECTS)

endif


ifeq ($(DELIVERY_TYPE), FBK)

  $(LIB)      : $(OBJECTS) $(KRN_LIB)

  ifeq ($(MAKE_ENVIRONMENT), SOFTING)

    $(KRN_LIB)  : $(KRN_OBJS)

  else

    $(KRN_LIB)  :

  endif

endif


#-------------------------------------------------------------------------------
# Clean up

clean_$(COMPONENT):
	@echo cleaning $(COMPONENT)
	-@rm -f $(LIB) \
            $(OBJECTS) \
            $(TARGET)/sys/$(COMPONENT)/mak/makefile.dps
ifeq ($(MAKE_ENVIRONMENT), SOFTING)
	-@rm -f $(KRN_LIB) \
						$(MAIN_LIB) \
            $(KRN_OBJS) \
else
  ifneq ($(DELIVERY_TYPE), FBK)
	-@rm -f $(KRN_OBJS)
  endif
endif

#-------------------------------------------------------------------------------
# Maintain dependencies
ifeq ($(MAKE_ENVIRONMENT), SOFTING)
$(MAKEDIR)/sys/$(COMPONENT)/mak/makefile.dps: $(SRCS) $(KRN_SRCS)
else
$(MAKEDIR)/sys/$(COMPONENT)/mak/makefile.dps: $(SRCS)
endif


#-------------------------------------------------------------------------------
# Deliver Sources

-include $(MAKEDIR)/sys/$(COMPONENT)/mak/Makedlv.mak


################################################################################
# source file dependecies

-include $(MAKEDIR)/sys/$(COMPONENT)/mak/makefile.dps

################################################################################
