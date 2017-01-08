/*
 * GreenTime.h
 *
 *  Created on: 2016-02-21
 *      Author: jeff
 */

#ifndef GREENTIME_H_
#define GREENTIME_H_

#include <stdint.h>
#include "Ethernets.h"

class GreenTime
{
    public:
        GreenTime();
        virtual ~GreenTime();
};

// Current (most-recently updated) time in Unix Epoch
extern uint64_t currentUnixTime;

void updateCurrentUnixTime();


#endif /* GREENTIME_H_ */
