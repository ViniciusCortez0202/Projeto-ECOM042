#include "plate.h"
#include <stddef.h>
#include <zephyr/random/random.h>

static bool is_upper(char c) { return (c >= 'A' && c <= 'Z'); }
static bool is_digit(char c) { return (c >= '0' && c <= '9'); }

bool plate_is_valid_mercosul(const char *p)
{
	if (!p) return false;

	for (int i = 0; i < 7; i++) {
		if (p[i] == '\0') return false;
	}
	if (p[7] != '\0') return false;

	return is_upper(p[0]) && is_upper(p[1]) && is_upper(p[2]) &&
	       is_digit(p[3]) &&
	       is_upper(p[4]) &&
	       is_digit(p[5]) && is_digit(p[6]);
}

static char rand_letter(void) { return (char)('A' + (sys_rand32_get() % 26U)); }
static char rand_digit(void)  { return (char)('0' + (sys_rand32_get() % 10U)); }

void plate_generate_mercosul(char out[8])
{
	out[0] = rand_letter();
	out[1] = rand_letter();
	out[2] = rand_letter();
	out[3] = rand_digit();
	out[4] = rand_letter();
	out[5] = rand_digit();
	out[6] = rand_digit();
	out[7] = '\0';
}

void plate_generate_invalid(char out[8])
{
	/* Gera uma válida e quebra a regra (posição 4 deveria ser letra) */
	plate_generate_mercosul(out);
	out[4] = rand_digit(); /* agora fica inválida */
}
