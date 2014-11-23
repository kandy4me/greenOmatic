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
#define LOOP_DELAY 5000 //ms


//// PIN DEFINITIONS
///Analog
#define Soil_Moisture_0_Pin A7


/// I2C PERIPHERALS
// RTC needs no extra pins...


/// SPI PERIPHERALS
// Ethernet Shield ( http://arduino.cc/en/Main/ArduinoEthernetShield )
#define WIZNET_INTERRUPT 	2 // not supported by ethernet library; need to solder jumper
#define SPI_SEL_SD 			4
#define SPI_SEL_NRF905		6
#define SPI_SEL_WIZNET 		10


// RF, see RH_ASK.h for more
extern RH_ASK 	RF_Transmitter;
#define ASK_RX_PIN 	0
#define ASK_TX_PIN 	25
#define ASK_BAUD 	2400


// DHT Sensors
extern dht 	DHT;
#define DHT11_PIN 49
//#define DHT21_PIN 49


//// ACTUATORS
#define Relay0_Pin 30



//// Function Definitions
int 	main	(void);
void 	setup	(void);



//// Object Definitions
Plant plant_0();
Plant plant_1();


#endif /* MAIN_H_ */
