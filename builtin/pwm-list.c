#include <stdio.h>
#include <getopt.h>
#include "pwm.h"

const char *usage_str =
  "pwm-list [<opts>]\n\n"
  "options:\n"
  "  -h              show this help";

void
usage() {
  fprintf(stderr, "usage: %s\n", usage_str);
  exit(EXIT_FAILURE);
}

int
print_ln(const char *line) {
  return printf("%s\n", line);
}

int
run() {
  int err;
  pwm_db_t *db = NULL;
  pwm_str_t buf = PWM_STR_INIT;

  if ((err = pwm_find_home(&buf)) < 0) {
    fprintf(stderr, "couldn't find the pwm home dir\n");
    goto cleanup;
  }

  if ((err = pwm_db_new(&db, buf.buf, pwm_find_email())) < 0) {
    goto cleanup;
  }
  err = pwm_db_list(db, print_ln);

cleanup:
  pwm_db_free(db);
  pwm_str_free(&buf);
  return err;
}

int
main(int argc, char **argv) {
  int err;

  while (getopt(argc, argv, "h") > -1) {
    usage(); /* -h is the only valid option */
  }
  pwm_init();
  err = run();
  pwm_shutdown();
  exit(err < 0);
}
