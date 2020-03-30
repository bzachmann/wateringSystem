#include "utilities.h"

bool Utilities::validDate(uint8_t month, uint8_t day, uint16_t year)
{
    if ((year > 9999) || (year < 2000))
    {
        return false;
    }
        
    if ((month < 1) || (month > 12))
    {
        return false;
    }
        
    if ((day < 1) || (day > 31))
    {
        return false;
    }

    if (month == 2) //february
    {
        if(leapYear(year))
        {
            return (day <= 29);
        }
        else
        {
            return (day <= 28);
        }
    }

    if (   (month == 4)   //april
        || (month == 6)   //june
        || (month == 9)   //september
        || (month == 11)) //november
    {
        return (day <= 30);
    }

    return true;
}

bool Utilities::leapYear(uint16_t year)
{
    // Return true if:
    // year is a multiple of 4 and not a multiple of 100 
    // OR 
    // year is multiple of 400
    return ((((year % 4) == 0) && ((year % 100) != 0)) 
            || ((year % 400) == 0));
}

bool Utilities::validTime(uint8_t hour, uint8_t minute)
{
    return ((hour < 24) && (minute < 60));
}