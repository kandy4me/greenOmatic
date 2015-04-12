#include "main.h"


/// runtime stacks
unsigned long int frame;
_status_flags status;
#ifdef PERIPHERAL_RTCC
RTC_DS1307 rtc;
tmElements_t tm;
#endif
#ifdef ethernet
#define localPort 8888	// local port to listen for UDP packets
EthernetUDP Udp;
EthernetServer server(80); // (port 80 is default for HTTP)

// ethernet and RTCC
#ifdef PERIPHERAL_RTCC
#define NTP_PACKET_SIZE 48  // NTP time stamp is in the first 48 bytes of the message
byte pb[NTP_PACKET_SIZE];       // buffer to hold incoming and outgoing packets
byte timeServer[] = {192, 43, 244, 18}; // time.nist.gov NTP server
#endif
#endif


void setup(void)
{
	status.reset();

	/// Discrete & Analog IO
	pinMode(13, OUTPUT);	// Arduino on-board LED

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
#endif
#ifdef INTERFACE_ETHERNET
	Serial.print("Starting Ethernet... ");
	int eth_stat = Ethernet.begin(mac);
	if (0 == eth_stat)
		Serial.println(" Problem starting ethernet !");
	else
	{
		server.begin();
		Serial.print("Ethernet Server started @  ");
		Serial.println( Ethernet.localIP() );
	}
	status.ethernet_valid = status.UNINSTALLED; // TODO, shouldn't be "uninstalled"
#endif


	/// Peripherals
	// I2C RTC
#ifdef PERIPHERAL_RTCC
	Wire.begin();
	rtc.begin();

	// Read Once to check status

	if (rtc.isrunning())
	{
		status.time_valid = status.VALID;

	    DateTime now = rtc.now();
	    Serial.print("RTCC Configured on I2C.  Time is currently ");
	    Serial.print(now.year(), DEC);
	    Serial.print('-');
	    Serial.print(now.month(), DEC);
	    Serial.print('-');
	    Serial.print(now.day(), DEC);
	    Serial.print(' ');
	    Serial.print(now.hour(), DEC);
	    Serial.print(':');
	    Serial.print(now.minute(), DEC);
	    Serial.print(':');
	    Serial.print(now.second(), DEC);
	    Serial.println();
	}
	else
	{
		status.time_valid = status.ERROR; // TODO, can we narrow this down further like with the other RTC library?

		 // following line sets the RTC to the date & time this sketch was compiled
		rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
		// This line sets the RTC with an explicit date & time, for example to set
		// January 21, 2014 at 3am you would call:
		// rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
	}
/*
	if ( RTC.read(tm) )
	{
	    Serial.print("RTCC Configured on I2C.  Time is currently ");
	    print2digits(tm.Hour);
	    Serial.write(':');
	    print2digits(tm.Minute);
	    Serial.write(':');
	    print2digits(tm.Second);
	    Serial.print(", Date is ");
	    Serial.print(tmYearToCalendar(tm.Year));
	    Serial.write('-');
	    Serial.print(tm.Month);
	    Serial.write('-');
	    Serial.print(tm.Day);
	    Serial.println();
	}
	else if (RTC.chipPresent())
	{
#ifdef ethernet
		//TODO: Get NTP Time
#else
	      Serial.println("The DS1307 is stopped.  Please set the RTC time.");
	      Serial.println();
#endif
	}
	else
	{
	      Serial.println("DS1307 read error!  Please check the circuitry.");
	      Serial.println();
	}
*/
#endif

	Serial.println("\nInitialization complete!\n\n");
}


