#ifndef RADAR_PROCESSING_H
#define RADAR_PROCESSING_H

#include "sensor.h"
#include "radar_display.h"


/*
    Inicializa o contexto zerado e o k_work com a função de finalização
*/
void radar_processing_init();

/*
    Faz a alternancia de estado entre inicial e quando o veículo passa pelos sensores,
    detectando a quantidade de eixos e guardando o intervalo de leitura no contexto

    @param evt dados do sensor foi acionado
*/
void radar_process_sensor_event(const struct sensor_event *evt);

/*
    Calcula a velocidade média do veículo

    @param distance_mm distância entre os sensores 1 e 2
    @param delta_ms variação de tempo entre a primeira leitura no sensor 1 e do sensor 2

    @return ((distance_mm * 36) / (delta_ms * 10))
*/
int32_t compute_speed_kmh(uint32_t distance_mm, uint32_t delta_ms);

/*
    Computa qual é o estado do veiculo:
    - NORMAL
    - ATENÇÃO
    - INFRAÇÃO
    e monta o objeto de display com status da infração e velocidade

    @param disp parâmetro de saída com informações do display

    @return 0 caso consiga montar o objeto ou -1 em caso de ruido do sensor
*/
int compute_vehicle_status(struct radar_display_data *disp);

/*
    Atualiza o contexto quado o veículo passa pelo primeiro sensor

    @param evt primeiro disparo do sensor quando o estado é IDLE
*/
void init_context(const struct sensor_event *evt);

/*
    Adiciona 1 eixo a contagem
*/
void add_axle();

/*
    Atualiza o contexto com a leitura do segundo sensor
*/
void register_timestamp_sensor2(const struct sensor_event *evt);

/*
    Calcula o timeout da chamada da função de finalização
    com base na velocidade do automóvel
*/
void compute_timeout();

#endif /* RADAR_PROCESSING_H */
