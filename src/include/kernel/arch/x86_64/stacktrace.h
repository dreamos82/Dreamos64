#ifndef __STACKTRACE_H__
#define __STACKTRACE_H__

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct StackFrame StackFrame;

struct StackFrame{
    StackFrame *next;
    uintptr_t rip;
};

void printStackTrace(size_t level, bool printFunctionNames);


#endif