int main(void)
{
	/// setup
	init();
	setup();

	/// loop control
	for(frame=0;;frame++)
	{
		digitalWrite(13, HIGH);

		status.reset();

	    String new_msg = "Loop #";
	    new_msg.concat(frame);
	    //Serial.println(new_msg);

		if(Serial.available() > 0)
		{
			char 	buff_console [8];
			for(uint8_t len_console = 0x00; Serial.available() > 0; len_console++)
			{
				buff_console[len_console] = Serial.read();
				CLI(buff_console, len_console);
			}
		}

#ifdef PERIPHERAL_RTCC
	    // check time
	    if (RTC.read(tm))
	    	status.time_valid = status.VALID;
	    else
	    {
	       if (RTC.chipPresent())
	       {
#ifdef ethernet
		//TODO: Get NTP Time
#else
	    	  status.time_valid = status.INVALID;
	 	      Serial.println("The DS1307 is stopped.  Please set the RTC time.");
	 	      Serial.println();
#endif
	       }
	       else
	       {
	    	 status.time_valid = status.UNINSTALLED;
	         Serial.println("DS1307 read error!  Please check the circuitry.");
	         Serial.println();
	       }
	     }
#endif

#ifdef INTERFACE_ASK_RX
	    // check RF
		uint8_t  buff_rf   [VW_MAX_MESSAGE_LEN];
		uint8_t  len_rf  =  VW_MAX_MESSAGE_LEN;
		if(vw_get_message(buff_rf, &len_rf))
		{
#ifdef PERIPHERAL_RTCC
			if (status.VALID == status.time_valid)
			{
			    Serial.print(tmYearToCalendar(tm.Year));
			    Serial.write('-');
			    Serial.print(tm.Month);
			    Serial.write('-');
			    Serial.print(tm.Day);

			    Serial.write(" ");
			    print2digits(tm.Hour);
			    Serial.write(':');
			    print2digits(tm.Minute);
			    Serial.write(':');
			    print2digits(tm.Second);
			    Serial.write(" | ");
			}
#endif
			Serial.print("RF Received :  ");
			for(uint8_t i = 0; i < len_rf; i++)
				Serial.print((char)buff_rf[i]);
			Serial.println();
		}
#endif

#ifdef INTERFACE_ETHERNET
		  EthernetClient client = server.available();
		  if (client) {
		    Serial.println("new client");
		    // an http request ends with a blank line
		    boolean currentLineIsBlank = true;
		    while (client.connected()) {
		      if (client.available()) {
		        char c = client.read();
		        Serial.write(c);
		        // if you've gotten to the end of the line (received a newline
		        // character) and the line is blank, the http request has ended,
		        // so you can send a reply
		        if (c == '\n' && currentLineIsBlank) {
		          // send a standard http response header
		          client.println("HTTP/1.1 200 OK");
		          client.println("Content-Type: text/html");
		          client.println("Connection: close");  // the connection will be closed after completion of the response
		          client.println("Refresh: 60");  // refresh the page automatically every 60 sec
		          client.println();
		          client.println("<!DOCTYPE HTML>");
		          client.println("<html>");
		          // output the value of each analog input pin
		          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
		            int sensorReading = analogRead(analogChannel);
		            client.print("analog input ");
		            client.print(analogChannel);
		            client.print(" is ");
		            client.print(sensorReading);
		            client.println("<br />");
		          }
		          client.println("</html>");
		          break;
		        }
		        if (c == '\n') {
		          // you're starting a new line
		          currentLineIsBlank = true;
		        }
		        else if (c != '\r') {
		          // you've gotten a character on the current line
		          currentLineIsBlank = false;
		        }
		      }
		    }
		    // give the web browser time to receive the data
		    delay(1);
		    // close the connection:
		    client.stop();
		    Serial.println("client disconnected");
		  }
#endif

		digitalWrite(13, LOW);

		delay (LOOP_DELAY);
	};
	return 0;
}


