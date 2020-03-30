#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdint.h>

class Utilities
{
public:
    Utilities();

    static bool validDate(uint8_t month, uint8_t day, uint16_t year);
    static bool leapYear(uint16_t year);

    static bool validTime(uint8_t hour, uint8_t minute);

};

#endif /* UTILITIES_H */