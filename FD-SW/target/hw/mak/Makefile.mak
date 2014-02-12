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
#                     Copyright (C) SOFTING IA GmbH 2013                     #
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
# PROJECT_NAME             Softing FF/PA FD 2.42.A                           #
#                                                                            #
# VERSION                  FF - 2.42.A                                       #
#                          PA - 2.42.A                                       #
#                                                                            #
# DATE                     14. Aug. 2013                                     #
#                                                                            #
#============================================================================#




# ===========================================================================

#FILE_NAME          hw/mak/Makefile.mak

# hw_mcpy.a16

#DESCRIPTION

#  Generates the hardware specific parts for the Softing FD software, not
#  depending on the fieldbus protocol (FF or PROFIBUS-PA).

# ===========================================================================

.PHONY: $(COMPONENT) clean_$(COMPONENT) deps_$(COMPONENT)

USED_MAKEFILES += $(TARGET)/$(COMPONENT)/mak/Makefile.mak

################################################################################
# defaults

COMPONENT = hw

include $(BASE)/mak/Makeenv.mak

SRC_DIRS =  $(TARGET)/$(COMPONENT)/src
INC_DIRS := $(INC_DIRS):$(subst /src,/inc,$(SRC_DIRS)):$(TARGET)/inc:$(TARGET)/STM32F10x_StdPeriph_Driver/inc

vpath %.c   $(SRC_DIRS)
vpath %.s		$(SRC_DIRS)
vpath %.h   $(INC_DIRS)


################################################################################
# target files of this component

ifeq ($(HW_TYPE), FBK2_HW)
SRCS = hw_init_fbk2.c
endif

ifeq ($(HW_TYPE), FBK_HW)
SRCS = hw_init.c
endif

ifeq ($(HW_TYPE), BFD_HW)
SRCS = hw_init.c
endif

#here the HW initialization file(s) of the customer HW shall be added
ifeq ($(HW_TYPE), CUSTOMER_HW)
SRCS = hw_init.c startup_stm32f10x_xl.s
endif

KRN_SRCS = hw_io.c hw_os.c hw_tim.c

SRCS += hw_periph_cfg.c hw_intr.c

ifneq (,$(findstring hm,$(ADD_COMM_COMP)))
  SRCS   += hw_hm.c
endif

ifneq (,$(findstring mm,$(ADD_COMM_COMP)))
  SRCS   += hw_mm.c
endif

ifneq ($(STACK_TYPE), PA)
  ifneq ($(SW_DOWNLOAD_TYPE), NO_SW_DOWNLOAD)
    ifeq ($(HW_TYPE), FBK2_HW)
      FBK_SRCS += hw_swdl_fbk2.c
    else
    	//SRCS += hw_swdl_svi.c
    	//GINCS = hw_svi_swdl_if.h
    endif
  endif
endif

#SRCS += $(if $(filter 32x 35x 3xx, $(IAR_COMPILER_VER)), malloc.c free.c)


GINCS += hw_if.h cpu.h stm32f10x.h core_cm3.h hw_i2c.h
CFGS  = hw_cfg.h

include $(TARGET)/$(COMPONENT)/mak/mn.mak

OBJECTS  = $(addprefix $(OBJ_DIR)/, $(addsuffix $(OBJ_SUFFIX), $(basename $(SRCS))))
ifeq ($(HW_TYPE), FBK_HW)
FBK_OBJS = $(addprefix $(OBJ_DIR)/, $(addsuffix $(OBJ_SUFFIX), $(basename $(FBK_SRCS))))
endif
ifeq ($(HW_TYPE), FBK2_HW)
FBK_OBJS = $(addprefix $(OBJ_DIR)/, $(addsuffix $(OBJ_SUFFIX), $(basename $(FBK_SRCS))))
endif

KRN_OBJS = $(addprefix $(OBJ_DIR)/, $(addsuffix $(OBJ_SUFFIX), $(basename $(KRN_SRCS))))


LIB = $(LIB_DIR)/$(COMPONENT)$(LIB_SUFFIX)
MAIN_LIB = $(LIB_DIR)/$(COMPONENT)_main$(LIB_SUFFIX)
KRN_LIB  = $(LIB_DIR)/$(COMPONENT)_krn$(LIB_SUFFIX)

################################################################################
# dependencies

# Generate Firmware

all: $(COMPONENT)

$(COMPONENT): $(LIB)

