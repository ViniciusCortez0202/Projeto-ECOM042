#ifndef RADAR_PROCESSING_H
#define RADAR_PROCESSING_H

#include "sensor.h"


void radar_process_sensor_event(const struct sensor_event *evt);
void radar_processing_init(void);

#endif /* RADAR_PROCESSING_H */
