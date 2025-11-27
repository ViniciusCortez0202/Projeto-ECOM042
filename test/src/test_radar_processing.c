#include <zephyr/ztest.h>
#include "radar_processing.h"
#include "radar_display.h"
#include "sensor.h"


ZTEST(app_tests, test_computed_speed_normal)
{
    int32_t speed = compute_speed_kmh(1000, 120);

    zassert_equal(speed, 30, NULL);
}

ZTEST(app_tests, test_computed_speed_attention)
{
    int32_t speed = compute_speed_kmh(1000, 40);

    zassert_equal(speed, 90, NULL);
}

ZTEST(app_tests, test_computed_speed_infraction)
{
    int32_t speed = compute_speed_kmh(1000, 20);

    zassert_equal(speed, 180, NULL);
}

ZTEST(app_tests, test_computed_vehicle_type_light)
{
    struct sensor_event evt = {
        .sensor_id = 1,
        .level = 1,
        .timestamp_ms = k_uptime_get_32(),
    };

    init_context(&evt);
    add_axle();

    evt.sensor_id = 2;
    evt.level = 1;
    evt.timestamp_ms += 120;

    register_timestamp_sensor2(&evt);

    struct radar_display_data disp;

	compute_vehicle_status(&disp);

    zassert_equal(disp.type, VEHICLE_TYPE_LIGHT, NULL);
}

ZTEST(app_tests, test_computed_vehicle_type_heavy)
{
    struct sensor_event evt = {
        .sensor_id = 1,
        .level = 1,
        .timestamp_ms = k_uptime_get_32(),
    };

    init_context(&evt);
    add_axle();
    add_axle();

    evt.sensor_id = 2;
    evt.level = 1;
    evt.timestamp_ms += 120;

    register_timestamp_sensor2(&evt);

    struct radar_display_data disp;

	compute_vehicle_status(&disp);

    zassert_equal(disp.type, VEHICLE_TYPE_HEAVY, NULL);
}

ZTEST(app_tests, test_computed_vehicle_status_normal_to_light_vehicle)
{
    struct sensor_event evt = {
        .sensor_id = 1,
        .level = 1,
        .timestamp_ms = k_uptime_get_32(),
    };

    init_context(&evt);
    add_axle();

    evt.sensor_id = 2;
    evt.level = 1;
    evt.timestamp_ms += 120;

    register_timestamp_sensor2(&evt);

    struct radar_display_data disp;

	compute_vehicle_status(&disp);

    zassert_equal(disp.status, RADAR_STATUS_NORMAL, NULL);
}

ZTEST(app_tests, test_computed_vehicle_status_attention_to_light_vehicle)
{
    struct sensor_event evt = {
        .sensor_id = 1,
        .level = 1,
        .timestamp_ms = k_uptime_get_32(),
    };

    init_context(&evt);
    add_axle();

    evt.sensor_id = 2;
    evt.level = 1;
    evt.timestamp_ms += 50;

    register_timestamp_sensor2(&evt);
    
    struct radar_display_data disp;
    
	compute_vehicle_status(&disp);

    zassert_equal(disp.status, RADAR_STATUS_WARNING, NULL);
}

ZTEST(app_tests, test_computed_vehicle_status_infraction_to_light_vehicle)
{
    struct sensor_event evt = {
        .sensor_id = 1,
        .level = 1,
        .timestamp_ms = k_uptime_get_32(),
    };

    init_context(&evt);
    add_axle();

    evt.sensor_id = 2;
    evt.level = 1;
    evt.timestamp_ms += 20;

    register_timestamp_sensor2(&evt);

    struct radar_display_data disp;

	compute_vehicle_status(&disp);

    zassert_equal(disp.status, RADAR_STATUS_INFRACTION, NULL);
}

ZTEST(app_tests, test_computed_vehicle_status_normal_to_heavy_vehicle)
{
    struct sensor_event evt = {
        .sensor_id = 1,
        .level = 1,
        .timestamp_ms = k_uptime_get_32(),
    };

    init_context(&evt);
    add_axle();
    add_axle();

    evt.sensor_id = 2;
    evt.level = 1;
    evt.timestamp_ms += 120;

    register_timestamp_sensor2(&evt);

    struct radar_display_data disp;

	compute_vehicle_status(&disp);

    zassert_equal(disp.status, RADAR_STATUS_NORMAL, NULL);
}

ZTEST(app_tests, test_computed_vehicle_status_attention_to_heavy_vehicle)
{
    struct sensor_event evt = {
        .sensor_id = 1,
        .level = 1,
        .timestamp_ms = k_uptime_get_32(),
    };

    init_context(&evt);
    add_axle();
    add_axle();

    evt.sensor_id = 2;
    evt.level = 1;
    evt.timestamp_ms += 60;

    register_timestamp_sensor2(&evt);
    
    struct radar_display_data disp;
    
	compute_vehicle_status(&disp);

    zassert_equal(disp.status, RADAR_STATUS_WARNING, NULL);
}

ZTEST(app_tests, test_computed_vehicle_status_infraction_to_heavy_vehicle)
{
    struct sensor_event evt = {
        .sensor_id = 1,
        .level = 1,
        .timestamp_ms = k_uptime_get_32(),
    };

    init_context(&evt);
    add_axle();
    add_axle();

    evt.sensor_id = 2;
    evt.level = 1;
    evt.timestamp_ms += 20;

    register_timestamp_sensor2(&evt);

    struct radar_display_data disp;

	compute_vehicle_status(&disp);

    zassert_equal(disp.status, RADAR_STATUS_INFRACTION, NULL);
}
