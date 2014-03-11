#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "pwm.h"

int
main(int argc, char **argv) {
  int err;
  pwm_db_t *db = NULL;
  pwm_str_t buf = PWM_STR_INIT;

  if (argc < 2) {
    fprintf(stderr, "usage: %s [-f] <key>\n", argv[0]);
    return 1;
  }
  pwm_init();

  if ((err = pwm_find_home(&buf)) < 0) {
    fprintf(stderr, "couldn't find the pwm buf dir\n");
    goto cleanup;
  }

  if ((err = pwm_db_new(&db, buf.buf, getenv(PWM_EMAIL_ENV_VAR))) < 0) {
    goto cleanup;
  }

  if (pwm_db_has(db, argv[1])) {
    fprintf(stderr, "password for %s already exists\n", argv[1]);
    err = -1;
    goto cleanup;
  }

  if (isatty(STDIN_FILENO)) {
    err = pwm_read_line_hidden(&buf, "insert your password: ");
  } else {
    err = pwm_str_read_line(&buf, stdin);
  }

  if (err < 0) {
    goto cleanup;
  }

  if ((err = pwm_db_set(db, argv[1], &buf)) >= 0) {
    fprintf(stderr, "\rsaved your %s password\n", argv[1]);
  }
  pwm_db_clean(db);

cleanup:
  pwm_db_free(db);
  pwm_str_free(&buf);
  pwm_shutdown();
  return err < 0;
}
