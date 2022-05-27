#include <rtc.h>
#include <io.h>
#include <logging.h>

uint64_t daysPerMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

uint8_t read_rtc_register(uint8_t port_number) {
    outportb(CMOS_ADDRESS_REGISTER, port_number);    
    return inportb(CMOS_DATA_REGISTER);
}

uint64_t read_rtc_time() {
    // Yes is nearly identical to the one in northport! (i implemented it there first and then ported it to Dreamos! :) 

    while ( is_rtc_updating() );
    uint8_t rtc_register_statusB = read_rtc_register(StatusRegisterB);
    bool is_24hours = rtc_register_statusB & 0x02;
    bool is_binary = rtc_register_statusB & 0x04;

    uint8_t seconds = read_rtc_register(Seconds);
    uint8_t minutes = read_rtc_register(Minutes);
    uint8_t hours = read_rtc_register(Hours);
    uint8_t year = read_rtc_register(Year);
    uint8_t dayofmonth = read_rtc_register(DayOfMonth);
    uint8_t month = read_rtc_register(Month);
    uint8_t last_seconds; 
    uint8_t last_minutes;
    uint8_t last_hours;
    uint8_t last_year;
    uint8_t last_dayofmonth;
    uint8_t last_month;
    do {
        last_seconds = seconds;
        last_minutes = minutes;
        last_hours = hours;
        last_year = year;
        last_dayofmonth = dayofmonth;
        last_month = month;

        while ( is_rtc_updating() );
        seconds = read_rtc_register(Seconds);
        minutes = read_rtc_register(Minutes);
        hours = read_rtc_register(Hours);
        year = read_rtc_register(Year);
        dayofmonth = read_rtc_register(DayOfMonth);
        month = read_rtc_register(Month);
    } while ( last_seconds == seconds && last_minutes == minutes && last_hours == hours && last_year == year &&  last_dayofmonth == dayofmonth && last_month == month);

    if ( !is_binary) {
        hours = convert_bcd_to_binary(hours);
        seconds = convert_bcd_to_binary(seconds);
        minutes = convert_bcd_to_binary(minutes);
        year = convert_bcd_to_binary(year);
        month = convert_bcd_to_binary(month);
        dayofmonth = convert_bcd_to_binary(dayofmonth);
    }

    uint64_t yearsSinceEpoch = (BASE_CENTURY + year) - 1970; // Let's count the number of years passed since the Epoch Year: (1970)
    uint64_t leapYears = yearsSinceEpoch / 4; // We need to know how many leap years too...
    bool addAnotherLeapYear = (yearsSinceEpoch % 4) > 1; // if yearsSinceEpoch % 4 is greater/equal than 2 we have to add another leap year
    
    if (addAnotherLeapYear) {
        leapYears++;
    }

    uint64_t daysCurrentYear = 0;
    for (int i = 0; i < month-1; i++) 
    {
        daysCurrentYear += daysPerMonth[i];
    }
    
    daysCurrentYear = daysCurrentYear + (dayofmonth);
    if ( !is_24hours && (hours & 0x80) )
    {
        hours = ((hours & 0x7F) + 12) % 24;
    }

    uint64_t daysSinceEpoch = (yearsSinceEpoch * 365) - 1; 
    uint64_t unixTimeOfDay = (hours * 3600) + (minutes * 60) + seconds;
    uint64_t secondsSinceEpoch = (daysSinceEpoch * 86400) + (leapYears * 86400) + (daysCurrentYear * 86400) + unixTimeOfDay;
    return secondsSinceEpoch;
}

bool is_rtc_updating() {
    uint8_t statusRegisterA = read_rtc_register(StatusRegisterA);
    return statusRegisterA & 0x80;
}

uint64_t convert_bcd_to_binary(uint8_t value) {
    return ((value / 16) * 10) + (value & 0x0f);
}
