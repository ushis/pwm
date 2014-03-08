#ifndef GEN_H
#define GEN_H 1

#include "str.h"
#include "hex.h"

/* generates a random hex of size n and stores it in s */
int pwm_gen_hex(pwm_str_t *s, size_t n);

/* generates a random alphanumeric string and stores it in s */
int pwm_gen_alnum(pwm_str_t *s, size_t n);

#endif /* GEN_H */
