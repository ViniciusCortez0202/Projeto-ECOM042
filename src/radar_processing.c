#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>

#include "radar_processing.h"
#include "radar_control.h"
#include "radar_display.h"

#define VEHICLE_LENGTH_MM 5000

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
int32_t radar_stop_timeout = 500;

// Volta o estado para os valores iniciais
static void vehicle_context_reset(struct vehicle_context *c)
{
	c->state = VEHICLE_STATE_IDLE;
	c->axle_count = 0;
	c->start_time_ms = 0;
}

// Calcula a velocidade média
int32_t compute_speed_kmh(uint32_t distance_mm, uint32_t delta_ms)
{
	if (delta_ms == 0) {
		return 0;
	}

	return (int32_t)((distance_mm * 36U) / (delta_ms * 10U));
}

int compute_vehicle_status(struct radar_display_data *disp)
{
	/* Só finaliza se estiver acompanhando */
	if (ctx.state != VEHICLE_STATE_TRACKING) {
		return -1;
	}

	/* Se nunca chegou no sensor 2, não dá pra calcular velocidade */
	if (!ctx.sensor2_seen) {
		LOG_WRN("Possível ruido");
		vehicle_context_reset(&ctx);
		return -1;
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
		return -1;
	}

	struct radar_evaluation_result res = radar_evaluate_speed(vtype, speed_kmh);

	*disp = (struct radar_display_data){
		.type = vtype,
		.axle_count = ctx.axle_count,
		.delta_ms = delta_ms,
		.measured_speed_kmh = res.measured_speed_kmh,
		.limit_kmh = res.limit_kmh,
		.status = res.status,
	};
	return 0;
}

static void finalize_vehicle_work(struct k_work *work)
{
	ARG_UNUSED(work);

	struct radar_display_data disp;
	if (compute_vehicle_status(&disp) == 0) {
		radar_display_show(&disp);
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
			init_context(evt);
			printk("Logou eixo 1\n");
			k_work_reschedule(&ctx.finalize_work, K_MSEC(radar_stop_timeout));
		}
		break;

	case VEHICLE_STATE_TRACKING:
		if (evt->sensor_id == 1 && evt->level == 1) {
			add_axle();
			printk("Logou eixo 1\n");
			k_work_reschedule(&ctx.finalize_work, K_MSEC(radar_stop_timeout));
		}

		if (evt->sensor_id == 2 && evt->level == 1 && !ctx.sensor2_seen) {
			add_sensor2(evt);
		}
		break;
	}
}

int64_t compute_timeout()
{
	uint32_t distance_mm = CONFIG_RADAR_SENSOR_DISTANCE_MM;
	uint32_t delta_ms = ctx.sensor2_time_ms - ctx.start_time_ms;
	int32_t speed_kmh = compute_speed_kmh(distance_mm, delta_ms);
	return ((int64_t)VEHICLE_LENGTH_MM * 3600) / ((int64_t)speed_kmh * 1000);
}

int init_context(const struct sensor_event *evt)
{
	ctx.state = VEHICLE_STATE_TRACKING;
	ctx.axle_count = 1;
	ctx.start_time_ms = evt->timestamp_ms;
	ctx.sensor2_seen = false;
	return 0;
}

int add_axle()
{
	ctx.axle_count++;
	return 0;
}

int add_sensor2(const struct sensor_event *evt)
{
	ctx.sensor2_seen = true;
	ctx.sensor2_time_ms = evt->timestamp_ms;
	radar_stop_timeout = compute_timeout();

	return 0;
}

void radar_processing_init(void)
{
	vehicle_context_reset(&ctx);
	k_work_init_delayable(&ctx.finalize_work, finalize_vehicle_work);
}
