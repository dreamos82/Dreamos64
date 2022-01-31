#ifndef __TIMER_H_
#define __TIMER_H_

#define PIT_CHANNEL_0_DATA_PORT 0x40
#define PIT_MODE_COMMAND_REGISTER   0x43

#define IO_APIC_IRQ_TIMER_INDEX 0x14 //Double check

#define PIT_COUNTER_VALUE 0x4A9

#define PIT_CONFIGURATION_BYTE 0b00110100
void calibrate_apic();
#endif
