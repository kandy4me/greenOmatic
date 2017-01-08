#include "main.h"

/// Runtime Stack
// status
unsigned long int frame;
_status_flags     status;

#ifdef PERIPHERAL_RTCC
// Date-Time objects
RTC_DS1307 rtc;
char currentTime[21];
#endif

#ifdef ETHERNET_WEBSERVER
// HTML server
EthernetServer server(80); // (port 80 is default for HTTP)
#endif //ETHERNET_WEBSERVER

#ifdef ETHERNET_UDPCLIENT
// UDP client, mostly for getting NTP time
#define 	localPort 	8888 // local port to listen for UDP packets
EthernetUDP Udp;
#define NTP_PACKET_SIZE 48  	// NTP time stamp is in the first 48 bytes of the message
byte pb[NTP_PACKET_SIZE];       // buffer to hold incoming and outgoing packets
byte timeServer[] = {192, 43, 244, 18}; // time.nist.gov NTP server
#endif //ETHERNET_UDPCLIENT


/// Utility Functions
#ifdef PERIPHERAL_RTCC
// Date-Time String
void GetDatetimeString(const tmElements_t &dt, char *stringBuffer = currentTime)
{
	sprintf(stringBuffer, "%04d-%02d-%02d %02d:%02d:%02d", dt.Year, dt.Month, dt.Day, dt.Hour, dt.Minute, dt.Second);
}


void GetDatetimeString(const DateTime &dt, char *stringBuffer = currentTime)
{
	sprintf(stringBuffer, "%04d-%02d-%02d %02d:%02d:%02d", dt.year(), dt.month(), dt.day(), dt.hour(), dt.minute(), dt.second());
}


/*
// Date-Time Printing
void PrintZeroPadded(const uint8_t &number)
{
    if ( (0 <= number) && (10 > number) )
    {
        Serial.write('0');
    }
    Serial.print(number, DEC);
}


void PrintDateTime(const tmElements_t &dt)
{
	char datestr[21];
    sprintf(datestr, "%04d-%02d-%02d %02d:%02d:%02d", dt.Year, dt.Month, dt.Day, dt.Hour, dt.Minute, dt.Second);
    Serial.write(datestr);

    //Serial.print(tmYearToCalendar(dt.Year));
    //Serial.write('-');
    //PrintZeroPadded(dt.Month);
    //Serial.write('-');
    //PrintZeroPadded(dt.Day);
    //Serial.write(' ');
    //Serial.write(" ");
    //PrintZeroPadded(dt.Hour);
    //Serial.write(':');
    //PrintZeroPadded(dt.Minute);
    //Serial.write(':');
    //PrintZeroPadded(dt.Second);
}


void PrintDateTime(const DateTime &dt)
{
	char datestr[21];
    sprintf(datestr, "%04d-%02d-%02d %02d:%02d:%02d", dt.year(), dt.month(), dt.day(), dt.hour(), dt.minute(), dt.second());
    Serial.write(datestr);

	//Serial.print(dt.year(), DEC);
	//Serial.write('-');
	//PrintZeroPadded(dt.month());
	//Serial.write('-');
	//PrintZeroPadded(dt.day());
	//Serial.write(' ');
	//PrintZeroPadded(dt.hour());
	//Serial.write(':');
	//PrintZeroPadded(dt.minute());
	//Serial.write(':');
	//PrintZeroPadded(dt.second());
}
*/
#endif


