#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "sensor.h"
#include "radar_processing.h"
#include "radar_display.h"

LOG_MODULE_REGISTER(radar, LOG_LEVEL_INF);

int main(void)
{
	int ret;

	LOG_INF("Iniciando o radar...");

	ret = sensor_init();
	if (ret < 0) {
		LOG_ERR("Erro ao iniciar sensor (err %d)", ret);
		return ret;
	}
	radar_processing_init();
	radar_display_init();

	LOG_INF("Radar inicializado.");
	return 0;
}
