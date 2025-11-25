#include "radar_control.h"

/* Define se velocidade do veiculo esta dentro ou fora do limite estabelecido:
 *  - CONFIG_RADAR_SPEED_LIMIT_LIGHT_KMH
 *  - CONFIG_RADAR_SPEED_LIMIT_HEAVY_KMH
 *  - CONFIG_RADAR_WARNING_THRESHOLD_PERCENT
 */
struct radar_evaluation_result radar_evaluate_speed(enum vehicle_type type,
                                                    int32_t measured_speed_kmh)
{
    struct radar_evaluation_result res = {
        .type = type,
        .measured_speed_kmh = measured_speed_kmh,
        .limit_kmh = 0,
        .status = RADAR_STATUS_NORMAL,
    };

    if (type == VEHICLE_TYPE_LIGHT) {
        res.limit_kmh = CONFIG_RADAR_SPEED_LIMIT_LIGHT_KMH;
    } else {
        res.limit_kmh = CONFIG_RADAR_SPEED_LIMIT_HEAVY_KMH;
    }

    if (res.limit_kmh <= 0) {
        res.status = RADAR_STATUS_NORMAL;
        return res;
    }
   
    int32_t warning_speed =
        (res.limit_kmh * CONFIG_RADAR_WARNING_THRESHOLD_PERCENT) / 100;

    if (measured_speed_kmh > res.limit_kmh) {
        res.status = RADAR_STATUS_INFRACTION;
    } else if (measured_speed_kmh >= warning_speed) {
        res.status = RADAR_STATUS_WARNING;
    } else {
        res.status = RADAR_STATUS_NORMAL;
    }

    return res;
}
