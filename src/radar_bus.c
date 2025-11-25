#include "radar_bus.h"

/* Sem observers estáticos; vamos adicionar via ZBUS_CHAN_ADD_OBS nos módulos. */
ZBUS_CHAN_DEFINE(chan_radar_infraction,
		 struct radar_infraction_msg,
		 NULL, NULL,
		 ZBUS_OBSERVERS_EMPTY,
		 ZBUS_MSG_INIT(0));

ZBUS_CHAN_DEFINE(chan_camera_result,
		 struct camera_result_msg,
		 NULL, NULL,
		 ZBUS_OBSERVERS_EMPTY,
		 ZBUS_MSG_INIT(0));
