#ifndef RADAR_DISPLAY_H
#define RADAR_DISPLAY_H

#include "radar_control.h"
#include <stdint.h>
#include <stdbool.h>

struct radar_display_data {
    enum vehicle_type type;
    uint8_t axle_count;
    uint32_t delta_ms;
    int32_t measured_speed_kmh;
    int32_t limit_kmh;
    enum radar_infraction_status status;
};

int radar_display_init(void);
void radar_display_show(const struct radar_display_data *data);

#endif /* RADAR_DISPLAY_H */
