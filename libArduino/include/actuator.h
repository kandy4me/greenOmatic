#ifndef GREENOMATIC_ACTUATOR_H_
#define GREENOMATIC_ACTUATOR_H_

#include "arduino_includes.h"
#include "hardware.h"


class actuator {
public:
  // types
  typedef enum { boolean, variable } actuator_types;
  typedef enum { discrete, PWM, analog, I2C, SPI, UART } interface_types;

  // control & status
  actuator();
  virtual ~actuator();
  unsigned int    get_ID(void)    { return actuator_ID; }
  actuator_types  get_type(void)  { return type; }
  interface_types get_interface() { return interface; }


private:
  unsigned int    actuator_ID;
  actuator_types  type;
  interface_types interface;
};


#endif /* GREENOMATIC_ACTUATOR_H_ */
