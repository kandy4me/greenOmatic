#include "main.h"

// Objects
dht 		DHT;
RH_ASK 		RF_Transmitter( ASK_BAUD, ASK_RX_PIN, ASK_TX_PIN, 10, false); // (uint16_t speed, uint8_t rxPin, uint8_t txPin, uint8_t pttPin, bool pttInverted);
RH_NRF905 	RF_Transceiver;



void setup(void)
{
	Serial.begin(115200);
	Serial.println("Starting up Green-O-Matic Mega Testbed");
	pinMode(13, OUTPUT);


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
    }


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
    Serial.print("Relay configured on pin ");
    Serial.println(Relay0_Pin);

    Serial.println();
}


int main(void)
{
	init();
	setup();

    /// runtime stacks


    /// loop control
	for( unsigned long int frame=0;;frame++)
	{
		digitalWrite(13, HIGH);

		/// update status
	    status_msg.number = frame;
	    String new_msg = "Loop #";
	    new_msg.concat(frame);


		/// Read in sensor values
	    // Soil Moisture
	    unsigned int val = analogRead(Soil_Moisture_0_Pin);    // read the input pin
	    Serial.print("soil moisture reading: ");
	    Serial.println(val, DEC);

	    // BH1750 Light Intensity
	    Wire.beginTransmission(BH1750_ADDR);
	    Wire.write(0x11); // Continuously H-Resolution Mode2
	    Wire.endTransmission();

	    delay(150);

	    byte buff[2];
	    int BHT_bytes = 0;

	    Wire.beginTransmission(BH1750_ADDR);
	    Wire.requestFrom(BH1750_ADDR, 2);
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
				Serial.print(val,DEC);
				Serial.println(" lux");
				break;
			}
			default:
			{
				Serial.println("Something went wrong with I2C reading...");
				break;
			}
	    }

	    // DHT sensor(s)
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

	    char *msg = "Test Transmission     ";
	    msg[18] = frame;
	    RF_Transmitter.send((uint8_t *)msg, strlen(msg));
	    RF_Transmitter.waitPacketSent();

	    //RF_Transmitter.send((uint8_t *)status_msg, 9+strlen((const char*)status_msg));
	    //RF_Transmitter.send((uint8_t *)status_msg.container, 70);
	    //RF_Transmitter.waitPacketSent();
		//Serial.println(*status_msg);

	    Serial.println();
		digitalWrite(13, LOW);
		delay (LOOP_DELAY);
	};


	return 0;
}

