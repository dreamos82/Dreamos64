#include <timer.h>
#include <io.h>
void calibrate_apic() {
    //Configuration byte value is: 0b00'11'010'0 where: bits 6-7 are the channel (0), 4-5 the access mode (read/load lsb first then msb)
    // 1-3 mode of operation (we are using mode 2 rate generator), 0: BCD/Binary mode (0 we are using 16bit binary mode) 
    outportb(PIT_MODE_COMMAND_REGISTER, 0b00110100);
    outportb(PIT_CHANNEL_0_DATA_PORT, PIT_COUNTER_VALUE & 0xFF);
    outportb(PIT_CHANNEL_0_DATA_PORT, (PIT_COUNTER_VALUE >> 8));
}
