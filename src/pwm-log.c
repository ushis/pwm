#include <stdio.h>
#include <time.h>
#include "pwm.h"

int
print_log(int64_t time, const char *name, const char *msg) {
  struct tm *t;
  char buf[40];

  if ((t = localtime(&time)) == NULL) {
    perror("localtime");
    return -1;
  }

  if (!strftime(buf, 40, "%Y-%m-%d %H:%M", t)) {
    perror("strftime");
    return -1;
  }
  return printf("[%s] (%s) %s\n", buf, name, msg);
}

int
main(int argc, char **argv) {
  int err;
  pwm_db_t *db = NULL;
  pwm_str_t buf = PWM_STR_INIT;

  pwm_init();

  if ((err = pwm_find_home(&buf)) < 0) {
    fprintf(stderr, "couldn't find the pwm buf dir\n");
    goto cleanup;
  }

  if ((err = pwm_db_new(&db, buf.buf, getenv(PWM_EMAIL_ENV_VAR))) < 0) {
    goto cleanup;
  }
  err = pwm_db_log(db, print_log);

cleanup:
  pwm_db_free(db);
  pwm_str_free(&buf);
  return err < 0;
}
