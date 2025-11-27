#include <zephyr/ztest.h>
#include "radar_display.h"

ZTEST(app_tests, test_str_type_light)
{
    zassert_equal(vehicle_type_str(VEHICLE_TYPE_LIGHT), "LEVE", "shoudl be LEVE");
}

ZTEST(app_tests, test_str_type_heavy)
{
    zassert_equal(vehicle_type_str(VEHICLE_TYPE_HEAVY), "PESADO", "shoudl be PESADO");
}

ZTEST(app_tests, test_str_status_normal)
{
    zassert_equal(status_str(RADAR_STATUS_NORMAL), "NORMAL", "shoudl be Normal");
}

ZTEST(app_tests, test_str_status_warning)
{
    zassert_equal(status_str(RADAR_STATUS_WARNING), "ATENÇÃO", "shoudl be Atenção");
}

ZTEST(app_tests, test_str_status_infraction)
{
    zassert_equal(status_str(RADAR_STATUS_INFRACTION), "INFRAÇÃO", "shoudl be Infração");
}

ZTEST(app_tests, test_str_status_unknown)
{
    zassert_equal(status_str(10), "DESCONHECIDO", "shoudl be Desconhecido");
}
