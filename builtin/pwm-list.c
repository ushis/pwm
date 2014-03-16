#include "pwm.h"
#include <stdio.h>
#include <getopt.h>

const char *usage_str =
  "pwm list [<opts>]\n\n"
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
  pwm_db_t *db;

  if ((err = pwm_db_new(&db, NULL, NULL)) < 0) {
    return err;
  }
  err = pwm_db_list(db, print_ln);
  pwm_db_free(db);
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
