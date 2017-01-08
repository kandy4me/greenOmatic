/*
 * main.h
 *
 *  Created on: Feb 22, 2014
 *      Author: jeff
 */
#ifndef MAIN_H_
#define MAIN_H_


/// INCLUDES
// green-O-matic Platform
#include "SystemConfig.h"

#include "Ethernets.h"
#include "GreenTime.h"
#include "RF.h"
#include "ReceivedMessage.h"

// System
#include "../arduino/Arduino.h"
#include "../arduino/EEPROM/EEPROM.h"
//Peripherals
#ifdef PERIPHERAL_RTCC
    #include "../arduino/Wire/Wire.h"
    #include "../arduino/RTClib/RTClib.h"
    #include "../arduino/Time/Time.h" // TODO: DELETE
#endif


/// TUNING PARAMETERS
#define LOOP_DELAY 1 //ms


/// Structures
struct _status_flags
{
    enum STATUS { INVALID = 0, VALID = 1, UNINSTALLED = 2, ERROR = 3};

    // Rather than litter this struct with #IFDEF's, instantiate a flag for ALL supported
    // peripherals, even if they're never going to be installed
    unsigned time_valid      : 2;
    unsigned ethernet_valid  : 2;
    unsigned udpclient_valid : 2;
    unsigned webserver_valid : 2;
    unsigned rfAskRx         : 2;
    unsigned rfAskTx         : 2;
    unsigned rfNrf905        : 2;

    _status_flags()
        : time_valid(UNINSTALLED)
        , ethernet_valid(UNINSTALLED)
        , udpclient_valid(UNINSTALLED)
        , webserver_valid(UNINSTALLED)
        , rfAskRx(UNINSTALLED)
        , rfAskTx(UNINSTALLED)
        , rfNrf905(UNINSTALLED)
    {}
};


/// Function Definitions
int     main            ( void );
void    setup           ( void );
void    CLI             ( char *, const uint8_t& );


#endif /* MAIN_H_ */
