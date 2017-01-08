/*
 * GreenTime.cpp
 *
 *  Created on: 2016-02-21
 *      Author: jeff
 */

#include "GreenTime.h"
// C++ libraries
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>


// Time
uint64_t currentUnixTime = 0;
char currentTime[21] = {"\0"};

GreenTime::GreenTime()
{
    // TODO Auto-generated constructor stub

}

GreenTime::~GreenTime()
{
    // TODO Auto-generated destructor stub
}


/*
void GetDatetimeString(const tmElements_t &dt, char *stringBuffer = currentTime)
{
    sprintf(stringBuffer, "%04d-%02d-%02d %02d:%02d:%02d", dt.Year, dt.Month, dt.Day, dt.Hour, dt.Minute, dt.Second);
}


void GetDatetimeString(const DateTime &dt, char *stringBuffer = currentTime)
{
    sprintf(stringBuffer, "%04d-%02d-%02d %02d:%02d:%02d", dt.year(), dt.month(), dt.day(), dt.hour(), dt.minute(), dt.second());
}



// Date-Time Printing
void PrintZeroPadded(const uint8_t &number)
{
    if ( (0 <= number) && (10 > number) )
    {
        Serial.write('0');
    }
    Serial.print(number, DEC);
}


void PrintDateTime(const tmElements_t &dt)
{
    char datestr[21];
    sprintf(datestr, "%04d-%02d-%02d %02d:%02d:%02d", dt.Year, dt.Month, dt.Day, dt.Hour, dt.Minute, dt.Second);
    Serial.write(datestr);
}


void PrintDateTime(const DateTime &dt)
{
    char datestr[21];
    sprintf(datestr, "%04d-%02d-%02d %02d:%02d:%02d", dt.year(), dt.month(), dt.day(), dt.hour(), dt.minute(), dt.second());
    Serial.write(datestr);

    //Serial.print(dt.year(), DEC);
    //Serial.write('-');
    //PrintZeroPadded(dt.month());
    //Serial.write('-');
    //PrintZeroPadded(dt.day());
    //Serial.write(' ');
    //PrintZeroPadded(dt.hour());
    //Serial.write(':');
    //PrintZeroPadded(dt.minute());
    //Serial.write(':');
    //PrintZeroPadded(dt.second());
}
*/


void updateCurrentUnixTime()
{
    currentUnixTime = getNTPtime();
}

