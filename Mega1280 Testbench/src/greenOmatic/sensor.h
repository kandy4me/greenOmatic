#ifndef SENSOR_H_
#define SENSOR_H_


/// DHT11 Digital Temperature Humidity Sensor Module
/// DHT21 Capacitive Digital Temperature & Humidity Sensor
#include "../arduino/DHT_lib/DHTlib.h"


/// DS18B20 Stainless Steel Temperature Sensor
// via http://arduino-info.wikispaces.com/Brick-Temperature-DS18B20


class sensor {
public:
	// types
	typedef enum { temperature, humidity, moisture, light, pH } sensor_types;
	typedef enum { discrete, PWM, analog, I2C, SPI, UART } 		interface_types;

	// control & status
	sensor ();
	sensor (unsigned int, sensor_types, interface_types, unsigned int);
	virtual ~sensor();

	unsigned int 	get_ID(void) 	{ return sensor_ID; }
	sensor_types 	get_type(void) 	{ return type; }
	interface_types get_interface() { return interface; }


private:
	unsigned int 	sensor_ID;
	sensor_types 	type;
	interface_types interface;
	unsigned int    pin;
	//unsigned int 	refresh_ms;

};

#endif /* SENSOR_H_ */
