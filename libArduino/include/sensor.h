#ifndef GREENOMATIC_SENSOR_H_
#define GREENOMATIC_SENSOR_H_

#include "arduino_includes.h"
#include "hardware.h"

class sensor {
public:
  // types
  typedef enum { temperature, humidity, moisture, light, pH } sensor_types;
  typedef enum { discrete, PWM, analog, I2C, SPI, UART } interface_types;

  // control & status
  sensor ();
  sensor (unsigned int, sensor_types, interface_types, unsigned int);
  virtual ~sensor();

  unsigned int    get_ID(void) 	{ return sensor_ID; }
  sensor_types    get_type(void) 	{ return type; }
  interface_types get_interface() { return interface; }

private:
  byte            sensor_ID;
  unsigned int    pin;
  interface_types interface;
  sensor_types    type;
  unsigned int    refresh_ms;
};


#endif /* GREENOMATIC_SENSOR_H_ */