#ifdef ETHERNET_UDPCLIENT
void sendNTPpacket(const byte *address)
/**
 * send an NTP request to the time server at the given address
 */
{
  // set all bytes in the buffer to 0
  memset(pb, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  pb[0] = 0b11100011;   // LI, Version, Mode
  pb[1] = 0;     // Stratum, or type of clock
  pb[2] = 6;     // Polling Interval
  pb[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  pb[12]  = 49;
  pb[13]  = 0x4E;
  pb[14]  = 49;
  pb[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(pb,NTP_PACKET_SIZE);
  Udp.endPacket();
}


void getNTPtime(const byte *address)
{
	// send an NTP packet to a time server
	sendNTPpacket(timeServer);

	// wait to see if a reply is available
	delay(1000);

	if ( Udp.available() )
	{
		// read the packet into the buffer
		Udp.read(pb, NTP_PACKET_SIZE);      // New from IDE 1.0,


		// NTP contains four timestamps with an integer part and a fraction part
		// we only use the integer part here
		unsigned long t1, t2, t3, t4;
		t1 = t2 = t3 = t4 = 0;
		for (int i=0; i< 4; i++)
		{
			t1 = t1 << 8 | pb[16+i];
			t2 = t2 << 8 | pb[24+i];
			t3 = t3 << 8 | pb[32+i];
			t4 = t4 << 8 | pb[40+i];
		}

		// part of the fractional part
		// could be 4 bytes but this is more precise than the 1307 RTC
		// which has a precision of ONE second
		// in fact one byte is sufficient for 1307
		float f1,f2,f3,f4;
		f1 = ((long)pb[20] * 256 + pb[21]) / 65536.0;
		f2 = ((long)pb[28] * 256 + pb[29]) / 65536.0;
		f3 = ((long)pb[36] * 256 + pb[37]) / 65536.0;
		f4 = ((long)pb[44] * 256 + pb[45]) / 65536.0;

		// NOTE:
		// one could use the fractional part to set the RTC more precise
		// 1) at the right (calculated) moment to the NEXT second!
		//    t4++;
		//    delay(1000 - f4*1000);
		//    RTC.adjust(DateTime(t4));
		//    keep in mind that the time in the packet was the time at
		//    the NTP server at sending time so one should take into account
		//    the network latency (try ping!) and the processing of the data
		//    ==> delay (850 - f4*1000);
		// 2) simply use it to round up the second
		//    f > 0.5 => add 1 to the second before adjusting the RTC
		//   (or lower threshold eg 0.4 if one keeps network latency etc in mind)
		// 3) a SW RTC might be more precise, => ardomic clock :)


		// convert NTP to UNIX time, differs seventy years = 2208988800 seconds
		// NTP starts Jan 1, 1900
		// Unix time starts on Jan 1 1970.
		const unsigned long seventyYears = 2208988800UL;
		t1 -= seventyYears;
		t2 -= seventyYears;
		t3 -= seventyYears;
		t4 -= seventyYears;


		//Serial.println("T1 .. T4 && fractional parts");
		//PrintDateTime(DateTime(t1)); Serial.println(f1,4);
		//PrintDateTime(DateTime(t2)); Serial.println(f2,4);
		//PrintDateTime(DateTime(t3)); Serial.println(f3,4);

		//PrintDateTime(t4);
		char ntpTime[21];
		//GetDatetimeString(t4, ntpTime);
		Serial.println(f4,4);
		Serial.println();

		// Adjust timezone and DST... in my case substract 4 hours for Chile Time
		// or work in UTC?
		t4 -= (3 * 3600L);     // Notice the L for long calculations!!
		t4 += 1;               // adjust the delay(1000) at begin of loop!
		if (f4 > 0.4) t4++;    // adjust fractional part, see above
	}

	else
	{
		Serial.println("Failed!");
	}
}
#endif


void setup(void)
{
	status.reset();

	/// Discrete & Analog IO
	pinMode(13, OUTPUT);	// Arduino on-board LED
	pinMode(A0, OUTPUT);	// Buzzer

	/// Comm. Channels
	// UART
	Serial.begin(115200); 	Serial.flush();
	Serial.println("Starting up greenOmatic Duemilanove Testbed...");
	Serial.print("Program compiled on ");
	Serial.print(__DATE__);
	Serial.print(" at ");
	Serial.println(__TIME__);
	Serial.println();

	// RF
#ifdef INTERFACE_ASK_RX
	pinMode(RF_RX_PIN, INPUT);
	vw_set_rx_pin(RF_RX_PIN);
    vw_setup(RF_BAUD);
    vw_rx_start  ();
	Serial.print  ("ASK RF Receiver configured on PIN ");
	Serial.print  (RF_RX_PIN);
	Serial.print  (" @ ");
	Serial.print  (RF_BAUD, DEC);
	Serial.println(" baud.");
#endif //INTERFACE_ASK_RX

	// Ethernet
#ifdef INTERFACE_ETHERNET
	Serial.print("Starting Ethernet... ");

#ifdef ETHERNET_DYNAMIC_IP
	int eth_stat = Ethernet.begin(mac);
	if (0 == eth_stat)
	{
		Serial.println(" Problem starting ethernet !");
		status.ethernet_valid = status.ERROR;
	}
	else
	{
		Serial.print("Ethernet started, IP = ");
		Serial.println( Ethernet.localIP() );
		status.ethernet_valid = status.VALID;
	}
#else
	Ethernet.begin(mac, IPaddr);
	Serial.print("Ethernet started, IP = ");
	Serial.println( Ethernet.localIP() );
	status.ethernet_valid = status.VALID;
#endif //ETHERNET_DYNAMIC_IP

#ifdef ETHERNET_WEBSERVER
		server.begin();
#endif //ETHERNET_WEBSERVER

#ifdef ETHERNET_UDPCLIENT
		Udp.begin(localPort);
#endif //ETHERNET_UDPCLIENT

#endif

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

		status.reset();

	    String new_msg = "Loop #";
	    new_msg.concat(frame);

#ifdef PERIPHERAL_RTCC
	    /// check time
		if (status.VALID == status.time_valid)
		{
			GetDatetimeString(rtc.now());
		}

/* 		TODO: port RTC.chipPresent() functionality over to RTClib
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
		if(Serial.available() > 0)
		{
			char 	buff_console [8];
			for(uint8_t len_console = 0x00; Serial.available() > 0; len_console++)
			{
				buff_console[len_console] = Serial.read();
				CLI(buff_console, len_console);
			}
		}

#ifdef INTERFACE_ASK_RX
	    // RF (1-wire ASK, aka VirtualWire), print to console
		uint8_t  buff_rf   [VW_MAX_MESSAGE_LEN];
		uint8_t  len_rf  =  VW_MAX_MESSAGE_LEN;
		if(vw_get_message(buff_rf, &len_rf))
		{
#ifdef PERIPHERAL_RTCC
			// Prefix received messages with current date-time on console
			if (status.VALID == status.time_valid)
			{
				Serial.print(currentTime);
			    Serial.write(" | ");
			}
#endif //PERIPHERAL_RTCC
			Serial.print("RF Received :  ");
			for(uint8_t i = 0; i < len_rf; i++)
			{
				Serial.print((char)buff_rf[i]);
			}
			Serial.println();
		}
#endif //INTERFACE_ASK_RX

#ifdef ETHERNET_WEBSERVER
		  EthernetClient client = server.available();
		  if (client) {
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
		          client.print("green-O-matic RTC Time : ");
		          client.println(currentTime);
#endif //PERIPHERAL_RTCC

#ifdef INTERFACE_ASK_RX
		          client.println("Most recently received 433MHz ASK Transmission : ");
#endif //INTERFACE_ASK_RX

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
	};
	return 0;
}


/******** Command Line Interpreter ********/
void CLI(char *msg, const uint8_t length)
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

