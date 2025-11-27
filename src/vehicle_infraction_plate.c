#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>

#include "vehicle_infraction_plate.h"
#include "camera_service.h"
#include "plate.h"

LOG_MODULE_DECLARE(radar);

ZBUS_MSG_SUBSCRIBER_DEFINE(msub_camera_evt);

ZBUS_CHAN_ADD_OBS(chan_camera_evt, msub_camera_evt, 3);

void handler_plate_infraction()
{

	int err;
	const struct zbus_channel *chan;
	
	/*
		Faz a chamada para o serviço de camera 
		e espera o retorno via zbus com as informações 
		da placa do veículo infrator
	*/
	err = camera_api_capture(K_FOREVER);
	if (err) {
		LOG_ERR("Erro ao iniciar a camera. Erro: %d\n", err);
		return;
	}

	struct msg_camera_evt rsp;

	err = zbus_sub_wait_msg(&msub_camera_evt, &chan, &rsp, K_FOREVER);
	if (err) {
		LOG_INF("Erro ao receber mensagem: %d\n", err);
		return;
	}

	if (rsp.type == MSG_CAMERA_EVT_TYPE_ERROR) {
		LOG_INF("Serviço de camera indisponível. Erro %d\n", rsp.error_code);
	} else if (rsp.type == MSG_CAMERA_EVT_TYPE_DATA) {
		char *is_valid =
			plate_is_valid_mercosul(rsp.captured_data->plate) ? "VALIDA" : "INVÁLIDA";
		printk("A placa %s é %s\n", rsp.captured_data->plate, is_valid);
	}
}