/******** Command Line Interpreter ********/
void CLI(char *msg, uint8_t length)
{
//  ignore any leading spaces
	while(' ' == *msg)
		msg++;

	Serial.print("Received Serial Input : ");
	Serial.println(*msg);


	if('f' == *msg)
	{
		Serial.print("Current frame : ");
		Serial.println(frame);
	}

#ifdef INTERFACE_ETHERNET
	else if ('D' == *msg)
	{
		if (status.ethernet_valid)
		{
			Serial.print("Attempting to get NTP time and set RTC...");

			tmElements_t new_time = getNTPtime(timeServer);

			if ( RTC.write(new_time) )
			{
				Serial.print("Time set!");
			}
		}
		else
		{
			Serial.println("Error: Ethernet status is invalid, cannot get NTP time");
		}

	}
#endif

#ifdef PERIPHERAL_RTCC
	else if ('M' == *msg)
	{
		tmElements_t new_time;
		uint8_t user_hour = 0;
		uint8_t user_min  = 0;
		uint8_t user_sec  = 0;
		char    serial_buff[8] = {0};

		Serial.println("Set RTC Time");
		Serial.print("Enter hours : ");
		// TODO
		while(Serial.available() > 2);
		int hour_count = 0;
		for(; Serial.available() > 0; hour_count++)
		{
			serial_buff[hour_count] = Serial.read();
			Serial.print(serial_buff[hour_count]);
		}

		new_time.Hour = serial_buff[0];

		Serial.print("Enter minutes : ");
		// TODO
		new_time.Hour = user_min;

		Serial.print("Enter seconds : ");
		// TODO
		new_time.Hour = user_sec;

		// TODO: SANITY CHECKING
		if (user_hour and user_min and user_sec)
		{
			// get date from current Time Object
			new_time.Day   = tm.Day;
			new_time.Month = tm.Month;
			new_time.Year  = tm.Year;

			Serial.print("Press 'Enter' to set time.");

		    if ( RTC.write(new_time) )
		    {
				Serial.print("Time set!");
		    }
		}

	}

	else if ('A' == *msg)
	{
		unsigned int Years, Months, Days, Hours, Minutes, Seconds;

		if ( RTC.read(tm) )
		{
		    Hours   = tm.Hour;
		    Minutes = tm.Minute;
		    Seconds = tm.Second;
		    Years  = tmYearToCalendar(tm.Year);
		    Months = tm.Month;
		    Days   = tm.Day;

		    Serial.print("RTC Read Successful!  Enter clock adjustment in minutes, preceeded by '+' or '-'");

		    // TODO

			Serial.print("Press 'Enter' to set time.");

			tmElements_t new_time;

			new_time.Day    = Days;
			new_time.Month  = Months;
			new_time.Year   = Years;
			new_time.Hour   = Hours;
			new_time.Minute = Minutes;
			new_time.Second = Seconds;

		    if ( RTC.write(new_time) )
		    {
				Serial.print("Time set!");
		    }
		    else
		    {
		    	Serial.print("Unsuccessful!");
		    }
		}
		else
		{
			Serial.println("Could not read from RTC, adjustment not possible!");
		}
	}
#endif

	Serial.println();
	Serial.flush();
}


#ifdef INTERFACE_ETHERNET
void sendNTPpacket(byte *address)
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


tmElements_t getNTPtime(byte *address)
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

		PrintDateTime(t4); Serial.println(f4,4);
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


#ifdef PERIPHERAL_RTCC
void PrintDateTime(unsigned long t)
{
    /*
	char datestr[24];
    sprintf(datestr, "%04d-%02d-%02d  %02d:%02d:%02d  ", t.year(), t.month(), t.day(), t.hour(), t.minute(), t.second());
    Serial.print(datestr);
    */
    Serial.print(tmYearToCalendar(tm.Year));
    Serial.write('-');
    Serial.print(tm.Month);
    Serial.write('-');
    Serial.print(tm.Day);

    Serial.write(" ");
    print2digits(tm.Hour);
    Serial.write(':');
    print2digits(tm.Minute);
    Serial.write(':');
    print2digits(tm.Second);
    Serial.write(" | ");

}


void print2digits(int number)
{
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}
#endif

