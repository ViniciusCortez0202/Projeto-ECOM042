#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>

#include "radar_processing.h"
#include "radar_display.h"
#include "sensor.h"
#include "vehicle_infraction_plate.h"

#define VEHICLE_LENGTH_MM 5000

LOG_MODULE_DECLARE(radar);

/*
	Estado de captura do sistema
*/
enum vehicle_track_state {
	VEHICLE_STATE_IDLE = 0,
	VEHICLE_STATE_TRACKING = 1,
};

/*
	Estrutura de contexto para salvar os dados
	do veículo que está passando pelo radar
*/
struct vehicle_context {
	enum vehicle_track_state state;
	uint32_t start_time_ms;
	uint32_t sensor2_time_ms;
	bool sensor2_seen;
	uint8_t axle_count;
	struct k_work_delayable finalize_work;
};

static struct vehicle_context ctx;

/* timeout para determinar quando o último eixo do veículo já passou */
int32_t radar_stop_timeout = 500;

/*
	Limpa o estado do contexto
*/
static void vehicle_context_reset(struct vehicle_context *c)
{
	c->state = VEHICLE_STATE_IDLE;
	c->axle_count = 0;
	c->start_time_ms = 0;
	radar_stop_timeout = 500;
}

int32_t compute_speed_kmh(uint32_t distance_mm, uint32_t delta_ms)
{
	if (delta_ms == 0) {
		return 0;
	}

	return (int32_t)((distance_mm * 36U) / (delta_ms * 10U));
}

void compute_timeout()
{
	uint32_t distance_mm = CONFIG_RADAR_SENSOR_DISTANCE_MM;
	uint32_t delta_ms = ctx.sensor2_time_ms - ctx.start_time_ms;
	int32_t speed_kmh = compute_speed_kmh(distance_mm, delta_ms);
	radar_stop_timeout = ((int64_t)VEHICLE_LENGTH_MM * 3600) / ((int64_t)speed_kmh * 1000);
}

void init_context(const struct sensor_event *evt)
{
	ctx.state = VEHICLE_STATE_TRACKING;
	ctx.axle_count = 1;
	ctx.start_time_ms = evt->timestamp_ms;
	ctx.sensor2_seen = false;
}

void add_axle()
{
	ctx.axle_count++;
}

void register_timestamp_sensor2(const struct sensor_event *evt)
{
	ctx.sensor2_seen = true;
	ctx.sensor2_time_ms = evt->timestamp_ms;
}

int compute_vehicle_status(struct radar_display_data *disp)
{
	if (!ctx.sensor2_seen || ctx.state != VEHICLE_STATE_TRACKING || ctx.axle_count == 1) {
		LOG_WRN("Possível ruido");
		vehicle_context_reset(&ctx);
		return -1;
	}

	uint32_t delta_ms = ctx.sensor2_time_ms - ctx.start_time_ms;
	uint32_t distance_mm = CONFIG_RADAR_SENSOR_DISTANCE_MM;
	int32_t speed_kmh = compute_speed_kmh(distance_mm, delta_ms);
	enum vehicle_type vtype = ctx.axle_count >= 3 ? VEHICLE_TYPE_HEAVY : VEHICLE_TYPE_LIGHT;
	int32_t limit_kmh;
	if (vtype == VEHICLE_TYPE_LIGHT) {
		limit_kmh = CONFIG_RADAR_SPEED_LIMIT_LIGHT_KMH;
	} else {
		limit_kmh = CONFIG_RADAR_SPEED_LIMIT_HEAVY_KMH;
	}

	enum radar_infraction_status status;
	int32_t warning_speed = (limit_kmh * CONFIG_RADAR_WARNING_THRESHOLD_PERCENT) / 100;
	if (speed_kmh > limit_kmh) {
		status = RADAR_STATUS_INFRACTION;
	} else if (speed_kmh >= warning_speed) {
		status = RADAR_STATUS_WARNING;
	} else {
		status = RADAR_STATUS_NORMAL;
	}

	*disp = (struct radar_display_data){
		.type = vtype,
		.axle_count = ctx.axle_count,
		.delta_ms = delta_ms,
		.measured_speed_kmh = speed_kmh,
		.limit_kmh = limit_kmh,
		.status = status,
	};

	return 0;
}

/*
	Responsável por emitir ao display quando o timer disparar
	e limpa o estado para o próximo veículo
*/
static void finalize_vehicle_work(struct k_work *work)
{
	ARG_UNUSED(work);

	struct radar_display_data disp;
	if (compute_vehicle_status(&disp) == 0) {
		radar_display_show(&disp);
	}
	/*
		Caso o veículo tenha cometido a infração ele dispara a captura de placa
	*/
	if (disp.status == RADAR_STATUS_INFRACTION) {
		handler_plate_infraction();
	}

	vehicle_context_reset(&ctx);
}

void radar_process_sensor_event(const struct sensor_event *evt)
{
	if (!evt) {
		return;
	}

	/*
		Faz a verificação de estado alternando de IDLE para TRACKING.
		Para sair de IDLE a função deve receber um evt do sensor 1
		A contagem de eixos é atualizada conforme o veículo passa pelo sensor 1,
		assim o sensor 2 serve apenas para calcular a velocidade do veículo.

		Sempre que um novo eixo é contado o timeout é zerado com base em
	   @radar_stop_timeout, que é calculado a partir da velocidade obtida depois da leitura do
	   segundo sensor.
	*/
	switch (ctx.state) {
	case VEHICLE_STATE_IDLE:
		if (evt->sensor_id == 1 && evt->level == 1) {
			init_context(evt);
			k_work_reschedule(&ctx.finalize_work, K_MSEC(radar_stop_timeout));
		}
		break;

	case VEHICLE_STATE_TRACKING:
		if (evt->sensor_id == 1 && evt->level == 1) {
			add_axle();
			k_work_reschedule(&ctx.finalize_work, K_MSEC(radar_stop_timeout));
		}

		if (evt->sensor_id == 2 && evt->level == 1 && !ctx.sensor2_seen) {
			register_timestamp_sensor2(evt);
			compute_timeout();
		}

		break;
	}
}

void radar_processing_init()
{
	vehicle_context_reset(&ctx);
	k_work_init_delayable(&ctx.finalize_work, finalize_vehicle_work);
}
