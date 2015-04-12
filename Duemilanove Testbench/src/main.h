/*
 * main.h
 *
 *  Created on: Feb 22, 2014
 *      Author: jeff
 */
#ifndef MAIN_H_
#define MAIN_H_


/// SYSTEM DEFINITION
#define SHIELD_WIZNET
#define PERIPHERAL_RTCC
//#define PERIPHERAL_BH1750
//#define PERIPHERAL_DHT11
//#define INTERFACE_ETHERNET // seperate from other shield functionality
//#define INTERFACE_NRF905
//#define INTERFACE_ASK_TX
#define INTERFACE_ASK_RX


/// PIN & ADDRESS DEFINITIONS
#ifdef SHIELD_WIZNET
	// Ethernet Shield ( http://arduino.cc/en/Main/ArduinoEthernetShield )
	#define SPI_SEL_SD 			4  // SD Card Reader
	#define SPI_SEL_WIZNET 		10 // Ethernet NIC
	#define WIZNET_INTERRUPT 	2  // not supported by ethernet library; need to solder jumper
#endif
// Communication Channels
#ifdef INTERFACE_NRF905
	#define SPI_SEL_NRF905      6  // TODO
#endif
// One-Wire RF
#ifdef INTERFACE_ASK_RX or INTERFACE_ASK_TX
	#define RF_BAUD				2400
#endif
#ifdef INTERFACE_ASK_RX
	#define RF_RX_PIN			3
#endif
// Ethernet
#ifdef INTERFACE_ETHERNET
	uint8_t 	mac[] 		= { 0xEE, 0x69, 0xF7, 0x55, 0x00, 0x01 };
	#define 	localPort 	8888 // local port to listen for UDP packets
#endif


/// INCLUDES
#include "arduino/Arduino.h"
//#include "greenOmatic/greenOmatic_core.h"
// Interfaces
#include "arduino/Wire/Wire.h"
#include "arduino/SPI/SPI.h"
#include "arduino/EEPROM/EEPROM.h"
#ifdef INTERFACE_ETHERNET
	#include "arduino/Ethernet/Ethernet.h"
#endif
#ifdef INTERFACE_ASK_RX or INTERFACE_ASK_TX
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
	#include "arduino/Time/Time.h"
	#include "arduino/DS1307RTC/DS1307RTC.h"
#endif


/// Function Definitions
int 	main			( void );
void 	setup			( void );
void 	CLI				( char *, uint8_t );
#ifdef PERIPHERAL_RTCC
	void 	print2digits	( int );  // used by DS1307 lib, TODO: get rid of ASAP
#endif


/// TUNING PARAMETERS
#define LOOP_DELAY 1000 //ms


/// Structures
typedef struct _status_flags
{
	enum { INVALID = 0, VALID = 1, UNINSTALLED = 2, ERROR = 3} STATUS;
#ifdef PERIPHERAL_RTCC
	unsigned char time_valid     : 2;
#endif
#ifdef PERIPHERAL_DHT11
	unsigned char DHT11_valid    : 2;
#endif
#ifdef PERIPHERAL_BH1750
	unsigned char BH1750_valid   : 2;
#endif
#ifdef INTERFACE_ETHERNET
	unsigned char ethernet_valid : 2;
#endif

	void reset (void)
	{
#ifdef PERIPHERAL_RTCC
		//time_valid   = 0; // don't include RTC, as that is only determined once on start-up
#endif
#ifdef PERIPHERAL_DHT11
		DHT11_valid  = 0;
#endif
#ifdef PERIPHERAL_BH1750
		BH1750_valid = 0;
#endif
		// don't include ethernet, as that is only determined once on start-up
	}
};


/// Object Definitions
//volatile extern Plant plant_0();
//volatile extern Plant plant_1();


#endif /* MAIN_H_ */
