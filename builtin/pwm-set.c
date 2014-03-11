#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include "pwm.h"

const char *usage_str =
  "pwm-set [<opts>] <key>\n\n"
  "options:\n"
  "  -c              store password in the clipboard\n"
  "  -f              override existing password\n"
  "  -h              show this help\n"
  "  -p              print password";

void
usage() {
  fprintf(stderr, "usage: %s\n", usage_str);
  exit(EXIT_FAILURE);
}

typedef struct {
  int clip;
  int force;
  int print;
} opts_t;

#define OPTS_DEFAULT {0,0,0}

int
run(opts_t *opts, const char *key) {
  int err;
  pwm_db_t *db = NULL;
  pwm_str_t buf = PWM_STR_INIT;

  if ((err = pwm_find_home(&buf)) < 0) {
    fprintf(stderr, "couldn't find the pwm buf dir\n");
    goto cleanup;
  }

  if ((err = pwm_db_new(&db, buf.buf, getenv(PWM_EMAIL_ENV_VAR))) < 0) {
    goto cleanup;
  }

  if (!opts->force && pwm_db_has(db, key)) {
    fprintf(stderr, "password for %s already exists\n", key);
    err = -1;
    goto cleanup;
  }

  if (!isatty(STDIN_FILENO)) {
    err = pwm_str_read_line(&buf, stdin);
  } else if (opts->print) {
    err = pwm_read_line(&buf, "password: ");
  } else {
    err = pwm_read_line_hidden(&buf, "password: ");
  }

  if (err < 0) {
    goto cleanup;
  }

  if ((err = pwm_db_set(db, key, &buf)) >= 0) {
    fprintf(stderr, "\rsaved your %s password\n", key);
  }
  pwm_db_clean(db);

  if ((err = pwm_clip_set(&buf)) >= 0) {
    fprintf(stderr, "stored your %s password in the clipboard\n", key);
  }

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

  while ((c = getopt(argc, argv, "cfhp")) > -1) {
    switch (c) {
    case 'c':
      opts.clip = 1;
      break;
    case 'f':
      opts.force = 1;
      break;
    case 'p':
      opts.print = 1;
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
