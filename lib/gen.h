#ifndef GEN_H
#define GEN_H 1

#include "str.h"
#include "hex.h"

/* gen struct */
typedef struct {
  const char *name;
  const char *desc;
  int (*func)(pwm_str_t *s, size_t n);
} pwm_gen_t;

/* generates a random alphanumeric string and stores it in s */
int pwm_gen_alnum(pwm_str_t *s, size_t n);

/* generates a random ascii string and stores it in s */
int pwm_gen_ascii(pwm_str_t *s, size_t n);

/* generates a random hex of size n and stores it in s */
int pwm_gen_hex(pwm_str_t *s, size_t n);

/* generates a random numeric string and stores it in s */
int pwm_gen_num(pwm_str_t *s, size_t n);

/* finds a generator by index */
const pwm_gen_t *pwm_gen_by_index(size_t i);

/* finds a generator by name */
const pwm_gen_t *pwm_gen_by_name(const char *name);

#endif /* GEN_H */
