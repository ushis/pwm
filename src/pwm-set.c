#include <stdio.h>
#include <string.h>
#include "pwm.h"

int
main(int argc, char **argv) {
  int err;
  pwm_db_t *db;
  pwm_str_t *home, *passwd = NULL;

  if (argc < 2) {
    fprintf(stderr, "usage: %s [-f] <key>\n", argv[0]);
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

  if (pwm_db_has(db, argv[1])) {
    fprintf(stderr, "entry already exists: %s\n", argv[1]);
    err = -1;
    goto cleanup;
  }

  if ((passwd = pwm_read_line_hidden("enter password: ")) == NULL) {
    err = -1;
    goto cleanup;
  }

  if ((err = pwm_db_set(db, argv[1], passwd)) >= 0) {
    fprintf(stderr, "set entry: %s\n", argv[1]);
  }
  pwm_db_clean(db);

cleanup:
  pwm_str_free(passwd);
  pwm_db_free(db);
  return err < 0;
}
