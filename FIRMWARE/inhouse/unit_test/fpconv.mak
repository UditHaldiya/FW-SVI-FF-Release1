UTEST_HARNESS:=sysstubs_M3.c harness_fpconv_common.c harness_fpconv_tab.c stubs\nvramstubs.c
UTEST_C:= fpconvert.c customunits.c unitstable.c numutils.c#
UTEST_A:=
UTEST_LIB:=$(PROJDIR)\mn_instrum\maestra\maestra$(output_format).a
UTEST_LIB:=mn_instrum\maestra\maestraM3-5.50$(output_format).a

#We don't want instrumentation clutter in a sweep test: would be too much.
#UTEST_INSTRUM_HDR:=maestra_instrum.h

MN_INSTRUM=maestra
PRIVATE_INCDIR:=$(PROJDIR)\interface $(PROJDIR)\mn_instrum\maestra
CFLAGS_Utest+= -DUNIT_TEST_ENVIRONMENT 