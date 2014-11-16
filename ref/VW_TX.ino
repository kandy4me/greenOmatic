// transmitter.pde
//
// Simple example of how to use VirtualWire to transmit messages
// Implements a simplex (one-way) transmitter with an TX-C1 module
//
// See VirtualWire.h for detailed API docs
// Author: Mike McCauley (mikem@airspayce.com)
// Copyright (C) 2008 Mike McCauley
// $Id: transmitter.pde,v 1.3 2009/03/30 00:07:24 mikem Exp $

#include <VirtualWire.h>

const char *msg = "long test string 123";
    
void setup()
{
    Serial.begin(115200);	  // Debugging only
    
    // Initialise the IO and ISR
    vw_setup(1200);	 // Bits per sec
    
    Serial.println("setup complete, TX running");
    
    Serial.print("Transmission string : ");
    Serial.println(msg);
    
    Serial.print("Reception Preview : ");
    	for (int p = 0; p < 21; p++)
	{
	    Serial.print(msg[p], HEX);
            //Serial.print( msg[p] );
	    Serial.print(" ");
	}
Serial.println();
}

void loop()
{
   digitalWrite(13, true); // Flash a light to show transmitting
    
    vw_send((uint8_t *)msg, strlen(msg));
    vw_wait_tx(); // Wait until the whole message is gone
    
    digitalWrite(13, false);
    
    delay(333);
}

