vpath %.c ../../../../FIRMWARE/services
SRCS+=mnassert.c
vpath %.c ../../../../FIRMWARE/utilities
SRCS+=crc.c
vpath %.c ../../appl/hw/src
MNSOURCE+=hw_i2c.c
vpath %.c ../../../../FIRMWARE/framework
SRCS+=cortextrapc.c

SRCS+=stubs_app.c

SRCS+=$(MNSOURCE)

#A copy of Softing Assembler rule for MN (historic) extension
$(OBJ_DIR)/%$(OBJ_SUFFIX) : %.s79
	$(ASSEMBLE_ARM)

vpath %.s79 ../../../../FIRMWARE/framework
SRCS+=cortextrap.s79
SRCS+=mncortexm.s79 endofimage.s79
