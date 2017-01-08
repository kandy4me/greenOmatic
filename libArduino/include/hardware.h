#ifndef GREENOMATIC_HARDWARE_H
#define GREENOMATIC_HARDWARE_H

#include "arduino_includes.h"


typedef enum arduino_pin
{
  //TODO
};


int hardware_init();

//TODO: Board-specific pins_arduino

#ifdef INTERFACE_SERIAL0
  void serial_start(unsigned int baud);
  void serial_stop();
  bool serial_running();
#endif
#ifdef INTERFACE_I2C
  // TODO: struct
  void  i2c_start();
  void  i2c_stop();
  bool i2c_running();
  void  i2c_send(unsigned int addr, char data);
  int  i2c_get(unsigned int addr);
  int  i2c_transaction(unsigned int addr, char data)
  {
    if (!i2c_send(addr, data))
      return i2c_get(addr);
  }
#endif
#ifdef INTERFACE_SPI
  // TODO: struct
  void spi_start();
  void spi_stop();
  bool spi_running();
  char spi_transaction(arduino_pin cs, char data);
#endif
//TODO: INTERFACE_NRF905
//TODO: INTERFACE_ASK_RX
//TODO: INTERFACE_ASK_TX

/// GPIO
// TODO: struct
bool get_gpio_state(arduino_pin pin);
void set_gpio_state(arduino_pin pin, bool state);

/// Analog
unsigned int get_analog_value(arudino_pin pin);

/// Timing & Timers
// TODO: struct


#endif  // GREENOMATIC_HARDWARE_H
