#include <stdio.h>
#include <getopt.h>
#include "pwm.h"

const char *usage_str =
  "pwm-get [<opts>] <key>\n\n"
  "options:\n"
  "  -c              store the password in the clipboard (not implemented)\n"
  "  -h              show this help";

void
usage() {
  fprintf(stderr, "usage: %s\n", usage_str);
  exit(EXIT_FAILURE);
}

typedef struct {
  int clip;
} opts_t;

#define OPTS_DEFAULT {0}

int
run(opts_t *opts, const char *key) {
  int err;
  pwm_db_t *db = NULL;
  pwm_str_t buf = PWM_STR_INIT;

  if ((err = pwm_find_home(&buf)) < 0) {
    fprintf(stderr, "couldn't find the pwm home dir\n");
    goto cleanup;
  }

  if ((err = pwm_db_new(&db, buf.buf, getenv(PWM_EMAIL_ENV_VAR))) < 0) {
    goto cleanup;
  }

  if (!pwm_db_has(db, key)) {
    fprintf(stderr, "couldn't find your %s password\n", key);
    err = -1;
    goto cleanup;
  }

  if ((err = pwm_db_get(db, key, &buf)) < 0) {
    goto cleanup;
  }
  fprintf(stdout, "%s\n", buf.buf);

cleanup:
  pwm_db_free(db);
  pwm_str_free(&buf);
  return err;
}
int
main(int argc, char **argv) {
  char c;
  int err;
  opts_t opts = OPTS_DEFAULT;

  while ((c = getopt(argc, argv, "ch")) > -1) {
    switch (c) {
    case 'c':
      opts.clip = 1;
      break;
    default:
      usage();
    }
  }

  if (optind >= argc) {
    usage();
  }
  pwm_init();
  err = run(&opts, argv[optind]);
  pwm_shutdown();
  exit(err < 0);
}
