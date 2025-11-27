#ifndef PLATE_H
#define PLATE_H

#include <stdbool.h>

/*
    Valida se uma string é uma placa de carro pertencente ao mercosul

    @param plate placa de um carro

    @return true se é uma placa do mercosul e false caso contrário
*/
bool plate_is_valid_mercosul(const char *plate);

/*
    Valida se uma string é uma placa de carro pertencente ao Brasil no padrão mercosul

    @param plate placa de um carro

    @return true se é uma placa do mercosul e false caso contrário
*/
bool plate_is_br(const char *plate);

/*
    Valida se uma string é uma placa de carro pertencente ao Argentina no padrão mercosul

    @param plate placa de um carro

    @return true se é uma placa do mercosul e false caso contrário
*/
bool plate_is_ar(const char *plate);

/*
    Valida se uma string é uma placa de carro pertencente ao Paraguai no padrão mercosul

    @param plate placa de um carro

    @return true se é uma placa do mercosul e false caso contrário
*/
bool plate_is_pr(const char *plate);

/*
    Valida se uma string é uma placa de carro pertencente ao Uruguai no padrão mercosul

    @param plate placa de um carro

    @return true se é uma placa do mercosul e false caso contrário
*/
bool plate_is_ur(const char *plate);

/*
    Valida se uma string é uma placa de carro pertencente ao Bolívia no padrão mercosul

    @param plate placa de um carro

    @return true se é uma placa do mercosul e false caso contrário
*/
bool plate_is_bo(const char *plate);

#endif /* PLATE_H */
