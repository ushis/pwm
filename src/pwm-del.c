#include <stdio.h>
#include "pwm.h"

int
main(int argc, char **argv) {
  int err;
  pwm_db_t *db;
  pwm_str_t *home;

  if (argc < 2) {
    fprintf(stderr, "usage: %s <key>\n", argv[0]);
    return 1;
  }

  if ((home = pwm_find_home()) == NULL) {
    fprintf(stderr, "could not find the pwm home dir\n");
    return 1;
  }

  if ((db = pwm_db_new(home, getenv(PWM_EMAIL_ENV_VAR))) == NULL) {
    pwm_str_free(home);
    return 1;
  }

  if (!pwm_db_has(db, argv[1])) {
    fprintf(stderr, "entry does not exist: %s\n", argv[1]);
    err = -1;
    goto cleanup;
  }

  if ((err = pwm_db_del(db, argv[1])) >= 0) {
    fprintf(stderr, "deleted entry: %s\n", argv[1]);
  }
  pwm_db_clean(db);

cleanup:
  pwm_db_free(db);
  return err < 0;
}
