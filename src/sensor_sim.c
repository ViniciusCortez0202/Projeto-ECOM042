#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "sensor.h"

LOG_MODULE_DECLARE(radar);

#define AXLE_GAP_MS            3
#define BETWEEN_VEHICLES_S     5
#define VEHICLE_END_SILENCE_MS 600

static void pulse_sensor(uint8_t sensor_id)
{
	sensor_send_event(sensor_id, 1);
}

static void emit_vehicle(uint8_t axles, uint32_t travel_ms)
{
	for (uint8_t i = 0; i < axles; i++) {
		pulse_sensor(1);
		k_msleep(travel_ms);
		pulse_sensor(2);
	}

	k_msleep(VEHICLE_END_SILENCE_MS);
}

static void sensor_sim_thread(void *a, void *b, void *c)
{
	ARG_UNUSED(a);
	ARG_UNUSED(b);
	ARG_UNUSED(c);

	while (1) {
		// Veiculo leve normal
		emit_vehicle(2, 120);

		K_MSEC(BETWEEN_VEHICLES_S);

		// Veiculo leve com atenção
		emit_vehicle(2, 40);

		K_MSEC(BETWEEN_VEHICLES_S);

		// Veiculo leve com infração
		emit_vehicle(2, 20);

		K_MSEC(BETWEEN_VEHICLES_S);

		// Veiculo pesado normal
		emit_vehicle(3, 120);

		K_MSEC(BETWEEN_VEHICLES_S);

		// Veiculo pesado  com atenção
		emit_vehicle(3, 50);

		K_MSEC(BETWEEN_VEHICLES_S);

		// Veiculo pesado  com infração
		emit_vehicle(3, 20);

		K_MSEC(BETWEEN_VEHICLES_S);
	}
}

K_THREAD_DEFINE(sensor_sim_thread_id, 1024, sensor_sim_thread, NULL, NULL, NULL, 6, 0, 0);
