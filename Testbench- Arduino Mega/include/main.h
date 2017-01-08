/*
 * main.h
 *
 *  Created on: Feb 22, 2014
 *      Author: jeff
 */

#ifndef MAIN_H_
#define MAIN_H_


/// SYSTEM DEFINITION
//#define SHIELD_WIZNET
#define PERIPHERAL_BH1750
#define PERIPHERAL_DHT11
//#define PERIPHERAL_RTC
//#define PERIPHERAL_RELAY
#define INTERFACE_NRF905
#define INTERFACE_ASK_RX
//#define INTERFACE_ASK_TX
    // Ethernet Shield ( http://arduino.cc/en/Main/ArduinoEtherne tShield )
#ifdef SHIELD_WIZNET
	#define SPI_SEL_SD 			4  // SD Card Reader
	#define SPI_SEL_WIZNET 		10 // Ethernet NIC
	#define WIZNET_INTERRUPT 	2  // not supported by ethernet library; need to solder jumper
#endif
#define RF_BAUD				2400
#endif

#ifdef INTERFACE_ASK_RX
	#define RF_RX_PIN			36
	extern RH_ASK ASK_Receiver;




/// INCLUDES
// C++ Includes
#include <stdio.h>
#include <stdint.h>
// Arduino
#include <avr/io.h>
#include "arduino/Arduino.h"
#include "arduino/Wire/Wire.h"
#if defined(INTERFACE_ASK_RX) || defined(INTERFACE_ASK_TX)
	#include "arduino/RadioHead/RH_ASK.h"
	#include "arduino/RadioHead/RH_NRF905.h"
	#include "arduino/RadioHead/RHReliableDatagram.h"
#ENDIF
#ifdef INTERFACE_NRF905
	#include "arduino/SPI/SPI.h"
	#include "arduino/RadioHead/RadioHead.h"
	#include "arduino/RadioHead/RH_NRF905.h"
#endif
#include "arduino/EEPROM/EEPROM.h"
#ifdef INTERFACE_ETHERNET
	#include "arduino/SPI/SPI.h"
	#include "arduino/Ethernet/Ethernet.h"
	
	#ifdef ETHERNET_UDPCLIENT
		#include "arduino/Ethernet/EthernetUdp.h"
	#endif
#endif

#include "greenOmatic/greenOmatic_core.h"
//Peripherals
#ifdef PERIPHERAL_RTCC
	#include "arduino/Wire/Wire.h"
	#include "arduino/RTClib/RTClib.h"
	#include "arduino/Time/Time.h" // TODO: DELETE
#endif

    #define NRF905_TxEN 31
    #define NRF905_CSN  SPI_SS
    extern RH_NRF905 NRF_905;
#endif



//// TUNING PARAMETERS
#define LOOP_DELAY 		5000U //ms
#define LOW_MOISTURE 	180U

//// PINS & ADDRESSES
///Analog
#define Soil_Moisture_0_Pin A7

/// Ethernet Clients
// Define ethernet functionality seperate from other shield functionality
#define ETHERNET_WEBSERVER
//#define ETHERNET_UDPCLIENT
#if defined(ETHERNET_WEBSERVER) || defined(ETHERNET_UDPCLIENT)
	#define INTERFACE_ETHERNET
#endif
// Static vs Dynamic IP, incurs a heavy memory cost
//#define ETHERNET_STATIC_IP
#define ETHERNET_DYNAMIC_IP

/// I2C PERIPHERALS
// RTC needs no extra pins...
#ifdef PERIPHERAL_BH1750
#define BH1750_ADDR			0x23
#endif

/// SPI PERIPHERALS
// NRF905
#ifdef INTERFACE_NRF905
#define SPI_SEL_NRF905		6U
    #define NRF905_CE   39
    #define NRF905_PWR  35
    #define NRF905_uCK  0       // Not Used
    #define NRF905_CD   26
    #define NRF905_AM   43
    #define NRF905_DR   27
    #define NRF905_MISO SPI_MISO
    #define NRF905_MOSI SPI_MOSI
    #define NRF905_SCK  SPI_SCK

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
	enum { INVALID = 0, VALID = 1, UNINSTALLED = 2, ERROR = 3} STATUS;

#ifdef RTC
	unsigned char time_valid   : 2;
#endif
#ifdef PERIPHERAL_DHT11
	unsigned char DHT11_valid  : 2;
#endif
#ifdef PERIPHERAL_BH1750
	unsigned char BH1750_valid : 2;
#endif
#ifdef INTERFACE_NRF905
	unsigned char NRF905_valid : 2;
#endif
#ifdef INTERFACE_ETHERNET
	unsigned char ethernet_valid : 2;
#endif
	void reset (void)
	{
#ifdef PERIPHERAL_RTC
	time_valid   = 0; // don't include RTC, as that is only determined once on start-up
#endif
#ifdef PERIPHERAL_DHT11
		DHT11_valid = 0;
#endif
#ifdef PERIPHERAL_BH1750
		BH1750_valid = 0;
#endif
#ifdef INTERFACE_ETHERNET
		// don't include ethernet, as that is only determined once on start-up
#endif

/// TUNING PARAMETERS
#define LOOP_DELAY 1 //ms

//// FUNCTIONS
int 	 main	(void);
void 	 Setup	(void);
void 	 CLI	( char *, const uint8_t& );
/// Interfaces
#ifdef INTERFACE_ASK_TX
int 	 ASK_Send    	(char *msg, unsigned int length);
int 	 ASK_Send    	(String msg);
#endif
#ifdef INTERFACE_NRF905
int 	 NRF_Send    	(String msg);
#endif
// Ethernet
#ifdef INTERFACE_ETHERNET
	uint8_t 	mac[]  = { 0xEE, 0x69, 0xF7, 0x55, 0x00, 0x01 };
	#ifdef ETHERNET_STATIC_IP
		IPAddress   IPaddr(128, 169, 0, 13);
	#endif
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
