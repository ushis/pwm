#include "pwm.h"
#include <stdio.h>
#include <time.h>
#include <getopt.h>

const char *usage_str =
  "pwm log [<opts>]\n\n"
  "options:\n"
  "  -h              show this help";

void
usage() {
  fprintf(stderr, "usage: %s\n", usage_str);
  exit(EXIT_FAILURE);
}

int
print_log(int64_t time, const char *name, const char *msg) {
  struct tm *t;
  char buf[40];

  if ((t = localtime((time_t *) &time)) == NULL) {
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
run() {
  int err;
  pwm_db_t *db;

  if ((err = pwm_db_new(&db, NULL, NULL)) < 0) {
    return err;
  }
  err = pwm_db_log(db, print_log);
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
