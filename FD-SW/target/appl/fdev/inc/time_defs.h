#ifndef _TIME_DEFS_H_
#define _TIME_DEFS_H_

#define TIME_INSECOND_ONEHOUR    (60*60)
#define TIME_INSECOND_ONEDAY     (24*60*60)
#define TIME_INSECOND_ONEWEEK    (7*TIME_INSECOND_ONEDAY)
#define TIME_INSECOND_ONEMONTH   (30*TIME_INSECOND_ONEDAY)
#define TIME_INSECOND_ONEYEAR    (12*TIME_INSECOND_ONEMONTH)

#define NUMBER_OF_MS_IN_SECOND          (1000u)
#define MS_TO_SEC(x)                    ((x) / NUMBER_OF_MS_IN_SECOND)

#define TIME_DIFF(basetime,currenttime)     ((currenttime) - (basetime))

#endif //_TIME_DEFS_H_
