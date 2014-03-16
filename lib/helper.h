#ifndef HELPER_H
#define HELPER_H 1

#include "str.h"

/**
 * Finds the pwm home directory.
 *
 * @param  s  String pointer to store the result in
 * @return 0 on success or an error code
 */
int pwm_find_home(pwm_str_t *s);

/**
 * Finds the pwm key id.
 *
 * @return The key id or NULL if no id was found
 */
const char *pwm_find_key_id();

/**
 * Reads a line from stdin.
 *
 * @param  s       String pointer to store the result in
 * @param  prompt  Prompt to show
 * @return 0 on success or an error code
 */
int pwm_read_line(pwm_str_t *s, const char *prompt);

/**
 * Reads a line from stdin without printing the entered characters.
 *
 * @param  s       String pointer to store the result in
 * @param  prompt  Prompt to show
 * @return 0 on success or an error code
 */
int pwm_read_line_hidden(pwm_str_t *s, const char *prompt);

#endif /* HELPER_H */
