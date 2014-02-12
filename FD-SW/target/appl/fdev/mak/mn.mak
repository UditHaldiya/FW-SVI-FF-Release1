MNSOURCE+=mnhart2ff.c mncharactff.c mnctllimff.c

MNSOURCE+=lui_ipc_variables.c
MNSOURCE+=histogram.c
MNSOURCE+=ctrl_param_set.c
MNSOURCE+=ui_language.c
MNSOURCE+=app_mode.c
MNSOURCE+=mndoswff.c
MNSOURCE+=pressure_range.c
MNSOURCE+=mnptbexec.c
MNSOURCE+=appl_alert.c
MNSOURCE+=mnfaultsimport.c mnptberrors.c
MNSOURCE+=mn_cycle_counter_alert.c
MNSOURCE+=mn_travel_accumulation.c
MNSOURCE+=mn_trends.c
MNSOURCE+=eep_blk2.c
MNSOURCE+=mn_clear_status.c
MNSOURCE+=mn_actuator_3.c
MNSOURCE+=mn_near_closed_alert.c
MNSOURCE+=mnfindstopsff.c
MNSOURCE+=mn_travel_units.c
MNSOURCE+=mn_working_times.c
MNSOURCE+=mn_working_time_alert.c
MNSOURCE+=mn_grant_deny.c
MNSOURCE+=mndiagprocff.c mntbmspff.c
MNSOURCE+=mn_fstate.c
MNSOURCE+=mn_xd_errors.c
MNSOURCE+=mn_errorbits.c
MNSOURCE+=mn_ui_access_control.c
MNSOURCE+=mnipcff.c
MNSOURCE+=mn_supply_pressure_alert.c
MNSOURCE+=mn_pressure_extremes.c
MNSOURCE+=mn_ip_drive_current_alert.c
MNSOURCE+=mn_factory_defaults.c
MNSOURCE+=appl_ptb.c
MNSOURCE+=ffactoryuse.c
MNSOURCE+=mn_temperature_alert.c
MNSOURCE+=mn_remote_sensor.c
MNSOURCE+=mn_checkproc.c
MNSOURCE+=mn_fwdl.c
MNSOURCE+=mn_fwdl_hart.c
MNSOURCE+=mn_decomp.c
MNSOURCE+=mn_hdr_validate.c
MNSOURCE+=controller_pid.c
MNSOURCE+=mn_block_header.c
MNSOURCE+=mn_deviation_alert.c
MNSOURCE+=mn_advanced.c
MNSOURCE+=mn_diagnostic_configuration.c
MNSOURCE+=eep_blk3.c

SRCS+=$(MNSOURCE)

#This is how we integrate MN source files in Softing build framework
vpath %.c ../../../../FIRMWARE/sysio
SRCS+=poschrtab.c
vpath %.c ../../../../FIRMWARE/utilities
SRCS+=bufutils.c crc.c
vpath %.c ../../../../FIRMWARE/framework
SRCS+=mncbdefs.c verinfo.c
vpath %.c ../../../../FIRMWARE/framework/bios
SRCS+=stm_flash.c
vpath %.c ../../../../FIRMWARE/services
SRCS+=fault2alert.c

#A copy of Softing Assembler rule for MN (historic) extension
$(OBJ_DIR)/%$(OBJ_SUFFIX) : %.s79
        $(ASSEMBLE_ARM)

