#include "main.h"

#define DHT11_PIN 36
#define DHT22_PIN 25


int PUT(void)
{
	return 0;
}

void setup(void)
{
	Serial.begin(115200);
	Serial.println("Starting up Green-O-Matic Testbed");

	Serial.print("Using DHT Library version ");
	Serial.println(DHT_LIB_VERSION);

	pinMode(13, OUTPUT);
/* no need to actually configure them ahead of time !
#ifdef DHT11_PIN
	pinMode(3, DHT11_PIN);
#endif
#ifdef DHT21_PIN
	pinMode(3, DHT21_PIN);
#endif
*/

}


int main(void)
{
	init();
	setup();
    int ret_code = PUT();

	for( unsigned long int frame=0;;frame++)
	{
		digitalWrite(13, HIGH);
		Serial.write("\nLoop #");
		Serial.println(frame);

		// READ DATA



#ifdef DHT11_PIN
		Serial.print("DHT11 Status = ");
		int DHT11_chk = DHT.read11(DHT11_PIN);
		switch (DHT11_chk)
		{
		case DHTLIB_OK:
		{
			Serial.print("OK!");

			// DISPLAY DATA
			Serial.print("\tMeasurments : ");
			Serial.print(DHT.humidity);
			Serial.print("RH , ");
			Serial.print(DHT.temperature);
			Serial.println("decC");
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

#ifdef DHT22_PIN
		Serial.print("DHT22 Status = ");
		int DHT22_chk = DHT.read22(DHT22_PIN);
		switch (DHT22_chk)
		{
		case DHTLIB_OK:
		{
			Serial.print("OK!");

			// DISPLAY DATA
			Serial.print("\tMeasurments : ");
			Serial.print(DHT.humidity);
			Serial.print("RH , ");
			Serial.print(DHT.temperature);
			Serial.println("decC");
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
		digitalWrite(13, LOW);
		delay(2000);
	};


	return 0;
}

