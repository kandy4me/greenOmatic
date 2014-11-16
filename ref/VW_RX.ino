// receiver.pde
//
// Simple example of how to use VirtualWire to receive messages
// Implements a simplex (one-way) receiver with an Rx-B1 module
//
// See VirtualWire.h for detailed API docs
// Author: Mike McCauley (mikem@airspayce.com)
// Copyright (C) 2008 Mike McCauley
// $Id: receiver.pde,v 1.3 2009/03/30 00:07:24 mikem Exp $

#include <VirtualWire.h>

void setup()
{
    Serial.begin(115200);	// Debugging only

    // Initialise the IO and ISR
    vw_setup(1200);	 // Bits per sec
    vw_rx_start();       // Start the receiver PLL running
    
    Serial.println("setup complete, running RX");
}

void loop()
{

    uint8_t buf[VW_MAX_MESSAGE_LEN];
    uint8_t buflen = VW_MAX_MESSAGE_LEN;
vw_wait_rx_max(500);
 digitalWrite(13, true); 
    if ( vw_get_message(buf, &buflen) ) // Non-blocking
    { 
	// Message with a good checksum received, dump it.
	Serial.print("Got: ");

	int i;
	for (i = 0; i < buflen; i++)
	{
	    Serial.print(buf[i], HEX);
	    Serial.print(" ");
	}
	Serial.println("");

    }
    /*
    else 
    {
  	Serial.print("DEBUG: ");

	int i;
	for (i = 0; i < buflen; i++)
	{
	    //Serial.print(buf[i], HEX);
            Serial.print( buf[i] );
	    Serial.print(" ");
	}
	Serial.println("");
    }
    */
    digitalWrite(13, false);  
}
