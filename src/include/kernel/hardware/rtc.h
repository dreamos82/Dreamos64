#ifndef _RTC_H_
#define _RTC_H_

#include <stdint.h>
#include <stdbool.h>

#define CMOS_ADDRESS_REGISTER 0x70
#define CMOS_DATA_REGISTER 0x71

#define BASE_EPOCH_YEAR 1970
#define BASE_CENTURY    2000

enum rtc_ports {
    Seconds = 0x00,
    Minutes = 0x02,
    Hours = 0x04,
    WeekDay = 0x06,
    DayOfMonth = 0x07,
    Month = 0x08,
    Year = 0x09,

    StatusRegisterA = 0x0A,
    StatusRegisterB = 0x0B
};

typedef struct {

    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;

    uint8_t dayofmonth;
    uint8_t month;
    uint8_t year;

    uint8_t statusRegisterA;
    uint8_t statusRegisterB;

    bool is_binary;
    bool is_24hours;

} rtc_time_t;


uint8_t read_rtc_register(uint8_t port_number);
uint64_t convert_bcd_to_binary(uint8_t value);
uint64_t read_rtc_time();
bool is_rtc_updating();

#endif
