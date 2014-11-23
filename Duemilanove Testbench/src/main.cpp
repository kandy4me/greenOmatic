#include "main.h"

typedef union
{
	char container[68];
	struct{
		char header[6];
		unsigned long int number;
	};
}_status_msg;
_status_msg status_msg;


// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

void setup(void)
{
	Serial.begin(115200);
	Serial.println("Starting up greenOmatic Duemilanove Testbed");


	// Discrete IO
	pinMode(13, OUTPUT);	// Arduino on-board LED



	// I2C RTC



	// RF
	pinMode(RF_RX_PIN,INPUT);
	vw_set_rx_pin(RF_RX_PIN);
    vw_setup(RF_BAUD);
    vw_rx_start  ();
	Serial.print  ("RF Receiver Configured, PIN ");
	Serial.print  (RF_RX_PIN);
	Serial.print  (", ");
	Serial.print  (RF_BAUD, DEC);
	Serial.println(" baud.");


	// Ethernet
	int eth_stat = Ethernet.begin(mac);
	if (0 == eth_stat)
		Serial.println("Problem starting ethernet!");
	else
	{
		server.begin();
		Serial.print("Ethernet Server started @  ");
		Serial.println( Ethernet.localIP() );
	}


	Serial.println("Initialization complete.");
}


int main(void)
{
	 /// setup
	init();
	setup();

    /// runtime stacks
	unsigned long int frame;

    /// loop control
	for(frame=0;;frame++)
	{
		digitalWrite(13, HIGH);

	    String new_msg = "Loop #";
	    new_msg.concat(frame);
	    Serial.println(new_msg);


	    /* check time
	    Serial.println("Creating tm thing");
	    tmElements_t tm;

	    Serial.print("Doing thing");

	    if (RTC.read(tm))
	    {
	       Serial.print("Ok, Time = ");
	       print2digits(tm.Hour);
	       Serial.write(':');
	       print2digits(tm.Minute);
	       Serial.write(':');
	       print2digits(tm.Second);

	       Serial.print(", Date (D/M/Y) = ");
	       Serial.print(tm.Day);
	       Serial.write('/');
	       Serial.print(tm.Month);
	       Serial.write('/');
	       Serial.print(tmYearToCalendar(tm.Year));
	       Serial.println();
	     }
	    else
	    {
	    	Serial.println("Elsed Out, checking chip present...");
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
*/
	    // check RF
		uint8_t  buff_rf      [VW_MAX_MESSAGE_LEN];
		uint8_t  len_rf   =    VW_MAX_MESSAGE_LEN;
		if(vw_get_message(buff_rf, &len_rf))
		{
			Serial.print("RF: ");
			for(uint8_t i = 0; i < len_rf; i++)
				Serial.print((char)buff_rf[i]);
			Serial.println();
		}


		  // check ethernet
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
			  client.println("Refresh: 5");  // refresh the page automatically every 5 sec
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


		digitalWrite(13, LOW);

		delay (LOOP_DELAY);
	};


	return 0;
}

void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}

