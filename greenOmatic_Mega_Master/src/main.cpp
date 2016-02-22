#include "main.h"

/// Runtime Stack
// status
unsigned long int frame;
_status_flags     status;

// RTCC
#ifdef PERIPHERAL_RTCC
    // Date-Time objects
    RTC_DS1307 rtc;
#endif

/// Functions
void setup(void)
{
    /// Discrete & Analog IO
    pinMode(13, OUTPUT);	// Arduino on-board LED
    //pinMode(A0, OUTPUT);	// Buzzer

    /// Comm. Channels
    // UART
    Serial.begin(115200); 	Serial.flush();
    Serial.println("Starting up greenOmatic Mega 2560 Testbed...");
    Serial.print("Program compiled on ");
    Serial.print(__DATE__);
    Serial.print(" at ");
    Serial.println(__TIME__);
    Serial.println();

    // RF
#ifdef INTERFACE_ASK_RX
    if ( SetupAskRx() )
    {
        status.rfAskRx = status.VALID;
    }
    else
    {
        status.rfAskRx = status.ERROR;
        Serial.println("Could not initialize 1-wire ASK RF Receiver!");
    }
#endif //INTERFACE_ASK_RX

#ifdef INTERFACE_ASK_TX
    if ( SetupAskTx() )
    {
        status.rfAskTx = status.VALID;
    }
    else
    {
        status.rfAskTx = status.ERROR;
        Serial.println("Could not initialize 1-wire ASK RF Transmitter!");
    }
#endif //INTERFACE_ASK_TX

#ifdef INTERFACE_ETHERNET
	// Ethernet
    if ( SetupEthernet() )
    {
        status.ethernet_valid = status.VALID;
    }
    else
    {
        status.ethernet_valid = status.ERROR;
        Serial.println("Could not initialize Ethernet!!!");
    }

    if (status.VALID == status.ethernet_valid)
    {
#ifdef ETHERNET_WEBSERVER
        SetupWebServer();
        status.webserver_valid = status.VALID;
#endif

        if ( SetupUdpClient() )
        {
            status.udpclient_valid = status.VALID;
        }
        else
        {
            status.udpclient_valid = status.ERROR;
            Serial.println("Could not start up UDP client!");
        }
    }
#endif // #ifdef INTERFACE_ETHERNET

    /// Peripherals
    // I2C RTC
#ifdef PERIPHERAL_RTCC
    Wire.begin();
    rtc.begin();
    if (rtc.isrunning())
    {
        status.time_valid = status.VALID;
        GetDatetimeString(rtc.now());
        Serial.print("RTCC configured on I2C.  Time is currently ");
        Serial.println(currentTime);

#ifdef ETHERNET_UDPCLIENT
        //TODO: Get NTP Time
#else
        // Compare RTC time to this programs compile time
        DateTime rtcTime = rtc.now();
        DateTime compileTime(F(__DATE__), F(__TIME__));

        // If the compile-time is later (more recent) than the current RTC time, update the RTC
        if (compileTime.secondstime() > rtcTime.secondstime())
        {
            Serial.println("Program compile-time is later than RTC time; updating RTC.");
            rtc.adjust( DateTime(F(__DATE__), F(__TIME__)) );
        }
#endif //ETHERNET_UDPCLIENT
    }
    else
    {
        status.time_valid = status.ERROR;
        // TODO, can we narrow this down further like with the DS1307RTC library?
    }
#endif

	Serial.println("\nInitialization complete!\n\n");
}


