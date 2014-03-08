#include <stdio.h>
#include "pwm.h"

int
print_ln(const char *line) {
  return printf("%s\n", line);
}

int
main(int argc, char **argv) {
  int err;
  pwm_db_t *db = NULL;
  pwm_str_t home = PWM_STR_INIT;

  if (pwm_find_home(&home) < 0) {
    fprintf(stderr, "couldn't find the pwm home dir\n");
    return 1;
  }

  if ((db = pwm_db_new(&home, getenv(PWM_EMAIL_ENV_VAR))) == NULL) {
    pwm_str_free(&home);
    return 1;
  }
  err = pwm_db_list(db, print_ln);
  pwm_db_free(db);
  pwm_str_free(&home);
  return err < 0;
}
