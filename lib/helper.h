#ifndef HELPER_H
#define HELPER_H 1

#include "str.h"

/* finds the pwm home directory */
int pwm_find_home(pwm_str_t *s);

/* finds the pwm email address */
const char *pwm_find_email();

/* reads a line from stdin into s */
int pwm_read_line(pwm_str_t *s, const char *prompt);

/* same as pwm_read_line() without echoing the entered characters */
int pwm_read_line_hidden(pwm_str_t *s, const char *prompt);

#endif /* HELPER_H */
