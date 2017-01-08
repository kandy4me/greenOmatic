#ifndef GREENOMATIC_PLANT_H_
#define GREENOMATIC_PLANT_H_

#include "arduio_includes.h"
#include "sensor.h"
#include "actuator.h"
#include "status.h"


class Plant {
public:
  Plant();
  virtual ~Plant();

  void update_sensors(void);
  int IBIT(void); // test all sensors & actuators connected to this plant

private:
  int config_sensors(int, int, int);

};


#endif /* GREENOMATIC_PLANT_H_ */
