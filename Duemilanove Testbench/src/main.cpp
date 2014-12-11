#include "main.h"


/// runtime stacks
unsigned long int frame;
_status_flags status;
tmElements_t tm;
EthernetServer server(80); // (port 80 is default for HTTP):


void setup(void)
{
	status.reset();

	/// Discrete & Analog IO
	pinMode(13, OUTPUT);	// Arduino on-board LED


	/// Comm. Channels
	// UART
	Serial.begin(115200);
	Serial.println("Starting up greenOmatic Duemilanove Testbed...");

	// RF
	pinMode(RF_RX_PIN, INPUT);
	vw_set_rx_pin(RF_RX_PIN);
    vw_setup(RF_BAUD);
    vw_rx_start  ();
	Serial.print  ("RF Receiver Configured on PIN ");
	Serial.print  (RF_RX_PIN);
	Serial.print  (" @ ");
	Serial.print  (RF_BAUD, DEC);
	Serial.println(" baud.");

	/* Ethernet
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
	*/

	/// Peripherals
	// I2C RTC
	// Read Once to check status
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
		// Get NTP Time
	      Serial.println("The DS1307 is stopped.  Please run the SetTime");
	      Serial.println("example to initialize the time and begin running.");
	      Serial.println();
	}
	else
	{
	      Serial.println("DS1307 read error!  Please check the circuitry.");
	      Serial.println();
	}

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
			char 	buff_console [VW_MAX_MESSAGE_LEN];	//same size as RF buffer, I guess
			uint8_t len_console = 0x00;

			for(; Serial.available() > 0; len_console++)
				buff_console[len_console] = Serial.read();

			CLI(buff_console, len_console);
		}



	    // check time
	    if (RTC.read(tm))
	    	status.time_valid = 1;

	    else
	    {
	       if (RTC.chipPresent())
	       {
	         Serial.println("The DS1307 is stopped.  Please run the SetTime");
	         Serial.println("example to initialize the time and begin running.");
	         Serial.println();
	       }
	       else
	       {
	         Serial.println("DS1307 read error!  Please check the circuitry.");
	         Serial.println();
	       }
	     }

	    // check RF
		uint8_t  buff_rf   [VW_MAX_MESSAGE_LEN];
		uint8_t  len_rf  =  VW_MAX_MESSAGE_LEN;
		if(vw_get_message(buff_rf, &len_rf))
		{
			if (status.time_valid)
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
			Serial.print("RF Received :  ");
			for(uint8_t i = 0; i < len_rf; i++)
				Serial.print((char)buff_rf[i]);
			Serial.println();
		}


		  /* check ethernet
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
*/

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

	if ('D' == *msg)
	{
		tmElements_t new_time;
		Serial.println("Set RTC Date");
		Serial.print("Enter year: ");

		Serial.print("Enter month: ");

		Serial.print("Enter day: ");

	}
	else if ('T' == *msg)
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

/******************** LIDAR *****************/
	else if('f' == *msg)
	{
		Serial.print("Current frame : ");
		Serial.println(frame);
	}

	Serial.println();
	Serial.flush();
}




void print2digits(int number)
{
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}