$(OBJECTS)  : $(USED_MAKEFILES)
$(FBK_OBJS) : $(USED_MAKEFILES)
$(KRN_OBJS) : $(USED_MAKEFILES)


ifeq ($(DELIVERY_TYPE), SOURCE)

  $(LIB)      : $(OBJECTS) $(KRN_OBJS)

endif


ifeq ($(DELIVERY_TYPE), OBJECT)

$(LIB)      : $(MAIN_LIB) $(KRN_LIB)

  ifeq ($(MAKE_ENVIRONMENT), SOFTING)

    ifeq ($(HW_TYPE), BFD_HW)

      $(KRN_LIB)  : $(KRN_OBJS)

    else

    	ifeq ($(HW_TYPE), CUSTOMER_HW)

    		$(MAIN_LIB) : $(OBJECTS)
    		$(KRN_LIB)  : $(KRN_OBJS)

    	else

      	$(KRN_LIB)  : $(KRN_OBJS) $(FBK_OBJS)

      endif

    endif

  else

    ifeq ($(HW_TYPE), BFD_HW)

      $(KRN_LIB)  : $(KRN_OBJS)

    else
    	ifeq ($(HW_TYPE), CUSTOMER_HW)

    		$(MAIN_LIB) : $(OBJECTS)
    		$(KRN_LIB)  : $(KRN_OBJS)

    	else

      	$(KRN_LIB)  :

      endif

    endif

  endif

endif

#ifeq ($(DELIVERY_TYPE), FBK)
#  $(LIB)      : $(OBJECTS) $(FBK_OBJS)
#  $(KRN_LIB)  : $(KRN_OBJS)
#else
#  $(LIB)      : $(OBJECTS) $(KRN_OBJS)
#  $(KRN_LIB)  :
#endif

ifeq ($(DELIVERY_TYPE), FBK)

  $(LIB)      : $(OBJECTS) $(KRN_LIB)

  ifeq ($(MAKE_ENVIRONMENT), SOFTING)

    $(KRN_LIB)  : $(KRN_OBJS) $(FBK_OBJS)

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
						$(TARGET)/$(COMPONENT)/mak/makefile.dps
ifeq ($(MAKE_ENVIRONMENT), SOFTING)
	-@rm -f $(KRN_LIB) \
						$(MAIN_LIB) \
						$(KRN_OBJS) \
						$(FBK_OBJS)
else
  ifneq ($(DELIVERY_TYPE), FBK)
	-@rm -f $(KRN_OBJS)
  endif
endif



#-------------------------------------------------------------------------------
# Maintain dependencies
ifeq ($(MAKE_ENVIRONMENT), SOFTING)
  ifeq ($(DELIVERY_TYPE), SOURCE)
    $(MAKEDIR)/$(COMPONENT)/mak/makefile.dps: $(SRCS) $(KRN_SRCS)
  else
    ifeq ($(HW_TYPE), BFD_HW)
      $(MAKEDIR)/$(COMPONENT)/mak/makefile.dps: $(SRCS) $(KRN_SRCS)
    else
    	ifeq ($(HW_TYPE), CUSTOMER_HW)
      	$(MAKEDIR)/$(COMPONENT)/mak/makefile.dps: $(SRCS) $(KRN_SRCS)
     	else
     	  $(or ifeq ($(HW_TYPE), FBK_HW), ifeq ($(HW_TYPE), FBK2_HW))
      	$(MAKEDIR)/$(COMPONENT)/mak/makefile.dps: $(SRCS) $(KRN_SRCS) $(FBK_SRCS)
     	endif
    endif
  endif
else
  ifeq ($(HW_TYPE), BFD_HW)
    $(MAKEDIR)/$(COMPONENT)/mak/makefile.dps: $(SRCS) $(KRN_SRCS)
  else
  	ifeq ($(HW_TYPE), CUSTOMER_HW)
    	$(MAKEDIR)/$(COMPONENT)/mak/makefile.dps: $(SRCS) $(KRN_SRCS)
   	else
    	$(MAKEDIR)/$(COMPONENT)/mak/makefile.dps: $(SRCS)
    endif
  endif
endif


#-------------------------------------------------------------------------------
# Deliver Sources

-include $(MAKEDIR)/$(COMPONENT)/mak/Makedlv.mak


################################################################################
# source file dependecies

-include $(TARGET)/$(COMPONENT)/mak/makefile.dps

################################################################################
