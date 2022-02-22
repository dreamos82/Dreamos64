#include <logging.h>
#include <qemu.h>
#include <framebuffer.h>
#include <video.h>
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

void logline(log_level_t level, char* msg){
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
                if (useVgaOutput) {
                    _setVgaCursorPos(0, fbCurrentLine);
                    _printStr(logLevelStrings[level]);
                    _printStr(msg);
                    fbCurrentLine++;
                }
                else {
                    _fb_printStr(logLevelStrings[level], 0, fbCurrentLine, 0xFFFFFFFF, 0);
                    _fb_printStr(msg, logLevelStrLen, fbCurrentLine, 0xFFFFFFFF, 0);
                    fbCurrentLine++;
                }

                if (fbCurrentLine > fbMaxLine)
                        fbCurrentLine = 0;
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
