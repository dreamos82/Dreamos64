#include <logging.h>
#include <stacktrace.h>
#include <stacktrace.h>


void printStackTrace(size_t level, bool printFunctionNames) {
    if (printFunctionNames == true) {
        pretty_log(Verbose, "Print function names not implemented yet");
    }
    
    StackFrame *cur_frame = __builtin_frame_address(0);
    size_t cur_level = 0;
    pretty_log(Verbose, "Stacktrace:");
    while ( cur_level < level && cur_frame != NULL ) {
        pretty_logf(Verbose, "(%d):\t at 0x%x", cur_level, cur_frame->rip);
        cur_frame = cur_frame->next;
        cur_level++;
    }
    
}
