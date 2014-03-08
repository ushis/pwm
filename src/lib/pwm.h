#ifndef PWM_H
#define PWM_H 1

#define PWM_EMAIL_ENV_VAR "PWM_EMAIL"
#define PWM_HOME_ENV_VAR "PWM_HOME"
#define PWM_HOME_NAME ".pwm"

#include "str.h"
#include "gen.h"
#include "db.h"

/* finds the pwm home directory */
pwm_str_t *pwm_find_home();

/* */
pwm_str_t *pwm_read_line(const char *prompt);

/* */
pwm_str_t *pwm_read_line_hidden(const char *prompt);

#endif /* PWM_H */
