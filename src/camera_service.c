#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/random/random.h>

#include "radar_bus.h"
#include "plate.h"
#include "camera_service.h"

LOG_MODULE_DECLARE(radar);

ZBUS_MSG_SUBSCRIBER_DEFINE(msub_radar_infraction);

ZBUS_CHAN_ADD_OBS(chan_radar_infraction, msub_radar_infraction, 3);

//Função para definir se vai emitir uma placa errada ou não
static bool should_fail_capture(void)
{
	const uint32_t rate = (uint32_t)CONFIG_RADAR_CAMERA_FAILURE_RATE_PERCENT;

	if (rate == 0U) {
		return false;
	}
	if (rate >= 100U) {
		return true;
	}

	const uint32_t pct = sys_rand32_get() % 100U;
	return pct < rate;
}

static void camera_thread(void *a, void *b, void *c)
{
	ARG_UNUSED(a);
	ARG_UNUSED(b);
	ARG_UNUSED(c);

	LOG_INF("Iniciando câmera");

	while (1) {
		int err;
		const struct zbus_channel *chan;
		struct radar_infraction_msg inf = {0};

		err = zbus_sub_wait_msg(&msub_radar_infraction, &chan, &inf, K_FOREVER);
		if (err) {
			LOG_ERR("Falha ao receber mensagem do canal: %d", err);
			continue;
		}

		//Simula processamento de câmera/LPR
		k_sleep(K_MSEC(250));

		struct camera_result_msg out = {0};
		out.infraction = inf;

		bool fail = should_fail_capture();
		if (fail) {
			plate_generate_invalid(out.plate);
		} else {
			plate_generate_mercosul(out.plate);
		}

		out.plate_valid = plate_is_valid_mercosul(out.plate);

		// Publica resultado
		err = zbus_chan_pub(&chan_camera_result, &out, K_NO_WAIT);
		if (err) {
			LOG_ERR("zbus_chan_pub(camera_result) failed: %d", err);
			continue;
		}

		LOG_INF("Camera result: plate=%s valid=%d (speed=%d limit=%d type=%d)", out.plate,
			(int)out.plate_valid, out.infraction.speed_kmh, out.infraction.limit_kmh,
			(int)out.infraction.vehicle_type);
	}
}

K_THREAD_DEFINE(camera_thread_id, 1024, camera_thread, NULL, NULL, NULL, 7, 0, 0);
