SOURCE+=vramptest.c

ifeq ($(FEATURE_LOGFILES),USED)
SOURCE+=diagrw.c
endif
