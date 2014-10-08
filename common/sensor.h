#ifndef SENSOR_H_
#define SENSOR_H_

class sensor {
public:
	// types
	typedef enum { temperature, humidity, moisture, light, pH } sensor_types;
	typedef enum { discrete, PWM, analog, I2C, SPI, UART } 		interface_types;

	// control & status
	sensor();
	virtual ~sensor();
	unsigned int 	get_ID(void) 	{ return sensor_ID; }
	sensor_types 	get_type(void) 	{ return type; }
	interface_types get_interface() { return interface; }


private:
	unsigned int 	sensor_ID;
	sensor_types 	type;
	interface_types interface;
	unsigned int    pin;
	unsigned int 	refresh_ms;

};

#endif /* SENSOR_H_ */
