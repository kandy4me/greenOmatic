#ifndef ARDUINO_CONFIG_H_
#define ARDUINO_CONFIG_H_


/// Arduino Shields
#define SHIELD_WIZNET

/// Peripheral Sensors
#define PERIPHERAL_RTCC
#define PERIPHERAL_BH1750
#define PERIPHERAL_DHT11

/// Peripheral Actuators
/// TODO

/// Comm Interfaces
#define INTERFACE_SERIAL
#define INTERFACE_ASK_RX
#define INTERFACE_ASK_TX
#define INTERFACE_NRF905

/// Ethernet Config (?)
#define INTERFACE_ETHERNET // seperate from other shield functionality
// Static vs Dynamic IP, incurs a heavy memory cost
//#define ETHERNET_STATIC_IP
#define ETHERNET_DYNAMIC_IP


/// Ethernet Clients
// Define ethernet functionality seperate from other shield functionality
//#define ETHERNET_WEBSERVER
//#define ETHERNET_UDPCLIENT
#if defined(ETHERNET_WEBSERVER) || defined(ETHERNET_UDPCLIENT)
	#define INTERFACE_ETHERNET
#endif


/// Communication Channels
/// PIN & ADDRESS DEFINITIONS
#ifdef SHIELD_WIZNET
	// Ethernet Shield ( http://arduino.cc/en/Main/ArduinoEthernetShield )
	#define SPI_SEL_SD 			  4  // SD Card Reader
	#define SPI_SEL_WIZNET 		10 // Ethernet NIC
	#define WIZNET_INTERRUPT 	2  // not supported by ethernet library; need to solder jumper
#endif

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
		#define 	localPort 	8888 // local port to listen for UDP packets
#endif


/// TUNING PARAMETERS
#define LOOP_DELAY 333 //ms


#endif
