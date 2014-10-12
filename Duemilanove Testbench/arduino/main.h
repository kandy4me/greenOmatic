#ifndef MAIN_H_
#define MAIN_H_

#include "arduino.h"

/*****MAIN FUNCTIONS*****/
void CLI		(char *, uint8_t);
void check_msg	();
void RC_mode	() 	{}

void GPS_send   ();

void setup		();
int  main		();



#endif /* MAIN_H_ */
