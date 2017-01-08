#include "main.h"


///// STACK OBJECTS /////
_status_flags 	STATUS;
char 			broadcast_msg[512] = {0};
unsigned int 	broadcast_msg_length = 0;

dht 			DHT;
RH_ASK 			RF_Transmitter( ASK_BAUD, ASK_RX_PIN, ASK_TX_PIN, 10, false); // (uint16_t speed, uint8_t rxPin, uint8_t txPin, uint8_t pttPin, bool pttInverted);
RH_NRF905 		RF_Transceiver;



///// FUNCTIONS /////
int BroadcastUpdate (char *sub_msg, const unsigned int length)
{
	int return_code = sprintf( &(broadcast_msg[broadcast_msg_length]), sub_msg);

	if (static_cast<int>(length) != return_code)
	{
		Serial.println("Failed to update broadcast message!");
	}
	else if (0 >= return_code)
	{
		Serial.println("Something went wrong appending to Broadcast message, return code " + return_code);
	}
	else if (0 < return_code)
	{
		broadcast_msg_length += return_code;
	}

	return return_code;
}


void Broadcast(char *message = broadcast_msg, int length = broadcast_msg_length)
//void broadcast(char *message, unsigned int length)
{
	if(RH_ASK_MAX_MESSAGE_LEN < length)
	{
		{
		String tooLargeString = "Broadcast message too long, splitting into sub-messages.\n";
		Serial.println(tooLargeString);
		ASK_Send(tooLargeString);
		}

		// Use a pointer to track the start of each sub-string within the original 'message'
		char *subMessage = message;
		for(int remaining = length; 0 < remaining; )
		{
			// If the remaining message length is smaller than the maximum peritted message length,
			// broadcast the final subMessage
			if (RH_ASK_MAX_MESSAGE_LEN > remaining)
			{
#ifdef INTERFACE_ASK_TX
				ASK_Send(subMessage, remaining);
#endif
#ifdef INTERFACE_NRF905
				NRF_Send(subMessage, remaining);
#endif
				Serial.println(subMessage);

				subMessage += remaining; // pointer should now be at the end of the string
				remaining = 0;
			}

			// If the remaining message length is larger-than or equal-to the maximum peritted
			//  message length, broadcast as much as we're allowed, and update 'subMessage' and
			// 'remaining'
			else if(0 <= (remaining - RH_ASK_MAX_MESSAGE_LEN))
			{
				char subChar[RH_ASK_MAX_MESSAGE_LEN+2];

				int snprintfReturnCode = snprintf(subChar, RH_ASK_MAX_MESSAGE_LEN, subMessage);

				if (RH_ASK_MAX_MESSAGE_LEN != snprintfReturnCode)
				{
					Serial.println("Critical error creating sub-string for broadcast, return code = " + snprintfReturnCode);
				}

#ifdef INTERFACE_ASK_TX
				ASK_Send(subChar, RH_ASK_MAX_MESSAGE_LEN);
#endif
#ifdef INTERFACE_NRF905
				NRF_Send(subChar, RH_ASK_MAX_MESSAGE_LEN);
#endif
				Serial.print(subChar);

				subMessage += RH_ASK_MAX_MESSAGE_LEN;
				remaining -= RH_ASK_MAX_MESSAGE_LEN;
			}
			else
			{
				Serial.print("ERROR: unknown remaining");
			}
			delay (100U);
		}
	}

	else
	{
		#ifdef INTERFACE_ASK_TX
			ASK_Send(message, length);
			ASK_Send((String)message);
		#endif

		#ifdef INTERFACE_NRF905
			NRF_Send(message, length);
		#endif

		Serial.println("Here is the CHAR broadcast message : ");
	    Serial.println( message );
	    Serial.println();
	}
}


void Broadcast(String message)
{
	Broadcast(const_cast<char *>(message.c_str()), message.length());
}


