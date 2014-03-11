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
  pwm_str_t buf = PWM_STR_INIT;

  pwm_init();

  if ((err = pwm_find_home(&buf)) < 0) {
    fprintf(stderr, "couldn't find the pwm home dir\n");
    goto cleanup;
  }

  if ((err = pwm_db_new(&db, buf.buf, getenv(PWM_EMAIL_ENV_VAR))) < 0) {
    goto cleanup;
  }
  err = pwm_db_list(db, print_ln);

cleanup:
  pwm_db_free(db);
  pwm_str_free(&buf);
  pwm_shutdown();
  return err < 0;
}
