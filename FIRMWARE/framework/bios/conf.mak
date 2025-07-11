#framework/bios
#SOURCE, if defined, is a space-separated list of the sources in this directory
SOURCE:= ad.c timer.c

ifeq ($(FEATURE_CPU),LPC21x4)
SOURCE+=lpc_uart.c watchdog.c bsp.c timesrv.c pwm.c
ASOURCE+=bsp_a.s79
endif

ifeq ($(FEATURE_CPU),STM32F10xxE)
SOURCE+=stm_uart.c stm_watchdog.c \
   stm_dma.c stm_flash.c stm_timer.c stm_nvic.c stm_rcc.c stm_vectors.c
#Ernie's implementation of stm_flash.c wants this:
CFLAGS_LOCAL+=-D STM32F10X_XL
endif

#SUBDIR, if defined,  is a space-separated list of subdirectories with sources
SUBDIR:=
#ISUBDIR, if defined,  is a space-separated list of header subdirectories

#Compiler flags private to this directory are below
