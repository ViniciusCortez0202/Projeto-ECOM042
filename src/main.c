#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "sensor.h"

LOG_MODULE_REGISTER(radar, LOG_LEVEL_INF);

int main(void)
{
    int ret;

    LOG_INF("Radar app starting...");

    ret = sensor_init();
    if (ret < 0) {
        LOG_ERR("Sensor init failed (err %d)", ret);
        return ret;
    }

    LOG_INF("Radar app initialized.");
    /* main thread can simply return; sensor threads keep running */
	return 0;
}
