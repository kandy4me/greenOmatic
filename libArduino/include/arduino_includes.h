#ifndef ARDUINO_INCLUDES_H_
#define ARDUINO_INCLUDES_H_


/// Determine what interfaces and peripherals are being used,
///  and create convenience #define's
// Arduino Shields
#ifdef SHIELD_WIZNET
  #define INTERFACE_ETHERNET
  #define PIN_SPISEL_SD      4  // SD Card Reader
  #define PIN_SPISEL_WIZNET  10 // Ethernet NIC
  #define PIN_INT_WIZNET     2  // not supported by ethernet library; need to solder jumper
#endif
//TODO: Ethernet Shield ( http://arduino.cc/en/Main/ArduinoEthernetShield )

// Peripherals
#if defined(I2C_ADDR_BH1750)
  #define PERIPHERAL_BH1750
#endif
#ifdef I2C_ADDR_RTC
  #define PERIPHERAL_RTC
#endif
#ifdef PIN_DHT11
  #define PERIPHERAL_DHT11
#endif
#ifdef PIN_DHT22
  #define Peripheral_DHT22
#endif

// Communication Interfaces
#ifdef BAUD_SERIAL_TERMINAL
  #define INTERFACE_SERIAL
#endif
#ifdef PIN_ASK_RX
  #define INTERFACE_ASK_RX
#endif
#ifdef PIN_ASK_TX
  #define INTERFACE_ASK_TX
#endif
#ifdef SPI_SEL_NRF905
  #define INTERFACE_SPI
  #define INTERFACE_NRF905
#endif
#if defined(PERIPHERAL_RTC) || if defined(PERIPHERAL_BH1750)
  #define INTERFACE_I2C
#endif
// Ethernet
#if defined(ETHERNET_WEBSERVER) || if defined(ETHERNET_UDPCLIENT)
  #define INTERFACE_ETHERNET
#endif



/// Includes
// C++
#include <stdint.h>

// Hardware
#include "arduino/Arduino.h"
#include "arduino/EEPROM/EEPROM.h"

// Communication Interfaces
#ifdef INTERFACE_I2C
  #include "arduino/Wire/Wire.h"
#endif
#ifdef INTERFACE_SPI
  #include "arduino/SPI/SPI.h"
#endif
#if defined(INTERFACE_ASK_RX) || defined(INTERFACE_ASK_TX)
  #include "external/VirtualWire/VirtualWire.h"
  //#include "external/RadioHead/RadioHead.h"
  //#include "external/RadioHead/RH_ASK.h"
#endif
#ifdef INTERFACE_NRF905
  #include "external/RadioHead/RadioHead.h"
  #include "external/RadioHead/RH_NRF905.h"
#endif

// Ethernet
#ifdef INTERFACE_ETHERNET
  #include "arduino/Ethernet/Ethernet.h"
  #ifdef ETHERNET_UDPCLIENT
    #include "arduino/Ethernet/EthernetUdp.h"
  #endif
#endif

// Sensors
#ifdef PERIPHERAL_BH1750
  // TODO
#endif
#ifdef PERIPHERAL_RTC
  #include "arduino/Time/Time.h"
  #include "external/DS1307RTC/DS1307RTC.h"
#endif
#ifdef defined(PERIPHERAL_DHT11) || if defined(PERIPHERAL_DHT22)
// DHT11 Digital Temperature Humidity Sensor Module
// DHT21 Capacitive Digital Temperature & Humidity Sensor
  #include "external/DHT_lib/DHTlib.h"
#endif
//TODO: DS18B20 Stainless Steel Temperature Sensor
// via http://arduino-info.wikispaces.com/Brick-Temperature-DS18B20

#endif
