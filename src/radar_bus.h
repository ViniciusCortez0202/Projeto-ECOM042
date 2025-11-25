#ifndef RADAR_BUS_H
#define RADAR_BUS_H

#include <stdint.h>
#include <stdbool.h>
#include <zephyr/zbus/zbus.h>

#include "radar_control.h"

struct radar_infraction_msg {
	enum vehicle_type vehicle_type;
	int32_t speed_kmh;
	int32_t limit_kmh;
	uint32_t timestamp_ms;
};

struct camera_result_msg {
	struct radar_infraction_msg infraction;
	char plate[8];
	bool plate_valid;
};

ZBUS_CHAN_DECLARE(chan_radar_infraction);
ZBUS_CHAN_DECLARE(chan_camera_result);

#endif /* RADAR_BUS_H */
