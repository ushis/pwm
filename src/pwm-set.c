#include <stdio.h>
#include <string.h>
#include "pwm.h"

int
main(int argc, char **argv) {
  int err;
  pwm_db_t *db = NULL;
  pwm_str_t home = PWM_STR_INIT, passwd = PWM_STR_INIT;

  if (argc < 2) {
    fprintf(stderr, "usage: %s [-f] <key>\n", argv[0]);
    return 1;
  }

  if ((err = pwm_find_home(&home)) < 0) {
    fprintf(stderr, "couldn't find the pwm home dir\n");
    goto cleanup;
  }

  if ((err = pwm_db_new(&db, &home, getenv(PWM_EMAIL_ENV_VAR))) < 0) {
    goto cleanup;
  }

  if (pwm_db_has(db, argv[1])) {
    fprintf(stderr, "password for `%s` already exists\n", argv[1]);
    err = -1;
    goto cleanup;
  }

  if ((err = pwm_read_line_hidden(&passwd, "insert your password: ")) < 0) {
    goto cleanup;
  }

  if ((err = pwm_db_set(db, argv[1], &passwd)) >= 0) {
    fprintf(stderr, "\rsaved your `%s` password\n", argv[1]);
  }
  pwm_db_clean(db);

cleanup:
  pwm_db_free(db);
  pwm_str_free(&home);
  pwm_str_free(&passwd);
  return err < 0;
}
