// Tracing macros/functions.

#ifndef TRACE_H
#define TRACE_H
#include "utils.h"

typedef enum { TRACE_TRY, TRACE_ASSIGN, TRACE_BACKTRACK, TRACE_FAIL } TraceEvent;

typedef void (*trace_cb)(const char* line);
void trace_set_callback(trace_cb cb);
void trace_log_text(const char* line);


void trace_enable(int on);
int  trace_is_enabled(void);
void trace_emit(TraceEvent ev, int r, int c, int val);

#endif