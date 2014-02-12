# FF AP project features

# =========== Microcontroller definitions ==============
include $(helpers)/mcu_LPC2114.mak

#=========== Discrete features (switches) ==============
# --------------- large-scale stuff -------------
FEATURE_CPU = LPC21x4
FEATURE_POSITION_CONTROL = INCLUDED
FEATURE_HARDWARE_PLATFORM = MNCB
##FEATURE_LOCAL_UI = NONE
FEATURE_LOCAL_UI = LCD
FEATURE_PRESSURE_INPUTS = INCLUDED
FEATURE_HART_TRANSMITTER_COMMANDS = EXCLUDED
FEATURE_CONTROL_LIMITS = INCLUDED
FEATURE_AO_RETRANSMIT = UNAVAILABLE
FEATURE_ACTIVATION = NONE

# -------------- I/O channels --------------------
FEATURE_PRESSURE_INPUTS = INCLUDED
FEATURE_PRESSURE_SENSOR_1 = SUPPORTED
FEATURE_PRESSURE_SENSOR_2 = SUPPORTED
FEATURE_PRESSURE_SENSOR_3 = SUPPORTED
FEATURE_PRESSURE_SENSOR_PILOT = SUPPORTED
FEATURE_PRESSURE_SENSOR_ATM = SUPPORTED
FEATURE_PVINPUT_SENSOR = AVAILABLE
FEATURE_REMOTE_POSITION_SENSOR = AVAILABLE
FEATURE_LOOPSIGNAL_SENSOR = UNSUPPORTED
# --------------- Analog Output support ------------------------------
FEATURE_AO = UNSUPPORTED
#FEATURE_CTLOUT = SUPPORTED


# --------------- presentation features --------
FEATURE_CUSTOM_UNITS = UNSUPPORTED
#------------- diagnostic features -------------
FEATURE_DIAG_POSITIONER_DIAG = INCLUDED
FEATURE_PARTIAL_STROKE_TEST = UNSUPPORTED
FEATURE_ESD_LIMITED_HART_CMDS = INCLUDED
FEATURE_TERMINAL_BOARD_TEST = UNSUPPORTED
FEATURE_ESD_CONTINUOUS_DIAGS = UNSUPPORTED
#------------- bumpless transfer -------------
FEATURE_BUMPLESS_XFER = DISABLED
#------------- local UI features -------------
FEATURE_UI_TRANSITION_FSAFE2MANUAL = DISABLED
FEATURE_UI_TRANSITION_FSAFE2SETUP = ENABLED
FEATURE_UI_SHOW_TRIPPED = DISABLED
FEATURE_UI_CYCLING_SIGNAL = DONT_SHOW
FEATURE_UI_CYCLING_SIGNAL_RES = POINT3
FEATURE_UI_CHARACTERIZATION = SHOW
FEATURE_UI_OPSTOP_ADJ = SHOW
FEATURE_UI_SIGNAL_LIMITS = SHOW
FEATURE_UI_TIGHT_SHUTOFF = SHOW #Temporary kludge
# --------------- setpoint features -----------------------------
FEATURE_SIGNAL_SETPOINT = DROPPED
FEATURE_DIGITAL_SETPOINT = SUPPORTED
# --------------- shutdown monitoring ---------------------
FEATURE_EVENT_MONITOR_SHUTDOWN = UNSUPPORTED
FEATURE_SOFTWARE_SHUTDOWN = DISABLED
# --------------- fault storage features ---------------------
FEATURE_USER_FAULT_CATEGORIES = UNSUPPORTED
# --------------- DO switch 1 features ----------------------
FEATURE_DO1_OVERRIDE_OPTION_CONFIG = FALSE
FEATURE_DO1_ACTION_CONFIGURABLE = TRUE
FEATURE_DO1_DIRECTION_CONFIGURABLE = TRUE
# --------------- DO switch 2 features ----------------------
FEATURE_DO2_OVERRIDE_OPTION_CONFIG = FALSE
FEATURE_DO2_ACTION_CONFIGURABLE = TRUE
FEATURE_DO2_DIRECTION_CONFIGURABLE = TRUE
# --------------- Log file system ---------------------------
FEATURE_LOGFILES = NOTUSED
# --------------- Write-protect hardware -------------
FEATURE_WRITEPROTECT = HWJUMPER
# --------------- Write-protect jumper behavior -------------
FEATURE_WPROTECT_MODE = FALSE
FEATURE_WPROTECT_CLRERR = FALSE
#Note: Similar stuff for HART is configured in message flags in xml definitions

#===== Project-dependent implementations of project-independent interfaces =====
# ======== parameter files =================
# These are the C headers that define parameters of whatever nature
# for the implementation that needs the parameters but otherwise
# doesn't care what they are.
# The parameter files macros in the makefile must use _H_PARAM suffix as in
# FOO_H_PARAM = foo123.h
# The C code must use the symbol with the _H_ suffix, as in
# #include FOO_H_. The Unimal parameter files have _U_PARAM suffix.
MEMMAP_H_PARAM = memmap_$(PROJ).h
SIGNALSP_H_PARAM = signalsp.h
#Became inc_$(PROJ)/mncbdefs_proj.h MNCBDEFS_PROJ_H_PARAM = mncbdefs_$(PROJ).h
BIOS_DEF_H_PARAM = bios_def.h
TIMEBASE_H_PARAM = timebase.h
PWM_H_PARAM = pwm.h

# ======= implementation files ==============
# These are the C sources that provide an implementation of required
# interfaces defined in a header file
# The parameter files macros in the makefile must use _C_IMPLEM suffix as in
# BAR_C_IMPLEM = bar2007.c
# The definitions of this sort are used in conf.mak files to include the
# appropriate implementation for the project. Whether they propagate to the
# project header is TBD. There is no obvious use for them in the C code.
# E.g. MNCBDEF_C_IMPLEM = mncbdefs.c
