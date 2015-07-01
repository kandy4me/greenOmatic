/*
 * main.h
 *
 *  Created on: Feb 22, 2014
 *      Author: jeff
 */

#ifndef MAIN_H_
#define MAIN_H_

/// C++ Includes
#include <stdio.h>
#include <avr/io.h>

#include "arduino/Arduino.h"
#include "arduino/Wire/Wire.h"
#include "arduino/RadioHead/RH_ASK.h"
#include "arduino/RadioHead/RH_NRF905.h"
#include "arduino/RadioHead/RHReliableDatagram.h"

#include "greenOmatic/greenOmatic_core.h"


/// SYSTEM DEFINITION
#define PERIPHERAL_BH1750
#define PERIPHERAL_DHT11
#define PERIPHERAL_RELAY
//#define INTERFACE_NRF905
#define INTERFACE_ASK_TX

//// TUNING PARAMETERS
#define LOOP_DELAY 		5000U //ms
#define LOW_MOISTURE 	180U


//// PINS & ADDRESSES
///Analog
#define Soil_Moisture_0_Pin A7

/// I2C PERIPHERALS
// RTC needs no extra pins...
#ifdef PERIPHERAL_BH1750
#define BH1750_ADDR			0x23
#endif

/// SPI PERIPHERALS
// NRF905
#ifdef INTERFACE_NRF905
#define SPI_SEL_NRF905		6U
extern RH_NRF905 RF_Transceiver;
#endif

/// 1-wire Digital Interfaces
// ASK RF, see RH_ASK.h for more
#ifdef INTERFACE_ASK_TX
extern RH_ASK RF_Transmitter;
#define ASK_RX_PIN 	0U
#define ASK_TX_PIN 	7U
#define ASK_BAUD 	2400U
#endif

// DHT Sensors
#ifdef PERIPHERAL_DHT11
extern dht 	DHT;
#define DHT11_PIN A15
//#define DHT21_PIN 49
#endif

//// ACTUATORS
#ifdef PERIPHERAL_RELAY
#define Relay0_Pin 30U
#endif


//// STRUCTURES
struct _status_flags
{
#ifdef RTC
	unsigned char time_valid   : 1;
#endif
#ifdef PERIPHERAL_DHT11
	unsigned char DHT11_valid  : 1;
#endif
#ifdef PERIPHERAL_BH1750
	unsigned char BH1750_valid : 1;
#endif
#ifdef INTERFACE_NRF905
	unsigned char NRF905_valid : 1;
#endif

	void reset (void)
	{
#ifdef RTC
		time_valid = 0;
#endif
#ifdef PERIPHERAL_DHT11
		DHT11_valid = 0;
#endif
#ifdef PERIPHERAL_BH1750
		BH1750_valid = 0;
#endif
	}
};


//// FUNCTIONS
int 	 main	(void);
void 	 Setup	(void);
/// Interface
#ifdef INTERFACE_ASK_TX
int 	 ASK_Send    	(char *msg, unsigned int length);
int 	 ASK_Send    	(String msg);
#endif
#ifdef INTERFACE_NRF905
int 	 NRF_Send    	(String msg);
#endif

/// Perception
int 	 Dht11Update 	(int pin);
uint16_t Bht1750Read  	(int pin);

/// Actuation
void 		RelayCycleOn 	(int pin, uint16_t cycle_time_ms);


//// OBJECTS
extern volatile Plant plant_0();
extern volatile Plant plant_1();


//// MACROS
#define Relay_ON(x) 	digitalWrite(x, HIGH)
#define Relay_OFF(x) 	digitalWrite(x, LOW)


#endif /* MAIN_H_ */
