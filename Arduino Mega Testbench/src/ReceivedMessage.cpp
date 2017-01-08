/*
 * ReceivedMessage.cpp
 *
 *  Created on: 2016-02-21
 *      Author: jeff
 */

#include "../include/ReceivedMessage.h"
#include "../arduino/RadioHead/RH_ASK.h"
#include "../arduino/RadioHead/RH_NRF905.h"


ReceivedMessage::ReceivedMessage()
{
    // TODO Auto-generated constructor stub
}


ReceivedMessage::~ReceivedMessage()
{
    // TODO Auto-generated destructor stub
}



rfMessage::rfMessage() // Default Constructor
    : message{"\0"}
    , rxTime(currentUnixTime)
{
#ifdef DEBUG
    Serial.println("Creating a new rfMessage via DEFAULT constructor!");
#endif
}


rfMessage::rfMessage( char msg[] ) // Supply a message buffer
        : message{"\0"}
        , rxTime(currentUnixTime)
{
#ifdef DEBUG
    Serial.println("Creating a new rfMessage via BUFFER constructor!");
#endif
    // Fill in the message contents
    memcpy(message, msg, RH_NRF905_MAX_MESSAGE_LEN);
}


const uint64_t& rfMessage::GetRxTime()
{
    return rxTime;
}


