#include <logging.h>
#include <qemu.h>
#include <framebuffer.h>
#include <video.h>
#include <io.h>
#include <stdio.h>
#include <stdarg.h>

char* logLevelStrings[] = {
    "  [DEBUG] ",
    "[VERBOSE] ",
    "   [INFO] ",
    "  [ERROR] ",
    "  [FATAL] ",
};
const size_t logLevelStrLen = 10; //all the above strings are 10 chars long (excluding null terminator)
const size_t formatBufferLen = 256; //formatted log output limit, in characters.

size_t logDestBitmap;
size_t logTrimLevel;

size_t fbCurrentLine;
size_t fbMaxLine;
bool useVgaOutput;

void init_log(size_t defaultOutputs, log_level_t trimBelowLevel, bool useVgaVideo){
    if (defaultOutputs == LOG_OUTPUT_DONT_CARE)
        defaultOutputs = LOG_OUTPUT_SERIAL; //default to serial

    logDestBitmap = defaultOutputs;
    logTrimLevel = trimBelowLevel;

    useVgaOutput = useVgaVideo;

    if (useVgaOutput) {
        fbMaxLine = _SCR_H;
    }
    else {
        uint32_t pw, ph, cw, ch;
        get_framebuffer_mode(&pw, &ph, &cw, &ch);
        fbMaxLine = ch;
    }
}

void set_log_trim_level(size_t newTrim){
    logTrimLevel = newTrim;
}

void logline(log_level_t level, const char* msg){
    logline_to(logDestBitmap, level, msg);
}

void logline_to(size_t outputs, log_level_t level, const char* msg){
    if (level < logTrimLevel)
        return; //dont log things that we dont want to see for now. (would be nice to store these somewhere in the future perhaps, just not display them?)

    for (size_t i = 0; i < LOG_OUTPUT_COUNT; i++){
        if ((outputs & (1 << i)) == 0)
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
                //TODO: fbCurrentLine should be aligned with cur_fbLine in the framebuffer case.
                // fbCurrentLine can be removed, but we probably need to still use _fb_printStrAt
                _fb_printStrAt(logLevelStrings[level], 0, cur_fb_line, 0xFFFFFFFF, 0);
                _fb_printStr(msg, 0xFFFFFFFF, 0);
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



void loglinef(log_level_t level, const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    loglinef_to(logDestBitmap, level, msg, args);
    va_end(args);
}

void loglinef_serial_only(log_level_t level, const char* msg, ...){
    va_list args;
    va_start(args, msg);
    loglinef_to(LOG_OUTPUT_SERIAL, level, msg, args);
    va_end(args);
}

void loglinef_to(size_t outputs, log_level_t level, const char* msg, va_list format_args)
{
    char format_buffer[formatBufferLen];

    //va_list format_args;
    //va_start(format_args, msg);
    vsprintf(format_buffer, msg, format_args);
    //va_end(format_args);

    logline_to(outputs, level, format_buffer);
}
