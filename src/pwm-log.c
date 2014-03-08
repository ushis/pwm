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
  pwm_str_t home = PWM_STR_INIT;

  if (pwm_find_home(&home) < 0) {
    fprintf(stderr, "couldn't find the pwm home dir\n");
    return 1;
  }

  if ((db = pwm_db_new(&home, getenv(PWM_EMAIL_ENV_VAR))) == NULL) {
    pwm_str_free(&home);
    return 1;
  }
  err = pwm_db_log(db, print_log);
  pwm_db_free(db);
  pwm_str_free(&home);
  return err < 0;
}
