#ifndef RADAR_SENSOR_H
#define RADAR_SENSOR_H

#include <stdint.h>

/**
 * Initialize sensor hardware (GPIO + interrupts).
 *
 * @return 0 on success, negative errno on failure.
 */
int sensor_init(void);

/**
 * Send a sensor event into the internal queue.
 *
 * This is used both by the real GPIO ISR and by the simulator.
 */
void sensor_send_event(uint8_t sensor_id, uint8_t level);

#endif /* RADAR_SENSOR_H */
