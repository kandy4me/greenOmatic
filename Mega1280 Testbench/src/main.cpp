#include "main.h"

// Objects
dht 	DHT;
RH_ASK 	RF_Transmitter( ASK_BAUD, ASK_RX_PIN, ASK_TX_PIN, 10, false); // (uint16_t speed, uint8_t rxPin, uint8_t txPin, uint8_t pttPin, bool pttInverted);


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
	Serial.println("Starting up Green-O-Matic Mega Testbed");
	pinMode(13, OUTPUT);

	Serial.print("Using DHT Library version ");
	Serial.println(DHT_LIB_VERSION);

    Serial.print("DHT11 Sensor configured on pin ");
    Serial.println(DHT11_PIN, DEC);

    // RF Setup
    RF_Transmitter.setModeTx();
    if ( !RF_Transmitter.init() ) {
         Serial.println("RadioHead init failed");
//    } else {
//        Serial.print("ASK RF Transmitter configured on pin ");
//		Serial.println(ASK_TX_PIN);

//		Serial.print("ASK RF configured at Baud ");
//		Serial.println(ASK_BAUD);
    }

}


int main(void)
{
	init();
	setup();

    /// runtime stacks
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
	    status_msg.number = frame;

		/// Read in sensor values
		int DHT11_chk = DHT.read11(DHT11_PIN);
		switch (DHT11_chk)
		{
			case DHTLIB_OK:
			{
				// DISPLAY DATA
				Serial.print("DHT11 Measurments : ");
				Serial.print((double)DHT.humidity, 2);
				Serial.print("RH , ");
				Serial.print((double)DHT.temperature);
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

	    const char *msg = "Test Transmission";
	    RF_Transmitter.send((uint8_t *)msg, strlen(msg));
	    RF_Transmitter.waitPacketSent();

	    //RF_Transmitter.send((uint8_t *)status_msg, 9+strlen((const char*)status_msg));
	    //RF_Transmitter.send((uint8_t *)status_msg.container, 70);
	    //RF_Transmitter.waitPacketSent();
		//Serial.println(*status_msg);

		digitalWrite(13, LOW);
		delay (LOOP_DELAY);
	};


	return 0;
}

