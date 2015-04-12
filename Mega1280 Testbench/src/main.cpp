#include "main.h"

///// OBJECTS /////
_status_flags 	STATUS;
dht 			DHT;
RH_ASK 			RF_Transmitter( ASK_BAUD, ASK_RX_PIN, ASK_TX_PIN, 10, false); // (uint16_t speed, uint8_t rxPin, uint8_t txPin, uint8_t pttPin, bool pttInverted);
RH_NRF905 		RF_Transceiver;

volatile Plant plant_0();
volatile Plant plant_1();

// stack
char broadcast_msg[RH_ASK_MAX_MESSAGE_LEN] = {0};


///// FUNCTIONS /////
void setup(void)
{
	Serial.begin(115200);
	Serial.println("Starting up Green-O-Matic Mega Testbed");
	
	
	Serial.print("Program compiled on ");
	Serial.print(__DATE__);
	Serial.print(" at ");
	Serial.println(__TIME__);
	Serial.println();


	// on-board LED
	pinMode(13, OUTPUT);


	/// Sensors
	// Analog
	Serial.print("Analog sensor configured on pin");
	Serial.println(Soil_Moisture_0_Pin, DEC);

	// DHT11
	Serial.print("Using DHT Library version ");
	Serial.println(DHT_LIB_VERSION);
    Serial.print("DHT11 Sensor configured on pin ");
    Serial.println(DHT11_PIN, DEC);

    // Light Intensity Meter
    Serial.print("I2C Light Intensity Meter at address 0x");
    Serial.println(BH1750_ADDR, HEX);


    /// Actuators
    Serial.print("Discrete Relay configured on pin ");
    Serial.println(Relay0_Pin);

    /// RF Setup
    RF_Transmitter.setModeTx();
    if ( !RF_Transmitter.init() ) {
         Serial.println("RadioHead init failed");
    } else {
    	Serial.print("433MHz ASK RF Transmitter configured on pin ");
    	Serial.print(ASK_TX_PIN);
    	Serial.print(" at ");
    	Serial.print(ASK_BAUD);
    	Serial.println(" baud");

		const char init_msg[] = "greenOmatic Duemilanove Startup";
	    RF_Transmitter.send((uint8_t *)init_msg, strlen(init_msg));
	    RF_Transmitter.waitPacketSent();
    }

    // NRF-905
    if (!RF_Transceiver.init())
        Serial.println("NRF905 init failed!");

    Serial.println();
}

void broadcast(String message)
{
	message.toCharArray( broadcast_msg, message.length() );

	// be productive with our time: begin RF broadcast first,
	// then everything else BEFORE waitPacketSent()
    RF_Transmitter.send( (uint8_t *)broadcast_msg, message.length() );
    Serial.println( broadcast_msg );

    RF_Transmitter.waitPacketSent();

	/*
	const char header_msg[] = "Frame ";
	RF_Transmitter.send((uint8_t *)msg, strlen(msg));
	RF_Transmitter.waitPacketSent();
	 */
	//RF_Transmitter.send((uint8_t *)status_msg, 9+strlen((const char*)status_msg));
	//RF_Transmitter.send((uint8_t *)status_msg.container, 70);
	//RF_Transmitter.waitPacketSent();
	//Serial.println(*status_msg);
}

int main(void)
{
	init();
	setup();

    /// loop control
	for( unsigned long int frame=0;;frame++)
	{
		digitalWrite(13, HIGH);
		STATUS.reset();

		// Prepare broadcast message
	    String Broadcast_Message = "GreenOmatic Sensor Node Loop #";
	    Broadcast_Message.concat(frame);
	    broadcast_update(Broadcast_Message);

		/// Read in sensor values
	    // Soil Moisture
	    unsigned int Soil_Moisture_0_Reading = analogRead(Soil_Moisture_0_Pin);    // read the input pin

	    String STR_analog = ("ADC reading : ");
	    STR_analog.concat(Soil_Moisture_0_Reading);
	    Broadcast_Message.concat(STR_analog);
	    Broadcast_Message.concat("\n");
	    Serial.println(STR_analog);

	    // BH1750 Light Intensity
	    uint16_t Light_Reading = Light_read(BH1750_ADDR);
	    if ( STATUS.BH1750_valid )
	    {
			String STR_BH1750("Light reading: ");
		    STR_BH1750.concat(Light_Reading);

		    Broadcast_Message.concat(STR_BH1750);
		    Broadcast_Message.concat("\n");
			Serial.println(STR_BH1750);
	    }

	    // DHT sensor(s)
	    DHT11_update(DHT11_PIN);
		if ( STATUS.DHT11_valid )
		{
			//<DEBUG>
			Serial.println(DHT.humidity);
			Serial.println(DHT.temperature);
			//</DEBUG>

			// String does not support Float; must first convert floats to char array
			char humid_chars[8];
			char temp_chars[8];
			sprintf(humid_chars, "%f", DHT.humidity);
			sprintf(temp_chars, "%f", DHT.temperature);

			//<DEBUG>
			Serial.println(humid_chars);
			Serial.println(temp_chars);
			//</DEBUG>

			String STR_DHT11("DHT11 Measurments : ");
			STR_DHT11.concat(humid_chars);
			STR_DHT11.concat("RH , ");
			STR_DHT11.concat(temp_chars);
			STR_DHT11.concat("degC");

		    Broadcast_Message.concat(STR_DHT11);
		    Broadcast_Message.concat("\n");
			Serial.println(STR_DHT11);
		}

		broadcast_Transmit(Broadcast_Message);

	    Serial.println();
		
		digitalWrite(13, LOW);

		delay (LOOP_DELAY);
	};

	return 0;
}


int DHT_validity(int ret_code)
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

int DHT11_update (int pin)
{
	// DHT sensor(s)
	int DHT11_chk = DHT.read11(pin);
	STATUS.DHT11_valid = DHT_validity(DHT11_chk);

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

uint16_t Light_read (int addr)
{
    Wire.beginTransmission(addr);
    Wire.write(0x11); // Continuously H-Resolution Mode2
    Wire.endTransmission();

    delay(150);

    byte buff[2];
    int BHT_bytes = 0;

    Wire.beginTransmission(addr);
    Wire.requestFrom(addr, 2);
    while(Wire.available()) //
    {
      buff[BHT_bytes] = Wire.read();  // receive one byte
      BHT_bytes++;
    }
    Wire.endTransmission();

    switch(BHT_bytes)
    {
		case 2:
		{
			uint16_t val=((buff[0]<<8)|buff[1])/1.2/2.0;
	    	STATUS.BH1750_valid = 1;
			return val;
			break;
		}
		default:
		{
	    	STATUS.BH1750_valid = 0;
	    	Serial.print("Error reading ");
	    	Serial.println(addr, HEX);
	    	return 0xFF;
			break;
		}
    }
	return 0xFF;
}

void Relay_Cycle_ON (int pin, uint16_t time_ms)
{
	digitalWrite(pin, HIGH);
	delay(time_ms);
	digitalWrite(pin, LOW);
}


int ASK_Send (String message)
{
    char Broadcast_Buffer[RH_ASK_MAX_MESSAGE_LEN] = {0};
    message.toCharArray(Broadcast_Buffer, RH_ASK_MAX_MESSAGE_LEN); // TODO, RH_ASK_MAX_MESSAGE_LEN instead of String Length?

    RF_Transmitter.send( (uint8_t *)Broadcast_Buffer, message.length() );
    RF_Transmitter.waitPacketSent();

    return 0;
}

int NRF_Send (String message)
{
	return 0;
}

