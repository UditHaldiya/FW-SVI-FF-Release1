# Control Selector block
CS_SRCS    =  ffbl_cs.c
CS_INCS    =  ffbl_cs.h
CS_SCRIPTS =  csfb.gw

# MN Part of PID block (done by MESCO)

#Temprarily
PID_SRCS+=ffbl_pid.c ffbl_krn.c
MN_PID_SRCS=
#MN_PID_SRCS = ffbl_pid.c ffbl_krn.c

MN_PID_INC = ffbl_pid.h


# Collect sources for lint
MNSOURCE+=$(CS_SRCS) $(MN_PID_SRCS)

