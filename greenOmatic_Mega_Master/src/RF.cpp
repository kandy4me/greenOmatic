/*
 * RF.cpp
 *
 *  Created on: 2016-02-21
 *      Author: jeff
 */

#include "RF.h"

/// Runtime Stack
#ifdef INTERFACE_ASK_RX
    RH_ASK ASK_Receiver(RF_BAUD, RF_RX_PIN, 0, 0, false);

    char rawAskMessage[RH_ASK_MAX_PAYLOAD_LEN] = {"\0"};
    //rfMessage askMessages[MAX_MESSAGE_QUEUE];
    rfMessageCircleBuffer askMessages;
#endif

#ifdef INTERFACE_NRF905
    RH_NRF905 NRF_905(NRF905_CE, NRF905_CE, NRF905_CSN);

    char rawNrfMessage[RH_NRF905_MAX_MESSAGE_LEN] = {"\0"};
    //rfMessage nrf905Messages[MAX_MESSAGE_QUEUE];
    rfMessageCircleBuffer nrf905Messages;
#endif


/// Functions
#ifdef INTERFACE_ASK_RX
bool SetupAskRx()
{
    bool success = false;;

    ASK_Receiver.setModeRx();

    if ( !ASK_Receiver.init() )
    {
         Serial.println("RadioHead ASK init failed!");
    }
    else
    {
        success = true;
        Serial.print  ("ASK RF Receiver configured on PIN ");
        Serial.print  (RF_RX_PIN);
        Serial.print  (" @ ");
        Serial.print  (RF_BAUD, DEC);
        Serial.println(" baud.");
    }

    return success;
}

bool RxAskMessage()
{
    bool success = false;

    if ( ASK_Receiver.available() )
    {
        uint8_t len_rf = RH_ASK_MAX_PAYLOAD_LEN;

        // Get the message
        if( ASK_Receiver.recv( (uint8_t*)rawAskMessage, &len_rf) )
        {
            success = true;

    #ifdef DEBUG
            ASK_Receiver.printBuffer("Got:", rawAskMessage, len_rf);
            Serial.print("RF Received :  ");
            for(uint8_t i = 0; i < len_rf; i++)
            {
                Serial.print( (char)rawAskMessage[i] );
            }
            Serial.println();
    #endif // DEBUG

            // Place in the ring buffer
            askMessages.Parse(rawAskMessage);
        }
    }

    return success;
}
#endif


#ifdef INTERFACE_ASK_TX
bool TxAskMessage(unsigned char *mem, size_t length)
{
    bool success = false;

    // TODO

    return success;
}
#endif


#ifdef INTERFACE_NRF905
bool RxNrfMessage()
{
    bool success = false;

    if ( NRF_905.available() )
    {
        uint8_t len_rf = RH_NRF905_MAX_MESSAGE_LEN;

        // Get the message
        if( NRF_905.recv( (uint8_t*)rawNrfMessage, &len_rf) )
        {
            success = true;

    #ifdef DEBUG
            Serial.print("NRF905 Received :  ");
            for(uint8_t i = 0; i < len_rf; i++)
            {
                Serial.print( (char)rawNrfMessage[i] );
            }
            Serial.println();
    #endif // DEBUG

            // Place in the ring buffer
            nrf905Messages.Parse(rawNrfMessage);
        }
    }

    return success;
}


bool TxNrfMessage(unsigned char *mem, size_t length)
{
    bool success = false;

    // TODO

    return success;
}
#endif

