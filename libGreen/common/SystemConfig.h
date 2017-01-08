/*
 * SystemConfig.h
 *
 *  Created on: 2016-02-21
 *      Author: jeff
 */

#ifndef SYSTEMCONFIG_H_
#define SYSTEMCONFIG_H_


/// SYSTEM DEFINITION
#define SHIELD_WIZNET
// Peripheral Sensors & Actuators
//#define PERIPHERAL_RTCC
// RF Communication Interfaces
#define INTERFACE_ASK_RX
#define INTERFACE_NRF905
/// Ethernet Clients
// Define ethernet functionality separate from other shield functionality
#define ETHERNET_WEBSERVER
#define ETHERNET_UDPCLIENT
#if defined(ETHERNET_WEBSERVER) || defined(ETHERNET_UDPCLIENT)
    #define INTERFACE_ETHERNET
#endif
// Static vs Dynamic IP, incurs a heavy memory cost
//#define ETHERNET_STATIC_IP
#define ETHERNET_DYNAMIC_IP


/// PIN & ADDRESS DEFINITIONS
// Standard Mega 2560 SPI pinout
#define SPI_MOSI 51
#define SPI_MISO 50
#define SPI_SCK  52
#define SPI_SS   53
// Ethernet Shield ( http://arduino.cc/en/Main/ArduinoEtherne tShield )
#ifdef SHIELD_WIZNET
    #define SPI_SEL_SD          4  // SD Card Reader
    #define SPI_SEL_WIZNET      10 // Ethernet NIC
    #define WIZNET_INTERRUPT    2  // not supported by ethernet library; need to solder jumper
#endif
/// Communication Channels
#ifdef INTERFACE_NRF905
    #define NRF905_TxEN         31
    #define NRF905_CE           39
    #define NRF905_PWR          35
    #define NRF905_uCK          0       // Not Used
    #define NRF905_CD           26
    #define NRF905_AM           43
    #define NRF905_DR           27
    #define NRF905_MISO         SPI_MISO
    #define NRF905_MOSI         SPI_MOSI
    #define NRF905_SCK          SPI_SCK
    #define NRF905_CSN          SPI_SS
#endif
// One-Wire RF
#if defined(INTERFACE_ASK_RX) || defined(INTERFACE_ASK_TX)
    #define RF_BAUD             2400
#endif
#ifdef INTERFACE_ASK_RX
    #define RF_RX_PIN           36
#endif


#endif /* SYSTEMCONFIG_H_ */
