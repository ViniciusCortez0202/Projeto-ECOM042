#ifndef RADAR_SENSOR_H
#define RADAR_SENSOR_H

#include <stdint.h>

/*
    Estrutura responsável por transmitir e qual sensor foi acionado.
*/
struct sensor_event {
    uint8_t sensor_id;
    uint8_t level;
    uint32_t timestamp_ms;
};

/*
    Inicializa os sensores.
*/
int sensor_init();


/*
    Recebe os dados do sensor que foi acionado e publica na fila a estrutura acima.

    @param sensor_id Identificador do sensor
    @param estado do sensor
*/
void sensor_send_event(uint8_t sensor_id, uint8_t level);

#endif /* RADAR_SENSOR_H */