int main(void)
{
    /// setup
    init();
    setup();

    /// loop control
    for(frame=0; ; ++frame)
    {
        digitalWrite(13, HIGH);

        String new_msg = "Loop #";
        new_msg.concat(frame);

        // Only update NTP time every 10,000 loops
#ifdef ETHERNET_UDPCLIENT
        if ( 0 == frame % 10000 )
        {
            updateCurrentUnixTime();
        }
#endif



#ifdef PERIPHERAL_RTCC
        /// check time
        if (status.VALID == status.time_valid)
        {
            GetDatetimeString(rtc.now());
        }

/*      TODO: port RTC.chipPresent() functionality over to RTClib
        if ( RTC.read(tm) )
        {
            status.time_valid = status.VALID;
        }
        else
        {
           if ( RTC.chipPresent() )
           {
              status.time_valid = status.INVALID;
              Serial.println("The DS1307 is stopped.  Please set the RTC time.");
              Serial.println();
           }
           else
           {
             status.time_valid = status.UNINSTALLED;
             Serial.println("DS1307 read error!  Please check the circuitry.");
             Serial.println();
           }
         }
*/
#endif

        /// Check interfaces for received messages
        // Serial, direct to the Command Line Interface (CLI)
        if( Serial.available() > 0 )
        {
            char buff_console [8] = {"\0"};
            for(uint8_t len_console = 0x00; Serial.available() > 0; len_console++)
            {
                buff_console[len_console] = Serial.read();
                CLI(buff_console, len_console);
            }
        }

#ifdef INTERFACE_ASK_RX
        // RF (1-wire ASK, aka VirtualWire), print to console
        if( RxAskMessage() )
        {
#ifdef PERIPHERAL_RTCC
            // Prefix received messages with current date-time on console
            if (status.VALID == status.time_valid)
            {
                Serial.print(currentTime);
                Serial.write(" | ");
            }
#endif //PERIPHERAL_RTCC
            // TODO: print off ASK-RF Received Message!!!
        }
#endif //INTERFACE_ASK_RX


#ifdef INTERFACE_NRF905
        // RF (1-wire ASK, aka VirtualWire), print to console
        if( RxNrfMessage() )
        {
#ifdef PERIPHERAL_RTCC
            // Prefix received messages with current date-time on console
            if (status.VALID == status.time_valid)
            {
                Serial.print(currentTime);
                Serial.write(" | ");
            }
#endif //PERIPHERAL_RTCC
            // TODO: print off ASK-RF Received Message!!!
        }
#endif //INTERFACE_NRF905


#ifdef ETHERNET_WEBSERVER
       EthernetClient client = server.available();
       if (client)
       {
            Serial.println("new http client");
            // an http request ends with a blank line
            boolean currentLineIsBlank = true;
            while (client.connected())
            {
                if (client.available())
                {
                    char c = client.read();
                    Serial.write(c);
                    // if you've gotten to the end of the line (received a newline
                    // character) and the line is blank, the http request has ended,
                    // so you can send a reply
                    if (c == '\n' && currentLineIsBlank)
                    {
                        // send a standard http response header
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connection: close");  // the connection will be closed after completion of the response
                        client.println("Refresh: 60");  // refresh the page automatically every 60 sec
                        client.println();
                        client.println("<!DOCTYPE HTML>");
                        client.println("<html>");

#ifdef PERIPHERAL_RTCC
                        client.print("\n\ngreen-O-matic RTC Time : ");
                        // TODO
#endif //PERIPHERAL_RTCC

#ifdef ETHERNET_UDPCLIENT
                        client.print("\nUDP NTP Time : ");
                        // TODO
#endif //ETHERNET_UDPCLIENT

#ifdef INTERFACE_ASK_RX
                        client.println("\n\nMost recently received 433MHz ASK message : ");
                        // TODO
#endif //INTERFACE_ASK_RX

#ifdef INTERFACE_NRF905
                        client.println("\n\nMost recently received NRF_905 message: ");
                        // TODO

                        client.println("\nMost recently transmitted NRF_905 message: ");
                        // TODO
#endif //INTERFACE_NRF905

                        client.println("</html>");
                        break;
                    }

                    if (c == '\n')
                    {
                      // you're starting a new line
                      currentLineIsBlank = true;
                    }
                    else if (c != '\r')
                    {
                      // you've gotten a character on the current line
                      currentLineIsBlank = false;
                    }
                }
            }
            // give the web browser time to receive the data
            delay(5);
            // close the connection
            client.stop();
            Serial.println("client disconnected");
	    }
#endif //ETHERNET_WEBSERVER

        digitalWrite(13, LOW);

        delay (LOOP_DELAY);
    }
    return 0;
}


/******** Command Line Interpreter ********/
void CLI(char *msg, const uint8_t &length)
{
    // ignore any leading spaces
    while(' ' == *msg)
    {
        ++msg;
    }
    Serial.print("Received Serial Input : ");
    Serial.println(*msg);

    if('f' == *msg)
    {
        Serial.print("Current frame : ");
        Serial.println(frame);
    }

#ifdef PERIPHERAL_RTCC
    else if ('t' == *msg)
    {
        Serial.print("RTC time is : ");
        Serial.println(currentTime);
    }

    else if ('T' == *msg)
    {
        // TODO: Manually update time
        // This line sets the RTC with an explicit date & time, for example to set
        // January 21, 2014 at 3am you would call:
        // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    }

#ifdef ETHERNET_UDPCLIENT
    else if ('N' == *msg)
    {
        Serial.print("RTC time is : ");
        Serial.println(currentTime);

        if (status.ethernet_valid)
        {
            Serial.print("Attempting to get NTP time and set RTC...");

            //TODO: Update RTC time with NTP
        }
        else
        {
            Serial.println("Error: Ethernet status is invalid, cannot get NTP time");
        }
    }
#endif // ETHERNET_UDPCLIENT

#endif // PERIPHERAL_RTCC
}

