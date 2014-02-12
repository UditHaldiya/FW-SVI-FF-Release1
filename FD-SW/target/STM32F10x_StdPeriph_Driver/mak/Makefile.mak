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

#FILE_NAME          STM32F10x_StdPeriph_Driver/mak/Makefile.mak

# hw_mcpy.a16

#DESCRIPTION

#  Generates the hardware specific parts for the Softing FD software, not
#  depending on the fieldbus protocol (FF or PROFIBUS-PA).

# ===========================================================================

.PHONY: $(COMPONENT) clean_$(COMPONENT) deps_$(COMPONENT)

USED_MAKEFILES += $(TARGET)/STM32F10x_StdPeriph_Driver/mak/Makefile.mak

################################################################################
# defaults

#COMPONENT = driver
COMPONENT = STM32F10x_StdPeriph_Driver

include $(BASE)/mak/Makeenv.mak

SRC_DIRS =  $(TARGET)/STM32F10x_StdPeriph_Driver/src
INC_DIRS := $(INC_DIRS):$(subst /src,/inc,$(SRC_DIRS)):$(TARGET)/inc

vpath %.c   $(SRC_DIRS)
vpath %.h   $(INC_DIRS)


################################################################################
# target files of this component

SRCS =	misc.c stm32f10x_crc.c stm32f10x_exti.c stm32f10x_flash.c stm32f10x_fsmc.c stm32f10x_gpio.c \
				stm32f10x_i2c.c stm32f10x_rcc.c stm32f10x_tim.c stm32f10x_usart.c

GINCS = 
INCS =	misc.h stm32f10x_crc.h stm32f10x_exti.h stm32f10x_flash.h stm32f10x_fsmc.h stm32f10x_gpio.h \
				stm32f10x_i2c.h stm32f10x_rcc.h stm32f10x_tim.h stm32f10x_usart.h

OBJECTS  = $(addprefix $(OBJ_DIR)/, $(addsuffix $(OBJ_SUFFIX), $(basename $(SRCS))))

LIB = $(LIB_DIR)/$(COMPONENT)$(LIB_SUFFIX)

################################################################################
# dependencies

# Generate Firmware

all: $(COMPONENT)

$(COMPONENT): $(LIB)

$(OBJECTS)  : $(USED_MAKEFILES)


$(LIB)      : $(OBJECTS) $(KRN_OBJS)


#-------------------------------------------------------------------------------
# Clean up

clean_$(COMPONENT):
	@echo cleaning $(COMPONENT)
	-@rm -f $(LIB) \
						$(OBJECTS) \
						$(TARGET)/STM32F10x_StdPeriph_Driver/mak/makefile.dps



#-------------------------------------------------------------------------------
# Maintain dependencies

$(MAKEDIR)/STM32F10x_StdPeriph_Driver/mak/makefile.dps: $(SRCS)

#-------------------------------------------------------------------------------
# Deliver Sources

-include $(MAKEDIR)/STM32F10x_StdPeriph_Driver/mak/Makedlv.mak


################################################################################
# source file dependecies

-include $(TARGET)/STM32F10x_StdPeriph_Driver/mak/makefile.dps

################################################################################
