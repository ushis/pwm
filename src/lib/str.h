#ifndef STR_H
#define STR_H 1

#include <stdio.h>

/* string struct */
typedef struct {
  char *buf;
  size_t len;
  size_t cap;
} pwm_str_t;

#define PWM_STR_INIT {NULL,0,0}

/* resizes the string buffer */
int pwm_str_resize(pwm_str_t *s, size_t n);

/* frees a string */
void pwm_str_free(pwm_str_t *s);

/* copies a string */
int pwm_str_cpy(pwm_str_t *dst, pwm_str_t *src);

/* sets the string buffer */
int pwm_str_set(pwm_str_t *s, const char *buf, size_t n);

/* appends a buffer to the string */
int pwm_str_append(pwm_str_t *s, const char *buf, size_t n);

/* same as pwm_str_append, but inserts a '/' before the buf if necessary */
int pwm_str_append_path_component(pwm_str_t *s, const char *buf, size_t n);

/* shrinks a string */
void pwm_str_shrink(pwm_str_t *s, size_t n);

/* compares two strings */
int pwm_str_cmp(pwm_str_t *a, pwm_str_t *b);

/* reads a line from stream into s */
int pwm_str_read_line(pwm_str_t *s, FILE *stream);

/* encodes a raw byte buffer to hex and writes the result to dst */
int pwm_str_hex_encode(pwm_str_t *dst, const char *buf, size_t n);

/* decodes a hex buffer to raw bytes and writes the result to dst */
int pwm_str_hex_decode(pwm_str_t *dst, const char *buf, size_t n);

#endif /* STR_H */
