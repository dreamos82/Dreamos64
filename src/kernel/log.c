#include <log.h>
#include <qemu.h>
#include <framebuffer.h>
#include <io.h>

char* logLevelStrings[] = {
    "  [DEBUG] ",
    "[VERBOSE] ",
    "   [INFO] ",
    "  [ERROR] ",
    "  [FATAL] ",
};
const size_t logLevelStrLen = 10; //all the above strings are 10 chars long (excluding null terminator)

size_t logDestBitmap;
size_t logTrimLevel;

size_t fbCurrentLine;
size_t fbMaxLine;
void init_log(size_t defaultOutputs, log_level_t trimBelowLevel){
    if (defaultOutputs == LOG_OUTPUT_DONT_CARE)
        defaultOutputs = LOG_OUTPUT_SERIAL; //default to serial
    
    logDestBitmap = defaultOutputs;
    logTrimLevel = trimBelowLevel;

    uint32_t pw, ph, cw, ch;
    get_framebuffer_mode(&pw, &ph, &cw, &ch);
    fbMaxLine = ch;
}

void set_log_trim_level(size_t newTrim){
    logTrimLevel = newTrim;
}

void logln(log_level_t level, char* msg){
    if (level < logTrimLevel)
        return; //dont log things that we dont want to see for now. (would be nice to store these somewhere in the future perhaps, just not display them?)
    
    for (size_t i = 0; i < LOG_OUTPUT_COUNT; i++){
        if ((logDestBitmap & (1 << i)) == 0)
            continue; //bit is cleared, we should not log there

        switch (1 << i){
            case LOG_OUTPUT_SERIAL:
                qemu_write_string(logLevelStrings[level]);
                qemu_write_string(msg);
                qemu_write_string("\r\n");
                break;

            case LOG_OUTPUT_DEBUGCON:
                debugcon_write_string(logLevelStrings[level]);
                debugcon_write_string(msg);
                debugcon_write_string("\r\n");
                break;

            case LOG_OUTPUT_FRAMEBUFFER:
                
                break;
        
            default:
                continue;
        }
    }

    if (level == Fatal)
    {
        cli();
        while (1);
    }
}