#ifdef INTERFACE_ASK_TX
int ASK_Send (String message)
{
	const char header1[] = "TestHeader1";
    RF_Transmitter.send( (uint8_t *)header1, 11 );
    RF_Transmitter.waitPacketSent();
    delay(100);

    char Broadcast_Buffer[RH_ASK_MAX_MESSAGE_LEN] = {0};
    message.toCharArray( Broadcast_Buffer, message.length() );
    RF_Transmitter.send( (uint8_t *)Broadcast_Buffer, message.length() );
    RF_Transmitter.waitPacketSent();
    delay(150);

	String header2 = "TestHeader2";
    RF_Transmitter.send( (const uint8_t*)header2.c_str(), header2.length() );
    RF_Transmitter.waitPacketSent();
    delay(100);

    RF_Transmitter.send( (uint8_t *)message.c_str(), message.length() );
    RF_Transmitter.waitPacketSent();
    delay(150);

    return 0;
}


int ASK_Send (char *message, unsigned int length)
{
//  char Broadcast_Buffer[RH_ASK_MAX_MESSAGE_LEN] = {0};
//  message.toCharArray( Broadcast_Buffer, message.length() );

    RF_Transmitter.send( (uint8_t *)message, length );
    RF_Transmitter.waitPacketSent();

    return 0;
}
#endif


#ifdef INTERFACE_NRF905
int NRF_Send (String message)
{
	return 0;
}
#endif


void Setup(void)
{
	Serial.begin(115200);

	Serial.println("Starting up Green-O-Matic Mega Testbed");
	Serial.print("Program compiled on ");
	Serial.print(__DATE__);
	Serial.print(" at ");
	Serial.println(__TIME__);
	Serial.println();

	// On-board LED
	pinMode(13, OUTPUT);

	/// Sensors
	// Analog
	Serial.print("Analog sensor configured on pin");
	Serial.println(Soil_Moisture_0_Pin, DEC);

	// DHT11
#ifdef PERIPHERAL_DHT11
	Serial.print("Using DHT Library version ");
	Serial.println(DHT_LIB_VERSION);
    Serial.print("DHT11 Sensor configured on pin ");
    Serial.println(DHT11_PIN, DEC);
#endif

    // Light Intensity Meter
#ifdef PERIPHERAL_BH1750
    Serial.print("I2C Light Intensity Meter at address 0x");
    Serial.println(BH1750_ADDR, HEX);
#endif

    /// Actuators
#ifdef PERIPHERAL_RELAY
    Serial.print("Discrete Relay configured on pin ");
    Serial.println(Relay0_Pin);
#endif

    /// RF Setup
#ifdef INTERFACE_ASK_TX
    RF_Transmitter.setModeTx();
    if ( !RF_Transmitter.init() ) {
         Serial.println("RadioHead init failed");
    } else {
    	Serial.print("433MHz ASK RF Transmitter configured on pin ");
    	Serial.print(ASK_TX_PIN);
    	Serial.print(" at ");
    	Serial.print(ASK_BAUD);
    	Serial.println(" baud");
    }
#endif

#ifdef INTERFACE_NRF905
    if (!RF_Transceiver.init())
        Serial.println("NRF905 init failed!");
#endif

    // Send a startup message to all RF nodes
	String startupMessage = "greenOmatic Mega Testbench Startup";
	Broadcast(startupMessage);

    Serial.println();
    Serial.println();
}



int DhtValidity(int ret_code)
{
	switch (ret_code)
	{
		case DHTLIB_OK:
		{
			return 1;
			break;
		}
		case DHTLIB_ERROR_CHECKSUM:
		case DHTLIB_ERROR_TIMEOUT:
		default:
		{
			return 0;
			break;
		}
	}
}


