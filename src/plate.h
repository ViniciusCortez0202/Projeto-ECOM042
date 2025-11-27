#ifndef PLATE_H
#define PLATE_H

#include <stdbool.h>

bool plate_is_valid_mercosul(const char *plate);

bool plate_is_br(const char *plate);
bool plate_is_ar(const char *plate);
bool plate_is_pr(const char *plate);
bool plate_is_ur(const char *plate);
bool plate_is_bo(const char *plate);

#endif /* PLATE_H */
