/**
    \brief prototypes for maestra instrumentation of C keywords

   Version control:
    $Date: 9/29/09 11:53p $
	$Change: 233 $
	$Id: //depot/maestra/maestra_instrum.h#3 $
*/
/* Copyright (C) 2007-2008 MacroExpressions.
   Permission is granted to freely use and modify this file provided this
   comment block is retained verbatim.
*/

#ifndef MAESTRA_IMPLEM_H_
#define MAESTRA_IMPLEM_H_

#ifdef __cplusplus
extern "C" {
#endif

extern int maestra_false; //a 0 unbeknownst to the UUT

extern int maestra_condition(const char *opname,
                             const char *condition_name,
                             int condition,
                             const char *filename,
                             int line,
                             const char *function_name);

extern int maestra_while(const char *condition_name,
                         int condition,
                         const char *filename,
                         int line,
                         const char *function_name);

extern int maestra_false; //a 0 unbeknownst to the UUT

extern int maestra_announce(const char *opname,
                            int retval,
                            const char *filename,
                            int line,
                            const char *function_name);

extern long long maestra_announce_ll(const char *opname,
                                     const char *condition_name,
                                     long long condition,
                                     const char *filename,
                                     int line,
                                     const char *function_name);

extern int maestra_fail(const char *opname,
                             int retval,
                             const char *filename,
                             int line,
                             const char *function_name);

extern int maestra_logical_orand(const char *opname,
                             const char *exp_name,
                             int retval,
                             const char *filename,
                             int line,
                             const char *function_name);
#ifdef __cplusplus
}
#endif

#endif //MAESTRA_IMPLEM_H_
