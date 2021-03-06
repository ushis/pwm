#ifndef STR_H
#define STR_H 1

#include <stdio.h>
#include <stdarg.h>

/** Representation of a string. */
typedef struct {
  char *buf;
  size_t len;
  size_t cap;
} pwm_str_t;

#define PWM_STR_INIT(s) pwm_str_t s = {NULL,0,0}

/**
 * Resizes the string buffer.
 *
 * @param  s  String pointer
 * @param  n  The new buffer size
 * @return 0 on success or an error code
 */
int pwm_str_resize(pwm_str_t *s, size_t n);

/**
 * Frees the string buffer.
 *
 * Note: This function only frees the internal buffer of the string, not the
 * string itself.
 *
 * @param  s  String pointer
 */
void pwm_str_free(pwm_str_t *s);

/**
 * Sets the length of a string after checking bounds.
 *
 * @param  s  String pointer
 * @param  n  The new length
 * @return 0 on success or an error code
 */
int pwm_str_set_len(pwm_str_t *s, size_t n);

/**
 * Copies a string.
 *
 * @param  dst  String pointer to store the result
 * @param  src  String pointer to copy
 * return 0 on success or an error code
 */
int pwm_str_cpy(pwm_str_t *dst, pwm_str_t *src);

/**
 * Sets the string buffer.
 *
 * @param  s    String pointer
 * @param  buf  Buffer to copy into the string buffer
 * @param  n    Number of bytes to copy
 * @return 0 on success or an error code
 */
int pwm_str_set(pwm_str_t *s, const char *buf, size_t n);

/**
 * Appends a buffer to a string.
 *
 * @param  s    String pointer
 * @param  buf  Buffer to append
 * @param  n    Number of bytes to append
 * @return 0 on success or an error code
 */
int pwm_str_append(pwm_str_t *s, const char *buf, size_t n);

/**
 * Appends a buffer to a string and insert a '/' before if necessary.
 *
 * @param  s    String pointer
 * @param  buf  Buffer to append
 * @param  n    Number of bytes to append
 * @return 0 on success or an error code
 */
int pwm_str_append_path_component(pwm_str_t *s, const char *buf, size_t n);

/**
 * Shrinks a string.
 *
 * @param  s  String pointer
 * @param  n  The new size
 */
void pwm_str_shrink(pwm_str_t *s, size_t n);

/**
 * Compares two strings with each other.
 *
 * @param  a  A string pointer
 * @param  b  Another string pointer
 * @return 0 if the a and b are equal - see man memcmp for more info
 */
int pwm_str_cmp(pwm_str_t *a, pwm_str_t *b);

/**
 * Formats a string printf style.
 *
 * @param  s    String pointer
 * @param  fmt  Format string (see printf man page)
 * @param  ...  Variable number of format arguments (see printf man page)
 * @return 0 on success or an error code
 */
int pwm_str_fmt(pwm_str_t *s, const char *fmt, ...);

/**
 * Same as pwm_str_fmt, but takes a va_list instead of a variable number of
 * arguments.
 *
 * @param  s    String pointer
 * @param  fmt  Format string (see printf man page)
 * @param  ap   Variable argument list (see stdarg man page)
 * @return 0 on success or an error code
 */
int pwm_str_vfmt(pwm_str_t *s, const char *fmt, va_list ap);

/**
 * Reads from a file descriptor until EOF.
 *
 * @param  s   String pointer to store the result in
 * @param  fd  File descriptor to read from
 * @return 0 on success or an error code
 */
int pwm_str_read_all(pwm_str_t *s, int fd);

/**
 * Reads a line from a stream.
 *
 * @param  s       String pointer to store the result in
 * @param  stream  The stream to read from
 * @return 0 on success or an error code
 */
int pwm_str_read_line(pwm_str_t *s, FILE *stream);

/**
 * Encodes a buffer to hex.
 *
 * @param  dst  String pointer to store the result in
 * @param  buf  Buffer to encode
 * @param  n    Number of bytes to encode
 * @return 0 on success or an error code
 */
int pwm_str_hex_encode(pwm_str_t *s, const char *buf, size_t n);

/**
 * Decodes a hex encoded buffer.
 *
 * @param  dst  String pointer to store the result in
 * @param  buf  Buffer to decode
 * @param  n    Number of bytes to decode
 * @return 0 on success or an error code
 */
int pwm_str_hex_decode(pwm_str_t *s, const char *buf, size_t n);

#endif /* STR_H */
