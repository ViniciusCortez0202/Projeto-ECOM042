#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>

#include "radar_processing.h"
#include "radar_control.h"
#include "radar_display.h"
#include "radar_bus.h"

LOG_MODULE_DECLARE(radar);

enum vehicle_track_state {
	VEHICLE_STATE_IDLE = 0,
	VEHICLE_STATE_TRACKING = 1,
};

struct vehicle_context {
	enum vehicle_track_state state;
	uint32_t start_time_ms;
	uint32_t sensor2_time_ms;
	bool sensor2_seen;
	uint8_t axle_count;

	struct k_work_delayable finalize_work;
};

static struct vehicle_context ctx;

// Volta o estado para os valores iniciais
static void vehicle_context_reset(struct vehicle_context *c)
{
	c->state = VEHICLE_STATE_IDLE;
	c->axle_count = 0;
	c->start_time_ms = 0;
}

// Calcula a velocidade média
static int32_t compute_speed_kmh(uint32_t distance_mm, uint32_t delta_ms)
{
	if (delta_ms == 0) {
		return 0;
	}

	return (int32_t)((distance_mm * 36U) / (delta_ms * 10U));
}

// Publica no canal de que veiculo cometeu uma infração
static void publish_infraction(enum vehicle_type type, int32_t speed_kmh, int32_t limit_kmh)
{
	struct radar_infraction_msg msg = {
		.vehicle_type = type,
		.speed_kmh = speed_kmh,
		.limit_kmh = limit_kmh,
		.timestamp_ms = k_uptime_get_32(),
	};

	int err = zbus_chan_pub(&chan_radar_infraction, &msg, K_NO_WAIT);
	if (err) {
		LOG_ERR("Falha ao publicar no canal: %d", err);
	}
}

static void finalize_vehicle_work(struct k_work *work)
{
	ARG_UNUSED(work);

	/* Só finaliza se estiver acompanhando */
	if (ctx.state != VEHICLE_STATE_TRACKING) {
		return;
	}

	/* Se nunca chegou no sensor 2, não dá pra calcular velocidade */
	if (!ctx.sensor2_seen) {
		LOG_WRN("Possível ruido");
		vehicle_context_reset(&ctx);
		return;
	}

	uint32_t delta_ms = ctx.sensor2_time_ms - ctx.start_time_ms;
	uint32_t distance_mm = CONFIG_RADAR_SENSOR_DISTANCE_MM;
	int32_t speed_kmh = compute_speed_kmh(distance_mm, delta_ms);

	enum vehicle_type vtype;
	if (ctx.axle_count >= 3) {
		vtype = VEHICLE_TYPE_HEAVY;
	} else if (ctx.axle_count == 2) {
		vtype = VEHICLE_TYPE_LIGHT;
	} else {
		LOG_WRN("Possível ruido");
		vehicle_context_reset(&ctx);
		return;
	}

	struct radar_evaluation_result res = radar_evaluate_speed(vtype, speed_kmh);

	struct radar_display_data disp = {
		.type = vtype,
		.axle_count = ctx.axle_count,
		.delta_ms = delta_ms,
		.measured_speed_kmh = res.measured_speed_kmh,
		.limit_kmh = res.limit_kmh,
		.status = res.status,
	};

	radar_display_show(&disp);

	if (res.status == RADAR_STATUS_INFRACTION) {
		publish_infraction(vtype, speed_kmh, res.limit_kmh);
	}

	vehicle_context_reset(&ctx);
}

// Processa os estados de leitura, e emite as informações de velocidade e infração
void radar_process_sensor_event(const struct sensor_event *evt)
{
	if (!evt) {
		return;
	}

	switch (ctx.state) {
	case VEHICLE_STATE_IDLE:
		if (evt->sensor_id == 1 && evt->level == 1) {
			ctx.state = VEHICLE_STATE_TRACKING;
			ctx.axle_count = 1;
			ctx.start_time_ms = evt->timestamp_ms;
			ctx.sensor2_seen = false;

			/* agenda finalização para 500ms */
			k_work_reschedule(&ctx.finalize_work, K_MSEC(500));
		}
		break;

	case VEHICLE_STATE_TRACKING:
		/* Conta eixos somente no sensor 1 */
		if (evt->sensor_id == 1 && evt->level == 1) {
			ctx.axle_count++;

			/* reinicia janela de 500ms: ainda tem eixos chegando */
			k_work_reschedule(&ctx.finalize_work, K_MSEC(500));
		}

		/* Marca o PRIMEIRO momento que o veículo chega no sensor 2 */
		if (evt->sensor_id == 2 && evt->level == 1 && !ctx.sensor2_seen) {
			ctx.sensor2_seen = true;
			ctx.sensor2_time_ms = evt->timestamp_ms;
		}
		break;
	}
}

void radar_processing_init(void)
{
	vehicle_context_reset(&ctx);
	k_work_init_delayable(&ctx.finalize_work, finalize_vehicle_work);
}
