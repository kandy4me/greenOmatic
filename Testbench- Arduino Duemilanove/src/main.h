#ifndef MAIN_H_
#define MAIN_H_


/// Structures
typedef struct _status_flags
{
	enum { INVALID = 0, VALID = 1, UNINSTALLED = 2, ERROR = 3} STATUS;
#ifdef PERIPHERAL_RTCC
	unsigned char time_valid     : 2;
#endif
#ifdef PERIPHERAL_DHT11
	unsigned char DHT11_valid    : 2;
#endif
#ifdef PERIPHERAL_BH1750
	unsigned char BH1750_valid   : 2;
#endif
#ifdef INTERFACE_ETHERNET
	unsigned char ethernet_valid : 2;
#endif

	void reset (void)
	{
#ifdef PERIPHERAL_RTCC
		//time_valid   = 0; // don't include RTC, as that is only determined once on start-up
#endif
#ifdef PERIPHERAL_DHT11
		DHT11_valid  = 0;
#endif
#ifdef PERIPHERAL_BH1750
		BH1750_valid = 0;
#endif
#ifdef INTERFACE_ETHERNET
		// don't include ethernet, as that is only determined once on start-up
#endif
	}
};


/// Function Definitions
int 	main			( void );
void 	setup			( void );
void 	CLI				( char *, const uint8_t);
#ifdef PERIPHERAL_RTCC
	void 	print2digits	( int );  // used by DS1307 lib, TODO: get rid of ASAP
#endif


/// Object Definitions
volatile extern Plant plant_0();
volatile extern Plant plant_1();


#endif /* MAIN_H_ */
