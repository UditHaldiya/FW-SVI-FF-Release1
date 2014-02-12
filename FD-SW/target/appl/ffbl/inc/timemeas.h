
#ifdef KION_PARAMS

typedef struct
  {
    STRINGV area[32];
    USIGN32 current;
    USIGN32 counter;
    USIGN32 max;
    USIGN32 max_cycle;
    USIGN32 zone1;
    USIGN32 zone2;
    USIGN32 zone3;
    USIGN32 zone4;
    USIGN32 zone5;
    USIGN32 zone6;
    USIGN32 zone7;
  }
AREA_TIMESPAN;


#ifndef P_XFB
	#error P_XFB should be defined.
#endif

#define MEASURING_LOCALS \
  T_SYS_TIME				  start_time; \
  T_SYS_TIME				  phase_start_time; \
  T_SYS_TIME				  phase_end_time; \
  T_SYS_TIME				  area_start_time; \
  T_SYS_TIME				  area_end_time;

#define ZONE1 80
#define ZONE2 160
#define ZONE3 256
#define ZONE4 384
#define ZONE5 512
#define ZONE6 640


#define GET_TIMESPAN(endtime, starttime) (##endtime .low > ##starttime .low) ? ##endtime .low - ##starttime .low : 0

#define AREA_START() osif_get_system_time(&area_start_time);

#define AREA_END(val) \
	osif_get_system_time(&area_end_time); \
	if ((GET_TIMESPAN(area_end_time, area_start_time)) > P_XFB->max_subarea.current) \
  {\
		P_XFB->max_subarea.current = GET_TIMESPAN(area_end_time, area_start_time); \
		memcpy(P_XFB->max_subarea.area, val, strlen(val)); \
		P_XFB->max_subarea.max_cycle = P_XFB->cycle_counter; \
  }\
	if ((area_end_time.low - area_start_time.low) < ZONE1) P_XFB->max_subarea.zone1++; \
  else if ((area_end_time.low - area_start_time.low) < ZONE2) P_XFB->max_subarea.zone2++; \
  else if ((area_end_time.low - area_start_time.low) < ZONE3) P_XFB->max_subarea.zone3++; \
  else if ((area_end_time.low - area_start_time.low) < ZONE4) P_XFB->max_subarea.zone4++; \
  else if ((area_end_time.low - area_start_time.low) < ZONE5) P_XFB->max_subarea.zone5++; \
  else if ((area_end_time.low - area_start_time.low) < ZONE6) P_XFB->max_subarea.zone6++; \
  else P_XFB->max_subarea.zone7++; \


#define PHASE_START() osif_get_system_time(&phase_start_time);

#define PHASE_END(areaname) \
	osif_get_system_time(&phase_end_time); \
	P_XFB-> ##areaname .current = GET_TIMESPAN(phase_end_time, phase_start_time); \
	P_XFB-> ##areaname .counter++; \
	if (P_XFB-> ##areaname .current > P_XFB-> ##areaname .max) \
  {\
		P_XFB-> ##areaname .max = P_XFB-> ##areaname .current; \
		P_XFB-> ##areaname .max_cycle = P_XFB->cycle_counter; \
  }\
  if (P_XFB-> ##areaname .current < ZONE1) P_XFB-> ##areaname .zone1++; \
  else if (P_XFB-> ##areaname .current < ZONE2) P_XFB-> ##areaname .zone2++; \
  else if (P_XFB-> ##areaname .current < ZONE3) P_XFB-> ##areaname .zone3++; \
  else if (P_XFB-> ##areaname .current < ZONE4) P_XFB-> ##areaname .zone4++; \
  else if (P_XFB-> ##areaname .current < ZONE5) P_XFB-> ##areaname .zone5++; \
  else if (P_XFB-> ##areaname .current < ZONE6) P_XFB-> ##areaname .zone6++; \
  else P_XFB-> ##areaname .zone7++; \

// 	memcpy(P_XFB-> ##areaname .area, val, strlen(val)); \

#define FB_EXEC_START() \
	osif_get_system_time(&start_time);\
	P_XFB->cycle_counter++;

#define FB_EXEC_END() \
	phase_start_time = start_time;\
	PHASE_END(exec_area)

#endif

#ifndef KION_PARAMS

#define MEASURING_LOCALS

#define AREA_START()
#define AREA_END(val)
#define PHASE_START()
#define PHASE_END(areaname)

#endif