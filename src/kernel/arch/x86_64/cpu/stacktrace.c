#include <logging.h>
#include <stacktrace.h>
#include <stacktrace.h>


void printStackTrace(size_t level, bool printFunctionNames) {
    if (printFunctionNames == true) {
        pretty_log(Info, "Print function names not implemented yet");
    }

    StackFrame *cur_frame = __builtin_frame_address(0);
    size_t cur_level = 0;
    pretty_log(Info, "Stacktrace:");
    while ( cur_level < level && cur_frame != NULL ) {
        pretty_logf(Info, "(%d):\t at 0x%x", cur_level, cur_frame->rip);
        cur_frame = cur_frame->next;
        cur_level++;
    }

}
