#ifndef RADAR_PROCESSING_H
#define RADAR_PROCESSING_H

#include "sensor.h"
#include "radar_display.h"


void radar_processing_init(void);
void radar_process_sensor_event(const struct sensor_event *evt);
int32_t compute_speed_kmh(uint32_t distance_mm, uint32_t delta_ms);
int compute_vehicle_status(struct radar_display_data *disp);
int init_context(const struct sensor_event *evt);
int add_axle();
int add_sensor2(const struct sensor_event *evt);

#endif /* RADAR_PROCESSING_H */
