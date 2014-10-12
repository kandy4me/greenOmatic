/*
 * main.h
 *
 *  Created on: Feb 22, 2014
 *      Author: jeff
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "arduino/Arduino.h"
#include "greenOmatic/greenOmatic_core.h"


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

// NRF905 RF


//// Function Definitions
int main(void);
void setup(void);

//// Object Definitions
Plant plant_0();
Plant plant_1();



#endif /* MAIN_H_ */
