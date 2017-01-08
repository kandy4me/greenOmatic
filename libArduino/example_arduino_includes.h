#ifndef ARDUINO_INCLUDES_H_
#define ARDUINO_INCLUDES_H_


// C++
#include <stdint.h>


#include "arduino/Arduino.h"
// Interfaces
#include "arduino/Wire/Wire.h"
#include "arduino/SPI/SPI.h"
#include "arduino/EEPROM/EEPROM.h"
#ifdef INTERFACE_ETHERNET
	#include "arduino/Ethernet/Ethernet.h"
	#ifdef ETHERNET_UDPCLIENT
		#include "arduino/Ethernet/EthernetUdp.h"
	#endif
#endif
#if defined(INTERFACE_ASK_RX) || defined(INTERFACE_ASK_TX)
	#include "arduino/VirtualWire/VirtualWire.h"
	//#include "arduino/RadioHead/RadioHead.h"
	//#include "arduino/RadioHead/RH_ASK.h"
#endif
#ifdef INTERFACE_NRF905
	#include "arduino/RadioHead/RadioHead.h"
	#include "arduino/RadioHead/RH_NRF905.h"
#endif
//Peripherals
#ifdef PERIPHERAL_RTCC
	#include "arduino/Time/Time.h"
	#include "arduino/DS1307RTC/DS1307RTC.h"
#endif

#endif
