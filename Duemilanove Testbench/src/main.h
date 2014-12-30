/*
 * main.h
 *
 *  Created on: Feb 22, 2014
 *      Author: jeff
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "arduino/Arduino.h"
#include "arduino/Wire/Wire.h"
#include "arduino/SPI/SPI.h"
#include "arduino/EEPROM/EEPROM.h"

#include "arduino/Ethernet/Ethernet.h"
#include "arduino/VirtualWire/VirtualWire.h"
#include "arduino/Time/Time.h"
#include "arduino/DS1307RTC/DS1307RTC.h"

#include "greenOmatic/greenOmatic_core.h"


//// TUNING PARAMETERS
#define LOOP_DELAY 1000 //ms


//// PIN & ADDRESS DEFINITIONS
/// I2C PERIPHERALS

/// SPI PERIPHERALS
// Slave-Select Lines
#define SPI_SEL_SD 			4  // SD Card Reader
#define SPI_SEL_WIZNET 		10 // Ethernet NIC
#define SPI_SEL_NRF905      6  // TODO

// Ethernet Shield ( http://arduino.cc/en/Main/ArduinoEthernetShield )
#define WIZNET_INTERRUPT 	2  // not supported by ethernet library; need to solder jumper

/// Communication Channels
// One-Wire RF
#define RF_BAUD				2400
#define RF_RX_PIN			3

// Ethernet
uint8_t 	mac[] 		= { 0xEE, 0x69, 0xF7, 0x55, 0x00, 0x01 };
#define 	localPort 	8888 // local port to listen for UDP packets


//// Function Definitions
int 	main			( void );
void 	setup			( void );
void 	CLI				( char *, uint8_t );
void 	print2digits	( int );  // used by DS1307 lib, TODO: get rid of ASAP



//// Object Definitions
Plant plant_0();
Plant plant_1();



//// Structures
typedef struct _status_flags
{
	char time_valid   : 1;
	char DHT11_valid  : 1;
	char BH1750_valid : 1;

	void reset (void)
	{
		time_valid = 0;
		DHT11_valid = 0;
		BH1750_valid = 0;
	}
};


#endif /* MAIN_H_ */
