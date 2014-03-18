#ifndef GEN_H
#define GEN_H 1

#include "str.h"
#include "hex.h"

/** Representation of a generator. */
typedef struct {
  const char *name;
  const char *desc;
  int (*func)(pwm_str_t *s, size_t n);
} pwm_gen_t;

/**
 * Generates random data.
 *
 * @param  s  String pointer to store the result in
 * @param  n  Length of the result
 * @return 0 on success or an error code
 */
int pwm_gen_rand(pwm_str_t *s, size_t n);

/**
 * Generates a random alphanumeric string.
 *
 * @param  s  String pointer to store the result in
 * @param  n  Length of the result
 * @return 0 on success or an error code
 */
int pwm_gen_alnum(pwm_str_t *s, size_t n);

/**
 * Generates a random ascii string.
 *
 * @param  s  String pointer to store the result in
 * @param  n  Length of the result
 * @return 0 on success or an error code
 */
int pwm_gen_ascii(pwm_str_t *s, size_t n);

/**
 * Generates a random hex string.
 *
 * @param  s  String pointer to store the result in
 * @param  n  Length of the result
 * @return 0 on success or an error code
 */
int pwm_gen_hex(pwm_str_t *s, size_t n);

/**
 * Generates a random numeric string.
 *
 * @param  s  String pointer to store the result in
 * @param  n  Length of the result
 * @return 0 on success or an error code
 */
int pwm_gen_num(pwm_str_t *s, size_t n);

/**
 * Finds a generator by index.
 *
 * @param  i  The index of the generator
 * @return The generator or NULL if no generator was found
 */
const pwm_gen_t *pwm_gen_by_index(size_t i);

/**
 * Finds a generator by name.
 *
 * @param  name  The name of the generator
 * @return The generator or NULL if no generator was found
 */
const pwm_gen_t *pwm_gen_by_name(const char *name);

#endif /* GEN_H */
