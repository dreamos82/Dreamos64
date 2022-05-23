#ifndef _RTC_H_
#define _RTC_H_

#include <stdint.h>
#include <stdbool.h>

#define CMOS_ADDRESS_REGISTER 0x70
#define CMOS_DATA_REGISTER 0x71

uint64_t read_rtc_time();
bool is_rtc_updating();
#endif
