#include <stdio.h>
#include "pwm.h"

int
main(int argc, char **argv) {
  int err;
  pwm_db_t *db = NULL;
  pwm_str_t buf = PWM_STR_INIT;

  pwm_init();

  if (argc < 2) {
    fprintf(stderr, "usage: %s <key>\n", argv[0]);
    return 1;
  }

  if ((err = pwm_find_home(&buf)) < 0) {
    fprintf(stderr, "couldn't find the pwm home dir\n");
    goto cleanup;
  }

  if ((err = pwm_db_new(&db, buf.buf, getenv(PWM_EMAIL_ENV_VAR))) < 0) {
    goto cleanup;
  }

  if (!pwm_db_has(db, argv[1])) {
    fprintf(stderr, "couldn't find your %s password\n", argv[1]);
    err = -1;
    goto cleanup;
  }

  if ((err = pwm_db_del(db, argv[1])) >= 0) {
    fprintf(stderr, "deleted your %s password\n", argv[1]);
  }
  pwm_db_clean(db);

cleanup:
  pwm_db_free(db);
  pwm_str_free(&buf);
  return err < 0;
}
