#include "pwm.h"
#include <stdio.h>
#include <getopt.h>

static const char *usage_str =
  "pwm note list [<opts>]\n\n"
  "options:\n"
  "  -h              show this help";

static void
usage() {
  fprintf(stderr, "usage: %s\n", usage_str);
  exit(EXIT_FAILURE);
}

static int
print_ln(const char *line) {
  return printf("%s\n", line);
}

static int
run() {
  int err;
  pwm_db_t *db;

  if ((err = pwm_db_new(&db, NULL, NULL)) < 0) {
    return err;
  }
  err = pwm_db_list(db, "notes", print_ln);
  pwm_db_free(db);
  return err;
}

int
main(int argc, char **argv) {
  int err;

  while (getopt(argc, argv, ":h") > -1) {
    usage(); /* -h is the only valid option */
  }
  pwm_init();
  err = run();
  pwm_shutdown();
  exit(err < 0);
}
