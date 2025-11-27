#ifndef RADAR_DISPLAY_H
#define RADAR_DISPLAY_H

#include <stdint.h>
#include <stdbool.h>

/*
    Define o tipo de veículo com base 
    na quantidade de eixos
*/
enum vehicle_type {
    VEHICLE_TYPE_LIGHT = 0,
    VEHICLE_TYPE_HEAVY = 1,
};

/*
    Define o status com base 
    no limite do radar e tipo de veículo
*/
enum radar_infraction_status {
    RADAR_STATUS_NORMAL = 0,
    RADAR_STATUS_WARNING = 1,
    RADAR_STATUS_INFRACTION = 2,
};

/*
    Estrutura usada para transmitir ao display 
    o estado e informações do veiculo, além de informações
    do radar como limite da via para o tipo de
    veiculo
*/
struct radar_display_data {
    enum vehicle_type type;
    uint8_t axle_count;
    uint32_t delta_ms;
    int32_t measured_speed_kmh;
    int32_t limit_kmh;
    enum radar_infraction_status status;
};

/*
    Inicializa o display

    @retorna 0 caso consiga inicializar e -ENODEV caso a operação não seja permitida
*/
int radar_display_init();

/*
    Exibi as informações no display e no console

    @param data estrutura com informações do veículo e do rada
*/
void radar_display_show(const struct radar_display_data *data);


/*
	Formata a saida do tipo de veículo no display

	@param type tipo do veículo conforme os eixos

	@return uma string com o tipo de veículo
		- Pesado (3 eixos ou mais)
		- Leve (menos de 3 eixo)
*/
char *vehicle_type_str(enum vehicle_type type);

/*
	Formata a saída do tipo de veículo no display
	
	@param status status do veículo conforme velocidade da via e tipo de veículo

	@return uma string com o status do veículo
		- Normal (dentro da velocidade da via para o tipo de veículo)
		- Atenção (a uma a velocidade perto do limite da via para o tipo de veículo)
		- Infração (acima da velocidade da via para o tipo de veículo)
*/
char *status_str(enum radar_infraction_status status);

/*
	Formata a saída do tipo de veículo no display

	@param status status do veículo conforme velocidade da via e tipo de veículo

	@return uma string com a cor conforme o status
*/
char *status_color(enum radar_infraction_status s);


#endif /* RADAR_DISPLAY_H */
