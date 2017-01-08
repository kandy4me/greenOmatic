#ifndef ARDUINO_CONFIG_H_
#define ARDUINO_CONFIG_H_


//// Hardware Defines
/// Arduino Model
#define ARDUINO_MEGA
//#define ARDUINO_DUEMILANOVE
//#define ARDUINO_MINI_PRO

/// Arduino Shields
#define SHIELD_WIZNET

/// Communication Interfaces
//  NOTE: Ethernet has its own section below
// Serial
#define BAUD_SERIAL_TERMINAL     115200
// One-Wire RF
#define BAUD_ASK_RF              2400
#define PIN_ASK_RX               8
#define PIN_ASK_TX               9
// NRF905
#define PIN_SPISEL_NRF905        6

/// Ethernet Configuration
// Hardware properties. NOTE: This does NOT need to be populated if a shield is used
uint8_t mac[] = { 0xEE, 0x69, 0xF7, 0x55, 0x00, 0x01 };  // comment-out to use device MAC
IPAddress IPaddr(128, 169, 0, 13);  // comment-out to use dynamic IP
// Ethernet Client/Servers
#define ETHERNET_WEBSERVER
#define ETHERNET_UDPCLIENT
#define udpLocalPort 8888 // comment-out to to not use UDP

/// Sensors
// I2C
#define I2C_ADDR_BH1750       0xFF
#define I2C_ADDR_RTC          0xFF
// GPIO
#define PIN_DHT11             A8
#define PIN_DHT22             A9
#define PIN_FLOW_METER        A7
// Analog
#define PIN_MOISTURE_SENSOR   A1

/// Actuators
// Water pumps or sprays
#define PIN_WATER_PUMP        A5
// Fluid-line valves (if sharing a common pump)
#define PIN_VALVE_0           A6


/// TUNING PARAMETERS
#define LOOP_DELAY  333 //ms


#endif
