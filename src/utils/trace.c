// Minimal tracing utilities used during debug.

#include "trace.h"
#include <stdio.h>
static int g_trace=0;
static trace_cb g_cb=NULL;

// trace_enable(): helper function.
void trace_enable(int on){ g_trace = on?1:0; }
// trace_is_enabled(): helper function.
int trace_is_enabled(void){ return g_trace; }

// trace_emit(): helper function.
void trace_emit(TraceEvent ev, int r, int c, int val){
    if(!g_trace) return;
    const char* s="UNK";
    if(ev==TRACE_TRY) s="TRY";
    else if(ev==TRACE_ASSIGN) s="ASSIGN";
    else if(ev==TRACE_BACKTRACK) s="BACKTRACK";
    else if(ev==TRACE_FAIL) s="FAIL";
    printf("[TRACE] %s r=%d c=%d v=%d\n", s, r+1, c+1, val);
    if(g_cb){ char buf[96]; snprintf(buf,sizeof(buf),"%s r=%d c=%d v=%d", s, r+1, c+1, val); g_cb(buf);}
}

// trace_set_callback(): helper function.
void trace_set_callback(trace_cb cb){ g_cb = cb; }

// trace_log_text(): helper function.
void trace_log_text(const char* line){
    if(!line) return;
    if(g_cb) g_cb(line);
    // also mirror to stdout for CLI debugging
    printf("[TRACE] %s\n", line);
}