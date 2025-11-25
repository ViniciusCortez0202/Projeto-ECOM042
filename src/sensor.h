#ifndef RADAR_SENSOR_H
#define RADAR_SENSOR_H

#include <stdint.h>

struct sensor_event {
    uint8_t sensor_id;     // 1 or 2
    uint8_t level;         // 0 or 1
    uint32_t timestamp_ms; // k_uptime_get_32()
};

int sensor_init(void);


void sensor_send_event(uint8_t sensor_id, uint8_t level);

#endif /* RADAR_SENSOR_H */
