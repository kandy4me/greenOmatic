/*
 * Ethernets.h
 *
 *  Created on: 2016-02-21
 *      Author: jeff
 */

#ifndef ETHERNETS_H_
#define ETHERNETS_H_


/// Includes
#include <stdint.h>
#include "SystemConfig.h"
#ifdef INTERFACE_ETHERNET
    #include "../arduino/SPI/SPI.h"
    #include "../arduino/Ethernet/Ethernet.h"
    #ifdef ETHERNET_UDPCLIENT
        #include "../arduino/Ethernet/EthernetUdp.h"
    #endif
#endif


/// Functions
bool     SetupEthernet   ( void );
bool     SetupUdpClient  ( void );
bool     SetupWebServer  ( void );
void     sendNTPpacket   (const uint8_t *address);
uint64_t getNTPtime();

/// Objects
#ifdef ETHERNET_UDPCLIENT
    extern EthernetUDP Udp;
#endif
#ifdef ETHERNET_WEBSERVER
    extern EthernetServer server;
#endif


#endif /* ETHERNETS_H_ */
