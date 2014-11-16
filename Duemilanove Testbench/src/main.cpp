#include "main.h"

// Objects
dht 	DHT;
//RH_ASK 	RF_Transmitter;
RH_ASK 	RF_Receiver;


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

#if ASK_RF_RX_PIN and not ASK_RF_TX_PIN
	RF_Receiver.setModeRx();
    if ( !RF_Receiver.init() ) {
         Serial.println("RadioHead init failed");
    } else {
        Serial.print("ASK RF Receiver configured on pin ");
		Serial.println(ASK_RF_RX_PIN);
		Serial.print("ASK RF configured at Baud ");
		Serial.println(ASK_BAUD);
    }
#elif ASK_RF_TX_PIN and not ASK_RF_RX_PIN
    RF_Transmitter.setModeTx();
    if ( !RF_Transmitter.init() ) {
         Serial.println("RadioHead init failed");
    } else {
        Serial.print("ASK RF Transmitter configured on pin ");
		Serial.println(ASK_RF_TX_PIN);

		Serial.print("ASK RF configured at Baud ");
		Serial.println(ASK_BAUD);
    }
#endif

}


int main(void)
{
	init();
	setup();

    /// runtime stacks
#ifdef ASK_RF_RX_PIN
    uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
    uint8_t buflen = sizeof(buf);
#endif

    status_msg.header[0] = 'L';
    status_msg.header[1] = 'o';
    status_msg.header[2] = 'o';
    status_msg.header[3] = 'p';
    status_msg.header[4] = ' ';
    status_msg.header[5] = '#';

    /// loop control
	for( unsigned long int frame=0;;frame++)
	{
		digitalWrite(13, HIGH);


	    String new_msg = "Loop #";
	    new_msg.concat(frame);

		/// Read in sensor values
#ifdef ASK_RF_RX_PIN
		// Check for RF messages
	    if (RF_Receiver.recv(buf, &buflen)) // Non-blocking
	    {
	        int i;
	        // Message with a good checksum received, dump it.
	        RF_Receiver.printBuffer("RF Got:", buf, buflen);
	    }
#endif

#ifdef DHT11_PIN
		int DHT11_chk = DHT.read11(DHT11_PIN);
		switch (DHT11_chk)
		{
			case DHTLIB_OK:
			{
				// DISPLAY DATA
				Serial.print("DHT11 Measurments : ");
				Serial.print(DHT.humidity);
				Serial.print("RH , ");
				Serial.print(DHT.temperature);
				Serial.println("degC");
				break;
			}
			case DHTLIB_ERROR_CHECKSUM:
			{
				Serial.println("DHT11 Checksum error!\t");
				break;
			}
			case DHTLIB_ERROR_TIMEOUT:
			{
				Serial.println("DHT11 Time-out error!\t");
				break;
			}
			default:
			{
				Serial.println("DHT11 Unknown error!!!\t");
				break;
			}
		}
#endif

#ifdef DHT21_PIN
		Serial.print("DHT21 Status = ");
		int DHT21_chk = DHT.read21(DHT21_PIN);
		switch (DHT21_chk)
		{
			case DHTLIB_OK:
			{
				Serial.print("OK!");

				// DISPLAY DATA
				Serial.print("\tMeasurments : ");
				Serial.print(DHT.humidity);
				Serial.print("RH , ");
				Serial.print(DHT.temperature);
				Serial.println("degC");
				break;
			}
			case DHTLIB_ERROR_CHECKSUM:
			{
				Serial.println("Checksum error!\t");
				break;
			}
			case DHTLIB_ERROR_TIMEOUT:
			{
				Serial.println("Time-out error!\t");
				break;
			}
			default:
			{
				Serial.println("Unknown error!!!\t");
				break;
			}
		}
#endif


#ifdef ASK_RF_TX_PIN
	    //RF_Transmitter.send((uint8_t *)status_msg, 9+strlen((const char*)status_msg));
	    RF_Transmitter.send((uint8_t *)status_msg.container, 70);
	    RF_Transmitter.waitPacketSent();
		//Serial.println(*status_msg);
#endif

		digitalWrite(13, LOW);

		delay (LOOP_DELAY);
	};


	return 0;
}

