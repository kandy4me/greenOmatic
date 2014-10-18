#ifndef PLANT_H_
#define PLANT_H_

#include "sensor.h"
#include "actuator.h"

class Plant {
public:
	Plant();
	virtual ~Plant();

	void update_sensors(void);
	int IBIT(void); // test all sensors & actuators connected to this plant

private:
	int config_sensors(int, int, int);

};

#endif /* PLANT_H_ */

