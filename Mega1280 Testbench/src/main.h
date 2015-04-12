/*
 * main.h
 *
 *  Created on: Feb 22, 2014
 *      Author: jeff
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "arduino/Arduino.h"

#include "arduino/RadioHead/RH_ASK.h"
#include "arduino/RadioHead/RHReliableDatagram.h"
#include "arduino/RadioHead/RH_NRF905.h"
#include "arduino/Wire/Wire.h"

#include "greenOmatic/greenOmatic_core.h"


//// TUNING PARAMETERS
#define LOOP_DELAY 		5000 //ms
#define LOW_MOISTURE 	180


//// PINS & ADDRESSES
///Analog
#define Soil_Moisture_0_Pin A7

/// I2C PERIPHERALS
// RTC needs no extra pins...
#define BH1750_ADDR			0x23

/// SPI PERIPHERALS
// NRF905
#define SPI_SEL_NRF905		6



extern RH_NRF905 RF_Transceiver;

/// 1-wire Digital Interfaces
// ASK RF, see RH_ASK.h for more
extern RH_ASK 	RF_Transmitter;
#define ASK_RX_PIN 	0
#define ASK_TX_PIN 	25
#define ASK_BAUD 	2400

// DHT Sensors
extern dht 	DHT;
#define DHT11_PIN A15
//#define DHT21_PIN 49

//// ACTUATORS
#define Relay0_Pin 30


//// STRUCTURES
typedef struct _status_flags
{
#ifdef RTC
	char time_valid   : 1;
#endif
	char DHT11_valid  : 1;
	char BH1750_valid : 1;
	char NRF905_valid : 1;
	void reset (void)
	{
#ifdef RTC
		time_valid = 0;
#endif
		DHT11_valid = 0;
		BH1750_valid = 0;
	}
};


//// FUNCTIONS
int 	main	(void);
void 	setup	(void);

/// Perception
//int    	Time_read		(void);
int 		DHT11_update	(int);
uint16_t 	Light_read  	(int);
/// Actuation
void 		Relay_Cycle_ON 	(int, uint16_t);
/// Interface
int 		ASK_Send    	(String);
int 		NRF_Send    	(String);



//// OBJECTS
// All That's necessary is a String object for broadcast,
// just because the "macros" below need them.  TODO
extern String Broadcast_Message;
extern volatile Plant plant_0();
extern volatile Plant plant_1();


//// MACROS
// broadcast_update is fine as a macro for now
#define broadcast_update(x)		\
{ 									\
    Broadcast_Message.concat(x); 	\
    Broadcast_Message.concat("\n"); \
	Serial.println(x); 			\
}

#define broadcast_Transmit(x)		ASK_Send (x); 	NRF_Send (x);
#define Relay_ON(x) 				digitalWrite(x, HIGH);
#define Relay_OFF(x) 				digitalWrite(x, LOW);



#endif /* MAIN_H_ */
