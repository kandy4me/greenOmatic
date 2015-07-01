/*
 * main.h
 *
 *  Created on: Feb 22, 2014
 *      Author: jeff
 */
#ifndef MAIN_H_
#define MAIN_H_

#include <stdint.h>


/// SYSTEM DEFINITION
#define SHIELD_WIZNET
// Peripheral Sensors & Actuators
#define PERIPHERAL_RTCC

// RF Communication Interfaces
#define INTERFACE_ASK_RX
//#define INTERFACE_NRF905

/// Ethernet Clients
// Define ethernet functionality seperate from other shield functionality
//#define ETHERNET_WEBSERVER
//#define ETHERNET_UDPCLIENT
#if defined(ETHERNET_WEBSERVER) || defined(ETHERNET_UDPCLIENT)
	#define INTERFACE_ETHERNET
#endif
// Static vs Dynamic IP, incurs a heavy memory cost
//#define ETHERNET_STATIC_IP
#define ETHERNET_DYNAMIC_IP


/// INCLUDES
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
	#include "arduino/RTClib/RTClib.h"
	#include "arduino/Time/Time.h" // TODO: DELETE
#endif


/// PIN & ADDRESS DEFINITIONS
#ifdef SHIELD_WIZNET
	// Ethernet Shield ( http://arduino.cc/en/Main/ArduinoEthernetShield )
	#define SPI_SEL_SD 			4  // SD Card Reader
	#define SPI_SEL_WIZNET 		10 // Ethernet NIC
	#define WIZNET_INTERRUPT 	2  // not supported by ethernet library; need to solder jumper
#endif

/// Communication Channels
#ifdef INTERFACE_NRF905
	#define SPI_SEL_NRF905      6  // TODO
#endif

// One-Wire RF
#if defined(INTERFACE_ASK_RX) || defined(INTERFACE_ASK_TX)
	#define RF_BAUD				2400
#endif

#ifdef INTERFACE_ASK_RX
	#define RF_RX_PIN			3
#endif

// Ethernet
#ifdef INTERFACE_ETHERNET
	uint8_t 	mac[]  = { 0xEE, 0x69, 0xF7, 0x55, 0x00, 0x01 };
	#ifdef ETHERNET_STATIC_IP
		IPAddress   IPaddr(128, 169, 0, 13);
	#endif
#endif


/// TUNING PARAMETERS
#define LOOP_DELAY 333 //ms


/// Function Definitions
int 	main			( void );
void 	setup			( void );
void 	CLI				( char *, const uint8_t);


/// Structures
struct _status_flags
{
	enum { INVALID = 0, VALID = 1, UNINSTALLED = 2, ERROR = 3} STATUS;
#ifdef PERIPHERAL_RTCC
	unsigned char time_valid     : 2;
#endif
#ifdef INTERFACE_ETHERNET
	unsigned char ethernet_valid : 2;
#endif

	void reset (void)
	{
#ifdef PERIPHERAL_RTCC
		//time_valid   = 0; // don't include RTC, as that is only determined once on start-up
#endif
#ifdef INTERFACE_ETHERNET
		// don't include ethernet, as that is only determined once on start-up
#endif
	}
};


/// Object Definitions
//volatile extern Plant plant_0();
//volatile extern Plant plant_1();


#endif /* MAIN_H_ */
