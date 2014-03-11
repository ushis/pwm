#ifndef PWM_H
#define PWM_H 1

#define PWM_EMAIL_ENV_VAR "PWM_EMAIL"
#define PWM_HOME_ENV_VAR "PWM_HOME"
#define PWM_HOME_NAME ".pwm"

#include "str.h"
#include "gen.h"
#include "db.h"

/* */
void pwm_init();

/*  */
void pwm_shutdown();

/* finds the pwm home directory */
int pwm_find_home(pwm_str_t *s);

/* reads a line from stdin into s */
int pwm_read_line(pwm_str_t *s, const char *prompt);

/* same as pwm_read_line() without echoing the entered characters */
int pwm_read_line_hidden(pwm_str_t *s, const char *prompt);

#endif /* PWM_H */
