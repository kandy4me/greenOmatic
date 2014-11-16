#include "main.h"

// Objects
dht 	DHT;


typedef union
{
	char container[68];
	struct{
		char header[6];
		unsigned long int number;
	};
}_status_msg;
_status_msg status_msg;



void setup(void)
{
	Serial.begin(115200);
	Serial.println("Starting up Green-O-Matic Duemilanove Testbed");
	pinMode(13, OUTPUT);

	Serial.print("Using DHT Library version ");
	Serial.println(DHT_LIB_VERSION);

#ifdef DHT11_PIN
	//pinMode(3, DHT11_PIN);
    Serial.print("DHT11 Sensor configured on pin ");
    Serial.println(DHT11_PIN);
#endif

#ifdef DHT21_PIN
	//pinMode(3, DHT21_PIN);
    Serial.print("DHT21 Sensor configured on pin ");
    Serial.println(DHT21_PIN);
#endif

	pinMode(RF_RX_PIN,INPUT);
	vw_set_rx_pin(RF_RX_PIN);
    vw_setup(RF_BAUD);
    vw_rx_start  ();
	Serial.print ("RF RX Baud = ");
	Serial.println(RF_BAUD, DEC);

}


int main(void)
{
	init();
	setup();

    /// runtime stacks
    uint8_t buf[VW_MAX_MESSAGE_LEN];
    uint8_t buflen = sizeof(buf);


    /// loop control
	for( unsigned long int frame=0;;frame++)
	{
		digitalWrite(13, HIGH);


	    String new_msg = "Loop #";
	    new_msg.concat(frame);

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

		digitalWrite(13, LOW);

		delay (LOOP_DELAY);
	};


	return 0;
}

