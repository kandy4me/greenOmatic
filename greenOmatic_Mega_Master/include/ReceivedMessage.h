/*
 * ReceivedMessage.h
 *
 *  Created on: 2016-02-21
 *      Author: jeff
 */

#ifndef RECEIVEDMESSAGE_H_
#define RECEIVEDMESSAGE_H_

#include "SystemConfig.h"
#include "GreenTime.h"
// C++ libraries
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>


/// Structures
class ReceivedMessage
{
    public:
        ReceivedMessage();
        virtual ~ReceivedMessage();
};

struct rfMessage
{
    // Message
    char message[256];

    // Default Constructor
    rfMessage();
    // Supply a message buffer
    rfMessage( char msg[] );
    // Get this message's timestamp
    const uint64_t& GetRxTime();

    private:
        uint64_t rxTime;
};
#define MAX_MESSAGE_QUEUE 8

struct rfMessageCircleBuffer
{
    // Default constructor
    rfMessageCircleBuffer()
        : message()
        , latest(0)
    {}

    // Take a raw byte-stream, time-stamp it, and place in the circle-buffer
    size_t Parse ( char buff[] )
    {
        // Step 1: Copy data and apply current system time
        rfMessage parsedMessage(buff);

        // Step 2: place in the circle-buffer
        ++latest;
        if ( MAX_MESSAGE_QUEUE == latest )
        {
            latest = 0;
        }
        message[latest] = parsedMessage;

        // return the index of the placed message
        return latest;
    }

    const rfMessage& GetLatestMessage()
    {
        return message[latest];
    }

    const size_t& GetLatestIndex()
    {
        return latest;
    }

    private:
        rfMessage message[MAX_MESSAGE_QUEUE];
        size_t    latest;
};


/// Functions
// Setup
bool    SetupAskRf      ( void );
bool    SetupNrf905     ( void );
// Wireless Receive
bool    RxAskMessage    ( void );
bool    RxNrfMessage    ( void );
// Wireless Transmit
bool    RxNrfMessage    ( unsigned char *mem, size_t length );


/// Objects
#ifdef INTERFACE_NRF905
    extern rfMessageCircleBuffer nrf905Messages;
#endif
#ifdef INTERFACE_ASK_RX
    extern rfMessageCircleBuffer askMessages;
#endif
#endif /* RECEIVEDMESSAGE_H_ */
