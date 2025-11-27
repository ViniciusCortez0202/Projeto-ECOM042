#include <zephyr/ztest.h>
#include "radar_display.h"

ZTEST(app_tests, test_str_type_light)
{
    zassert_equal(vehicle_type_str(VEHICLE_TYPE_LIGHT), "LEVE", NULL);
}

ZTEST(app_tests, test_str_type_heavy)
{
    zassert_equal(vehicle_type_str(VEHICLE_TYPE_HEAVY), "PESADO", NULL);
}

ZTEST(app_tests, test_str_status_normal)
{
    zassert_equal(status_str(RADAR_STATUS_NORMAL), "NORMAL", NULL);
}

ZTEST(app_tests, test_str_status_warning)
{
    zassert_equal(status_str(RADAR_STATUS_WARNING), "ATENÇÃO", NULL);
}

ZTEST(app_tests, test_str_status_infraction)
{
    zassert_equal(status_str(RADAR_STATUS_INFRACTION), "INFRAÇÃO", NULL);
}

ZTEST(app_tests, test_str_status_unknown)
{
    zassert_equal(status_str(10), "DESCONHECIDO", NULL);
}
