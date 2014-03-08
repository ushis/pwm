#include <stdio.h>
#include "pwm.h"

int
main(int argc, char **argv) {
  int err;
  pwm_db_t *db;
  pwm_str_t home = PWM_STR_INIT;

  if (argc < 2) {
    fprintf(stderr, "usage: %s <key>\n", argv[0]);
    return 1;
  }

  if (pwm_find_home(&home) < 0) {
    fprintf(stderr, "couldn't find the pwm home dir\n");
    return 1;
  }

  if ((db = pwm_db_new(&home, getenv(PWM_EMAIL_ENV_VAR))) == NULL) {
    pwm_str_free(&home);
    return 1;
  }

  if (!pwm_db_has(db, argv[1])) {
    fprintf(stderr, "couldn't find your `%s` password\n", argv[1]);
    err = -1;
    goto cleanup;
  }

  if ((err = pwm_db_del(db, argv[1])) >= 0) {
    fprintf(stderr, "deleted your `%s` password\n", argv[1]);
  }
  pwm_db_clean(db);

cleanup:
  pwm_db_free(db);
  pwm_str_free(&home);
  return err < 0;
}
