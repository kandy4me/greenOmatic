/*
 * main.h
 *
 *  Created on: Feb 22, 2014
 *      Author: jeff
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "arduino/Arduino.h"
#include "arduino/VirtualWire/VirtualWire.h"

#include "greenOmatic/greenOmatic_core.h"

//// TUNING PARAMETERS
#define LOOP_DELAY 3000 //ms


//// PIN DEFINITIONS
/// Serial (RS232)
#define UART_RX 0
#define UART_TX 1

/// I2C
#define I2C_SDA 0
#define I2C_SCL 0

/// I2C PERIPHERALS

/// SPI
#define SPI_MOSI 11
#define SPI_MISO 12
#define SPI_SCK  13

/// SPI PERIPHERALS
// Ethernet Shield ( http://arduino.cc/en/Main/ArduinoEthernetShield )
#define WIZNET_INTERRUPT 2// not supported by ethernet library; need to solder jumper
#define SPI_SEL_SD 4
#define SPI_SEL_WIZNET 10

// RF
#define RF_BAUD		2000
#define RF_RX_PIN	11


// DHT Sensors
extern dht 	DHT;
//#define DHT11_PIN 16
//#define DHT21_PIN 49


//// ACTUATORS
#define Relay0_Pin 7



//// Function Definitions
int 	main	(void);
void 	setup	(void);


//// Object Definitions
Plant plant_0();
Plant plant_1();


#endif /* MAIN_H_ */
