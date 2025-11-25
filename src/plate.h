#ifndef PLATE_H
#define PLATE_H

#include <stdbool.h>

bool plate_is_valid_mercosul(const char *plate);

void plate_generate_mercosul(char out[8]);
void plate_generate_invalid(char out[8]);

#endif /* PLATE_H */
