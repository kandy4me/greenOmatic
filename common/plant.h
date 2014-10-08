#ifndef PLANT_H_
#define PLANT_H_

#include "sensors.h"
#include "actuators.h"

class Plant {
public:
	Plant();
	virtual ~Plant();
	virtual int IBIT(void); // test all sensors & actuators connected to this plant


	void update_sensors(void);


private:
	int config_sensors(int, int, int);

};

#endif /* PLANT_H_ */