int Dht11Update (int pin)
{
	// DHT sensor(s)
	int DHT11_chk = DHT.read11(pin);
	STATUS.DHT11_valid = DhtValidity(DHT11_chk);

	// TODO: error checking
	return 0;
	
	/*
	// DHT sensor(s)
		int DHT11_chk = DHT.read11(DHT11_PIN);
		switch (DHT11_chk)
		{
			case DHTLIB_OK:
			{
				char humidity_chars[5];
				char temperature_chars[5];

				dtostrf(DHT.humidity, 5, 2, humidity_chars);
				dtostrf(DHT.temperature, 5, 2, temperature_chars);

				String DHT_msg = "DHT11 : ";
				DHT_msg.concat(humidity_chars);
				DHT_msg.concat("RH, ");
				DHT_msg.concat(temperature_chars);
				DHT_msg.concat("degC");

				broadcast(DHT_msg);

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
	*/
}


uint16_t Bht1750Read (int addr)
{
    Wire.beginTransmission(addr);
    Wire.write(0x11); // Continuously H-Resolution Mode2
    Wire.endTransmission();
    delay(150);

    Wire.beginTransmission(addr);
    Wire.requestFrom(addr, 2);

    byte i2cBuff[8];
    unsigned short int BHT_bytes = 0;

    while(Wire.available())
    {
    	i2cBuff[BHT_bytes] = Wire.read();  // receive one byte
      BHT_bytes++;
    }
    Wire.endTransmission();

    uint16_t returnVal = 0;
    switch(BHT_bytes)
    {
		case 2:
		{
			returnVal=((i2cBuff[0]<<8)|i2cBuff[1])/1.2/2.0;
	    	STATUS.BH1750_valid = 1;
			break;
		}
		default:
		{
	    	Serial.println("Error reading BH1750!");
	    	STATUS.BH1750_valid = 0;
	    	returnVal = 0xFFFF;
			break;
		}
    }

	return returnVal;
}


void RelayCycleOn (int pin, uint16_t time_ms)
{
	Relay_ON(pin);
	delay(time_ms);
	Relay_OFF(pin);
}



/// Main Program
int main(void)
{
	init();
	Setup();

    /// loop control
	for( unsigned long int frame=0;;frame++)
	{
		digitalWrite(13, HIGH);
		STATUS.reset();
		broadcast_msg_length = 0;
		char temp_msg[64];

		unsigned int temp_length = sprintf(temp_msg, "GreenOmatic Sensor Node Loop #%u\n", frame);
	    BroadcastUpdate(temp_msg, temp_length);

		/// Read in sensor values
	    // Soil Moisture
	    unsigned int Soil_Moisture_0_Reading = analogRead(Soil_Moisture_0_Pin);    // read the input pin

//	    String STR_analog = ("ADC reading : ");
//	    STR_analog.concat(Soil_Moisture_0_Reading);
//	    broadcast_update((char*)STR_analog.c_str(), STR_analog.length());

	    char ADC_msg[16];
	    unsigned int ADC_length = sprintf(ADC_msg, "ADC Reading : %u\n", Soil_Moisture_0_Reading);
	    BroadcastUpdate(ADC_msg, ADC_length);


	    // BH1750 Light Intensity
	    uint16_t Light_Reading = Bht1750Read(BH1750_ADDR);
	    if ( STATUS.BH1750_valid )
	    {
//			String STR_BH1750("Light reading: ");
//		    STR_BH1750.concat(Light_Reading);
//		    broadcast_update( (char*)STR_BH1750.c_str(), STR_BH1750.length() );

			char light_msg[20];
			unsigned int light_length = sprintf(light_msg, "Light Reading : %u\n", Light_Reading);
		    BroadcastUpdate( light_msg, light_length );
	    }

	    // DHT sensor(s)
	    Dht11Update(DHT11_PIN);
		if ( STATUS.DHT11_valid )
		{
			// String does not support Float; must first convert floats to char array
			char humid_chars[8], temp_chars[8];
			dtostrf(DHT.humidity,3,1,humid_chars);
			dtostrf(DHT.temperature,3,1,temp_chars);

			char DHT_Msg[40];
			int len = sprintf(DHT_Msg, "DHT11 Measurments : %sRH, %sdegC\n", humid_chars, temp_chars );
		    BroadcastUpdate( DHT_Msg, len );
		}

		Broadcast();

		digitalWrite(13, LOW);

		delay (LOOP_DELAY);
	};

	return 0;
}
