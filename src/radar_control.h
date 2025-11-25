#ifndef RADAR_CONTROL_H
#define RADAR_CONTROL_H

#include <stdint.h>

enum vehicle_type {
    VEHICLE_TYPE_LIGHT = 0,
    VEHICLE_TYPE_HEAVY = 1,
};

enum radar_infraction_status {
    RADAR_STATUS_NORMAL = 0,
    RADAR_STATUS_WARNING = 1,
    RADAR_STATUS_INFRACTION = 2,
};

struct radar_evaluation_result {
    enum vehicle_type type;
    int32_t measured_speed_kmh;
    int32_t limit_kmh;
    enum radar_infraction_status status;
};


struct radar_evaluation_result radar_evaluate_speed(enum vehicle_type type,
                                                    int32_t measured_speed_kmh);

#endif /* RADAR_CONTROL_H */
