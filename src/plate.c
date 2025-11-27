#include "plate.h"
#include <stddef.h>
#include <regex.h>

bool plate_is_valid_mercosul(const char *plate)
{
	return plate_is_br(plate) || plate_is_ar(plate) || plate_is_pr(plate) ||
	       plate_is_ur(plate) || plate_is_bo(plate);
}

/*
	Cria um regex e valida, foi utilizada a biblioteca (https://man7.org/linux/man-pages/man3/regcomp.3.html)

	@param plate placa de um veículo
	@param pattern regex para validação

	@return valida de plate está dentro do pattern
*/
static bool plate_match_pattern(const char *plate, const char *pattern)
{
	regex_t rx;
	int rc = regcomp(&rx, pattern, REG_EXTENDED | REG_NOSUB);
	if (rc != 0) {
		return false;
	}

	bool ok = (regexec(&rx, plate, 0, NULL, 0) == 0);
	regfree(&rx);
	return ok;
}

/* Brasil */
bool plate_is_br(const char *plate)
{
	return plate_match_pattern(plate, "^[A-Z]{3}[0-9]{1}[A-Z]{1}[0-9]{2}$");
}

/* Argentina */
bool plate_is_ar(const char *plate)
{
	return plate_match_pattern(plate, "^[A-Z]{2} [0-9]{3} [A-Z]{2}$");
}

/* Paraguai */
bool plate_is_pr(const char *plate)
{
	return plate_match_pattern(plate, "^([A-Z]{4} [0-9]{3}|[0-9]{3} [A-Z]{4})$");
}

/* Uruguai */
bool plate_is_ur(const char *plate)
{
	return plate_match_pattern(plate, "^[A-Z]{3} [0-9]{4}$");
}

/* Bolivia */
bool plate_is_bo(const char *plate)
{
	return plate_match_pattern(plate, "^[A-Z]{2} [0-9]{5}$");
}
