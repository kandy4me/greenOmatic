/*
 * sensor.cpp
 *
 *  Created on: 2013-02-13
 *      Author: Jeff
 */

#include "sensor.h"


sensor::sensor ()
{
	sensor_ID = 0;
	pin = 0;
}

sensor::sensor(	unsigned int 	_ID,
				sensor_types 	_type,
				interface_types _interface,
				unsigned int    _pin)
{
	sensor_ID 	= _ID;
	type 		= _type;
	interface 	= _interface;
	pin 		= _pin;
}

sensor::~sensor() {
	// TODO Auto-generated destructor stub
}

